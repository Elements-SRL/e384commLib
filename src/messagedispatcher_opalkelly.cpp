#include "messagedispatcher_opalkelly.h"

using namespace std;

MessageDispatcher_OpalKelly::MessageDispatcher_OpalKelly(string deviceId) :
    MessageDispatcher(deviceId) {

    rxRawBufferMask = OKY_RX_BUFFER_MASK;

    /*! \todo FCON ricordarsi di mettere il mutex per evitare che la librerira dell'opal kelly crashi */
}

MessageDispatcher_OpalKelly::~MessageDispatcher_OpalKelly() {

}

ErrorCodes_t MessageDispatcher_OpalKelly::connect() {
    if (connected) {
        return ErrorDeviceAlreadyConnected;
    }

    dev = new okCFrontPanel;

    okCFrontPanel::ErrorCode error = dev->OpenBySerial(deviceId);
    if (error != okCFrontPanel::NoError) {
        return ErrorDeviceConnectionFailed;
    }

    error = dev->ConfigureFPGA(fwName);

    if (error != okCFrontPanel::NoError) {
        return ErrorDeviceFwLoadingFailed;
    }

    ErrorCodes_t err = this->initializeBuffers();
    if (err != Success) {
        return err;

    } else {
        return MessageDispatcher::connect();
    }
}

ErrorCodes_t MessageDispatcher_OpalKelly::disconnect() {
    if (!connected) {
        return ErrorDeviceNotConnected;
    }

    this->deinitializeBuffers();

    if (dev != nullptr) {
        dev->Close();
        delete dev;
        dev = nullptr;
    }

    return MessageDispatcher::disconnect();
}

void MessageDispatcher_OpalKelly::sendCommandsToDevice() {
    int writeTries = 0;
    okTRegisterEntries regs;
    regs.reserve(txMaxRegs);

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */

    /*! Variables used to access the tx data buffer */
    uint32_t txDataBufferReadIdx;

    bool notSentTxData;

    unique_lock <mutex> txMutexLock (txMutex);
    txMutexLock.unlock();

    while (!stopConnectionFlag) {

        /***********************\
         *  Data copying part  *
        \***********************/

        txMutexLock.lock();
        while (txMsgBufferReadLength <= 0) {
            txMsgBufferNotEmpty.wait_for(txMutexLock, chrono::milliseconds(100));
            if (stopConnectionFlag) {
                break;
            }
        }
        txMutexLock.unlock();
        if (stopConnectionFlag) {
            continue;
        }

        /*! Moving from 16 bits words to 32 bits registers (+= 2, /2, etc, are due to this conversion) */
        for (txDataBufferReadIdx = 0; txDataBufferReadIdx < txMsgLength[txMsgBufferReadOffset]; txDataBufferReadIdx += 2) {
            regs[txDataBufferReadIdx/2].address = (txMsgOffsetWord[txMsgBufferReadOffset]+txDataBufferReadIdx)/2;
            regs[txDataBufferReadIdx/2].data = (((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx]) << 16) + (uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx+1];
        }

        txMsgBufferReadOffset = (txMsgBufferReadOffset+1)&TX_MSG_BUFFER_MASK;

        /******************\
         *  Sending part  *
        \******************/

        notSentTxData = true;
        while (notSentTxData && (writeTries++ < TX_MAX_WRITE_TRIES)) { /*! \todo FCON prevedere un modo per notificare ad alto livello e all'utente */
            if (dev->WriteRegisters(regs) != okCFrontPanel::NoError) {
                continue;
            }

#ifdef DEBUG_PRINT
            fprintf(txFid, "\n%d <= %d == %d\n", txMaxWords*FTD_TX_WORD_SIZE, bytesToWrite, ftdiWrittenBytes);
            fflush(txFid);

            int o = txRawBuffer[2]*256+txRawBuffer[3];
            int i = 0;
            fprintf(txFid, "HDR:%02x%02x ", txRawBuffer[i], txRawBuffer[i+1]);
            i += FTD_TX_WORD_SIZE;
            fprintf(txFid, "OFF:%02x%02x ", txRawBuffer[i], txRawBuffer[i+1]);
            i += FTD_TX_WORD_SIZE;
            fprintf(txFid, "LEN:%02x%02x\n", txRawBuffer[i], txRawBuffer[i+1]);
            i += FTD_TX_WORD_SIZE;

            for (; i < ftdiWrittenBytes-FTD_TX_WORD_SIZE; i += FTD_TX_WORD_SIZE) {
                fprintf(txFid, "%03d:%02x%02x ", (i-FTD_TX_SYNC_WORD_SIZE-FTD_TX_OF_LN_SIZE)/2+o, txRawBuffer[i], txRawBuffer[i+1]);
                if ((i-FTD_TX_SYNC_WORD_SIZE-FTD_TX_OF_LN_SIZE) % 32 == 32-FTD_TX_WORD_SIZE) {
                    fprintf(txFid, "\n");
                }
            }
            fprintf(txFid, "\nCRC:%02x%02x\n", txRawBuffer[i], txRawBuffer[i+1]);
            i += FTD_TX_WORD_SIZE;
            fflush(txFid);
#endif

            notSentTxData = false;
            writeTries = 0;
        }

        txMutexLock.lock();
        txMsgBufferReadLength--;
        txMsgBufferNotFull.notify_all();
        txMutexLock.unlock();
    }
}

void MessageDispatcher_OpalKelly::readDataFromDevice() {
    stopConnectionFlag = false;

    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead; /*!< Bytes read during last transfer from Opal Kelly */
    uint32_t bytesToEnd; /*!< Bytes to the end of the buffer */
    uint32_t availablePackets; /*!< Approximate number of packets available in RX queue */

    RxParsePhase_t rxParsePhase = RxParseLookForHeader;

    rxRawBufferReadOffset = 0;
    uint32_t rxSyncWordSize = sizeof(rxSyncWord);
    uint32_t rxOffsetLengthSize = 2*RX_WORD_SIZE;
    uint32_t rxRawBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint32_t rxRawBufferReadIdx = 0; /*!< Index being processed wrt rxRawBufferReadOffset */
    uint32_t rxFrameOffset; /*!< Offset of the current frame */
    uint16_t rxWordOffset; /*!< Offset of the first word in the received frame */
    uint16_t rxWordsLength; /*!< Number of words in the received frame */
    uint32_t rxDataBytes; /*!< Number of bytes in the received frame */
    uint16_t rxCandidateHeader;

    bool notEnoughRxData;

    /*! Rx sync word variables */
    bool rxSyncOk;

    unique_lock <mutex> rxMutexLock(rxMutex);
    rxMutexLock.unlock();

    unique_lock <mutex> deviceMutexLock(deviceMutex);
    rxMutexLock.unlock();

#ifdef DEBUG_RAW_BIT_RATE_PRINT
    std::chrono::steady_clock::time_point startPrintfTime;
    std::chrono::steady_clock::time_point currentPrintfTime;
    startPrintfTime = std::chrono::steady_clock::now();
    long long int acc = 0;
#endif

    /******************\
     *  Reading part  *
    \******************/

    while (!stopConnectionFlag) {
        /*! Read the data */
        rxMutexLock.lock();
        bytesRead = dev->ReadFromBlockPipeOut(OKY_RX_PIPE_ADDR, OKY_RX_BLOCK_SIZE, OKY_RX_TRANSFER_SIZE, rxTransferBuffer);
        rxMutexLock.unlock();

        /*! Copy the data from the transfer buffer to the circular buffer */
        bytesToEnd = OKY_RX_BUFFER_SIZE-rxRawBufferWriteOffset;
        if (bytesRead > bytesToEnd) {
            memcpy(rxRawBuffer+rxRawBufferWriteOffset, rxTransferBuffer, bytesToEnd);
            memcpy(rxRawBuffer, rxTransferBuffer+bytesToEnd, bytesRead-bytesToEnd);

        } else {
            memcpy(rxRawBuffer+rxRawBufferWriteOffset, rxTransferBuffer, bytesRead);
        }

        /******************\
         *  Parsing part  *
        \******************/

#ifdef DEBUG_RAW_BIT_RATE_PRINT
        currentPrintfTime = std::chrono::steady_clock::now();
        acc += ftdiQueuedBytes;
        if ((double)(std::chrono::duration_cast <std::chrono::microseconds> (currentPrintfTime-startPrintfTime).count()) > 1.0e6) {
            printf("%f byte/s\n", 1.0e6*((double)acc)/(double)(std::chrono::duration_cast <std::chrono::microseconds> (currentPrintfTime-startPrintfTime).count()));
            fflush(stdout);
            startPrintfTime = currentPrintfTime;
            acc = 0;
        }
#endif

        /*! Update buffer writing point */
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+bytesRead)&OKY_RX_BUFFER_MASK;

        /*! Compute the approximate number of available packets */
        rxRawBufferReadLength += bytesRead;
        availablePackets = rxRawBufferReadLength/(uint32_t)(totalChannelsNum*RX_WORD_SIZE);

        /*! If there are not enough packets wait for a minimum packets number to decrease overhead */
        if (availablePackets < minPacketsNumber) {
            this_thread::sleep_for(chrono::microseconds(fewPacketsSleepUs));
            continue;
        }

        notEnoughRxData = false;

        while (!notEnoughRxData) {
            switch (rxParsePhase) {
            case RxParseLookForHeader:
                /*! Look for header */
                if (rxRawBufferReadLength < rxSyncWordSize) {
                    notEnoughRxData = true;

                } else {
                    rxSyncOk = true;
                    /*! Check byte by byte if the buffer contains a sync word (frame header) */
                    for (rxRawBufferReadIdx = 0; rxRawBufferReadIdx < rxSyncWordSize; rxRawBufferReadIdx++) {
                        if (rxRawBuffer[(rxRawBufferReadOffset+rxRawBufferReadIdx)&OKY_RX_BUFFER_MASK] != ((uint8_t *)&rxSyncWord)[rxSyncWordSize-rxRawBufferReadIdx-1]) {
                            rxSyncOk = false;
                            break;
                        }
                    }

                    if (rxSyncOk) {
                        /*! If all the bytes match the sync word move rxSyncWordSize bytes ahead and look for the message length */
                        rxFrameOffset = rxRawBufferReadOffset;
                        rxParsePhase = RxParseLookForLength;
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxSyncWordSize)&OKY_RX_BUFFER_MASK;
                        rxRawBufferReadLength -= rxSyncWordSize;

                    } else {
                        /*! If not all the bytes match the sync word move one byte ahead and recheck */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+1)&OKY_RX_BUFFER_MASK;
                        rxRawBufferReadLength--;
                    }
                }
                break;

            case RxParseLookForLength:
                /*! Look for length */
                if (rxRawBufferReadLength < rxOffsetLengthSize) {
                    notEnoughRxData = true;

                } else {
                    /*! Offset of the words received */
                    rxWordOffset = (rxRawBuffer[(rxRawBufferReadOffset+rxRawBufferReadIdx)&OKY_RX_BUFFER_MASK] << 8) & 0xFF00;
                    rxWordOffset += (rxRawBuffer[(rxRawBufferReadOffset+rxRawBufferReadIdx+1)&OKY_RX_BUFFER_MASK]) & 0x00FF;
                    rxRawBufferReadIdx = RX_WORD_SIZE;

                    /*! Number of the words received */
                    rxWordsLength = (rxRawBuffer[(rxRawBufferReadOffset+rxRawBufferReadIdx)&OKY_RX_BUFFER_MASK] << 8) & 0xFF00;
                    rxWordsLength += (rxRawBuffer[(rxRawBufferReadOffset+rxRawBufferReadIdx+1)&OKY_RX_BUFFER_MASK]) & 0x00FF;
                    rxRawBufferReadIdx += RX_WORD_SIZE;

                    rxRawBufferReadOffset = (rxRawBufferReadOffset+rxRawBufferReadIdx)&OKY_RX_BUFFER_MASK;
                    rxRawBufferReadLength -= rxRawBufferReadIdx;

                    rxDataBytes = rxWordsLength*RX_WORD_SIZE;

                    if (rxDataBytes > maxInputFrameSize) {
                        /*! To many bytes to be read, restarting looking for a sync word from the previous one */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize)&OKY_RX_BUFFER_MASK;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBufferReadLength += rxOffsetLengthSize;
#ifdef DEBUGPRINT
                        fprintf(rxFid,
                                "crc1 wrong\n"
                                "hb: \t0x%04x\n\n",
                                rxHeartbeat);
                        fflush(rxFid);
#endif
                        rxParsePhase = RxParseLookForHeader;

                    } else {
                        rxParsePhase = RxParseCheckNextHeader;
                    }
                }
                break;

            case RxParseCheckNextHeader:
                /*! Check that after the frame end there is a valid header */
                if (rxRawBufferReadLength < rxDataBytes+rxSyncWordSize) {
                    notEnoughRxData = true;

                } else {
                    rxCandidateHeader = (rxRawBuffer[(rxRawBufferReadOffset+rxDataBytes)&OKY_RX_BUFFER_MASK] << 8) & 0xFF00;
                    rxCandidateHeader += (rxRawBuffer[(rxRawBufferReadOffset+rxDataBytes+1)&OKY_RX_BUFFER_MASK]) & 0x00FF;

                    rxSyncOk = (rxCandidateHeader == rxSyncWord);

                    if (rxSyncOk) {
#ifdef DEBUGPRINT
                        currentPrintfTime = std::chrono::steady_clock::now();
                        fprintf(rxFid,
                                "%d us\n"
                                "recd message\n"
                                "offset:\t0x%04x\n"
                                "length:\t0x%04x\n",
                                (int)(std::chrono::duration_cast <std::chrono::microseconds> (currentPrintfTime-startPrintfTime).count()),
                                rxWordOffset,
                                rxWordsLength);
#endif

                        rxMutexLock.lock(); /*!< Protects data modified in storeXxxFrame */
                        if (rxWordOffset == rxWordOffsets[rxMessageDataLoad]) {
                            this->storeDataLoadFrame();

                        } else if (rxWordOffset == rxWordOffsets[rxMessageDataHeader]) {
                            this->storeDataHeaderFrame();

                        } else if (rxWordOffset == rxWordOffsets[rxMessageDataTail]) {
                            this->storeDataTailFrame();

                        } else if (rxWordOffset == rxWordOffsets[rxMessageStatus]) {
                            this->storeStatusFrame();

                        } else if (rxWordOffset == rxWordOffsets[rxMessageVoltageOffset]) {
                            this->storeVoltageOffsetFrame();
                        }
                        rxMutexLock.unlock();

#ifdef DEBUGPRINT
                        fprintf(rxFid, "crc1:\t0x%04x\n\n",
                                rxReadCrc1);
                        fflush(rxFid);
#endif

                        rxFrameOffset = rxRawBufferReadOffset;
                        /*!< rxRawBufferReadOffset is decreased by rxDataBytes in storeXxxFrame */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxSyncWordSize)&OKY_RX_BUFFER_MASK;
                        rxRawBufferReadLength -= rxDataBytes+rxSyncWordSize;

                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! Sync word not found, restarting looking from the previous sync word */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize)&OKY_RX_BUFFER_MASK;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBufferReadLength += rxOffsetLengthSize;
#ifdef DEBUGPRINT
                        fprintf(rxFid,
                                "crc1 wrong\n"
                                "hb: \t0x%04x\n\n",
                                rxHeartbeat);
                        fflush(rxFid);
#endif
                        rxParsePhase = RxParseLookForHeader;
                    }
                }
                break;
            }
        }
    }
}

ErrorCodes_t MessageDispatcher_OpalKelly::initializeBuffers() {
    unique_lock <mutex> deviceMutexLock(deviceMutex);


    rxTransferBuffer = new (std::nothrow) uint8_t[OKY_RX_TRANSFER_SIZE];
    rxRawBuffer = new (std::nothrow) uint8_t[OKY_RX_BUFFER_SIZE];
    if (rxRawBuffer != nullptr) {
        return Success;

    } else {
        return ErrorMemoryInitialization;
    }
}

ErrorCodes_t MessageDispatcher_OpalKelly::deinitializeBuffers() {
    unique_lock <mutex> deviceMutexLock(deviceMutex);

    if (rxTransferBuffer != nullptr) {
        delete [] rxTransferBuffer;
        rxTransferBuffer = nullptr;
    }

    if (rxRawBuffer != nullptr) {
        delete [] rxRawBuffer;
        rxRawBuffer = nullptr;
    }

    return Success;
}
