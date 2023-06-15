#include "messagedispatcher_opalkelly.h"
#include "utils.h"

MessageDispatcher_OpalKelly::MessageDispatcher_OpalKelly(std::string deviceId) :
    MessageDispatcher(deviceId) {

    rxRawBufferMask = OKY_RX_BUFFER_MASK;

}

MessageDispatcher_OpalKelly::~MessageDispatcher_OpalKelly() {

}

ErrorCodes_t MessageDispatcher_OpalKelly::connect() {
    if (connected) {
        return ErrorDeviceAlreadyConnected;
    }

#ifdef DEBUG_TX_DATA_PRINT
    createDebugFile(txFid, "e384CommLib_tx");
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
    createDebugFile(rxRawFid, "e384CommLib_rxRaw");
#endif

#ifdef DEBUG_RX_PROCESSING_PRINT
    createDebugFile(rxProcFid, "e384CommLib_rxProcessing");
#endif

#ifdef DEBUG_RX_DATA_PRINT
    createDebugFile(rxFid, "e384CommLib_rx");
#endif

    dev = new okCFrontPanel;

    okCFrontPanel::ErrorCode error = dev->OpenBySerial(deviceId);

    if (error != okCFrontPanel::NoError) {
        return ErrorDeviceConnectionFailed;
    }

    if(!dev->IsFrontPanelEnabled()){ // doesn't load FW is one is already loaded
        error = dev->ConfigureFPGA(fwName);

        if (error != okCFrontPanel::NoError) {
            return ErrorDeviceFwLoadingFailed;
        }

    } else {

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

    MessageDispatcher::disconnect();

    this->deinitializeBuffers();

    if (dev != nullptr) {
        dev->Close();
        delete dev;
        dev = nullptr;
    }
    return Success;
}

void MessageDispatcher_OpalKelly::handleCommunicationWithDevice() {
    regs.reserve(txMaxRegs);

    std::unique_lock <std::mutex> txMutexLock (txMutex);
    txMutexLock.unlock();

    std::unique_lock <std::mutex> rxRawMutexLock (rxRawMutex);
    rxRawMutexLock.unlock();

    std::chrono::steady_clock::time_point startWhileTime = std::chrono::steady_clock::now();

    bool waitingTimeForReadingPassed = false;

//    int okWrites = 0;
//    std::chrono::steady_clock::time_point startPrintfTime;
//    std::chrono::steady_clock::time_point currentPrintfTime;
//    startPrintfTime = std::chrono::steady_clock::now();

    while (!stopConnectionFlag) {

        /***********************\
         *  Data copying part  *
        \***********************/

        txMutexLock.lock();
        if (txMsgBufferReadLength > 0) {
            txMutexLock.unlock();

            this->sendCommandsToDevice();

            txMutexLock.lock();
            txMsgBufferReadLength--;
            txMsgBufferNotFull.notify_all();
            txMutexLock.unlock();

        } else {
            txMutexLock.unlock();
        }

        /*! Avoid performing reads too early, might trigger Opal Kelly's API timeout, which appears to be a non escapable condition */
        if (waitingTimeForReadingPassed) {
            rxRawMutexLock.lock();
            if (rxRawBufferReadLength+OKY_RX_TRANSFER_SIZE <= OKY_RX_BUFFER_SIZE && !stopConnectionFlag) {
                rxRawMutexLock.unlock();
                if (stopConnectionFlag) {
                    break;
                }

                uint32_t bytesRead = this->readDataFromDevice();

                if (bytesRead <= INT32_MAX) {
                    rxRawMutexLock.lock();
                    rxRawBufferReadLength += bytesRead;
                    rxRawBufferNotEmpty.notify_all();
                    rxRawMutexLock.unlock();
//                    okWrites++;
//                    if (okWrites > 100) {
//                        okWrites = 0;
//                        currentPrintfTime = steady_clock::now();
//                        printf("%lld\n", (duration_cast <milliseconds> (currentPrintfTime-startPrintfTime).count()));
//                        fflush(stdout);
//                        startPrintfTime = currentPrintfTime;
//                    }
                }

            } else {
                rxRawMutexLock.unlock();
            }

        } else {
            long long t = std::chrono::duration_cast <std::chrono::microseconds> (std::chrono::steady_clock::now()-startWhileTime).count();
            if (t > waitingTimeBeforeReadingData*1e6) {
                waitingTimeForReadingPassed = true;
                parsingFlag = true;
            }
        }
    }
}

void MessageDispatcher_OpalKelly::sendCommandsToDevice() {
    int writeTries = 0;

    bool notSentTxData;

    /*! Moving from 16 bits words to 32 bits registers (+= 2, /2, etc, are due to this conversion) */
    regs.resize(txMsgLength[txMsgBufferReadOffset]/2);
    for (uint32_t txDataBufferReadIdx = 0; txDataBufferReadIdx < txMsgLength[txMsgBufferReadOffset]; txDataBufferReadIdx += 2) {
        regs[txDataBufferReadIdx/2].address = (txMsgOffsetWord[txMsgBufferReadOffset]+txDataBufferReadIdx)/2;
        regs[txDataBufferReadIdx/2].data =
                ((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx] +
                 ((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx+1] << 16)); /*! Little endian */
    }
    TxTriggerType_t type = txMsgTrigger[txMsgBufferReadOffset];

    txMsgBufferReadOffset = (txMsgBufferReadOffset+1) & TX_MSG_BUFFER_MASK;

    /******************\
     *  Sending part  *
    \******************/

    notSentTxData = true;
    writeTries = 0;
    while (notSentTxData && (writeTries++ < TX_MAX_WRITE_TRIES)) { /*! \todo FCON prevedere un modo per notificare ad alto livello e all'utente */
        if (!writeRegistersAndActivateTriggers(type)) {
            continue;
        }

#ifdef DEBUG_TX_DATA_PRINT
        for (uint16_t regIdx = 0; regIdx < regs.size(); regIdx++) {
            fprintf(txFid, "%04d:0x%08X ", regs[regIdx].address, regs[regIdx].data);
            if (regIdx % 16 == 15) {
                fprintf(txFid, "\n");
            }
        }
        fprintf(txFid, "\n");
        fflush(txFid);
#endif

        notSentTxData = false;
    }
}

bool MessageDispatcher_OpalKelly::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    if (dev->WriteRegisters(regs) == okCFrontPanel::NoError) {
        switch (type) {
        case TxTriggerParameteresUpdated:
            dev->ActivateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
            break;

        case TxTriggerStartProtocol:
            dev->ActivateTriggerIn(OKY_START_PROTOCOL_TRIGGER_IN_ADDR, OKY_START_PROTOCOL_TRIGGER_IN_BIT);
            break;

        case TxTriggerStartStateArray:
            dev->ActivateTriggerIn(OKY_START_STATE_ARRAY_TRIGGER_IN_ADDR, OKY_START_STATE_ARRAY_TRIGGER_IN_BIT);
            break;
        }
        return true;

    } else {
        return false;
    }
}

uint32_t MessageDispatcher_OpalKelly::readDataFromDevice() {
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead; /*!< Bytes read during last transfer from Opal Kelly */

    /******************\
     *  Reading part  *
    \******************/

#ifdef DEBUG_RX_PROCESSING_PRINT
    fprintf(rxProcFid, "Entering while loop\n");
    fflush(rxProcFid);
#endif

//    int okWrites = 0;
//    std::chrono::steady_clock::time_point startPrintfTime;
//    std::chrono::steady_clock::time_point currentPrintfTime;
//    startPrintfTime = std::chrono::steady_clock::now();
    /*! Read the data */
    bytesRead = dev->ReadFromBlockPipeOut(OKY_RX_PIPE_ADDR, OKY_RX_BLOCK_SIZE, OKY_RX_TRANSFER_SIZE, rxRawBuffer+rxRawBufferWriteOffset);

    if (bytesRead > INT32_MAX) {
        if (bytesRead == ok_Timeout) {
            /*! The device cannot recover from timeout condition */
            dev->Close();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            txModifiedStartingWord = 0;
            txModifiedEndingWord = txMaxWords;
            this->sendCommands();
            dev->OpenBySerial(deviceId);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
#ifdef DEBUG_RX_PROCESSING_PRINT
        fprintf(rxProcFid, "Error %x\n", bytesRead);
        fflush(rxProcFid);
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
        fprintf(rxRawFid, "Error %x\n", bytesRead);
        fflush(rxRawFid);
#endif

    } else {

#ifdef DEBUG_RX_PROCESSING_PRINT
        fprintf(rxProcFid, "Bytes read %d\n", bytesRead);
        fflush(rxProcFid);
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
        fprintf(rxRawFid, "Bytes read %d\n", bytesRead);
        fflush(rxRawFid);
#endif
    }

    /*! Update buffer writing point */
    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+bytesRead) & OKY_RX_BUFFER_MASK;
    return bytesRead;
}

void MessageDispatcher_OpalKelly::parseDataFromDevice() {
    RxParsePhase_t rxParsePhase = RxParseLookForHeader;

    rxRawBufferReadOffset = 0;
    uint32_t rxSyncWordSize = sizeof(rxSyncWord);
    uint32_t rxOffsetLengthSize = 2*RX_WORD_SIZE;
    uint32_t rxFrameOffset; /*!< Offset of the current frame */
    uint16_t rxWordOffset; /*!< Offset of the first word in the received frame */
    uint16_t rxWordsLength; /*!< Number of words in the received frame */
    uint32_t rxDataBytes; /*!< Number of bytes in the received frame */
    uint16_t rxCandidateHeader;

    bool notEnoughRxData;

    /******************\
     *  Parsing part  *
    \******************/

    std::unique_lock <std::mutex> rxRawMutexLock (rxRawMutex);
    rxRawMutexLock.unlock();

    while (!stopConnectionFlag) {
        rxRawMutexLock.lock();
        while (rxRawBufferReadLength <= OKY_RX_TRANSFER_SIZE && !stopConnectionFlag) {
            rxRawBufferNotEmpty.wait_for(rxRawMutexLock, std::chrono::milliseconds(10));
        }
        maxRxRawBytesRead = rxRawBufferReadLength;
        rxRawBytesAvailable = maxRxRawBytesRead;
        rxRawMutexLock.unlock();
        if (stopConnectionFlag) {
            break;
        }

        /*! Compute the approximate number of available packets */
        notEnoughRxData = false;

        while (!notEnoughRxData) {
            switch (rxParsePhase) {
            case RxParseLookForHeader:

#ifdef DEBUG_RX_PROCESSING_PRINT
                fprintf(rxProcFid, "Look for header: %x\n", rxRawBufferReadOffset);
                fflush(rxProcFid);
#endif

                /*! Look for header */
                if (rxRawBytesAvailable < rxSyncWordSize) {
                    notEnoughRxData = true;

                } else {
                    rxFrameOffset = rxRawBufferReadOffset;
                    /*! Check byte by byte if the buffer contains a sync word (frame header) */
                    if (popUint16FromRxRawBuffer() == rxSyncWord) {
                        /*! If all the bytes match the sync word move rxSyncWordSize bytes ahead and look for the message length */
                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! If not all the bytes match the sync word restore one of the removed bytes and recheck */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset-1) & OKY_RX_BUFFER_MASK;
                        rxRawBytesAvailable++;
                    }
                }
                break;

            case RxParseLookForLength:

#ifdef DEBUG_RX_PROCESSING_PRINT
                fprintf(rxProcFid, "Look for length: %x\n", rxRawBufferReadOffset);
                fflush(rxProcFid);
#endif

                /*! Look for length */
                if (rxRawBytesAvailable < rxOffsetLengthSize) {
                    notEnoughRxData = true;

                } else {
                    /*! Offset of the words received */
                    rxWordOffset = popUint16FromRxRawBuffer();

                    /*! Number of the words received */
                    rxWordsLength = popUint16FromRxRawBuffer();

                    rxDataBytes = rxWordsLength*RX_WORD_SIZE;

                    if (rxDataBytes > maxInputDataLoadSize) {
                        /*! Too many bytes to be read, restarting looking for a sync word from the previous one */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBytesAvailable += rxOffsetLengthSize;
#ifdef DEBUG_RX_DATA_PRINT
                        /*! aggiungere printata di debug se serve */
#endif
                        rxParsePhase = RxParseLookForHeader;

                    } else {
                        rxParsePhase = RxParseCheckNextHeader;
                    }
                }
                break;

            case RxParseCheckNextHeader:

#ifdef DEBUG_RX_PROCESSING_PRINT
                fprintf(rxProcFid, "Check next header: %x\n", rxRawBufferReadOffset);
                fflush(rxProcFid);
#endif

                /*! Check that after the frame end there is a valid header */
                if (rxRawBytesAvailable < rxDataBytes+rxSyncWordSize) {
                    notEnoughRxData = true;

                } else {
                    rxCandidateHeader = readUint16FromRxRawBuffer(rxDataBytes);

                    if (rxCandidateHeader == rxSyncWord) {
                        if (rxWordOffset == rxWordOffsets[RxMessageDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageCurrentDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageCurrentDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageVoltageDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdInvalid, RxMessageVoltageDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageDataHeader]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader, RxMessageDataHeader);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageDataTail]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail, RxMessageDataTail);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageStatus]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus, RxMessageStatus);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageVoltageOffset]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp, RxMessageVoltageOffset);
                        }

                        rxFrameOffset = rxRawBufferReadOffset;
                        /*! remove the bytes that were not popped to read the next header */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        rxRawBytesAvailable -= rxSyncWordSize;

                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! Sync word not found, restart looking from the previous sync word */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBytesAvailable += rxOffsetLengthSize;
                        rxParsePhase = RxParseLookForHeader;
                    }
                }
                break;
            }
        }

        rxRawMutexLock.lock();
        rxRawBufferReadLength -= maxRxRawBytesRead-rxRawBytesAvailable;
        rxRawBufferNotFull.notify_all();
        rxRawMutexLock.unlock();
    }

    if (rxMsgBufferReadLength <= 0) {
        std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
    }
}

ErrorCodes_t MessageDispatcher_OpalKelly::initializeBuffers() {
    rxRawBuffer = new (std::nothrow) uint8_t[OKY_RX_BUFFER_SIZE];
    if (rxRawBuffer != nullptr) {
        return Success;

    } else {
        return ErrorMemoryInitialization;
    }
}

ErrorCodes_t MessageDispatcher_OpalKelly::deinitializeBuffers() {
    if (rxRawBuffer != nullptr) {
        delete [] rxRawBuffer;
        rxRawBuffer = nullptr;
    }

    return Success;
}
