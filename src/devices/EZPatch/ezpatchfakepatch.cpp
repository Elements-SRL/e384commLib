#include "ezpatchfakepatch.h"
#include "utils.h"

#include <thread>
#include <ctime>
#include <cmath>
#include <sstream>

/*****************\
 *  Ctor / Dtor  *
\*****************/

EZPatchFakePatch::EZPatchFakePatch(std::string di) :
    EZPatchePatchEL03F_4F_PCBV03_V03(di) {

    /*! Sampling rates */
    samplingRatesNum = 4;
    samplingRatesArray.resize(samplingRatesNum);

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate1_25kHz].value = 1.25;
    realSamplingRatesArray[SamplingRate1_25kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate1_25kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate5kHz].value = 5.0;
    realSamplingRatesArray[SamplingRate5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate10kHz].value = 10.0;
    realSamplingRatesArray[SamplingRate10kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate10kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate20kHz].value = 20.0;
    realSamplingRatesArray[SamplingRate20kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate20kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate1_25kHz].value = 800.0;
    integrationStepArray[SamplingRate1_25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_25kHz].unit = "s";
    integrationStepArray[SamplingRate5kHz].value = 200.0;
    integrationStepArray[SamplingRate5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5kHz].unit = "s";
    integrationStepArray[SamplingRate10kHz].value = 100.0;
    integrationStepArray[SamplingRate10kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate10kHz].unit = "s";
    integrationStepArray[SamplingRate20kHz].value = 50.0;
    integrationStepArray[SamplingRate20kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate20kHz].unit = "s";

    /*! Protocols parameters */
    protocolFpgaClockFrequencyHz = 10.0e3;

    protocolTimeRange.step = 1000.0/protocolFpgaClockFrequencyHz;
    protocolTimeRange.min = LINT32_MIN*protocolTimeRange.step;
    protocolTimeRange.max = LINT32_MAX*protocolTimeRange.step;
    protocolTimeRange.prefix = UnitPfxMilli;
    protocolTimeRange.unit = "s";

    positiveProtocolTimeRange = protocolTimeRange;
    positiveProtocolTimeRange.min = 0.0;

    protocolMaxItemsNum = 1e6;

    voltageProtocolStepImplemented = true;
    currentProtocolStepImplemented = true;
    voltageProtocolRampImplemented = true;
    currentProtocolRampImplemented = true;
    voltageProtocolSinImplemented = true;
    currentProtocolSinImplemented = true;

    voltageHoldTunerImplemented = true;
    currentHoldTunerImplemented = true;

    /*! LEDs */
    ledsNum = 0;
    ledsWord.resize(ledsNum);
    ledsByte.resize(ledsNum);
    ledsColorsArray.resize(ledsNum);
}

/************************\
 *  Connection methods  *
\************************/

void EZPatchFakePatch::createCommunicationThreads() {
    rxThread = std::thread(&EZPatchFakePatch::readAndParseMessagesForGenerator, this);
    txThread = std::thread(&EZPatchFakePatch::unwrapAndSendMessagesForGenerator, this);
    gnThread = std::thread(&EZPatchFakePatch::generateData, this);
    satThread = std::thread(&EZPatchFakePatch::saturationFromGenerator, this);

    threadsStarted = true;
}

void EZPatchFakePatch::joinCommunicationThreads() {
    if (threadsStarted) {
        rxThread.join();
        txThread.join();
        gnThread.join();
        satThread.join();

        threadsStarted = false;
    }
}

/*******************************\
 *  Rx methods from generator  *
\*******************************/

void EZPatchFakePatch::ackFromGenerator(uint16_t hb) {
    std::unique_lock <std::mutex> locker(genRxMutex);
    /*! This assumes no more than 1 ack can be expected at any time */
    ackFromGenAvailable = true;
    lastAckHbFromGen = hb;
    genRxMsgBufferNotEmpty.notify_all();
}

void EZPatchFakePatch::saturationFromGenerator() {
    long checkStepMs = 1000.0L;
    std::chrono::time_point <std::chrono::system_clock> checkTime;

    checkTime = std::chrono::system_clock::now()+std::chrono::milliseconds(checkStepMs);

    while (!stopConnectionFlag) {
        std::this_thread::sleep_until(checkTime);
        checkTime += std::chrono::milliseconds(checkStepMs);

        if (stopConnectionFlag) {
            break;
        }

        if (saturationFlag) {
            std::unique_lock <std::mutex> rxMutexLock(rxMutex);
            while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
                txMsgBufferNotFull.wait(rxMutexLock);
            }
            rxMutexLock.unlock();
            saturationFlag = false;

            rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat++;
            rxMsgBuffer[rxMsgBufferWriteOffset].typeId = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation;
            rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
            rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = 1;

            rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;

            rxDataBuffer[rxDataBufferWriteOffset] = 0x0001; /*!< For this device only channel 1 can saturate */

            rxDataBufferWriteOffset = (rxDataBufferWriteOffset+1)&EZP_RX_DATA_BUFFER_MASK;

            rxMutexLock.lock();
            rxMsgBufferReadLength++;
            rxMsgBufferNotEmpty.notify_all();
            rxMutexLock.unlock();
        }
    }
}

/******************************\
 *  Tx methods for generator  *
\******************************/

ErrorCodes_t EZPatchFakePatch::setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    ErrorCodes_t ret;
    ret = EZPatchDevice::setVCCurrentRange(currentRangeIdx, applyFlag);
    if (ret == Success) {
        genVcCurrentRange = vcCurrentRangesArray[currentRangeIdx];
        genVcCurrentRange.convertValues(UnitPfxNone);
        this->selectChannelsResolutions();
    }
    return ret;
}

ErrorCodes_t EZPatchFakePatch::setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    ErrorCodes_t ret;
    ret = EZPatchDevice::setCCCurrentRange(currentRangeIdx, applyFlag);
    if (ret == Success) {
        genCcCurrentRange = ccCurrentRangesArray[currentRangeIdx];
        genCcCurrentRange.convertValues(UnitPfxNone);
        this->selectChannelsResolutions();
    }
    return ret;
}

ErrorCodes_t EZPatchFakePatch::setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) {
    ErrorCodes_t ret;
    ret = EZPatchDevice::setSamplingRate(samplingRateIdx, applyFlag);
    if (ret == Success) {
        genSamplingRate = samplingRate;
        genSamplingRate.convertValue(UnitPfxNone);
        integrationItemStepsNum = (int)round(genSamplingRate.value*integrationStep);
        samplingTime = 1.0/genSamplingRate.value;
    }
    return ret;
}

ErrorCodes_t EZPatchFakePatch::digitalOffsetCompensation(uint16_t channelIdx) {
    std::unique_lock <std::mutex> genRxMutexLock(genRxMutex);
    std::unique_lock <std::mutex> rxMutexLock(rxMutex);
    while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
        txMsgBufferNotFull.wait(rxMutexLock);
    }
    rxMutexLock.unlock();

    rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat++;
    rxMsgBuffer[rxMsgBufferWriteOffset].typeId = MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp;
    rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
    rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = 2;

    rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;

    rxDataBuffer[rxDataBufferWriteOffset] = channelIdx;
    rxDataBuffer[(rxDataBufferWriteOffset+1)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)round(13.2/liquidJunctionControl.step+liquidJunctionOffsetBinary);

    rxDataBufferWriteOffset = (rxDataBufferWriteOffset+2)&EZP_RX_DATA_BUFFER_MASK;

    rxMutexLock.lock();
    rxMsgBufferReadLength++;
    rxMsgBufferNotEmpty.notify_all();
    rxMutexLock.unlock();
    return Success;
}

ErrorCodes_t EZPatchFakePatch::resetFpga() {
    std::unique_lock <std::mutex> genRxMutexLock(genRxMutex);
    std::unique_lock <std::mutex> rxMutexLock(rxMutex);
    while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
        txMsgBufferNotFull.wait(rxMutexLock);
    }
    rxMutexLock.unlock();
    protocolOpened = false;

    rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat++;
    rxMsgBuffer[rxMsgBufferWriteOffset].typeId = MsgDirectionDeviceToPc+MsgTypeIdFpgaReset;
    rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
    rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = 0;

    rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;

    rxMutexLock.lock();
    rxMsgBufferReadLength++;
    rxMsgBufferNotEmpty.notify_all();
    rxMutexLock.unlock();
    return Success;
}

/*********************\
 *  Private methods  *
\*********************/

void EZPatchFakePatch::selectChannelsResolutions() {
    EZPatchePatchEL03F_4F_PCBV03_V03::selectChannelsResolutions();
    for (unsigned int channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        if (selectedCurrentSourceIdx == ChannelSourceCurrentFromVoltageClamp) {
            genCurrentNorm = 1.0/genVcCurrentRange.step;
            currentTunerCorrection[channelIdx] = 0.0;

        } else if (selectedCurrentSourceIdx == ChannelSourceCurrentFromCurrentClamp) {
            genCurrentNorm = 1.0/genCcCurrentRange.step;
            currentTunerCorrection[channelIdx] = selectedCurrentHoldVector[channelIdx].value;
        }

        if (selectedVoltageSourceIdx == ChannelSourceVoltageFromVoltageClamp) {
            genVoltageNorm = 1.0/genVcVoltageRange.step;
            voltageTunerCorrection[channelIdx] = selectedVoltageHoldVector[channelIdx].value;

        } else if (selectedVoltageSourceIdx == ChannelSourceVoltageFromCurrentClamp) {
            genVoltageNorm = 1.0/genCcVoltageRange.step;
            voltageTunerCorrection[channelIdx] = 0.0;
        }
    }
}

void EZPatchFakePatch::readAndParseMessagesForGenerator() {
    parsingFlag = true;

    std::unique_lock <std::mutex> genRxMutexLock(genRxMutex);
    genRxMutexLock.unlock();

    while ((!stopConnectionFlag) || (txWaitingOnAcks > 0)) {

        /******************\
         *  Reading part  *
        \******************/

        genRxMutexLock.lock();
        while ((!ackFromGenAvailable) && (genRxMsgBufferLen <= 0)) {
            genRxMsgBufferNotEmpty.wait_for(genRxMutexLock, std::chrono::milliseconds(1000));
            if (stopConnectionFlag && (txWaitingOnAcks == 0)) {
                break;
            }
        }
        genRxMutexLock.unlock();
        if (stopConnectionFlag && (txWaitingOnAcks == 0)) {
            continue;
        }

        genRxMutexLock.lock();
        if (ackFromGenAvailable) {
            txAckMutex.lock();
            txAckReceived = true;
            txAckCv.notify_all();
            txAckMutex.unlock();

            ackFromGenAvailable = false;
        }
        genRxMutexLock.unlock();
    }

    if (rxMsgBufferReadLength <= 0) {
        rxMutex.lock();
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
        rxMutex.unlock();
    }
}

void EZPatchFakePatch::unwrapAndSendMessagesForGenerator() {
    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed  */

    /*! Variables used to access the tx data buffer */
    uint32_t txDataBufferReadOffset = 0;

    /*! Tx data length variables */
    uint16_t msgTypeId;
    uint16_t txDataWords;

    std::unique_lock <std::mutex> txMutexLock(txMutex);
    txMutexLock.unlock();

    std::unique_lock <std::mutex> genTxMutexLock(genTxMutex);
    genTxMutexLock.unlock();

    ProtocolItem_t protocolItem;

    double aCoeff = 1.0;
    double bCoeff = 1.0;
    double cCoeff = 1.0;

    while ((!stopConnectionFlag) || (txWaitingOnAcks > 0)) {

        /*********************\
         *  Unwrapping part  *
        \*********************/

        txMutexLock.lock();
        while (txMsgBufferReadLength <= 0) {
            txMsgBufferNotEmpty.wait_for(txMutexLock, std::chrono::milliseconds(100));
            if (stopConnectionFlag && (txWaitingOnAcks == 0)) {
                break;
            }
        }
        txMutexLock.unlock();
        if (stopConnectionFlag && (txWaitingOnAcks == 0)) {
            continue;
        }

        msgTypeId = txMsgBuffer[txMsgBufferReadOffset].typeId;
        txDataBufferReadOffset = txMsgBuffer[txMsgBufferReadOffset].startDataPtr;
        txDataWords = txMsgBuffer[txMsgBufferReadOffset].dataLength;

        protocolItem.type = ProtocolItemVStepTStep;

        switch (msgTypeId) {
        case MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl:
            for (unsigned int txDataWord = 0; txDataWord < txDataWords; txDataWord++) {
                if (txDataBuffer[(txDataBufferReadOffset+txDataWord)&EZP_TX_DATA_BUFFER_MASK] == vcHoldTunerHwRegisterOffset) {
                    int32_t rawSample = (int32_t)(txDataBuffer[(txDataBufferReadOffset+txDataWord+1)&EZP_TX_DATA_BUFFER_MASK]) & 0x0000FFFF;
                    rawSample += (int32_t)((((uint32_t)(txDataBuffer[(txDataBufferReadOffset+txDataWord+3)&EZP_TX_DATA_BUFFER_MASK])) << 16) & 0xFFFF0000);
                    genVHoldTuner = ((double)rawSample)*vcVoltageRangesArray[selectedVcVoltageRangeIdx].step;
                    txDataWord += 3;

                } else if (txDataBuffer[(txDataBufferReadOffset+txDataWord)&EZP_TX_DATA_BUFFER_MASK] == ccHoldTunerHwRegisterOffset) {
                    int32_t rawSample = (int32_t)(txDataBuffer[(txDataBufferReadOffset+txDataWord+1)&EZP_TX_DATA_BUFFER_MASK]) & 0x0000FFFF;
                    rawSample += (int32_t)((((uint32_t)(txDataBuffer[(txDataBufferReadOffset+txDataWord+3)&EZP_TX_DATA_BUFFER_MASK])) << 16) & 0xFFFF0000);
                    genIHoldTuner = ((double)rawSample)*ccCurrentRangesArray[selectedCcCurrentRangeIdx].step;
                    txDataWord += 3;
                }
            }
            break;

        case MsgDirectionPcToDevice+MsgTypeIdVoltageProtocolStruct:
        case MsgDirectionPcToDevice+MsgTypeIdCurrentProtocolStruct:
            genTxMutexLock.lock();
            builtProtocol.protocolId = txDataBuffer[(txDataBufferReadOffset)&EZP_TX_DATA_BUFFER_MASK];
            builtProtocol.itemsNum = txDataBuffer[(txDataBufferReadOffset+1)&EZP_TX_DATA_BUFFER_MASK];
            builtProtocol.sweepsNum = txDataBuffer[(txDataBufferReadOffset+2)&EZP_TX_DATA_BUFFER_MASK];
            builtProtocol.items.clear();
            genTxMutexLock.unlock();
            break;

        case MsgDirectionPcToDevice+MsgTypeIdVoltageStepTimeStep:
        case MsgDirectionPcToDevice+MsgTypeIdVoltageRamp:
        case MsgDirectionPcToDevice+MsgTypeIdVoltageSin:
        case MsgDirectionPcToDevice+MsgTypeIdCurrentStepTimeStep:
        case MsgDirectionPcToDevice+MsgTypeIdCurrentRamp:
        case MsgDirectionPcToDevice+MsgTypeIdCurrentSin:
            genTxMutexLock.lock();
            if (msgTypeId == MsgDirectionPcToDevice+MsgTypeIdVoltageStepTimeStep) {
                protocolItem.type = ProtocolItemVStepTStep;
                aCoeff = vcVoltageRangesArray[selectedVcVoltageRangeIdx].step;
                bCoeff = protocolTimeRange.step;
                cCoeff = 1.0;

            } else if (msgTypeId == MsgDirectionPcToDevice+MsgTypeIdVoltageRamp) {
                protocolItem.type = ProtocolItemVRamp;
                aCoeff = vcVoltageRangesArray[selectedVcVoltageRangeIdx].step;
                bCoeff = vcVoltageRangesArray[selectedVcVoltageRangeIdx].step;
                cCoeff = protocolTimeRange.step;

            } else if (msgTypeId == MsgDirectionPcToDevice+MsgTypeIdVoltageSin) {
                protocolItem.type = ProtocolItemVSin;
                aCoeff = vcVoltageRangesArray[selectedVcVoltageRangeIdx].step;
                bCoeff = vcVoltageRangesArray[selectedVcVoltageRangeIdx].step;
                cCoeff = protocolFrequencyRange.step;

            } else if (msgTypeId == MsgDirectionPcToDevice+MsgTypeIdCurrentStepTimeStep) {
                protocolItem.type = ProtocolItemIStepTStep;
                aCoeff = ccCurrentRangesArray[selectedCcCurrentRangeIdx].step;
                bCoeff = protocolTimeRange.step;
                cCoeff = 1.0;

            } else if (msgTypeId == MsgDirectionPcToDevice+MsgTypeIdCurrentRamp) {
                protocolItem.type = ProtocolItemIRamp;
                aCoeff = ccCurrentRangesArray[selectedCcCurrentRangeIdx].step;
                bCoeff = ccCurrentRangesArray[selectedCcCurrentRangeIdx].step;
                cCoeff = protocolTimeRange.step;

            } else if (msgTypeId == MsgDirectionPcToDevice+MsgTypeIdCurrentSin) {
                protocolItem.type = ProtocolItemISin;
                aCoeff = ccCurrentRangesArray[selectedCcCurrentRangeIdx].step;
                bCoeff = ccCurrentRangesArray[selectedCcCurrentRangeIdx].step;
                cCoeff = protocolFrequencyRange.step;
            }

            protocolItem.a0 = ((double)(* ((int32_t *)(txDataBuffer+((txDataBufferReadOffset)&EZP_TX_DATA_BUFFER_MASK)))))*aCoeff;
            protocolItem.da = ((double)(* ((int32_t *)(txDataBuffer+((txDataBufferReadOffset+2)&EZP_TX_DATA_BUFFER_MASK)))))*aCoeff;
            protocolItem.b0 = ((double)(* ((int32_t *)(txDataBuffer+((txDataBufferReadOffset+4)&EZP_TX_DATA_BUFFER_MASK)))))*bCoeff;
            protocolItem.db = ((double)(* ((int32_t *)(txDataBuffer+((txDataBufferReadOffset+6)&EZP_TX_DATA_BUFFER_MASK)))))*bCoeff;
            protocolItem.c0 = ((double)(* ((int32_t *)(txDataBuffer+((txDataBufferReadOffset+8)&EZP_TX_DATA_BUFFER_MASK)))))*cCoeff;
            protocolItem.dc = ((double)(* ((int32_t *)(txDataBuffer+((txDataBufferReadOffset+10)&EZP_TX_DATA_BUFFER_MASK)))))*cCoeff;

            protocolItem.currentItem = txDataBuffer[(txDataBufferReadOffset+12)&EZP_TX_DATA_BUFFER_MASK];
            protocolItem.nextItem = txDataBuffer[(txDataBufferReadOffset+13)&EZP_TX_DATA_BUFFER_MASK];
            protocolItem.repsNum = txDataBuffer[(txDataBufferReadOffset+14)&EZP_TX_DATA_BUFFER_MASK];
            protocolItem.applySteps = (txDataBuffer[(txDataBufferReadOffset+15)&EZP_TX_DATA_BUFFER_MASK] > 0 ? true : false);

            if (protocolItem.currentItem < builtProtocol.items.size()) {
                builtProtocol.items.resize(protocolItem.currentItem);
            }
            builtProtocol.items.push_back(protocolItem);
            if (builtProtocol.items.size() == builtProtocol.itemsNum) {
                pushedProtocol = builtProtocol;
                pushedProtocol.items = builtProtocol.items;
                protocolAvailable = true;
                genTxProtocolAvailable.notify_all();
            }
            genTxMutexLock.unlock();
            break;
        }

        if (txExpectAckMap[msgTypeId]) {
            this->ackFromGenerator(txMsgBuffer[txMsgBufferReadOffset].heartbeat);
        }

        txMsgBufferReadOffset = (txMsgBufferReadOffset+1)&EZP_TX_MSG_BUFFER_MASK;

        /******************\
         *  Sending part  *
        \******************/

        txMutexLock.lock();
        txMsgBufferReadLength--;
        txMsgBufferNotFull.notify_all();
        txMutexLock.unlock();
    }
}

void EZPatchFakePatch::generateData() {
    std::unique_lock <std::mutex> genTxMutexLock(genTxMutex);
    genTxMutexLock.unlock();

    std::unique_lock <std::mutex> genParamMutexLock(genParamMutex);
    genParamMutexLock.unlock();

    genVcVoltageRange = vcVoltageRangesArray[VCVoltageRange500mV];
    genCcVoltageRange = ccVoltageRangesArray[CCVoltageRange700mV];
    genVcCurrentRange = vcCurrentRangesArray[VCCurrentRange200pA];
    genCcCurrentRange = ccCurrentRangesArray[CCCurrentRange2500pA];
    genSamplingRate = samplingRate;
    genVcVoltageRange.convertValues(UnitPfxNone);
    genCcVoltageRange.convertValues(UnitPfxNone);
    genVcCurrentRange.convertValues(UnitPfxNone);
    genCcCurrentRange.convertValues(UnitPfxNone);
    genSamplingRate.convertValue(UnitPfxNone);
    samplingTime = 1.0/genSamplingRate.value;
    this->selectChannelsResolutions();
    double tempValue;
    double temp2Value;

    unsigned short protocolId;
    unsigned short itemIdx;
    unsigned short repsIdx;
    unsigned short sweepIdx;

    int itemStepsIdx = 0;
    int itemStepsNum = 0;
    int itemStepsInt;

    bool itemFinished;

    long integrationStepMs = (long)(integrationStep*1000.0);
    integrationItemStepsNum = (int)round(genSamplingRate.value*integrationStep);

    ProtocolItem_t protocolItem;
    protocolItem.type = ProtocolItemVStepTStep;

    /*! Variables used to access the rx data buffer */
    uint32_t rxDataBufferWriteIdx = 0; /*!< Index being written wrt rxDataBufferWriteOffset */

    uint32_t firstSampleIdx;

    std::unique_lock <std::mutex> genRxMutexLock(genRxMutex);
    genRxMutexLock.unlock();

    std::unique_lock <std::mutex> rxMutexLock(rxMutex);
    rxMutexLock.unlock();

    std::chrono::time_point <std::chrono::system_clock> sampleTime;

    /*! For each protocol */
    while (!stopConnectionFlag) {
        genTxMutexLock.lock();
        while (!protocolAvailable) {
            genTxProtocolAvailable.wait_for(genTxMutexLock, std::chrono::milliseconds(1000));
            if (stopConnectionFlag) {
                break;
            }
        }

        poppedProtocol = pushedProtocol;
        poppedProtocol.items = pushedProtocol.items;

        protocolAvailable = false;
        genTxMutexLock.unlock();

        protocolId = poppedProtocol.protocolId;

        itemIdx = 0;
        repsIdx = 0;
        sweepIdx = 0;

        firstSampleIdx = 0;
        itemStepsInt = 0;

        itemFinished = true;
        protocolOpened = true;

        sampleTime = std::chrono::system_clock::now()+std::chrono::milliseconds(integrationStepMs);

        /*! For each iteration within protocol */
        while (!stopConnectionFlag && protocolOpened) {
            genTxMutexLock.lock();
            if (protocolAvailable) {
                genRxMutexLock.lock();
                rxMutexLock.lock();
                while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
                    txMsgBufferNotFull.wait(rxMutexLock);
                }
                rxMutexLock.unlock();

                rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat++;
                rxMsgBuffer[rxMsgBufferWriteOffset].typeId = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail;
                rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
                rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = 1;

                rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;

                rxDataBuffer[rxDataBufferWriteOffset] = protocolId;

                rxDataBufferWriteOffset = (rxDataBufferWriteOffset+1)&EZP_RX_DATA_BUFFER_MASK;

                rxMutexLock.lock();
                rxMsgBufferReadLength++;
                rxMsgBufferNotEmpty.notify_all();
                rxMutexLock.unlock();
                genRxMutexLock.unlock();
                protocolOpened = false;

                genTxMutexLock.unlock();
                continue;
            }
            genTxMutexLock.unlock();

            if (itemFinished) {
                protocolItem = poppedProtocol.items[itemIdx];

                switch (protocolItem.type) {
                case ProtocolItemVStepTStep:
                    genVoltage = protocolItem.a0+protocolItem.da*((protocolItem.applySteps ? (double)repsIdx : 0.0)+sweepIdx);
                    genCurrent = genVoltage/genResistance;
                    itemStepsNum = (int)round(genSamplingRate.value*(protocolItem.b0+protocolItem.db*((protocolItem.applySteps ? (double)repsIdx : 0.0)+sweepIdx)));
                    break;

                case ProtocolItemVRamp:
                    genVoltage = protocolItem.a0;
                    genCurrent = genVoltage/genResistance;
                    itemStepsNum = (int)round(genSamplingRate.value*protocolItem.c0);
                    genVoltageStep = (protocolItem.b0-genVoltage)/(double(itemStepsNum));
                    genCurrentStep = genVoltageStep/genResistance;
                    break;

                case ProtocolItemVSin:
                    genVoltage = protocolItem.a0;
                    genCurrent = genVoltage/genResistance;
                    genVoltageAmp = protocolItem.b0;
                    genCurrentAmp = genVoltageAmp/genResistance;
                    itemStepsNum = (int)round(genSamplingRate.value/protocolItem.c0);
                    genVoltageStep = 8.0*atan(1.0)/((double)itemStepsNum);
                    genCurrentStep = genVoltageStep;
                    break;

                case ProtocolItemIStepTStep:
                    genCurrent = protocolItem.a0+protocolItem.da*((protocolItem.applySteps ? (double)repsIdx : 0.0)+sweepIdx);
                    itemStepsNum = (int)round(genSamplingRate.value*(protocolItem.b0+protocolItem.db*((protocolItem.applySteps ? (double)repsIdx : 0.0)+sweepIdx)));
                    break;

                case ProtocolItemIRamp:
                    genCurrent = protocolItem.a0;
                    itemStepsNum = (int)round(genSamplingRate.value*protocolItem.c0);
                    genCurrentStep = (protocolItem.b0-genCurrent)/(double(itemStepsNum));
                    break;

                case ProtocolItemISin:
                    genCurrent = protocolItem.a0;
                    genCurrentAmp = protocolItem.b0;
                    itemStepsNum = (int)round(genSamplingRate.value/protocolItem.c0);
                    genCurrentStep = 8.0*atan(1.0)/((double)itemStepsNum);
                    break;
                }

                genRxMutexLock.lock();
                rxMutexLock.lock();
                while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
                    txMsgBufferNotFull.wait(rxMutexLock);
                }
                rxMutexLock.unlock();

                rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat++;
                rxMsgBuffer[rxMsgBufferWriteOffset].typeId = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader;
                rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
                rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = 4;

                rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;

                rxDataBuffer[rxDataBufferWriteOffset] = protocolId;
                rxDataBuffer[(rxDataBufferWriteOffset+1)&EZP_RX_DATA_BUFFER_MASK] = itemIdx;
                rxDataBuffer[(rxDataBufferWriteOffset+2)&EZP_RX_DATA_BUFFER_MASK] = repsIdx;
                rxDataBuffer[(rxDataBufferWriteOffset+3)&EZP_RX_DATA_BUFFER_MASK] = sweepIdx;

                rxDataBufferWriteOffset = (rxDataBufferWriteOffset+4)&EZP_RX_DATA_BUFFER_MASK;

                rxMutexLock.lock();
                rxMsgBufferReadLength++;
                rxMsgBufferNotEmpty.notify_all();
                rxMutexLock.unlock();
                genRxMutexLock.unlock();
                itemFinished = false;
            }

            genRxMutexLock.lock();
            rxMutexLock.lock();
            while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
                txMsgBufferNotFull.wait(rxMutexLock);
            }
            rxMutexLock.unlock();

            rxDataBufferWriteIdx = 0;

            rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(firstSampleIdx & 0xFFFF);
            rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)((firstSampleIdx >> 16) & 0xFFFF);

            genParamMutexLock.lock();
            switch (protocolItem.type) {
            case ProtocolItemVStepTStep:
                genVoltageInt = (uint16_t)(32768.0+genVoltage*genVoltageNorm);
                tempValue = (genCurrent+genVHoldTuner/genResistance)*genCurrentNorm;
                genCurrentInt = (uint16_t)(32768.0+this->checkSaturation(tempValue));

                for (itemStepsIdx = 0; (itemStepsIdx < itemStepsNum) && (itemStepsIdx < integrationItemStepsNum); itemStepsIdx++) {
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = genVoltageInt;
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = genCurrentInt;
                }
                break;

            case ProtocolItemVRamp:
                for (itemStepsIdx = 0; (itemStepsIdx < itemStepsNum) && (itemStepsIdx < integrationItemStepsNum); itemStepsIdx++) {
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+(((double)(itemStepsInt+itemStepsIdx))*genVoltageStep+genVoltage)*genVoltageNorm);
                    tempValue = (((double)(itemStepsInt+itemStepsIdx))*genCurrentStep+genCurrent+genVHoldTuner/genResistance)*genCurrentNorm;
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+this->checkSaturation(tempValue));
                }
                break;

            case ProtocolItemVSin:
                for (itemStepsIdx = 0; (itemStepsIdx < itemStepsNum) && (itemStepsIdx < integrationItemStepsNum); itemStepsIdx++) {
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+(sin(((double)(itemStepsInt+itemStepsIdx))*genVoltageStep)*genVoltageAmp+genVoltage)*genVoltageNorm);
                    tempValue = (sin(((double)(itemStepsInt+itemStepsIdx))*genCurrentStep)*genCurrentAmp+genCurrent+genVHoldTuner/genResistance)*genCurrentNorm;
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+this->checkSaturation(tempValue));
                }
                break;

            case ProtocolItemIStepTStep:
                genCurrentInt = (uint16_t)(32768.0+genCurrent*genCurrentNorm);

                for (itemStepsIdx = 0; (itemStepsIdx < itemStepsNum) && (itemStepsIdx < integrationItemStepsNum); itemStepsIdx++) {
                    this->integrateFHNModel(genCurrent+genIHoldTuner);
                    tempValue = genVoltage*genVoltageNorm;
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+this->checkSaturation(tempValue));
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = genCurrentInt;
                }
                break;

            case ProtocolItemIRamp:
                for (itemStepsIdx = 0; (itemStepsIdx < itemStepsNum) && (itemStepsIdx < integrationItemStepsNum); itemStepsIdx++) {
                    temp2Value = ((double)(itemStepsInt+itemStepsIdx))*genCurrentStep+genCurrent;
                    this->integrateFHNModel(temp2Value+genIHoldTuner);
                    tempValue = genVoltage*genVoltageNorm;
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+this->checkSaturation(tempValue));
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+temp2Value*genCurrentNorm);
                }
                break;

            case ProtocolItemISin:
                for (itemStepsIdx = 0; (itemStepsIdx < itemStepsNum) && (itemStepsIdx < integrationItemStepsNum); itemStepsIdx++) {
                    temp2Value = sin(((double)(itemStepsInt+itemStepsIdx))*genCurrentStep)*genCurrentAmp+genCurrent;
                    this->integrateFHNModel(temp2Value+genIHoldTuner);
                    tempValue = genVoltage*genVoltageNorm;
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+this->checkSaturation(tempValue));
                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++)&EZP_RX_DATA_BUFFER_MASK] = (uint16_t)(32768.0+temp2Value*genCurrentNorm);
                }
                break;
            }
            genParamMutexLock.unlock();
            itemStepsNum -= itemStepsIdx;
            itemStepsInt += itemStepsIdx;

            rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat++;
            rxMsgBuffer[rxMsgBufferWriteOffset].typeId = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData;
            rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
            rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = (uint16_t)rxDataBufferWriteIdx;

            rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;

            rxDataBufferWriteOffset = (rxDataBufferWriteOffset+rxDataBufferWriteIdx)&EZP_RX_DATA_BUFFER_MASK;

            rxMutexLock.lock();
            rxMsgBufferReadLength++;
            rxMsgBufferNotEmpty.notify_all();
            rxMutexLock.unlock();
            genRxMutexLock.unlock();

            if (0 == itemStepsNum) {
                itemStepsInt = 0;
                itemFinished = true;

                if (protocolItem.nextItem < itemIdx+1) {
                    /*! The current item is the end of an internal loop */
                    if ((protocolItem.repsNum > repsIdx+1) ||
                            (protocolItem.repsNum == 0)) {
                        /*! Either not reached loopRes for internal loop or infinite loop, so goto loop initial item and increase repsIdx */
                        repsIdx = (repsIdx+1)&0xFFFF;
                        itemIdx = (protocolItem.nextItem);

                    } else {
                        /*! Reached loopRes for internal loop, so goto next item and reset repsIdx */
                        repsIdx = 0;
                        itemIdx++;
                    }

                } else {
                    /*! The current item is not the end of an internal loop, so goto next item */
                    itemIdx++;
                }

                if (itemIdx >= poppedProtocol.itemsNum) {
                    if (sweepIdx+1 < poppedProtocol.sweepsNum) {
                        /*! If the protocol is finished but we've not performed all the sweeps, reset the protocol and start the new sweep */
                        sweepIdx++;
                        itemIdx = 0;

                    } else {
                        /*! If the protocol is finished but we've performed all the sweeps, exit the loop */
                        genRxMutexLock.lock();
                        rxMutexLock.lock();
                        while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
                            txMsgBufferNotFull.wait(rxMutexLock);
                        }
                        rxMutexLock.unlock();

                        rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat++;
                        rxMsgBuffer[rxMsgBufferWriteOffset].typeId = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail;
                        rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
                        rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = 1;

                        rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;

                        rxDataBuffer[(rxDataBufferWriteOffset)&EZP_RX_DATA_BUFFER_MASK] = protocolId;

                        rxDataBufferWriteOffset = (rxDataBufferWriteOffset+1)&EZP_RX_DATA_BUFFER_MASK;

                        rxMutexLock.lock();
                        rxMsgBufferReadLength++;
                        rxMsgBufferNotEmpty.notify_all();
                        rxMutexLock.unlock();
                        genRxMutexLock.unlock();
                        protocolOpened = false;
                    }
                }
            }

            std::this_thread::sleep_until(sampleTime);
            sampleTime += std::chrono::milliseconds(integrationStepMs);
        }
    }
}

double EZPatchFakePatch::checkSaturation(double value) {
    if (value > 32767.0) {
        value = 32767.0;
        saturationFlag = true;

    } else if (value < -32768.0) {
        value = -32768.0;
        saturationFlag = true;
    }
    return value;
}

void EZPatchFakePatch::integrateFHNModel(double current) {
    dvFHN = vFHN-vFHN*vFHN*vFHN/3e-6-wFHN+current*20.0e6;
    dwFHN = (vFHN+aFHN-bFHN*wFHN)/tFHN;
    vFHN += samplingTime*dvFHN*1000.0;
    wFHN += samplingTime*dwFHN*1000.0;
    genVoltage = vFHN*33.5-0.0248;
}
