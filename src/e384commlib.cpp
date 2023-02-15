#include "e384commlib.h"

#include <algorithm>

#include "messagedispatcher.h"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

static MessageDispatcher * messageDispatcher = nullptr;

static void input2String(E384clString_t i, string &s);
static void input2Measurement(E384clMeasurement_t i, Measurement_t &m);

static void string2Output(string s, E384clString_t E384CL_OUTPUT_SYMBOL o);
static void vectorString2Output(vector <string> v, E384clStringVector_t E384CL_OUTPUT_SYMBOL o);
static void vectorMeasurement2Output(vector <Measurement_t> v, E384clMeasurementVector_t E384CL_VECTOR_OUTPUT_SYMBOL E384CL_OUTPUT_SYMBOL o);
static void vectorRangedMeasurement2Output(vector <RangedMeasurement_t> v, E384clRangedMeasurementVector_t E384CL_VECTOR_OUTPUT_SYMBOL E384CL_OUTPUT_SYMBOL o);

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
        vector <string> &deviceIds) {
    /*! Gets number of devices */
//    DWORD numDevs;
//    bool devCountOk = getDeviceCount(numDevs);
//    if (!devCountOk) {
//        return ErrorListDeviceFailed;

//    } else if (numDevs == 0) {
//        deviceIds.clear();
//        return ErrorNoDeviceFound;
//    }

//    deviceIds.clear();
//    string deviceName;

//    /*! Lists all serial numbers */
//    for (uint32_t i = 0; i < numDevs; i++) {
//        deviceName = getDeviceSerial(i, true);
//        if (find(deviceIds.begin(), deviceIds.end(), deviceName) == deviceIds.end()) {
//            /*! Adds only new devices (no distinction between channels A and B creates duplicates) */
//            if (deviceName.size() > 0) {
//                /*! Devices with an open channel are detected wrongly and their name is an empty string */
//                deviceIds.push_back(getDeviceSerial(i, true));
//            }
//        }
//    }

    return Success;
}

ErrorCodes_t connect(
        string deviceId) {

    ErrorCodes_t ret = Success;
//    if (messageDispatcher == nullptr) {
//        /*! Initializes eeprom */
//        /*! \todo FCON questa info dovrà essere appresa dal device detector e condivisa qui dal metodo connect */
//        FtdiEepromId_t ftdiEepromId = FtdiEepromId56;
//        if (deviceId == "ePatch Demo") {
//            ftdiEepromId = FtdiEepromIdDemo;
//        }

//        if (deviceId == "eP8 Demo") {
//            ftdiEepromId = FtdiEepromIdDemo;
//        }

//        /*! ftdiEeprom is deleted by the messageDispatcher if one is created successfully */
//        FtdiEeprom * ftdiEeprom = nullptr;
//        switch (ftdiEepromId) {
//        case FtdiEepromId56:
//            ftdiEeprom = new FtdiEeprom56(deviceId);
//            break;

//        case FtdiEepromIdDemo:
//            ftdiEeprom = new FtdiEepromDemo(deviceId);
//            break;
//        }

//        DeviceTuple_t deviceTuple = ftdiEeprom->getDeviceTuple();
//        DeviceTypes_t deviceType;

//        ret = MessageDispatcher::getDeviceType(deviceTuple, deviceType);
//        if (ret != Success) {
//            if (ftdiEeprom != nullptr) {
//                delete ftdiEeprom;
//            }
//            return ErrorDeviceTypeNotRecognized;
//        }

//        switch (deviceType) {
//        case DeviceEPatchEL03D:
//            messageDispatcher = new MessageDispatcher_ePatchEL03D(deviceId);
//            break;

//        case DeviceEPatchEL03D_V01:
//            messageDispatcher = new MessageDispatcher_ePatchEL03D_V01(deviceId);
//            break;

//        case DeviceEPatchEL03D_V00:
//            messageDispatcher = new MessageDispatcher_ePatchEL03D_V00(deviceId);
//            break;

//        case DeviceEPatchEL03F_4D:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D(deviceId);
//            break;

//        case DeviceEPatchEL03F_4D_V02:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D_V02(deviceId);
//            break;

//        case DeviceEPatchEL03F_4D_V01:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D_V01(deviceId);
//            break;

//        case DeviceEPatchEL03F_4D_V00:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D_V00(deviceId);
//            break;

//        case DeviceEPatchEL03F_4E:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E(deviceId);
//            break;

//        case DeviceEPatchEL03F_4E_V02:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E_V02(deviceId);
//            break;

//        case DeviceEPatchEL03F_4E_V01:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E_V01(deviceId);
//            break;

//        case DeviceEPatchEL03F_4E_V00:
//            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E_V00(deviceId);
//            break;

//        case DeviceEPatchEL04E:
//            messageDispatcher = new MessageDispatcher_ePatchEL04E(deviceId);
//            break;

//        case DeviceEPatchEL04F:
//            messageDispatcher = new MessageDispatcher_ePatchEL04F(deviceId);
//            break;

//        case DeviceE4PEL04F:
//            messageDispatcher = new MessageDispatcher_e4PEL04F(deviceId);
//            break;

//        case DeviceE4PPatchLiner:
//            messageDispatcher = new MessageDispatcher_e4PPatchliner(deviceId);
//            break;

//        case DeviceE8PPatchLiner:
//            messageDispatcher = new MessageDispatcher_e8PPatchliner(deviceId);
//            break;

//        case DeviceE4PPatchLinerEL07AB:
//            messageDispatcher = new MessageDispatcher_e4PPatchliner_el07ab(deviceId);
//            break;

//        case DeviceE8PPatchLinerEL07AB:
//            messageDispatcher = new MessageDispatcher_e8PPatchliner_el07ab(deviceId);
//            break;

//        case DeviceFakePatch:
//            messageDispatcher = new MessageDispatcher_fakePatch(deviceId);
//            break;

//        case DeviceFakeP8:
//            messageDispatcher = new MessageDispatcher_fakeP8(deviceId);
//            break;

//        case DeviceEPatchDlp:
//            messageDispatcher = new MessageDispatcher_ePatchDlp(deviceId);
//            break;

//        default:
//            if (ftdiEeprom != nullptr) {
//                delete ftdiEeprom;
//            }
//            return ErrorDeviceTypeNotRecognized;
//        }

//        if (messageDispatcher != nullptr) {
//            ret = messageDispatcher->connect(ftdiEeprom);

//            if (ret != Success) {
//                messageDispatcher->disconnect();
//                delete messageDispatcher;
//                messageDispatcher = nullptr;
//            }
//        }

//    } else {
//        ret = ErrorDeviceAlreadyConnected;
//    }
    return ret;
}

ErrorCodes_t enableRxMessageType(
        MsgTypeId_t messageType,
        bool flag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->enableRxMessageType(messageType, flag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t disconnect() {
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

ErrorCodes_t abort() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->abort();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnVoltageStimulusOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageStimulusOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentStimulusOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentStimulusOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnVoltageReaderOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageReaderOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentReaderOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentReaderOn(on);

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
        Measurement_t voltage) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVoltageHoldTuner(channelIdx, voltage);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCurrentHoldTuner(
        uint16_t channelIdx,
        Measurement_t current) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCurrentHoldTuner(channelIdx, current);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnOnLsbNoise(
        bool flag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnOnLsbNoise(flag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVCCurrentRange(
        uint16_t currentRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVCCurrentRange(currentRangeIdx);

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

ErrorCodes_t setFilterRatio(
        uint16_t filterRatioIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setFilterRatio(filterRatioIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setUpsamplingRatio(
        uint16_t upsamplingRatioIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setUpsamplingRatio(upsamplingRatioIdx);

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
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->digitalOffsetCompensation(channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t digitalOffsetCompensationOverride(
        uint16_t channelIdx,
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
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
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVcCurrentOffsetDelta(channelIdx, value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCcVoltageOffsetDelta(
        uint16_t channelIdx,
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCcVoltageOffsetDelta(channelIdx, value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t zap(
        Measurement_t duration,
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
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

ErrorCodes_t setConstantSwitches() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setConstantSwitches();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCompensationsChannel(
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCompensationsChannel(channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnVoltageCompensationsOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageCompensationsOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCurrentCompensationsOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentCompensationsOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnPipetteCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnPipetteCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnCCPipetteCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCCPipetteCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnMembraneCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnMembraneCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistanceCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnResistanceCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistanceCorrectionOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnResistanceCorrectionOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnAccessResistancePredictionOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnResistancePredictionOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnLeakConductanceCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnLeakConductanceCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t turnBridgeBalanceCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnBridgeBalanceCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setPipetteCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setPipetteCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCPipetteCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCPipetteCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setMembraneCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setMembraneCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCorrectionOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setLeakConductanceCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setLeakConductanceCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setBridgeBalanceCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setBridgeBalanceCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setPipetteCapacitance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setPipetteCapacitance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setCCPipetteCapacitance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCPipetteCapacitance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setMembraneCapacitance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setMembraneCapacitance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setAccessResistance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionPercentage(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCorrectionPercentage(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistanceCorrectionLag(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCorrectionLag(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionGain(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionGain(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionPercentage(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionPercentage(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionBandwidthGain(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionBandwidthGain(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setAccessResistancePredictionTau(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionTau(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setLeakConductance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setLeakConductance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setBridgeBalanceResistance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setBridgeBalanceResistance(value);

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
        E384CL_ARGIN Measurement_t delay) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setDigitalTriggerOutput(triggerIdx, terminator, polarity, triggerId, delay);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t setVoltageProtocolStructure(uint16_t protId,
        uint16_t itemsNum,
        uint16_t sweepsNum,
        Measurement_t vRest) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVoltageProtocolStructure(protId, itemsNum, sweepsNum, vRest);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltStepTimeStep(
        Measurement_t v0,
        Measurement_t vStep,
        Measurement_t t0,
        Measurement_t tStep,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->voltStepTimeStep(v0, vStep, t0, tStep, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltRamp(
        Measurement_t v0,
        Measurement_t vFinal,
        Measurement_t t,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->voltRamp(v0, vFinal, t, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t voltSin(
        Measurement_t v0,
        Measurement_t vAmp,
        Measurement_t freq,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
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
        Measurement_t iRest) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCurrentProtocolStructure(protId, itemsNum, sweepsNum, iRest);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currStepTimeStep(
        Measurement_t i0,
        Measurement_t iStep,
        Measurement_t t0,
        Measurement_t tStep,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->currStepTimeStep(i0, iStep, t0, tStep, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currRamp(
        Measurement_t i0,
        Measurement_t iFinal,
        Measurement_t t,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->currRamp(i0, iFinal, t, currentItem, nextItem, repsNum, applySteps);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t currSin(
        Measurement_t i0,
        Measurement_t iAmp,
        Measurement_t freq,
        uint16_t currentItem,
        uint16_t nextItem,
        uint16_t repsNum,
        uint16_t applySteps) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
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
        E384clRangedMeasurementVector_t E384CL_VECTOR_OUTPUT_SYMBOL E384CL_OUTPUT_SYMBOL currentRangesOut) {
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
        E384clRangedMeasurementVector_t E384CL_VECTOR_OUTPUT_SYMBOL E384CL_OUTPUT_SYMBOL currentRangesOut) {
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
        vector <RangedMeasurement_t> &voltageRanges) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVCVoltageRanges(voltageRanges);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCVoltageRanges(
        vector <RangedMeasurement_t> &voltageRanges) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCVoltageRanges(voltageRanges);

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
        vector <Measurement_t> &samplingRates) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getSamplingRates(samplingRates);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getRealSamplingRates(
        vector <Measurement_t> &samplingRates) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getRealSamplingRates(samplingRates);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getFilterRatios(
        vector <Measurement_t> &filterRatios) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getFilterRatios(filterRatios);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getUpsamplingRatios(
        vector <Measurement_t> &upsamplingRatios) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getUpsamplingRatios(upsamplingRatios);

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
        std::vector <std::string> &filterOptions) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVoltageStimulusLpfs(filterOptions);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCurrentStimulusLpfs(
        std::vector <std::string> &filterOptions) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCurrentStimulusLpfs(filterOptions);

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
        vector <uint32_t> &ledsColors) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLedsColors(ledsColors);

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
        E384CL_ARGOUT std::vector <uint16_t> &clampingModalities) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getClampingModalities(clampingModalities);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t multimeterStuckHazard(
        E384CL_ARGOUT bool &stuckFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->multimeterStuckHazard(stuckFlag);

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
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getPipetteCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCPipetteCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCPipetteCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMembraneCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMembraneCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceCorrectionOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistancePredictionOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLeakConductanceCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLeakConductanceCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBridgeBalanceCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getBridgeBalanceCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLiquidJunctionControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLiquidJunctionControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getPipetteCapacitanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getPipetteCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getCCPipetteCapacitanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCPipetteCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getMembraneCapacitanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMembraneCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getAccessResistanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getAccessResistanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistanceCorrectionPercentageControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionPercentageControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistanceCorrectionLagControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionLagControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionGainControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionGainControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionPercentageControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionPercentageControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionBandwidthGainControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionBandwidthGainControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getResistancePredictionTauControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionTauControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getLeakConductanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLeakConductanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t getBridgeBalanceResistanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getBridgeBalanceResistanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

#ifndef E384CL_LABVIEW_COMPATIBILITY
} // namespace e384CommLib
#endif

void input2String(E384clString_t i, string &s) {
#ifdef E4DCCOMMLIB_STATIC
    s = i;
#else
    s = string((char *)LStrBuf(* i), LStrLen(* i));
#endif
}

void input2Measurement(E384clMeasurement_t i, Measurement_t &m) {
#ifdef E4DCCOMMLIB_STATIC
    m = i;
#else
    m.value = i.value;
    m.prefix = i.prefix;
    input2String(i.unit, m.unit);
#endif
}

void string2Output(string s, E384clString_t E384CL_OUTPUT_SYMBOL o) {
#ifdef E4DCCOMMLIB_STATIC
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
#ifdef E4DCCOMMLIB_STATIC
    o = v;
#else
    string a;
    for (auto s : v) {
        a += s + ",";
    }
    string2Output(a, o);
#endif
}

void vectorMeasurement2Output(vector <Measurement_t> v, E384clMeasurementVector_t E384CL_VECTOR_OUTPUT_SYMBOL E384CL_OUTPUT_SYMBOL o) {
#ifdef E4DCCOMMLIB_STATIC
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

void vectorRangedMeasurement2Output(vector <RangedMeasurement_t> v, E384clRangedMeasurementVector_t E384CL_VECTOR_OUTPUT_SYMBOL E384CL_OUTPUT_SYMBOL o) {
#ifdef E4DCCOMMLIB_STATIC
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
