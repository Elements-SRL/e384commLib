#include "messagedispatcher.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <thread>
#include <math.h>
#include <random>
#include <algorithm>

#include "okFrontPanelDLL.h"

#include "messagedispatcher_384nanopores.h"
#include "messagedispatcher_384patchclamp.h"
#ifdef DEBUG
#include "messagedispatcher_384fake.h"
#endif
#include "utils.h"

using namespace std;

static unordered_map <string, DeviceTypes_t> deviceIdMapping = {
    {"221000107S", Device384Nanopores},
    {"pup", Device384PatchClamp}
    #ifdef DEBUG
    ,{"FAKE", Device384Fake}
    #endif
}; /*! \todo FCON queste info dovrebbero risiedere nel DB, e ci vanno comunque i numeri seriali corretti delle opal kelly */

/********************************************************************************************\
 *                                                                                          *
 *                                 MessageDispatcher                                        *
 *                                                                                          *
\********************************************************************************************/

/*****************\
 *  Ctor / Dtor  *
\*****************/

MessageDispatcher::MessageDispatcher(string deviceId) :
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
}

MessageDispatcher::~MessageDispatcher() {
    this->disconnectDevice();
}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t MessageDispatcher::init() {
    rxMsgBuffer = new (std::nothrow) MsgResume_t[RX_MSG_BUFFER_SIZE];
    if (rxMsgBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    rxDataBuffer = new (std::nothrow) uint16_t[RX_DATA_BUFFER_SIZE+1]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word */
    if (rxDataBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    txMsgBuffer = new (std::nothrow) vector <uint16_t>[TX_MSG_BUFFER_SIZE];
    if (txMsgBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    txMsgOffsetWord.resize(TX_MSG_BUFFER_SIZE);
    txMsgLength.resize(TX_MSG_BUFFER_SIZE);

    this->computeMinimumPacketNumber();

    /*! Allocate memory for raw data filters */
    this->initializeRawDataFilterVariables();

    /*! Allocate memory for compensations */
//    this->initializeCompensations(); /*! \todo FCON */

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

#ifdef DEBUG_PRINT
    fclose(fid);
#endif

    return Success;
}

ErrorCodes_t MessageDispatcher::detectDevices(
        vector <string> &deviceIds) {
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
    deviceIds.push_back("FAKE");
#endif

    return Success;
}

ErrorCodes_t MessageDispatcher::getDeviceType(string deviceId, DeviceTypes_t &type) {
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

#ifdef DEBUG
    case Device384Fake:
        messageDispatcher = new MessageDispatcher_384Fake(deviceId);
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

    ErrorCodes_t ret;
    connected = true;

    this->init();

    stopConnectionFlag = false;

#ifdef DEBUG_PRINT
#ifdef _WIN32
    string path = string(getenv("HOMEDRIVE"))+string(getenv("HOMEPATH"));
#else
    string path = string(getenv("HOME"));
#endif
    stringstream ss;

    for (size_t i = 0; i < path.length(); ++i) {
        if (path[i] == '\\') {
            ss << "\\\\";

        } else {
            ss << path[i];
        }
    }
#ifdef _WIN32
    ss << "\\\\e384CommLib_tx.txt";
#else
    ss << "/e384CommLib_tx.txt";
#endif

    fid = fopen(ss.str().c_str(), "wb");
#endif

    rxThread = thread(&MessageDispatcher::readDataFromDevice, this);

    txThread = thread(&MessageDispatcher::sendCommandsToDevice, this);

    threadsStarted = true;

    this->resetFpga(true, true);
    this->resetFpga(false, false);

    this_thread::sleep_for(chrono::milliseconds(1000));

    this->resetAsic(true, true);
    this_thread::sleep_for(chrono::milliseconds(100));
    ret = this->resetAsic(false, true);
    if (ret != Success) {
        return ErrorConnectionChipResetFailed;
    }

    /*! Initialize device */
    this->initializeDevice();
    this->stackOutgoingMessage(txStatus);

    this_thread::sleep_for(chrono::milliseconds(100));

    return ret;
}

ErrorCodes_t MessageDispatcher::disconnect() {
    if (!connected) {
        return ErrorDeviceNotConnected;
    }

    if (!stopConnectionFlag) {
        stopConnectionFlag = true;

        if (threadsStarted) {
            rxThread.join();
            txThread.join();
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

ErrorCodes_t MessageDispatcher::resetAsic(bool resetFlag, bool applyFlagIn) {
    asicResetCoder->encode(resetFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::resetFpga(bool resetFlag, bool applyFlagIn) {
    fpgaResetCoder->encode(resetFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageHoldTuner(vector<uint16_t> channelIndexes, vector<Measurement_t> voltages, bool applyFlag){
    if (vHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(voltages, vHoldRange.getMin(), vHoldRange.getMax())) {
        return ErrorValueOutOfRange;

    } else if (!amIinVoltageClamp) {
        return ErrorWrongClampModality;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            voltages[i].convertValue(vHoldRange.prefix);
            selectedVoltageHoldVector[channelIndexes[i]] = voltages[i];
            vHoldTunerCoders[channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCurrentHoldTuner(vector<uint16_t> channelIndexes, vector<Measurement_t> currents, bool applyFlag){
    if (cHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(currents, cHoldRange.getMin(), cHoldRange.getMax())) {
        return ErrorValueOutOfRange;

    } else if (amIinVoltageClamp) {
        return ErrorWrongClampModality;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            currents[i].convertValue(cHoldRange.prefix);
            cHoldTunerCoders[channelIndexes[i]]->encode(currents[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentGain(vector<uint16_t> channelIndexes, vector<Measurement_t> gains, bool applyFlag){
    if (calibVcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(gains, calibVcCurrentGainRange.getMin(), calibVcCurrentGainRange.getMax())) {
        return ErrorValueOutOfRange;

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

ErrorCodes_t MessageDispatcher::setCalibVcCurrentOffset(vector<uint16_t> channelIndexes, vector<Measurement_t> offsets, bool applyFlag){
    if (calibVcCurrentOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(offsets, calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].getMin(), calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].getMax())) {
        return ErrorValueOutOfRange;

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

ErrorCodes_t MessageDispatcher::setCalibCcVoltageGain(vector<uint16_t> channelIndexes, vector<Measurement_t> gains, bool applyFlag){
    if (calibCcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(gains, calibCcVoltageGainRange.getMin(), calibCcVoltageGainRange.getMax())) {
        return ErrorValueOutOfRange;

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

ErrorCodes_t MessageDispatcher::setCalibCcVoltageOffset(vector<uint16_t> channelIndexes, vector<Measurement_t> offsets, bool applyFlag){
    if (calibCcVoltageOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(offsets, calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].getMin(), calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].getMax())) {
        return ErrorValueOutOfRange;

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

ErrorCodes_t MessageDispatcher::setGateVoltagesTuner(vector<uint16_t> boardIndexes, vector<Measurement_t> gateVoltages, bool applyFlag){
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

ErrorCodes_t MessageDispatcher::setSourceVoltagesTuner(vector<uint16_t> boardIndexes, vector<Measurement_t> sourceVoltages, bool applyFlag){
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

ErrorCodes_t MessageDispatcher::digitalOffsetCompensation(vector<uint16_t> channelIndexes, vector<bool> onValues, bool applyFlag) {
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

ErrorCodes_t  MessageDispatcher::setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlagIn){
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

ErrorCodes_t  MessageDispatcher::setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlagIn){
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

ErrorCodes_t MessageDispatcher::enableStimulus(vector<uint16_t> channelIndexes, vector<bool> onValues, bool applyFlag) {
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

ErrorCodes_t MessageDispatcher::turnChannelsOn(vector<uint16_t> channelIndexes, vector<bool> onValues, bool applyFlag) {
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
        this->computeMinimumPacketNumber();
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
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
    boolConfig.bitsNum = 15;
    wordDebugCoder = new BoolArrayCoder(boolConfig);
    wordDebugCoder->encode(wordValue, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);
    delete wordDebugCoder;
    return Success;
}

ErrorCodes_t MessageDispatcher::turnVoltageReaderOn(bool onValueIn, bool applyFlagIn){
    if(onValueIn == true){
        amIinVoltageClamp = true;
    }else{
        amIinVoltageClamp = false;
    }
    setAdcFilter();
    /*
     * Still missing the actual method implementation
     */
    return Success;
}

ErrorCodes_t MessageDispatcher::turnCurrentReaderOn(bool onValueIn, bool applyFlagIn){
    if(onValueIn == true){
        amIinVoltageClamp = false;
    }else{
        amIinVoltageClamp = true;
    }
    setAdcFilter();
    /*
     * Still missing the actual method implementation
     */
    return Success;
}

/****************\
 *  Rx methods  *
\****************/

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

    unique_lock <mutex> rxMutexLock (rxMutex);
    if (rxMsgBufferReadLength <= 0) {
        rxMsgBufferNotEmpty.wait_for(rxMutexLock, chrono::milliseconds(10));
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
    uint16_t samplesNum;
    uint16_t sampleIdx;
    uint16_t timeSamplesNum;
    uint16_t rawFloat;
    uint16_t dataWritten;
    bool exitLoop = false;
    bool messageReadFlag = false;

    while (msgReadCount < maxMsgRead) {
        dataOffset = rxMsgBuffer[rxMsgBufferReadOffset].startDataPtr;
        sampleIdx = 0;
        switch (rxOutput.msgTypeId) {
        case (MsgDirectionDeviceToPc+MsgTypeIdFpgaReset):
            if (lastParsedMsgType == MsgTypeIdInvalid) {
                rxOutput.msgTypeId = rxMsgBuffer[rxMsgBufferReadOffset].typeId;
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

                for (uint16_t idx = 0; idx < timeSamplesNum; idx++) {
                    for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                        rawFloat = rxDataBuffer[dataOffset];
                        xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                        data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                        dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                    }

                    for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                        rawFloat = rxDataBuffer[dataOffset];
                        xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirVNum, iirVDen);
                        data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                        dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                    }

                    if (iirOff < 1) {
                        iirOff = IIR_ORD;

                    } else {
                        iirOff--;
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

ErrorCodes_t MessageDispatcher::getVoltageHoldTunerFeatures(RangedMeasurement_t &voltageHoldTunerFeatures){
    if (vHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else{
        voltageHoldTunerFeatures = vHoldRange;
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

ErrorCodes_t MessageDispatcher::getCalibVcCurrentOffsetFeatures(vector<RangedMeasurement_t> &calibVcCurrentOffsetFeatures){
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

ErrorCodes_t MessageDispatcher::getCalibCcVoltageOffsetFeatures(vector<RangedMeasurement_t> &calibCcVoltageOffsetFeatures){
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

ErrorCodes_t MessageDispatcher::getBoardsNumberFeatures(uint16_t &boardsNumberFeatures) {
    boardsNumberFeatures = totalBoardsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges) {
    if (vcCurrentRangesArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        currentRanges = vcCurrentRangesArray;
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

ErrorCodes_t MessageDispatcher::getVoltageStimulusLpfs(std::vector <std::string> &filterOptions){
    if(vcVoltageFiltersArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        for(uint16_t i = 0; i < vcVoltageFiltersArray.size(); i++){
            filterOptions[i] = vcVoltageFiltersArray[i].niceLabel();
        }
    }
}

ErrorCodes_t MessageDispatcher::getCurrentStimulusLpfs(std::vector <std::string> &filterOptions){
    if(ccCurrentFiltersArray.size()==0){
        return ErrorFeatureNotImplemented;
    } else {
        for(uint16_t i = 0; i < ccCurrentFiltersArray.size(); i++){
            filterOptions[i] = ccCurrentFiltersArray[i].niceLabel();
        }
    }
}

/*********************\
 *  Private methods  *
\*********************/

/*! \todo FCON questi due metodi dovrebbero cercare dispositivi con tutte le librerie di interfacciamento con device implementate (per ora c'è solo il front panel della opal kelly) */
string MessageDispatcher::getDeviceSerial(int index) {
    string serial;
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

void MessageDispatcher::initializeDevice() {
    /*! Some default values*/
    vector <bool> allTrue(currentChannelsNum, true);
    vector <bool> allFalse(currentChannelsNum, false);

    vector <uint16_t> channelIndexes(currentChannelsNum);
    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        channelIndexes[idx] = idx;
    }

    vector <uint16_t> boardIndexes(totalBoardsNum);
    for (uint16_t idx = 0; idx < totalBoardsNum; idx++) {
        boardIndexes[idx] = idx;
    }

    /*! Initialization in voltage clamp*/
    this->enableStimulus(channelIndexes, allTrue, false);
    this->turnChannelsOn(channelIndexes, allTrue, false);
    this->turnVoltageReaderOn(true, false);
    this->setVoltageHoldTuner(channelIndexes, selectedVoltageHoldVector, false);
    this->setCalibVcCurrentGain(channelIndexes, selectedCalibVcCurrentGainVector, false);
    this->setCalibVcCurrentOffset(channelIndexes, selectedCalibVcCurrentOffsetVector, false);
    this->setSamplingRate(defaultSamplingRateIdx, false);
    this->setVCCurrentRange(defaultVcCurrentRangeIdx, false);
    this->setVCVoltageRange(defaultVcVoltageRangeIdx, false);
    this->setCCCurrentRange(defaultCcCurrentRangeIdx, false);
    this->setCCVoltageRange(defaultCcVoltageRangeIdx, false);
    this->setVoltageStimulusLpf(selectedVcVoltageFilterIdx, false);
    this->setGateVoltagesTuner(boardIndexes, selectedGateVoltageVector, false);
    this->setSourceVoltagesTuner(boardIndexes, selectedSourceVoltageVector, false);
    this->digitalOffsetCompensation(channelIndexes, allFalse, false);
}

bool MessageDispatcher::checkProtocolValidity(string &message) {
    /*! \todo FCON da riempire */
    return false;
}

void MessageDispatcher::storeFrameData(uint16_t rxMsgTypeId, RxMessageTypes_t rxMessageType) {
    uint32_t rxDataWords = rxWordLengths[rxMessageType];

    if (rxEnabledTypesMap[rxMsgTypeId]) {
        /*! Update the message buffer only if the message is not filtered out */
        rxMsgBuffer[rxMsgBufferWriteOffset].typeId = rxMsgTypeId;
        rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = rxDataWords;
        rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;
        rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1) & RX_MSG_BUFFER_MASK;
    }

    for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
        rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx) & RX_DATA_BUFFER_MASK] = this->popUint16FromRxRawBuffer();
    }

    if (rxDataBufferWriteOffset <= rxDataWords) {
        rxDataBuffer[RX_DATA_BUFFER_SIZE] = rxDataBuffer[0]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word,
                                                              *   even if the first 16bit word is in position FTD_RX_DATA_BUFFER_SIZE-1 and the following one would go out of range otherwise */
    }

    if (rxEnabledTypesMap[rxMsgTypeId]) {
        /*! change the message buffer length only if the message is not filtered out */
        unique_lock <mutex> rxMutexLock(rxMutex);
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
    }
}

void MessageDispatcher::storeDataHeaderFrame() {

}

void MessageDispatcher::storeDataTailFrame() {

}

void MessageDispatcher::storeStatusFrame() {

}

void MessageDispatcher::storeVoltageOffsetFrame() {

}

void MessageDispatcher::stackOutgoingMessage(vector <uint16_t> &txDataMessage) {
    if (txModifiedEndingWord > txModifiedStartingWord) {
        unique_lock <mutex> txMutexLock (txMutex);
        while (txMsgBufferReadLength >= TX_MSG_BUFFER_SIZE) {
            txMsgBufferNotFull.wait(txMutexLock);
        }

        /*! The next 2 lines ensure that words are written in blocks of 32 bits in case any device libraries require it */
        txModifiedStartingWord = (txModifiedStartingWord/2)*2; /*! Round to the biggest smaller even number */
        txModifiedEndingWord = (txModifiedEndingWord/2)*2+2; /*! +1 Round to the smallest bigger odd number; another +1 because slicing does not include the last item */

        txMsgBuffer[txMsgBufferWriteOffset] = {txDataMessage.begin()+txModifiedStartingWord, txDataMessage.begin()+txModifiedEndingWord};
        txMsgOffsetWord[txMsgBufferWriteOffset] = txModifiedStartingWord;
        txMsgLength[txMsgBufferWriteOffset] = txModifiedEndingWord-txModifiedStartingWord;

        txModifiedStartingWord = txDataWords;
        txModifiedEndingWord = 0;

        txMsgBufferWriteOffset = (txMsgBufferWriteOffset+1)&TX_MSG_BUFFER_MASK;
        txMsgBufferReadLength++;

        txMsgBufferNotEmpty.notify_all();
    }
}

uint16_t MessageDispatcher::popUint16FromRxRawBuffer() {
    uint16_t value = 0;

    for (unsigned int byteIdx = 0; byteIdx < RX_WORD_SIZE; byteIdx++) {
        value <<= 8;
        value += rxRawBuffer[(rxRawBufferReadOffset+byteIdx) & rxRawBufferMask];
    }
    rxRawBufferReadOffset = (rxRawBufferReadOffset+RX_WORD_SIZE) & rxRawBufferMask;
    rxRawBufferReadLength -= RX_WORD_SIZE;
    return value;
}

uint16_t MessageDispatcher::readUint16FromRxRawBuffer(uint32_t n) {
    uint16_t value = 0;

    for (unsigned int byteIdx = 0; byteIdx < RX_WORD_SIZE; byteIdx++) {
        value <<= 8;
        value += rxRawBuffer[(rxRawBufferReadOffset+byteIdx+n) & rxRawBufferMask];
    }
    return value;
}

void MessageDispatcher::computeMinimumPacketNumber() {
    double samplingRateInHz = samplingRate.getNoPrefixValue();
    minPacketsNumber = (unsigned long)ceil(RX_FEW_PACKETS_COEFF*samplingRateInHz);
    minPacketsNumber = (unsigned long)min(minPacketsNumber, (unsigned long)ceil(((double)RX_MAX_BYTES_TO_WAIT_FOR)/(double)maxInputDataLoadSize));
    fewPacketsSleepUs = (unsigned int)ceil(((double)(minPacketsNumber*(unsigned long)packetsPerFrame))/samplingRateInHz*1.0e6);
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
    if (rawDataFilterActiveFlag && (rawDataFilterCutoffFrequency < samplingRate*0.5)) {
        rawDataFilterCutoffFrequency.convertValue(1.0/integrationStep.multiplier());
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
