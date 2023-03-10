#include "messagedispatcher_opalkelly.h"

using namespace std;

MessageDispatcher_OpalKelly::MessageDispatcher_OpalKelly(string deviceId) :
    MessageDispatcher(deviceId) {

    rxRawBufferMask = OKY_RX_BUFFER_MASK;
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

    MessageDispatcher::disconnect();

    this->deinitializeBuffers();

    if (dev != nullptr) {
        dev->Close();
        delete dev;
        dev = nullptr;
    }
    return Success;
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

    unique_lock <mutex> deviceMutexLock (deviceMutex);
    deviceMutexLock.unlock();

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
        regs.resize(txMsgLength[txMsgBufferReadOffset]);
        for (txDataBufferReadIdx = 0; txDataBufferReadIdx < txMsgLength[txMsgBufferReadOffset]; txDataBufferReadIdx += 2) {
            regs[txDataBufferReadIdx/2].address = (txMsgOffsetWord[txMsgBufferReadOffset]+txDataBufferReadIdx)/2;
            regs[txDataBufferReadIdx/2].data =
                    (((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx]) << 16) +
                    (uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx+1]; /*! Little endian */
        }

        txMsgBufferReadOffset = (txMsgBufferReadOffset+1) & TX_MSG_BUFFER_MASK;

        /******************\
         *  Sending part  *
        \******************/

        notSentTxData = true;
        while (notSentTxData && (writeTries++ < TX_MAX_WRITE_TRIES)) { /*! \todo FCON prevedere un modo per notificare ad alto livello e all'utente */
            deviceMutexLock.lock();
            if (dev->WriteRegisters(regs) != okCFrontPanel::NoError) {
                dev->ActivateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
                continue;
            }
            deviceMutexLock.unlock();

#ifdef DEBUG_PRINT
            /*! Aggiungere printata di debug se serve */
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
    uint32_t rxFrameOffset; /*!< Offset of the current frame */
    uint16_t rxWordOffset; /*!< Offset of the first word in the received frame */
    uint16_t rxWordsLength; /*!< Number of words in the received frame */
    uint32_t rxDataBytes; /*!< Number of bytes in the received frame */
    uint16_t rxCandidateHeader;

    bool notEnoughRxData;

    parsingFlag = true;

    unique_lock <mutex> deviceMutexLock(deviceMutex);
    deviceMutexLock.unlock();

    /******************\
     *  Reading part  *
    \******************/

    while (!stopConnectionFlag) {
        /*! Read the data */
        deviceMutexLock.lock();
        bytesRead = dev->ReadFromBlockPipeOut(OKY_RX_PIPE_ADDR, OKY_RX_BLOCK_SIZE, OKY_RX_TRANSFER_SIZE, rxTransferBuffer);
        deviceMutexLock.unlock();

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
                    rxFrameOffset = rxRawBufferReadOffset;
                    /*! Check byte by byte if the buffer contains a sync word (frame header) */
                    if (popUint16FromRxRawBuffer() == rxSyncWord) {
                        /*! If all the bytes match the sync word move rxSyncWordSize bytes ahead and look for the message length */
                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! If not all the bytes match the sync word restore one of the removed bytes and recheck */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset-1) & OKY_RX_BUFFER_MASK;
                        rxRawBufferReadLength++;
                    }
                }
                break;

            case RxParseLookForLength:
                /*! Look for length */
                if (rxRawBufferReadLength < rxOffsetLengthSize) {
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
                        rxRawBufferReadLength += rxOffsetLengthSize;
#ifdef DEBUG_PRINT
                        /*! aggiungere printata di debug se serve */
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
                    rxCandidateHeader = readUint16FromRxRawBuffer(rxDataBytes);

                    if (rxCandidateHeader == rxSyncWord) {
                        if (rxWordOffset == rxWordOffsets[RxMessageDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageDataHeader]) {
                            this->storeDataHeaderFrame();

                        } else if (rxWordOffset == rxWordOffsets[RxMessageDataTail]) {
                            this->storeDataTailFrame();

                        } else if (rxWordOffset == rxWordOffsets[RxMessageStatus]) {
                            this->storeStatusFrame();

                        } else if (rxWordOffset == rxWordOffsets[RxMessageVoltageOffset]) {
                            this->storeVoltageOffsetFrame();
                        }

                        rxFrameOffset = rxRawBufferReadOffset;
                        /*! remove the bytes that were not popped to read the next header */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        rxRawBufferReadLength -= rxSyncWordSize;

                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! Sync word not found, restart looking from the previous sync word */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBufferReadLength += rxOffsetLengthSize;
                        rxParsePhase = RxParseLookForHeader;
                    }
                }
                break;
            }
        }
    }

    if (rxMsgBufferReadLength <= 0) {
        unique_lock <mutex> rxMutexLock(rxMutex);
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
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
