#include "e4gcommlib.h"

#include <algorithm>

#include "messagedispatcher.h"
#include "messagedispatcher_epatch.h"
#include "messagedispatcher_epatchel03d.h"
#include "messagedispatcher_epatchel03f_4d.h"
#include "messagedispatcher_epatchel04e.h"
#include "messagedispatcher_epatchel03f_4e.h"
#include "messagedispatcher_e4pel04f.h"
#include "messagedispatcher_e4ppatchliner.h"
#include "messagedispatcher_e8ppatchliner.h"
#include "messagedispatcher_e4ppatchliner_el07ab.h"
#include "messagedispatcher_e8ppatchliner_el07ab.h"
#include "messagedispatcher_fakepatch.h"
#include "messagedispatcher_fakep8.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "ftd2xx_win.h"

using namespace std;

namespace e4gCommLib {

/*****************\
 *  Ctor / Dtor  *
\*****************/

CommLib::CommLib() {

}

CommLib::~CommLib() {
    this->disconnect();
}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t CommLib::detectDevices(
        vector <string> &deviceIds) {
    /*! Gets number of devices */
    DWORD numDevs;
    bool devCountOk = getDeviceCount(numDevs);
    if (!devCountOk) {
        return ErrorListDeviceFailed;

    } else if (numDevs == 0) {
        deviceIds.clear();
        return ErrorNoDeviceFound;
    }

//    FT_CreateDeviceInfoList(&numDevs);
//    FT_DEVICE_LIST_INFO_NODE * devInfo;
//    FT_GetDeviceInfoList(devInfo, &numDevs);
//    for (unsigned int i = 0; i < numDevs; i++) {
//        printf("Dev %d:\n", i);
//        printf(" Flags=0x%x\n", devInfo[i].Flags);
//        printf(" Type=0x%x\n", devInfo[i].Type);
//        printf(" ID=0x%x\n", devInfo[i].ID);
//        printf(" LocId=0x%x\n", devInfo[i].LocId);
//        printf(" SerialNumber=%s\n", devInfo[i].SerialNumber);
//        printf(" Description=%s\n", devInfo[i].Description);
//        printf(" ftHandle=0x%x\n", devInfo[i].ftHandle);
//    }

    deviceIds.clear();
    string deviceName;

    /*! Lists all serial numbers */
    for (uint32_t i = 0; i < numDevs; i++) {
        deviceName = getDeviceSerial(i, true);
        if (find(deviceIds.begin(), deviceIds.end(), deviceName) == deviceIds.end()) {
            /*! Adds only new devices (no distinction between channels A and B creates duplicates) */
            if (deviceName.size() > 0) {
                /*! Devices with an open channel are detected wrongly and their name is an empty string */
                deviceIds.push_back(getDeviceSerial(i, true));
            }
        }
    }

    return Success;
}

ErrorCodes_t CommLib::connect(
        string deviceId) {

    ErrorCodes_t ret = Success;
    if (messageDispatcher == nullptr) {
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

        DeviceTuple_t deviceTuple = ftdiEeprom->getDeviceTuple();
        DeviceTypes_t deviceType;

        ret = MessageDispatcher::getDeviceType(deviceTuple, deviceType);
        if (ret != Success) {
            if (ftdiEeprom != nullptr) {
                delete ftdiEeprom;
            }
            return ErrorDeviceTypeNotRecognized;
        }

        switch (deviceType) {
        case DeviceEPatchEL03D:
            messageDispatcher = new MessageDispatcher_ePatchEL03D(deviceId);
            break;

        case DeviceEPatchEL03D_V01:
            messageDispatcher = new MessageDispatcher_ePatchEL03D_V01(deviceId);
            break;

        case DeviceEPatchEL03D_V00:
            messageDispatcher = new MessageDispatcher_ePatchEL03D_V00(deviceId);
            break;

        case DeviceEPatchEL03F_4D:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D(deviceId);
            break;

        case DeviceEPatchEL03F_4D_V02:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D_V02(deviceId);
            break;

        case DeviceEPatchEL03F_4D_V01:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D_V01(deviceId);
            break;

        case DeviceEPatchEL03F_4D_V00:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4D_V00(deviceId);
            break;

        case DeviceEPatchEL03F_4E:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E(deviceId);
            break;

        case DeviceEPatchEL03F_4E_V02:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E_V02(deviceId);
            break;

        case DeviceEPatchEL03F_4E_V01:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E_V01(deviceId);
            break;

        case DeviceEPatchEL03F_4E_V00:
            messageDispatcher = new MessageDispatcher_ePatchEL03F_4E_V00(deviceId);
            break;

        case DeviceEPatchEL04E:
            messageDispatcher = new MessageDispatcher_ePatchEL04E(deviceId);
            break;

        case DeviceEPatchEL04F:
            messageDispatcher = new MessageDispatcher_ePatchEL04F(deviceId);
            break;

        case DeviceE4PEL04F:
            messageDispatcher = new MessageDispatcher_e4PEL04F(deviceId);
            break;

        case DeviceE4PPatchLiner:
            messageDispatcher = new MessageDispatcher_e4PPatchliner(deviceId);
            break;

        case DeviceE8PPatchLiner:
            messageDispatcher = new MessageDispatcher_e8PPatchliner(deviceId);
            break;

        case DeviceE4PPatchLinerEL07AB:
            messageDispatcher = new MessageDispatcher_e4PPatchliner_el07ab(deviceId);
            break;

        case DeviceE8PPatchLinerEL07AB:
            messageDispatcher = new MessageDispatcher_e8PPatchliner_el07ab(deviceId);
            break;

        case DeviceFakePatch:
            messageDispatcher = new MessageDispatcher_fakePatch(deviceId);
            break;

        case DeviceFakeP8:
            messageDispatcher = new MessageDispatcher_fakeP8(deviceId);
            break;

        case DeviceEPatchDlp:
            messageDispatcher = new MessageDispatcher_ePatchDlp(deviceId);
            break;

        default:
            if (ftdiEeprom != nullptr) {
                delete ftdiEeprom;
            }
            return ErrorDeviceTypeNotRecognized;
        }

        if (messageDispatcher != nullptr) {
            ret = messageDispatcher->connect(ftdiEeprom);

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

ErrorCodes_t CommLib::enableRxMessageType(
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

ErrorCodes_t CommLib::disconnect() {
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

ErrorCodes_t CommLib::ping() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->ping();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::abort() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->abort();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnVoltageStimulusOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageStimulusOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnCurrentStimulusOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentStimulusOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnVoltageReaderOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageReaderOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnCurrentReaderOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentReaderOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setChannelsSources(
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

ErrorCodes_t CommLib::setVoltageHoldTuner(
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

ErrorCodes_t CommLib::setCurrentHoldTuner(
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

ErrorCodes_t CommLib::turnOnLsbNoise(
        bool flag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnOnLsbNoise(flag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setVCCurrentRange(
        uint16_t currentRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVCCurrentRange(currentRangeIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setCCCurrentRange(
        uint16_t currentRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCCurrentRange(currentRangeIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setVCVoltageRange(
        uint16_t voltageRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVCVoltageRange(voltageRangeIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setCCVoltageRange(
        uint16_t voltageRangeIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCVoltageRange(voltageRangeIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setSamplingRate(
        uint16_t samplingRateIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setSamplingRate(samplingRateIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setFilterRatio(
        uint16_t filterRatioIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setFilterRatio(filterRatioIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setUpsamplingRatio(
        uint16_t upsamplingRatioIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setUpsamplingRatio(upsamplingRatioIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setDigitalFilter(
        E4GCL_ARGIN double cutoffFrequency,
        E4GCL_ARGIN bool lowPassFlag,
        E4GCL_ARGIN bool activeFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setDigitalFilter(cutoffFrequency, lowPassFlag, activeFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::digitalOffsetCompensation(
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->digitalOffsetCompensation(channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::digitalOffsetCompensationOverride(
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

ErrorCodes_t CommLib::digitalOffsetCompensationInquiry(
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->digitalOffsetCompensationInquiry(channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setVcCurrentOffsetDelta(
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

ErrorCodes_t CommLib::setCcVoltageOffsetDelta(
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

ErrorCodes_t CommLib::zap(
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

ErrorCodes_t CommLib::setVoltageStimulusLpf(
        uint16_t filterIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setVoltageStimulusLpf(filterIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setCurrentStimulusLpf(
        uint16_t filterIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCurrentStimulusLpf(filterIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::enableStimulus(
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

ErrorCodes_t CommLib::turnLedOn(
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

ErrorCodes_t CommLib::setSlave(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setSlave(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setConstantSwitches() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setConstantSwitches();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setCompensationsChannel(
        uint16_t channelIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCompensationsChannel(channelIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnVoltageCompensationsOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnVoltageCompensationsOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnCurrentCompensationsOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCurrentCompensationsOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnPipetteCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnPipetteCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnCCPipetteCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnCCPipetteCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnMembraneCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnMembraneCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnAccessResistanceCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnResistanceCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnAccessResistanceCorrectionOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnResistanceCorrectionOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnAccessResistancePredictionOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnResistancePredictionOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnLeakConductanceCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnLeakConductanceCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::turnBridgeBalanceCompensationOn(
        bool on) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->turnBridgeBalanceCompensationOn(on);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setPipetteCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setPipetteCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setCCPipetteCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCPipetteCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setMembraneCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setMembraneCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistanceCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistanceCorrectionOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCorrectionOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistancePredictionOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setLeakConductanceCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setLeakConductanceCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setBridgeBalanceCompensationOptions(
        uint16_t optionIdx) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setBridgeBalanceCompensationOptions(optionIdx);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setPipetteCapacitance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setPipetteCapacitance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setCCPipetteCapacitance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setCCPipetteCapacitance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setMembraneCapacitance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setMembraneCapacitance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setAccessResistance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistanceCorrectionPercentage(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCorrectionPercentage(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistanceCorrectionLag(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistanceCorrectionLag(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistancePredictionGain(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionGain(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistancePredictionPercentage(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionPercentage(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistancePredictionBandwidthGain(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionBandwidthGain(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setAccessResistancePredictionTau(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setResistancePredictionTau(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setLeakConductance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setLeakConductance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setBridgeBalanceResistance(
        Measurement_t value) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setBridgeBalanceResistance(value);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setDigitalTriggerOutput(
        E4GCL_ARGIN uint16_t triggerIdx,
        E4GCL_ARGIN bool terminator,
        E4GCL_ARGIN bool polarity,
        E4GCL_ARGIN uint16_t triggerId,
        E4GCL_ARGIN Measurement_t delay) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->setDigitalTriggerOutput(triggerIdx, terminator, polarity, triggerId, delay);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setVoltageProtocolStructure(uint16_t protId,
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

ErrorCodes_t CommLib::voltStepTimeStep(
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

ErrorCodes_t CommLib::voltRamp(
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

ErrorCodes_t CommLib::voltSin(
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

ErrorCodes_t CommLib::startProtocol() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->startProtocol();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::setCurrentProtocolStructure(uint16_t protId,
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

ErrorCodes_t CommLib::currStepTimeStep(
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

ErrorCodes_t CommLib::currRamp(
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

ErrorCodes_t CommLib::currSin(
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

ErrorCodes_t CommLib::resetChip(bool reset) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetChip(reset);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::resetDigitalOffsetCompensation(bool reset) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetDigitalOffsetCompensation(reset);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::resetFpga() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->resetFpga();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCalibrationConfiguration(
        CalibrationConfiguration_t * &calibrationConfiguration) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCalibrationConfiguration(calibrationConfiguration);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCalibrationEepromSize(
        uint32_t &size) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCalibrationEepromSize(size);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::writeCalibrationEeprom(
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

ErrorCodes_t CommLib::readCalibrationEeprom(
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

ErrorCodes_t CommLib::getSwitchesStatus(std::vector <uint16_t> &words, std::vector <std::vector <std::string>> &names) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getSwitchesStatus(words, names);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::singleSwitchDebug(uint16_t word, uint16_t bit, bool flag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->singleSwitchDebug(word, bit, flag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::multiSwitchDebug(vector <uint16_t> words) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->multiSwitchDebug(words);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::singleRegisterDebug(uint16_t index, uint16_t value) {
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

ErrorCodes_t CommLib::isDeviceUpgradable(
        E4GCL_ARGOUT std::string &upgradeNotes,
        E4GCL_ARGOUT std::string &notificationTag) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->isDeviceUpgradable(upgradeNotes, notificationTag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getDeviceInfo(
        E4GCL_ARGOUT std::string &deviceId,
        E4GCL_ARGOUT std::string &deviceName,
        E4GCL_ARGOUT uint8_t &deviceVersion,
        E4GCL_ARGOUT uint8_t &deviceSubversion,
        E4GCL_ARGOUT uint32_t &firmwareVersion) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getDeviceInfo(deviceId, deviceName, deviceVersion, deviceSubversion, firmwareVersion);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getDeviceInfo(
        E4GCL_ARGOUT std::string deviceId,
        E4GCL_ARGOUT uint8_t &deviceVersion,
        E4GCL_ARGOUT uint8_t &deviceSubversion,
        E4GCL_ARGOUT uint32_t &firmwareVersion) {
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

ErrorCodes_t CommLib::getNextMessage(
        RxOutput_t &rxOutput) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getNextMessage(rxOutput);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getChannelsNumber(
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

ErrorCodes_t CommLib::getAvailableChannelsSources(
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

ErrorCodes_t CommLib::hasVoltageHoldTuner() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasVoltageHoldTuner();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasCurrentHoldTuner() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasCurrentHoldTuner();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getVCCurrentRanges(
        vector <RangedMeasurement_t> &currentRanges) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVCCurrentRanges(currentRanges);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCCCurrentRanges(
        vector <RangedMeasurement_t> &currentRanges) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCCurrentRanges(currentRanges);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getVCCurrentRange(
        RangedMeasurement_t &currentRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVCCurrentRange(currentRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCCCurrentRange(
        RangedMeasurement_t &currentRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCCurrentRange(currentRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getVCVoltageRanges(
        vector <RangedMeasurement_t> &voltageRanges) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVCVoltageRanges(voltageRanges);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCCVoltageRanges(
        vector <RangedMeasurement_t> &voltageRanges) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCVoltageRanges(voltageRanges);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getVCVoltageRange(
        RangedMeasurement_t &voltageRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVCVoltageRange(voltageRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCCVoltageRange(
        RangedMeasurement_t &voltageRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCVoltageRange(voltageRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getSamplingRates(
        vector <Measurement_t> &samplingRates) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getSamplingRates(samplingRates);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getRealSamplingRates(
        vector <Measurement_t> &samplingRates) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getRealSamplingRates(samplingRates);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getFilterRatios(
        vector <Measurement_t> &filterRatios) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getFilterRatios(filterRatios);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getUpsamplingRatios(
        vector <Measurement_t> &upsamplingRatios) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getUpsamplingRatios(upsamplingRatios);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getVoltageProtocolRange(
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

ErrorCodes_t CommLib::getCurrentProtocolRange(
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

ErrorCodes_t CommLib::getTimeProtocolRange(
        RangedMeasurement_t &timeProtocolRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getTimeProtocolRange(timeProtocolRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getFrequencyProtocolRange(
        RangedMeasurement_t &frequencyProtocolRange) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getFrequencyProtocolRange(frequencyProtocolRange);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getMaxOutputTriggers(
        E4GCL_ARGOUT unsigned int &maxTriggersNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMaxOutputTriggers(maxTriggersNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getOutputTriggersNum(
        E4GCL_ARGOUT unsigned int &triggersNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getOutputTriggersNum(triggersNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getMaxProtocolItems(
        E4GCL_ARGOUT unsigned int &maxItemsNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMaxProtocolItems(maxItemsNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasProtocolStep() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolStep();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasProtocolRamp() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolRamp();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasProtocolSin() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasProtocolSin();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getVoltageStimulusLpfs(
        std::vector <std::string> &filterOptions) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getVoltageStimulusLpfs(filterOptions);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCurrentStimulusLpfs(
        std::vector <std::string> &filterOptions) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCurrentStimulusLpfs(filterOptions);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getLedsNumber(
        uint16_t &ledsNum) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLedsNumber(ledsNum);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getLedsColors(
        vector <uint32_t> &ledsColors) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLedsColors(ledsColors);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasSlaveModality() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasSlaveModality();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getClampingModalities(
        E4GCL_ARGOUT std::vector <uint16_t> &clampingModalities) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getClampingModalities(clampingModalities);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::multimeterStuckHazard(
        E4GCL_ARGOUT bool &stuckFlag) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->multimeterStuckHazard(stuckFlag);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasPipetteCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasPipetteCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasCCPipetteCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasCCPipetteCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasMembraneCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasMembraneCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasAccessResistanceCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasResistanceCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasAccessResistanceCorrection() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasResistanceCorrection();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasAccessResistancePrediction() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasResistancePrediction();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasLeakConductanceCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasLeakConductanceCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::hasBridgeBalanceCompensation() {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->hasBridgeBalanceCompensation();

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getPipetteCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getPipetteCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCCPipetteCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCPipetteCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getMembraneCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMembraneCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getAccessResistanceCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getAccessResistanceCorrectionOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getAccessResistancePredictionOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getLeakConductanceCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLeakConductanceCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getBridgeBalanceCompensationOptions(
        vector <string> &options) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getBridgeBalanceCompensationOptions(options);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getLiquidJunctionControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLiquidJunctionControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getPipetteCapacitanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getPipetteCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getCCPipetteCapacitanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getCCPipetteCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getMembraneCapacitanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getMembraneCapacitanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getAccessResistanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getAccessResistanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getResistanceCorrectionPercentageControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionPercentageControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getResistanceCorrectionLagControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistanceCorrectionLagControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getResistancePredictionGainControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionGainControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getResistancePredictionPercentageControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionPercentageControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getResistancePredictionBandwidthGainControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionBandwidthGainControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getResistancePredictionTauControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getResistancePredictionTauControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getLeakConductanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getLeakConductanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

ErrorCodes_t CommLib::getBridgeBalanceResistanceControl(
        CompensationControl_t &control) {
    ErrorCodes_t ret;
    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->getBridgeBalanceResistanceControl(control);

    } else {
        ret = ErrorDeviceNotConnected;
    }
    return ret;
}

} // namespace e4gCommLib
