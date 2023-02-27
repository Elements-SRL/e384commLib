#include "e384commlib.h"

#include <algorithm>

#include "messagedispatcher.h"
#include "okFrontPanelDLL.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static MessageDispatcher * messageDispatcher = nullptr;

/*! Private functions prototypes */
static string getDeviceSerial(int index);
static bool getDeviceCount(int &numDevs);

static void input2String(E384clString_t i, string &s);
static void input2Measurement(E384clMeasurement_t i, Measurement_t &m);

static void string2Output(string s, E384clString_t E384CL_OUTPUT_SYMBOL o);
static void vectorString2Output(vector <string> v, E384clStringVector_t E384CL_OUTPUT_SYMBOL o);
static void vectorMeasurement2Output(vector <Measurement_t> v, E384clMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL o);
static void vectorRangedMeasurement2Output(vector <RangedMeasurement_t> v, E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL o);

template<typename I_t, typename O_t> void numericVector2Output(I_t v, O_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL o);
template<typename I_t, typename O_t> void input2NumericVector(I_t E384CL_VECTOR_SYMBOL v, O_t &o, int inputLength);

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
        E384clStringVector_t E384CL_OUTPUT_SYMBOL deviceIdsOut) {
    vector <string> deviceIds;
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
    vectorString2Output(deviceIds, deviceIdsOut);

    return Success;
}

ErrorCodes_t connectDevice(
        E384clString_t deviceIdIn) {

    ErrorCodes_t ret = Success;
    if (messageDispatcher == nullptr) {
        string deviceId;
        input2String(deviceIdIn, deviceId);

        DeviceTypes_t deviceType;
        ret = MessageDispatcher::getDeviceType(deviceId, deviceType);
        if (ret != Success) {
            return ErrorDeviceTypeNotRecognized;
        }

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

    } else {
        ret = ErrorDeviceAlreadyConnected;
    }
    return ret;
}

ErrorCodes_t disconnectDevice() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->disconnect();
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

ErrorCodes_t ping() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->ping();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnVoltageStimulusOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnVoltageStimulusOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentStimulusOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnCurrentStimulusOn(channelIndexes, onValues, applyFlagIn);

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
        ret = messageDispatcher->setChannelsSources(voltageSourcesIdx, currentSourcesIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageHoldTuner(
        uint16_t channelIdx,
        E384clMeasurement_t voltageIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t voltage;
        input2Measurement(voltageIn, voltage);
        ret = messageDispatcher->setVoltageHoldTuner(channelIdx, voltage);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentHoldTuner(
        uint16_t channelIdx,
        E384clMeasurement_t currentIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t current;
        input2Measurement(currentIn, current);
        ret = messageDispatcher->setCurrentHoldTuner(channelIdx, current);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnOnLsbNoise(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL flagValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> flagValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(flagValuesIn, flagValues, vectorLengthIn);
        ret = messageDispatcher->turnOnLsbNoise(channelIndexes, flagValues, vectorLengthIn);

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
        uint16_t currentRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCCurrentRange(currentRangeIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVCVoltageRange(
        uint16_t voltageRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVCVoltageRange(voltageRangeIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCVoltageRange(
        uint16_t voltageRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCVoltageRange(voltageRangeIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setSamplingRate(
        uint16_t samplingRateIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setSamplingRate(samplingRateIdx);

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
        ret = messageDispatcher->setDigitalFilter(cutoffFrequency, lowPassFlag, activeFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t digitalOffsetCompensation(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
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

ErrorCodes_t digitalOffsetCompensationOverride(
        uint16_t channelIdx,
        E384clMeasurement_t valueIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t value;
        input2Measurement(valueIn, value);
        ret = messageDispatcher->digitalOffsetCompensationOverride(channelIdx, value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t digitalOffsetCompensationInquiry(
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->digitalOffsetCompensationInquiry(channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVcCurrentOffsetDelta(
        uint16_t channelIdx,
        E384clMeasurement_t valueIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t value;
        input2Measurement(valueIn, value);
        ret = messageDispatcher->setVcCurrentOffsetDelta(channelIdx, value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCcVoltageOffsetDelta(
        uint16_t channelIdx,
        E384clMeasurement_t valueIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t value;
        input2Measurement(valueIn, value);
        ret = messageDispatcher->setCcVoltageOffsetDelta(channelIdx, value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t zap(
        E384clMeasurement_t durationIn,
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t duration;
        input2Measurement(durationIn, duration);
        ret = messageDispatcher->zap(duration, channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageStimulusLpf(
        uint16_t filterIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVoltageStimulusLpf(filterIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentStimulusLpf(
        uint16_t filterIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCurrentStimulusLpf(filterIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t enableStimulus(
        uint16_t channelIdx,
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->enableStimulus(channelIdx, on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnLedOn(
        uint16_t ledIndex,
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnLedOn(ledIndex, on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setSlave(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setSlave(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnVoltageCompensationsOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnVoltageCompensationsOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentCompensationsOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnCurrentCompensationsOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnPipetteCompensationOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnPipetteCompensationOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCCPipetteCompensationOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnCCPipetteCompensationOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnMembraneCompensationOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnMembraneCompensationOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistanceCompensationOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnResistanceCorrectionOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistancePredictionOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<bool> onValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<bool>(onValuesIn, onValues, vectorLengthIn);
        ret = messageDispatcher->turnResistancePredictionOn(channelIndexes, onValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnLeakConductanceCompensationOn(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL onValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<uint16_t> optionIndexes;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<uint16_t>(optionIndexesIn, optionIndexes, vectorLengthIn);
        ret = messageDispatcher->setResistanceCorrectionOptions(channelIndexes, optionIndexes, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionOptions(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL optionIndexesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setPipetteCapacitance(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCPipetteCapacitance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setCCPipetteCapacitance(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setMembraneCapacitance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setMembraneCapacitance(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setAccessResistance(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionPercentage(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setResistanceCorrectionPercentage(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionLag(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
        bool applyFlagIn,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);
        input2NumericVector<double>(channelValuesIn, channelValues, vectorLengthIn);

        ret = messageDispatcher->setResistancePredictionGain(channelIndexes, channelValues, applyFlagIn);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionPercentage(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
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
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL channelValuesIn,
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

ErrorCodes_t setDigitalTriggerOutput(
        E384CL_ARGIN uint16_t triggerIdx,
        E384CL_ARGIN bool terminator,
        E384CL_ARGIN bool polarity,
        E384CL_ARGIN uint16_t triggerId,
        E384CL_ARGIN E384clMeasurement_t delayIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        Measurement_t delay;
        input2Measurement(delayIn, delay);
        ret = messageDispatcher->setDigitalTriggerOutput(triggerIdx, terminator, polarity, triggerId, delay);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageProtocolStructure(uint16_t protId,
        uint16_t itemsNum,
        uint16_t sweepsNum,
        E384clMeasurement_t vRestIn) {
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
        E384clMeasurement_t v0In,
        E384clMeasurement_t vStepIn,
        E384clMeasurement_t t0In,
        E384clMeasurement_t tStepIn,
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
        ret = messageDispatcher->voltStepTimeStep(v0, vStep, t0, tStep, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltRamp(
        E384clMeasurement_t v0In,
        E384clMeasurement_t vFinalIn,
        E384clMeasurement_t tIn,
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
        ret = messageDispatcher->voltRamp(v0, vFinal, t, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltSin(
        E384clMeasurement_t v0In,
        E384clMeasurement_t vAmpIn,
        E384clMeasurement_t freqIn,
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
        ret = messageDispatcher->voltSin(v0, vAmp, freq, currentItem, nextItem, repsNum, applySteps);

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
        E384clMeasurement_t iRestIn) {
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
        E384clMeasurement_t i0In,
        E384clMeasurement_t iStepIn,
        E384clMeasurement_t t0In,
        E384clMeasurement_t tStepIn,
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
        ret = messageDispatcher->currStepTimeStep(i0, iStep, t0, tStep, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currRamp(
        E384clMeasurement_t i0In,
        E384clMeasurement_t iFinalIn,
        E384clMeasurement_t tIn,
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
        ret = messageDispatcher->currRamp(i0, iFinal, t, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currSin(
        E384clMeasurement_t i0In,
        E384clMeasurement_t iAmpIn,
        E384clMeasurement_t freqIn,
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
        ret = messageDispatcher->currSin(i0, iAmp, freq, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t resetChip(bool reset) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetChip(reset);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t resetDigitalOffsetCompensation(bool reset) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetDigitalOffsetCompensation(reset);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t resetFpga() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetFpga();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCalibrationConfiguration(
        CalibrationConfiguration_t * &calibrationConfiguration) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCalibrationConfiguration(calibrationConfiguration);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCalibrationEepromSize(
        uint32_t &size) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCalibrationEepromSize(size);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t writeCalibrationEeprom(
        vector <uint32_t> value,
        vector <uint32_t> address,
        vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->writeCalibrationEeprom(value, address, size);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t readCalibrationEeprom(
        vector <uint32_t> &value,
        vector <uint32_t> address,
        vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->readCalibrationEeprom(value, address, size);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getSwitchesStatus(std::vector <uint16_t> &words, std::vector <std::vector <std::string>> &names) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getSwitchesStatus(words, names);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t singleSwitchDebug(uint16_t word, uint16_t bit, bool flag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->singleSwitchDebug(word, bit, flag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t multiSwitchDebug(vector <uint16_t> words) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->multiSwitchDebug(words);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t singleRegisterDebug(uint16_t index, uint16_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->singleRegisterDebug(index, value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

/****************\
 *  Rx methods  *
\****************/

ErrorCodes_t isDeviceUpgradable(
        E384CL_ARGOUT std::string &upgradeNotes,
        E384CL_ARGOUT std::string &notificationTag) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->isDeviceUpgradable(upgradeNotes, notificationTag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getDeviceInfo(
        E384CL_ARGOUT std::string &deviceId,
        E384CL_ARGOUT std::string &deviceName,
        E384CL_ARGOUT uint8_t &deviceVersion,
        E384CL_ARGOUT uint8_t &deviceSubversion,
        E384CL_ARGOUT uint32_t &firmwareVersion) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getDeviceInfo(deviceId, deviceName, deviceVersion, deviceSubversion, firmwareVersion);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getDeviceInfo(
        E384CL_ARGOUT std::string deviceId,
        E384CL_ARGOUT uint8_t &deviceVersion,
        E384CL_ARGOUT uint8_t &deviceSubversion,
        E384CL_ARGOUT uint32_t &firmwareVersion) {
    ErrorCodes_t ret = Success;
    /*! Initializes eeprom */
    /*! \todo FCON questa info dovrà essere appresa dal device detector e condivisa qui dal metodo connect */
    FtdiEepromId_t ftdiEepromId = FtdiEepromId56;
    if (deviceId == "ePatch Demo") {
        ftdiEepromId = FtdiEepromIdDemo;
    }

    if (deviceId == "eP8 Demo") {
        ftdiEepromId = FtdiEepromIdDemo;
    }

    /*! ftdiEeprom is deleted by the messageDispatcher if one is created successfully */
    FtdiEeprom * ftdiEeprom = nullptr;
    switch (ftdiEepromId) {
    case FtdiEepromId56:
        ftdiEeprom = new FtdiEeprom56(deviceId);
        break;

    case FtdiEepromIdDemo:
        ftdiEeprom = new FtdiEepromDemo(deviceId);
        break;
    }

    if (ftdiEeprom != nullptr) {
        DeviceTuple_t deviceTuple = ftdiEeprom->getDeviceTuple();

        deviceVersion = deviceTuple.version;
        deviceSubversion = deviceTuple.subversion;
        firmwareVersion = deviceTuple.fwVersion;

    } else {
        ret = ErrorEepromNotRecognized;
    }
    return ret;
}

ErrorCodes_t getNextMessage(
        RxOutput_t &rxOutput) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getNextMessage(rxOutput);

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
        ret = messageDispatcher->getChannelsNumber(currentChannelsNum, voltageChannelsNum);

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
        ret = messageDispatcher->getAvailableChannelsSources(voltageSourcesIdxs, currentSourcesIdxs);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasVoltageHoldTuner() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasVoltageHoldTuner();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasCurrentHoldTuner() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasCurrentHoldTuner();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVCCurrentRanges(
        E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL currentRangesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <RangedMeasurement_t> currentRanges;
        ret = messageDispatcher->getVCCurrentRanges(currentRanges);
        vectorRangedMeasurement2Output(currentRanges, currentRangesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCCurrentRanges(
        E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL currentRangesOut) {
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
        E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL voltageRangesOut) {
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
        E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL voltageRangesOut) {
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
        E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL samplingRatesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <RangedMeasurement_t> samplingRates;
        ret = messageDispatcher->getSamplingRates(samplingRates);
        vectorRangedMeasurement2Output(samplingRates, samplingRatesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getRealSamplingRates(
        E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL samplingRatesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        vector <RangedMeasurement_t> samplingRates;
        ret = messageDispatcher->getRealSamplingRates(samplingRates);
        vectorRangedMeasurement2Output(samplingRates, samplingRatesOut);

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

ErrorCodes_t getMaxOutputTriggers(
        E384CL_ARGOUT unsigned int &maxTriggersNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMaxOutputTriggers(maxTriggersNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getOutputTriggersNum(
        E384CL_ARGOUT unsigned int &triggersNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getOutputTriggersNum(triggersNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMaxProtocolItems(
        E384CL_ARGOUT unsigned int &maxItemsNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMaxProtocolItems(maxItemsNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasProtocolStep() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolStep();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasProtocolRamp() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolRamp();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasProtocolSin() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolSin();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getVoltageStimulusLpfs(
        E384clStringVector_t E384CL_OUTPUT_SYMBOL filterOptionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> filterOptions;
        ret = messageDispatcher->getVoltageStimulusLpfs(filterOptions);
        vectorString2Output(filterOptions, filterOptionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCurrentStimulusLpfs(
        E384clStringVector_t E384CL_OUTPUT_SYMBOL filterOptionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> filterOptions;
        ret = messageDispatcher->getCurrentStimulusLpfs(filterOptions);
        vectorString2Output(filterOptions, filterOptionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLedsNumber(
        uint16_t &ledsNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLedsNumber(ledsNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLedsColors(
        E384clUint32Vector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL ledsColorsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <uint32_t> ledsColors;
        ret = messageDispatcher->getLedsColors(ledsColors);
        numericVector2Output<std::vector <uint32_t>, E384clUint32Vector_t>(ledsColors, ledsColorsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasSlaveModality() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasSlaveModality();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getClampingModalities(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL clampingModalitiesOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <uint16_t> clampingModalities;
        ret = messageDispatcher->getClampingModalities(clampingModalities);
        numericVector2Output<std::vector <uint16_t>, E384clUint16Vector_t>(clampingModalities, clampingModalitiesOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasPipetteCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasPipetteCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasCCPipetteCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasCCPipetteCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasMembraneCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasMembraneCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasAccessResistanceCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasResistanceCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasAccessResistanceCorrection() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasResistanceCorrection();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasAccessResistancePrediction() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasResistancePrediction();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasLeakConductanceCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasLeakConductanceCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t hasBridgeBalanceCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasBridgeBalanceCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getPipetteCompensationOptions(
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
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
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
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
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
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
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
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
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector <std::string> options;
        ret = messageDispatcher->getResistanceCorrectionOptions(options);
        vectorString2Output(options, optionsOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionOptions(
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
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
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
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
        E384clStringVector_t E384CL_OUTPUT_SYMBOL optionsOut) {
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
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLiquidJunctionControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getPipetteCapacitanceControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getPipetteCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCPipetteCapacitanceControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCPipetteCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMembraneCapacitanceControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMembraneCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getAccessResistanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistanceCorrectionPercentageControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionPercentageControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistanceCorrectionLagControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionLagControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionGainControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionGainControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionPercentageControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionPercentageControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionBandwidthGainControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionBandwidthGainControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionTauControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionTauControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLeakConductanceControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLeakConductanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBridgeBalanceResistanceControl(
        E384clCompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getBridgeBalanceResistanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

// NEW MICHELANGELO'S GETS

ErrorCodes_t getPipetteCapacitance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getPipetteCapacitance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCPipetteCapacitance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getCCPipetteCapacitance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMembraneCapacitance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getMembraneCapacitance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceCorrectionPercentage(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistanceCorrectionPercentage(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceCorrectionLag(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistanceCorrectionLag(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionGain(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistancePredictionGain(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionPercentage(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistancePredictionPercentage(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionBandwidthGain(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistancePredictionBandwidthGain(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionTau(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn = 0) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getAccessResistancePredictionTau(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLeakConductance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getLeakConductance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBridgeBalanceResistance(
        E384clUint16Vector_t E384CL_VECTOR_SYMBOL channelIndexesIn,
        E384clDoubleVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL channelValuesOut,
        E384clBoolVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL activeNotActiveOut,
        int vectorLengthIn) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        std::vector<uint16_t> channelIndexes;
        std::vector<double> channelValues;
        std::vector<bool> activeNotActive;
        input2NumericVector<uint16_t>(channelIndexesIn, channelIndexes, vectorLengthIn);

        ret = messageDispatcher->getBridgeBalanceResistance(channelIndexes, channelValues, activeNotActive);

        numericVector2Output<std::vector <double>, E384clDoubleVector_t>(channelValues, channelValuesOut);
        numericVector2Output<std::vector <bool>, E384clBoolVector_t>(activeNotActive, activeNotActiveOut);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

// END NEW MICHELANGELO'S GETS

#ifndef E384CL_LABVIEW_COMPATIBILITY
} // namespace e384CommLib
#endif

/*! Private functions */
string getDeviceSerial(int index) {
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

bool getDeviceCount(int &numDevs) {
    okCFrontPanel okDev;
    numDevs = okDev.GetDeviceCount();
    return true;
}

void input2String(E384clString_t i, string &s) {
#ifndef E384CL_LABVIEW_COMPATIBILITY
    s = i;
#else
    s = string((char *)LStrBuf(* i), LStrLen(* i));
#endif
}

void input2Measurement(E384clMeasurement_t i, Measurement_t &m) {
#ifndef E384CL_LABVIEW_COMPATIBILITY
    m = i;
#else
    m.value = i.value;
    m.prefix = i.prefix;
    input2String(i.unit, m.unit);
#endif
}

void string2Output(string s, E384clString_t E384CL_OUTPUT_SYMBOL o) {
#ifndef E384CL_LABVIEW_COMPATIBILITY
    o = s;
#else
    MgErr err = NumericArrayResize(uB, 1, (UHandle *)&o, s.length());
    if (!err) {
         MoveBlock(s.c_str(), LStrBuf(* o), s.length());
         LStrLen(* o) = s.length();
    }
#endif
}

void vectorString2Output(vector <string> v, E384clStringVector_t E384CL_OUTPUT_SYMBOL o) {
#ifndef E384CL_LABVIEW_COMPATIBILITY
    o = v;
#else
    string a;
    for (auto s : v) {
        a += s + ",";
    }
    string2Output(a, o);
#endif
}

void vectorMeasurement2Output(vector <Measurement_t> v, E384clMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL o) {
#ifndef E384CL_LABVIEW_COMPATIBILITY
    o = v;
#else
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
#endif
}

void vectorRangedMeasurement2Output(vector <RangedMeasurement_t> v, E384clRangedMeasurementVector_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL o) {
#ifndef E384CL_LABVIEW_COMPATIBILITY
    o = v;
#else
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
#endif
}

template<typename I_t, typename O_t> void numericVector2Output(I_t v, O_t E384CL_VECTOR_SYMBOL E384CL_OUTPUT_SYMBOL o){
#ifndef E384CL_LABVIEW_COMPATIBILITY
    o = v;
#else
    for(unsigned int i = 0; i<v.size(); i++){
        o[i] = v[i];
    }
#endif
}

template<typename I_t, typename O_t> void input2NumericVector(I_t E384CL_VECTOR_SYMBOL v, O_t &o, int inputLength){
#ifndef E384CL_LABVIEW_COMPATIBILITY
    o = v;
#else
    o.resize(inputLength);
    for(unsigned int i = 0; i<o.size(); i++){
        o[i] = v[i];
    }
#endif
}
