#include "e384commlib_labview.h"

#include <algorithm>

#include "messagedispatcher.h"

#ifdef _WIN32
#include <windows.h>
#endif

static MessageDispatcher * messageDispatcher = nullptr;
static std::vector <Measurement_t> internalMeasVec;
static std::vector <RangedMeasurement_t> internalRangeVec;

/*! Private functions prototypes */

static void input2String(LStrHandle i, std::string &s);
static void input2Measurement(LVMeasurement_t i, Measurement_t &m);
static void input2VectorMeasurement(LMeasHandle i, std::vector <Measurement_t> &m);

static void string2Output(std::string s, LStrHandle * o);
static void measurement2Output(Measurement_t m, LVMeasurement_t &o);
static void measurement2Output(Measurement_t m, LVMeasurement_t * &o);
static void rangedMeasurement2Output(RangedMeasurement_t r, LVRangedMeasurement_t &o);
static void compensationControl2Output(CompensationControl_t c, LVCompensationControl_t &o);
static void vectorString2Output(std::vector <std::string> v, LStrHandle * o);
static void vectorMeasurement2Output(std::vector <Measurement_t> v, LMeasHandle * o);
static void calibrationRanges2Output(CalibrationRanges_t ranges, LVecMeasHandle * o);
static void calibrationRanges2MatrixMeasurement(CalibrationRanges_t ranges, std::vector <std::vector <Measurement_t> > &v);
static void vectorRangedMeasurement2Output(std::vector <RangedMeasurement_t> v, LRangeHandle * o);

template <typename I_t, typename O_t> void numericVector2Output(I_t v, O_t * o);
template <typename I_t, typename O_t> void input2NumericVector(I_t * v, O_t &o, int inputLength);

static ErrorCodes_t turnCompensationOn(uint16_t * channelIndexesIn, bool * onValuesIn, bool applyFlagIn, int vectorLengthIn, MessageDispatcher::CompensationTypes_t type);
static ErrorCodes_t setCompensationOptions(uint16_t * channelIndexesIn, uint16_t * optionIndexesIn, bool applyFlagIn, int vectorLengthIn, MessageDispatcher::CompensationTypes_t type);
static ErrorCodes_t setCompensationValues(uint16_t * channelIndexesIn, double * channelValuesIn, bool applyFlagIn, int vectorLengthIn, MessageDispatcher::CompensationUserParams_t param);
static ErrorCodes_t setCompensationRanges(uint16_t * channelIndexesIn, uint16_t * channelRangesIn, bool applyFlagIn, int vectorLengthIn, MessageDispatcher::CompensationUserParams_t param);
static ErrorCodes_t getCompensationOptions(LStrHandle * optionsOut, MessageDispatcher::CompensationTypes_t type);
static ErrorCodes_t getCompensationControl(LVCompensationControl_t &controlOut, MessageDispatcher::CompensationUserParams_t param);
static ErrorCodes_t getCompensationValues(uint16_t * channelIndexesIn, double * channelValuesOut, bool * activeNotActiveOut, int vectorLengthIn, MessageDispatcher::CompensationTypes_t type, MessageDispatcher::CompensationUserParams_t param);

#ifdef HELPERLOG
#include <fstream>
#include <iostream>

static std::fstream logFile;
#endif

#define SELECT_MEAS_VECTOR(o, i, l) {\
    if (i == nullptr) {\
    if (l != internalMeasVec.size()) {\
    return ErrorWrongInputLength;\
    }\
    o = internalMeasVec; \
    } else { \
    input2VectorMeasurement(* i, o); \
    } \
    }

ErrorCodes_t getMeasVecSize(
        uint16_t &size) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

#ifdef HELPERLOG
    logFile << "getting size" << std::endl;
#endif
    size = internalMeasVec.size();
#ifdef HELPERLOG
    logFile << "done, size: " << internalMeasVec.size() << std::endl;
#endif
    return Success;
}


ErrorCodes_t measVecResize(
        uint16_t &size) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    internalMeasVec.resize(size);
    return Success;
}

ErrorCodes_t measVecClear() {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

#ifdef HELPERLOG
    logFile << "clearing" << std::endl;
#endif
    internalMeasVec.clear();
#ifdef HELPERLOG
    logFile << "done, size: " << internalMeasVec.size() << std::endl;
#endif

    return Success;
}

ErrorCodes_t appendMeasToVec(
        double value,
        UnitPfx_t prefix,
        char * unitIn) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

#ifdef HELPERLOG
    logFile << "creating meas" << std::endl;
#endif
    Measurement_t meas = {value, prefix, std::string(unitIn)};
#ifdef HELPERLOG
    logFile << "appending meas: {" << meas.value << ", " << meas.prefix << ", " << meas.unit << std::endl;
#endif
    internalMeasVec.push_back(meas);
#ifdef HELPERLOG
    logFile << "done, measVec[" <<
               internalMeasVec.size()-1 << "]: {" <<
               internalMeasVec.back().value << ", " <<
               internalMeasVec.back().prefix << ", " <<
               internalMeasVec.back().unit << std::endl;
#endif
    return Success;
}

ErrorCodes_t setMeasInVec(
        uint16_t idx,
        double value,
        UnitPfx_t prefix,
        char * unitIn) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    if (idx >= internalMeasVec.size()) {
        return ErrorValueOutOfRange;
    }
    Measurement_t meas = {value, prefix, std::string(unitIn)};
    internalMeasVec[idx]  = meas;
    return Success;
}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t detectDevices(
        LStrHandle * deviceIdsOut) {

    std::vector <std::string> deviceIds;

    MessageDispatcher::detectDevices(deviceIds);

    vectorString2Output(deviceIds, deviceIdsOut);

    return Success;
}

ErrorCodes_t connectDevice(
        LStrHandle deviceIdIn,
        LStrHandle fwPathIn) {

    if (messageDispatcher != nullptr) {
        return ErrorDeviceAlreadyConnected;
    }

    std::string deviceId;
    std::string fwPath;
    input2String(deviceIdIn, deviceId);
    input2String(fwPathIn, fwPath);

    internalMeasVec.clear();
    internalRangeVec.clear();
#ifdef HELPERLOG
    logFile.open("C:\\log.txt", std::ios_base::trunc | std::ios_base::trunc);
#endif

    return MessageDispatcher::connectDevice(deviceId, messageDispatcher, fwPath);
}

ErrorCodes_t disconnectDevice() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

#ifdef HELPERLOG
    logFile.close();
#endif
    ErrorCodes_t ret = messageDispatcher->disconnectDevice();
    if (ret == Success) {
        delete messageDispatcher;
        messageDispatcher = nullptr;
    }
    return ret;
}

ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    return messageDispatcher->enableRxMessageType(messageType, flag);
}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t turnVoltageStimulusOn(
        bool onValueIn,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->turnVoltageStimulusOn(onValueIn, applyFlagIn);
}

ErrorCodes_t turnCurrentStimulusOn(
        bool onValueIn,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->turnCurrentStimulusOn(onValueIn, applyFlagIn);
}

ErrorCodes_t turnVoltageReaderOn(
        bool onValueIn,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->turnVoltageReaderOn(onValueIn, applyFlagIn);
}

ErrorCodes_t turnCurrentReaderOn(
        bool onValueIn,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->turnCurrentReaderOn(onValueIn, applyFlagIn);
}

ErrorCodes_t setChannelsSources(
        int16_t voltageSourcesIdx,
        int16_t currentSourcesIdx) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    ErrorCodes_t ret = messageDispatcher->setSourceForVoltageChannel(voltageSourcesIdx, false);
    if (ret == Success) {
        ret = messageDispatcher->setSourceForCurrentChannel(currentSourcesIdx, true);
    }
    return ret;
}

ErrorCodes_t setVoltageHoldTuner(
        uint16_t * channelIndexesIn,
        LMeasHandle * voltagesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> voltages;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(voltages, voltagesIn, channelIndexes.size())
    return messageDispatcher->setVoltageHoldTuner(channelIndexes, voltages, applyFlagIn);
}

ErrorCodes_t setCurrentHoldTuner(
        uint16_t * channelIndexesIn,
        LMeasHandle * currentsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> currents;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(currents, currentsIn, channelIndexes.size())
    return messageDispatcher->setCurrentHoldTuner(channelIndexes, currents, applyFlagIn);
}

ErrorCodes_t setVoltageHalf(
        uint16_t * channelIndexesIn,
        LMeasHandle * voltagesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> voltages;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(voltages, voltagesIn, channelIndexes.size())
    return messageDispatcher->setVoltageHalf(channelIndexes, voltages, applyFlagIn);
}

ErrorCodes_t setCurrentHalf(
        uint16_t * channelIndexesIn,
        LMeasHandle * currentsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> currents;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(currents, currentsIn, channelIndexes.size())
    return messageDispatcher->setCurrentHalf(channelIndexes, currents, applyFlagIn);
}

ErrorCodes_t subtractLiquidJunctionFromCc(bool flag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->subtractLiquidJunctionFromCc(flag);
}

ErrorCodes_t setLiquidJunctionVoltage(
        uint16_t * channelIndexesIn,
        LMeasHandle * voltagesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> voltages;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(voltages, voltagesIn, channelIndexes.size())
    return messageDispatcher->setLiquidJunctionVoltage(channelIndexes, voltages, applyFlagIn);
}

ErrorCodes_t resetLiquidJunctionVoltage(
        uint16_t * channelIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    return messageDispatcher->resetLiquidJunctionVoltage(channelIndexes, applyFlagIn);
}

ErrorCodes_t setCalibVcCurrentGain(
        uint16_t * channelIndexesIn,
        LMeasHandle * gainsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> gains;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(gains, gainsIn, channelIndexes.size())
    return messageDispatcher->setCalibVcCurrentGain(channelIndexes, gains, applyFlagIn);
}

ErrorCodes_t setCalibVcCurrentOffset(
        uint16_t * channelIndexesIn,
        LMeasHandle * offsetsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> offsets;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(offsets, offsetsIn, channelIndexes.size())
    return messageDispatcher->setCalibVcCurrentOffset(channelIndexes, offsets, applyFlagIn);
}

ErrorCodes_t setCalibVcVoltageGain(
        uint16_t * channelIndexesIn,
        LMeasHandle * gainsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> gains;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(gains, gainsIn, channelIndexes.size())
    return messageDispatcher->setCalibVcVoltageGain(channelIndexes, gains, applyFlagIn);
}

ErrorCodes_t setCalibVcVoltageOffset(
        uint16_t * channelIndexesIn,
        LMeasHandle * offsetsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> offsets;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(offsets, offsetsIn, channelIndexes.size())
    return messageDispatcher->setCalibVcVoltageOffset(channelIndexes, offsets, applyFlagIn);
}

ErrorCodes_t setCalibCcVoltageGain(
        uint16_t * channelIndexesIn,
        LMeasHandle * gainsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> gains;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(gains, gainsIn, channelIndexes.size())
    return messageDispatcher->setCalibCcVoltageGain(channelIndexes, gains, applyFlagIn);
}

ErrorCodes_t setCalibCcVoltageOffset(
        uint16_t * channelIndexesIn,
        LMeasHandle * offsetsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> offsets;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(offsets, offsetsIn, channelIndexes.size())
    return messageDispatcher->setCalibCcVoltageOffset(channelIndexes, offsets, applyFlagIn);
}

ErrorCodes_t setCalibCcCurrentGain(
        uint16_t * channelIndexesIn,
        LMeasHandle * gainsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> gains;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(gains, gainsIn, channelIndexes.size())
    return messageDispatcher->setCalibCcCurrentGain(channelIndexes, gains, applyFlagIn);
}

ErrorCodes_t setCalibCcCurrentOffset(
        uint16_t * channelIndexesIn,
        LMeasHandle * offsetsIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> offsets;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(offsets, offsetsIn, channelIndexes.size())
    return messageDispatcher->setCalibCcCurrentOffset(channelIndexes, offsets, applyFlagIn);
}

ErrorCodes_t writeCalibrationEeprom(
        uint32_t * valueIn,
        uint32_t * addressIn,
        uint32_t * sizeIn,
        uint32_t vectorLength) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint32_t> value;
    std::vector <uint32_t> address;
    std::vector <uint32_t> size;
    input2NumericVector(valueIn, value, vectorLength);
    input2NumericVector(addressIn, address, vectorLength);
    input2NumericVector(sizeIn, size, vectorLength);
    return messageDispatcher->writeCalibrationEeprom(value, address, size);
}

ErrorCodes_t setGateVoltage(
        uint16_t * boardIndexesIn,
        LMeasHandle * gateVoltagesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> boardIndexes;
    std::vector <Measurement_t> gateVoltages;
    input2NumericVector(boardIndexesIn, boardIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(gateVoltages, gateVoltagesIn, boardIndexes.size())
    return messageDispatcher->setGateVoltages(boardIndexes, gateVoltages, applyFlagIn);
}

ErrorCodes_t setSourceVoltage(
        uint16_t * boardIndexesIn,
        LMeasHandle * sourceVoltagesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> boardIndexes;
    std::vector <Measurement_t> sourceVoltages;
    input2NumericVector(boardIndexesIn, boardIndexes, vectorLengthIn);
    SELECT_MEAS_VECTOR(sourceVoltages, sourceVoltagesIn, boardIndexes.size())
    return messageDispatcher->setSourceVoltages(boardIndexes, sourceVoltages, applyFlagIn);
}

ErrorCodes_t setVCCurrentRange(
        uint16_t currentRangeIdx,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setVCCurrentRange(currentRangeIdx, applyFlagIn);
}

ErrorCodes_t setCCCurrentRange(
        uint16_t currentRangeIdx,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setCCCurrentRange(currentRangeIdx, applyFlagIn);
}

ErrorCodes_t setVCVoltageRange(
        uint16_t voltageRangeIdx,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setVCVoltageRange(voltageRangeIdx, applyFlagIn);
}

ErrorCodes_t setCCVoltageRange(
        uint16_t voltageRangeIdx,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setCCVoltageRange(voltageRangeIdx, applyFlagIn);
}

ErrorCodes_t setSamplingRate(
        uint16_t samplingRateIdx,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setSamplingRate(samplingRateIdx, applyFlagIn);
}

ErrorCodes_t setDownsamplingRatio(
        uint32_t ratio) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setDownsamplingRatio(ratio);
}

ErrorCodes_t setDigitalFilter(
        LVMeasurement_t cutoffFrequencyIn,
        bool lowPassFlag,
        bool activeFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t cutoffFrequency;
    input2Measurement(cutoffFrequencyIn, cutoffFrequency);
    return messageDispatcher->setRawDataFilter(cutoffFrequency, lowPassFlag, activeFlag);
}

ErrorCodes_t setDigitalFilterVec(
        LMeasHandle * vecIn,
        bool lowPassFlag,
        bool activeFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 1)
    return messageDispatcher->setRawDataFilter(vec[0], lowPassFlag, activeFlag);
}

ErrorCodes_t readoutOffsetRecalibration(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <bool> onValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(onValuesIn, onValues, vectorLengthIn);
    return messageDispatcher->readoutOffsetRecalibration(channelIndexes, onValues, applyFlagIn);
}

ErrorCodes_t liquidJunctionCompensation(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <bool> onValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(onValuesIn, onValues, vectorLengthIn);
    return messageDispatcher->liquidJunctionCompensation(channelIndexes, onValues, applyFlagIn);
}

ErrorCodes_t digitalOffsetCompensation(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return liquidJunctionCompensation(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn);
}

ErrorCodes_t zap(
        LVMeasurement_t durationIn,
        uint16_t * channelIndexesIn,
        uint16_t vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    Measurement_t duration;
    input2Measurement(durationIn, duration);
    return messageDispatcher->zap(channelIndexes, duration);
}

ErrorCodes_t zap(
        LMeasHandle * vecIn,
        uint16_t * channelIndexesIn,
        uint16_t vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 1)
    return messageDispatcher->zap(channelIndexes, vec[0]);
}

ErrorCodes_t setVoltageStimulusLpf(
        uint16_t filterIdx,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setVoltageStimulusLpf(filterIdx, applyFlagIn);
}

ErrorCodes_t setCurrentStimulusLpf(
        uint16_t filterIdx,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setCurrentStimulusLpf(filterIdx, applyFlagIn);
}

ErrorCodes_t enableStimulus(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <bool> onValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(onValuesIn, onValues, vectorLengthIn);
    return messageDispatcher->enableStimulus(channelIndexes, onValues, applyFlagIn);
}

ErrorCodes_t turnChannelsOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <bool> onValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(onValuesIn, onValues, vectorLengthIn);
    return messageDispatcher->turnChannelsOn(channelIndexes, onValues, applyFlagIn);
}

ErrorCodes_t turnCalSwOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <bool> onValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(onValuesIn, onValues, vectorLengthIn);
    return messageDispatcher->turnCalSwOn(channelIndexes, onValues, applyFlagIn);
}

ErrorCodes_t enableCcStimulus(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <bool> onValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(onValuesIn, onValues, vectorLengthIn);
    return messageDispatcher->enableCcStimulus(channelIndexes, onValues, applyFlagIn);
}

ErrorCodes_t setClampingModality(
        ClampingModality_t clampingModalityIndex,
        bool applyFlagIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setClampingModality(clampingModalityIndex, applyFlagIn, true);
}

ErrorCodes_t turnVoltageCompensationsOn(
        bool onValue
        ) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->enableVcCompensations(onValue, true);
}

ErrorCodes_t turnCurrentCompensationsOn(
        bool onValue) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->enableCcCompensations(onValue, true);
}

ErrorCodes_t turnPipetteCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompCfast);
}

ErrorCodes_t turnCCPipetteCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompCcCfast);
}

ErrorCodes_t turnMembraneCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompCslow);
}

ErrorCodes_t turnAccessResistanceCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompRsComp);
}

ErrorCodes_t turnAccessResistanceCorrectionOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompRsCorr);
}

ErrorCodes_t turnAccessResistancePredictionOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompRsPred);
}

ErrorCodes_t turnLeakConductanceCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompGLeak);
}

ErrorCodes_t turnBridgeBalanceCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return turnCompensationOn(channelIndexesIn, onValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompBridgeRes);
}

ErrorCodes_t setPipetteCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompCfast);
}

ErrorCodes_t setCCPipetteCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompCcCfast);
}

ErrorCodes_t setMembraneCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompCslow);
}

ErrorCodes_t setAccessResistanceCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompRsComp);
}

ErrorCodes_t setAccessResistanceCorrectionOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompRsCorr);
}

ErrorCodes_t setAccessResistancePredictionOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompRsPred);
}

ErrorCodes_t setLeakConductanceCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompGLeak);
}

ErrorCodes_t setBridgeBalanceCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationOptions(channelIndexesIn, optionIndexesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::CompBridgeRes);
}

ErrorCodes_t setPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_CpVc);
}

ErrorCodes_t setCCPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_CpCc);
}

ErrorCodes_t setMembraneCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_Cm);
}

ErrorCodes_t setAccessResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_Rs);
}

ErrorCodes_t setAccessResistanceCorrectionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsCp);
}

ErrorCodes_t setAccessResistanceCorrectionLag(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsCl);
}

ErrorCodes_t setAccessResistancePredictionGain(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsPg);
}

ErrorCodes_t setAccessResistancePredictionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsPp);
}

ErrorCodes_t setAccessResistancePredictionTau(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsPt);
}

ErrorCodes_t setLeakConductance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_LkG);
}

ErrorCodes_t setBridgeBalanceResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationValues(channelIndexesIn, channelValuesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_BrB);
}

ErrorCodes_t setPipetteCapacitanceRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_CpVc);
}

ErrorCodes_t setCCPipetteCapacitanceRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_CpCc);
}

ErrorCodes_t setMembraneCapacitanceRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_Cm);
}

ErrorCodes_t setAccessResistanceRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_Rs);
}

ErrorCodes_t setAccessResistanceCorrectionPercentageRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsCp);
}

ErrorCodes_t setAccessResistanceCorrectionLagRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsCl);
}

ErrorCodes_t setAccessResistancePredictionGainRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsPg);
}

ErrorCodes_t setAccessResistancePredictionPercentageRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsPp);
}

ErrorCodes_t setAccessResistancePredictionTauRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_RsPt);
}

ErrorCodes_t setLeakConductanceRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_LkG);
}

ErrorCodes_t setBridgeBalanceResistanceRange(
        uint16_t * channelIndexesIn,
        uint16_t * channelRangesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    return setCompensationRanges(channelIndexesIn, channelRangesIn, applyFlagIn, vectorLengthIn, MessageDispatcher::U_BrB);
}

ErrorCodes_t setVoltageProtocolStructure(uint16_t protId,
                                         uint16_t itemsNum,
                                         uint16_t sweepsNum,
                                         LVMeasurement_t vRestIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t vRest;
    input2Measurement(vRestIn, vRest);
    return messageDispatcher->setVoltageProtocolStructure(protId, itemsNum, sweepsNum, vRest, true);
}

ErrorCodes_t setVoltageProtocolStructure(uint16_t protId,
                                         uint16_t itemsNum,
                                         uint16_t sweepsNum,
                                         LMeasHandle * vecIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 1)
    return messageDispatcher->setVoltageProtocolStructure(protId, itemsNum, sweepsNum, vec[0], true);
}

ErrorCodes_t voltStepTimeStep(
        LVMeasurement_t v0In,
        LVMeasurement_t vStepIn,
        LVMeasurement_t t0In,
        LVMeasurement_t tStepIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t v0;
    Measurement_t vStep;
    Measurement_t t0;
    Measurement_t tStep;
    input2Measurement(v0In, v0);
    input2Measurement(vStepIn, vStep);
    input2Measurement(t0In, t0);
    input2Measurement(tStepIn, tStep);
    return messageDispatcher->setVoltageProtocolStep(currentItem, nextItem, repsNum, applySteps, v0, vStep, t0, tStep, vHalfFlag);
}

ErrorCodes_t voltStepTimeStepVec(
        LMeasHandle * vecIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 4)
    return messageDispatcher->setVoltageProtocolStep(currentItem, nextItem, repsNum, applySteps, vec[0], vec[1], vec[2], vec[3], vHalfFlag);
}

ErrorCodes_t voltRamp(
        LVMeasurement_t v0In,
        LVMeasurement_t vFinalIn,
        LVMeasurement_t tIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t v0;
    Measurement_t vFinal;
    Measurement_t t;
    input2Measurement(v0In, v0);
    input2Measurement(vFinalIn, vFinal);
    input2Measurement(tIn, t);
    Measurement_t vStep = v0*0.0;
    Measurement_t tStep = t*0.0;
    return messageDispatcher->setVoltageProtocolRamp(currentItem, nextItem, repsNum, applySteps, v0, vStep, vFinal, vStep, t, tStep, vHalfFlag);
}

ErrorCodes_t voltRampVec(
        LMeasHandle * vecIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 3)
    return messageDispatcher->setVoltageProtocolRamp(currentItem, nextItem, repsNum, applySteps, vec[0], vec[0]*0.0, vec[1], vec[1]*0.0, vec[2], vec[2]*0.0, vHalfFlag);
}

ErrorCodes_t voltSin(
        LVMeasurement_t v0In,
        LVMeasurement_t vAmpIn,
        LVMeasurement_t freqIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t v0;
    Measurement_t vAmp;
    Measurement_t freq;
    input2Measurement(v0In, v0);
    input2Measurement(vAmpIn, vAmp);
    input2Measurement(freqIn, freq);
    Measurement_t vStep = v0*0.0;
    Measurement_t fStep = freq*0.0;
    return messageDispatcher->setVoltageProtocolSin(currentItem, nextItem, repsNum, applySteps, v0, vStep, vAmp, vStep, freq, fStep, vHalfFlag);
}

ErrorCodes_t voltSinVec(
        LMeasHandle * vecIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 3)
    return messageDispatcher->setVoltageProtocolSin(currentItem, nextItem, repsNum, applySteps, vec[0], vec[0]*0.0, vec[1], vec[1]*0.0, vec[2], vec[2]*0.0, vHalfFlag);
}

ErrorCodes_t startProtocol() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->startProtocol();
}

ErrorCodes_t stopProtocol() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->stopProtocol();
}

ErrorCodes_t setCurrentProtocolStructure(uint16_t protId,
                                         uint16_t itemsNum,
                                         uint16_t sweepsNum,
                                         LVMeasurement_t iRestIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t iRest;
    input2Measurement(iRestIn, iRest);
    return messageDispatcher->setCurrentProtocolStructure(protId, itemsNum, sweepsNum, iRest, true);
}

ErrorCodes_t setCurrentProtocolStructure(uint16_t protId,
                                         uint16_t itemsNum,
                                         uint16_t sweepsNum,
                                         LMeasHandle * vecIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 1)
    return messageDispatcher->setCurrentProtocolStructure(protId, itemsNum, sweepsNum, vec[0], true);
}

ErrorCodes_t currStepTimeStep(
        LVMeasurement_t i0In,
        LVMeasurement_t iStepIn,
        LVMeasurement_t t0In,
        LVMeasurement_t tStepIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t i0;
    Measurement_t iStep;
    Measurement_t t0;
    Measurement_t tStep;
    input2Measurement(i0In, i0);
    input2Measurement(iStepIn, iStep);
    input2Measurement(t0In, t0);
    input2Measurement(tStepIn, tStep);
    return messageDispatcher->setCurrentProtocolStep(currentItem, nextItem, repsNum, applySteps, i0, iStep, t0, tStep, cHalfFlag);
}

ErrorCodes_t currStepTimeStepVec(
        LMeasHandle * vecIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 4)
    return messageDispatcher->setCurrentProtocolStep(currentItem, nextItem, repsNum, applySteps, vec[0], vec[1], vec[2], vec[3], cHalfFlag);
}

ErrorCodes_t currRamp(
        LVMeasurement_t i0In,
        LVMeasurement_t iFinalIn,
        LVMeasurement_t tIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t i0;
    Measurement_t iFinal;
    Measurement_t t;
    input2Measurement(i0In, i0);
    input2Measurement(iFinalIn, iFinal);
    input2Measurement(tIn, t);
    Measurement_t iStep = i0*0.0;
    Measurement_t tStep = t*0.0;
    return messageDispatcher->setCurrentProtocolRamp(currentItem, nextItem, repsNum, applySteps, i0, iStep, iFinal, iStep, t, tStep, cHalfFlag);
}

ErrorCodes_t currRampVec(
        LMeasHandle * vecIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 3)
    return messageDispatcher->setCurrentProtocolRamp(currentItem, nextItem, repsNum, applySteps, vec[0], vec[0]*0.0, vec[1], vec[1]*0.0, vec[2], vec[2]*0.0, cHalfFlag);
}

ErrorCodes_t currSin(
        LVMeasurement_t i0In,
        LVMeasurement_t iAmpIn,
        LVMeasurement_t freqIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    Measurement_t i0;
    Measurement_t iAmp;
    Measurement_t freq;
    input2Measurement(i0In, i0);
    input2Measurement(iAmpIn, iAmp);
    input2Measurement(freqIn, freq);
    Measurement_t iStep = i0*0.0;
    Measurement_t fStep = freq*0.0;
    return messageDispatcher->setCurrentProtocolSin(currentItem, nextItem, repsNum, applySteps, i0, iStep, iAmp, iStep, freq, fStep, cHalfFlag);
}

ErrorCodes_t currSinVec(
        LMeasHandle * vecIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vec;
    SELECT_MEAS_VECTOR(vec, vecIn, 3)
    return messageDispatcher->setCurrentProtocolSin(currentItem, nextItem, repsNum, applySteps, vec[0], vec[0]*0.0, vec[1], vec[1]*0.0, vec[2], vec[2]*0.0, cHalfFlag);
}

ErrorCodes_t resetAsic(bool reset) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->resetAsic(reset);
}

ErrorCodes_t resetFpga(bool reset) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->resetFpga(reset);
}

ErrorCodes_t resetLiquidJunctionCompensation() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    uint16_t vChNum;
    uint16_t cChNum;
    messageDispatcher->getChannelNumberFeatures(vChNum, cChNum);
    std::vector <uint16_t> channelIndexes(cChNum);
    for (uint16_t chIdx = 0; chIdx < cChNum; cChNum++) {
        channelIndexes[chIdx] = chIdx;
    }
    return messageDispatcher->resetLiquidJunctionVoltage(channelIndexes, true);
}

/****************\
 *  Rx methods  *
\****************/

ErrorCodes_t getRxDataBufferSize(
        uint32_t &size) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->getRxDataBufferSize(size);
}

ErrorCodes_t getNextMessage(
        RxOutput_t &rxOutput, int16_t* data) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->getNextMessage(rxOutput, data);
}

ErrorCodes_t getLiquidJunctionVoltages(
        uint16_t * channelIndexesIn,
        LMeasHandle * voltagesOut,
        int vectorLengthIn) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <Measurement_t> voltages;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    ErrorCodes_t ret = messageDispatcher->getLiquidJunctionVoltages(channelIndexes, voltages);
    vectorMeasurement2Output(voltages, voltagesOut);
    return ret;
}

ErrorCodes_t purgeData() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->purgeData();
}

ErrorCodes_t getChannelsNumber(
        uint32_t &currentChannelsNum,
        uint32_t &voltageChannelsNum) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    uint16_t currChanNum;
    uint16_t voltChanNum;
    ErrorCodes_t ret = messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
    currentChannelsNum = (uint32_t)currChanNum;
    voltageChannelsNum = (uint32_t)voltChanNum;
    return ret;
}

ErrorCodes_t getAvailableChannelsSources(
        ChannelSources_t &voltageSourcesIdxs,
        ChannelSources_t &currentSourcesIdxs) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->getAvailableChannelsSourcesFeatures(voltageSourcesIdxs, currentSourcesIdxs);
}

ErrorCodes_t getBoardsNumber(
        uint32_t &boardsNum) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    uint16_t borNum;
    ErrorCodes_t ret = messageDispatcher->getBoardsNumberFeatures(borNum);
    boardsNum = (uint32_t)borNum;
    return ret;
}

ErrorCodes_t getVoltageHoldTunerFeatures(
        LRangeHandle * voltageHoldTunerFeaturesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> ranges;
    ErrorCodes_t ret = messageDispatcher->getVoltageHoldTunerFeatures(ranges);
    vectorRangedMeasurement2Output(ranges, voltageHoldTunerFeaturesOut);
    return ret;
}

ErrorCodes_t getCurrentHoldTunerFeatures(
        LRangeHandle * currentHoldTunerFeaturesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> ranges;
    ErrorCodes_t ret = messageDispatcher->getCurrentHoldTunerFeatures(ranges);
    vectorRangedMeasurement2Output(ranges, currentHoldTunerFeaturesOut);
    return ret;
}

ErrorCodes_t getVoltageHalfFeatures(
        LRangeHandle * voltageHalfFeaturesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> ranges;
    ErrorCodes_t ret = messageDispatcher->getVoltageHalfFeatures(ranges);
    vectorRangedMeasurement2Output(ranges, voltageHalfFeaturesOut);
    return ret;
}

ErrorCodes_t getCurrentHalfFeatures(
        LRangeHandle * currentHalfFeaturesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> ranges;
    ErrorCodes_t ret = messageDispatcher->getCurrentHalfFeatures(ranges);
    vectorRangedMeasurement2Output(ranges, currentHalfFeaturesOut);
    return ret;
}

ErrorCodes_t getClampingModalitiesFeatures(
        uint16_t * clampingModalitiesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <ClampingModality_t> clampingModalities;
    ErrorCodes_t ret = messageDispatcher->getClampingModalitiesFeatures(clampingModalities);
    numericVector2Output<std::vector <ClampingModality_t>, uint16_t>(clampingModalities, clampingModalitiesOut);
    return ret;
}

ErrorCodes_t getVCCurrentRanges(
        LRangeHandle * currentRangesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> currentRanges;
    uint16_t _;
    ErrorCodes_t ret = messageDispatcher->getVCCurrentRanges(currentRanges, _);
    vectorRangedMeasurement2Output(currentRanges, currentRangesOut);
    return ret;
}

ErrorCodes_t getCCCurrentRanges(
        LRangeHandle * currentRangesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> currentRanges;
    uint16_t _;
    ErrorCodes_t ret = messageDispatcher->getCCCurrentRanges(currentRanges, _);
    vectorRangedMeasurement2Output(currentRanges, currentRangesOut);
    return ret;
}

ErrorCodes_t getVCCurrentRange(
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t currentRange;
    ErrorCodes_t ret = messageDispatcher->getVCCurrentRange(currentRange);
    rangedMeasurement2Output(currentRange, rangeOut);
    return ret;
}

ErrorCodes_t getCCCurrentRange(
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t currentRange;
    ErrorCodes_t ret = messageDispatcher->getCCCurrentRange(currentRange);
    rangedMeasurement2Output(currentRange, rangeOut);
    return ret;
}

ErrorCodes_t getVCVoltageRanges(
        LRangeHandle * voltageRangesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> voltageRanges;
    uint16_t _;
    ErrorCodes_t ret = messageDispatcher->getVCVoltageRanges(voltageRanges, _);
    vectorRangedMeasurement2Output(voltageRanges, voltageRangesOut);
    return ret;
}

ErrorCodes_t getCCVoltageRanges(
        LRangeHandle * voltageRangesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <RangedMeasurement_t> voltageRanges;
    uint16_t _;
    ErrorCodes_t ret = messageDispatcher->getCCVoltageRanges(voltageRanges, _);
    vectorRangedMeasurement2Output(voltageRanges, voltageRangesOut);
    return ret;
}

ErrorCodes_t getVCVoltageRange(
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t voltageRange;
    ErrorCodes_t ret = messageDispatcher->getVCVoltageRange(voltageRange);
    rangedMeasurement2Output(voltageRange, rangeOut);
    return ret;
}

ErrorCodes_t getCCVoltageRange(
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t voltageRange;
    ErrorCodes_t ret = messageDispatcher->getCCVoltageRange(voltageRange);
    rangedMeasurement2Output(voltageRange, rangeOut);
    return ret;
}

ErrorCodes_t getTemperatureChannelsInfo(
        LStrHandle * namesOut,
        LRangeHandle * rangesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <std::string> names;
    std::vector <RangedMeasurement_t> ranges;
    ErrorCodes_t ret = messageDispatcher->getTemperatureChannelsFeatures(names, ranges);
    vectorRangedMeasurement2Output(ranges, rangesOut);
    vectorString2Output(names, namesOut);
    return ret;
}

ErrorCodes_t getSamplingRates(
        LMeasHandle * samplingRatesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> samplingRates;
    ErrorCodes_t ret = messageDispatcher->getSamplingRatesFeatures(samplingRates);
    vectorMeasurement2Output(samplingRates, samplingRatesOut);
    return ret;
}

ErrorCodes_t getRealSamplingRates(
        LMeasHandle * realSamplingRatesOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> realSamplingRates;
    ErrorCodes_t ret = messageDispatcher->getRealSamplingRatesFeatures(realSamplingRates);
    vectorMeasurement2Output(realSamplingRates, realSamplingRatesOut);
    return ret;
}

ErrorCodes_t getVoltageProtocolRange(
        unsigned int rangeIdx,
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t range;
    ErrorCodes_t ret = messageDispatcher->getVoltageProtocolRangeFeature(rangeIdx, range);
    rangedMeasurement2Output(range, rangeOut);
    return ret;
}

ErrorCodes_t getCurrentProtocolRange(
        unsigned int rangeIdx,
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t range;
    ErrorCodes_t ret = messageDispatcher->getCurrentProtocolRangeFeature(rangeIdx, range);
    rangedMeasurement2Output(range, rangeOut);
    return ret;
}

ErrorCodes_t getTimeProtocolRange(
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t range;
    ErrorCodes_t ret = messageDispatcher->getTimeProtocolRangeFeature(range);
    rangedMeasurement2Output(range, rangeOut);
    return ret;
}

ErrorCodes_t getFrequencyProtocolRange(
        LVRangedMeasurement_t &rangeOut) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    RangedMeasurement_t range;
    ErrorCodes_t ret = messageDispatcher->getFrequencyProtocolRangeFeature(range);
    rangedMeasurement2Output(range, rangeOut);
    return ret;
}

ErrorCodes_t getMaxProtocolItems(
        unsigned int &maxItemsNum) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->getMaxProtocolItemsFeature(maxItemsNum);
}

ErrorCodes_t hasProtocolStep() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->hasProtocolStepFeature();
}

ErrorCodes_t hasProtocolRamp() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->hasProtocolRampFeature();
}

ErrorCodes_t hasProtocolSin() {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->hasProtocolSinFeature();
}

ErrorCodes_t getVoltageStimulusLpfs(
        LStrHandle * filterOptionsOut) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> vcVoltageFilters;
    ErrorCodes_t ret = messageDispatcher->getVCVoltageFilters(vcVoltageFilters);
    std::vector <std::string> filterOptions;
    for (uint16_t i = 0; i < vcVoltageFilters.size(); i++) {
        filterOptions[i] = vcVoltageFilters[i].niceLabel();
    }
    vectorString2Output(filterOptions, filterOptionsOut);
    return ret;
}

ErrorCodes_t getCurrentStimulusLpfs(
        LStrHandle * filterOptionsOut) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <Measurement_t> ccCurrentFilters;
    ErrorCodes_t ret = messageDispatcher->getCCCurrentFilters(ccCurrentFilters);
    std::vector <std::string> filterOptions;
    for (uint16_t i = 0; i < ccCurrentFilters.size(); i++) {
        filterOptions[i] = ccCurrentFilters[i].niceLabel();
    }
    vectorString2Output(filterOptions, filterOptionsOut);
    return ret;
}

ErrorCodes_t getPipetteCompensationOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompCfast);
}

ErrorCodes_t getCCPipetteCompensationOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompCcCfast);
}

ErrorCodes_t getMembraneCompensationOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompCslow);
}

ErrorCodes_t getAccessResistanceCompensationOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompRsComp);
}

ErrorCodes_t getAccessResistanceCorrectionOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompRsCorr);
}

ErrorCodes_t getAccessResistancePredictionOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompRsPred);
}

ErrorCodes_t getLeakConductanceCompensationOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompGLeak);
}

ErrorCodes_t getBridgeBalanceCompensationOptions(
        LStrHandle * optionsOut) {
    return getCompensationOptions(optionsOut, MessageDispatcher::CompBridgeRes);
}

ErrorCodes_t getPipetteCapacitanceControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_CpVc);
}

ErrorCodes_t getCCPipetteCapacitanceControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_CpCc);
}

ErrorCodes_t getMembraneCapacitanceControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_Cm);
}

ErrorCodes_t getAccessResistanceControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_Rs);
}

ErrorCodes_t getResistanceCorrectionPercentageControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_RsCp);
}

ErrorCodes_t getResistanceCorrectionLagControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_RsCl);
}

ErrorCodes_t getResistancePredictionGainControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_RsPg);
}

ErrorCodes_t getResistancePredictionPercentageControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_RsPp);
}

ErrorCodes_t getResistancePredictionTauControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_RsPt);
}

ErrorCodes_t getLeakConductanceControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_LkG);
}

ErrorCodes_t getBridgeBalanceResistanceControl(
        LVCompensationControl_t &controlOut) {
    return getCompensationControl(controlOut, MessageDispatcher::U_BrB);
}

ErrorCodes_t getPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompCfast, MessageDispatcher::U_CpVc);
}

ErrorCodes_t getCCPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompCcCfast, MessageDispatcher::U_CpCc);
}

ErrorCodes_t getMembraneCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompCslow, MessageDispatcher::U_Cm);
}

ErrorCodes_t getAccessResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompCslow, MessageDispatcher::U_Rs);
}

ErrorCodes_t getAccessResistanceCorrectionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompRsCorr, MessageDispatcher::U_RsCp);
}

ErrorCodes_t getAccessResistanceCorrectionLag(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompRsCorr, MessageDispatcher::U_RsCl);
}

ErrorCodes_t getAccessResistancePredictionGain(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompRsPred, MessageDispatcher::U_RsPg);
}

ErrorCodes_t getAccessResistancePredictionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompRsPred, MessageDispatcher::U_RsPp);
}

ErrorCodes_t getAccessResistancePredictionTau(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompRsPred, MessageDispatcher::U_RsPt);
}

ErrorCodes_t getLeakConductance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompGLeak, MessageDispatcher::U_LkG);
}

ErrorCodes_t getBridgeBalanceResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    return getCompensationValues(channelIndexesIn, channelValuesOut, activeNotActiveOut, vectorLengthIn, MessageDispatcher::CompBridgeRes, MessageDispatcher::U_BrB);
}

ErrorCodes_t getVcAdcGainCalibration(
        uint16_t samplingRateIdx,
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesVcGainAdc, samplingRateIdx), meas);
    return ret;
}

ErrorCodes_t getVcAdcOffsetCalibration(
        uint16_t samplingRateIdx,
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesVcOffsetAdc, samplingRateIdx), meas);
    return ret;
}

ErrorCodes_t getVcDacGainCalibration(
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesVcGainDac, 0), meas);
    return ret;
}

ErrorCodes_t getVcDacOffsetCalibration(
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesVcOffsetDac, 0), meas);
    return ret;
}

ErrorCodes_t getCcAdcGainCalibration(
        uint16_t samplingRateIdx,
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesCcGainAdc, samplingRateIdx), meas);
    return ret;
}

ErrorCodes_t getCcAdcOffsetCalibration(
        uint16_t samplingRateIdx,
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesCcOffsetAdc, samplingRateIdx), meas);
    return ret;
}

ErrorCodes_t getCcDacGainCalibration(
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesCcGainDac, 0), meas);
    return ret;
}

ErrorCodes_t getCcDacOffsetCalibration(
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesCcOffsetDac, 0), meas);
    return ret;
}

ErrorCodes_t getRsCorrDacOffsetCalibration(
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesRsCorrOffsetDac, 0), meas);
    return ret;
}

ErrorCodes_t getRsShuntConductanceCalibration(
        LVecMeasHandle * meas) {

    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }

    CalibrationParams_t calibParams;
    ErrorCodes_t ret = messageDispatcher->getCalibParams(calibParams);
    calibrationRanges2Output(calibParams.getRanges(CalTypesRShuntConductance, 0), meas);
    return ret;
}

ErrorCodes_t getCalibrationEepromSize(
        uint32_t &size) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->getCalibrationEepromSize(size);
}

ErrorCodes_t readCalibrationEeprom(
        uint32_t * valueOut,
        uint32_t * addressIn,
        uint32_t * sizeIn,
        uint32_t vectorLength) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint32_t> value;
    std::vector <uint32_t> address;
    std::vector <uint32_t> size;
    input2NumericVector(addressIn, address, vectorLength);
    input2NumericVector(sizeIn, size, vectorLength);
    ErrorCodes_t ret = messageDispatcher->writeCalibrationEeprom(value, address, size);
    numericVector2Output(value, valueOut);
    return ret;
}

ErrorCodes_t setDebugBit(
        uint16_t wordOffset,
        uint16_t bitOffset,
        bool status) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setDebugBit(wordOffset, bitOffset, status);
}

ErrorCodes_t setDebugWord(
        uint16_t wordOffset,
        uint16_t wordValue) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    return messageDispatcher->setDebugWord(wordOffset, wordValue);
}

/*! Private functions */
void input2String(LStrHandle i, std::string &s) {
    s = std::string((char *)LStrBuf(* i), LStrLen(* i));
}

void input2Measurement(LVMeasurement_t i, Measurement_t &m) {
    m.value = i.value;
    m.prefix = i.prefix;
    input2String(i.unit, m.unit);
}

void input2VectorMeasurement(LMeasHandle i, std::vector <Measurement_t> &m) {
    m.resize(LVecLen(* i));
    for (int j = 0; j < LVecLen(* i); j++) {
        input2Measurement(*LVecItem(* i, j), m[j]);
    }
}

void string2Output(std::string s, LStrHandle * o) {
    MgErr err = NumericArrayResize(uB, 1, (UHandle *)o, s.length());
    if (!err) {
        MoveBlock(s.c_str(), LStrBuf(** o), s.length());
        LStrLen(** o) = s.length();
    }
}

void measurement2Output(Measurement_t m, LVMeasurement_t &o) {
    o.value = m.value;
    o.prefix = m.prefix;
    string2Output(m.unit, &o.unit);
}

void measurement2Output(Measurement_t m, LVMeasurement_t * &o) {
    MgErr err = 0;
    if (o == nullptr) {
        o = (LVMeasurement_t *)DSNewHClr(sizeof(LVMeasurement_t));

    } else {
        err = DSSetHSzClr(o, sizeof(LVMeasurement_t));
    }
    if (!err) {
        measurement2Output(m, * o);
    }
}

void rangedMeasurement2Output(RangedMeasurement_t r, LVRangedMeasurement_t &o) {
    o.min = r.min;
    o.max = r.max;
    o.step = r.step;
    o.prefix = r.prefix;
    string2Output(r.unit, &o.unit);
}

void compensationControl2Output(CompensationControl_t c, LVCompensationControl_t &o) {
    o.implemented = c.implemented;
    o.min = c.min;
    o.max = c.max;
    o.compensable = c.maxCompensable;
    o.steps = c.steps;
    o.step = c.step;
    o.decimals = c.decimals;
    o.value = c.value;
    o.prefix = c.prefix;
    string2Output(c.unit, &o.unit);
    string2Output(c.name, &o.name);
}

void vectorString2Output(std::vector <std::string> v, LStrHandle * o) {
    std::string a;
    for (auto s : v) {
        a += s + ",";
    }
    string2Output(a, o);
}

void vectorMeasurement2Output(std::vector <Measurement_t> v, LMeasHandle * o) {
    int offset = 0;
    MgErr err = 0;
    if (o == nullptr) {
        * o = (LMeasHandle)DSNewHClr(Offset(LMeas, item)+sizeof(LVMeasurement_t)*v.size());

    } else {
        err = DSSetHSzClr(* o, Offset(LMeas, item)+sizeof(LVMeasurement_t)*v.size());
    }
    if (!err) {
        for (auto m : v) {
            LVMeasurement_t * meas = LVecItem(** o, offset);
            measurement2Output(m, * meas);
            offset++;
        }
        LVecLen(** o) = v.size();
    }
}

void calibrationRanges2Output(CalibrationRanges_t ranges, LVecMeasHandle * o) {
    std::vector <std::vector <Measurement_t> > v2;
    calibrationRanges2MatrixMeasurement(ranges, v2);
    int offset = 0;
    MgErr err = 0;
    if (o == nullptr) {
        * o = (LVecMeasHandle)DSNewHClr(Offset(LVecMeas, item)+sizeof(LVMeasurement_t)*v2.size()*v2[0].size());

    } else {
        err = DSSetHSzClr(* o, Offset(LVecMeas, item)+sizeof(LVMeasurement_t)*v2.size()*v2[0].size());
    }
    if (!err) {
        for (auto v : v2) {
            for (auto m : v) {
                LVMeasurement_t * meas = LVecItem(** o, offset);
                measurement2Output(m, * meas);
                offset++;
            }
        }
        LMatS1(** o) = v2.size();
        LMatS2(** o) = v2[0].size();
    }
}

void calibrationRanges2MatrixMeasurement(CalibrationRanges_t ranges, std::vector <std::vector <Measurement_t> > &v2) {
    v2.resize(ranges.ranges.size());
    for (int idx = 0; idx < ranges.ranges.size(); idx++) {
        v2[idx] = ranges.getValues(idx);
    }
}

void vectorRangedMeasurement2Output(std::vector <RangedMeasurement_t> v, LRangeHandle * o) {
    int offset = 0;
    MgErr err = 0;
    if (o == nullptr) {
        * o = (LRangeHandle)DSNewHClr(Offset(LRange, item)+sizeof(LVRangedMeasurement_t)*v.size());

    } else {
        err = DSSetHSzClr(* o, Offset(LRange, item)+sizeof(LVRangedMeasurement_t)*v.size());
    }
    if (!err) {
        for (auto r : v) {
            LVRangedMeasurement_t * range = LVecItem(** o, offset);
            rangedMeasurement2Output(r, * range);
            offset++;
        }
        LVecLen(** o) = v.size();
    }
}

template<typename I_t, typename O_t> void numericVector2Output(I_t v, O_t * o) {
    for (unsigned int i = 0; i<v.size(); i++) {
        o[i] = v[i];
    }
}

template<typename I_t, typename O_t> void input2NumericVector(I_t * v, O_t &o, int inputLength) {
    o.resize(inputLength);
    for (unsigned int i = 0; i<o.size(); i++) {
        o[i] = v[i];
    }
}

ErrorCodes_t turnCompensationOn(uint16_t * channelIndexesIn, bool * onValuesIn, bool applyFlagIn,
                                int vectorLengthIn, MessageDispatcher::CompensationTypes_t type) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <bool> onValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(onValuesIn, onValues, vectorLengthIn);
    return messageDispatcher->enableCompensation(channelIndexes, type, onValues, applyFlagIn);
}

ErrorCodes_t setCompensationOptions(uint16_t * channelIndexesIn, uint16_t * optionIndexesIn, bool applyFlagIn,
                                    int vectorLengthIn, MessageDispatcher::CompensationTypes_t type) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <uint16_t> optionIndexes;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(optionIndexesIn, optionIndexes, vectorLengthIn);
    return messageDispatcher->setCompOptions(channelIndexes, type, optionIndexes, applyFlagIn);
}

ErrorCodes_t setCompensationValues(uint16_t * channelIndexesIn, double * channelValuesIn, bool applyFlagIn,
                                   int vectorLengthIn, MessageDispatcher::CompensationUserParams_t param) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <double> channelValues;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(channelValuesIn, channelValues, vectorLengthIn);
    return messageDispatcher->setCompValues(channelIndexes, param, channelValues, applyFlagIn);
}

ErrorCodes_t setCompensationRanges(uint16_t * channelIndexesIn, uint16_t * channelRangesIn, bool applyFlagIn,
                                   int vectorLengthIn, MessageDispatcher::CompensationUserParams_t param) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <uint16_t> channelRanges;
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);
    input2NumericVector(channelRangesIn, channelRanges, vectorLengthIn);
    return messageDispatcher->setCompRanges(channelIndexes, param, channelRanges, applyFlagIn);
}

ErrorCodes_t getCompensationOptions(LStrHandle * optionsOut, MessageDispatcher::CompensationTypes_t type) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <std::string> options;
    ErrorCodes_t ret = messageDispatcher->getCompOptionsFeatures(type, options);
    vectorString2Output(options, optionsOut);
    return ret;
}

ErrorCodes_t getCompensationControl(LVCompensationControl_t &controlOut, MessageDispatcher::CompensationUserParams_t param) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    CompensationControl_t control;
    ErrorCodes_t ret = messageDispatcher->getCompensationControl(param, control);
    compensationControl2Output(control, controlOut);
    return ret;
}

ErrorCodes_t getCompensationValues(uint16_t * channelIndexesIn, double * channelValuesOut, bool * activeNotActiveOut,
                                  int vectorLengthIn, MessageDispatcher::CompensationTypes_t type, MessageDispatcher::CompensationUserParams_t param) {
    if (messageDispatcher == nullptr) {
        return ErrorDeviceNotConnected;
    }
    std::vector <uint16_t> channelIndexes;
    std::vector <double> channelValues;
    std::vector <bool> activeNotActive;
    std::vector <std::vector <double> > thisCompValueMatrix;
    uint16_t currChanNum;
    uint16_t voltChanNum;
    messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
    input2NumericVector(channelIndexesIn, channelIndexes, vectorLengthIn);

    ErrorCodes_t ret = messageDispatcher->getCompensationEnables(channelIndexes, type, activeNotActive);
    if (ret == ErrorFeatureNotImplemented) {
        return ret;
    }
    ret = messageDispatcher->getCompValueMatrix(thisCompValueMatrix);
    for (int i = 0; i < currChanNum; i++) {
        channelValues[i] = thisCompValueMatrix[channelIndexes[i]][param];
    }
    numericVector2Output(channelValues, channelValuesOut);
    numericVector2Output(activeNotActive, activeNotActiveOut);
    return ret;
}
