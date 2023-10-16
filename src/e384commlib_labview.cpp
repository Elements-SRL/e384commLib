#include "e384commlib_labview.h"

#include <algorithm>

#include "messagedispatcher.h"

#ifdef _WIN32
#include <windows.h>
#endif

static MessageDispatcher * messageDispatcher = nullptr;

/*! Private functions prototypes */

static void input2String(LStrHandle i, std::string &s);
static void input2Measurement(CharMeasurement_t i, Measurement_t &m);
static void input2VectorMeasurement(LMeasHandle i, std::vector <Measurement_t> &m);

static void string2Output(std::string s, LStrHandle o);
static void measurement2Output(Measurement_t m, CharMeasurement_t &o);
static void rangedMeasurement2Output(RangedMeasurement_t r, CharRangedMeasurement_t &o);
static void compensationControl2Output(CompensationControl_t c, CharCompensationControl_t &o);
static void calibrationParams2Output(CalibrationParams_t p, CharCalibrationParams_t &o);
static void vectorString2Output(std::vector <std::string> v, LStrHandle o);
static void vectorMeasurement2Output(std::vector <Measurement_t> v, LMeasHandle * o);
static void vectorVectorMeasurement2Output(std::vector <std::vector <Measurement_t>> v, LVecMeasHandle * o);
static void vectorRangedMeasurement2Output(std::vector <RangedMeasurement_t> v, LRangeHandle * o);

template<typename I_t, typename O_t> void numericVector2Output(I_t v, O_t * o);
template<typename I_t, typename O_t> void input2NumericVector(I_t * v, O_t &o, int inputLength);

/*****************\
 *  Ctor / Dtor  *
\*****************/

ErrorCodes_t init() {
    /*! \todo FCON sicuri non ci sia da fare nulla? */
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t deinit() {
    /*! \todo FCON sicuri non ci sia da fare nulla? */
    return ErrorFeatureNotImplemented;
}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t detectDevices(
        LStrHandle deviceIdsOut) {
    std::vector <std::string> deviceIds;

    MessageDispatcher::detectDevices(deviceIds);

    vectorString2Output(deviceIds, deviceIdsOut);

    return Success;
}

ErrorCodes_t connectDevice(
        LStrHandle deviceIdIn,
        LStrHandle fwPathIn) {

    ErrorCodes_t ret = Success;
    if (messageDispatcher == nullptr) {
        std::string deviceId;
        std::string fwPath;
        input2String(deviceIdIn, deviceId);
        input2String(fwPathIn, fwPath);

        ret = MessageDispatcher::connectDevice(deviceId, messageDispatcher, fwPath);

    } else {
        ret = ErrorDeviceAlreadyConnected;
    }
    return ret;
}

ErrorCodes_t disconnectDevice() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->disconnectDevice();
        if (ret == Success) {
            delete messageDispatcher;
            messageDispatcher = nullptr;
        }

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t turnVoltageStimulusOn(
        bool onValueIn,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageStimulusOn(onValueIn, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentStimulusOn(
        bool onValueIn,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentStimulusOn(onValueIn, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnVoltageReaderOn(
        bool onValueIn,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageReaderOn(onValueIn, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentReaderOn(
        bool onValueIn,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentReaderOn(onValueIn, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setChannelsSources(
        int16_t voltageSourcesIdx,
        int16_t currentSourcesIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setSourceForVoltageChannel(voltageSourcesIdx,false);
        ret = messageDispatcher->setSourceForCurrentChannel(voltageSourcesIdx,true);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageHoldTuner(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> voltages;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* voltagesIn, voltages);
        ret = messageDispatcher->setVoltageHoldTuner(channelIndexes, voltages, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentHoldTuner(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * currentsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> currents;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* currentsIn, currents);
        ret = messageDispatcher->setCurrentHoldTuner(channelIndexes, currents, true);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageHalf(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> voltages;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* voltagesIn, voltages);
        ret = messageDispatcher->setVoltageHalf(channelIndexes, voltages, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentHalf(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * currentsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> currents;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* currentsIn, currents);
        ret = messageDispatcher->setCurrentHalf(channelIndexes, currents, true);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setLiquidJunctionVoltage(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> voltages;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* voltagesIn, voltages);
        ret = messageDispatcher->setLiquidJunctionVoltage(channelIndexes, voltages, true);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t resetLiquidJunctionVoltage(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        ret = messageDispatcher->resetLiquidJunctionVoltage(channelIndexes, true);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcCurrentGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* gainsIn, gains);
        ret = messageDispatcher->setCalibVcCurrentGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcCurrentOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* offsetsIn, offsets);
        ret = messageDispatcher->setCalibVcCurrentOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcVoltageGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* gainsIn, gains);
        ret = messageDispatcher->setCalibVcVoltageGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcVoltageOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* offsetsIn, offsets);
        ret = messageDispatcher->setCalibVcVoltageOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}



ErrorCodes_t setCalibCcVoltageGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* gainsIn, gains);
        ret = messageDispatcher->setCalibCcVoltageGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibCcVoltageOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* offsetsIn, offsets);
        ret = messageDispatcher->setCalibCcVoltageOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibCcCurrentGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* gainsIn, gains);
        ret = messageDispatcher->setCalibCcCurrentGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibCcCurrentOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(* offsetsIn, offsets);
        ret = messageDispatcher->setCalibCcCurrentOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setGateVoltage(
        E384CL_ARGIN uint16_t * boardIndexesIn,
        E384CL_ARGIN LMeasHandle * gateVoltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> boardIndexes;
        std::vector<Measurement_t> gateVoltages;
        input2NumericVector<uint16_t>(boardIndexesIn, boardIndexes, vectorLengthIn);
        input2VectorMeasurement(* gateVoltagesIn, gateVoltages);
        ret = messageDispatcher->setGateVoltages(boardIndexes, gateVoltages, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setSourceVoltage(
        E384CL_ARGIN uint16_t * boardIndexesIn,
        E384CL_ARGIN LMeasHandle * sourceVoltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> boardIndexes;
        std::vector<Measurement_t> sourceVoltages;
        input2NumericVector<uint16_t>(boardIndexesIn, boardIndexes, vectorLengthIn);
        input2VectorMeasurement(* sourceVoltagesIn, sourceVoltages);
        ret = messageDispatcher->setSourceVoltages(boardIndexes, sourceVoltages, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVCCurrentRange(
        uint16_t currentRangeIdx,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVCCurrentRange(currentRangeIdx, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCCurrentRange(
        uint16_t currentRangeIdx,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCCurrentRange(currentRangeIdx, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVCVoltageRange(
        uint16_t voltageRangeIdx,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVCVoltageRange(voltageRangeIdx, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCVoltageRange(
        uint16_t voltageRangeIdx,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCVoltageRange(voltageRangeIdx, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setSamplingRate(
        uint16_t samplingRateIdx,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setSamplingRate(samplingRateIdx, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setDigitalFilter(
        E384CL_ARGIN double cutoffFrequency,
        E384CL_ARGIN bool lowPassFlag,
        E384CL_ARGIN bool activeFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        return ErrorFeatureNotImplemented;
//        ret = messageDispatcher->setDigitalFilter(cutoffFrequency, lowPassFlag, activeFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t digitalOffsetCompensation(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->digitalOffsetCompensation(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t zap(
        CharMeasurement_t durationIn,
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        return ErrorFeatureNotImplemented;
//        Measurement_t duration;
//        input2Measurement(durationIn, duration);
//        ret = messageDispatcher->zap(duration, channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageStimulusLpf(
        uint16_t filterIdx,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVoltageStimulusLpf(filterIdx, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentStimulusLpf(
        uint16_t filterIdx,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCurrentStimulusLpf(filterIdx, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t enableStimulus(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->enableStimulus(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnChannelsOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnChannelsOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCalSwOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnCalSwOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t enableCcStimulus(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->enableCcStimulus(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setClampingModality(
        ClampingModality_t clampingModalityIndex,
        bool applyFlagIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setClampingModality(clampingModalityIndex, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnVoltageCompensationsOn(
        bool onValue
        ) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->enableVcCompensations(onValue);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentCompensationsOn(
        bool onValue) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {

        ret = messageDispatcher->enableCcCompensations(onValue);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnPipetteCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->enableCompensation(channelIndexes, MessageDispatcher::CompCfast, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCCPipetteCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->enableCompensation(channelIndexes, MessageDispatcher::CompCcCfast, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnMembraneCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->enableCompensation(channelIndexes, MessageDispatcher::CompCslow, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistanceCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnResistanceCompensationOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistanceCorrectionOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->enableCompensation(channelIndexes, MessageDispatcher::CompRsCorr, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistancePredictionOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->enableCompensation(channelIndexes, MessageDispatcher::CompRsPred, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnLeakConductanceCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnLeakConductanceCompensationOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnBridgeBalanceCompensationOn(
        uint16_t * channelIndexesIn,
        bool * onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnBridgeBalanceCompensationOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setPipetteCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setPipetteCompensationOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCPipetteCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setCCPipetteCompensationOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setMembraneCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setMembraneCompensationOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setResistanceCompensationOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setCompOptions(channelIndexes, MessageDispatcher::CompRsCorr, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setResistancePredictionOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setLeakConductanceCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setLeakConductanceCompensationOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setBridgeBalanceCompensationOptions(
        uint16_t * channelIndexesIn,
        uint16_t * optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setBridgeBalanceCompensationOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setCompValues(channelIndexes, MessageDispatcher::U_CpVc, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setCompValues(channelIndexes, MessageDispatcher::U_CpCc, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setMembraneCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setCompValues(channelIndexes, MessageDispatcher::U_Cm, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setCompValues(channelIndexes, MessageDispatcher::U_Rs, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setCompValues(channelIndexes, MessageDispatcher::U_RsCp, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionLag(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setResistanceCorrectionLag(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionGain(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setCompValues(channelIndexes, MessageDispatcher::U_RsPg, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setResistancePredictionPercentage(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionBandwidthGain(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setResistancePredictionBandwidthGain(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionTau(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setResistancePredictionTau(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setLeakConductance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setLeakConductance(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setBridgeBalanceResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setBridgeBalanceResistance(channelIndexes, channelValues, applyFlagIn);
    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageProtocolStructure(uint16_t protId,
        uint16_t itemsNum,
        uint16_t sweepsNum,
        CharMeasurement_t vRestIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t vRest;
        input2Measurement(vRestIn, vRest);
        ret = messageDispatcher->setVoltageProtocolStructure(protId, itemsNum, sweepsNum, vRest);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltStepTimeStep(
        CharMeasurement_t v0In,
        CharMeasurement_t vStepIn,
        CharMeasurement_t t0In,
        CharMeasurement_t tStepIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t v0;
        Measurement_t vStep;
        Measurement_t t0;
        Measurement_t tStep;
        input2Measurement(v0In, v0);
        input2Measurement(vStepIn, vStep);
        input2Measurement(t0In, t0);
        input2Measurement(tStepIn, tStep);
        ret = messageDispatcher->setVoltageProtocolStep(currentItem, nextItem, repsNum, applySteps, v0, vStep, t0, tStep, vHalfFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltRamp(
        CharMeasurement_t v0In,
        CharMeasurement_t vFinalIn,
        CharMeasurement_t tIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t v0;
        Measurement_t vFinal;
        Measurement_t t;
        input2Measurement(v0In, v0);
        input2Measurement(vFinalIn, vFinal);
        input2Measurement(tIn, t);
        Measurement_t vStep = v0*0.0;
        Measurement_t tStep = t*0.0;
        ret = messageDispatcher->setVoltageProtocolRamp(currentItem, nextItem, repsNum, applySteps, v0, vStep, vFinal, vStep, t, tStep, vHalfFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltSin(
        CharMeasurement_t v0In,
        CharMeasurement_t vAmpIn,
        CharMeasurement_t freqIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t vHalfFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t v0;
        Measurement_t vAmp;
        Measurement_t freq;
        input2Measurement(v0In, v0);
        input2Measurement(vAmpIn, vAmp);
        input2Measurement(freqIn, freq);
        Measurement_t vStep = v0*0.0;
        Measurement_t fStep = freq*0.0;
        ret = messageDispatcher->setVoltageProtocolSin(currentItem, nextItem, repsNum, applySteps, v0, vStep, vAmp, vStep, freq, fStep, vHalfFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t startProtocol() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->startProtocol();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentProtocolStructure(uint16_t protId,
        uint16_t itemsNum,
        uint16_t sweepsNum,
        CharMeasurement_t iRestIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t iRest;
        input2Measurement(iRestIn, iRest);
        ret = messageDispatcher->setCurrentProtocolStructure(protId, itemsNum, sweepsNum, iRest);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currStepTimeStep(
        CharMeasurement_t i0In,
        CharMeasurement_t iStepIn,
        CharMeasurement_t t0In,
        CharMeasurement_t tStepIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t i0;
        Measurement_t iStep;
        Measurement_t t0;
        Measurement_t tStep;
        input2Measurement(i0In, i0);
        input2Measurement(iStepIn, iStep);
        input2Measurement(t0In, t0);
        input2Measurement(tStepIn, tStep);
        ret = messageDispatcher->setCurrentProtocolStep(currentItem, nextItem, repsNum, applySteps, i0, iStep, t0, tStep, cHalfFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currRamp(
        CharMeasurement_t i0In,
        CharMeasurement_t iFinalIn,
        CharMeasurement_t tIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t i0;
        Measurement_t iFinal;
        Measurement_t t;
        input2Measurement(i0In, i0);
        input2Measurement(iFinalIn, iFinal);
        input2Measurement(tIn, t);
        Measurement_t iStep = i0*0.0;
        Measurement_t tStep = t*0.0;
        ret = messageDispatcher->setCurrentProtocolRamp(currentItem, nextItem, repsNum, applySteps, i0, iStep, iFinal, iStep, t, tStep, cHalfFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currSin(
        CharMeasurement_t i0In,
        CharMeasurement_t iAmpIn,
        CharMeasurement_t freqIn,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps,
        uint16_t cHalfFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t i0;
        Measurement_t iAmp;
        Measurement_t freq;
        input2Measurement(i0In, i0);
        input2Measurement(iAmpIn, iAmp);
        input2Measurement(freqIn, freq);
        Measurement_t iStep = i0*0.0;
        Measurement_t fStep = freq*0.0;
        ret = messageDispatcher->setCurrentProtocolSin(currentItem, nextItem, repsNum, applySteps, i0, iStep, iAmp, iStep, freq, fStep, cHalfFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t resetAsic(bool reset) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetAsic(reset);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t resetFpga(bool reset) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetFpga(reset);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t resetDigitalOffsetCompensation(bool reset) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        return ErrorFeatureNotImplemented;
//        ret = messageDispatcher->resetDigitalOffsetCompensation(reset);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

/****************\
 *  Rx methods  *
\****************/

ErrorCodes_t getRxDataBufferSize(
        uint32_t &size) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getRxDataBufferSize(size);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getNextMessage(
        RxOutput_t &rxOutput, int16_t* data) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getNextMessage(rxOutput, data);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLiquidJunctionVoltages(
        uint16_t * channelIndexesIn,
        LMeasHandle * voltagesOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector <Measurement_t> voltages;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        ret = messageDispatcher->getLiquidJunctionVoltages(channelIndexes, voltages);
        vectorMeasurement2Output(voltages, voltagesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t purgeData() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->purgeData();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getChannelsNumber(
        uint32_t &currentChannelsNum,
        uint32_t &voltageChannelsNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        uint16_t currChanNum;
        uint16_t voltChanNum;
        ret = messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
        currentChannelsNum = (uint32_t)currChanNum;
        voltageChannelsNum = (uint32_t)voltChanNum;

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAvailableChannelsSources(
        ChannelSources_t &voltageSourcesIdxs,
        ChannelSources_t &currentSourcesIdxs) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getAvailableChannelsSourcesFeatures(voltageSourcesIdxs, currentSourcesIdxs);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBoardsNumber(
        uint32_t &boardsNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        uint16_t borNum;
        ret = messageDispatcher->getBoardsNumberFeatures(borNum);
        boardsNum = (uint32_t)borNum;

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVoltageHoldTunerFeatures(
        LRangeHandle * voltageHoldTunerFeaturesOut){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> ranges;
        ret = messageDispatcher->getVoltageHoldTunerFeatures(ranges);
        vectorRangedMeasurement2Output(ranges, voltageHoldTunerFeaturesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCurrentHoldTunerFeatures(
        LRangeHandle * currentHoldTunerFeaturesOut){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> ranges;
        ret = messageDispatcher->getCurrentHoldTunerFeatures(ranges);
        vectorRangedMeasurement2Output(ranges, currentHoldTunerFeaturesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVoltageHalfFeatures(
        LRangeHandle * voltageHalfFeaturesOut){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> ranges;
        ret = messageDispatcher->getVoltageHalfFeatures(ranges);
        vectorRangedMeasurement2Output(ranges, voltageHalfFeaturesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCurrentHalfFeatures(
        LRangeHandle * currentHalfFeaturesOut){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> ranges;
        ret = messageDispatcher->getCurrentHalfFeatures(ranges);
        vectorRangedMeasurement2Output(ranges, currentHalfFeaturesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibVcCurrentOffsetFeatures(
        LRangeHandle * calibVcCurrentOffsetFeaturesOut){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t>  calibVcCurrentOffsetFeatures;
        ret = messageDispatcher->getCalibVcCurrentOffsetFeatures(calibVcCurrentOffsetFeatures);
        vectorRangedMeasurement2Output( calibVcCurrentOffsetFeatures, calibVcCurrentOffsetFeaturesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;

}

ErrorCodes_t getClampingModalitiesFeatures(
        uint16_t * clampingModalitiesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <ClampingModality_t> clampingModalities;
        ret = messageDispatcher->getClampingModalitiesFeatures(clampingModalities);
        numericVector2Output<std::vector <ClampingModality_t>, uint16_t>(clampingModalities, clampingModalitiesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCCurrentRanges(
        LRangeHandle * currentRangesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> currentRanges;
        uint16_t unused;
        ret = messageDispatcher->getVCCurrentRanges(currentRanges, unused);
        vectorRangedMeasurement2Output(currentRanges, currentRangesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCCurrentRanges(
        LRangeHandle * currentRangesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> currentRanges;
        ret = messageDispatcher->getCCCurrentRanges(currentRanges);
        vectorRangedMeasurement2Output(currentRanges, currentRangesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCCurrentRange(
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t currentRange;
        ret = messageDispatcher->getVCCurrentRange(currentRange);
        rangedMeasurement2Output(currentRange, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCCurrentRange(
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t currentRange;
        ret = messageDispatcher->getCCCurrentRange(currentRange);
        rangedMeasurement2Output(currentRange, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCVoltageRanges(
        LRangeHandle * voltageRangesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> voltageRanges;
        ret = messageDispatcher->getVCVoltageRanges(voltageRanges);
        vectorRangedMeasurement2Output(voltageRanges, voltageRangesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCVoltageRanges(
        LRangeHandle * voltageRangesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <RangedMeasurement_t> voltageRanges;
        ret = messageDispatcher->getCCVoltageRanges(voltageRanges);
        vectorRangedMeasurement2Output(voltageRanges, voltageRangesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCVoltageRange(
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t voltageRange;
        ret = messageDispatcher->getVCVoltageRange(voltageRange);
        rangedMeasurement2Output(voltageRange, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCVoltageRange(
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t voltageRange;
        ret = messageDispatcher->getCCVoltageRange(voltageRange);
        rangedMeasurement2Output(voltageRange, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getSamplingRates(
        LMeasHandle * samplingRatesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <Measurement_t> samplingRates;
        ret = messageDispatcher->getSamplingRatesFeatures(samplingRates);
        vectorMeasurement2Output(samplingRates, samplingRatesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getRealSamplingRates(
        LMeasHandle * realSamplingRatesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <Measurement_t> realSamplingRates;
        ret = messageDispatcher->getRealSamplingRatesFeatures(realSamplingRates);
        vectorMeasurement2Output(realSamplingRates, realSamplingRatesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVoltageProtocolRange(
        unsigned int rangeIdx,
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t range;
        ret = messageDispatcher->getVoltageProtocolRangeFeature(rangeIdx, range);
        rangedMeasurement2Output(range, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCurrentProtocolRange(
        unsigned int rangeIdx,
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t range;
        ret = messageDispatcher->getCurrentProtocolRangeFeature(rangeIdx, range);
        rangedMeasurement2Output(range, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getTimeProtocolRange(
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t range;
        ret = messageDispatcher->getTimeProtocolRangeFeature(range);
        rangedMeasurement2Output(range, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getFrequencyProtocolRange(
        CharRangedMeasurement_t &rangeOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        RangedMeasurement_t range;
        ret = messageDispatcher->getFrequencyProtocolRangeFeature(range);
        rangedMeasurement2Output(range, rangeOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMaxProtocolItems(
        E384CL_ARGOUT unsigned int &maxItemsNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMaxProtocolItemsFeature(maxItemsNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasProtocolStep() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolStepFeature();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasProtocolRamp() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolRampFeature();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasProtocolSin() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolSinFeature();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVoltageStimulusLpfs(
        LStrHandle filterOptionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <Measurement_t> vcVoltageFilters;
        ret = messageDispatcher->getVCVoltageFilters(vcVoltageFilters);
        std::vector <std::string> filterOptions;
        for(uint16_t i = 0; i < vcVoltageFilters.size(); i++){
            filterOptions[i] = vcVoltageFilters[i].niceLabel();
        }
        vectorString2Output(filterOptions, filterOptionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCurrentStimulusLpfs(
        LStrHandle filterOptionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <Measurement_t> ccCurrentFilters;
        ret = messageDispatcher->getCCCurrentFilters(ccCurrentFilters);
        std::vector <std::string> filterOptions;
        for(uint16_t i = 0; i < ccCurrentFilters.size(); i++){
            filterOptions[i] = ccCurrentFilters[i].niceLabel();
        }
        vectorString2Output(filterOptions, filterOptionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getPipetteCompensationOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getPipetteCompensationOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCPipetteCompensationOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getCCPipetteCompensationOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMembraneCompensationOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getMembraneCompensationOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceCompensationOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getResistanceCompensationOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceCorrectionOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getCompOptionsFeatures(MessageDispatcher::CompRsCorr, options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getResistancePredictionOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLeakConductanceCompensationOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getLeakConductanceCompensationOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBridgeBalanceCompensationOptions(
        LStrHandle optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getBridgeBalanceCompensationOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getPipetteCapacitanceControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getPipetteCapacitanceControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCPipetteCapacitanceControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getCCPipetteCapacitanceControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMembraneCapacitanceControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getMembraneCapacitanceControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getAccessResistanceControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistanceCorrectionPercentageControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getResistanceCorrectionPercentageControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistanceCorrectionLagControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getResistanceCorrectionLagControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionGainControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getResistancePredictionGainControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionPercentageControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getResistancePredictionPercentageControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionBandwidthGainControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getResistancePredictionBandwidthGainControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionTauControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getResistancePredictionTauControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLeakConductanceControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getLeakConductanceControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBridgeBalanceResistanceControl(
        CharCompensationControl_t &controlOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CompensationControl_t control;
        ret = messageDispatcher->getBridgeBalanceResistanceControl(control);
        compensationControl2Output(control, controlOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

// NEW MICHELANGELO'S GETS

ErrorCodes_t getPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        std::vector<std::vector<double>> thisCompValueMatrix;
        uint16_t currChanNum;
        uint16_t voltChanNum;
        messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getCompensationEnables(channelIndexes, MessageDispatcher::CompCfast, activeNotActive);
        if(ret == ErrorFeatureNotImplemented){
            return ret;
        } else {
            ret = messageDispatcher->getCompValueMatrix(thisCompValueMatrix);
            for(int i = 0; i < currChanNum; i++){
                channelValues[i] = thisCompValueMatrix[channelIndexes[i]][MessageDispatcher::U_CpVc];
            }
            numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
            numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);
        }
    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCPipetteCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        std::vector<std::vector<double>> thisCompValueMatrix;
        uint16_t currChanNum;
        uint16_t voltChanNum;
        messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getCompensationEnables(channelIndexes, MessageDispatcher::CompCcCfast, activeNotActive);
        if(ret == ErrorFeatureNotImplemented){
            return ret;
        } else {
            ret = messageDispatcher->getCompValueMatrix(thisCompValueMatrix);
            for(int i = 0; i < currChanNum; i++){
                channelValues[i] = thisCompValueMatrix[channelIndexes[i]][MessageDispatcher::U_CpCc];
            }
            numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
            numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);
        }
    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}


ErrorCodes_t getMembraneCapacitance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        std::vector<std::vector<double>> thisCompValueMatrix;
        uint16_t currChanNum;
        uint16_t voltChanNum;
        messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getCompensationEnables(channelIndexes, MessageDispatcher::CompCslow, activeNotActive);
        if(ret == ErrorFeatureNotImplemented){
            return ret;
        } else {
            ret = messageDispatcher->getCompValueMatrix(thisCompValueMatrix);
            for(int i = 0; i < currChanNum; i++){
                channelValues[i] = thisCompValueMatrix[channelIndexes[i]][MessageDispatcher::U_Cm];
            }
            numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
            numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);
        }
    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}


ErrorCodes_t getAccessResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        std::vector<std::vector<double>> thisCompValueMatrix;
        uint16_t currChanNum;
        uint16_t voltChanNum;
        messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getCompensationEnables(channelIndexes, MessageDispatcher::CompCslow, activeNotActive);
        if(ret == ErrorFeatureNotImplemented){
            return ret;
        } else {
            ret = messageDispatcher->getCompValueMatrix(thisCompValueMatrix);
            for(int i = 0; i < currChanNum; i++){
                channelValues[i] = thisCompValueMatrix[channelIndexes[i]][MessageDispatcher::U_Rs];
            }
            numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
            numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);
        }
    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}


ErrorCodes_t getAccessResistanceCorrectionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        std::vector<std::vector<double>> thisCompValueMatrix;
        uint16_t currChanNum;
        uint16_t voltChanNum;
        messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getCompensationEnables(channelIndexes, MessageDispatcher::CompRsCorr, activeNotActive);
        if(ret == ErrorFeatureNotImplemented){
            return ret;
        } else {
            ret = messageDispatcher->getCompValueMatrix(thisCompValueMatrix);
            for(int i = 0; i < currChanNum; i++){
                channelValues[i] = thisCompValueMatrix[channelIndexes[i]][MessageDispatcher::U_RsCp];
            }
            numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
            numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);
        }
    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}


ErrorCodes_t getAccessResistanceCorrectionLag(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistanceCorrectionLag(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionGain(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        std::vector<std::vector<double>> thisCompValueMatrix;
        uint16_t currChanNum;
        uint16_t voltChanNum;
        messageDispatcher->getChannelNumberFeatures(currChanNum, voltChanNum);
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getCompensationEnables(channelIndexes, MessageDispatcher::CompRsPred, activeNotActive);
        if(ret == ErrorFeatureNotImplemented){
            return ret;
        } else {
            ret = messageDispatcher->getCompValueMatrix(thisCompValueMatrix);
            for(int i = 0; i < currChanNum; i++){
                channelValues[i] = thisCompValueMatrix[channelIndexes[i]][MessageDispatcher::U_RsPg];
            }
            numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
            numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);
        }
    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}


ErrorCodes_t getAccessResistancePredictionPercentage(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistancePredictionPercentage(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionBandwidthGain(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistancePredictionBandwidthGain(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionTau(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistancePredictionTau(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLeakConductance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getLeakConductance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBridgeBalanceResistance(
        uint16_t * channelIndexesIn,
        double * channelValuesOut,
        bool * activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getBridgeBalanceResistance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCalibParams(
        CharCalibrationParams_t &calibrationParamsOut) {

    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        CalibrationParams_t calibParams;
        ret = messageDispatcher->getCalibParams(calibParams);
        calibrationParams2Output(calibParams, calibrationParamsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setDebugBit(
        E384CL_ARGIN uint16_t wordOffset,
        E384CL_ARGIN uint16_t bitOffset,
        E384CL_ARGIN bool status){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setDebugBit(wordOffset, bitOffset, status);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setDebugWord(
        E384CL_ARGIN uint16_t wordOffset,
        E384CL_ARGIN uint16_t wordValue){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setDebugWord(wordOffset, wordValue);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

/*! Private functions */
void input2String(LStrHandle i, std::string &s) {
    s = std::string((char *)LStrBuf(* i), LStrLen(* i));
}

void input2Measurement(CharMeasurement_t i, Measurement_t &m) {
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

void string2Output(std::string s, LStrHandle o) {
    MgErr err = NumericArrayResize(uB, 1, (UHandle *)&o, s.length());
    if (!err) {
         MoveBlock(s.c_str(), LStrBuf(* o), s.length());
         LStrLen(* o) = s.length();
    }
}

void measurement2Output(Measurement_t m, CharMeasurement_t &o) {
    o.value = m.value;
    o.prefix = m.prefix;
    string2Output(m.unit, o.unit);
}

void rangedMeasurement2Output(RangedMeasurement_t r, CharRangedMeasurement_t &o) {
    o.min = r.min;
    o.max = r.max;
    o.step = r.step;
    o.prefix = r.prefix;
    string2Output(r.unit, o.unit);
}

void compensationControl2Output(CompensationControl_t c, CharCompensationControl_t &o) {
    o.implemented = c.implemented;
    o.min = c.min;
    o.max = c.max;
    o.compensable = c.compensable;
    o.steps = c.steps;
    o.step = c.step;
    o.decimals = c.decimals;
    o.value = c.value;
    o.prefix = c.prefix;
    string2Output(c.unit, o.unit);
    string2Output(c.name, o.name);
}

void calibrationParams2Output(CalibrationParams_t p, CharCalibrationParams_t &o) {
    vectorVectorMeasurement2Output(p.allGainAdcMeas, &o.allGainAdcMeas);
    vectorVectorMeasurement2Output(p.allGainDacMeas, &o.allGainDacMeas);
    vectorVectorMeasurement2Output(p.allOffsetAdcMeas, &o.allOffsetAdcMeas);
    vectorVectorMeasurement2Output(p.allOffsetDacMeas, &o.allOffsetDacMeas);
    vectorVectorMeasurement2Output(p.ccAllGainAdcMeas, &o.ccAllGainAdcMeas);
    vectorVectorMeasurement2Output(p.ccAllGainDacMeas, &o.ccAllGainDacMeas);
    vectorVectorMeasurement2Output(p.ccAllOffsetAdcMeas, &o.ccAllOffsetAdcMeas);
    vectorVectorMeasurement2Output(p.ccAllOffsetDacMeas, &o.ccAllOffsetDacMeas);
}

void vectorString2Output(std::vector <std::string> v, LStrHandle o) {
    std::string a;
    for (auto s : v) {
        a += s + ",";
    }
    string2Output(a, o);
}

void vectorMeasurement2Output(std::vector <Measurement_t> v, LMeasHandle * o) {
    int offset = 0;
    MgErr err = DSSetHSzClr(* o, Offset(LMeas, item)+sizeof(CharMeasurement_t)*v.size());
    if (!err) {
        for (auto m : v) {
            CharMeasurement_t * meas = LVecItem(** o, offset);
            measurement2Output(m, * meas);
            offset++;
        }
        LVecLen(** o) = v.size();
    }
}

void vectorVectorMeasurement2Output(std::vector <std::vector <Measurement_t>> m, LVecMeasHandle * o) {
    int offset = 0;
    MgErr err = DSSetHSzClr(* o, Offset(LVecMeas, item)+sizeof(CharMeasurement_t)*m.size());
    if (!err) {
        for (auto v : m) {
            LMeasHandle * vec = LVecItem(** o, offset);
            vectorMeasurement2Output(v, vec);
            offset++;
        }
        LVecLen(** o) = m.size();
    }
}

void vectorRangedMeasurement2Output(std::vector <RangedMeasurement_t> v, LRangeHandle * o) {
    int offset = 0;
    MgErr err = DSSetHSzClr(* o, Offset(LRange, item)+sizeof(CharRangedMeasurement_t)*v.size());
    if (!err) {
        for (auto r : v) {
            CharRangedMeasurement_t * range = LVecItem(** o, offset);
            rangedMeasurement2Output(r, * range);
            offset++;
        }
        LVecLen(** o) = v.size();
    }
}

template<typename I_t, typename O_t> void numericVector2Output(I_t v, O_t * o){
    for(unsigned int i = 0; i<v.size(); i++){
        o[i] = v[i];
    }
}

template<typename I_t, typename O_t> void input2NumericVector(I_t * v, O_t &o, int inputLength){
    o.resize(inputLength);
    for(unsigned int i = 0; i<o.size(); i++){
        o[i] = v[i];
    }
}
