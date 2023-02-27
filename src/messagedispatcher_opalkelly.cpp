#include "messagedispatcher_opalkelly.h"

using namespace std;

MessageDispatcher_OpalKelly::MessageDispatcher_OpalKelly(string deviceId) :
    MessageDispatcher(deviceId) {

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

void MessageDispatcher_OpalKelly::storeDataLoadFrame() {
    uint16_t value;

    for (int packetIdx = 0; packetIdx < packetsPerFrame; packetIdx++) {
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            value = 0;

            for (unsigned int byteIdx = 0; byteIdx < RX_WORD_SIZE; byteIdx++) {
                value <<= 8;
                value += *(rxRawBuffer+rxRawBufferReadOffset);
                rxRawBufferReadOffset = (rxRawBufferReadOffset+1)&OKY_RX_BUFFER_MASK;
            }

            outputDataBuffer[outputBufferWriteOffset][idx] = value;
        }

        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            value = 0;

            for (unsigned int byteIdx = 0; byteIdx < RX_WORD_SIZE; byteIdx++) {
                value <<= 8;
                value += *(rxRawBuffer+rxRawBufferReadOffset);
                rxRawBufferReadOffset = (rxRawBufferReadOffset+1)&OKY_RX_BUFFER_MASK;
            }

            outputDataBuffer[outputBufferWriteOffset][voltageChannelsNum+idx] = value;
        }
#ifdef DEBUG_PRINT
//        fwrite((uint8_t*)outputDataBuffer[outputBufferWriteOffset], 2, totalChannelsNum, rxFid1);
#endif
        outputBufferWriteOffset = (outputBufferWriteOffset+1)&E4RCL_OUTPUT_BUFFER_MASK;
    }
    outputBufferAvailablePackets += packetsPerFrame;

    /*! If too many packets are written but not read from the user the buffer saturates */
//    if (outputBufferAvailablePackets > E4RCL_OUTPUT_BUFFER_SIZE/totalChannelsNum) {
//        outputBufferAvailablePackets = E4RCL_OUTPUT_BUFFER_SIZE/totalChannelsNum; /*!< Saturates available packets */
//        outputBufferReadOffset = outputBufferWriteOffset; /*! Move read offset just on top of the write offset so that it can read up to 1 position before after a full buffer read */
//        outputBufferOverflowFlag = true;
//    }
}

void MessageDispatcher_OpalKelly::storeDataHeaderFrame() {

}

void MessageDispatcher_OpalKelly::storeDataTailFrame() {

}

void MessageDispatcher_OpalKelly::storeStatusFrame() {

}

void MessageDispatcher_OpalKelly::storeVoltageOffsetFrame() {

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
