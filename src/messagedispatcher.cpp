#include "messagedispatcher.h"

#include <ctime>
#include <thread>
#include <math.h>
#include <algorithm>
#include <unordered_set>

#include "emcropalkellydevice.h"
#include "emcrudbdevice.h"
#include "ezpatchftdidevice.h"
#include "emcrftdidevice.h"
#include "utils.h"

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

}

MessageDispatcher::~MessageDispatcher() {

}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t MessageDispatcher::detectDevices(
        std::vector <std::string> &deviceIds) {

    ErrorCodes_t ret;
    deviceIds.clear();
    std::vector <std::string> demoDeviceIds;
    std::vector <std::string> deviceIdsTemp;

    ErrorCodes_t retTemp = EZPatchFtdiDevice::detectDevices(deviceIdsTemp);
    if (retTemp == Success) {
        for (auto &s : deviceIdsTemp) {
            if (EZPatchFtdiDevice::isDeviceRecognized(s) != Success) {
                continue;
            }
            if (s.starts_with("DEMO")) {
                demoDeviceIds.push_back(s);
            }
            else {
                deviceIds.push_back(s);
            }
        }
    }
    ret = retTemp; /*! Set the first return anyway, so even if all methods return an error the return is set here.
                       Afterwards update only on a Success */

    retTemp = EmcrUdbDevice::detectDevices(deviceIdsTemp);
    if (retTemp == Success) {
        for (auto &s : deviceIdsTemp) {
            if (EmcrUdbDevice::isDeviceRecognized(s) != Success) {
                continue;
            }
            if (s.starts_with("DEMO")) {
                demoDeviceIds.push_back(s);
            }
            else {
                deviceIds.push_back(s);
            }
        }
        ret = retTemp;
    }

    retTemp = EmcrFtdiDevice::detectDevices(deviceIdsTemp);
    if (retTemp == Success) {
        for (auto &s : deviceIdsTemp) {
            if (EmcrFtdiDevice::isDeviceRecognized(s) != Success) {
                continue;
            }
            if (s.starts_with("DEMO")) {
                demoDeviceIds.push_back(s);
            }
            else {
                deviceIds.push_back(s);
            }
        }
        ret = retTemp;
    }

    retTemp = EmcrOpalKellyDevice::detectDevices(deviceIdsTemp);
    if (retTemp == Success) {
        for (auto &s : deviceIdsTemp) {
            if (EmcrOpalKellyDevice::isDeviceRecognized(s) != Success) {
                continue;
            }
            if (s.starts_with("DEMO")) {
                demoDeviceIds.push_back(s);
            }
            else {
                deviceIds.push_back(s);
            }
        }
        ret = retTemp;
    }
    if (ret == Success) {
        deviceIds.insert(deviceIds.end(), demoDeviceIds.begin(), demoDeviceIds.end());
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion) {
    if (EmcrOpalKellyDevice::isDeviceRecognized(deviceId) == Success) {
        EmcrOpalKellyDevice::getDeviceInfo(deviceId, deviceVersion, deviceSubVersion, fwVersion);
        return Success;
    }

    if (EmcrUdbDevice::isDeviceRecognized(deviceId) == Success) {
        EmcrUdbDevice::getDeviceInfo(deviceId, deviceVersion, deviceSubVersion, fwVersion);
        return Success;
    }

    if (EmcrFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        EmcrFtdiDevice::getDeviceInfo(deviceId, deviceVersion, deviceSubVersion, fwVersion);
        return Success;
    }

    if (EZPatchFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        EZPatchFtdiDevice::getDeviceInfo(deviceId, deviceVersion, deviceSubVersion, fwVersion);
        return Success;
    }
    return ErrorDeviceTypeNotRecognized;
}

ErrorCodes_t MessageDispatcher::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher) {
    messageDispatcher = nullptr;
    if (EmcrOpalKellyDevice::isDeviceRecognized(deviceId) == Success) {
        return EmcrOpalKellyDevice::connectDevice(deviceId, messageDispatcher);
    }

    if (EmcrUdbDevice::isDeviceRecognized(deviceId) == Success) {
        return EmcrUdbDevice::connectDevice(deviceId, messageDispatcher);
    }

    if (EmcrFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        return EmcrFtdiDevice::connectDevice(deviceId, messageDispatcher);
    }

    if (EZPatchFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        return EZPatchFtdiDevice::connectDevice(deviceId, messageDispatcher);
    }
    return ErrorDeviceTypeNotRecognized;
}

ErrorCodes_t MessageDispatcher::isDeviceUpgradable(std::string deviceId) {
    if (EmcrOpalKellyDevice::isDeviceRecognized(deviceId) == Success) {
        return ErrorDeviceNotUpgradable;
    }

    if (EmcrUdbDevice::isDeviceRecognized(deviceId) == Success) {
        return EmcrUdbDevice::isDeviceUpgradable(deviceId);
    }

    if (EmcrFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        return EmcrFtdiDevice::isDeviceUpgradable(deviceId);
    }

    if (EZPatchFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        return ErrorDeviceNotUpgradable;
    }
    return ErrorDeviceTypeNotRecognized;
}

ErrorCodes_t MessageDispatcher::upgradeDevice(std::string deviceId) {
    if ((MessageDispatcher::isDeviceUpgradable(deviceId)) != Success) {
        return ErrorDeviceNotUpgradable;
    }

    if (EmcrOpalKellyDevice::isDeviceRecognized(deviceId) == Success) {
        return ErrorDeviceNotUpgradable;
    }

    if (EmcrUdbDevice::isDeviceRecognized(deviceId) == Success) {
        return EmcrUdbDevice::upgradeDevice(deviceId);
    }

    if (EmcrFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        return EmcrFtdiDevice::upgradeDevice(deviceId);
    }

    if (EZPatchFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        return ErrorDeviceNotUpgradable;
    }
    return ErrorDeviceTypeNotRecognized;
}

//ErrorCodes_t MessageDispatcher::getUpgradeProgress(int32_t &progress) {
//    if ((MessageDispatcher::isDeviceUpgradable(deviceId)) != Success) {
//        return ErrorDeviceNotUpgradable;
//    }

//    if (EmcrOpalKellyDevice::isDeviceRecognized(deviceId) == Success) {
//        return ErrorDeviceNotUpgradable;
//    }

//    if (EmcrUdbDevice::isDeviceRecognized(deviceId) == Success) {
//        return EmcrUdbDevice::upgradeDevice(deviceId);
//    }

//    if (EmcrFtdiDevice::isDeviceRecognized(deviceId) == Success) {
//        return EmcrFtdiDevice::upgradeDevice(deviceId);
//    }

//    if (EZPatchFtdiDevice::isDeviceRecognized(deviceId) == Success) {
//        return ErrorDeviceNotUpgradable;
//    }
//    return ErrorDeviceTypeNotRecognized;
//}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t MessageDispatcher::getChannelsOnBoard(uint16_t boardIdx, std::vector <ChannelModel *> & channels) {
    if (boardIdx >= totalBoardsNum) {
        return ErrorValueOutOfRange;
    }
    channels = boardModels[boardIdx]->getChannelsOnBoard();
    return Success;
}

ErrorCodes_t MessageDispatcher::getChannelsOnRow(uint16_t rowIdx, std::vector <ChannelModel *> &channels) {
    if (rowIdx >= channelsPerBoard) {
        return ErrorValueOutOfRange;
    }
    for (auto brd : boardModels) {
        channels.push_back(brd->getChannelsOnBoard()[rowIdx]);
    }
    return Success;
}

std::string MessageDispatcher::getDeviceName() {
    return this->deviceName;
}

ErrorCodes_t MessageDispatcher::sendCommands() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::startProtocol() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::stopProtocol() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::startStateArray() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::zap(std::vector <uint16_t>, Measurement_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::resetAsic(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::resetFpga(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageHoldTuner(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCurrentHoldTuner(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageRampTuner(std::vector <uint16_t>, std::vector <Measurement_t>, std::vector <Measurement_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageHalf(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCurrentHalf(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageReference(Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::subtractLiquidJunctionFromCc(bool flag) {
    subtractLiquidJunctionFromCcFlag = flag;
    for (int i = 0; i < voltageChannelsNum; i++) {
        if (subtractLiquidJunctionFromCcFlag) {
            ccLiquidJunctionVectorApplied[i] = ccLiquidJunctionVector[i];

        } else {
            ccLiquidJunctionVectorApplied[i] = 0;
        }
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::resetOffsetRecalibration(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    std::vector <Measurement_t> offsets(channelIndexes.size());
    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    int offsetIdx = 0;
    for (auto channelIdx : channelIndexes) {
        liquidJunctionStatuses[channelIdx] = LiquidJunctionResetted;
        offsets[offsetIdx++] = originalCalibrationParams.getValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx);
    }
    ljMutexLock.unlock();
    return this->setCalibVcCurrentOffset(channelIndexes, offsets, applyFlag);
}

ErrorCodes_t MessageDispatcher::setLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::resetLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    std::vector <Measurement_t> voltages(channelIndexes.size(), {0.0, liquidJunctionRange.prefix, "V"});
    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    for (auto channelIdx : channelIndexes) {
        liquidJunctionStatuses[channelIdx] = LiquidJunctionResetted;
    }
    ljMutexLock.unlock();
    return this->setLiquidJunctionVoltage(channelIndexes, voltages, applyFlag);
}

ErrorCodes_t MessageDispatcher::setGateVoltages(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setSourceVoltages(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibParams(CalibrationParams_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentGain(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibVcCurrentGain(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentOffset(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibVcCurrentOffset(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibCcVoltageGain(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibCcVoltageGain(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibCcVoltageOffset(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibCcVoltageOffset(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibVcVoltageGain(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibVcVoltageGain(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibVcVoltageOffset(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibVcVoltageOffset(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibCcCurrentGain(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibCcCurrentGain(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibCcCurrentOffset(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibRsCorrOffsetDac(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibRsCorrOffsetDac(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibCcCurrentOffset(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibRShuntConductance(std::vector <uint16_t>, std::vector <Measurement_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::updateCalibRShuntConductance(std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVCCurrentRange(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVCCurrentRange(std::vector <uint16_t>, std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVCVoltageRange(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCCCurrentRange(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCCVoltageRange(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCCVoltageRange(std::vector <uint16_t>, std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setLiquidJunctionRange(uint16_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageStimulusLpf(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCurrentStimulusLpf(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableStimulus(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnChannelsOn(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasCalSw() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCalSwOn(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnVcSwOn(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCcSwOn(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setAdcCore(std::vector <uint16_t>, std::vector <ClampingModality_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableCcStimulus(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setClampingModality(uint32_t, bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setClampingModality(ClampingModality_t mode, bool applyFlag, bool stopProtocolFlag) {
    ClampingModality_t currentMode;
    const auto ret = getClampingModality(currentMode);
    if (currentMode == mode) {
        return ret;
    }
    auto iter = std::find(clampingModalitiesArray.begin(), clampingModalitiesArray.end(), mode);
    if (iter == clampingModalitiesArray.end()) {
        return ErrorValueOutOfRange;
    }
    return this->setClampingModality((uint32_t)(iter-clampingModalitiesArray.begin()), applyFlag, stopProtocolFlag);
}

ErrorCodes_t MessageDispatcher::setSourceForVoltageChannel(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setSourceForCurrentChannel(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::readoutOffsetRecalibration(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::liquidJunctionCompensation(std::vector <uint16_t>, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::digitalOffsetCompensation(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    return liquidJunctionCompensation(channelIndexes, onValues, applyFlag);
}

ErrorCodes_t MessageDispatcher::setCurrentTracking(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool enable) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setAdcFilter(bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setSamplingRate(uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setDownsamplingRatio(uint32_t ratio) {
    if (ratio == 0) {
        return ErrorValueOutOfRange;
    }

    if (ratio == 1) {
        downsamplingFlag = false;

    } else {
        downsamplingFlag = true;
    }
    selectedDownsamplingRatio = ratio;
    this->computeRawDataFilterCoefficients();
    return Success;
}

ErrorCodes_t MessageDispatcher::setRawDataFilter(Measurement_t cutoffFrequency, bool lowPassFlag, bool activeFlag) {
    if ((cutoffFrequency.value <= 0.0) || (cutoffFrequency >= samplingRate*0.5)) {
        return ErrorValueOutOfRange;
    }
    rawDataFilterCutoffFrequency = cutoffFrequency;
    rawDataFilterLowPassFlag = lowPassFlag;
    rawDataFilterActiveFlag = activeFlag;
    this->computeRawDataFilterCoefficients();

    return Success;
}

ErrorCodes_t MessageDispatcher::turnVoltageReaderOn(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCurrentReaderOn(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnVoltageStimulusOn(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCurrentStimulusOn(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolStructure(uint16_t, uint16_t, uint16_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolStep(uint16_t, uint16_t, uint16_t, bool, Measurement_t, Measurement_t, Measurement_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolRamp(uint16_t, uint16_t, uint16_t, bool, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolSin(uint16_t, uint16_t, uint16_t, bool, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolStructure(uint16_t, uint16_t, uint16_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolStep(uint16_t, uint16_t, uint16_t, bool, Measurement_t, Measurement_t, Measurement_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolRamp(uint16_t, uint16_t, uint16_t, bool, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolSin(uint16_t, uint16_t, uint16_t, bool, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setStateArrayStructure(int, int, Measurement_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setSateArrayState(int, Measurement_t, bool, Measurement_t, int, Measurement_t, Measurement_t, int, bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setStateArrayEnabled(int, bool) {
    return ErrorFeatureNotImplemented;
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

ErrorCodes_t MessageDispatcher::getNextMessage(RxOutput_t &, int16_t *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::purgeData() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::convertVoltageValue(int16_t intValue, double &fltValue) {
    fltValue = voltageResolutions[0]*(double)intValue;

    return Success;
}

ErrorCodes_t MessageDispatcher::convertVoltageValue(int16_t intValue, uint16_t channelIdx, double &fltValue) {
    fltValue = voltageResolutions[channelIdx]*(double)intValue;

    return Success;
}

ErrorCodes_t MessageDispatcher::convertCurrentValue(int16_t intValue, double &fltValue) {
    fltValue = currentResolutions[0]*(double)intValue;

    return Success;
}

ErrorCodes_t MessageDispatcher::convertCurrentValue(int16_t intValue, uint16_t channelIdx, double &fltValue) {
    fltValue = currentResolutions[channelIdx]*(double)intValue;

    return Success;
}

ErrorCodes_t MessageDispatcher::convertVoltageValues(int16_t * intValues, double * fltValues, int valuesNum) {
    int idx = 0;
    while (idx < valuesNum) {
        for (int chIdx = 0; chIdx < voltageChannelsNum; chIdx++) {
            fltValues[idx] = voltageResolutions[chIdx]*(double)intValues[idx];
            idx++;
        }
    }

    return Success;
}

ErrorCodes_t MessageDispatcher::convertCurrentValues(int16_t * intValues, double * fltValues, int valuesNum) {
    int idx = 0;
    while (idx < valuesNum) {
        for (int chIdx = 0; chIdx < currentChannelsNum; chIdx++) {
            fltValues[idx] = currentResolutions[chIdx]*(double)intValues[idx];
            idx++;
        }
    }

    return Success;
}

ErrorCodes_t MessageDispatcher::convertTemperatureValues(int16_t * intValues, double * fltValues) {
    for (int idx = 0; idx < temperatureChannelsNum; idx++) {
        fltValues[idx] = temperatureChannelsRanges[idx].step*(double)intValues[idx];
    }

    return Success;
}

ErrorCodes_t MessageDispatcher::getReadoutOffsetRecalibrationStatuses(std::vector <uint16_t> channelIndexes, std::vector <OffsetRecalibStatus_t> &statuses) {
    if (offsetRecalibStatuses.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    statuses.resize(channelIndexes.size());
    for (int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
        statuses[channelIdx] = offsetRecalibStatuses[channelIndexes[channelIdx]];
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionVoltages(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> &voltages) {
    if (selectedLiquidJunctionVector.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    voltages.resize(channelIndexes.size());
    for (int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
        voltages[channelIdx] = selectedLiquidJunctionVector[channelIndexes[channelIdx]];
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionStatuses(std::vector <uint16_t> channelIndexes, std::vector <LiquidJunctionStatus_t> &statuses) {
    if (liquidJunctionStatuses.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    statuses.resize(channelIndexes.size());
    for (int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
        statuses[channelIdx] = liquidJunctionStatuses[channelIndexes[channelIdx]];
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getVoltageHoldTuner(std::vector <Measurement_t> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getVoltageRampTunerFeatures(std::vector <RangedMeasurement_t> &voltageRanges, RangedMeasurement_t &durationRange) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasGateVoltages() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasSourceVoltages() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getGateVoltagesFeatures(RangedMeasurement_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getSourceVoltagesFeatures(RangedMeasurement_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getChannelNumberFeatures(uint16_t &voltageChannelNumberFeatures, uint16_t &currentChannelNumberFeatures) {
    voltageChannelNumberFeatures = voltageChannelsNum;
    currentChannelNumberFeatures = currentChannelsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getChannelNumberFeatures(int &voltageChannelNumberFeatures, int &currentChannelNumberFeatures) {
    voltageChannelNumberFeatures = (int)voltageChannelsNum;
    currentChannelNumberFeatures = (int)currentChannelsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getChannelNumberFeatures(int &voltageChannelNumberFeatures, int &currentChannelNumberFeatures, int &gpChannelNumberFeatures) {
    voltageChannelNumberFeatures = (int)voltageChannelsNum;
    currentChannelNumberFeatures = (int)currentChannelsNum;
    gpChannelNumberFeatures = (int)gpChannelsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getAvailableChannelsSourcesFeatures(ChannelSources_t &voltageSourcesIdxs, ChannelSources_t &currentSourcesIdxs) {
    voltageSourcesIdxs = availableVoltageSourcesIdxs;
    currentSourcesIdxs = availableCurrentSourcesIdxs;
    return Success;
}

ErrorCodes_t MessageDispatcher::isEpisodic() {
    if (canDoEpisodic) {
        return Success;
    }
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasProperHeaderPackets() {
    if (properHeaderPackets) {
        return Success;
    }
    return ErrorFeatureNotImplemented;
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
    }
    clampingModalitiesFeatures = clampingModalitiesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getClampingModality(ClampingModality_t &clampingModality) {
    if (clampingModalitiesArray.empty()) {
        clampingModality = ClampingModality_t::UNDEFINED_CLAMP;
        return ErrorFeatureNotImplemented;
    }
    clampingModality = (ClampingModality_t)selectedClampingModality;
    return Success;
}

ErrorCodes_t MessageDispatcher::getClampingModalityIdx(uint32_t &idx) {
    if (clampingModalitiesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedClampingModalityIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasIndependentVCCurrentRanges() {
    return (independentVcCurrentRanges ? Success : ErrorFeatureNotImplemented);
}

ErrorCodes_t MessageDispatcher::hasIndependentCCVoltageRanges() {
    return (independentCcVoltageRanges ? Success : ErrorFeatureNotImplemented);
}

ErrorCodes_t MessageDispatcher::getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges, uint16_t &defaultRangeIdx) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentRanges = vcCurrentRangesArray;
    defaultRangeIdx = defaultVcCurrentRangeIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges, uint16_t &defaultRangeIdx) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageRanges = vcVoltageRangesArray;
    defaultRangeIdx = defaultVcVoltageRangeIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges, uint16_t &defaultRangeIdx) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentRanges = ccCurrentRangesArray;
    defaultRangeIdx = defaultCcCurrentRangeIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges, uint16_t &defaultRangeIdx) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageRanges = ccVoltageRangesArray;
    defaultRangeIdx = defaultCcVoltageRangeIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentRange(RangedMeasurement_t &range) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    range = vcCurrentRangesArray[selectedVcCurrentRangeIdx[0]];
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentRange(std::vector <RangedMeasurement_t> &ranges) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    ranges.clear();
    for (auto chIdx : allChannelIndexes) {
        ranges.push_back(vcCurrentRangesArray[selectedVcCurrentRangeIdx[chIdx]]);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageRange(RangedMeasurement_t &range) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    range = vcVoltageRangesArray[selectedVcVoltageRangeIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionRange(RangedMeasurement_t &range) {
    if (liquidJunctionRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    range = liquidJunctionRangesArray[selectedLiquidJunctionRangeIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentRange(RangedMeasurement_t &range) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    range = ccCurrentRangesArray[selectedCcCurrentRangeIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRange(RangedMeasurement_t &range) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    range = ccVoltageRangesArray[selectedCcVoltageRangeIdx[0]];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRange(std::vector <RangedMeasurement_t> &ranges) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    ranges.clear();
    for (auto chIdx : allChannelIndexes) {
        ranges.push_back(ccVoltageRangesArray[selectedCcVoltageRangeIdx[chIdx]]);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentRangeIdx(uint32_t &idx) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedVcCurrentRangeIdx[0];
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentRangeIdx(std::vector <uint32_t> &idxs) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idxs.clear();
    for (auto chIdx : allChannelIndexes) {
        idxs.push_back(selectedVcCurrentRangeIdx[chIdx]);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageRangeIdx(uint32_t &idx) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedVcVoltageRangeIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentRangeIdx(uint32_t &idx) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedCcCurrentRangeIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRangeIdx(uint32_t &idx) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedCcVoltageRangeIdx[0];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRangeIdx(std::vector <uint32_t> &idxs) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idxs.clear();
    for (auto chIdx : allChannelIndexes) {
        idxs.push_back(selectedCcVoltageRangeIdx[chIdx]);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageRange(RangedMeasurement_t &range) {
    range = voltageRanges[0];
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageRange(std::vector <RangedMeasurement_t> &ranges) {
    ranges.clear();
    for (auto chIdx : allChannelIndexes) {
        ranges.push_back(voltageRanges[chIdx]);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getCurrentRange(RangedMeasurement_t &range) {
    range = currentRanges[0];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCurrentRange(std::vector <RangedMeasurement_t> &ranges) {
    ranges.clear();
    for (auto chIdx : allChannelIndexes) {
        ranges.push_back(currentRanges[chIdx]);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxVCCurrentRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(vcCurrentRangesArray.begin(), std::max_element(vcCurrentRangesArray.begin(), vcCurrentRangesArray.end()));
    range = vcCurrentRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getMinVCCurrentRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(vcCurrentRangesArray.begin(), std::min_element(vcCurrentRangesArray.begin(), vcCurrentRangesArray.end()));
    range = vcCurrentRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxVCVoltageRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(vcVoltageRangesArray.begin(), std::max_element(vcVoltageRangesArray.begin(), vcVoltageRangesArray.end()));
    range = vcVoltageRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getMinVCVoltageRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(vcVoltageRangesArray.begin(), std::min_element(vcVoltageRangesArray.begin(), vcVoltageRangesArray.end()));
    range = vcVoltageRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxCCCurrentRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(ccCurrentRangesArray.begin(), std::max_element(ccCurrentRangesArray.begin(), ccCurrentRangesArray.end()));
    range = ccCurrentRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getMinCCCurrentRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(ccCurrentRangesArray.begin(), std::min_element(ccCurrentRangesArray.begin(), ccCurrentRangesArray.end()));
    range = ccCurrentRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxCCVoltageRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(ccVoltageRangesArray.begin(), std::max_element(ccVoltageRangesArray.begin(), ccVoltageRangesArray.end()));
    range = ccVoltageRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getMinCCVoltageRange(RangedMeasurement_t &range, uint32_t &idx) {
    if (ccVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = (uint32_t)std::distance(ccVoltageRangesArray.begin(), std::min_element(ccVoltageRangesArray.begin(), ccVoltageRangesArray.end()));
    range = ccVoltageRangesArray[idx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getTemperatureChannelsFeatures(std::vector <std::string> &names, std::vector <RangedMeasurement_t> &ranges) {
    if (temperatureChannelsNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    names = temperatureChannelsNames;
    ranges = temperatureChannelsRanges;
    return Success;
}

ErrorCodes_t MessageDispatcher::getSamplingRatesFeatures(std::vector <Measurement_t> &samplingRates) {
    if (realSamplingRatesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    samplingRates = realSamplingRatesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getSamplingRate(Measurement_t &samplingRate) {
    if (realSamplingRatesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    samplingRate = realSamplingRatesArray[selectedSamplingRateIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getSamplingRateIdx(uint32_t &idx) {
    if (realSamplingRatesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedSamplingRateIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getRealSamplingRatesFeatures(std::vector <Measurement_t> &realSamplingRates) {
    if (realSamplingRatesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    realSamplingRates = realSamplingRatesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxDownsamplingRatioFeature(uint32_t &ratio) {
    ratio = maxDownSamplingRatio;
    return Success;
}

ErrorCodes_t MessageDispatcher::getDownsamplingRatio(uint32_t &ratio) {
    ratio = selectedDownsamplingRatio;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageFilters(std::vector <Measurement_t> &filters) {
    if (vcVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filters = vcVoltageFiltersArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentFilters(std::vector <Measurement_t> &filters) {
    if (vcCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filters = vcCurrentFiltersArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageFilters(std::vector <Measurement_t> &filters) {
    if (ccVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filters = ccVoltageFiltersArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentFilters(std::vector <Measurement_t> &filters) {
    if (ccCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filters = ccCurrentFiltersArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageFilter(Measurement_t &filter) {
    if (vcVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filter = vcVoltageFiltersArray[selectedVcVoltageFilterIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentFilter(Measurement_t &filter) {
    if (vcCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filter = vcCurrentFiltersArray[selectedVcCurrentFilterIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageFilter(Measurement_t &filter) {
    if (ccVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filter = ccVoltageFiltersArray[selectedCcVoltageFilterIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentFilter(Measurement_t &filter) {
    if (ccCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    filter = ccCurrentFiltersArray[selectedCcCurrentFilterIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageFilterIdx(uint32_t &idx) {
    if (vcVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedVcVoltageFilterIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentFilterIdx(uint32_t &idx) {
    if (vcCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedVcCurrentFilterIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageFilterIdx(uint32_t &idx) {
    if (ccVoltageFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedCcVoltageFilterIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentFilterIdx(uint32_t &idx) {
    if (ccCurrentFiltersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    idx = selectedCcCurrentFilterIdx;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasChannelSwitches() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasStimulusSwitches() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasOffsetCompensation() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasStimulusHalf() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCalibParams(CalibrationParams_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCalibFileNames(std::vector <std::string> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCalibFilesFlags(std::vector <std::vector <bool> > &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCalibMappingFileDir(std::string &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCalibMappingFilePath(std::string &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCalibrationMode(bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCalibrationEepromSize(uint32_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::writeCalibrationEeprom(std::vector <uint32_t>, std::vector <uint32_t>, std::vector <uint32_t>) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::readCalibrationEeprom(std::vector <uint32_t> &, std::vector <uint32_t>, std::vector <uint32_t>) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getVoltageProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range) {
    if (vcVoltageRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    range = vcVoltageRangesArray[rangeIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getCurrentProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range) {
    if (ccCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    range = ccCurrentRangesArray[rangeIdx];
    return Success;
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
    }
    num = protocolMaxItemsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasProtocols() {
    if (protocolMaxItemsNum < 1) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::hasProtocolStepFeature() {
    if (!(voltageProtocolStepImplemented || currentProtocolStepImplemented)) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::hasProtocolRampFeature() {
    if (!(voltageProtocolRampImplemented || currentProtocolRampImplemented)) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::hasProtocolSinFeature() {
    if (!(voltageProtocolSinImplemented || currentProtocolSinImplemented)) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::isStateArrayAvailable() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getZapFeatures(RangedMeasurement_t &) {
    return ErrorFeatureNotImplemented;
}

/*********************\
 *  Private methods  *
\*********************/

void MessageDispatcher::createDebugFiles() {
    if (debugLevelEnabled(DebugLevelTx)) {
        if (txFid == nullptr) {
            createDebugFile(txFid, "e384CommLib_tx");
        }
    }

    if (debugLevelEnabled(DebugLevelRxRaw)) {
        if (rxRawFid == nullptr) {
            createDebugFile(rxRawFid, "e384CommLib_rxRaw");
        }
    }

    if (debugLevelEnabled(DebugLevelRx)) {
        if (rxFid == nullptr) {
            createDebugFile(rxFid, "e384CommLib_rx");
        }
    }

    if (debugLevelEnabled(DebugLevelDigitallOffsetCompensation)) {
        if (ljFid == nullptr) {
            createDebugFile(ljFid, "e384CommLib_lj");
        }
    }

    if (debugLevelEnabled(DebugLevelTx) || debugLevelEnabled(DebugLevelRx)) {
        startPrintfTime = std::chrono::steady_clock::now();
    }

    if (debugLevelEnabled(DebugLevelTemperature)) {
        if (tempFid == nullptr) {
            createDebugFile(tempFid, "e384CommLib_temp");
            std::fprintf(tempFid, "TSet TInt TExt Speed\n");
            std::fflush(tempFid);
        }
    }
}

void MessageDispatcher::initializeVariables() {
    channelsPerBoard = currentChannelsNum/totalBoardsNum;
    this->fillChannelList(totalBoardsNum, currentChannelsNum/totalBoardsNum);

    /*! Allocate memory for raw data filters */
    this->initializeRawDataFilterVariables();

    this->initializeLiquidJunction();

    this->initializeCalibration();
}

ErrorCodes_t MessageDispatcher::deviceConfiguration() {
    if (this->hasProperHeaderPackets() == Success) {
        this->enableRxMessageType(MsgTypeIdAcquisitionHeader, true);
    }
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

    storedVcCurrentRangeIdx = selectedVcCurrentRangeIdx;

    if (clampingModalitiesArray[defaultClampingModalityIdx] == VOLTAGE_CLAMP) {
        /*! Initialization in voltage clamp */
        this->setClampingModality(VOLTAGE_CLAMP, false, false);
        this->enableStimulus(allChannelIndexes, allTrue, false);
        this->turnChannelsOn(allChannelIndexes, allTrue, false);
        this->setVoltageHoldTuner(allChannelIndexes, selectedVoltageHoldVector, false);
        this->setLiquidJunctionVoltage(allChannelIndexes, selectedLiquidJunctionVector, false);
        this->setSamplingRate(defaultSamplingRateIdx, false);
        this->setVCCurrentRange(defaultVcCurrentRangeIdx, false);
        this->setVCVoltageRange(defaultVcVoltageRangeIdx, false);
        this->setVoltageStimulusLpf(selectedVcVoltageFilterIdx, false);
        this->setGateVoltages(boardIndexes, selectedGateVoltageVector, false);
        this->setSourceVoltages(boardIndexes, selectedSourceVoltageVector, false);
        this->liquidJunctionCompensation(allChannelIndexes, allFalse, false);
        this->turnCalSwOn(allChannelIndexes, allFalse, false);

        if (vcCurrentRangesNum > 0) {
            this->updateCalibVcCurrentGain(allChannelIndexes, false);
            this->updateCalibVcCurrentOffset(allChannelIndexes, false);
            this->updateCalibRShuntConductance(allChannelIndexes, false);
        }

        if (vcVoltageRangesNum > 0) {
            this->updateCalibVcVoltageGain(allChannelIndexes, false);
            this->updateCalibVcVoltageOffset(allChannelIndexes, false);
        }

    } else {
        /*! Initialization in current clamp */
        /*! \todo FCON ... Noone defaults in current clamp, why bother? */
    }

    /*! Make sure that at the beginning all the constant values tha might not be written later on are sent to the FPGA */
    this->sendCommands();

    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    return Success;
}

void MessageDispatcher::deinitializeVariables() {
    this->deinitializeCalibration();
    this->deInitializeRawDataFilterVariables();
    this->flushBoardList();
}

void MessageDispatcher::closeDebugFiles() {
    if (debugLevelEnabled(DebugLevelTx)) {
        fclose(txFid);
    }

    if (debugLevelEnabled(DebugLevelRxRaw)) {
        fclose(rxRawFid);
    }

    if (debugLevelEnabled(DebugLevelRx)) {
        fclose(rxFid);
    }

    if (debugLevelEnabled(DebugLevelDigitallOffsetCompensation)) {
        fclose(ljFid);
    }

    if (debugLevelEnabled(DebugLevelTemperature)) {
        fclose(tempFid);
    }
}

void MessageDispatcher::initializeCalibration() {

}

void MessageDispatcher::deinitializeCalibration() {

}

void MessageDispatcher::initializeLiquidJunction() {
    offsetRecalibStatuses.resize(currentChannelsNum);
    std::fill(offsetRecalibStatuses.begin(), offsetRecalibStatuses.end(), OffsetRecalibNotPerformed);
    offsetRecalibStates.resize(currentChannelsNum);
    std::fill(offsetRecalibStates.begin(), offsetRecalibStates.end(), OffsetRecalibIdle);

    if (liquidJunctionRangesArray.empty()) {
        liquidJunctionRangesArray = vcVoltageRangesArray;
    }
    liquidJunctionRange = liquidJunctionRangesArray[defaultLiquidJunctionRangeIdx];
    selectedLiquidJunctionVector.resize(currentChannelsNum);
    fill(selectedLiquidJunctionVector.begin(), selectedLiquidJunctionVector.end(), 0.0*liquidJunctionRange.getMax());

    ccLiquidJunctionVector.resize(currentChannelsNum);
    fill(ccLiquidJunctionVector.begin(), ccLiquidJunctionVector.end(), 0);
    ccLiquidJunctionVectorApplied.resize(currentChannelsNum);
    fill(ccLiquidJunctionVectorApplied.begin(), ccLiquidJunctionVectorApplied.end(), 0);
}

bool MessageDispatcher::checkProtocolValidity(std::string &) {
    /*! \todo FCON da riempire */
    return false;
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

void MessageDispatcher::deInitializeRawDataFilterVariables() {
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
}

void MessageDispatcher::computeRawDataFilterCoefficients() {
    bool enableFilter;
    double cutoffFrequency;
    bool lowPassFlag;

    if (downsamplingFlag) {
        rawDataFilterCutoffFrequencyOverride.convertValue(UnitPfxNone);
        rawDataFilterCutoffFrequencyOverride.value = samplingRate.getNoPrefixValue()*0.25/(double)selectedDownsamplingRatio;
        rawDataFilterLowPassFlagOverride = true;

    } else {
        rawDataFilterCutoffFrequencyOverride.convertValue(UnitPfxTera);
        rawDataFilterCutoffFrequencyOverride.value = 1.0e9;
        rawDataFilterLowPassFlagOverride = false;
    }
    lowPassFlag = rawDataFilterLowPassFlag || rawDataFilterLowPassFlagOverride; // When the downsampling is enabled the filter can be set only as low pass
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
            if (lowPassFlag) {
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
            if (lowPassFlag) {
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
        if (lowPassFlag) {
            kx = 1.0-ky;

        } else {
            kx = 1.0+ky;
        }
        if (rawDataFilterVoltageFlag) {

            /*! Denominators */
            iirVDen[0] = 1.0;
            iirVDen[1] = -ky;

            /*! Gains and numerators */
            if (lowPassFlag) {
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
            if (lowPassFlag) {
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

uint32_t MessageDispatcher::getSamplingRateModesNum() {
    std::unordered_set <uint32_t> uniqueValues;
    for (const auto& pair : sr2srm) {
        uniqueValues.insert(pair.second);
    }
    return uniqueValues.size();
}

ErrorCodes_t MessageDispatcher::enableCompensation(std::vector <uint16_t>, CompensationTypes_t, std::vector <bool>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableVcCompensations(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::enableCcCompensations(bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCompValues(std::vector <uint16_t>, CompensationUserParams_t, std::vector <double>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCompRanges(std::vector <uint16_t>, CompensationUserParams_t, std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCompOptions(std::vector <uint16_t>, CompensationTypes_t, std::vector <uint16_t>, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCoolingFansSpeed(Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setTemperatureControl(Measurement_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setTemperatureControlPid(PidParams_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCustomFlag(uint16_t, bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCustomOption(uint16_t, uint16_t, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCustomDouble(uint16_t, double, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setDebugBit(uint16_t, uint16_t, bool, bool) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setDebugWord(uint16_t, uint16_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::setDebugTrigger(uint16_t bitOffset) {
    return e384CommLib::ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasCompFeature(CompensationUserParams_t feature) {
    if (compensationControls[feature].empty()) {
        return ErrorFeatureNotImplemented;
    }

    if (!compensationControls[0][feature].implemented) {
        return ErrorFeatureNotImplemented;
    }

    return Success;
}

ErrorCodes_t MessageDispatcher::getCompFeatures(CompensationUserParams_t feature, std::vector <RangedMeasurement_t> &compensationRanges, double &defaultParamValue) {
    if (compensationControls[feature].empty()) {
        return ErrorFeatureNotImplemented;
    }

    if (!compensationControls[feature][0].implemented) {
        return ErrorFeatureNotImplemented;
    }

    for (int chIdx = 0; chIdx < currentChannelsNum; chIdx++) {
        compensationRanges[chIdx] = compensationControls[feature][chIdx].getCompensableRange();
        defaultParamValue = compensationControls[feature][chIdx].value;
    }

    return Success;
}

ErrorCodes_t MessageDispatcher::getCompOptionsFeatures(CompensationTypes_t, std::vector <std::string> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompValueMatrix(std::vector <std::vector <double> > &matrix) {
    if (compValueMatrix.empty()) {
        return ErrorFeatureNotImplemented;
    }
    matrix = compValueMatrix;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCompensationEnables(std::vector <uint16_t>, CompensationTypes_t, std::vector <bool> &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCompensationControl(CompensationUserParams_t, CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCoolingFansSpeedRange(RangedMeasurement_t &range) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCustomFlags(std::vector <std::string> &customFlags, std::vector <bool> &customFlagsDefault) {
    if (customFlagsNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    customFlags = customFlagsNames;
    customFlagsDefault = this->customFlagsDefault;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCustomOptions(std::vector <std::string> &customOptions, std::vector <std::vector <std::string> > &customOptionsDescriptions, std::vector <uint16_t> &customOptionsDefault) {
    if (customOptionsNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    customOptions = customOptionsNames;
    customOptionsDescriptions = this->customOptionsDescriptions;
    customOptionsDefault = this->customOptionsDefault;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCustomDoubles(std::vector <std::string> &customDoubles, std::vector <RangedMeasurement_t> &customDoublesRanges, std::vector <double> &customDoublesDefault) {
    if (customDoublesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    customDoubles = customDoublesNames;
    customDoublesRanges = this->customDoublesRanges;
    customDoublesDefault = this->customDoublesDefault;
    return Success;
}

std::vector <double> MessageDispatcher::user2AsicDomainTransform(int, std::vector <double>) {
    return std::vector <double>();
}

std::vector <double> MessageDispatcher::asic2UserDomainTransform(int, std::vector <double>, double, double) {
    return std::vector <double>();
}

ErrorCodes_t MessageDispatcher::asic2UserDomainCompensable(int, std::vector <double>, std::vector <double>) {
    return ErrorFeatureNotImplemented;
}

void MessageDispatcher::fillBoardList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard) {
    this->boardModels.resize(numOfBoards);
    for (uint16_t i = 0; i< numOfBoards; i++ ) {
        BoardModel* board = new BoardModel;
        board->setId(i);
        board->fillChannelList(numOfChannelsOnBoard);
        this->boardModels[i] = board;
    }
}

void MessageDispatcher::fillChannelList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard) {
    if (this->boardModels.size() == 0) {
        this->fillBoardList(numOfBoards, numOfChannelsOnBoard);
    }
    uint16_t newChannelId = 0;
    channelModels.resize(numOfChannelsOnBoard*numOfBoards);
    for (uint16_t i = 0; i< numOfBoards; i++ ) {
        for (uint16_t j = 0; j< numOfChannelsOnBoard; j++) {
            this->channelModels[newChannelId] = this->boardModels[i]->getChannelsOnBoard()[j];
            newChannelId++;
        }
    }
}

void MessageDispatcher::flushBoardList() {
    size_t numOfBoards = this->boardModels.size();
    for (uint16_t i = 0; i< numOfBoards; i++ ) {
        if (this->boardModels[i] != nullptr) {
            delete this->boardModels[i];
        }
    }
    boardModels.clear();
    channelModels.clear();
}
