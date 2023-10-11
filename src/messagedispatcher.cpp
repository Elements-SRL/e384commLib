#include "messagedispatcher.h"

#include <iostream>
#include <ctime>
#include <thread>
#include <math.h>
#include <random>
#include <algorithm>

#include "okFrontPanelDLL.h"

#include "messagedispatcher_384nanopores.h"
#include "messagedispatcher_384nanopores_sr7p5khz_v01.h"
#include "messagedispatcher_384patchclamp.h"
#include "messagedispatcher_4x10mhz.h"
#include "messagedispatcher_2x10mhz.h"
#ifdef DEBUG
/*! Fake device that generates synthetic data */
#include "messagedispatcher_384fakenanopores.h"
#include "messagedispatcher_384fakepatchclamp.h"
#include "messagedispatcher_4x10mhzfake.h"
#include "messagedispatcher_2x10mhzfake.h"
#endif
#include "calibrationmanager.h"
#include "utils.h"

static std::unordered_map <std::string, DeviceTypes_t> deviceIdMapping = {
    {"221000107S", Device384Nanopores_SR7p5kHz},
    {"221000108T", Device384Nanopores_SR7p5kHz},
    {"22510013B4", Device384Nanopores},
    {"2210001076", Device384PatchClamp},
    {"221000106B", Device384PatchClamp},
    {"221000106C", Device384PatchClamp},
    {"22370012CI", Device4x10MHz_PCBV01},
    {"22370012CB", Device2x10MHz_PCBV02},
    {"224800131L", Device2x10MHz_PCBV02},
    {"224800130Y", Device2x10MHz_PCBV02},
    {"224800130X", Device4x10MHz_PCBV01}
#ifdef DEBUG
    ,{"FAKE_Nanopores", Device384Fake},
    {"FAKE_PATCH_CLAMP", Device384FakePatchClamp},
    {"FAKE_4x10MHz", Device4x10MHzFake},
    {"FAKE_2x10MHz", Device2x10MHzFake}
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
    numDevs++;
    deviceIds.push_back("FAKE_2x10MHz");
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

ErrorCodes_t MessageDispatcher::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath) {
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

    case Device384Nanopores_SR7p5kHz:
        messageDispatcher = new MessageDispatcher_384NanoPores_SR7p5kHz_V01(deviceId);
        break;

    case Device384PatchClamp:
        messageDispatcher = new MessageDispatcher_384PatchClamp_V01(deviceId);
        break;

    case Device2x10MHz_PCBV01:
        messageDispatcher = new MessageDispatcher_2x10MHz_PCBV01_V02(deviceId);
        break;

    case Device2x10MHz_PCBV02:
        messageDispatcher = new MessageDispatcher_2x10MHz_PCBV02_V02(deviceId);
        break;

    case Device4x10MHz_PCBV01:
        messageDispatcher = new MessageDispatcher_4x10MHz_PCBV01_V03(deviceId);
        break;

#ifdef DEBUG
    case Device384Fake:
        messageDispatcher = new MessageDispatcher_384FakeNanopores(deviceId);
        break;
        
    case Device384FakePatchClamp:
        messageDispatcher = new MessageDispatcher_384FakePatchClamp(deviceId);
        break;

    case Device4x10MHzFake:
        messageDispatcher = new MessageDispatcher_4x10MHzFake(deviceId);
        break;

    case Device2x10MHzFake:
        messageDispatcher = new MessageDispatcher_2x10MHzFake(deviceId);
        break;
#endif

    default:
        return ErrorDeviceTypeNotRecognized;
    }

    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->connect(fwPath);

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

ErrorCodes_t MessageDispatcher::connect(std::string fwPath) {
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

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
    if (ljFid == nullptr) {
        createDebugFile(ljFid, "e384CommLib_lj");
    }
#endif

    /*! Initialize device */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    this->initializeDevice();

    deviceCommunicationThread = std::thread(&MessageDispatcher::handleCommunicationWithDevice, this);
    rxConsumerThread = std::thread(&MessageDispatcher::parseDataFromDevice, this);
    liquidJunctionThread = std::thread(&MessageDispatcher::computeLiquidJunction, this);

    threadsStarted = true;

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
            liquidJunctionThread.join();
        }

        this->deinit();
        this->flushBoardList();

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
    channelsPerBoard = currentChannelsNum/totalBoardsNum;

    this->fillChannelList(totalBoardsNum, currentChannelsNum/totalBoardsNum);

    this->initializeHW();

    CalibrationManager calibrationManager(deviceId, currentChannelsNum, totalBoardsNum, vcCurrentRangesNum, vcVoltageRangesNum, ccVoltageRangesNum, ccCurrentRangesNum);

    calibrationParams = calibrationManager.getCalibrationParams(calibrationLoadingError);
    calibrationFileNames = calibrationManager.getCalibrationFileNames();
    calibrationFilesOkFlags = calibrationManager.getCalibrationFilesOkFlags();
    calibrationMappingFileDir = calibrationManager.getMappingFileDir();
    calibrationMappingFilePath = calibrationManager.getMappingFilePath();

    liquidJunctionCurrentEstimatesNum = 0;
    liquidJunctionStates.resize(currentChannelsNum);
    std::fill(liquidJunctionStates.begin(), liquidJunctionStates.end(), LiquidJunctionIdle);
    liquidJunctionCurrentSums.resize(currentChannelsNum);
    std::fill(liquidJunctionCurrentSums.begin(), liquidJunctionCurrentSums.end(), 0);
    liquidJunctionCurrentEstimates.resize(currentChannelsNum);
    std::fill(liquidJunctionCurrentEstimates.begin(), liquidJunctionCurrentEstimates.end(), 0.0);
    liquidJunctionVoltagesBackup.resize(currentChannelsNum);
    std::fill(liquidJunctionVoltagesBackup.begin(), liquidJunctionVoltagesBackup.end(), Measurement({0.0, liquidJunctionRange.prefix, liquidJunctionRange.unit}));
    liquidJunctionDeltaVoltages.resize(currentChannelsNum);
    std::fill(liquidJunctionDeltaVoltages.begin(), liquidJunctionDeltaVoltages.end(), 0.0);
    liquidJunctionDeltaCurrents.resize(currentChannelsNum);
    std::fill(liquidJunctionDeltaCurrents.begin(), liquidJunctionDeltaCurrents.end(), 0.0);
    liquidJunctionSmallestCurrentChange.resize(currentChannelsNum);
    std::fill(liquidJunctionSmallestCurrentChange.begin(), liquidJunctionSmallestCurrentChange.end(), 10.0);
    liquidJunctionConvergingCount.resize(currentChannelsNum);
    std::fill(liquidJunctionConvergingCount.begin(), liquidJunctionConvergingCount.end(), 0);
    liquidJunctionConvergedCount.resize(currentChannelsNum);
    std::fill(liquidJunctionConvergedCount.begin(), liquidJunctionConvergedCount.end(), 0);
    liquidJunctionPositiveSaturationCount.resize(currentChannelsNum);
    std::fill(liquidJunctionPositiveSaturationCount.begin(), liquidJunctionPositiveSaturationCount.end(), 0);
    liquidJunctionNegativeSaturationCount.resize(currentChannelsNum);
    std::fill(liquidJunctionNegativeSaturationCount.begin(), liquidJunctionNegativeSaturationCount.end(), 0);
    liquidJunctionOpenCircuitCount.resize(currentChannelsNum);
    std::fill(liquidJunctionOpenCircuitCount.begin(), liquidJunctionOpenCircuitCount.end(), 0);

    liquidJunctionRange = liquidJunctionRangesArray[defaultLiquidJunctionRangeIdx];
    selectedLiquidJunctionVector.resize(currentChannelsNum);
    fill(selectedLiquidJunctionVector.begin(), selectedLiquidJunctionVector.end(), 0.0*liquidJunctionRange.getMax());

    /*! Some default values*/
    std::vector <bool> allTrue(currentChannelsNum, true);
    std::vector <bool> allFalse(currentChannelsNum, false);

    allChannelIndexes.resize(currentChannelsNum);
    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        allChannelIndexes[idx] = idx;
    }

    std::vector <uint16_t> boardIndexes(totalBoardsNum);
    for (uint16_t idx = 0; idx < totalBoardsNum; idx++) {
        boardIndexes[idx] = idx;
    }

    if (clampingModalitiesArray[defaultClampingModalityIdx] == VOLTAGE_CLAMP) {
        /*! Initialization in voltage clamp */
        this->setClampingModality(VOLTAGE_CLAMP, false);
        this->enableCcCompensations(false);
        this->enableVcCompensations(true);
        this->enableStimulus(allChannelIndexes, allTrue, false);
        this->turnChannelsOn(allChannelIndexes, allTrue, false);
        this->turnCurrentReaderOn(true, false);
        this->setVoltageHoldTuner(allChannelIndexes, selectedVoltageHoldVector, false);
        this->setLiquidJunctionVoltage(allChannelIndexes, selectedLiquidJunctionVector, false);
        this->setSamplingRate(defaultSamplingRateIdx, false);
        this->setVCCurrentRange(defaultVcCurrentRangeIdx, false);
        this->setVCVoltageRange(defaultVcVoltageRangeIdx, false);
        this->setVoltageStimulusLpf(selectedVcVoltageFilterIdx, false);
        this->setGateVoltages(boardIndexes, selectedGateVoltageVector, false);
        this->setSourceVoltages(boardIndexes, selectedSourceVoltageVector, false);
        this->digitalOffsetCompensation(allChannelIndexes, allFalse, false);
        if (calibrationData.vcCalibResArray.size() > 0) {
            this->turnCalSwOn(allChannelIndexes, allFalse, false);
        }

        if (vcCurrentRangesNum > 0) {
            this->updateCalibVcCurrentGain(allChannelIndexes, false);
            this->updateCalibVcCurrentOffset(allChannelIndexes, false);
        }

        if (vcVoltageRangesNum > 0) {
            this->updateCalibVcVoltageGain(allChannelIndexes, false);
            this->updateCalibVcVoltageOffset(allChannelIndexes, false);
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

ErrorCodes_t MessageDispatcher::setChannelSelected(uint16_t chIdx, bool newState) {
    if (chIdx < currentChannelsNum) {
        channelModels[chIdx]->setSelected(newState);
        return Success;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t MessageDispatcher::setBoardSelected(uint16_t brdIdx, bool newState) {
    if (brdIdx < totalBoardsNum) {
        for (auto ch : boardModels[brdIdx]->getChannelsOnBoard()) {
            ch->setSelected(newState);
        }
        return Success;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t MessageDispatcher::setRowSelected(uint16_t rowIdx, bool newState) {
    if (rowIdx < channelsPerBoard) {
        for (auto brd : boardModels) {
            brd->getChannelsOnBoard()[rowIdx]->setSelected(newState);
        }
        return Success;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t MessageDispatcher::setAllChannelsSelected(bool newState) {
    for (auto ch : channelModels) {
        ch->setSelected(newState);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::sendCommands() {
    this->forceOutMessage();
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
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            voltages[i].convertValue(vHoldRange[selectedVcVoltageRangeIdx].prefix);
            voltages[i].value = vHoldTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedVoltageHoldVector[channelIndexes[i]] = voltages[i];
            channelModels[channelIndexes[i]]->setVhold(voltages[i]);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }

        if (!areAllTheVectorElementsInRange(voltages, vHoldRange[selectedVcVoltageRangeIdx].getMin(), vHoldRange[selectedVcVoltageRangeIdx].getMax())) {
            return WarningValueClipped;

        } else {
            return Success;
        }
    }
}

ErrorCodes_t MessageDispatcher::setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag){
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality == VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            currents[i].convertValue(cHoldRange[selectedCcCurrentRangeIdx].prefix);
            currents[i].value = cHoldTunerCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(currents[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedCurrentHoldVector[channelIndexes[i]] = currents[i];
            channelModels[channelIndexes[i]]->setChold(currents[i]);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }

        if (!areAllTheVectorElementsInRange(currents, cHoldRange[selectedCcCurrentRangeIdx].getMin(), cHoldRange[selectedCcCurrentRangeIdx].getMax())) {
            return WarningValueClipped;

        } else {
            return Success;
        }
    }
}

ErrorCodes_t MessageDispatcher::setLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag){
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            voltages[i].convertValue(liquidJunctionRange.prefix);
            voltages[i].value = liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedLiquidJunctionVector[channelIndexes[i]] = voltages[i];
            channelModels[channelIndexes[i]]->setLiquidJunctionVoltage(voltages[i]);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }

        if (!areAllTheVectorElementsInRange(voltages, liquidJunctionRange.getMin(), liquidJunctionRange.getMax())) {
            return WarningValueClipped;

        } else {
            return Success;
        }
    }
}

ErrorCodes_t MessageDispatcher::resetLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, bool applyFlagIn) {
    std::vector<Measurement_t> voltages(channelIndexes.size(), {0.0, liquidJunctionRange.prefix, "V"});
    return setLiquidJunctionVoltage(channelIndexes, voltages, applyFlagIn);
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibVcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibVcCurrentGainRange.prefix);
            calibrationParams.allGainAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]] = gains[i];
        }
        this->updateCalibVcCurrentGain(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.allGainAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].convertValue(calibVcCurrentGainRange.prefix);
            double gain = calibrationParams.allGainAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].value;
            calibVcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].prefix);
            calibrationParams.allOffsetAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]] = offsets[i];
        }
        this->updateCalibVcCurrentOffset(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcCurrentOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.allOffsetAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].convertValue(calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].prefix);
            double offset = calibrationParams.allOffsetAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].value;
            calibVcCurrentOffsetCoders[selectedVcCurrentRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibCcVoltageGainRange.prefix);
            calibrationParams.ccAllGainAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]] = gains[i];
        }
        this->updateCalibCcVoltageGain(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.ccAllGainAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].convertValue(calibCcVoltageGainRange.prefix);
            double gain = calibrationParams.ccAllGainAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].value;
            calibCcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].prefix);
            calibrationParams.ccAllOffsetAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]] = offsets[i];
        }
        this->updateCalibCcVoltageOffset(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcVoltageOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.ccAllOffsetAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].convertValue(calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].prefix);
            double offset = calibrationParams.ccAllOffsetAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].value;
            calibCcVoltageOffsetCoders[selectedCcVoltageRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibVcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibVcVoltageGainRange.prefix);
            calibrationParams.allGainDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]] = gains[i];
        }
        this->updateCalibVcVoltageGain(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.allGainDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].convertValue(calibVcVoltageGainRange.prefix);
            double gain = calibrationParams.allGainDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].value;
            calibVcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
            calibrationParams.allOffsetDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]] = offsets[i];
        }
        this->updateCalibVcVoltageOffset(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcVoltageOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.allOffsetDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
            double offset = calibrationParams.allOffsetDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].value;
            calibVcVoltageOffsetCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(calibCcCurrentGainRange.prefix);
            calibrationParams.ccAllGainDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]] = gains[i];
        }
        this->updateCalibCcCurrentGain(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.ccAllGainDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].convertValue(calibCcCurrentGainRange.prefix);
            double gain = calibrationParams.ccAllGainDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].value;
            calibCcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
            calibrationParams.ccAllOffsetDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]] = offsets[i];
        }
        this->updateCalibCcCurrentOffset(channelIndexes, applyFlag);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::updateCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcCurrentOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            calibrationParams.ccAllOffsetDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].convertValue(calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
            double offset = calibrationParams.ccAllOffsetDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].value;
            calibCcCurrentOffsetCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setGateVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag){
    if (gateVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < boardIndexes.size(); i++){
            gateVoltages[i].convertValue(gateVoltageRange.prefix);
            gateVoltages[i].value = gateVoltageCoders[boardIndexes[i]]->encode(gateVoltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedGateVoltageVector[boardIndexes[i]] = gateVoltages[i];
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }

        if (!areAllTheVectorElementsInRange(gateVoltages, gateVoltageRange.getMin(), gateVoltageRange.getMax())) {
            return WarningValueClipped;

        } else {
            return Success;
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setSourceVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag){
    if (sourceVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < boardIndexes.size(); i++){
            sourceVoltages[i].convertValue(sourceVoltageRange.prefix);
            sourceVoltages[i].value = sourceVoltageCoders[boardIndexes[i]]->encode(sourceVoltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedSourceVoltageVector[boardIndexes[i]] = sourceVoltages[i];
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }

        if (!areAllTheVectorElementsInRange(sourceVoltages, sourceVoltageRange.getMin(), sourceVoltageRange.getMax())) {
            return WarningValueClipped;

        } else {
            return Success;
        }
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

        this->updateCalibVcCurrentGain(allChannelIndexes, false);
        this->updateCalibVcCurrentOffset(allChannelIndexes, false);

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

        this->updateCalibVcVoltageGain(allChannelIndexes, false);
        this->updateCalibVcVoltageOffset(allChannelIndexes, false);
        this->updateVoltageHoldTuner(false);

        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        /*! Most of the times the liquid junction (aka digital offset compensation) will be performed by the same DAC that appliese the voltage sitmulus
            Voltage clamp, so by default the same range is selected for the liquid junction
            When this isnot the case the boolean variable below is set properly by the corresponding derived class of the messagedispatcher */
        if (liquidJunctionSameRangeAsVcDac) {
            this->setLiquidJunctionRange(voltageRangeIdx);
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

        this->updateCalibCcCurrentGain(allChannelIndexes, false);
        this->updateCalibCcCurrentOffset(allChannelIndexes, false);
        this->updateCurrentHoldTuner(false);

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

        this->updateCalibCcVoltageGain(allChannelIndexes, false);
        this->updateCalibCcVoltageOffset(allChannelIndexes, false);

        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setLiquidJunctionRange(uint16_t idx) {
    if (idx >= liquidJunctionRangesNum) {
        return ErrorValueOutOfRange;

    } else {
        selectedLiquidJunctionRangeIdx = idx;
        liquidJunctionRange = liquidJunctionRangesArray[selectedLiquidJunctionRangeIdx];
        liquidJunctionResolution = liquidJunctionRange.step;

        this->setLiquidJunctionVoltage(allChannelIndexes, selectedLiquidJunctionVector, true);

        return Success;
    }
}

ErrorCodes_t MessageDispatcher::digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (digitalOffsetCompensationCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            uint16_t chIdx = channelIndexes[i];
            digitalOffsetCompensationCoders[chIdx]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            channelModels[chIdx]->setCompensatingLiquidJunction(onValues[i]);
            if (onValues[i] && (liquidJunctionStates[chIdx] == LiquidJunctionIdle)) {
                liquidJunctionStates[chIdx] = LiquidJunctionStarting;

            } else if (!onValues[i]) {
                liquidJunctionStates[chIdx] = LiquidJunctionTerminate;
            }
        }

        anyLiquidJuctionActive = true;

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::expandTraces(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues) {
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        channelModels[channelIndexes[i]]->setExpandedTrace(onValues[i]);
    }
    return Success;
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
            channelModels[channelIndexes[i]]->setInStimActive(onValues[i]);
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
           channelModels[channelIndexes[i]]->setOn(onValues[i]);
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

ErrorCodes_t MessageDispatcher::setClampingModality(uint32_t idx, bool applyFlag) {
    if (idx >= clampingModalitiesNum) {
        return ErrorValueOutOfRange;

    } else {
        selectedClampingModalityIdx = idx;
        selectedClampingModality = clampingModalitiesArray[selectedClampingModalityIdx];
        if (clampingModeCoder != nullptr) {
            clampingModeCoder->encode(selectedClampingModalityIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        switch (selectedClampingModality) {
        case VOLTAGE_CLAMP:
            rawDataFilterVoltageFlag = false;
            rawDataFilterCurrentFlag = true;
            break;

        case ZERO_CURRENT_CLAMP:
        case CURRENT_CLAMP:
            rawDataFilterVoltageFlag = true;
            rawDataFilterCurrentFlag = false;
            break;

        case DYNAMIC_CLAMP:
            rawDataFilterVoltageFlag = false;
            rawDataFilterCurrentFlag = false;
            break;
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setClampingModality(ClampingModality_t mode, bool applyFlag) {
    auto iter = std::find(clampingModalitiesArray.begin(), clampingModalitiesArray.end(), mode);
    if (iter == clampingModalitiesArray.end()) {
        return ErrorValueOutOfRange;

    } else {
        return this->setClampingModality((uint32_t)(iter-clampingModalitiesArray.begin()), applyFlag);
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
    if (selectedClampingModality == VOLTAGE_CLAMP){
        if (vcCurrentFilterCoder != nullptr) {
            vcCurrentFilterCoder->encode(sr2LpfVcCurrentMap[selectedSamplingRateIdx], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedVcCurrentFilterIdx = sr2LpfVcCurrentMap[selectedSamplingRateIdx];
        }

    } else {
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
        samplingRate = realSamplingRatesArray[selectedSamplingRateIdx];
        integrationStep = integrationStepArray[selectedSamplingRateIdx];
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
    selectedDownsamplingRatio = ratio;
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

ErrorCodes_t MessageDispatcher::turnVoltageReaderOn(bool, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCurrentReaderOn(bool, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnVoltageStimulusOn(bool, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCurrentStimulusOn(bool, bool){
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
    this->forceOutMessage();
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
    if ((unsigned int)numberOfStates > stateMaxNum || initialState >= numberOfStates){
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
    this->forceOutMessage();
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

ErrorCodes_t MessageDispatcher::getSerialNumber(std::string &serialNumber) {
    serialNumber = deviceId;
    return Success;
}

ErrorCodes_t MessageDispatcher::getBoards(std::vector <BoardModel *> &boards) {
    boards = boardModels;
    return Success;
}

ErrorCodes_t MessageDispatcher::getChannels(std::vector <ChannelModel *> &channels) {
    channels = channelModels;
    return Success;
}

ErrorCodes_t MessageDispatcher::getSelectedChannels(std::vector <bool> &selected) {
    selected.resize(channelModels.size());
    for (int idx = 0; idx < channelModels.size(); idx++) {
        selected[idx] = channelModels[idx]->isSelected();
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getSelectedChannelsIndexes(std::vector <uint16_t> &indexes) {
    for (int idx = 0; idx < channelModels.size(); idx++) {
        if (channelModels[idx]->isSelected()) {
            indexes.push_back(idx);
        }
    }
    return Success;
}

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
    lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdInvalid;

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
        switch (rxMsgBuffer[rxMsgBufferReadOffset].typeId) {
        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
                rxOutput.dataLen = 0;
                rxOutput.protocolId = rxDataBuffer[dataOffset];
                rxOutput.protocolItemIdx = rxDataBuffer[(dataOffset+1) & RX_DATA_BUFFER_MASK];
                rxOutput.protocolRepsIdx = rxDataBuffer[(dataOffset+2) & RX_DATA_BUFFER_MASK];
                rxOutput.protocolSweepIdx = rxDataBuffer[(dataOffset+3) & RX_DATA_BUFFER_MASK];

                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader;

                /*! This message cannot be merged, but stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader) {
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
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid ||
                    (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData && dataWritten+samplesNum < E384CL_OUT_STRUCT_DATA_LEN)) {
                /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid)
                    OR if we are merging successive acquisition data messages that do not overflow the available memory */

                std::unique_lock <std::mutex> ljMutexLock (ljMutex);
                if (downsamplingFlag) {
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    uint32_t downsamplingCount = 0;
                    for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                        if (++downsamplingOffset >= selectedDownsamplingRatio) {
                            downsamplingOffset = 0;
                            downsamplingCount++;
                            for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
#ifdef FILTER_CLIP_NEEDED
                                xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                                data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                                data[dataWritten+sampleIdx++] = (int16_t)round(this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen));
#endif
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }

                            for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
#ifdef FILTER_CLIP_NEEDED
                                xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                                data[dataWritten+sampleIdx] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                                data[dataWritten+sampleIdx] = (int16_t)round(this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen));
#endif
                                liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)data[dataWritten+sampleIdx];
                                sampleIdx++;
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }
                            liquidJunctionCurrentEstimatesNum++;

                        } else {
                            for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
                                xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }

                            for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
                                xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                                liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)round(xFlt);
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }
                            liquidJunctionCurrentEstimatesNum++;
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
#ifdef FILTER_CLIP_NEEDED
                            xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                            data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                            data[dataWritten+sampleIdx++] = (int16_t)round(this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen));
#endif
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            rawFloat = (int16_t)rxDataBuffer[dataOffset];
#ifdef FILTER_CLIP_NEEDED
                            xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                            data[dataWritten+sampleIdx] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                            data[dataWritten+sampleIdx] = (int16_t)round(this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen));
#endif
                            liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)data[dataWritten+sampleIdx];
                            sampleIdx++;
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }
                        liquidJunctionCurrentEstimatesNum++;

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
                            data[dataWritten+sampleIdx] = rxDataBuffer[dataOffset];
                            liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)data[dataWritten+sampleIdx];
                            sampleIdx++;
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }
                        liquidJunctionCurrentEstimatesNum++;
                    }
                }

                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData;

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
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                /*! Evaluate only if it's the first repetition */
                rxOutput.dataLen = 0;
                rxOutput.protocolId = rxDataBuffer[dataOffset];

                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail;

                /*! This message cannot be merged, but stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail) {
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
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation;

                /*! This message cannot be merged, leave anyway */
                exitLoop = true;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }

            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus):
//            not really managed, ignore it
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus;
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
            lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdInvalid;

            /*! Leave and look for following messages */
            exitLoop = true;
            messageReadFlag = true;
            break;
        }
        rxOutput.msgTypeId = lastParsedMsgType;
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

ErrorCodes_t MessageDispatcher::purgeData() {
    ErrorCodes_t ret = Success;

    std::unique_lock <std::mutex> rxMutexLock (rxMsgMutex);
    rxMsgBufferReadOffset = (rxMsgBufferReadOffset+rxMsgBufferReadLength) & RX_MSG_BUFFER_MASK;
    rxMsgBufferReadLength = 0;
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

ErrorCodes_t MessageDispatcher::getLiquidJunctionVoltages(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> &voltages) {
    if (selectedLiquidJunctionVector.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else {
        voltages.resize(channelIndexes.size());
        for (auto channel : channelIndexes) {
            voltages[channel] = selectedLiquidJunctionVector[channel];
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures){
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else{
        voltageHoldTunerFeatures = vHoldRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures){
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else{
        currentHoldTunerFeatures = cHoldRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges) {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else{
        ranges = liquidJunctionRangesArray;
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

ErrorCodes_t MessageDispatcher::hasGateVoltages() {
    if (gateVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::hasSourceVoltages() {
    if (sourceVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures){
    if (gateVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else{
        gateVoltagesFeatures = gateVoltageRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures){
    if (sourceVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else{
        sourceVoltagesFeatures = sourceVoltageRange;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getChannelNumberFeatures(uint16_t &voltageChannelNumberFeatures, uint16_t &currentChannelNumberFeatures){
    voltageChannelNumberFeatures = voltageChannelsNum;
    currentChannelNumberFeatures = currentChannelsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getChannelNumberFeatures(int &voltageChannelNumberFeatures, int &currentChannelNumberFeatures){
    voltageChannelNumberFeatures = (int)voltageChannelsNum;
    currentChannelNumberFeatures = (int)currentChannelsNum;
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

ErrorCodes_t MessageDispatcher::getBoardsNumberFeatures(int &boardsNumberFeatures) {
    boardsNumberFeatures = (int)totalBoardsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getClampingModalitiesFeatures(std::vector <ClampingModality_t> &clampingModalitiesFeatures) {
    if (clampingModalitiesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        clampingModalitiesFeatures = clampingModalitiesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getClampingModality(ClampingModality_t &clampingModality) {
    if (clampingModalitiesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        clampingModality = (ClampingModality_t)selectedClampingModality;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getClampingModalityIdx(uint32_t &idx) {
    if (clampingModalitiesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedClampingModalityIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges, uint16_t &defaultVcCurrRangeIdx) {
    if (vcCurrentRangesArray.empty()){
        return ErrorFeatureNotImplemented;
    } else {
        currentRanges = vcCurrentRangesArray;
        defaultVcCurrRangeIdx = defaultVcCurrentRangeIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges) {
    if(vcVoltageRangesArray.empty()){
        return ErrorFeatureNotImplemented;
    } else {
        voltageRanges = vcVoltageRangesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges) {
    if(ccCurrentRangesArray.empty()){
        return ErrorFeatureNotImplemented;
    } else {
        currentRanges = ccCurrentRangesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges) {
    if(ccVoltageRangesArray.empty()){
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

ErrorCodes_t MessageDispatcher::getLiquidJunctionRange(RangedMeasurement_t &range) {
    if (liquidJunctionRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        range = liquidJunctionRangesArray[selectedLiquidJunctionRangeIdx];
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

ErrorCodes_t MessageDispatcher::getVCCurrentRangeIdx(uint32_t &idx) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedVcCurrentRangeIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCVoltageRangeIdx(uint32_t &idx) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedVcVoltageRangeIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCCurrentRangeIdx(uint32_t &idx) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedVcCurrentRangeIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCVoltageRangeIdx(uint32_t &idx) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedCcVoltageRangeIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getSamplingRatesFeatures(std::vector <Measurement_t> &samplingRates) {
    if(samplingRatesArray.empty()){
        return ErrorFeatureNotImplemented;

    } else {
        samplingRates = samplingRatesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getSamplingRate(Measurement_t &samplingRate) {
    if(samplingRatesArray.empty()){
        return ErrorFeatureNotImplemented;

    } else {
        samplingRate = this->samplingRate;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getSamplingRateIdx(uint32_t &idx) {
    if(samplingRatesArray.empty()){
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedSamplingRateIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getRealSamplingRatesFeatures(std::vector <Measurement_t> &realSamplingRates) {
    if(realSamplingRatesArray.empty()){
        return ErrorFeatureNotImplemented;

    } else {
        realSamplingRates = realSamplingRatesArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getMaxDownsamplingRatioFeature(uint32_t &ratio) {
    ratio = maxDownSamplingRatio;
    return Success;
}

ErrorCodes_t MessageDispatcher::getDownsamplingRatio(uint32_t &ratio) {
    ratio = selectedDownsamplingRatio;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageFilters(std::vector <Measurement_t> &filters){
    if (vcVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filters = vcVoltageFiltersArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCCurrentFilters(std::vector <Measurement_t> &filters){
    if (vcCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filters = vcCurrentFiltersArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCVoltageFilters(std::vector <Measurement_t> &filters){
    if (ccVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filters = ccVoltageFiltersArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCCurrentFilters(std::vector <Measurement_t> &filters){
    if (ccCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filters = ccCurrentFiltersArray;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCVoltageFilter(Measurement_t &filter) {
    if (vcVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filter = vcVoltageFiltersArray[selectedVcVoltageFilterIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCCurrentFilter(Measurement_t &filter) {
    if (vcCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filter = vcCurrentFiltersArray[selectedVcCurrentFilterIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCVoltageFilter(Measurement_t &filter) {
    if (ccVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filter = ccVoltageFiltersArray[selectedCcVoltageFilterIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCCurrentFilter(Measurement_t &filter) {
    if (ccCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        filter = ccCurrentFiltersArray[selectedCcCurrentFilterIdx];
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCVoltageFilterIdx(uint32_t &idx) {
    if (vcVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedVcVoltageFilterIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getVCCurrentFilterIdx(uint32_t &idx) {
    if (vcCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedVcCurrentFilterIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCVoltageFilterIdx(uint32_t &idx) {
    if (ccVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedCcVoltageFilterIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::getCCCurrentFilterIdx(uint32_t &idx) {
    if (ccCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        idx = selectedCcCurrentFilterIdx;
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::hasChannelSwitches() {
    if (turnChannelsOnCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::hasStimulusSwitches() {
    if (enableStimulusCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::hasOffsetCompensation() {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else {
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

ErrorCodes_t MessageDispatcher::hasProtocols() {
    if (protocolMaxItemsNum < 1) {
        return ErrorFeatureNotImplemented;

    } else {
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
    if(calibrationData.vcCalibResArray.empty()){
        return ErrorFeatureNotImplemented;
    } else {
        calibData = calibrationData;
        return Success;
    }
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

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
    fclose(ljFid);
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

void MessageDispatcher::computeLiquidJunction() {
    std::unique_lock <std::mutex> ljMutexLock (ljMutex);
    ljMutexLock.unlock();

//    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> voltages;

    Measurement_t voltage;
    double estimatedResistance;

    while (!stopConnectionFlag) {
        if (anyLiquidJuctionActive && liquidJunctionCurrentEstimatesNum > 0) {
            anyLiquidJuctionActive = false;
            channelIndexes.clear();
            voltages.clear();
            ljMutexLock.lock();
            for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
                switch(liquidJunctionStates[channelIdx]) {
                case LiquidJunctionIdle:
                    break;

                case LiquidJunctionStarting:
                    anyLiquidJuctionActive = true;
                    channelIndexes.push_back(channelIdx);
                    liquidJunctionVoltagesBackup[channelIdx] = selectedLiquidJunctionVector[channelIdx];
                    voltages.push_back(selectedLiquidJunctionVector[channelIdx]);
                    liquidJunctionStates[channelIdx] = LiquidJunctionFirstStep;
                    liquidJunctionConvergingCount[channelIdx] = 0;
                    liquidJunctionConvergedCount[channelIdx] = 0;
                    liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                    liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                    liquidJunctionOpenCircuitCount[channelIdx] = 0;

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                    fprintf(ljFid,
                           "%d: starting. "
                           "Liq jun %s, "
                           "conving %d, "
                           "convd %d, "
                           "possat %d, "
                           "negsat %d, "
                           "opencirc %d\n",
                           channelIdx,
                           selectedLiquidJunctionVector[channelIdx].label().c_str(),
                           liquidJunctionConvergingCount[channelIdx],
                           liquidJunctionConvergedCount[channelIdx],
                           liquidJunctionPositiveSaturationCount[channelIdx],
                           liquidJunctionNegativeSaturationCount[channelIdx],
                           liquidJunctionOpenCircuitCount[channelIdx]);
                    fflush(ljFid);
#endif
                    break;

                case LiquidJunctionFirstStep:
                    anyLiquidJuctionActive = true;
                    liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                    if (liquidJunctionCurrentEstimates[channelIdx] > 30000.0) { /*! More or less 10% from saturation */
                        liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*100.0;
                        liquidJunctionStates[channelIdx] = LiquidJunctionConverge;

                    } else if (liquidJunctionCurrentEstimates[channelIdx] > 0.0) { /*! Positive but not saturating */
                        liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*10.0;
                        liquidJunctionStates[channelIdx] = LiquidJunctionConverge;

                    } else if (liquidJunctionCurrentEstimates[channelIdx] < -30000.0) { /*! More or less 10% from saturation */
                        liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*100.0;
                        liquidJunctionStates[channelIdx] = LiquidJunctionConverge;

                    } else { /*! Negative but not saturating */
                        liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*10.0;
                        liquidJunctionStates[channelIdx] = LiquidJunctionConverge;
                    }
                    selectedLiquidJunctionVector[channelIdx].value += liquidJunctionDeltaVoltages[channelIdx];
                    channelIndexes.push_back(channelIdx);
                    voltages.push_back(selectedLiquidJunctionVector[channelIdx]);

                    liquidJunctionConvergingCount[channelIdx]++;

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                    fprintf(ljFid,
                           "%d: first. "
                           "Curr est %f, "
                           "DV %f, "
                           "Liq jun %s, "
                           "conving %d, "
                           "convd %d, "
                           "possat %d, "
                           "negsat %d, "
                           "opencirc %d\n",
                           channelIdx,
                           liquidJunctionCurrentEstimates[channelIdx],
                           liquidJunctionDeltaVoltages[channelIdx],
                           selectedLiquidJunctionVector[channelIdx].label().c_str(),
                           liquidJunctionConvergingCount[channelIdx],
                           liquidJunctionConvergedCount[channelIdx],
                           liquidJunctionPositiveSaturationCount[channelIdx],
                           liquidJunctionNegativeSaturationCount[channelIdx],
                           liquidJunctionOpenCircuitCount[channelIdx]);
                    fflush(ljFid);
#endif
                    break;

                case LiquidJunctionConverge:
                    anyLiquidJuctionActive = true;
                    liquidJunctionDeltaCurrents[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum-liquidJunctionCurrentEstimates[channelIdx];
                    liquidJunctionCurrentEstimates[channelIdx] += liquidJunctionDeltaCurrents[channelIdx];

                    if (abs(liquidJunctionDeltaCurrents[channelIdx]) < 2.0*liquidJunctionSmallestCurrentChange[channelIdx]) { /*! current change smaller than 10 LSB */
                        if (liquidJunctionCurrentEstimates[channelIdx] > 30000.0) { /*! More or less 10% from saturation */
                            liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*100.0;
                            liquidJunctionConvergedCount[channelIdx] = 0;
                            liquidJunctionPositiveSaturationCount[channelIdx]++;
                            liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                            liquidJunctionOpenCircuitCount[channelIdx] = 0;

                        } else if (liquidJunctionCurrentEstimates[channelIdx] < -30000.0) { /*! More or less 10% from saturation */
                            liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*100.0;
                            liquidJunctionConvergedCount[channelIdx] = 0;
                            liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                            liquidJunctionNegativeSaturationCount[channelIdx]++;
                            liquidJunctionOpenCircuitCount[channelIdx] = 0;

                        } else { /*! Not saturating */
                            if (liquidJunctionDeltaCurrents[channelIdx]*liquidJunctionDeltaVoltages[channelIdx] > 0.0) { /*! Current change consistent with voltage change */
                                estimatedResistance = liquidJunctionDeltaVoltages[channelIdx]/liquidJunctionDeltaCurrents[channelIdx];
                                liquidJunctionSmallestCurrentChange[channelIdx] = liquidJunctionResolution/estimatedResistance;
                                liquidJunctionDeltaVoltages[channelIdx] = round(-liquidJunctionCurrentEstimates[channelIdx]*estimatedResistance/liquidJunctionResolution)*liquidJunctionResolution;
                                liquidJunctionConvergedCount[channelIdx] = 0;
                                liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                                liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                                liquidJunctionOpenCircuitCount[channelIdx] = 0;

                            } else if (abs(liquidJunctionCurrentEstimates[channelIdx]) < 2.0*liquidJunctionSmallestCurrentChange[channelIdx]) { /*! Current already very small, converging */
                                if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 0.5*liquidJunctionSmallestCurrentChange[channelIdx]) {
                                    if (liquidJunctionDeltaCurrents[channelIdx] > 0.0) {
                                        liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution;

                                    } else {
                                        liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution;
                                    }
                                }
                                liquidJunctionConvergedCount[channelIdx]++;
                                liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                                liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                                liquidJunctionOpenCircuitCount[channelIdx] = 0;

                            } else { /*! Current not so small, probable open circuit */
                                if (liquidJunctionDeltaCurrents[channelIdx] > 0.0) {
                                    liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*100.0;

                                } else {
                                    liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*100.0;
                                }
                                liquidJunctionConvergedCount[channelIdx] = 0;
                                liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                                liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                                liquidJunctionOpenCircuitCount[channelIdx]++;
                            }
                        }

                    } else { /*! current change greater than 10 LSB */
                        if (liquidJunctionDeltaCurrents[channelIdx]*liquidJunctionDeltaVoltages[channelIdx] > 0.0) { /*! Current change consistent with voltage change */
                            estimatedResistance = liquidJunctionDeltaVoltages[channelIdx]/liquidJunctionDeltaCurrents[channelIdx];
                            liquidJunctionSmallestCurrentChange[channelIdx] = liquidJunctionResolution/estimatedResistance;
                            liquidJunctionDeltaVoltages[channelIdx] = round(-liquidJunctionCurrentEstimates[channelIdx]*estimatedResistance/liquidJunctionResolution)*liquidJunctionResolution;
                            liquidJunctionOpenCircuitCount[channelIdx] = 0;

                        } else if (abs(liquidJunctionCurrentEstimates[channelIdx]) < 2.0*liquidJunctionSmallestCurrentChange[channelIdx]) { /*! Current already very small, converging */
                            if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 0.5*liquidJunctionSmallestCurrentChange[channelIdx]) {
                                if (liquidJunctionDeltaCurrents[channelIdx] > 0.0) {
                                    liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution;

                                } else {
                                    liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution;
                                }
                            }
                            liquidJunctionConvergedCount[channelIdx]++;
                            liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                            liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                            liquidJunctionOpenCircuitCount[channelIdx] = 0;

                        } else { /*! Current not so small, probable open circuit */
                            if (liquidJunctionDeltaCurrents[channelIdx] > 0.0) {
                                liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*100.0;

                            } else {
                                liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*100.0;
                            }
                            liquidJunctionConvergedCount[channelIdx] = 0;
                            liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                            liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                            liquidJunctionOpenCircuitCount[channelIdx]++;
                        }
                    }

                    selectedLiquidJunctionVector[channelIdx].value += liquidJunctionDeltaVoltages[channelIdx];
                    liquidJunctionConvergingCount[channelIdx]++;

                    channelIndexes.push_back(channelIdx);
                    voltages.push_back(selectedLiquidJunctionVector[channelIdx]);

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                    fprintf(ljFid,
                           "%d: converge. "
                           "DI %f, "
                           "Curr est %f, "
                           "DV %f, "
                           "Liq jun %s, "
                           "conving %d, "
                           "convd %d, "
                           "possat %d, "
                           "negsat %d, "
                           "opencirc %d\n",
                           channelIdx,
                           liquidJunctionDeltaCurrents[channelIdx],
                           liquidJunctionCurrentEstimates[channelIdx],
                           liquidJunctionDeltaVoltages[channelIdx],
                           selectedLiquidJunctionVector[channelIdx].label().c_str(),
                           liquidJunctionConvergingCount[channelIdx],
                           liquidJunctionConvergedCount[channelIdx],
                           liquidJunctionPositiveSaturationCount[channelIdx],
                           liquidJunctionNegativeSaturationCount[channelIdx],
                           liquidJunctionOpenCircuitCount[channelIdx]);
                    fflush(ljFid);
#endif

                    if (liquidJunctionConvergingCount[channelIdx] > 50) {
                        liquidJunctionStates[channelIdx] = LiquidJunctionFailTooManySteps;

                    } else if (liquidJunctionConvergedCount[channelIdx] > 2) {
                        liquidJunctionStates[channelIdx] = LiquidJunctionSuccess;

                    } else if (liquidJunctionOpenCircuitCount[channelIdx] > 10) {
                        liquidJunctionStates[channelIdx] = LiquidJunctionFailOpenCircuit;

                    } else if (liquidJunctionPositiveSaturationCount[channelIdx] > 20 || liquidJunctionNegativeSaturationCount[channelIdx] > 20) {
                        liquidJunctionStates[channelIdx] = LiquidJunctionFailSaturation;
                    }
                    break;

                case LiquidJunctionSuccess:
                    anyLiquidJuctionActive = true;
                    liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                    fprintf(ljFid,
                           "%d: success.",
                           channelIdx);
                    fflush(ljFid);
#endif
                    break;

                case LiquidJunctionFailOpenCircuit:
                    anyLiquidJuctionActive = true;
                    channelIndexes.push_back(channelIdx);
                    voltages.push_back(liquidJunctionVoltagesBackup[channelIdx]);
                    liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                    fprintf(ljFid,
                           "%d: open circuit.",
                           channelIdx);
                    fflush(ljFid);
#endif
                    break;

                case LiquidJunctionFailTooManySteps:
                    anyLiquidJuctionActive = true;
                    liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                    fprintf(ljFid,
                           "%d: too many steps.",
                           channelIdx);
                    fflush(ljFid);
#endif
                    break;

                case LiquidJunctionFailSaturation:
                    anyLiquidJuctionActive = true;
                    channelIndexes.push_back(channelIdx);
                    voltages.push_back(liquidJunctionVoltagesBackup[channelIdx]);
                    liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                    fprintf(ljFid,
                           "%d: saturation.",
                           channelIdx);
                    fflush(ljFid);
#endif
                    break;

                case LiquidJunctionTerminate:
                    anyLiquidJuctionActive = true;
                    liquidJunctionSmallestCurrentChange[channelIdx] = 10.0;
                    liquidJunctionStates[channelIdx] = LiquidJunctionIdle;
                    break;
                }
            }
            ljMutexLock.unlock();

            this->setLiquidJunctionVoltage(channelIndexes, voltages, true);

            std::unique_lock <std::mutex> txMutexLock (txMutex);
            while (txMsgBufferReadLength > 0 && !stopConnectionFlag) {
                txMsgBufferNotFull.wait_for(txMutexLock, std::chrono::milliseconds(100));
            }
            txMutexLock.unlock();

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
        } else {
            if (anyLiquidJuctionActive) {
                fprintf(ljFid,
                        "not performing: %lld.\n",
                        liquidJunctionCurrentEstimatesNum);
                fflush(ljFid);
            }
#endif
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        ljMutexLock.lock();
        liquidJunctionCurrentEstimatesNum = 0;
        std::fill(liquidJunctionCurrentSums.begin(), liquidJunctionCurrentSums.end(), 0);
        ljMutexLock.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void MessageDispatcher::forceOutMessage() {
    if (txModifiedEndingWord <= txModifiedStartingWord) {
        txModifiedStartingWord = 0;
        txModifiedEndingWord = 1;
    }
}

bool MessageDispatcher::checkProtocolValidity(std::string &) {
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
        rawDataFilterCutoffFrequencyOverride.value = samplingRate.getNoPrefixValue()*0.25/(double)selectedDownsamplingRatio;

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

#ifdef USE_2ND_ORDER_BUTTERWORTH
    if (enableFilter) {
        double k1 = tan(M_PI*rawDataFilterCutoffFrequency.value*integrationStep.value); /*!< pre-warp coefficient */
        double k12 = k1*k1;
        double k2 = -2+2*k12; /*!< frequently used expression */
        double d = 1.0/(1.0+k1*IIR_2_SIN_PI_4+k12); /*!< denominator */

        if (rawDataFilterVoltageFlag) {
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
#else
    if (enableFilter) {
        double wT = 2.0*M_PI*cutoffFrequency*integrationStep.value;
        double ky = (2.0-wT)/(2.0+wT);
        double kx;
        if (rawDataFilterLowPassFlag) {
            kx = 1.0-ky;

        } else {
            kx = 1.0+ky;
        }
        if (rawDataFilterVoltageFlag) {

            /*! Denominators */
            iirVDen[0] = 1.0;
            iirVDen[1] = -ky;

            /*! Gains and numerators */
            if (rawDataFilterLowPassFlag) {
                iirVNum[0] = kx*0.5;
                iirVNum[1] = kx*0.5;

            } else {
                iirVNum[0] = kx*0.5;
                iirVNum[1] = -kx*0.5;
            }

        } else {
            /*! Voltage is not filtered */
            iirVNum[0] = 1.0;
            iirVNum[1] = 0.0;
            iirVDen[0] = 1.0;
            iirVDen[1] = 0.0;
        }

        if (rawDataFilterCurrentFlag) {
            /*! Denominators */
            iirIDen[0] = 1.0;
            iirIDen[1] = -ky;

            /*! Gains and numerators */
            if (rawDataFilterLowPassFlag) {
                iirINum[0] = kx*0.5;
                iirINum[1] = kx*0.5;

            } else {
                iirINum[0] = kx*0.5;
                iirINum[1] = -kx*0.5;
            }

        } else {
            /*! Current is not filtered */
            iirINum[0] = 1.0;
            iirINum[1] = 0.0;
            iirIDen[0] = 1.0;
            iirIDen[1] = 0.0;
        }

    } else {
        /*! Delta impulse response with no autoregressive part */
        iirVNum[0] = 1.0;
        iirVNum[1] = 0.0;
        iirVDen[0] = 1.0;
        iirVDen[1] = 0.0;

        iirINum[0] = 1.0;
        iirINum[1] = 0.0;
        iirIDen[0] = 1.0;
        iirIDen[1] = 0.0;
    }
#endif

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

void MessageDispatcher::updateVoltageHoldTuner(bool applyFlag) {
    if (vHoldTunerCoders.empty()) {
        return;

    } else {
        for (uint32_t i = 0; i < currentChannelsNum; i++) {
            vHoldTunerCoders[selectedVcVoltageRangeIdx][i]->encode(selectedVoltageHoldVector[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
    }
}

void MessageDispatcher::updateCurrentHoldTuner(bool applyFlag) {
    if (cHoldTunerCoders.empty()) {
        return;

    } else {
        for (uint32_t i = 0; i < voltageChannelsNum; i++) {
            cHoldTunerCoders[selectedCcCurrentRangeIdx][i]->encode(selectedCurrentHoldVector[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
    }
}

ErrorCodes_t MessageDispatcher::turnResistanceCompensationOn(std::vector<uint16_t>,std::vector<bool>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnLeakConductanceCompensationOn(std::vector<uint16_t>,std::vector<bool>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnBridgeBalanceCompensationOn(std::vector<uint16_t>,std::vector<bool>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableCompensation(std::vector<uint16_t>, uint16_t, std::vector<bool>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableVcCompensations(bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableCcCompensations(bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCompValues(std::vector<uint16_t>, CompensationUserParams, std::vector<double>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCompOptions(std::vector<uint16_t>, CompensationTypes, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setPipetteCompensationOptions(std::vector<uint16_t>, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setMembraneCompensationOptions(std::vector<uint16_t>, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCompensationOptions(std::vector<uint16_t>, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionOptions(std::vector<uint16_t>, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCCPipetteCompensationOptions(std::vector<uint16_t>, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setLeakConductanceCompensationOptions(std::vector<uint16_t>, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setBridgeBalanceCompensationOptions(std::vector<uint16_t>, std::vector<uint16_t>, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCorrectionLag(std::vector<uint16_t>, std::vector<double> channelValues, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionPercentage(std::vector<uint16_t>, std::vector<double> channelValues, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionBandwidthGain(std::vector<uint16_t>, std::vector<double> channelValues, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionTau(std::vector<uint16_t>, std::vector<double> channelValues, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setLeakConductance(std::vector<uint16_t>, std::vector<double> channelValues, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setBridgeBalanceResistance(std::vector<uint16_t>, std::vector<double> channelValues, bool){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasCompFeature(uint16_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompFeatures(uint16_t, std::vector<RangedMeasurement_t> &, double &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompOptionsFeatures(CompensationTypes ,std::vector <std::string> &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompValueMatrix(std::vector<std::vector<double>> &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompensationEnables(std::vector<uint16_t>, uint16_t, std::vector<bool> &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getPipetteCompensationOptions(std::vector <std::string>){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCCPipetteCompensationOptions(std::vector <std::string>){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getMembraneCompensationOptions(std::vector <std::string>){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCompensationOptions(std::vector <std::string>){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionOptions(std::vector <std::string>){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductanceCompensationOptions(std::vector <std::string>){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceCompensationOptions(std::vector <std::string>){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getPipetteCapacitanceControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCCPipetteCapacitanceControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getMembraneCapacitanceControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistanceControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCorrectionPercentageControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCorrectionLagControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionGainControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionPercentageControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionBandwidthGainControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionTauControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductanceControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceResistanceControl(CompensationControl_t &){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistanceCorrectionLag(std::vector<uint16_t>, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistancePredictionPercentage(std::vector<uint16_t>, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistancePredictionBandwidthGain(std::vector<uint16_t>, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistancePredictionTau(std::vector<uint16_t>, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductance(std::vector<uint16_t>, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceResistance(std::vector<uint16_t>, std::vector<double> channelValues, std::vector<bool> activeNotActive){
    return ErrorFeatureNotImplemented;
}

std::vector<double> MessageDispatcher::user2AsicDomainTransform(int, std::vector<double>){
    return std::vector<double>();
}

std::vector<double> MessageDispatcher::asic2UserDomainTransform(int, std::vector<double>, double, double){
    return std::vector<double>();
}

ErrorCodes_t MessageDispatcher::asic2UserDomainCompensable(int, std::vector<double>, std::vector<double>){
    return ErrorFeatureNotImplemented;
}

double MessageDispatcher::computeAsicCmCinj(double, bool, MultiCoder::MultiCoderConfig_t){
    return -DBL_MAX;
}

void MessageDispatcher::fillBoardList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard){
    this->boardModels.resize(numOfBoards);
    for(uint16_t i = 0; i< numOfBoards; i++ ){
        BoardModel* board = new BoardModel;
        board->setId(i);
        board->fillChannelList(numOfChannelsOnBoard);
        this->boardModels[i] = board;
    }
}

void MessageDispatcher::fillChannelList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard){
    if(this->boardModels.size() == 0){
        this->fillBoardList(numOfBoards, numOfChannelsOnBoard);
    }
    uint16_t newChannelId = 0;
    channelModels.resize(numOfChannelsOnBoard*numOfBoards);
    for(uint16_t i = 0; i< numOfBoards; i++ ){
        for(uint16_t j = 0; j< numOfChannelsOnBoard; j++ ){
            this->channelModels[newChannelId] = this->boardModels[i]->getChannelsOnBoard()[j];
            newChannelId++;
        }
    }
}

void MessageDispatcher::flushBoardList() {
    size_t numOfBoards = this->boardModels.size();
    for(uint16_t i = 0; i< numOfBoards; i++ ){
        if (this->boardModels[i] != nullptr) {
            delete this->boardModels[i];
        }
    }
    boardModels.clear();
    channelModels.clear();
}
