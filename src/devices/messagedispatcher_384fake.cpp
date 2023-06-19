#include "messagedispatcher_384fake.h"

MessageDispatcher_384Fake::MessageDispatcher_384Fake(std::string id) :
    MessageDispatcher_384NanoPores_V01(id) {

    waitingTimeBeforeReadingData = 0;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate6kHz].value = 6.0;
    samplingRatesArray[SamplingRate6kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate6kHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate6kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate6kHz].value = 6.0;
    realSamplingRatesArray[SamplingRate6kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate6kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate6kHz].value = 1.0/6.0;
    integrationStepArray[SamplingRate6kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate6kHz].unit = "s";
}

MessageDispatcher_384Fake::~MessageDispatcher_384Fake() {

}

ErrorCodes_t MessageDispatcher_384Fake::connect() {
    this->initializeBuffers();
    this->fillBuffer();
    startTime = std::chrono::steady_clock::now();

    return MessageDispatcher::connect();
}

ErrorCodes_t MessageDispatcher_384Fake::disconnect() {
    MessageDispatcher::disconnect();
    return this->deinitializeBuffers();
}

bool MessageDispatcher_384Fake::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    return true;
}

uint32_t MessageDispatcher_384Fake::readDataFromDevice() {
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */
#ifdef DEBUG_MAX_SPEED
    currentTime = std::chrono::steady_clock::now();
    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    if (duration > (1000.0*(double)OKY_RX_TRANSFER_SIZE)/generatedSamplingRate) {
        startTime = currentTime;
        bytesRead = OKY_RX_TRANSFER_SIZE;
    }
#else
    /*! No data to receive, just sleep */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    while (bytesRead+(totalChannelsNum+3)*RX_WORD_SIZE < OKY_RX_TRANSFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X03;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        syntheticData += 1;
        bytesRead += (totalChannelsNum+3)*RX_WORD_SIZE;
    }

//    totalBytesWritten += bytesRead;
//    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::steady_clock::now()-startPrintfTime).count());
//    if (duration > 1000) {
//        printf("%lld kB/s\n", totalBytesWritten/duration);
//        fflush(stdout);
//        totalBytesWritten = 0;
//        startPrintfTime = std::chrono::steady_clock::now();
//    }
#endif

    return bytesRead;
}
#include <iostream>
void MessageDispatcher_384Fake::parseDataFromDevice() {
    uint32_t okCnt = 0;
    uint32_t nokCnt = 0;

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
        while (rxRawBufferReadLength < OKY_RX_TRANSFER_SIZE && !stopConnectionFlag) {
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
        if (rxRawBufferReadLength <= OKY_RX_TRANSFER_SIZE+(3+768)*2) {
            okCnt++;

        } else {
            nokCnt++;
        }

        if (okCnt+nokCnt > 50) {
            std::cout << "okCnt " << okCnt << " nokCnt " << nokCnt << std::endl;
            okCnt = 0;
            nokCnt = 0;
        }

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

static uint32_t totMsg = 0;
static uint32_t totCnt = 0;

ErrorCodes_t MessageDispatcher_384Fake::getNextMessage(RxOutput_t &rxOutput, int16_t * data) {
    ErrorCodes_t ret = Success;
    double xFlt;

    std::unique_lock <std::mutex> rxMutexLock (rxMsgMutex);
    if (rxMsgBufferReadLength <= 0) {
        rxMsgBufferNotEmpty.wait_for(rxMutexLock, std::chrono::milliseconds(10));
        if (rxMsgBufferReadLength <= 0) {
            return ErrorNoDataAvailable;
        }
    }
    uint32_t maxMsgRead = rxMsgBufferReadLength;
    rxMutexLock.unlock();

    if (!parsingFlag) {
        return ErrorDeviceNotConnected;
    }

    uint32_t msgReadCount = 0;

    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged */
    lastParsedMsgType = MsgTypeIdInvalid;

    uint32_t dataOffset;
    uint32_t samplesNum;
    uint32_t sampleIdx;
    uint32_t timeSamplesNum;
    int16_t rawFloat;
    uint32_t dataWritten;
    bool exitLoop = false;
    bool messageReadFlag = false;

    while (msgReadCount < maxMsgRead) {
        dataOffset = rxMsgBuffer[rxMsgBufferReadOffset].startDataPtr;
        sampleIdx = 0;
        rxOutput.msgTypeId = rxMsgBuffer[rxMsgBufferReadOffset].typeId;
        switch (rxOutput.msgTypeId) {
        case (MsgDirectionDeviceToPc+MsgTypeIdFpgaReset):
            if (lastParsedMsgType == MsgTypeIdInvalid) {
                lastParsedMsgType = MsgTypeIdFpgaReset;

                /*! This message cannot be merged, leave anyway */
                exitLoop = true;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp):
            if (lastParsedMsgType == MsgTypeIdInvalid) {
                /*! \todo FCON da implementare */
                //            rxOutput.dataLen = 1;
                //            rxOutput.channelIdx = * (rxDataBuffer+dataOffset);
                //            rawFloat = * (rxDataBuffer+((dataOffset+1) & RX_DATA_BUFFER_MASK));
                //            voltageOffsetCorrected = (((double)rawFloat)-liquidJunctionOffsetBinary)*liquidJunctionResolution;
                //            this->selectVoltageOffsetResolution();
                //            rxOutput.data[0] = (int16_t)(rawFloat-liquidJunctionOffsetBinary);
                //            lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;

                //            this->setDigitalOffsetCompensationOverrideValue(rxOutput.channelIdx, {voltageOffsetCorrected, liquidJunctionControl.prefix, nullptr});

                lastParsedMsgType = MsgTypeIdDigitalOffsetComp;

                /*! This message cannot be merged, leave anyway */
                exitLoop = true;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
            if (lastParsedMsgType == MsgTypeIdInvalid) {
                /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
                rxOutput.dataLen = 0;
                rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                rxOutput.protocolItemIdx = * (rxDataBuffer+((dataOffset+1) & RX_DATA_BUFFER_MASK));
                rxOutput.protocolRepsIdx = * (rxDataBuffer+((dataOffset+2) & RX_DATA_BUFFER_MASK));
                rxOutput.protocolSweepIdx = * (rxDataBuffer+((dataOffset+3) & RX_DATA_BUFFER_MASK));

                lastParsedMsgType = MsgTypeIdAcquisitionHeader;

                /*! This message cannot be merged, but stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else if (lastParsedMsgType == MsgTypeIdAcquisitionHeader) {
                /*! If there are more of this kind in a sequence ignore subsequent ones */
                exitLoop = false;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData):
            samplesNum = rxMsgBuffer[rxMsgBufferReadOffset].dataLength;
            dataWritten = rxOutput.dataLen;
            if (lastParsedMsgType == MsgTypeIdInvalid ||
                    (lastParsedMsgType == MsgTypeIdAcquisitionData && dataWritten+samplesNum < E384CL_OUT_STRUCT_DATA_LEN)) {
                /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid)
                    OR if we are merging successive acquisition data messages that do not overflow the available memory */
                rxOutput.dataLen += samplesNum;
                timeSamplesNum = samplesNum/totalChannelsNum;
                /*! \todo FCON è da lasciare implementato questo? */
                //            if (msgReadCount == 0) {
                //                /*! Update firstSampleOffset only if it is not merging messages */
                //                rxOutput.firstSampleOffset = * ((uint32_t *)(rxDataBuffer+dataOffset));
                //            }
                //            dataOffset = (dataOffset+2) & RX_DATA_BUFFER_MASK;

                for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
#ifdef DISABLE_IIR
                    for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                        data[dataWritten+sampleIdx++] = rxDataBuffer[dataOffset];
                        dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                    }

                    for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                        data[dataWritten+sampleIdx++] = rxDataBuffer[dataOffset];
                        dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                    }
#else
                    for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                        rawFloat = (int16_t)rxDataBuffer[dataOffset];
                        xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                        data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                        dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                    }

                    for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                        rawFloat = (int16_t)rxDataBuffer[dataOffset];
                        xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                        data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                        dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                    }

                    if (iirOff < 1) {
                        iirOff = IIR_ORD;

                    } else {
                        iirOff--;
                    }
#endif
                }

                lastParsedMsgType = MsgTypeIdAcquisitionData;

                /*! This message can be merged, stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
            if (lastParsedMsgType == MsgTypeIdInvalid) {
                /*! Evaluate only if it's the first repetition */
                rxOutput.dataLen = 0;
                rxOutput.protocolId = rxDataBuffer[dataOffset];

                lastParsedMsgType = MsgTypeIdAcquisitionTail;

                /*! This message cannot be merged, but stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else if (lastParsedMsgType == MsgTypeIdAcquisitionTail) {
                /*! If there are more of this kind in a sequence ignore subsequent ones */
                exitLoop = false;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }

            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
            if (lastParsedMsgType == MsgTypeIdInvalid) {
                //            rxOutput.dataLen = rxMsgBuffer[rxMsgBufferReadOffset].dataLength;
                //            for (uint16_t dataIdx = 0; dataIdx < rxOutput.dataLen; dataIdx++) {
                //                rxOutput.uintData[dataIdx] = * (rxDataBuffer+dataOffset);
                //                dataOffset = (dataOffset+1)&FTD_RX_DATA_BUFFER_MASK;
                //            }

                lastParsedMsgType = MsgTypeIdAcquisitionSaturation;

                /*! This message cannot be merged, leave anyway */
                exitLoop = true;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }

            break;

        default:
            lastParsedMsgType = MsgTypeIdInvalid;

            /*! Leave and look for following messages */
            exitLoop = true;
            messageReadFlag = true;
            break;
        }

        if (messageReadFlag) {
            msgReadCount++;
            rxMsgBufferReadOffset = (rxMsgBufferReadOffset+1) & RX_MSG_BUFFER_MASK;
        }

        if (exitLoop) {
            break;
        }
    }

    rxMutexLock.lock();
    totMsg += rxMsgBufferReadLength;
    totCnt++;

    rxMsgBufferReadLength -= msgReadCount;
    rxMsgBufferNotFull.notify_all();
    rxMutexLock.unlock();

    if (totCnt > 50*700) {
        std::cout << "totMsg/totCnt " << totMsg/totCnt << std::endl;
        totMsg = 0;
        totCnt = 0;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher_384Fake::fillBuffer() {
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */

    while (bytesRead+(totalChannelsNum+3)*RX_WORD_SIZE < OKY_RX_BUFFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X03;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        syntheticData += 1;
        bytesRead += (totalChannelsNum+3)*RX_WORD_SIZE;
    }

    return Success;
}
