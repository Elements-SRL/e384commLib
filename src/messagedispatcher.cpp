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

#ifdef DEBUG_PRINT
//    rxFid1 = fopen("outputBuffer.txt", "wb+");
//    rxFid2 = fopen("rxBuffer.txt", "wb+");
    txFid = fopen("tx.txt", "wb+");
#endif

    this->computeMinimumPacketNumber();

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

ErrorCodes_t MessageDispatcher::getNextMessage(RxOutput_t &rxOutput) {
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
    bool headerSet = false;
    bool tailSet = false;

    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged */
    bool exitLoop = false;

    while (msgReadCount < maxMsgRead) {
        if (rxOutput.msgTypeId == rxMsgBuffer[rxMsgBufferReadOffset].typeId) {
            switch (rxOutput.msgTypeId) {
            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionHeader):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionData):
                if ((msgReadCount+1)*rxDataMessageMaxLen > E4GCL_OUT_STRUCT_DATA_LEN) {
                    /*! Exit if the merge may exceed the data field length */
                    exitLoop = true;

                } else {
                    exitLoop = false;
                }
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionTail):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdFpgaReset):
            case (MsgDirectionDeviceToEdr+MsgTypeIdDigitalOffsetComp):
            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionSaturation):
            default:
                /*! Messages of these types can't be merged. */
                if (msgReadCount == 0) {
                    /*! If no messages have been read yet process it... */
                    exitLoop = false;

                } else {
                    /*! otherwise exit and force the beginning of another packet */
                    exitLoop = true;
                }
                break;
            }

        } else {
            /*! If new message type is different from previous one stay and read only if no other messages have been read yet */
            if (msgReadCount > 0) {
                /*! Otherwise exit or messages of different types will be merged */
                exitLoop = true;
            }
        }

        if (exitLoop) {
            break;

        } else {
            rxOutput.msgTypeId = rxMsgBuffer[rxMsgBufferReadOffset].typeId;
            uint32_t dataOffset = rxMsgBuffer[rxMsgBufferReadOffset].startDataPtr;
            uint16_t samplesNum;
            uint16_t sampleIdx = 0;
            uint16_t timeSamplesNum;
            uint16_t rawFloat;
            uint16_t dataWritten;

            switch (rxOutput.msgTypeId) {
            case (MsgDirectionDeviceToPc+MsgTypeIdFpgaReset):
                lastParsedMsgType = MsgTypeIdFpgaReset;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp):
                rxOutput.dataLen = 1;
                rxOutput.channelIdx = * (rxDataBuffer+dataOffset);
                rawFloat = * (rxDataBuffer+((dataOffset+1)&FTD_RX_DATA_BUFFER_MASK));
                voltageOffsetCorrected = (((double)rawFloat)-liquidJunctionOffsetBinary)*liquidJunctionResolution;
                this->selectVoltageOffsetResolution();
                rxOutput.data[0] = (int16_t)(rawFloat-liquidJunctionOffsetBinary);
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToEdr;

                this->setDigitalOffsetCompensationOverrideValue(rxOutput.channelIdx, {voltageOffsetCorrected, liquidJunctionControl.prefix, nullptr});
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
                if (lastParsedMsgType != MsgTypeIdAcquisitionHeader) {
                    /*! Evaluate only if it's the first repetition */
                    rxOutput.dataLen = 0;
                    rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                    rxOutput.protocolItemIdx = * (rxDataBuffer+((dataOffset+1)&FTD_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolRepsIdx = * (rxDataBuffer+((dataOffset+2)&FTD_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolSweepIdx = * (rxDataBuffer+((dataOffset+3)&FTD_RX_DATA_BUFFER_MASK));
                    lastParsedMsgType = MsgTypeIdAcquisitionHeader;
                    headerSet = true;

                } else {
                    if (!headerSet) {
                        /*! This message is a repetition and should not be passed to SW only if the first header message was not processed (headerSet) during this call */
                        ret = ErrorRepeatedHeader;
                    }
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData):
                if (rxMsgBuffer[rxMsgBufferReadOffset].dataLength < FTD_RX_MIN_DATA_PACKET_VALID_LEN) {
                    ret = ErrorIllFormedMessage;

                } else {
                    samplesNum = rxMsgBuffer[rxMsgBufferReadOffset].dataLength-2; /*!< The first 2 samples are used for firstSampleOffset */
                    dataWritten = rxOutput.dataLen;
                    rxOutput.dataLen += samplesNum;
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    if (msgReadCount == 0) {
                        /*! Update firstSampleOffset only if it is not merging messages */
                        rxOutput.firstSampleOffset = * ((uint32_t *)(rxDataBuffer+dataOffset));
                    }
                    dataOffset = (dataOffset+2)&FTD_RX_DATA_BUFFER_MASK;

                    for (uint16_t idx = 0; idx < timeSamplesNum; idx++) {
                        /*! \todo FCON questo doppio ciclo va modificato per raccogliere i dati di impedenza in modalità lock-in */
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyVoltageFilter(voltageChannelIdx, ((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx]);
                            xFlt = iirY[voltageChannelIdx][iirOff];
                            rxOutput.data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&FTD_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&LSB_NOISE_ARRAY_MASK;
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyCurrentFilter(currentChannelIdx+voltageChannelsNum, ((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx]);
                            xFlt = iirY[currentChannelIdx+voltageChannelsNum][iirOff];
                            rxOutput.data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&FTD_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&LSB_NOISE_ARRAY_MASK;
                        }

                        if (iirOff < 1) {
                            iirOff = IIR_ORD;

                        } else {
                            iirOff--;
                        }
                    }
                }
                lastParsedMsgType = MsgTypeIdAcquisitionData;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
                if (lastParsedMsgType != MsgTypeIdAcquisitionTail) {
                    /*! Evaluate only if it's the first repetition */
                    rxOutput.dataLen = 0;
                    rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                    lastParsedMsgType = MsgTypeIdAcquisitionTail;
                    tailSet = true;

                } else {
                    if (!tailSet) {
                        /*! This message is a repetition and should not be passed to SW only if the first tail message was not processed (tailSet) during this call */
                        ret = ErrorRepeatedTail;
                    }
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
                rxOutput.dataLen = rxMsgBuffer[rxMsgBufferReadOffset].dataLength;
                for (uint16_t dataIdx = 0; dataIdx < rxOutput.dataLen; dataIdx++) {
                    rxOutput.uintData[dataIdx] = * (rxDataBuffer+dataOffset);
                    dataOffset = (dataOffset+1)&FTD_RX_DATA_BUFFER_MASK;
                }
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToEdr;
                break;

            default:
                lastParsedMsgType = MsgTypeIdInvalid;
                break;
            }

            msgReadCount++;
            rxMsgBufferReadOffset = (rxMsgBufferReadOffset+1)&FTD_RX_MSG_BUFFER_MASK;
        }
    }

    rxMutexLock.lock();
    rxMsgBufferReadLength -= msgReadCount;
    rxMsgBufferNotFull.notify_all();
    rxMutexLock.unlock();

    return ret;
}

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
        rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx) & RX_DATA_BUFFER_MASK] = popUint16FromRxRawBuffer();
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
