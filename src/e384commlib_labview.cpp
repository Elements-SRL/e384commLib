#include "e384commlib_labview.h"

#include <algorithm>

#include "messagedispatcher.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static MessageDispatcher * messageDispatcher = nullptr;

/*! Private functions prototypes */

static void input2String(LStrHandle i, string &s);
static void input2Measurement(CharMeasurement_t i, Measurement_t &m);
static void input2VectorMeasurement(LMeasHandle i, vector <Measurement_t> &m);

static void string2Output(string s, LStrHandle o);
static void vectorString2Output(vector <string> v, LStrHandle o);
static void vectorMeasurement2Output(vector <Measurement_t> v, LMeasHandle * o);
static void vectorRangedMeasurement2Output(vector <RangedMeasurement_t> v, LRangeHandle * o);

template<typename I_t, typename O_t> void numericVector2Output(I_t v, O_t * o);
template<typename I_t, typename O_t> void input2NumericVector(I_t * v, O_t &o, int inputLength);

#ifndef E384CL_LABVIEW_COMPATIBILITY
namespace e384CommLib {
#endif

/*****************\
 *  Ctor / Dtor  *
\*****************/

ErrorCodes_t init() {

}

ErrorCodes_t deinit() {

}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t detectDevices(
        LStrHandle deviceIdsOut) {
    vector <string> deviceIds;

    MessageDispatcher::detectDevices(deviceIds);

    vectorString2Output(deviceIds, deviceIdsOut);

    return Success;
}

ErrorCodes_t connectDevice(
        LStrHandle deviceIdIn) {

    ErrorCodes_t ret = Success;
    if (messageDispatcher == nullptr) {
        string deviceId;
        input2String(deviceIdIn, deviceId);

        ret = MessageDispatcher::connectDevice(deviceId, messageDispatcher);

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
        E384CL_ARGIN LMeasHandle voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> voltages;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(voltagesIn, voltages);
        ret = messageDispatcher->setVoltageHoldTuner(channelIndexes, voltages, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcCurrentGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(gainsIn, gains);
        ret = messageDispatcher->setCalibVcCurrentGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcCurrentOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(offsetsIn, offsets);
        ret = messageDispatcher->setCalibVcCurrentOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcVoltageGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(gainsIn, gains);
        ret = messageDispatcher->setCalibVcVoltageGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibVcVoltageOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(offsetsIn, offsets);
        ret = messageDispatcher->setCalibVcVoltageOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}



ErrorCodes_t setCalibCcVoltageGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(gainsIn, gains);
        ret = messageDispatcher->setCalibCcVoltageGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibCcVoltageOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(offsetsIn, offsets);
        ret = messageDispatcher->setCalibCcVoltageOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibCcCurrentGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> gains;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(gainsIn, gains);
        ret = messageDispatcher->setCalibCcCurrentGain(channelIndexes, gains, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCalibCcCurrentOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> offsets;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(offsetsIn, offsets);
        ret = messageDispatcher->setCalibCcCurrentOffset(channelIndexes, offsets, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setGateVoltage(
        E384CL_ARGIN uint16_t * boardIndexesIn,
        E384CL_ARGIN LMeasHandle gateVoltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> boardIndexes;
        std::vector<Measurement_t> gateVoltages;
        input2NumericVector<uint16_t>(boardIndexesIn, boardIndexes, vectorLengthIn);
        input2VectorMeasurement(gateVoltagesIn, gateVoltages);
        ret = messageDispatcher->setGateVoltagesTuner(boardIndexes, gateVoltages, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setSourceVoltage(
        E384CL_ARGIN uint16_t * boardIndexesIn,
        E384CL_ARGIN LMeasHandle sourceVoltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> boardIndexes;
        std::vector<Measurement_t> sourceVoltages;
        input2NumericVector<uint16_t>(boardIndexesIn, boardIndexes, vectorLengthIn);
        input2VectorMeasurement(sourceVoltagesIn, sourceVoltages);
        ret = messageDispatcher->setSourceVoltagesTuner(boardIndexes, sourceVoltages, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentHoldTuner(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle currentsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<Measurement_t> currents;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2VectorMeasurement(currentsIn, currents);
        ret = messageDispatcher->setCurrentHoldTuner(channelIndexes, currents, true);
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

//ErrorCodes_t setDigitalFilter(
//        E384CL_ARGIN double cutoffFrequency,
//        E384CL_ARGIN bool lowPassFlag,
//        E384CL_ARGIN bool activeFlag) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->setDigitalFilter(cutoffFrequency, lowPassFlag, activeFlag);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

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

//ErrorCodes_t digitalOffsetCompensationOverride(
//        uint16_t channelIdx,
//        CharMeasurement_t valueIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        Measurement_t value;
//        input2Measurement(valueIn, value);
//        ret = messageDispatcher->digitalOffsetCompensationOverride(channelIdx, value);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t setVcCurrentOffsetDelta(
//        uint16_t channelIdx,
//        CharMeasurement_t valueIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        Measurement_t value;
//        input2Measurement(valueIn, value);
//        ret = messageDispatcher->setVcCurrentOffsetDelta(channelIdx, value);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t setCcVoltageOffsetDelta(
//        uint16_t channelIdx,
//        CharMeasurement_t valueIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        Measurement_t value;
//        input2Measurement(valueIn, value);
//        ret = messageDispatcher->setCcVoltageOffsetDelta(channelIdx, value);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t zap(
//        CharMeasurement_t durationIn,
//        uint16_t channelIdx) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        Measurement_t duration;
//        input2Measurement(durationIn, duration);
//        ret = messageDispatcher->zap(duration, channelIdx);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

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
        uint16_t applySteps) {
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
        ret = messageDispatcher->setVoltageProtocolStep(currentItem, nextItem, repsNum, applySteps, v0, vStep, t0, tStep);

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
        uint16_t applySteps) {
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
        ret = messageDispatcher->setVoltageProtocolRamp(currentItem, nextItem, repsNum, applySteps, v0, vStep, vFinal, vStep, t, tStep);

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
        uint16_t applySteps) {
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
        ret = messageDispatcher->setVoltageProtocolSin(currentItem, nextItem, repsNum, applySteps, v0, vStep, vAmp, vStep, freq, fStep);

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
        uint16_t applySteps) {
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
        ret = messageDispatcher->setCurrentProtocolStep(currentItem, nextItem, repsNum, applySteps, i0, iStep, t0, tStep);

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
        uint16_t applySteps) {
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
        ret = messageDispatcher->setCurrentProtocolRamp(currentItem, nextItem, repsNum, applySteps, i0, iStep, iFinal, iStep, t, tStep);

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
        uint16_t applySteps) {
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
        ret = messageDispatcher->setCurrentProtocolSin(currentItem, nextItem, repsNum, applySteps, i0, iStep, iAmp, iStep, freq, fStep);

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

//ErrorCodes_t resetDigitalOffsetCompensation(bool reset) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->resetDigitalOffsetCompensation(reset);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getCalibrationEepromSize(
//        uint32_t &size) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getCalibrationEepromSize(size);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t writeCalibrationEeprom(
//        vector <uint32_t> value,
//        vector <uint32_t> address,
//        vector <uint32_t> size) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->writeCalibrationEeprom(value, address, size);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t readCalibrationEeprom(
//        vector <uint32_t> &value,
//        vector <uint32_t> address,
//        vector <uint32_t> size) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->readCalibrationEeprom(value, address, size);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}


/****************\
 *  Rx methods  *
\****************/

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

//ErrorCodes_t getAvailableChannelsSources(
//        ChannelSources_t &voltageSourcesIdxs,
//        ChannelSources_t &currentSourcesIdxs) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getAvailableChannelsSources(voltageSourcesIdxs, currentSourcesIdxs);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasVoltageHoldTuner() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasVoltageHoldTuner();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasCurrentHoldTuner() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasCurrentHoldTuner();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//------------------------------------------------------------------------
/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getVoltageHoldTunerFeatures(
        LRange voltageHoldTunerFeaturesOut){

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCurrentHoldTunerFeatures(
        LRange currentHoldTunerFeaturesOut){

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibVcCurrentGainFeatures(
        LRange calibVcCurrentGainFeaturesOut){

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibVcCurrentOffsetFeatures(
        LRangeHandle * calibVcCurrentOffsetFeaturesOut){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <RangedMeasurement_t>  calibVcCurrentOffsetFeatures;
        ret = messageDispatcher->getCalibVcCurrentOffsetFeatures(calibVcCurrentOffsetFeatures);
        vectorRangedMeasurement2Output( calibVcCurrentOffsetFeatures, calibVcCurrentOffsetFeaturesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibVcVoltageGainFeatures(
        LRange calibVcVoltageGainFeaturesOut){

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibVcVoltageOffsetFeatures(
        LRange calibVcVoltageOffsetFeaturesOut){

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibCcVoltageGainFeatures(
        LRange calibCcVoltageGainFeaturesOut){

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibCcVoltageOffsetFeatures(
        LRangeHandle * calibCcVoltageOffsetFeaturesOut){
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <RangedMeasurement_t>  calibCcVoltageOffsetFeatures;
        ret = messageDispatcher->getCalibCcVoltageOffsetFeatures(calibCcVoltageOffsetFeatures);
        vectorRangedMeasurement2Output( calibCcVoltageOffsetFeatures, calibCcVoltageOffsetFeaturesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibCcCurrentGainFeatures(
        LRange calibCcCurrentGainFeaturesOut){

}

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
ErrorCodes_t getCalibCcCurrentOffsetFeatures(
        LRange calibCcCurrentOffsetFeaturesOut){

}

ErrorCodes_t getClampingModalitiesFeatures(
        uint16_t * clampingModalitiesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <uint16_t> clampingModalities;
        ret = messageDispatcher->getClampingModalitiesFeatures(clampingModalities);
        numericVector2Output<std::vector <uint16_t>, uint16_t>(clampingModalities, clampingModalitiesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}
//------------------------------------------------------------------------

ErrorCodes_t getVCCurrentRanges(
        LRangeHandle * currentRangesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <RangedMeasurement_t> currentRanges;
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
        vector <RangedMeasurement_t> currentRanges;
        ret = messageDispatcher->getCCCurrentRanges(currentRanges);
        vectorRangedMeasurement2Output(currentRanges, currentRangesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCCurrentRange(
        RangedMeasurement_t &currentRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVCCurrentRange(currentRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCCurrentRange(
        RangedMeasurement_t &currentRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCCurrentRange(currentRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCVoltageRanges(
        LRangeHandle * voltageRangesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <RangedMeasurement_t> voltageRanges;
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
        vector <RangedMeasurement_t> voltageRanges;
        ret = messageDispatcher->getCCVoltageRanges(voltageRanges);
        vectorRangedMeasurement2Output(voltageRanges, voltageRangesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCVoltageRange(
        RangedMeasurement_t &voltageRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVCVoltageRange(voltageRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCVoltageRange(
        RangedMeasurement_t &voltageRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCVoltageRange(voltageRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getSamplingRates(
        LMeasHandle * samplingRatesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <Measurement_t> samplingRates;
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
        vector <Measurement_t> realSamplingRates;
        ret = messageDispatcher->getRealSamplingRatesFeatures(realSamplingRates);
        vectorMeasurement2Output(realSamplingRates, realSamplingRatesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVoltageProtocolRange(
        unsigned int rangeIdx,
        RangedMeasurement_t &voltageProtocolRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVoltageProtocolRange(rangeIdx, voltageProtocolRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCurrentProtocolRange(
        unsigned int rangeIdx,
        RangedMeasurement_t &currentProtocolRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCurrentProtocolRange(rangeIdx, currentProtocolRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getTimeProtocolRange(
        RangedMeasurement_t &timeProtocolRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getTimeProtocolRange(timeProtocolRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getFrequencyProtocolRange(
        RangedMeasurement_t &frequencyProtocolRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getFrequencyProtocolRange(frequencyProtocolRange);

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
        ret = messageDispatcher->getVoltageStimulusLpfs(vcVoltageFilters);
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
        ret = messageDispatcher->getCurrentStimulusLpfs(ccCurrentFilters);
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

//ErrorCodes_t hasPipetteCompensation() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasPipetteCompensation();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasCCPipetteCompensation() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasCCPipetteCompensation();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasMembraneCompensation() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasMembraneCompensation();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasAccessResistanceCompensation() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasResistanceCompensation();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasAccessResistanceCorrection() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasResistanceCorrection();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasAccessResistancePrediction() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasResistancePrediction();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasLeakConductanceCompensation() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasLeakConductanceCompensation();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t hasBridgeBalanceCompensation() {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->hasBridgeBalanceCompensation();

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

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

ErrorCodes_t getLiquidJunctionControl(
        CharCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLiquidJunctionControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

//ErrorCodes_t getPipetteCapacitanceControl(
//        CharCompensationControl_t &control) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getPipetteCapacitanceControl(control);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getCCPipetteCapacitanceControl(
//        CharCompensationControl_t &control) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getCCPipetteCapacitanceControl(control);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getMembraneCapacitanceControl(
//        CharCompensationControl_t &control) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getMembraneCapacitanceControl(control);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistanceControl(
//        CharCompensationControl_t &control) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getAccessResistanceControl(control);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getResistanceCorrectionPercentageControl(
//        CharCompensationControl_t &control) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getResistanceCorrectionPercentageControl(control);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

ErrorCodes_t getResistanceCorrectionLagControl(
        CharCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionLagControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

//ErrorCodes_t getResistancePredictionGainControl(
//        CharCompensationControl_t &control) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getResistancePredictionGainControl(control);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

ErrorCodes_t getResistancePredictionPercentageControl(
        CharCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionPercentageControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionBandwidthGainControl(
        CharCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionBandwidthGainControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionTauControl(
        CharCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionTauControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

//ErrorCodes_t getLeakConductanceControl(
//        CharCompensationControl_t &control) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        ret = messageDispatcher->getLeakConductanceControl(control);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

ErrorCodes_t getBridgeBalanceResistanceControl(
        CharCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getBridgeBalanceResistanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

// NEW MICHELANGELO'S GETS

//ErrorCodes_t getPipetteCapacitance(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getPipetteCapacitance(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getCCPipetteCapacitance(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getCCPipetteCapacitance(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getMembraneCapacitance(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getMembraneCapacitance(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistance(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getAccessResistance(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistanceCorrectionPercentage(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getAccessResistanceCorrectionPercentage(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistanceCorrectionLag(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getAccessResistanceCorrectionLag(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistancePredictionGain(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getAccessResistancePredictionGain(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistancePredictionPercentage(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getAccessResistancePredictionPercentage(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistancePredictionBandwidthGain(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getAccessResistancePredictionBandwidthGain(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getAccessResistancePredictionTau(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn = 0) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getAccessResistancePredictionTau(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getLeakConductance(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getLeakConductance(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

//ErrorCodes_t getBridgeBalanceResistance(
//        uint16_t * channelIndexesIn,
//        double * channelValuesOut,
//        bool * activeNotActiveOut,
//        int vectorLengthIn) {
//    ErrorCodes_t ret;
//    if (messageDispatcher != nullptr) {
//        std::vector<uint16_t> channelIndexes;
//        std::vector<double> channelValues;
//        std::vector<bool> activeNotActive;
//        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

//        ret = messageDispatcher->getBridgeBalanceResistance(channelIndexes, channelValues, activeNotActive);

//        numericVector2Output<std::vector <double>, double>(channelValues, channelValuesOut);
//        numericVector2Output<std::vector <bool>, bool>(activeNotActive, activeNotActiveOut);

//    } else {
//        ret = ErrorDeviceNotConnected;
//    }
//    return ret;
//}

// END NEW MICHELANGELO'S GETS

/*! Private functions */
void input2String(LStrHandle i, string &s) {
    s = string((char *)LStrBuf(* i), LStrLen(* i));
}

void input2Measurement(CharMeasurement_t i, Measurement_t &m) {
    m.value = i.value;
    m.prefix = i.prefix;
    input2String(i.unit, m.unit);
}

void input2VectorMeasurement(LMeasHandle i, vector <Measurement_t> &m){
    for(int j = 0; j<LVecLen(*i); j++){
        input2Measurement(*LVecItem(*i,j), m[j]);
    }
}

void string2Output(string s, LStrHandle o) {
    MgErr err = NumericArrayResize(uB, 1, (UHandle *)&o, s.length());
    if (!err) {
         MoveBlock(s.c_str(), LStrBuf(* o), s.length());
         LStrLen(* o) = s.length();
    }
}

void vectorString2Output(vector <string> v, LStrHandle o) {
    string a;
    for (auto s : v) {
        a += s + ",";
    }
    string2Output(a, o);
}

void vectorMeasurement2Output(vector <Measurement_t> v, LMeasHandle * o) {
    int offset = 0;
    MgErr err = DSSetHSzClr(* o, Offset(LMeas, item)+sizeof(CharMeasurement_t)*v.size());
    if (!err) {
        for (auto m : v) {
            CharMeasurement_t * meas = LVecItem(** o, offset);
            meas->value = m.value;
            meas->prefix = m.prefix;

            size_t unitSize = m.unit.length();
            meas->unit = (LStrHandle)DSNewHClr(sizeof(int32_t)+sizeof(uChar)*unitSize);
            if (meas->unit == nullptr) {
                return;
            }
            MoveBlock(&m.unit, LStrBuf(*(meas->unit)), unitSize);
            LStrLen(* meas->unit) = unitSize;
            offset++;
        }
        LVecLen(** o) = v.size();
    }
}

void vectorRangedMeasurement2Output(vector <RangedMeasurement_t> v, LRangeHandle * o) {
    int offset = 0;
    MgErr err = DSSetHSzClr(* o, Offset(LRange, item)+sizeof(CharRangedMeasurement_t)*v.size());
    if (!err) {
        for (auto r : v) {
            CharRangedMeasurement_t * range = LVecItem(** o, offset);
            range->min = r.min;
            range->max = r.max;
            range->step = r.step;
            range->prefix = r.prefix;

            size_t unitSize = r.unit.length();
            range->unit = (LStrHandle)DSNewHClr(sizeof(int32_t)+sizeof(uChar)*unitSize);
            if (range->unit == nullptr) {
                return;
            }
            MoveBlock(&r.unit, LStrBuf(*(range->unit)), unitSize);
            LStrLen(* range->unit) = unitSize;
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
