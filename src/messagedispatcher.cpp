﻿#include "messagedispatcher.h"

#include <iostream>
#include <ctime>
#include <thread>
#include <math.h>
#include <random>
#include <algorithm>

#include "okFrontPanelDLL.h"

#include "messagedispatcher_384nanopores.h"
#include "messagedispatcher_384patchclamp.h"
#include "messagedispatcher_4x10mhz.h"
#include "messagedispatcher_2x10mhz.h"
#ifdef DEBUG
/*! Fake device that generates synthetic data */
#include "messagedispatcher_384fake.h"
#include "messagedispatcher_384fakepatchclamp.h"
#include "messagedispatcher_4x10mhzfake.h"
#endif
#include "calibrationmanager.h"
#include "utils.h"

static std::unordered_map <std::string, DeviceTypes_t> deviceIdMapping = {
    {"221000107S", Device384Nanopores},
    {"221000108T", Device384Nanopores},
    {"2210001076", Device384PatchClamp},
    {"221000106B", Device384PatchClamp},
    {"221000106C", Device384PatchClamp},
    {"22370012CI", Device4x10MHz},
    {"22370012CB", Device2x10MHz},
    {"224800131L", Device2x10MHz},
    {"224800130Y", Device2x10MHz},
    {"224800130X", Device4x10MHz}
#ifdef DEBUG
    ,{"FAKE_Nanopores", Device384Fake}
    ,{"FAKE_PATCH_CLAMP", Device384FakePatchClamp}
    ,{"FAKE_4x10MHz", Device4x10MHzFake}
#endif
}; /*! \todo FCON queste info dovrebbero risiedere nel DB */

/********************************************************************************************\
 *                                                                                          *
 *                                 MessageDispatcher                                        *
 *                                                                                          *
\********************************************************************************************/

/*****************\
 *  Ctor / Dtor  *
\*****************/

MessageDispatcher::MessageDispatcher(std::string deviceId) :
    deviceId(deviceId) {

    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAck] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdNack] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdPing] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdFpgaReset] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdInvalid] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus] = true;

    /*! Initialize rx word offsets and lengths with default values */
    rxWordOffsets.resize(RxMessageNum);
    rxWordLengths.resize(RxMessageNum);

    fill(rxWordOffsets.begin(), rxWordOffsets.end(), 0xFFFF);
    fill(rxWordLengths.begin(), rxWordLengths.end(), 0x0000);
}

MessageDispatcher::~MessageDispatcher() {
    this->disconnectDevice();

    for (auto coder : coders) {
        delete coder;
    }
}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t MessageDispatcher::detectDevices(
        std::vector <std::string> &deviceIds) {
    /*! Gets number of devices */
    int numDevs;
    bool devCountOk = getDeviceCount(numDevs);
    if (!devCountOk) {
        return ErrorListDeviceFailed;

#ifndef DEBUG
    } else if (numDevs == 0) {
        deviceIds.clear();
        return ErrorNoDeviceFound;
#endif
    }

    deviceIds.clear();

    /*! Lists all serial numbers */
    for (int i = 0; i < numDevs; i++) {
        deviceIds.push_back(getDeviceSerial(i));
    }

#ifdef DEBUG
    numDevs++;
    deviceIds.push_back("FAKE_Nanopores");
    numDevs++;
    deviceIds.push_back("FAKE_PATCH_CLAMP");
    numDevs++;
    deviceIds.push_back("FAKE_4x10MHz");
#endif

    return Success;
}

ErrorCodes_t MessageDispatcher::getDeviceType(std::string deviceId, DeviceTypes_t &type) {
    if (deviceIdMapping.count(deviceId) > 0) {
        type = deviceIdMapping[deviceId];
        return Success;

    } else {
        return ErrorDeviceTypeNotRecognized;
    }
}

ErrorCodes_t MessageDispatcher::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher) {
    ErrorCodes_t ret = Success;

    DeviceTypes_t deviceType;
    ret = MessageDispatcher::getDeviceType(deviceId, deviceType);
    if (ret != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    messageDispatcher = nullptr;

    switch (deviceType) {
    case Device384Nanopores:
        messageDispatcher = new MessageDispatcher_384NanoPores_V01(deviceId);
        break;

    case Device384PatchClamp:
        messageDispatcher = new MessageDispatcher_384PatchClamp_V01(deviceId);
        break;

    case Device2x10MHz:
        messageDispatcher = new MessageDispatcher_2x10MHz_V01(deviceId);
        break;

    case Device4x10MHz:
        messageDispatcher = new MessageDispatcher_4x10MHz_V01(deviceId);
        break;

#ifdef DEBUG
    case Device384Fake:
        messageDispatcher = new MessageDispatcher_384Fake(deviceId);
        break;
        
    case Device384FakePatchClamp:
        messageDispatcher = new MessageDispatcher_384FakePatchClamp(deviceId);
        break;

    case Device4x10MHzFake:
        messageDispatcher = new MessageDispatcher_4x10MHzFake(deviceId);
        break;
#endif

    default:
        return ErrorDeviceTypeNotRecognized;
    }

    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->connect();

        if (ret != Success) {
            messageDispatcher->disconnect();
            delete messageDispatcher;
            messageDispatcher = nullptr;
        }
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::disconnectDevice() {
    return this->disconnect();
}

ErrorCodes_t MessageDispatcher::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    if (!connected) {
        return ErrorDeviceNotConnected;
    }

    uint16_t uType = (uint16_t)messageType;

    if (uType < MsgDirectionDeviceToPc) {
        /*! This method controls only messages going from the device to the SW */
        uType += MsgDirectionDeviceToPc;
    }

    rxEnabledTypesMap[uType] = flag;

    return Success;
}

ErrorCodes_t MessageDispatcher::connect() {
    if (connected) {
        return ErrorDeviceAlreadyConnected;
    }

    connected = true;

    this->init();

    stopConnectionFlag = false;

#ifdef DEBUG_TX_DATA_PRINT
    if (txFid == nullptr) {
        createDebugFile(txFid, "e384CommLib_tx");
    }
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
    if (rxRawFid == nullptr) {
        createDebugFile(rxRawFid, "e384CommLib_rxRaw");
    }
#endif

#ifdef DEBUG_RX_PROCESSING_PRINT
    if (rxProcFid == nullptr) {
        createDebugFile(rxProcFid, "e384CommLib_rxProcessing");
    }
#endif

#ifdef DEBUG_RX_DATA_PRINT
    if (rxFid == nullptr) {
        createDebugFile(rxFid, "e384CommLib_rx");
    }
#endif

    deviceCommunicationThread = std::thread(&MessageDispatcher::handleCommunicationWithDevice, this);
    rxConsumerThread = std::thread(&MessageDispatcher::parseDataFromDevice, this);

    threadsStarted = true;

    /*! Initialize device */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    this->initializeDevice();

    return Success;
}


ErrorCodes_t MessageDispatcher::disconnect() {
    if (!connected) {
        return ErrorDeviceNotConnected;
    }

    if (!stopConnectionFlag) {
        stopConnectionFlag = true;

        if (threadsStarted) {
            deviceCommunicationThread.join();
            rxConsumerThread.join();
        }

        this->deinit();

        connected = false;

        return Success;

    } else {
        return ErrorDeviceNotConnected;
    }
}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t MessageDispatcher::initializeDevice() {
    amIinVoltageClamp = clampingModalitiesArray[defaultClampingModalityIdx] == VOLTAGE_CLAMP;

    this->initializeHW();

    /*! Some default values*/
    std::vector <bool> allTrue(currentChannelsNum, true);
    std::vector <bool> allFalse(currentChannelsNum, false);

    std::vector <uint16_t> channelIndexes(currentChannelsNum);
    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        channelIndexes[idx] = idx;
    }

    std::vector <uint16_t> boardIndexes(totalBoardsNum);
    for (uint16_t idx = 0; idx < totalBoardsNum; idx++) {
        boardIndexes[idx] = idx;
    }

    if (amIinVoltageClamp) {
        /*! Initialization in voltage clamp */
        this->enableCcCompensations(false);
        this->enableVcCompensations(true);
        this->enableStimulus(channelIndexes, allTrue, false);
        this->turnChannelsOn(channelIndexes, allTrue, false);
        this->turnCurrentReaderOn(true, false);
        this->setVoltageHoldTuner(channelIndexes, selectedVoltageHoldVector, false);
        this->setSamplingRate(defaultSamplingRateIdx, false);
        this->setVCCurrentRange(defaultVcCurrentRangeIdx, false);
        this->setVCVoltageRange(defaultVcVoltageRangeIdx, false);
        this->setVoltageStimulusLpf(selectedVcVoltageFilterIdx, false);
        this->setGateVoltagesTuner(boardIndexes, selectedGateVoltageVector, false);
        this->setSourceVoltagesTuner(boardIndexes, selectedSourceVoltageVector, false);
        this->digitalOffsetCompensation(channelIndexes, allFalse, false);
        if (calibrationData.vcCalibResArray.size() > 0) {
            this->turnCalSwOn(channelIndexes, allFalse, false);
        }

        CalibrationManager calibrationManager(deviceId, currentChannelsNum, totalBoardsNum, vcCurrentRangesNum, vcVoltageRangesNum, ccVoltageRangesNum, ccCurrentRangesNum);

        calibrationParams = calibrationManager.getCalibrationParams(calibrationLoadingError);
        calibrationFileNames = calibrationManager.getCalibrationFileNames();
        calibrationFilesOkFlags = calibrationManager.getCalibrationFilesOkFlags();
        calibrationMappingFileDir = calibrationManager.getMappingFileDir();
        calibrationMappingFilePath = calibrationManager.getMappingFilePath();

        /*! Perform CC calibration first to initialize the commlib values: the VC calibration commands will override the FPGA calibration later */
        if (ccVoltageRangesNum > 0) {
            this->setCalibCcVoltageGain(channelIndexes, calibrationParams.ccAllGainDacMeas[selectedCcVoltageRangeIdx], false);
            this->setCalibCcVoltageOffset(channelIndexes, calibrationParams.ccAllOffsetDacMeas[selectedCcVoltageRangeIdx], false);
        }

        if (ccCurrentRangesNum > 0) {
            this->setCalibCcCurrentGain(channelIndexes, calibrationParams.ccAllGainAdcMeas[selectedCcCurrentRangeIdx], false);
            this->setCalibCcCurrentOffset(channelIndexes, calibrationParams.ccAllOffsetAdcMeas[selectedCcCurrentRangeIdx], false);
        }

        if (vcCurrentRangesNum > 0) {
            this->setCalibVcCurrentGain(channelIndexes, calibrationParams.allGainAdcMeas[selectedVcCurrentRangeIdx], false);
            this->setCalibVcCurrentOffset(channelIndexes, calibrationParams.allOffsetAdcMeas[selectedVcCurrentRangeIdx], false);
        }

        if (vcVoltageRangesNum > 0) {
            this->setCalibVcVoltageGain(channelIndexes, calibrationParams.allGainDacMeas[selectedVcVoltageRangeIdx], false);
            this->setCalibVcVoltageOffset(channelIndexes, calibrationParams.allOffsetDacMeas[selectedVcVoltageRangeIdx], false);
        }

    } else {
        /*! Initialization in current clamp */
        /*! \todo FCON ... Noone deafult in current clamp, why bother? */
    }

    /*! Make sure that at the beginning all the constant values tha might not be written later on are sent to the FPGA */
    txModifiedStartingWord = 0;
    txModifiedEndingWord = txDataWords;

    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    return Success;
}

ErrorCodes_t MessageDispatcher::sendCommands() {
    this->stackOutgoingMessage(txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::resetAsic(bool resetFlag, bool applyFlagIn) {
    if (asicResetCoder != nullptr) {
        asicResetCoder->encode(resetFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::resetFpga(bool resetFlag, bool applyFlagIn) {
    if (fpgaResetCoder != nullptr) {
        fpgaResetCoder->encode(resetFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag){
    if (vHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(voltages, vHoldRange[selectedVcVoltageRangeIdx].getMin(), vHoldRange[selectedVcVoltageRangeIdx].getMax())) {
        return ErrorValueOutOfRange;

    } else if (!amIinVoltageClamp) {
        return ErrorWrongClampModality;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            voltages[i].convertValue(vHoldRange[selectedVcVoltageRangeIdx].prefix);
            selectedVoltageHoldVector[channelIndexes[i]] = voltages[i];
            vHoldTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag){
    if (cHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(currents, cHoldRange[selectedCcCurrentRangeIdx].getMin(), cHoldRange[selectedCcCurrentRangeIdx].getMax())) {
        return ErrorValueOutOfRange;

    } else if (amIinVoltageClamp) {
        return ErrorWrongClampModality;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            currents[i].convertValue(cHoldRange[selectedCcCurrentRangeIdx].prefix);
            selectedCurrentHoldVector[channelIndexes[i]] = currents[i];
            cHoldTunerCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(currents[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibVcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

//    } else if (!areAllTheVectorElementsInRange(gains, calibVcCurrentGainRange.getMin(), calibVcCurrentGainRange.getMax())) {
//        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibVcCurrentGainRange.prefix);
            selectedCalibVcCurrentGainVector[channelIndexes[i]] = gains[i];
            calibVcCurrentGainCoders[channelIndexes[i]]->encode(gains[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibVcCurrentOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

//    } else if (!areAllTheVectorElementsInRange(offsets, calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].getMin(), calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].getMax())) {
//        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].prefix);
            selectedCalibVcCurrentOffsetVector[channelIndexes[i]] = offsets[i];
            calibVcCurrentOffsetCoders[selectedVcCurrentRangeIdx][channelIndexes[i]]->encode(offsets[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibCcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

//    } else if (!areAllTheVectorElementsInRange(gains, calibCcVoltageGainRange.getMin(), calibCcVoltageGainRange.getMax())) {
//        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibCcVoltageGainRange.prefix);
            selectedCalibCcVoltageGainVector[channelIndexes[i]] = gains[i];
            calibCcVoltageGainCoders[channelIndexes[i]]->encode(gains[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibCcVoltageOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

//    } else if (!areAllTheVectorElementsInRange(offsets, calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].getMin(), calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].getMax())) {
//        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].prefix);
            selectedCalibCcVoltageOffsetVector[channelIndexes[i]] = offsets[i];
            calibCcVoltageOffsetCoders[selectedCcVoltageRangeIdx][channelIndexes[i]]->encode(offsets[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}


//---------------------------------
ErrorCodes_t MessageDispatcher::setCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibVcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibVcVoltageGainRange.prefix);
            selectedCalibVcVoltageGainVector[channelIndexes[i]] = gains[i];
            calibVcVoltageGainCoders[channelIndexes[i]]->encode(gains[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibVcVoltageOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

//    } else if (!areAllTheVectorElementsInRange(offsets, calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].getMin(), calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].getMax())) {
//        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
            selectedCalibVcVoltageOffsetVector[channelIndexes[i]] = offsets[i];
            calibVcVoltageOffsetCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(offsets[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibCcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

//    } else if (!areAllTheVectorElementsInRange(gains, calibCcCurrentGainRange.getMin(), calibCcCurrentGainRange.getMax())) {
//        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibCcCurrentGainRange.prefix);
            selectedCalibCcCurrentGainVector[channelIndexes[i]] = gains[i];
            calibCcCurrentGainCoders[channelIndexes[i]]->encode(gains[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibCcCurrentOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

//    } else if (!areAllTheVectorElementsInRange(offsets, calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].getMin(), calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].getMax())) {
//        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
            selectedCalibCcCurrentOffsetVector[channelIndexes[i]] = offsets[i];
            calibCcCurrentOffsetCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(offsets[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}


//---------------------------------

ErrorCodes_t MessageDispatcher::setGateVoltagesTuner(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag){
    if (gateVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(gateVoltages, gateVoltageRange.getMin(), gateVoltageRange.getMax())) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < boardIndexes.size(); i++){
            gateVoltages[i].convertValue(gateVoltageRange.prefix);
            selectedGateVoltageVector[boardIndexes[i]] = gateVoltages[i];
            gateVoltageCoders[boardIndexes[i]]->encode(gateVoltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setSourceVoltagesTuner(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag){
    if (sourceVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(sourceVoltages, sourceVoltageRange.getMin(), sourceVoltageRange.getMax())) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < boardIndexes.size(); i++){
            sourceVoltages[i].convertValue(sourceVoltageRange.prefix);
            selectedSourceVoltageVector[boardIndexes[i]] = sourceVoltages[i];
            sourceVoltageCoders[boardIndexes[i]]->encode(sourceVoltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn) {
    if (vcCurrentRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (currentRangeIdx >= vcCurrentRangesNum) {
        return ErrorValueOutOfRange;

    } else {
        vcCurrentRangeCoder->encode(currentRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedVcCurrentRangeIdx = currentRangeIdx;
        currentRange = vcCurrentRangesArray[selectedVcCurrentRangeIdx];
        currentResolution = currentRange.step;
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn) {
    if (vcVoltageRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (voltageRangeIdx >= vcVoltageRangesNum) {
        return ErrorValueOutOfRange;

    } else {
        vcVoltageRangeCoder->encode(voltageRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedVcVoltageRangeIdx = voltageRangeIdx;
        voltageRange = vcVoltageRangesArray[selectedVcVoltageRangeIdx];
        voltageResolution = voltageRange.step;
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn) {
    if (ccCurrentRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (currentRangeIdx >= ccCurrentRangesNum) {
        return ErrorValueOutOfRange;

    } else {
        ccCurrentRangeCoder->encode(currentRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedCcCurrentRangeIdx = currentRangeIdx;
        currentRange = ccCurrentRangesArray[selectedCcCurrentRangeIdx];
        currentResolution = currentRange.step;
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn) {
    if (ccVoltageRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (voltageRangeIdx >= ccVoltageRangesNum) {
        return ErrorValueOutOfRange;

    } else {
        ccVoltageRangeCoder->encode(voltageRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedCcVoltageRangeIdx = voltageRangeIdx;
        voltageRange = ccVoltageRangesArray[selectedCcVoltageRangeIdx];
        voltageResolution = voltageRange.step;
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (digitalOffsetCompensationCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            digitalOffsetCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);

        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlagIn){
    if (vcVoltageFilterCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (filterIdx >= vcVoltageFiltersNum) {
        return ErrorValueOutOfRange;

    } else {
        vcVoltageFilterCoder->encode(filterIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedVcVoltageFilterIdx = filterIdx;
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlagIn){
    if (ccCurrentFilterCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (filterIdx >= ccCurrentFiltersNum) {
        return ErrorValueOutOfRange;

    } else {
        ccCurrentFilterCoder->encode(filterIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedCcCurrentFilterIdx = filterIdx;
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (enableStimulusCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
           enableStimulusCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);

        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::turnChannelsOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (turnChannelsOnCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
           turnChannelsOnCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);

        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::turnCalSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (calSwCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
           calSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }
        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::turnVcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (vcSwCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
           vcSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }
        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::turnCcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (ccSwCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
           ccSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }
        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::turnVcCcSelOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (vcCcSelCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
           vcCcSelCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }
        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::enableCcStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (ccStimEnCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
           ccStimEnCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }
        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }

}

ErrorCodes_t MessageDispatcher::setSourceForVoltageChannel(uint16_t source, bool applyFlag){
    if (sourceForVoltageChannelCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    } else {
        sourceForVoltageChannelCoder->encode(source, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedSourceForVoltageChannelIdx = source;
        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setSourceForCurrentChannel(uint16_t source, bool applyFlag){
    if (sourceForCurrentChannelCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    } else {
        sourceForCurrentChannelCoder->encode(source, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedSourceForCurrentChannelIdx = source;
        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setAdcFilter(){
    // Still to be properly implemented
    if(amIinVoltageClamp){
        if (vcCurrentFilterCoder != nullptr) {
            vcCurrentFilterCoder->encode(sr2LpfVcCurrentMap[selectedSamplingRateIdx], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedVcCurrentFilterIdx = sr2LpfVcCurrentMap[selectedSamplingRateIdx];
        }
    }else{
        if (ccVoltageFilterCoder != nullptr) {
            ccVoltageFilterCoder->encode(sr2LpfCcVoltageMap[selectedSamplingRateIdx], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedCcVoltageFilterIdx = sr2LpfCcVoltageMap[selectedSamplingRateIdx];
        }
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setSamplingRate(uint16_t samplingRateIdx, bool applyFlagIn) {
    if (samplingRateCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (samplingRateIdx >= samplingRatesNum) {
        return ErrorValueOutOfRange;

    } else {
        samplingRateCoder->encode(samplingRateIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedSamplingRateIdx = samplingRateIdx;
        this->setAdcFilter();
        this->computeRawDataFilterCoefficients();
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setDownsamplingRatio(uint32_t ratio) {
    if (ratio == 0) {
        return ErrorValueOutOfRange;

    } else if (ratio == 1) {
        downsamplingFlag = false;

    } else {
        downsamplingFlag = true;

    }
    downsamplingRatio = ratio;
    this->computeRawDataFilterCoefficients();
    return Success;
}

ErrorCodes_t MessageDispatcher::setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status) {
    BoolCoder::CoderConfig_t boolConfig;
    boolConfig.initialWord = wordOffset;
    boolConfig.initialBit = bitOffset;
    boolConfig.bitsNum = 1;
    bitDebugCoder = new BoolArrayCoder(boolConfig);
    bitDebugCoder->encode(status ? 1 : 0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);
    delete bitDebugCoder;
    return Success;
}

ErrorCodes_t MessageDispatcher::setDebugWord(uint16_t wordOffset, uint16_t wordValue) {
    BoolCoder::CoderConfig_t boolConfig;
    boolConfig.initialWord = wordOffset;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    wordDebugCoder = new BoolArrayCoder(boolConfig);
    wordDebugCoder->encode(wordValue, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);
    delete wordDebugCoder;
    return Success;
}

ErrorCodes_t MessageDispatcher::turnVoltageReaderOn(bool onValueIn, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCurrentReaderOn(bool onValueIn, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnVoltageStimulusOn(bool onValue, bool applyFlag){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCurrentStimulusOn(bool onValue, bool applyFlag){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibParams(CalibrationParams_t calibParams){
    if(calibParams.allGainAdcMeas.size()>0 && calibrationParams.allGainAdcMeas.size()){
        calibrationParams.allGainAdcMeas = calibParams.allGainAdcMeas;
    }

    if(calibParams.allOffsetAdcMeas.size()>0 && calibrationParams.allOffsetAdcMeas.size()){
        calibrationParams.allOffsetAdcMeas = calibParams.allOffsetAdcMeas;
    }

    if(calibParams.allGainDacMeas.size()>0 && calibrationParams.allGainDacMeas.size()){
        calibrationParams.allGainDacMeas = calibParams.allGainDacMeas;
    }

    if(calibParams.allOffsetDacMeas.size()>0 && calibrationParams.allOffsetDacMeas.size()){
        calibrationParams.allOffsetDacMeas = calibParams.allOffsetDacMeas;
    }

    if(calibParams.ccAllGainAdcMeas.size()>0 && calibrationParams.ccAllGainAdcMeas.size()){
        calibrationParams.ccAllGainAdcMeas = calibParams.ccAllGainAdcMeas;
    }

    if(calibParams.ccAllOffsetAdcMeas.size()>0 && calibrationParams.ccAllOffsetAdcMeas.size()){
        calibrationParams.ccAllOffsetAdcMeas = calibParams.ccAllOffsetAdcMeas;
    }

    if(calibParams.ccAllGainDacMeas.size()>0 && calibrationParams.ccAllGainDacMeas.size()){
        calibrationParams.ccAllGainDacMeas = calibParams.ccAllGainDacMeas;
    }

    if(calibParams.ccAllOffsetDacMeas.size()>0 && calibrationParams.ccAllOffsetDacMeas.size()){
        calibrationParams.ccAllOffsetDacMeas = calibParams.ccAllOffsetDacMeas;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest) {
    if (voltageProtocolRestCoders.empty()) {
            return ErrorFeatureNotImplemented;

    } else if (itemsNum >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;

    } else {
        selectedProtocolItemsNum = itemsNum;
        UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
        protocolIdCoder->encode(protId, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemsNumberCoder->encode(itemsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolSweepsNumberCoder->encode(sweepsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        vRest.convertValue(voltagePrefix);
        voltageProtocolRestCoders[selectedVcVoltageRangeIdx]->encode(vRest.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step) {
    if (!voltageProtocolStepImplemented) {
            return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0Step) ||
               !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
           return ErrorValueOutOfRange;

    } else {
        UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
        UnitPfx_t timePrefix = protocolTimeRange.prefix;
        protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemTypeCoders[itemIdx]->encode(ProtocolItemStep, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        v0.convertValue(voltagePrefix);
        voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        v0Step.convertValue(voltagePrefix);
        voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0.convertValue(timePrefix);
        protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0Step.convertValue(timePrefix);
        protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step) {
    if (!voltageProtocolRampImplemented) {
            return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0Step) ||
               !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vFinal) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vFinalStep) ||
               !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
           return ErrorValueOutOfRange;

    } else {
        UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
        UnitPfx_t timePrefix = protocolTimeRange.prefix;
        protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemTypeCoders[itemIdx]->encode(ProtocolItemRamp, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        v0.convertValue(voltagePrefix);
        voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        v0Step.convertValue(voltagePrefix);
        voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        vFinal.convertValue(voltagePrefix);
        voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(vFinal.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        vFinalStep.convertValue(voltagePrefix);
        voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(vFinalStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0.convertValue(timePrefix);
        protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0Step.convertValue(timePrefix);
        protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step) {
    if (!voltageProtocolSinImplemented) {
            return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0Step) ||
               !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vAmp) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vAmpStep) ||
               !positiveProtocolFrequencyRange.includes(f0) || !protocolFrequencyRange.includes(f0Step)) {
           return ErrorValueOutOfRange;

    } else {
        UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
        UnitPfx_t freqPrefix = protocolFrequencyRange.prefix;
        protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemTypeCoders[itemIdx]->encode(ProtocolItemSin, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        v0.convertValue(voltagePrefix);
        voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        v0Step.convertValue(voltagePrefix);
        voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        vAmp.convertValue(voltagePrefix);
        voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(vAmp.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        vAmpStep.convertValue(voltagePrefix);
        voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(vAmpStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        f0.convertValue(freqPrefix);
        protocolFrequency0Coders[itemIdx]->encode(f0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        f0Step.convertValue(freqPrefix);
        protocolFrequency0StepCoders[itemIdx]->encode(f0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest) {
    if (currentProtocolRestCoders.empty()) {
            return ErrorFeatureNotImplemented;

    } else if (itemsNum >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;

    } else {
        selectedProtocolItemsNum = itemsNum;
        UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
        protocolIdCoder->encode(protId, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemsNumberCoder->encode(itemsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolSweepsNumberCoder->encode(sweepsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        iRest.convertValue(currentPrefix);
        currentProtocolRestCoders[selectedCcCurrentRangeIdx]->encode(iRest.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step) {
    if (!currentProtocolStepImplemented) {
            return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0Step) ||
               !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
           return ErrorValueOutOfRange;

    } else {
        UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
        UnitPfx_t timePrefix = protocolTimeRange.prefix;
        protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemTypeCoders[itemIdx]->encode(ProtocolItemStep, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        i0.convertValue(currentPrefix);
        currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        i0Step.convertValue(currentPrefix);
        currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0.convertValue(timePrefix);
        protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0Step.convertValue(timePrefix);
        protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step) {
    if (!currentProtocolRampImplemented) {
            return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0Step) ||
               !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iFinal) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iFinalStep) ||
               !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
           return ErrorValueOutOfRange;

    } else {
        UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
        UnitPfx_t timePrefix = protocolTimeRange.prefix;
        protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemTypeCoders[itemIdx]->encode(ProtocolItemRamp, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        i0.convertValue(currentPrefix);
        currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        i0Step.convertValue(currentPrefix);
        currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        iFinal.convertValue(currentPrefix);
        currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(iFinal.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        iFinalStep.convertValue(currentPrefix);
        currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(iFinalStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0.convertValue(timePrefix);
        protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        t0Step.convertValue(timePrefix);
        protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step) {
    if (!currentProtocolSinImplemented) {
        return ErrorFeatureNotImplemented;
    } else if (itemIdx >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0Step) ||
               !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iAmp) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iAmpStep) ||
               !positiveProtocolFrequencyRange.includes(f0) || !protocolFrequencyRange.includes(f0Step)) {
        return ErrorValueOutOfRange;


    } else {
        UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
        UnitPfx_t freqPrefix = protocolFrequencyRange.prefix;
        protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        protocolItemTypeCoders[itemIdx]->encode(ProtocolItemSin, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        i0.convertValue(currentPrefix);
        currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        i0Step.convertValue(currentPrefix);
        currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        iAmp.convertValue(currentPrefix);
        currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(iAmp.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        iAmpStep.convertValue(currentPrefix);
        currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(iAmpStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        f0.convertValue(freqPrefix);
        protocolFrequency0Coders[itemIdx]->encode(f0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        f0Step.convertValue(freqPrefix);
        protocolFrequency0StepCoders[itemIdx]->encode(f0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::startProtocol() {
    this->stackOutgoingMessage(txStatus, TxTriggerStartProtocol);
    return Success;
}

/*****************\
 *  State Array  *
\*****************/
ErrorCodes_t MessageDispatcher::setStateArrayStructure(int numberOfStates, int initialState){
    if (numberOfStatesCoder == nullptr ) {
        return ErrorFeatureNotImplemented;
    }
    if (numberOfStates > stateMaxNum || initialState >= numberOfStates){
        return ErrorValueOutOfRange;
    }
    numberOfStatesCoder->encode(numberOfStates, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    initialStateCoder->encode(initialState, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

ErrorCodes_t MessageDispatcher::setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, double timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag){
    if (appliedVoltageCoders.empty()){
        return ErrorFeatureNotImplemented;
    }
    voltage.convertValue(vHoldRange[selectedVcVoltageRangeIdx].prefix);
    appliedVoltageCoders[selectedVcVoltageRangeIdx][stateIdx]->encode(voltage.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTimeoutFlagCoders[stateIdx]->encode(timeoutStateFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTriggerFlagCoders[stateIdx]->encode(triggerFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTriggerDeltaFlagCoders[stateIdx]->encode(deltaFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTimeoutValueCoders[stateIdx]->encode(timeout, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTimeoutNextStateCoders[stateIdx]->encode(timeoutState, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    minTriggerValue.convertValue(vcCurrentRangesArray[selectedVcCurrentRangeIdx].prefix);
    maxTriggerValue.convertValue(vcCurrentRangesArray[selectedVcCurrentRangeIdx].prefix);
    stateMinTriggerCurrentCoders[selectedVcCurrentRangeIdx][stateIdx]->encode(minTriggerValue.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateMaxTriggerCurrentCoders[selectedVcCurrentRangeIdx][stateIdx]->encode(maxTriggerValue.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTriggerNextStateCoders[stateIdx]->encode(triggerState, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

ErrorCodes_t MessageDispatcher::startStateArray(){
    this->stackOutgoingMessage(txStatus, TxTriggerStartStateArray);
    return Success;
}

ErrorCodes_t MessageDispatcher::setStateArrayEnabled(int chIdx, bool enabledFlag){
    if (enableStateArrayChannelsCoder.empty()){
        return ErrorFeatureNotImplemented;
    }
    enableStateArrayChannelsCoder[chIdx]->encode(enabledFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

bool MessageDispatcher::isStateArrayAvailable(){
    if (numberOfStatesCoder == nullptr){
        return false;
    }
    return true;
}

/****************\
 *  Rx methods  *
\****************/

ErrorCodes_t MessageDispatcher::getRxDataBufferSize(uint32_t &size) {
    size = E384CL_OUT_STRUCT_DATA_LEN;
    return Success;
}

ErrorCodes_t MessageDispatcher::allocateRxDataBuffer(int16_t * &data) {
    ErrorCodes_t ret = Success;

    data = new (std::nothrow) int16_t[E384CL_OUT_STRUCT_DATA_LEN];
    if (data == nullptr) {
        ret = ErrorMemoryInitialization;
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::deallocateRxDataBuffer(int16_t * &data) {
    if (data != nullptr) {
        delete [] data;
        data = nullptr;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getNextMessage(RxOutput_t &rxOutput, int16_t * data) {
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

                if (downsamplingFlag) {
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    uint32_t downsamplingCount = 0;
                    for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                        if (++downsamplingOffset == downsamplingRatio) {
                            downsamplingOffset = 0;
                            downsamplingCount++;
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

                        } else {
                            for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
                                xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }

                            for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
                                xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }
                        }

                        if (iirOff < 1) {
                            iirOff = IIR_ORD;

                        } else {
                            iirOff--;
                        }
                    }

                    rxOutput.dataLen += downsamplingCount*totalChannelsNum;

                } else if (rawDataFilterActiveFlag) {
                    rxOutput.dataLen += samplesNum;
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
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
                    }

                } else {
                    rxOutput.dataLen += samplesNum;
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            data[dataWritten+sampleIdx++] = rxDataBuffer[dataOffset];
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            data[dataWritten+sampleIdx++] = rxDataBuffer[dataOffset];
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }
                    }
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
    rxMsgBufferReadLength -= msgReadCount;
    rxMsgBufferNotFull.notify_all();
    rxMutexLock.unlock();

    return ret;
}

ErrorCodes_t MessageDispatcher::convertVoltageValue(int16_t intValue, double &fltValue) {
    fltValue = voltageResolution*(double)intValue;

    return Success;
}

ErrorCodes_t MessageDispatcher::convertCurrentValue(int16_t intValue, double &fltValue) {
    fltValue = currentResolution*(double)intValue;

    return Success;
}

ErrorCodes_t MessageDispatcher::convertVoltageValues(int16_t * intValues, double * fltValues, int valuesNum) {
    for (int idx = 0; idx < valuesNum; idx++) {
        fltValues[idx] = voltageResolution*(double)intValues[idx];
    }

    return Success;
}

ErrorCodes_t MessageDispatcher::convertCurrentValues(int16_t * intValues, double * fltValues, int valuesNum) {
    for (int idx = 0; idx < valuesNum; idx++) {
        fltValues[idx] = currentResolution*(double)intValues[idx];
    }

    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures){
    if (vHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        voltageHoldTunerFeatures = vHoldRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures){
    if (cHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        currentHoldTunerFeatures = cHoldRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCalibVcCurrentGainFeatures(RangedMeasurement_t &calibVcCurrentGainFeatures){
    if (calibVcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        calibVcCurrentGainFeatures = calibVcCurrentGainRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCalibVcCurrentOffsetFeatures(std::vector<RangedMeasurement_t> &calibVcCurrentOffsetFeatures){
    if (calibVcCurrentOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        calibVcCurrentOffsetFeatures = calibVcCurrentOffsetRanges;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCalibCcVoltageGainFeatures(RangedMeasurement_t &calibCcVoltageGainFeatures){
    if (calibCcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        calibCcVoltageGainFeatures = calibCcVoltageGainRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCalibCcVoltageOffsetFeatures(std::vector<RangedMeasurement_t> &calibCcVoltageOffsetFeatures){
    if (calibCcVoltageOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        calibCcVoltageOffsetFeatures = calibCcVoltageOffsetRanges;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getGateVoltagesTunerFeatures(RangedMeasurement_t &gateVoltagesTunerFeatures){
    if (gateVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        gateVoltagesTunerFeatures = gateVoltageRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getSourceVoltagesTunerFeatures(RangedMeasurement_t &sourceVoltagesTunerFeatures){
    if (sourceVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        sourceVoltagesTunerFeatures = sourceVoltageRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getChannelNumberFeatures(uint16_t &voltageChannelNumberFeatures, uint16_t &CurrentChannelNumberFeatures){
    voltageChannelNumberFeatures = voltageChannelsNum;
    CurrentChannelNumberFeatures = currentChannelsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getAvailableChannelsSourcesFeatures(ChannelSources_t &voltageSourcesIdxs, ChannelSources_t &currentSourcesIdxs) {
    voltageSourcesIdxs = availableVoltageSourcesIdxs;
    currentSourcesIdxs = availableCurrentSourcesIdxs;
    return Success;
}

ErrorCodes_t MessageDispatcher::getBoardsNumberFeatures(uint16_t &boardsNumberFeatures) {
    boardsNumberFeatures = totalBoardsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getClampingModalitiesFeatures(std::vector <uint16_t> &clampingModalitiesFeatures) {
    if (clampingModalitiesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        clampingModalitiesFeatures = clampingModalitiesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges, uint16_t &defaultVcCurrRangeIdx) {
    if (vcCurrentRangesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        currentRanges = vcCurrentRangesArray;
        defaultVcCurrRangeIdx = defaultVcCurrentRangeIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges) {
    if(vcVoltageRangesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        voltageRanges = vcVoltageRangesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges) {
    if(ccCurrentRangesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        currentRanges = ccCurrentRangesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges) {
    if(ccVoltageRangesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        voltageRanges = ccVoltageRangesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCCurrentRange(RangedMeasurement_t &range) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        range = vcCurrentRangesArray[selectedVcCurrentRangeIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCVoltageRange(RangedMeasurement_t &range) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        range = vcVoltageRangesArray[selectedVcVoltageRangeIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCCurrentRange(RangedMeasurement_t &range) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        range = ccCurrentRangesArray[selectedVcCurrentRangeIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCVoltageRange(RangedMeasurement_t &range) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        range = ccVoltageRangesArray[selectedCcVoltageRangeIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getSamplingRatesFeatures(std::vector <Measurement_t> &samplingRates) {
    if(samplingRatesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        samplingRates = samplingRatesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getRealSamplingRatesFeatures(std::vector <Measurement_t> &realSamplingRates) {
    if(realSamplingRatesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        realSamplingRates = realSamplingRatesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getDownsamplingRatiosFeatures(std::vector <uint32_t> &downsamplingRatios) {
    downsamplingRatios = {10, 20, 50, 100, 200, 500, 1000};
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageStimulusLpfs(std::vector <Measurement_t> &vcVoltageFilters){
    if(vcVoltageFiltersArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        vcVoltageFilters = vcVoltageFiltersArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCurrentStimulusLpfs(std::vector <Measurement_t> &ccCurrentFilters){
    if(ccCurrentFiltersArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        ccCurrentFilters = ccCurrentFiltersArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCalibParams(CalibrationParams_t &calibParams){
    if(calibrationParams.allGainAdcMeas.size()>0){
        calibParams.allGainAdcMeas = calibrationParams.allGainAdcMeas;
    }

    if(calibrationParams.allOffsetAdcMeas.size()>0){
        calibParams.allOffsetAdcMeas = calibrationParams.allOffsetAdcMeas;
    }

    if(calibrationParams.allGainDacMeas.size()>0){
        calibParams.allGainDacMeas = calibrationParams.allGainDacMeas;
    }

    if(calibrationParams.allOffsetDacMeas.size()>0){
        calibParams.allOffsetDacMeas = calibrationParams.allOffsetDacMeas;
    }

    if(calibrationParams.ccAllGainAdcMeas.size()>0){
        calibParams.ccAllGainAdcMeas = calibrationParams.ccAllGainAdcMeas;
    }

    if(calibrationParams.ccAllOffsetAdcMeas.size()>0){
        calibParams.ccAllOffsetAdcMeas = calibrationParams.ccAllOffsetAdcMeas;
    }

    if(calibrationParams.ccAllGainDacMeas.size()>0){
        calibParams.ccAllGainDacMeas = calibrationParams.ccAllGainDacMeas;
    }

    if(calibrationParams.ccAllOffsetDacMeas.size()>0){
        calibParams.ccAllOffsetDacMeas = calibrationParams.ccAllOffsetDacMeas;
    }
    return calibrationLoadingError;
}

ErrorCodes_t MessageDispatcher::getCalibFileNames(std::vector<std::string> &calibFileNames){
    calibFileNames = this->calibrationFileNames;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibFilesFlags(std::vector<std::vector <bool>> &calibFilesFlags) {
    calibFilesFlags = calibrationFilesOkFlags;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibMappingFileDir(std::string &dir){
    dir = this->calibrationMappingFileDir;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibMappingFilePath(std::string &path){
    path = this->calibrationMappingFilePath;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range) {
    if (vHoldRange.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        range = vHoldRange[rangeIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCurrentProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range) {
    if (cHoldRange.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        range = cHoldRange[rangeIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getTimeProtocolRangeFeature(RangedMeasurement_t &range) {
    range = protocolTimeRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getFrequencyProtocolRangeFeature(RangedMeasurement_t &range) {
    range = protocolFrequencyRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxProtocolItemsFeature(uint32_t &num) {
    if (protocolMaxItemsNum < 1) {
        return ErrorFeatureNotImplemented;

    } else {
        num = protocolMaxItemsNum;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::hasProtocolStepFeature() {
    if (voltageProtocolStepImplemented || currentProtocolStepImplemented) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::hasProtocolRampFeature() {
    if (voltageProtocolRampImplemented || currentProtocolRampImplemented) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::hasProtocolSinFeature() {
    if (voltageProtocolSinImplemented || currentProtocolSinImplemented) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}


ErrorCodes_t MessageDispatcher::getCalibData(CalibrationData_t &calibData){
    if(calibrationData.vcCalibResArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        calibData = calibrationData;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCalibDefaultVcAdcGain(Measurement_t &defaultVcAdcGain){
    defaultVcAdcGain = defaultCalibVcCurrentGain;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibDefaultVcAdcOffset(Measurement_t &defaultVcAdcOffset){
    defaultVcAdcOffset =defaultCalibVcCurrentOffset;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibDefaultVcDacGain(Measurement_t &defaultVcDacGain){
    defaultVcDacGain = defaultCalibVcVoltageGain;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibDefaultVcDacOffset(Measurement_t &defaultVcDacOffset){
    defaultVcDacOffset = defaultCalibVcVoltageOffset;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibDefaultCcAdcGain(Measurement_t &defaultCcAdcGain){
    defaultCcAdcGain = defaultCalibCcVoltageGain;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibDefaultCcAdcOffset(Measurement_t &defaultCcAdcOffset){
    defaultCcAdcOffset = defaultCalibCcVoltageOffset;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibDefaultCcDacGain(Measurement_t &defaultCcDacGain){
    defaultCcDacGain = defaultCalibCcCurrentGain;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibDefaultCcDacOffset(Measurement_t &defaultCcDacOffset){
    defaultCcDacOffset = defaultCalibCcCurrentOffset;
    return Success;
}

/*********************\
 *  Private methods  *
\*********************/

ErrorCodes_t MessageDispatcher::init() {
    rxMsgBuffer = new (std::nothrow) MsgResume_t[RX_MSG_BUFFER_SIZE];
    if (rxMsgBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    rxDataBuffer = new (std::nothrow) uint16_t[RX_DATA_BUFFER_SIZE+1]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word */
    if (rxDataBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    txMsgBuffer = new (std::nothrow) std::vector <uint16_t>[TX_MSG_BUFFER_SIZE];
    if (txMsgBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    txMsgOffsetWord.resize(TX_MSG_BUFFER_SIZE);
    txMsgLength.resize(TX_MSG_BUFFER_SIZE);
    txMsgTrigger.resize(TX_MSG_BUFFER_SIZE);

    /*! Allocate memory for raw data filters */
    this->initializeRawDataFilterVariables();

    /*! Allocate memory for compensations */
//    this->initializeCompensations(); /*! \todo FCON */

    /*! Allocate memory for voltage values for devices that send only data current in standard data frames */
    voltageDataValues.resize(voltageChannelsNum);
    std::fill(voltageDataValues.begin(), voltageDataValues.end(), 0);

    return Success;
}

ErrorCodes_t MessageDispatcher::deinit() {
    if (rxMsgBuffer != nullptr) {
        delete [] rxMsgBuffer;
        rxMsgBuffer = nullptr;
    }

    if (rxDataBuffer != nullptr) {
        delete [] rxDataBuffer;
        rxDataBuffer = nullptr;
    }

    if (txMsgBuffer != nullptr) {
        delete [] txMsgBuffer;
        txMsgBuffer = nullptr;
    }

    if (iirX != nullptr) {
        for (unsigned int channelIdx = 0; channelIdx < totalChannelsNum; channelIdx++) {
            delete [] iirX[channelIdx];
        }
        delete [] iirX;
        iirX = nullptr;
    }

    if (iirY != nullptr) {
        for (unsigned int channelIdx = 0; channelIdx < totalChannelsNum; channelIdx++) {
            delete [] iirY[channelIdx];
        }
        delete [] iirY;
        iirY = nullptr;
    }

#ifdef DEBUG_TX_DATA_PRINT
    fclose(txFid);
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
    fclose(rxRawFid);
#endif

#ifdef DEBUG_RX_PROCESSING_PRINT
    fclose(rxProcFid);
#endif

#ifdef DEBUG_RX_DATA_PRINT
    fclose(rxFid);
#endif

    return Success;
}

/*! \todo FCON questi due metodi dovrebbero cercare dispositivi con tutte le librerie di interfacciamento con device implementate (per ora c'è solo il front panel della opal kelly) */
std::string MessageDispatcher::getDeviceSerial(int index) {
    std::string serial;
    int numDevs;
    getDeviceCount(numDevs);
    if (index < numDevs) {
        okCFrontPanel okDev;
        okDev.GetDeviceCount();
        serial = okDev.GetDeviceListSerial(index);
        return serial;

    } else {
        return "";
    }
}

bool MessageDispatcher::getDeviceCount(int &numDevs) {
    okCFrontPanel okDev;
    numDevs = okDev.GetDeviceCount();
    return true;
}

bool MessageDispatcher::checkProtocolValidity(std::string &message) {
    /*! \todo FCON da riempire */
    return false;
}

void MessageDispatcher::storeFrameData(uint16_t rxMsgTypeId, RxMessageTypes_t rxMessageType) {
    uint32_t rxDataWords = rxWordLengths[rxMessageType];

#ifdef DEBUG_RX_PROCESSING_PRINT
            fprintf(rxProcFid, "Store data frame: %x\n", rxMessageType);
            fflush(rxProcFid);
#endif

    rxMsgBuffer[rxMsgBufferWriteOffset].typeId = rxMsgTypeId;
    rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = rxDataWords;
    rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;

    if (rxMessageType == RxMessageCurrentDataLoad) {
        /*! Data frame with only current */
        uint32_t packetsNum = rxDataWords/currentChannelsNum;
        uint32_t rxDataBufferWriteIdx = 0;

#ifdef DEBUG_RX_PROCESSING_PRINT
            fprintf(rxProcFid, "rxDataWords: %d\n", rxDataWords);
            fprintf(rxProcFid, "packetsNum: %d\n", packetsNum);
            fflush(rxProcFid);
#endif

        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! For each packet retrieve the last recevied voltage values */
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++) & RX_DATA_BUFFER_MASK] = voltageDataValues[idx];
            }

            /*! The store the new current values */
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++) & RX_DATA_BUFFER_MASK] = this->popUint16FromRxRawBuffer();
            }
        }

        /* The size of the data returned by the message dispatcher is different from the size of the fram returned by the FPGA */
        rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = rxDataBufferWriteIdx;

    } else if (rxMessageType == RxMessageVoltageDataLoad) {
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            voltageDataValues[idx] = this->popUint16FromRxRawBuffer();
        }

    } else {
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx) & RX_DATA_BUFFER_MASK] = this->popUint16FromRxRawBuffer();
        }
    }

    rxDataBufferWriteOffset = (rxDataBufferWriteOffset+rxMsgBuffer[rxMsgBufferWriteOffset].dataLength) & RX_DATA_BUFFER_MASK;

    if (rxDataBufferWriteOffset <= rxMsgBuffer[rxMsgBufferWriteOffset].dataLength) {
        rxDataBuffer[RX_DATA_BUFFER_SIZE] = rxDataBuffer[0]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word,
                                                              *   even if the first 16bit word is in position FTD_RX_DATA_BUFFER_SIZE-1 and the following one would go out of range otherwise */
    }

    if (rxEnabledTypesMap[rxMsgTypeId]) {
        /*! Update the message buffer only if the message is not filtered out */
        rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1) & RX_MSG_BUFFER_MASK;
    }

    if (rxEnabledTypesMap[rxMsgTypeId]) {
        /*! change the message buffer length only if the message is not filtered out */
        std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
    }
}

void MessageDispatcher::stackOutgoingMessage(std::vector <uint16_t> &txDataMessage, TxTriggerType_t triggerType) {
    if (txModifiedEndingWord > txModifiedStartingWord) {
        std::unique_lock <std::mutex> txMutexLock (txMutex);
        while (txMsgBufferReadLength >= TX_MSG_BUFFER_SIZE) {
            txMsgBufferNotFull.wait_for(txMutexLock, std::chrono::milliseconds(100));
        }

        /*! The next 2 lines ensure that words are written in blocks of 32 bits in case any device libraries require it */
        txModifiedStartingWord = (txModifiedStartingWord/2)*2; /*! Round to the biggest smaller even number */
        txModifiedEndingWord = ((txModifiedEndingWord+1)/2)*2; /*! +1 : if odd, makes the number even and /2 followed by *2 doesn't change a thing. If even, makes the number odd and /2 truncates to the smaller integer and *2 drives you to the correct ending word. */

        txMsgBuffer[txMsgBufferWriteOffset] = {txDataMessage.begin()+txModifiedStartingWord, txDataMessage.begin()+txModifiedEndingWord};
        txMsgOffsetWord[txMsgBufferWriteOffset] = txModifiedStartingWord;
        txMsgLength[txMsgBufferWriteOffset] = txModifiedEndingWord-txModifiedStartingWord;
        txMsgTrigger[txMsgBufferWriteOffset] = triggerType;

        txModifiedStartingWord = txDataWords;
        txModifiedEndingWord = 0;

        txMsgBufferWriteOffset = (txMsgBufferWriteOffset+1)&TX_MSG_BUFFER_MASK;
        txMsgBufferReadLength++;

        txMsgBufferNotEmpty.notify_all();
    }
}

uint16_t MessageDispatcher::popUint16FromRxRawBuffer() {
    uint16_t value = (rxRawBuffer[rxRawBufferReadOffset] << 8) + rxRawBuffer[rxRawBufferReadOffset+1];
    rxRawBufferReadOffset = (rxRawBufferReadOffset+RX_WORD_SIZE) & rxRawBufferMask;
    rxRawBytesAvailable -= RX_WORD_SIZE;
    return value;
}

uint16_t MessageDispatcher::readUint16FromRxRawBuffer(uint32_t n) {
    uint16_t value = (rxRawBuffer[(rxRawBufferReadOffset+n) & rxRawBufferMask] << 8) + rxRawBuffer[(rxRawBufferReadOffset+n+1) & rxRawBufferMask];
    return value;
}

void MessageDispatcher::initializeRawDataFilterVariables() {
    iirX = new double * [totalChannelsNum];
    iirY = new double * [totalChannelsNum];
    for (unsigned int channelIdx = 0; channelIdx < totalChannelsNum; channelIdx++) {
        iirX[channelIdx] = new double[IIR_ORD+1];
        iirY[channelIdx] = new double[IIR_ORD+1];
        for (int tapIdx = 0; tapIdx < IIR_ORD+1; tapIdx++) {
            iirX[channelIdx][tapIdx] = 0.0;
            iirY[channelIdx][tapIdx] = 0.0;
        }
    }
}

void MessageDispatcher::computeRawDataFilterCoefficients() {
    bool enableFilter;
    double cutoffFrequency;

    if (downsamplingFlag) {
        rawDataFilterCutoffFrequencyOverride.convertValue(UnitPfxNone);
        rawDataFilterCutoffFrequencyOverride.value = samplingRate.getNoPrefixValue()*0.25;

    } else {
        rawDataFilterCutoffFrequencyOverride.convertValue(UnitPfxTera);
        rawDataFilterCutoffFrequencyOverride.value = 1.0e9;
    }
    rawDataFilterCutoffFrequency.convertValue(1.0/integrationStep.multiplier());
    rawDataFilterCutoffFrequencyOverride.convertValue(1.0/integrationStep.multiplier());

    if (rawDataFilterActiveFlag) {
        if (downsamplingFlag) {
            if (rawDataFilterCutoffFrequency < rawDataFilterCutoffFrequencyOverride) {
                if (rawDataFilterCutoffFrequency < samplingRate*0.5) {
                    cutoffFrequency = rawDataFilterCutoffFrequency.value;
                    enableFilter = true;

                } else {
                    enableFilter = false;
                }

            } else {
                cutoffFrequency = rawDataFilterCutoffFrequencyOverride.value;
                enableFilter = true;
            }

        } else {
            if (rawDataFilterCutoffFrequency < samplingRate*0.5) {
                cutoffFrequency = rawDataFilterCutoffFrequency.value;
                enableFilter = true;

            } else {
                enableFilter = false;
            }
        }

    } else {
        if (downsamplingFlag) {
            cutoffFrequency = rawDataFilterCutoffFrequencyOverride.value;
            enableFilter = true;

        } else {
            enableFilter = false;
        }
    }

    if (enableFilter) {
        if (rawDataFilterVoltageFlag) {
            double k1 = tan(M_PI*rawDataFilterCutoffFrequency.value*integrationStep.value); /*!< pre-warp coefficient */
            double k12 = k1*k1;
            double k2 = -2+2*k12; /*!< frequently used expression */
            double d = 1.0/(1.0+k1*IIR_2_SIN_PI_4+k12); /*!< denominator */

            /*! Denominators */
            iirVDen[0] = 1.0;
            iirVDen[1] = k2*d;
            iirVDen[2] = (-1.0-k2+k12*k12+k12*IIR_2_COS_PI_4_2)*d*d;

            /*! Gains and numerators */
            double iirG;
            if (rawDataFilterLowPassFlag) {
                iirG = (1.0+iirVDen[1]+iirVDen[2])*0.25;

                iirVNum[1] = 2.0*iirG;

            } else {
                iirG = (1.0-iirVDen[1]+iirVDen[2])*0.25;

                iirVNum[1] = -2.0*iirG;
            }

            iirVNum[0] = iirG;
            iirVNum[2] = iirG;

        } else {
            /*! Voltage is not filtered */
            iirVNum[0] = 1.0;
            iirVNum[1] = 0.0;
            iirVNum[2] = 0.0;
            iirVDen[0] = 1.0;
            iirVDen[1] = 0.0;
            iirVDen[2] = 0.0;
        }

        if (rawDataFilterCurrentFlag) {
            double k1 = tan(M_PI*rawDataFilterCutoffFrequency.value*integrationStep.value); /*!< pre-warp coefficient */
            double k12 = k1*k1;
            double k2 = -2+2*k12; /*!< frequently used expression */
            double d = 1.0/(1.0+k1*IIR_2_SIN_PI_4+k12); /*!< denominator */

            /*! Denominators */
            iirIDen[0] = 1.0;
            iirIDen[1] = k2*d;
            iirIDen[2] = (-1.0-k2+k12*k12+k12*IIR_2_COS_PI_4_2)*d*d;

            /*! Gains and numerators */
            double iirG;
            if (rawDataFilterLowPassFlag) {
                iirG = (1.0+iirIDen[1]+iirIDen[2])*0.25;

                iirINum[1] = 2.0*iirG;

            } else {
                iirG = (1.0-iirIDen[1]+iirIDen[2])*0.25;

                iirINum[1] = -2.0*iirG;
            }

            iirINum[0] = iirG;
            iirINum[2] = iirG;

        } else {
            /*! Current is not filtered */
            iirINum[0] = 1.0;
            iirINum[1] = 0.0;
            iirINum[2] = 0.0;
            iirIDen[0] = 1.0;
            iirIDen[1] = 0.0;
            iirIDen[2] = 0.0;
        }

    } else {
        /*! Delta impulse response with no autoregressive part */
        iirVNum[0] = 1.0;
        iirVNum[1] = 0.0;
        iirVNum[2] = 0.0;
        iirVDen[0] = 1.0;
        iirVDen[1] = 0.0;
        iirVDen[2] = 0.0;

        iirINum[0] = 1.0;
        iirINum[1] = 0.0;
        iirINum[2] = 0.0;
        iirIDen[0] = 1.0;
        iirIDen[1] = 0.0;
        iirIDen[2] = 0.0;
    }

    /*! reset FIFOs */
    for (uint16_t channelIdx = 0; channelIdx < totalChannelsNum; channelIdx++) {
        for (int tapIdx = 0; tapIdx < IIR_ORD+1; tapIdx++) {
            iirX[channelIdx][tapIdx] = 0.0;
            iirY[channelIdx][tapIdx] = 0.0;
        }
    }
}

double MessageDispatcher::applyRawDataFilter(uint16_t channelIdx, double x, double * iirNum, double * iirDen) {
    /*! 2nd order Butterworth filter */
    int tapIdx;

    int coeffIdx = 0;
    iirX[channelIdx][iirOff] = x;
    double y = x*iirNum[coeffIdx++];

    for (tapIdx = iirOff+1; tapIdx <= IIR_ORD; tapIdx++) {
        y += iirX[channelIdx][tapIdx]*iirNum[coeffIdx]-iirY[channelIdx][tapIdx]*iirDen[coeffIdx];
        coeffIdx++;
    }

    for (tapIdx = 0; tapIdx < iirOff; tapIdx++) {
        y += iirX[channelIdx][tapIdx]*iirNum[coeffIdx]-iirY[channelIdx][tapIdx]*iirDen[coeffIdx];
        coeffIdx++;
    }

    iirY[channelIdx][iirOff] = y;
    return y;
}

ErrorCodes_t MessageDispatcher::turnResistanceCompensationOn(std::vector<uint16_t> channelIndexes,std::vector<bool> onValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnLeakConductanceCompensationOn(std::vector<uint16_t> channelIndexes,std::vector<bool> onValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnBridgeBalanceCompensationOn(std::vector<uint16_t> channelIndexes,std::vector<bool> onValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableCompensation(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> onValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableVcCompensations(bool enable){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableCcCompensations(bool enable){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCompValues(std::vector<uint16_t> channelIndexes, CompensationUserParams paramToUpdate, std::vector<double> newParamValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCompOptions(std::vector<uint16_t> channelIndexes, CompensationTypes type, std::vector<uint16_t> options, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setPipetteCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setMembraneCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCCPipetteCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setLeakConductanceCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setBridgeBalanceCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCorrectionLag(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionPercentage(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionBandwidthGain(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionTau(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setLeakConductance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setBridgeBalanceResistance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn){
    return ErrorFeatureNotImplemented;
}



/*! \todo FCON recheck*/
ErrorCodes_t MessageDispatcher::getCompFeatures(uint16_t paramToExtractFeatures, std::vector<RangedMeasurement_t> &compensationFeatures, double &defaultParamValue){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompOptionsFeatures(CompensationTypes type ,std::vector <std::string> &compOptionsArray){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompValueMatrix(std::vector<std::vector<double>> &compValueMatrix){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompensationEnables(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> &onValues){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getPipetteCompensationOptions(std::vector <std::string> options){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCCPipetteCompensationOptions(std::vector <std::string> options){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getMembraneCompensationOptions(std::vector <std::string> options){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCompensationOptions(std::vector <std::string> options){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionOptions(std::vector <std::string> options){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductanceCompensationOptions(std::vector <std::string> options){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceCompensationOptions(std::vector <std::string> options){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getPipetteCapacitanceControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCCPipetteCapacitanceControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getMembraneCapacitanceControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistanceControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCorrectionPercentageControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCorrectionLagControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionGainControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionPercentageControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionBandwidthGainControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionTauControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductanceControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceResistanceControl(CompensationControl_t &control){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistanceCorrectionLag(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistancePredictionPercentage(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistancePredictionBandwidthGain(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistancePredictionTau(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceResistance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

std::vector<double> MessageDispatcher::user2AsicDomainTransform(int chIdx, std::vector<double> userDomainParams){
    return std::vector<double>();
}

std::vector<double> MessageDispatcher::asic2UserDomainTransform(int chIdx, std::vector<double> asicDomainParams, double oldUCpVc, double oldUCpCc){
    return std::vector<double>();
}

ErrorCodes_t MessageDispatcher::asic2UserDomainCompensable(int chIdx, std::vector<double> asicDomainParams, std::vector<double> userDomainParams){
    return ErrorFeatureNotImplemented;
}

double MessageDispatcher::computeAsicCmCinj(double cm, bool chanCslowEnable, MultiCoder::MultiCoderConfig_t multiconfigCslow){
    return -DBL_MAX;
}
