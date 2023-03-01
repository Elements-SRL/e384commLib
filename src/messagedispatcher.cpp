#include "messagedispatcher.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <thread>
#include <math.h>
#include <random>
#include <algorithm>

#include "okFrontPanelDLL.h"

#include "utils.h"

using namespace std;

static unordered_map <string, DeviceTypes_t> deviceIdMapping = {
    {"opp", Device384Nanopores},
    {"pup", Device384PatchClamp}}; /*! \todo FCON queste info dovrebbero risiedere nel DB */

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

}

MessageDispatcher::~MessageDispatcher() {

}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t MessageDispatcher::init() {
    outputDataBuffer = new (std::nothrow) uint16_t * [E384CL_OUTPUT_BUFFER_SIZE];
    if (outputDataBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    outputDataBuffer[0] = new (std::nothrow) uint16_t[E384CL_OUTPUT_BUFFER_SIZE*totalChannelsNum];
    if (outputDataBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }
    for (int packetIdx = 1; packetIdx < E384CL_OUTPUT_BUFFER_SIZE; packetIdx++) {
        outputDataBuffer[packetIdx] = outputDataBuffer[0]+((int)totalChannelsNum)*packetIdx;
    }

    txMsgBuffer = new (std::nothrow) vector <uint16_t>[TX_MSG_BUFFER_SIZE];
    if (txMsgBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    txMsgOffsetWord.resize(TX_MSG_BUFFER_SIZE);
    txMsgLength.resize(TX_MSG_BUFFER_SIZE);

#ifdef DEBUG_PRINT
//    rxFid1 = fopen("outputBuffer.txt", "wb+");
//    rxFid2 = fopen("rxBuffer.txt", "wb+");
    txFid = fopen("tx.txt", "wb+");
#endif

    this->computeMinimumPacketNumber();

    return Success;
}

ErrorCodes_t MessageDispatcher::deinit() {
    if (outputDataBuffer != nullptr) {
        if (outputDataBuffer[0] != nullptr) {
            delete [] outputDataBuffer[0];
            outputDataBuffer[0] = nullptr;
        }
        delete [] outputDataBuffer;
        outputDataBuffer = nullptr;
    }

    if (txMsgBuffer != nullptr) {
        delete [] txMsgBuffer;
        txMsgBuffer = nullptr;
    }

#ifdef DEBUG_PRINT
//    fclose(rxFid1);
//    fclose(rxFid2);
    fclose(txFid);
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

    } else if (numDevs == 0) {
        deviceIds.clear();
        return ErrorNoDeviceFound;
    }

    deviceIds.clear();

    /*! Lists all serial numbers */
    for (int i = 0; i < numDevs; i++) {
        deviceIds.push_back(getDeviceSerial(i));
    }

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
        messageDispatcher = new MessageDispatcher_ePatchEL03D(deviceId);
        break;

    case Device384PatchClamp:
        messageDispatcher = new MessageDispatcher_ePatchEL03D_V01(deviceId);
        break;

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

ErrorCodes_t MessageDispatcher::connect() {
    if (connected) {
        return ErrorDeviceAlreadyConnected;
    }

    ErrorCodes_t ret;
    connected = true;

    this->init();

    /*! Calculate the LSB noise vector */
    this->initializeLsbNoise();

    /*! Allocate memory for compensations */
    this->initializeCompensations();

    stopConnectionFlag = false;

#ifdef DEBUGPRINT
    if (rxFid == nullptr) {
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
        ss << "\\\\rx.txt";
#else
        ss << "/rx.txt";
#endif

        rxFid = fopen(ss.str().c_str(), "wb");
    }

    if (txFid == nullptr) {
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
        ss << "\\\\tx.txt";
#else
        ss << "/tx.txt";
#endif

        txFid = fopen(ss.str().c_str(), "wb");
    }

    startPrintfTime = std::chrono::steady_clock::now();
#endif
#ifdef RAW_DEBUGPRINT
    if (rawRxFid == nullptr) {
#ifdef _WIN32
        string path = string(getenv("HOMEDRIVE"))+string(getenv("HOMEPATH"));
#else
        string path = string(getenv("HOME"));
#endif
        stringstream ss;

        for (int i = 0; i < path.length(); ++i) {
            if (path[i] == '\\') {
                ss << "\\\\";

            } else {
                ss << path[i];
            }
        }
#ifdef _WIN32
        ss << "\\\\rawRx.txt";
#else
        ss << "/rawRx.txt";
#endif

        rawRxFid = fopen(ss.str().c_str(), "wb");
    }
#endif

    rxThread = thread(&MessageDispatcher::readDataFromDevice, this);

    txThread = thread(&MessageDispatcher::sendCommandsToDevice, this);

    threadsStarted = true;

    this->resetFpga();

    this->resetChip(true);
    this_thread::sleep_for(chrono::milliseconds(100));
    ret = this->resetChip(false);
    if (ret != Success) {
        return ErrorConnectionChipResetFailed;
    }

    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        this->setVcCurrentOffsetDelta(idx, {0.0, UnitPfxNone, nullptr});
    }

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

#ifdef DEBUGPRINT
        if (rxFid != nullptr) {
            fclose(rxFid);
            rxFid = nullptr;
        }

        if (txFid != nullptr) {
            fclose(txFid);
            txFid = nullptr;
        }
#endif
#ifdef RAW_DEBUGPRINT
        if (rawRxFid != nullptr) {
            fclose(rawRxFid);
            rawRxFid = nullptr;
        }
#endif

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

ErrorCodes_t MessageDispatcher::setVoltageHoldTuner(vector<uint16_t> channelIndexes, vector<Measurement_t> voltages, bool applyFlag){
    if (vHoldTunerCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (!areAllTheVectorElementsInRange(voltages, vHoldRange.getMin(), vHoldRange.getMax())) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            voltages[i].convertValue(vHoldRange.prefix);
            vHoldTunerCoders[channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

    } else if (!areAllTheVectorElementsInRange(gains, vcCurrentGainRange.getMin(), vcCurrentGainRange.getMax())) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            gains[i].convertValue(vcCurrentGainRange.prefix);
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

    } else if (!areAllTheVectorElementsInRange(offsets, vcCurrentOffsetRange.getMin(), vcCurrentOffsetRange.getMax())) {
        return ErrorValueOutOfRange;

    } else {
        for(uint32_t i = 0; i < channelIndexes.size(); i++){
            offsets[i].convertValue(vcCurrentOffsetRange.prefix);
            calibVcCurrentOffsetCoders[channelIndexes[i]]->encode(offsets[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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

ErrorCodes_t MessageDispatcher::setAdcFilter(){
    // Still to be properly implemented
    if(amIinVoltageClamp){
        if (vcCurrentFilterCoder != nullptr) {
            vcCurrentFilterCoder->encode(sr2LpfVcMap[selectedSamplingRateIdx], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedVcCurrentFilterIdx = sr2LpfVcMap[selectedSamplingRateIdx];
        }
    }else{
        if (ccVoltageFilterCoder != nullptr) {
            ccVoltageFilterCoder->encode(sr2LpfCcMap[selectedSamplingRateIdx], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedCcVoltageFilterIdx = sr2LpfCcMap[selectedSamplingRateIdx];
        }
    }
}

ErrorCodes_t MessageDispatcher::setSamplingRate(uint16_t samplingRateIdx, bool applyFlagIn) {
    if (samplingRateCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (samplingRateIdx >= samplingRatesNum) {
        return ErrorValueOutOfRange;

    } else {
        samplingRateCoder->encode(samplingRateIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedSamplingRateIdx = samplingRateIdx;
        setAdcFilter();
        if (applyFlagIn) {
            this->stackOutgoingMessage(txStatus);
        }
        return Success;
    }
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
}

/****************\
 *  Rx methods  *
\****************/

ErrorCodes_t MessageDispatcher::getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges) {
    currentRanges = vcCurrentRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges) {
     voltageRanges = vcVoltageRangesArray;
     return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges) {
    currentRanges = ccCurrentRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges) {
     voltageRanges = ccVoltageRangesArray;
     return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageStimulusLpfs(std::vector <std::string> &filterOptions){
        for(uint16_t i = 0; i < vcVoltageFiltersArray.size(); i++){
            filterOptions[i] = vcVoltageFiltersArray[i].niceLabel();
        }
}

ErrorCodes_t MessageDispatcher::getCurrentStimulusLpfs(std::vector <std::string> &filterOptions){
        for(uint16_t i = 0; i < ccCurrentFiltersArray.size(); i++){
            filterOptions[i] = ccCurrentFiltersArray[i].niceLabel();
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

void MessageDispatcher::storeDataLoadFrame() {
    uint16_t value;

    for (int packetIdx = 0; packetIdx < packetsPerFrame; packetIdx++) {
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            value = 0;

            for (unsigned int byteIdx = 0; byteIdx < RX_WORD_SIZE; byteIdx++) {
                value <<= 8;
                value += *(rxRawBuffer+rxRawBufferReadOffset);
                rxRawBufferReadOffset = (rxRawBufferReadOffset+1)&rxRawBufferMask;
            }

            outputDataBuffer[outputBufferWriteOffset][idx] = value;
        }

        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            value = 0;

            for (unsigned int byteIdx = 0; byteIdx < RX_WORD_SIZE; byteIdx++) {
                value <<= 8;
                value += *(rxRawBuffer+rxRawBufferReadOffset);
                rxRawBufferReadOffset = (rxRawBufferReadOffset+1)&rxRawBufferMask;
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
        txModifiedEndingWord = (txModifiedEndingWord/2)*2+1; /*! Round to the smallest bigger odd number */

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

}
