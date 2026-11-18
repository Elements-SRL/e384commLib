#include "messagedispatcher.h"

#include <ctime>
#include <thread>
#include <math.h>
#include <algorithm>
#include <unordered_set>

#include "commandcoder.h"
#include "framemanager.h"
#include "tomlcalibrationmanager.h"
#include "csvcalibrationmanager.h"
#include "speed_test.h"

#include "emcropalkellydevice.h"
#include "emcrudbdevice.h"
#include "emcrftdidevice.h"
#include "utils.h"

static std::list <MessageDispatcher *> connectedDevices;

/*****************\
 *  Ctor / Dtor  *
\*****************/

MessageDispatcher::MessageDispatcher(std::string deviceId) :
    deviceId(deviceId) {

    txStatus = new CommandStatus();

    /*! Initialize rx word offsets and lengths with default values */
    rxWordOffsets.resize(RxMessageNum);
    rxWordLengths.resize(RxMessageNum);

    fill(rxWordOffsets.begin(), rxWordOffsets.end(), 0xFFFF);
    fill(rxWordLengths.begin(), rxWordLengths.end(), 0x0000);
}

MessageDispatcher::~MessageDispatcher() {
    /*! \todo FCON probabilmente crasha per via di alcuni coders che vengono sovrascritti. Meglio usare smart pointers */
    // for (auto coder : coders) {
    //     delete coder;
    // }
    coders.clear();
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

    ErrorCodes_t retTemp = EmcrUdbDevice::detectDevices(deviceIdsTemp);
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
    }
    ret = retTemp; /*! Set the first return anyway, so even if all methods return an error the return is set here.
                       Afterwards update only on a Success */

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

ErrorCodes_t MessageDispatcher::getFtdiDriverStatus(std::string &details) {
    return EmcrFtdiDevice::getDriverStatus(details);
}

ErrorCodes_t MessageDispatcher::getCyUsbDriverStatus(std::string &details) {
    return EmcrUdbDevice::getDriverStatus(details);
}

ErrorCodes_t MessageDispatcher::getOkDriverStatus(std::string &details) {
    return EmcrOpalKellyDevice::getDriverStatus(details);
}

ErrorCodes_t MessageDispatcher::listAllDevices(
    std::vector <std::string> &deviceIds) {

    deviceIds.clear();
    std::unordered_set <std::string> deviceIdSet;

    for (auto & d : connectedDevices) {
        auto okd = dynamic_cast <EmcrOpalKellyDevice *> (d);
        if (okd == nullptr) {
            continue;
        }
        if (okd->isDeviceConnected()) {
            deviceIdSet.emplace(d->getDeviceSerial());
        }
    }

    std::vector <std::string> deviceIdsTemp;

    if (EmcrUdbDevice::detectDevices(deviceIdsTemp) == Success) {
        for (auto &s : deviceIdsTemp) {
            if (!s.starts_with("DEMO")) {
                deviceIdSet.emplace(s);
            }
        }
    }

    if (EmcrFtdiDevice::detectDevices(deviceIdsTemp) == Success) {
        for (auto &s : deviceIdsTemp) {
            if (!s.starts_with("DEMO")) {
                deviceIdSet.emplace(s);
            }
        }
    }

    if (EmcrOpalKellyDevice::detectDevices(deviceIdsTemp) == Success) {
        for (auto &s : deviceIdsTemp) {
            if (!s.starts_with("DEMO")) {
                deviceIdSet.emplace(s);
            }
        }
    }

    deviceIds.assign(deviceIdSet.begin(), deviceIdSet.end());
    if (deviceIds.empty()) {
        return ErrorNoDeviceFound;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwMajor, unsigned int &fwMinor, unsigned int &fwPatch) {
    if (EmcrOpalKellyDevice::isDeviceRecognized(deviceId) == Success) {
        std::vector <uint8_t> userInfo;
        EmcrOpalKellyDevice::getDeviceInfo(deviceId, deviceVersion, deviceSubVersion, fwMajor, fwMinor, fwPatch, userInfo);
        return Success;
    }

    if (EmcrUdbDevice::isDeviceRecognized(deviceId) == Success) {
        EmcrUdbDevice::getDeviceInfo(deviceId, deviceVersion, deviceSubVersion, fwMajor, fwMinor, fwPatch);
        return Success;
    }

    if (EmcrFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        EmcrFtdiDevice::getDeviceInfo(deviceId, deviceVersion, deviceSubVersion, fwMajor, fwMinor, fwPatch);
        return Success;
    }
    return ErrorDeviceTypeNotRecognized;
}

ErrorCodes_t MessageDispatcher::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher) {
    messageDispatcher = nullptr;
    ErrorCodes_t ret = ErrorDeviceTypeNotRecognized;
    if (EmcrOpalKellyDevice::isDeviceRecognized(deviceId) == Success) {
        if ((ret = EmcrOpalKellyDevice::connectDevice(deviceId, messageDispatcher)) == Success) {
            connectedDevices.push_back(messageDispatcher);
        }
        return ret;
    }

    if (EmcrUdbDevice::isDeviceRecognized(deviceId) == Success) {
        if ((ret = EmcrUdbDevice::connectDevice(deviceId, messageDispatcher)) == Success) {
            connectedDevices.push_back(messageDispatcher);
        }
        return ret;
    }

    if (EmcrFtdiDevice::isDeviceRecognized(deviceId) == Success) {
        if ((ret = EmcrFtdiDevice::connectDevice(deviceId, messageDispatcher)) == Success) {
            connectedDevices.push_back(messageDispatcher);
        }
        return ret;
    }
    return ret;
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
    return ErrorDeviceTypeNotRecognized;
}

ErrorCodes_t MessageDispatcher::initialize(std::string fwPath) {
    this->createDebugFiles();

    ErrorCodes_t ret = this->startCommunication(fwPath);
    if (ret != Success) {
        return ret;
    }

    ret = this->initializeMemory();
    if (ret != Success) {
        return ret;
    }

    this->initializeVariables();

    this->deviceConfiguration();
    if (ret != Success) {
        return ret;
    }

    stopConnectionFlag = false;
    this->createCommunicationThreads();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return this->initializeHW();
}

void MessageDispatcher::deinitialize() {
    stopConnectionFlag = true;
    this->joinCommunicationThreads();

    this->deinitializeVariables();

    this->deinitializeMemory();

    this->stopCommunication();

    this->closeDebugFiles();
}

ErrorCodes_t MessageDispatcher::disconnectDevice(bool overheatFlag) {
    if (std::find(connectedDevices.begin(), connectedDevices.end(), this) == connectedDevices.end()) {
        return Success;
    }
    if (overheatFlag) {
        this->enableOverheatingCounterMeasures();
    }
    this->deinitialize();
    connectedDevices.remove(this);
    return Success;
}

ErrorCodes_t MessageDispatcher::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    frameManager->enableRxMessageType(messageType, flag);

    return Success;
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
//    return ErrorDeviceTypeNotRecognized;
//}

/***************************************\
 *  Channels overview support methods  *
\***************************************/

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

std::string MessageDispatcher::getDeviceSerial() {
    return deviceId;
}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t MessageDispatcher::sendCommands() {
    this->forceOutMessage();
    this->stackOutgoingMessage(txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::startProtocol() {
    if (protocolResetCoder == nullptr) {
        this->forceOutMessage();
        this->stackOutgoingMessage(txStatus, {TxTriggerStartProtocol, ResetIndifferent});
        // this->stackOutgoingMessage(txStatus, {TxTriggerSw, ResetIndifferent});
    }
    else {
        if (protocolResetFlag == false) {
            /*! This is called if the startProtocol is called without protocol structure and items, just to repeat the previous protocol
              Since the protocol structure would otherwise be responsible for calling the resetProtocol, in this case the startProtocol does it instead */
            this->resetProtocol();
        }
        this->stackOutgoingMessage(txStatus); /*! Make sure the registers are submitted */
        protocolResetCoder->encode(0, txStatus);
        this->stackOutgoingMessage(txStatus); /*! Then take the protocol out of the reset state */
        protocolResetFlag = false;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::stopProtocol() {
    bool stopProtocolFlag = false; /*! We're already commiting a stop protocol, so commiting another one on the protocol structure will create an infinite recursion */
    if (selectedClampingModality == ClampingModality_t::VOLTAGE_CLAMP) {
        this->setVoltageProtocolStructure(selectedProtocolId-1, 1, 1, selectedProtocolVrest, stopProtocolFlag);
        this->setVoltageProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "V"}, {0.0, UnitPfxNone, "V"}, {1000.0, UnitPfxNone, "s"}, {0.0, UnitPfxNone, "s"}, false);
    }
    else {
        this->setCurrentProtocolStructure(selectedProtocolId-1, 1, 1, selectedProtocolIrest, stopProtocolFlag);
        this->setCurrentProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "A"}, {0.0, UnitPfxNone, "A"}, {1000.0, UnitPfxNone, "s"}, {0.0, UnitPfxNone, "s"}, false);
    }
    return this->startProtocol();
}

ErrorCodes_t MessageDispatcher::startStateArray() {
    if (numberOfStatesCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    this->stopProtocol();
    this->forceOutMessage();
    this->stackOutgoingMessage(txStatus, {TxTriggerStartStateArray, ResetIndifferent});
    return Success;
}

ErrorCodes_t MessageDispatcher::zap(std::vector <uint16_t> channelIndexes, Measurement_t duration) {
    if (zapCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (duration > zapDurationRange.getMax()) {
        return ErrorValueOutOfRange;
    }
    duration.convertValue(zapDurationRange.prefix);
    zapDurationCoder->encode(duration.value, txStatus);
    for (auto chIdx : channelIndexes) {
        zapCoders[chIdx]->encode(1, txStatus);
    }

    this->stackOutgoingMessage(txStatus, {TxTriggerZap, ResetIndifferent});

    for (auto chIdx : channelIndexes) {
        zapCoders[chIdx]->encode(0, txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::resetAsic(bool resetFlag, bool applyFlag) {
    if (asicResetCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    asicResetCoder->encode(resetFlag, txStatus);

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus, {TxTriggerParameteresUpdated, resetFlag ? ResetTrue : ResetFalse});
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::resetFpga(bool resetFlag, bool applyFlag) {
    if (fpgaResetCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    fpgaResetCoder->encode(resetFlag, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) {
    bool clipFlag = false;
    if (vHoldTunerCoders.empty()) {
        std::vector <Measurement_t> durations(channelIndexes.size(), {0.0, UnitPfxNone, "s"});
        return this->setVoltageRampTuner(channelIndexes, voltages, voltages, durations, applyFlag);
    }
    else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    else if (!allInRange(voltages, vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMin(), vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMax())) {
        clipFlag = true;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        voltages[i].convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
        voltages[i].value = vHoldTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus);
        selectedVoltageHoldVector[channelIndexes[i]] = voltages[i];
        channelModels[channelIndexes[i]]->setVhold(voltages[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (clipFlag) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCurrentHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool applyFlag) {
    bool clipFlag = false;
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    else if (selectedClampingModality == VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    else if (!allInRange(currents, ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMin(), ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMax())) {
        clipFlag = true;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        currents[i].convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
        currents[i].value = cHoldTunerCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(currents[i].value, txStatus);
        selectedCurrentHoldVector[channelIndexes[i]] = currents[i];
        channelModels[channelIndexes[i]]->setChold(currents[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (clipFlag) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageRampTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> initialVoltages, std::vector <Measurement_t> finalVoltages, std::vector <Measurement_t> durations, bool applyFlag) {
    bool clipFlag = false;
    if (vInitRampTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    else if (!allInRange(initialVoltages, vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMin(), vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMax()) ||
             !allInRange(finalVoltages, vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMin(), vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMax())) {
        clipFlag = true;
    }

    double q;
    double r;
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        initialVoltages[i].convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
        initialVoltages[i].value = vInitRampTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(initialVoltages[i].value, txStatus);
        finalVoltages[i].convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
        finalVoltages[i].value = vFinalRampTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(finalVoltages[i].value, txStatus);
        /*! \todo FCON qui bisognerebbe salvare i parametri (almeno la tensione finale) in modo che durante un cambio della VcVoltageRange si possa aggiornare il parametro come avviene per gli hold tuner */
        durations[i].convertValue(positiveProtocolTimeRange.prefix);
        durations[i].value = tRampTunerCoders[channelIndexes[i]]->encode(durations[i].value, txStatus);
        if (durations[i].value == 0.0) {
            q = 0.0;
            initialVoltages[i].value = vInitRampTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(finalVoltages[i].value, txStatus);
        }
        else {
            q = (finalVoltages[i].value-initialVoltages[i].value)/durations[i].value;
            q = quotRampTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(q, txStatus);
        }

        r = (finalVoltages[i].value-initialVoltages[i].value)-durations[i].value*q;
        remRampTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(r, txStatus);
        activateRampTunerCoders[channelIndexes[i]]->encode(true, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus, {TxTriggerSingleChannelRamp, ResetIndifferent});
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        activateRampTunerCoders[channelIndexes[i]]->encode(0, txStatus);
    }
    if (clipFlag) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageHalf(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) {
    bool clipFlag = false;
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    else if (!allInRange(voltages, vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMin(), vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMax())) {
        clipFlag = true;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        voltages[i].convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
        voltages[i].value = vHalfTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus);
        selectedVoltageHalfVector[channelIndexes[i]] = voltages[i];
        channelModels[channelIndexes[i]]->setVhalf(voltages[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (clipFlag) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCurrentHalf(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool applyFlag) {
    bool clipFlag = false;
    if (cHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality == VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    else if (!allInRange(currents, ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMin(), ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMax())) {
        clipFlag = true;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        currents[i].convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
        currents[i].value = cHalfTunerCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(currents[i].value, txStatus);
        selectedCurrentHalfVector[channelIndexes[i]] = currents[i];
        channelModels[channelIndexes[i]]->setChalf(currents[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (clipFlag) {
        return WarningValueClipped;
    }
    return Success;
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
        offsets[offsetIdx++] = originalCalibrationParams.getValue(CalTypesVcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx);
    }
    ljMutexLock.unlock();
    return this->setCalibVcCurrentOffset(channelIndexes, offsets, applyFlag);
}

ErrorCodes_t MessageDispatcher::setLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) {
    bool clipFlag = false;
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    else if (!allInRange(voltages, liquidJunctionRange.getMin(), liquidJunctionRange.getMax())) {
        clipFlag = true;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        selectedLiquidJunctionVector[channelIndexes[i]] = voltages[i];
        this->updateLiquidJunctionVoltage(channelIndexes[i], false);
        channelModels[channelIndexes[i]]->setLiquidJunctionVoltage(selectedLiquidJunctionVector[channelIndexes[i]]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (clipFlag) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::resetLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (liquidJunctionResetCoders.empty()) {
        std::vector <Measurement_t> voltages(channelIndexes.size(), {0.0, liquidJunctionRange.prefix, "V"});
        std::unique_lock <std::mutex> ljMutexLock(ljMutex);
        for (auto channelIdx : channelIndexes) {
            liquidJunctionStatuses[channelIdx] = LiquidJunctionResetted;
        }
        ljMutexLock.unlock();
        return this->setLiquidJunctionVoltage(channelIndexes, voltages, applyFlag);
    }
    else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        liquidJunctionCompensationCoders[channelIndexes[i]]->encode(false, txStatus);
        liquidJunctionResetCoders[channelIndexes[i]]->encode(true, txStatus);
    }

    this->stackOutgoingMessage(txStatus);

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        liquidJunctionResetCoders[channelIndexes[i]]->encode(false, txStatus);
    }

    this->stackOutgoingMessage(txStatus);

    return Success;
}

ErrorCodes_t MessageDispatcher::setGateVoltages(std::vector <uint16_t> boardIndexes, std::vector <Measurement_t> gateVoltages, bool applyFlag) {
    if (gateVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < boardIndexes.size(); i++) {
        gateVoltages[i].convertValue(gateVoltageRange.prefix);
        gateVoltages[i].value = gateVoltageCoders[boardIndexes[i]]->encode(gateVoltages[i].value, txStatus);
        selectedGateVoltageVector[boardIndexes[i]] = gateVoltages[i];
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!allInRange(gateVoltages, gateVoltageRange.getMin(), gateVoltageRange.getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setSourceVoltages(std::vector <uint16_t> boardIndexes, std::vector <Measurement_t> sourceVoltages, bool applyFlag) {
    if (sourceVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < boardIndexes.size(); i++) {
        sourceVoltages[i].convertValue(sourceVoltageRange.prefix);
        sourceVoltages[i].value = sourceVoltageCoders[boardIndexes[i]]->encode(sourceVoltages[i].value, txStatus);
        selectedSourceVoltageVector[boardIndexes[i]] = sourceVoltages[i];
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!allInRange(sourceVoltages, sourceVoltageRange.getMin(), sourceVoltageRange.getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibParams(CalibrationParams_t calibParams) {
    for (int type = 0; type < CalTypesNum; type++) {
        if (!calibParams.types[type].modes.empty() && calibrationParams.types[type].modes.empty()) {
            calibrationParams.types[type] = calibParams.types[type];
        }
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibVcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibVcCurrentGainRange.prefix);
        calibrationParams.setValue(CalTypesVcGainAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], gains[i]);
    }
    this->updateCalibVcCurrentGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibVcCurrentGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcGainAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], calibVcCurrentGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesVcGainAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibVcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibVcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        calibrationParams.setValue(CalTypesVcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], offsets[i]);
    }
    this->updateCalibVcCurrentOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        double offset = calibrationParams.getValue(CalTypesVcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibVcCurrentOffsetCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibCcVoltageGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibCcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibCcVoltageGainRange.prefix);
        calibrationParams.setValue(CalTypesCcGainAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], gains[i]);
    }
    this->updateCalibCcVoltageGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibCcVoltageGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcGainAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], calibCcVoltageGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesCcGainAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibCcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibCcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx[channelIndexes[i]]].prefix);
        calibrationParams.setValue(CalTypesCcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], offsets[i]);
    }
    this->updateCalibCcVoltageOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx[channelIndexes[i]]].prefix);
        double offset = calibrationParams.getValue(CalTypesCcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibCcVoltageOffsetCoders[selectedCcVoltageRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibVcVoltageGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibVcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibVcVoltageGainRange.prefix);
        calibrationParams.setValue(CalTypesVcGainDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], gains[i]);
    }
    this->updateCalibVcVoltageGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibVcVoltageGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcGainDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], calibVcVoltageGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesVcGainDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i]).value;
        calibVcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibVcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibVcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        calibrationParams.setValue(CalTypesVcOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], offsets[i]);
    }
    this->updateCalibVcVoltageOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibVcVoltageOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        double offset = calibrationParams.getValue(CalTypesVcOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i]).value;
        calibVcVoltageOffsetCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibCcCurrentGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibCcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibCcCurrentGainRange.prefix);
        calibrationParams.setValue(CalTypesCcGainDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], gains[i]);
    }
    this->updateCalibCcCurrentGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibCcCurrentGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcGainDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], calibCcCurrentGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesCcGainDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i]).value;
        calibCcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibCcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibCcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
        calibrationParams.setValue(CalTypesCcOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], offsets[i]);
    }
    this->updateCalibCcCurrentOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibCcCurrentOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
        double offset = calibrationParams.getValue(CalTypesCcOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i]).value;
        calibCcCurrentOffsetCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibRsCorrOffsetDac(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibRsCorrOffsetDacCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        calibrationParams.setValue(CalTypesRsCorrOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], offsets[i]);
    }
    this->updateCalibRsCorrOffsetDac(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibRsCorrOffsetDac(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibRsCorrOffsetDacCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesRsCorrOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        double offset = calibrationParams.getValue(CalTypesRsCorrOffsetDac, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibRsCorrOffsetDacCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCalibRShuntConductance(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> conductances, bool applyFlag) {
    if (calibRShuntConductanceCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        conductances[i].convertValue(rRShuntConductanceCalibRange[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        calibrationParams.setValue(CalTypesRShuntConductance, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], conductances[i]);
    }
    this->updateCalibRShuntConductance(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t MessageDispatcher::updateCalibRShuntConductance(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibRShuntConductanceCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesRShuntConductance, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], rRShuntConductanceCalibRange[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        double conductance = calibrationParams.getValue(CalTypesRShuntConductance, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibRShuntConductanceCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(conductance, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    std::vector <uint16_t> currentRangeIdxs;
    currentRangeIdxs.resize(1);
    currentRangeIdxs[0] = currentRangeIdx;
    return setVCCurrentRange(allChannelIndexes, currentRangeIdxs, applyFlag);
}

ErrorCodes_t MessageDispatcher::setVCCurrentRange(std::vector <uint16_t> channelIndexes, std::vector <uint16_t> currentRangeIdx, bool applyFlag) {
    if (vcCurrentRangeCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (!allLessThan(currentRangeIdx, (uint16_t)vcCurrentRangesNum)) {
        return ErrorValueOutOfRange;
    }

    if (independentVcCurrentRanges) {
        for (int idx = 0; idx < channelIndexes.size(); idx++) {
            auto chIdx = channelIndexes[idx];
            vcCurrentRangeCoders[chIdx]->encode(currentRangeIdx[idx], txStatus);
            selectedVcCurrentRangeIdx[chIdx] = currentRangeIdx[idx];
            currentRanges[chIdx] = vcCurrentRangesArray[selectedVcCurrentRangeIdx[chIdx]];
            currentResolutions[chIdx] = currentRanges[chIdx].step;
        }
    }
    else {
        channelIndexes = allChannelIndexes;
        vcCurrentRangeCoders[0]->encode(currentRangeIdx[0], txStatus);
        for (int idx = 0; idx < channelIndexes.size(); idx++) {
            auto chIdx = channelIndexes[idx];
            selectedVcCurrentRangeIdx[chIdx] = currentRangeIdx[0];
            currentRanges[chIdx] = vcCurrentRangesArray[selectedVcCurrentRangeIdx[chIdx]];
            currentResolutions[chIdx] = currentRanges[chIdx].step;
        }
    }

    this->updateCalibVcCurrentGain(channelIndexes, false);
    this->updateCalibVcCurrentOffset(channelIndexes, false);
    this->updateCalibRShuntConductance(channelIndexes, false);
    for (auto chIdx : channelIndexes) {
        this->updateLiquidJunctionVoltage(chIdx, false);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) {
    if (vcVoltageRangeCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (voltageRangeIdx >= vcVoltageRangesNum) {
        return ErrorValueOutOfRange;
    }
    for (auto coder : vcVoltageRangeCoders) {
        coder->encode(voltageRangeIdx, txStatus);
    }
    selectedVcVoltageRangeIdx = voltageRangeIdx;
    for (int chIdx = 0; chIdx < voltageChannelsNum; chIdx++) {
        voltageRanges[chIdx] = vcVoltageRangesArray[selectedVcVoltageRangeIdx];
        voltageResolutions[chIdx] = voltageRanges[chIdx].step;
    }

    this->updateCalibVcVoltageGain(allChannelIndexes, false);
    this->updateCalibVcVoltageOffset(allChannelIndexes, false);
    this->updateVoltageHoldTuner(false);

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    /*! Most of the times the liquid junction (aka digital offset compensation) will be performed by the same DAC that applies the voltage sitmulus
            Voltage clamp, so by default the same range is selected for the liquid junction
            When this is not the case the boolean variable below is set properly by the corresponding derived class of the messagedispatcher */
    if (liquidJunctionSameRangeAsVcDac) {
        this->setLiquidJunctionRange(voltageRangeIdx);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    if (ccCurrentRangeCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (currentRangeIdx >= ccCurrentRangesNum) {
        return ErrorValueOutOfRange;
    }
    for (auto coder : ccCurrentRangeCoders) {
        coder->encode(currentRangeIdx, txStatus);
    }
    selectedCcCurrentRangeIdx = currentRangeIdx;
    for (int chIdx = 0; chIdx < currentChannelsNum; chIdx++) {
        currentRanges[chIdx] = ccCurrentRangesArray[selectedCcCurrentRangeIdx];
        currentResolutions[chIdx] = currentRanges[chIdx].step;
    }

    this->updateCalibCcCurrentGain(allChannelIndexes, false);
    this->updateCalibCcCurrentOffset(allChannelIndexes, false);
    this->resetCalibRShuntConductance(allChannelIndexes, false);
    this->updateCurrentHoldTuner(false);

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) {
    std::vector <uint16_t> voltageRangeIdxs;
    voltageRangeIdxs.resize(currentChannelsNum);
    std::fill(voltageRangeIdxs.begin(), voltageRangeIdxs.end(), voltageRangeIdx);
    return setCCVoltageRange(allChannelIndexes, voltageRangeIdxs, applyFlag);
}

ErrorCodes_t MessageDispatcher::setCCVoltageRange(std::vector <uint16_t> channelIndexes, std::vector <uint16_t> voltageRangeIdx, bool applyFlag) {
    if (ccVoltageRangeCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (!allLessThan(voltageRangeIdx, (uint16_t)ccVoltageRangesNum)) {
        return ErrorValueOutOfRange;
    }
    for (int idx = 0; idx < channelIndexes.size(); idx++) {
        auto chIdx = channelIndexes[idx];
        if (independentVcCurrentRanges) {
            ccVoltageRangeCoders[chIdx]->encode(voltageRangeIdx[idx], txStatus);
        }
        else {
            ccVoltageRangeCoders[0]->encode(voltageRangeIdx[idx], txStatus);
        }
        selectedCcVoltageRangeIdx[chIdx] = voltageRangeIdx[idx];
        voltageRanges[chIdx] = ccVoltageRangesArray[selectedCcVoltageRangeIdx[chIdx]];
        voltageResolutions[chIdx] = voltageRanges[chIdx].step;
    }

    this->updateCalibCcVoltageGain(channelIndexes, false);
    this->updateCalibCcVoltageOffset(channelIndexes, false);

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setLiquidJunctionRange(uint16_t idx) {
    if (idx >= liquidJunctionRangesNum) {
        return ErrorValueOutOfRange;
    }
    selectedLiquidJunctionRangeIdx = idx;
    liquidJunctionRange = liquidJunctionRangesArray[selectedLiquidJunctionRangeIdx];
    liquidJunctionResolution = liquidJunctionRange.step;

    this->setLiquidJunctionVoltage(allChannelIndexes, selectedLiquidJunctionVector, true);

    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlag) {
    if (vcVoltageFilterCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    if (filterIdx >= vcVoltageFiltersNum) {
        return ErrorValueOutOfRange;
    }
    vcVoltageFilterCoder->encode(filterIdx, txStatus);
    selectedVcVoltageFilterIdx = filterIdx;
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlag) {
    if (ccCurrentFilterCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    if (filterIdx >= ccCurrentFiltersNum) {
        return ErrorValueOutOfRange;
    }
    ccCurrentFilterCoder->encode(filterIdx, txStatus);
    selectedCcCurrentFilterIdx = filterIdx;
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::enableStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (enableStimulusCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        enableStimulusCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
        channelModels[channelIndexes[i]]->setStimActive(onValues[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::turnChannelsOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (turnChannelsOnCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        turnChannelsOnCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
        channelModels[channelIndexes[i]]->setOn(onValues[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::turnCalSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (calSwCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::hasCalSw() {
    if (calSwCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::turnVcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (vcSwCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        vcSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::turnCcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (ccSwCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        ccSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setAdcCore(std::vector <uint16_t> channelIndexes, std::vector <ClampingModality_t> clampingModes, bool applyFlag) {
    if (vcCcSelCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        switch (clampingModes[i]) {
        case VOLTAGE_CLAMP:
            vcCcSelCoders[channelIndexes[i]]->encode(1, txStatus);
            break;

        case ZERO_CURRENT_CLAMP:
            vcCcSelCoders[channelIndexes[i]]->encode(0, txStatus);
            break;

        case CURRENT_CLAMP:
            vcCcSelCoders[channelIndexes[i]]->encode(0, txStatus);
            break;

        case DYNAMIC_CLAMP:
            return ErrorFeatureNotImplemented;

        case UNDEFINED_CLAMP:
            return ErrorFeatureNotImplemented;
        }
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::enableCcStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (ccStimEnCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        ccStimEnCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setClampingModality(uint32_t idx, bool applyFlag, bool stopProtocolFlag) {
    if (idx >= clampingModalitiesNum) {
        return ErrorValueOutOfRange;
    }

    if ((idx == selectedClampingModalityIdx) && clampingModalitySetFlag) {
        return Success;
    }

    if (stopProtocolFlag) {
        this->stopProtocol();
    }

    clampingModalitySetFlag = true;
    selectedClampingModalityIdx = idx;
    previousClampingModality = selectedClampingModality;
    selectedClampingModality = clampingModalitiesArray[selectedClampingModalityIdx];
    if (clampingModeCoder != nullptr) {
        clampingModeCoder->encode(selectedClampingModalityIdx, txStatus);
    }

    std::vector <bool> trues(currentChannelsNum);
    std::vector <bool> falses(currentChannelsNum);

    std::fill(trues.begin(), trues.end(), true);
    std::fill(falses.begin(), falses.end(), false);

    switch (selectedClampingModality) {
    case VOLTAGE_CLAMP:
        rawDataFilterVoltageFlag = false;
        rawDataFilterCurrentFlag = true;

        /*! Restore liquid junction and remove it from the voltage reading */
        for (uint32_t i = 0; i < currentChannelsNum; i++) {
            this->updateLiquidJunctionVoltage(i, false);
            ccLiquidJunctionVectorApplied[i] = 0;
        }

        this->enableCcCompensations(false, false);
        this->turnCurrentReaderOn(true, false);
        this->turnVoltageStimulusOn(true, true);
        /*! Apply on previous command to turn the voltage clamp on first */
        this->turnCurrentStimulusOn(false, false);
        this->turnVoltageReaderOn(false, false);
        if (previousClampingModality == VOLTAGE_CLAMP) {
            this->setVCCurrentRange(allChannelIndexes, selectedVcCurrentRangeIdx, false);
        }
        else {
            this->setVCCurrentRange(allChannelIndexes, storedVcCurrentRangeIdx, false);
        }
        this->setVCVoltageRange(selectedVcVoltageRangeIdx, false);
        this->enableVcCompensations(true, false);

        this->setSourceForVoltageChannel(0, false);
        this->setSourceForCurrentChannel(0, false);

        break;

    case ZERO_CURRENT_CLAMP:
        rawDataFilterVoltageFlag = true;
        rawDataFilterCurrentFlag = false;

        /*! Remove liquid junction and subtract it from voltage reading */
        for (uint32_t i = 0; i < currentChannelsNum; i++) {
            this->updateLiquidJunctionVoltage(i, false);
            selectedLiquidJunctionVector[i].convertValue(ccVoltageRangesArray[selectedCcVoltageRangeIdx[i]].prefix);
            ccLiquidJunctionVector[i] = (int16_t)(selectedLiquidJunctionVector[i].value/ccVoltageRangesArray[selectedCcVoltageRangeIdx[i]].step);
            if (subtractLiquidJunctionFromCcFlag) {
                ccLiquidJunctionVectorApplied[i] = ccLiquidJunctionVector[i];
            }
        }

        if (previousClampingModality == VOLTAGE_CLAMP) {
            this->enableVcCompensations(false, false);
            storedVcCurrentRangeIdx = selectedVcCurrentRangeIdx;
            RangedMeasurement_t range;
            uint32_t idx;
            this->getMaxVCCurrentRange(range, idx);
            this->setVCCurrentRange(idx, true);
            /*! Apply on previous command to set the lowest input impedance first */
        }
        this->turnVoltageReaderOn(true, false);
        this->turnCurrentStimulusOn(false, true);
        /*! Apply on previous command to turn the current clamp on then */
        this->turnVoltageStimulusOn(false, false);
        this->turnCurrentReaderOn(false, false);
        this->setCCCurrentRange(selectedCcCurrentRangeIdx, false);
        this->setCCVoltageRange(allChannelIndexes, selectedCcVoltageRangeIdx, false);
        this->enableCcCompensations(true, false);

        this->setSourceForVoltageChannel(1, false);
        this->setSourceForCurrentChannel(1, false);

        break;

    case CURRENT_CLAMP:
        rawDataFilterVoltageFlag = true;
        rawDataFilterCurrentFlag = false;

        /*! Remove liquid junction and subtract it from voltage reading */
        for (uint32_t i = 0; i < currentChannelsNum; i++) {
            this->updateLiquidJunctionVoltage(i, false);
            selectedLiquidJunctionVector[i].convertValue(ccVoltageRangesArray[selectedCcVoltageRangeIdx[i]].prefix);
            ccLiquidJunctionVector[i] = (int16_t)(selectedLiquidJunctionVector[i].value/ccVoltageRangesArray[selectedCcVoltageRangeIdx[i]].step);
            if (subtractLiquidJunctionFromCcFlag) {
                ccLiquidJunctionVectorApplied[i] = ccLiquidJunctionVector[i];
            }
        }

        if (previousClampingModality == VOLTAGE_CLAMP) {
            this->enableVcCompensations(false, false);
            storedVcCurrentRangeIdx = selectedVcCurrentRangeIdx;
            RangedMeasurement_t range;
            uint32_t idx;
            this->getMaxVCCurrentRange(range, idx);
            this->setVCCurrentRange(idx, true);
            /*! Apply on previous command to set the lowest input impedance first */
        }
        this->turnVoltageReaderOn(true, false);
        this->turnCurrentStimulusOn(true, true);
        /*! Apply on previous command to turn the current clamp on then */
        this->turnVoltageStimulusOn(false, false);
        this->turnCurrentReaderOn(false, false);
        this->setCCCurrentRange(selectedCcCurrentRangeIdx, false);
        this->setCCVoltageRange(allChannelIndexes, selectedCcVoltageRangeIdx, false);
        this->enableCcCompensations(true, false);

        this->setSourceForVoltageChannel(1, false);
        this->setSourceForCurrentChannel(1, false);

        break;

    case DYNAMIC_CLAMP:
        rawDataFilterVoltageFlag = false;
        rawDataFilterCurrentFlag = false;
        break;
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
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

ErrorCodes_t MessageDispatcher::setSourceForVoltageChannel(uint16_t source, bool applyFlag) {
    if (sourceForVoltageChannelCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    sourceForVoltageChannelCoder->encode(source, txStatus);
    selectedSourceForVoltageChannelIdx = source;
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setSourceForCurrentChannel(uint16_t source, bool applyFlag) {
    if (sourceForCurrentChannelCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    sourceForCurrentChannelCoder->encode(source, txStatus);
    selectedSourceForCurrentChannelIdx = source;
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::readoutOffsetRecalibration(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (enableStimulusCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    std::unique_lock <std::mutex> ljMutexLock (ljMutex);
    if (anyLiquidJunctionActive) {
        return ErrorLiquidJunctionAndRecalibration;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        uint16_t chIdx = channelIndexes[i];
        enableStimulusCoders[chIdx]->encode(!onValues[i], txStatus); /*!< Disables protocols and vhold */
        channelModels[chIdx]->setRecalibratingReadoutOffset(onValues[i]);
        if (onValues[i] && (offsetRecalibStates[chIdx] == OffsetRecalibIdle)) {
            offsetRecalibStates[chIdx] = OffsetRecalibStarting;
            offsetRecalibStatuses[chIdx] = OffsetRecalibNotPerformed;
            anyOffsetRecalibrationActive = true;
        }
        else if (!onValues[i] && (offsetRecalibStates[chIdx] != OffsetRecalibIdle)) {
            offsetRecalibStates[chIdx] = OffsetRecalibTerminate;
            anyOffsetRecalibrationActive = true;
        }
    }
    if (anyOffsetRecalibrationActive) {
        liquidJunctionControlPending = true;
        computeCurrentOffsetFlag = true;
    }

    ljMutexLock.unlock();

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::liquidJunctionCompensation(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (liquidJunctionCompensationCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    if (anyOffsetRecalibrationActive) {
        return ErrorLiquidJunctionAndRecalibration;
    }

    /*! Liquid junction in SW */
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        uint16_t chIdx = channelIndexes[i];
        liquidJunctionCompensationCoders[chIdx]->encode(onValues[i], txStatus); /*!< Disables protocols and vhold */
        channelModels[chIdx]->setCompensatingLiquidJunction(onValues[i]);
        if (onValues[i] && (liquidJunctionStates[chIdx] == LiquidJunctionIdle)) {
            ljTargetCurrents[chIdx] = 0.0;
            if (!(currentTrackingCoders.empty())) {
                currentTrackingCoders[selectedVcCurrentRangeIdx[chIdx]][chIdx]->encode(ljTargetCurrents[chIdx], txStatus);
                liquidJunctionAutoStopCoders[chIdx]->encode(true, txStatus);
            }
            liquidJunctionStates[chIdx] = LiquidJunctionStarting;
            liquidJunctionStatuses[chIdx] = LiquidJunctionNotPerformed;
            anyLiquidJunctionActive = true;
        }
        else if (!onValues[i] && (liquidJunctionStates[chIdx] != LiquidJunctionIdle)) {
            liquidJunctionStates[chIdx] = LiquidJunctionTerminate;
            anyLiquidJunctionActive = true;
        }
    }
    if (anyLiquidJunctionActive) {
        liquidJunctionControlPending = true;
        computeCurrentOffsetFlag = true;
        targetCurrentEnabled = false;
    }

    ljMutexLock.unlock();

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::digitalOffsetCompensation(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    return liquidJunctionCompensation(channelIndexes, onValues, applyFlag);
}

ErrorCodes_t MessageDispatcher::setCurrentTracking(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool enable) {
    if (liquidJunctionCompensationCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    if (anyOffsetRecalibrationActive) {
        return ErrorLiquidJunctionAndRecalibration;
    }

    /*! Current Tracking in SW */
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        uint16_t chIdx = channelIndexes[i];
        liquidJunctionCompensationCoders[chIdx]->encode(enable, txStatus); /*!< Disables protocols and vhold */
        channelModels[chIdx]->setCompensatingLiquidJunction(enable);
        if (enable) {
            currents[i].convertValue(currentRanges[chIdx].prefix);
            if (!(currentTrackingCoders.empty())) {
                ljTargetCurrents[chIdx] = currentTrackingCoders[selectedVcCurrentRangeIdx[chIdx]][chIdx]->encode(currents[i].value, txStatus);
                liquidJunctionAutoStopCoders[chIdx]->encode(false, txStatus);
            }
            else {
                ljTargetCurrents[chIdx] = currents[i].value/currentRanges[chIdx].step;
            }
            liquidJunctionStates[chIdx] = LiquidJunctionStarting;
            liquidJunctionStatuses[chIdx] = LiquidJunctionNotPerformed;
            anyLiquidJunctionActive = true;
        }
        else if (!enable && (liquidJunctionStates[chIdx] != LiquidJunctionIdle)) {
            liquidJunctionStates[chIdx] = LiquidJunctionTerminate;
            anyLiquidJunctionActive = true;
        }
    }
    if (anyLiquidJunctionActive) {
        liquidJunctionControlPending = true;
        computeCurrentOffsetFlag = true;
        targetCurrentEnabled = true;
    }

    ljMutexLock.unlock();
    this->stackOutgoingMessage(txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setAdcFilter(bool applyFlag) {
    switch (selectedClampingModality) {
    case VOLTAGE_CLAMP:
        if (vcCurrentFilterCoder != nullptr) {
            vcCurrentFilterCoder->encode(sr2LpfVcCurrentMap[selectedSamplingRateIdx], txStatus);
            selectedVcCurrentFilterIdx = sr2LpfVcCurrentMap[selectedSamplingRateIdx];
        }
        break;

    case CURRENT_CLAMP:
    case ZERO_CURRENT_CLAMP:
        if (ccVoltageFilterCoder != nullptr) {
            ccVoltageFilterCoder->encode(sr2LpfCcVoltageMap[selectedSamplingRateIdx], txStatus);
            selectedCcVoltageFilterIdx = sr2LpfCcVoltageMap[selectedSamplingRateIdx];
        }
        break;
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) {
    if (samplingRateCoder == nullptr) {
        selectedSamplingRateIdx = 0;
        return ErrorFeatureNotImplemented;
    }

    if (samplingRateIdx >= samplingRatesNum) {
        return ErrorValueOutOfRange;
    }

    if (samplingRateIdx == selectedSamplingRateIdx && (clockDividerCoder == nullptr || selectedClockDividerIdx == sr2srm[samplingRateIdx])) {
        return Success;
    }

    samplingRateCoder->encode(samplingRateIdx, txStatus);
    selectedSamplingRateIdx = samplingRateIdx;
    samplingRate = realSamplingRatesArray[selectedSamplingRateIdx];
    integrationStep = integrationStepArray[selectedSamplingRateIdx];
    this->setAdcFilter();
    this->computeRawDataFilterCoefficients();
    this->computeDataReadPolicy();

    if (this->setClockDiv(sr2srm[selectedSamplingRateIdx], false) != Success) { /*! Update the calibrations only if the clock divider hasn't already */
        switch (selectedClampingModality) {
        case VOLTAGE_CLAMP:
            this->updateCalibVcCurrentGain(allChannelIndexes, false);
            this->updateCalibVcCurrentOffset(allChannelIndexes, false);
            break;

        case CURRENT_CLAMP:
        case ZERO_CURRENT_CLAMP:
            this->updateCalibCcVoltageGain(allChannelIndexes, false);
            this->updateCalibCcVoltageOffset(allChannelIndexes, false);
            break;
        }
    }

    if (stateArrayMovingAverageLengthCoder != nullptr) {
        stateArrayMovingAverageLengthCoder->encode(stateArrayReactionTime.getNoPrefixValue()*samplingRate.getNoPrefixValue(), txStatus);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    this->purgeData();
    return Success;
}

ErrorCodes_t MessageDispatcher::setClockDiv(uint16_t clockDivIdx, bool applyFlag) {
    if (clockDividerCoder == nullptr) {
        clockDivIdx = 0;
        return ErrorFeatureNotImplemented;
    }

    if (clockDivIdx >= clockDividersNum) {
        return ErrorValueOutOfRange;
    }

    if (clockDivIdx == selectedClockDividerIdx) {
        return Success;
    }

    clockDividerCoder->encode(clockDivIdx, txStatus);
    selectedClockDividerIdx = clockDivIdx;
    switch (selectedClampingModality) {
    case VOLTAGE_CLAMP:
        this->updateCalibVcCurrentGain(allChannelIndexes, false);
        this->updateCalibVcCurrentOffset(allChannelIndexes, false);
        break;

    case CURRENT_CLAMP:
    case ZERO_CURRENT_CLAMP:
        this->updateCalibCcVoltageGain(allChannelIndexes, false);
        this->updateCalibCcVoltageOffset(allChannelIndexes, false);
        break;
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setDownsamplingRatio(uint32_t ratio) {
    if (ratio == 0) {
        return ErrorValueOutOfRange;
    }

    if (ratio == 1) {
        downsamplingFlag = false;
    }
    else {
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

ErrorCodes_t MessageDispatcher::setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest, bool stopProtocolFlag) {
    if (voltageProtocolRestCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    else if (itemsNum > protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;
    }
    if (stopProtocolFlag) {
        this->resetProtocol();
    }
    selectedProtocolId = protId;
    selectedProtocolItemsNum = itemsNum;
    selectedProtocolVrest = vRest;
    UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
    protocolIdCoder->encode(protId, txStatus);
    protocolItemsNumberCoder->encode(itemsNum, txStatus);
    protocolSweepsNumberCoder->encode(sweepsNum, txStatus);
    vRest.convertValue(voltagePrefix);
    voltageProtocolRestCoders[selectedVcVoltageRangeIdx]->encode(vRest.value, txStatus);

    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag, std::vector <uint16_t> activeDigitalOutputs) {
    if (!voltageProtocolStepImplemented) {
        return ErrorFeatureNotImplemented;
    }
    else if (itemIdx >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0Step) ||
             !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
        return ErrorValueOutOfRange;
    }
    UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
    UnitPfx_t timePrefix = protocolTimeRange.prefix;
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(vHalfFlag, txStatus);
    }
    if (!protocolItemDigitalOutCoders.empty()) {
        uint32_t digOuts = 0;
        for (auto idx : activeDigitalOutputs) {
            if (idx < digitalOutputsNum) {
                digOuts |= (((uint32_t)1) << idx);
            }
        }
        protocolItemDigitalOutCoders[itemIdx]->encode(digOuts, txStatus);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemStep, txStatus);
    v0.convertValue(voltagePrefix);
    voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus);
    v0Step.convertValue(voltagePrefix);
    voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus);
    voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(0.0, txStatus);
    voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(0.0, txStatus);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag, std::vector <uint16_t> activeDigitalOutputs) {
    if (!voltageProtocolRampImplemented) {
        return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0Step) ||
               !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vFinal) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vFinalStep) ||
               !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
        return ErrorValueOutOfRange;
    }
    UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
    UnitPfx_t timePrefix = protocolTimeRange.prefix;
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(vHalfFlag, txStatus);
    }
    if (!protocolItemDigitalOutCoders.empty()) {
        uint32_t digOuts = 0;
        for (auto idx : activeDigitalOutputs) {
            if (idx < digitalOutputsNum) {
                digOuts |= (((uint32_t)1) << idx);
            }
        }
        protocolItemDigitalOutCoders[itemIdx]->encode(digOuts, txStatus);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemRamp, txStatus);
    v0.convertValue(voltagePrefix);
    voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus);
    v0Step.convertValue(voltagePrefix);
    voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus);
    vFinal.convertValue(voltagePrefix);
    voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(vFinal.value, txStatus);
    vFinalStep.convertValue(voltagePrefix);
    voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(vFinalStep.value, txStatus);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag, std::vector <uint16_t> activeDigitalOutputs) {
    if (!voltageProtocolSinImplemented) {
        return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0Step) ||
               !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vAmp) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vAmpStep) ||
               !positiveProtocolFrequencyRange.includes(f0) || !protocolFrequencyRange.includes(f0Step)) {
        return ErrorValueOutOfRange;
    }
    UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
    UnitPfx_t freqPrefix = protocolFrequencyRange.prefix;
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(vHalfFlag, txStatus);
    }
    if (!protocolItemDigitalOutCoders.empty()) {
        uint32_t digOuts = 0;
        for (auto idx : activeDigitalOutputs) {
            if (idx < digitalOutputsNum) {
                digOuts |= (((uint32_t)1) << idx);
            }
        }
        protocolItemDigitalOutCoders[itemIdx]->encode(digOuts, txStatus);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemSin, txStatus);
    v0.convertValue(voltagePrefix);
    voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus);
    v0Step.convertValue(voltagePrefix);
    voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus);
    vAmp.convertValue(voltagePrefix);
    voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(vAmp.value, txStatus);
    vAmpStep.convertValue(voltagePrefix);
    voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(vAmpStep.value, txStatus);
    f0.convertValue(freqPrefix);
    protocolFrequency0Coders[itemIdx]->encode(f0.value, txStatus);
    f0Step.convertValue(freqPrefix);
    protocolFrequency0StepCoders[itemIdx]->encode(f0Step.value, txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest, bool stopProtocolFlag) {
    if (currentProtocolRestCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (itemsNum > protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;
    }
    if (stopProtocolFlag) {
        this->resetProtocol();
    }
    selectedProtocolId = protId;
    selectedProtocolItemsNum = itemsNum;
    selectedProtocolIrest = iRest;
    UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
    protocolIdCoder->encode(protId, txStatus);
    protocolItemsNumberCoder->encode(itemsNum, txStatus);
    protocolSweepsNumberCoder->encode(sweepsNum, txStatus);
    iRest.convertValue(currentPrefix);
    currentProtocolRestCoders[selectedCcCurrentRangeIdx]->encode(iRest.value, txStatus);

    return Success;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag, std::vector <uint16_t> activeDigitalOutputs) {
    if (!currentProtocolStepImplemented) {
        return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0Step) ||
               !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
        return ErrorValueOutOfRange;
    }
    UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
    UnitPfx_t timePrefix = protocolTimeRange.prefix;
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(cHalfFlag, txStatus);
    }
    if (!protocolItemDigitalOutCoders.empty()) {
        uint32_t digOuts = 0;
        for (auto idx : activeDigitalOutputs) {
            if (idx < digitalOutputsNum) {
                digOuts |= (((uint32_t)1) << idx);
            }
        }
        protocolItemDigitalOutCoders[itemIdx]->encode(digOuts, txStatus);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemStep, txStatus);
    i0.convertValue(currentPrefix);
    currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus);
    i0Step.convertValue(currentPrefix);
    currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus);
    currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(0.0, txStatus);
    currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(0.0, txStatus);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag, std::vector <uint16_t> activeDigitalOutputs) {
    if (!currentProtocolRampImplemented) {
        return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0Step) ||
               !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iFinal) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iFinalStep) ||
               !positiveProtocolTimeRange.includes(t0) || !protocolTimeRange.includes(t0Step)) {
        return ErrorValueOutOfRange;
    }
    UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
    UnitPfx_t timePrefix = protocolTimeRange.prefix;
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(cHalfFlag, txStatus);
    }
    if (!protocolItemDigitalOutCoders.empty()) {
        uint32_t digOuts = 0;
        for (auto idx : activeDigitalOutputs) {
            if (idx < digitalOutputsNum) {
                digOuts |= (((uint32_t)1) << idx);
            }
        }
        protocolItemDigitalOutCoders[itemIdx]->encode(digOuts, txStatus);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemRamp, txStatus);
    i0.convertValue(currentPrefix);
    currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus);
    i0Step.convertValue(currentPrefix);
    currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus);
    iFinal.convertValue(currentPrefix);
    currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(iFinal.value, txStatus);
    iFinalStep.convertValue(currentPrefix);
    currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(iFinalStep.value, txStatus);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag, std::vector <uint16_t> activeDigitalOutputs) {
    if (!currentProtocolSinImplemented) {
        return ErrorFeatureNotImplemented;
    } else if (itemIdx >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(i0Step) ||
               !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iAmp) || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iAmpStep) ||
               !positiveProtocolFrequencyRange.includes(f0) || !protocolFrequencyRange.includes(f0Step)) {
        return ErrorValueOutOfRange;
    }
    UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
    UnitPfx_t freqPrefix = protocolFrequencyRange.prefix;
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(cHalfFlag, txStatus);
    }
    if (!protocolItemDigitalOutCoders.empty()) {
        uint32_t digOuts = 0;
        for (auto idx : activeDigitalOutputs) {
            if (idx < digitalOutputsNum) {
                digOuts |= (((uint32_t)1) << idx);
            }
        }
        protocolItemDigitalOutCoders[itemIdx]->encode(digOuts, txStatus);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemSin, txStatus);
    i0.convertValue(currentPrefix);
    currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus);
    i0Step.convertValue(currentPrefix);
    currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus);
    iAmp.convertValue(currentPrefix);
    currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(iAmp.value, txStatus);
    iAmpStep.convertValue(currentPrefix);
    currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(iAmpStep.value, txStatus);
    f0.convertValue(freqPrefix);
    protocolFrequency0Coders[itemIdx]->encode(f0.value, txStatus);
    f0Step.convertValue(freqPrefix);
    protocolFrequency0StepCoders[itemIdx]->encode(f0Step.value, txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setStateArrayStructure(int numberOfStates, int initialState, Measurement_t reactionTime) {
    if (numberOfStatesCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    if ((unsigned int)numberOfStates > stateMaxNum || initialState >= numberOfStates) {
        return ErrorValueOutOfRange;
    }
    numberOfStatesCoder->encode(numberOfStates, txStatus);
    initialStateCoder->encode(initialState, txStatus);
    if (stateArrayMovingAverageLengthCoder != nullptr) {
        stateArrayReactionTime = reactionTime;
        stateArrayMovingAverageLengthCoder->encode(reactionTime.getNoPrefixValue()*samplingRate.getNoPrefixValue(), txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setStateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, Measurement_t timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag) {
    if (stateAppliedVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltage.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
    stateAppliedVoltageCoders[selectedVcVoltageRangeIdx][stateIdx]->encode(voltage.value, txStatus);
    stateTimeoutFlagCoders[stateIdx]->encode(timeoutStateFlag, txStatus);
    stateTriggerFlagCoders[stateIdx]->encode(triggerFlag, txStatus);
    stateTriggerDeltaFlagCoders[stateIdx]->encode(deltaFlag, txStatus);
    stateTimeoutValueCoders[stateIdx]->encode(timeout.getNoPrefixValue(), txStatus);
    stateTimeoutNextStateCoders[stateIdx]->encode(timeoutState, txStatus);
    /*! \todo FCON gli state array non sono implementati per gestire range diversi a livello FW */
    minTriggerValue.convertValue(vcCurrentRangesArray[selectedVcCurrentRangeIdx[0]].prefix);
    maxTriggerValue.convertValue(vcCurrentRangesArray[selectedVcCurrentRangeIdx[0]].prefix);
    stateMinTriggerCurrentCoders[selectedVcCurrentRangeIdx[0]][stateIdx]->encode(minTriggerValue.value, txStatus);
    stateMaxTriggerCurrentCoders[selectedVcCurrentRangeIdx[0]][stateIdx]->encode(maxTriggerValue.value, txStatus);
    stateTriggerNextStateCoders[stateIdx]->encode(triggerState, txStatus);
    return Success;
}

ErrorCodes_t MessageDispatcher::setStateArrayEnabled(int chIdx, bool enabledFlag) {
    if (enableStateArrayChannelsCoder.empty()) {
        return ErrorFeatureNotImplemented;
    }
    enableStateArrayChannelsCoder[chIdx]->encode(enabledFlag, txStatus);
    return Success;
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

ErrorCodes_t MessageDispatcher::getNextMessage(RxOutput_t &rxOutput, int16_t * data, MsgTypeId_t type) {
    if (parsingStatus != ParsingParsing) {
        return ErrorDeviceNotConnected;
    }

    ErrorCodes_t ret = ErrorNoDataAvailable;
    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged or if an error is returned before this can be set to its proper value */
    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;

    uint32_t samplesNum = 0;
    uint32_t sampleIdx = 0; /*! Data index in the read message */
    uint32_t outSampleIdx = 0; /*! Data index in the written buffer */
    uint32_t timeSamplesNum;
    int16_t rawFloat;
    bool keepReading = true;

    while (keepReading) {
        sampleIdx = 0;
        auto msg = frameManager->getNextMessage(type);
        msg.typeId |= MsgDirectionDeviceToPc;
        if (msg.typeId == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
            return ret;
        }

        rxOutput.msgTypeId = msg.typeId;
        switch (msg.typeId) {
        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
            rxOutput.dataLen = 0;
            rxOutput.protocolId = msg.data[sampleIdx++];
            rxOutput.protocolItemIdx = msg.data[sampleIdx++];
            rxOutput.protocolRepsIdx = msg.data[sampleIdx++];
            rxOutput.protocolSweepIdx = msg.data[sampleIdx++];
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData): {
            samplesNum = msg.data.size();

            std::unique_lock <std::mutex> ljMutexLock(ljMutex);
            if (downsamplingFlag) {
                timeSamplesNum = samplesNum/totalChannelsNum;
                for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                    if (++downsamplingOffset >= selectedDownsamplingRatio) {
                        downsamplingOffset = 0;
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            rawFloat = ((int16_t)msg.data[sampleIdx++])-ccLiquidJunctionVectorApplied[voltageChannelIdx];
#ifdef FILTER_CLIP_NEEDED
                            xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                            data[outSampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                            data[outSampleIdx++] = (int16_t)round(this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen));
#endif
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            rawFloat = (int16_t)msg.data[sampleIdx++];
#ifdef FILTER_CLIP_NEEDED
                            xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                            data[outSampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                            data[outSampleIdx++] = (int16_t)round(this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen));
#endif
                        }
                    }
                    else {
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            rawFloat = ((int16_t)msg.data[sampleIdx++])-ccLiquidJunctionVectorApplied[voltageChannelIdx];
                            this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            rawFloat = (int16_t)msg.data[sampleIdx++];
                            this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                        }
                    }

                    if (iirOff < 1) {
                        iirOff = IIR_ORD;

                    } else {
                        iirOff--;
                    }
                }

                rxOutput.dataLen = outSampleIdx;
            }
            else if (rawDataFilterActiveFlag) {
                timeSamplesNum = samplesNum/totalChannelsNum;
                for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                    for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                        rawFloat = ((int16_t)msg.data[sampleIdx++])-ccLiquidJunctionVectorApplied[voltageChannelIdx];
#ifdef FILTER_CLIP_NEEDED
                        xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                        data[outSampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                        data[outSampleIdx++] = (int16_t)round(this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen));
#endif
                    }

                    for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                        rawFloat = (int16_t)msg.data[sampleIdx++];
#ifdef FILTER_CLIP_NEEDED
                        xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                        data[outSampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                        data[outSampleIdx++] = (int16_t)round(this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen));
#endif
                    }

                    if (iirOff < 1) {
                        iirOff = IIR_ORD;

                    } else {
                        iirOff--;
                    }
                }
                rxOutput.dataLen = outSampleIdx;
            }
            else {
                timeSamplesNum = samplesNum/totalChannelsNum;
                for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                    for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                        data[outSampleIdx++] = ((int16_t)msg.data[sampleIdx++])-ccLiquidJunctionVectorApplied[voltageChannelIdx];
                    }

                    for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                        data[outSampleIdx++] = msg.data[sampleIdx++];
                    }
                }
                rxOutput.dataLen = outSampleIdx;
            }
            break;
        }
        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
            rxOutput.dataLen = 0;
            rxOutput.protocolId = msg.data[sampleIdx];
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
            rxOutput.dataLen = 0;
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss):
            rxOutput.dataLen = 2;
            data[0] = (int16_t)msg.data[sampleIdx++];
            data[1] = (int16_t)msg.data[sampleIdx++];
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSyncStatus):
            rxOutput.dataLen = 2;
            data[1] = (int16_t)msg.data[sampleIdx++];
            data[0] = (int16_t)msg.data[sampleIdx++];
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdSpiDataLoad):
            rxOutput.dataLen = 2;
            data[0] = (int16_t)msg.data[sampleIdx++];
            data[1] = (int16_t)msg.data[sampleIdx++];
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdCalEeprom):
            rxOutput.dataLen = msg.data.size();
            for (int idx = 0; idx < msg.data.size(); idx++) {
                data[idx] = (int16_t)msg.data[sampleIdx++];
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus):
            // not really managed, ignore it
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdDebugData):
            rxOutput.dataLen = msg.data.size();
            for (int idx = 0; idx < msg.data.size(); idx++) {
                data[idx] = (int16_t)msg.data[sampleIdx++];
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdTemperature):
            /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
            rxOutput.dataLen = temperatureChannelsNum;
            /*! \todo FCON check sulla lunghezza del messaggio */
            for (uint16_t temperatureChannelIdx = 0; temperatureChannelIdx < temperatureChannelsNum; temperatureChannelIdx++) {
                data[temperatureChannelIdx] = (int16_t)msg.data[sampleIdx++];
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdOnTime):
            /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
            rxOutput.dataLen = 2;
            data[0] = (int16_t)msg.data[sampleIdx++];
            data[1] = (int16_t)msg.data[sampleIdx++];
            break;
        }
        keepReading = msg.mergeable && (rxOutput.dataLen+frameManager->getMaxDataMessageSize() < E384CL_OUT_STRUCT_DATA_LEN);
        ret = Success;
    }

#ifdef SPT_LOG_GET_NEXT_MESSAGE
    speedTestLog(SpeedTestGetNextMessage, rxOutput.dataLen*2);
#endif
    return ret;
}

ErrorCodes_t MessageDispatcher::getStoredMessage(RxOutput_t &rxOutput, int16_t * data, MsgTypeId_t type) {
    if (parsingStatus != ParsingParsing) {
        return ErrorDeviceNotConnected;
    }

    ErrorCodes_t ret = ErrorNoDataAvailable;
    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged or if an error is returned before this can be set to its proper value */
    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;

    uint32_t sampleIdx = 0; /*! Data index in the read message */

    sampleIdx = 0;
    auto msg = frameManager->getStoredMessage(type);
    msg.typeId |= MsgDirectionDeviceToPc;
    if (msg.typeId == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
        return ret;
    }

    rxOutput.msgTypeId = msg.typeId;
    switch (msg.typeId) {
    case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
        rxOutput.dataLen = 0;
        rxOutput.protocolId = msg.data[sampleIdx];
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss):
        rxOutput.dataLen = 2;
        data[0] = (int16_t)msg.data[sampleIdx++];
        data[1] = (int16_t)msg.data[sampleIdx++];
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSyncStatus):
        rxOutput.dataLen = 2;
        data[1] = (int16_t)msg.data[sampleIdx++];
        data[0] = (int16_t)msg.data[sampleIdx++];
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdSpiDataLoad):
        rxOutput.dataLen = 2;
        data[0] = (int16_t)msg.data[sampleIdx++];
        data[1] = (int16_t)msg.data[sampleIdx++];
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdCalEeprom):
        rxOutput.dataLen = msg.data.size();
        for (int idx = 0; idx < msg.data.size(); idx++) {
            data[idx] = (int16_t)msg.data[sampleIdx++];
        }
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus):
        // not really managed, ignore it
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdDebugData):
        rxOutput.dataLen = msg.data.size();
        for (int idx = 0; idx < msg.data.size(); idx++) {
            data[idx] = (int16_t)msg.data[sampleIdx++];
        }
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdTemperature):
        /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
        rxOutput.dataLen = temperatureChannelsNum;
        /*! \todo FCON check sulla lunghezza del messaggio */
        for (uint16_t temperatureChannelIdx = 0; temperatureChannelIdx < temperatureChannelsNum; temperatureChannelIdx++) {
            data[temperatureChannelIdx] = (int16_t)msg.data[sampleIdx++];
        }
        break;

    case (MsgDirectionDeviceToPc+MsgTypeIdOnTime):
        /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
        rxOutput.dataLen = 2;
        data[0] = (int16_t)msg.data[sampleIdx++];
        data[1] = (int16_t)msg.data[sampleIdx++];
        break;

    default:
        return ret;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::purgeData() {
    frameManager->purgeData();

    return Success;
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

ErrorCodes_t MessageDispatcher::convertOnTimeValue(int16_t * intValues, double * fltValue) {
    fltValue[0] = onTimeRange.step*(((double)((uint16_t)intValues[0]))*USHORT_MAX+(double)((uint16_t)intValues[1]));

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

ErrorCodes_t MessageDispatcher::getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures) {
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageHoldTunerFeatures = vcVoltageRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageHoldTuner(std::vector <Measurement_t> &voltages) {
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltages = selectedVoltageHoldVector;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTunerFeatures) {
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageHalfTunerFeatures = vcVoltageRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures) {
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentHoldTunerFeatures = ccCurrentRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTunerFeatures) {
    if (cHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentHalfTunerFeatures = ccCurrentRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageRampTunerFeatures(std::vector <RangedMeasurement_t> &voltageRanges, RangedMeasurement_t &durationRange) {
    if (vInitRampTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageRanges = vcVoltageRangesArray;
    durationRange = positiveProtocolTimeRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges) {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    ranges = liquidJunctionRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasGateVoltages() {
    if (gateVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::hasSourceVoltages() {
    if (sourceVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures) {
    if (gateVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    gateVoltagesFeatures = gateVoltageRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures) {
    if (sourceVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    sourceVoltagesFeatures = sourceVoltageRange;
    return Success;
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
    defaultRangeIdx = defaultVcCurrentRangeIdxs[0];
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges, std::vector <uint16_t> &defaultRangeIdxs) {
    if (vcCurrentRangesArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentRanges = vcCurrentRangesArray;
    defaultRangeIdxs = defaultVcCurrentRangeIdxs;
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

ErrorCodes_t MessageDispatcher::getOnTimeFeatures(RangedMeasurement_t &range) {
    if (!onTimeHwFlag) {
        return ErrorFeatureNotImplemented;
    }
    range = onTimeRange;
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

ErrorCodes_t MessageDispatcher::getClockDividers(std::vector <uint32_t> &clockDividers) {
    if (clockDividersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    clockDividers = clockDividersArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getClockDividers(uint32_t &clockDivider) {
    if (clockDividersArray.empty()) {
        return ErrorFeatureNotImplemented;
    }
    clockDivider = clockDividersArray[selectedClockDividerIdx];
    return Success;
}

ErrorCodes_t MessageDispatcher::getClockDividerIdx(uint32_t &idx) {
    if (clockDividersArray.empty()) {
        idx = -1;
        return ErrorFeatureNotImplemented;
    }
    idx = selectedClockDividerIdx;
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
    if (turnChannelsOnCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getStimulusBlockSize(uint32_t &blockSize) {
    blockSize = stimulusBlockSize;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasStimulusSwitches() {
    if (enableStimulusCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::hasOffsetCompensation() {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::hasStimulusHalf() {
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibParams(CalibrationParams_t &calibParams) {
    for (int type = 0; type < CalTypesNum; type++) {
        if (!calibrationParams.types[type].modes.empty()) {
            calibParams.types[type] = calibrationParams.types[type];
        }
    }
    calibParams.sr2srm = calibrationParams.sr2srm;
    return calibrationLoadingError;
}

ErrorCodes_t MessageDispatcher::getCalibFileNames(std::vector <std::string> &calibFileNames) {
    calibFileNames = calibrationFileNames;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibFilesFlags(std::vector <std::vector <bool> > &calibFilesFlags) {
    calibFilesFlags = calibrationFilesOkFlags;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibMappingFileDir(std::string &dir) {
    dir = calibrationMappingFileDir;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibMappingFilePath(std::string &path) {
    path = calibrationMappingFilePath;
    return Success;
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

ErrorCodes_t MessageDispatcher::okMoveCalibrationEepromToRams() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::okMoveCalibrationRamsToEeprom() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::okSelectCalibrationRam(uint16_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::okWriteCalibrationRam(uint16_t, uint8_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::okReadCalibrationRam() {
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
    if (numberOfStatesCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::getZapFeatures(RangedMeasurement_t &durationRange) {
    if (zapCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    durationRange = zapDurationRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCalibrationStatus() {
    if (!calibrationFileRequired) {
        return Success;
    }
    return calibrationLoadingError;
}

/*********************\
 *  Private methods  *
\*********************/

void MessageDispatcher::createDebugFiles() {
    this->createDebugFiles(DebugLevelTx);
    this->createDebugFiles(DebugLevelRxRaw);
    this->createDebugFiles(DebugLevelRx);
    this->createDebugFiles(DebugLevelDigitallOffsetCompensation);
    this->createDebugFiles(DebugLevelTemperature);

    if (debugLevelEnabled(DebugLevelTx) || debugLevelEnabled(DebugLevelRx)) {
        startPrintfTime = std::chrono::steady_clock::now();
    }

    if (debugLevelEnabled(DebugLevelTemperature)) {
        if (tempFid != nullptr) {
            std::fprintf(tempFid, "TSet TInt TExt Speed\n");
            std::fflush(tempFid);
        }
    }
}

void MessageDispatcher::createDebugFiles(int debugLevel) {
    static std::string suffix = "_0";
    std::string filename = "";

    FILE ** fid;
    fid = nullptr;
    switch (debugLevel) {
    case DebugLevelTx:
        fid = &txFid;
        filename = "e384CommLib_tx";
        break;

    case DebugLevelRxRaw:
        fid = &rxRawFid;
        if (suffix == "_0") {
            suffix = "_1";
        }
        else {
            suffix = "_0";
        }
        filename = "e384CommLib_rxRaw" + suffix;
        break;

    case DebugLevelRx:
        fid = &rxFid;
        filename = "e384CommLib_rx";
        break;

    case DebugLevelDigitallOffsetCompensation:
        fid = &ljFid;
        filename = "e384CommLib_lj";
        break;

    case DebugLevelTemperature:
        fid = &tempFid;
        filename = "e384CommLib_temp";
        break;
    }

    if (fid != nullptr && debugLevelEnabled((DebugLevels_t)debugLevel)) {
        if (* fid == nullptr) {
            * fid = createDebugFile(filename);
        }
    }
}

ErrorCodes_t MessageDispatcher::initializeMemory() {
    txMsgBuffer = new (std::nothrow) std::vector <uint16_t>[TX_MSG_BUFFER_SIZE];
    if (txMsgBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txMsgToBeSentWords.resize(TX_MSG_BUFFER_SIZE);
    txMsgOption.resize(TX_MSG_BUFFER_SIZE);

    if (independentVcCurrentRanges) {
        selectedVcCurrentRangeIdx = defaultVcCurrentRangeIdxs;
    }
    else {
        selectedVcCurrentRangeIdx.resize(currentChannelsNum);
        std::fill(selectedVcCurrentRangeIdx.begin(), selectedVcCurrentRangeIdx.end(), defaultVcCurrentRangeIdxs[0]);
    }

    selectedCcVoltageRangeIdx.resize(voltageChannelsNum);
    std::fill(selectedCcVoltageRangeIdx.begin(), selectedCcVoltageRangeIdx.end(), defaultCcVoltageRangeIdx);

    selectedVoltageHoldVector.resize(currentChannelsNum);
    std::fill(selectedVoltageHoldVector.begin(), selectedVoltageHoldVector.end(), defaultVoltageHoldTuner);

    selectedCurrentHoldVector.resize(currentChannelsNum);
    std::fill(selectedCurrentHoldVector.begin(), selectedCurrentHoldVector.end(), defaultCurrentHoldTuner);

    selectedVoltageHalfVector.resize(currentChannelsNum);
    std::fill(selectedVoltageHalfVector.begin(), selectedVoltageHalfVector.end(), defaultVoltageHalfTuner);

    selectedCurrentHalfVector.resize(currentChannelsNum);
    std::fill(selectedCurrentHalfVector.begin(), selectedCurrentHalfVector.end(), defaultCurrentHalfTuner);

    return Success;
}

void MessageDispatcher::initializeVariables() {
    channelsPerBoard = currentChannelsNum/totalBoardsNum;
    this->fillChannelList(totalBoardsNum, currentChannelsNum/totalBoardsNum);

    /*! Allocate memory for raw data filters */
    this->initializeRawDataFilterVariables();

    this->initializeLiquidJunction();

    this->initializeCalibration();

    frameManager = new FrameManager(this);
    frameManager->setMaxDataMessageSize(totalChannelsNum*packetsPerFrame);
    frameManager->setRxWordParams(rxWordOffsets);
    frameManager->setCurrentBlockLength(rxCurrentBlockLength);
}

ErrorCodes_t MessageDispatcher::deviceConfiguration() {
    std::this_thread::sleep_for (std::chrono::milliseconds(10));

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
        this->setVCCurrentRange(allChannelIndexes, defaultVcCurrentRangeIdxs, false);
        this->setVCVoltageRange(defaultVcVoltageRangeIdx, false);
        this->setVoltageStimulusLpf(defaultVcVoltageFilterIdx, false);
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

void MessageDispatcher::createCommunicationThreads() {
    if (!threadsStarted) {
        deviceCommunicationThread = std::thread(&MessageDispatcher::handleCommunicationWithDevice, this);
        rxConsumerThread = std::thread(&MessageDispatcher::parseDataFromDevice, this);
        liquidJunctionThread = std::thread(&MessageDispatcher::computeLiquidJunction, this);

        threadsStarted = true;
    }
}

void MessageDispatcher::enableOverheatingCounterMeasures() {
    if (overHeatingModeCoder == nullptr) {
        return;
    }
    overHeatingModeCoder->encode(1, txStatus);
}

void MessageDispatcher::deinitializeMemory() {
    if (txMsgBuffer != nullptr) {
        delete [] txMsgBuffer;
        txMsgBuffer = nullptr;
    }
}

void MessageDispatcher::deinitializeVariables() {
    this->deinitializeCalibration();
    this->deInitializeRawDataFilterVariables();
    this->flushBoardList();
    if (frameManager != nullptr) {
        delete frameManager;
        frameManager = nullptr;
    }
}

void MessageDispatcher::closeDebugFiles() {
    this->closeDebugFiles(DebugLevelTx);
    this->closeDebugFiles(DebugLevelRxRaw);
    this->closeDebugFiles(DebugLevelRx);
    this->closeDebugFiles(DebugLevelDigitallOffsetCompensation);
    this->closeDebugFiles(DebugLevelTemperature);
}

void MessageDispatcher::closeDebugFiles(int debugLevel) {
    FILE ** fid;
    fid = nullptr;
    switch (debugLevel) {
    case DebugLevelTx:
        fid = &txFid;
        break;

    case DebugLevelRxRaw:
        fid = &rxRawFid;
        break;

    case DebugLevelRx:
        fid = &rxFid;
        break;

    case DebugLevelDigitallOffsetCompensation:
        fid = &ljFid;
        break;

    case DebugLevelTemperature:
        fid = &tempFid;
        break;
    }

    if (fid != nullptr && debugLevelEnabled((DebugLevels_t)debugLevel) && * fid != nullptr) {
        fclose(* fid);
        * fid = nullptr;
    }
}

void MessageDispatcher::joinCommunicationThreads() {
    if (threadsStarted) {
        deviceCommunicationThread.join();
        rxConsumerThread.join();
        liquidJunctionThread.join();

        threadsStarted = false;
    }
}

void MessageDispatcher::initializeCalibration() {
    TomlCalibrationManager tomlCalibrationManager(deviceId, currentChannelsNum, totalBoardsNum, vcCurrentRangesNum, vcVoltageRangesNum, ccVoltageRangesNum, ccCurrentRangesNum, samplingRatesNum, this->getSamplingRateModesNum());
    calibrationParams = tomlCalibrationManager.getCalibrationParams(calibrationLoadingError);
    if (calibrationLoadingError == Success) {
        calibrationFileNames = {tomlCalibrationManager.getCalibrationFileName()};
        calibrationFilesOkFlags = {{tomlCalibrationManager.getCalibrationFilesOkFlag()}};
    }
    else {
        CsvCalibrationManager csvCalibrationManager(deviceId, currentChannelsNum, totalBoardsNum, vcCurrentRangesNum, vcVoltageRangesNum, ccVoltageRangesNum, ccCurrentRangesNum, samplingRatesNum, this->getSamplingRateModesNum());

        calibrationParams = csvCalibrationManager.getCalibrationParams(calibrationLoadingError);
        calibrationFileNames = csvCalibrationManager.getCalibrationFileNames();
        calibrationFilesOkFlags = csvCalibrationManager.getCalibrationFilesOkFlags();
        calibrationMappingFileDir = csvCalibrationManager.getMappingFileDir();
        calibrationMappingFilePath = csvCalibrationManager.getMappingFilePath();
    }
    calibrationParams.sr2srm = sr2srm;
    originalCalibrationParams = calibrationParams;
}

void MessageDispatcher::deinitializeCalibration() {

}

void MessageDispatcher::initializeLiquidJunction() {
    liquidJunctionCurrentEstimatesNum = 0;
    offsetRecalibrationReadouts.resize(currentChannelsNum);
    std::fill(offsetRecalibrationReadouts.begin(), offsetRecalibrationReadouts.end(), 0.0);
    offsetRecalibrationReadCount.resize(currentChannelsNum);
    std::fill(offsetRecalibrationReadCount.begin(), offsetRecalibrationReadCount.end(), 0);
    liquidJunctionStatuses.resize(currentChannelsNum);
    std::fill(liquidJunctionStatuses.begin(), liquidJunctionStatuses.end(), LiquidJunctionNotPerformed);
    liquidJunctionStates.resize(currentChannelsNum);
    std::fill(liquidJunctionStates.begin(), liquidJunctionStates.end(), LiquidJunctionIdle);
    liquidJunctionCurrentSums.resize(currentChannelsNum);
    std::fill(liquidJunctionCurrentSums.begin(), liquidJunctionCurrentSums.end(), 0);
    liquidJunctionCurrentEstimates.resize(currentChannelsNum);
    std::fill(liquidJunctionCurrentEstimates.begin(), liquidJunctionCurrentEstimates.end(), 0.0);
    liquidJunctionMinPosSatVoltage.resize(currentChannelsNum);
    std::fill(liquidJunctionMinPosSatVoltage.begin(), liquidJunctionMinPosSatVoltage.end(), 0.0);
    liquidJunctionMaxNegSatVoltage.resize(currentChannelsNum);
    std::fill(liquidJunctionMaxNegSatVoltage.begin(), liquidJunctionMaxNegSatVoltage.end(), 0.0);
    liquidJunctionResistanceEstimates.resize(currentChannelsNum);
    std::fill(liquidJunctionResistanceEstimates.begin(), liquidJunctionResistanceEstimates.end(), 1.0); /*! Start assuming a very high resistance value */
    liquidJunctionSatResistanceEstimates.resize(currentChannelsNum);
    std::fill(liquidJunctionSatResistanceEstimates.begin(), liquidJunctionSatResistanceEstimates.end(), 1.0); /*! Start assuming a very high resistance value */
    liquidJunctionVoltagesBackup.resize(currentChannelsNum);
    std::fill(liquidJunctionVoltagesBackup.begin(), liquidJunctionVoltagesBackup.end(), Measurement({0.0, liquidJunctionRange.prefix, liquidJunctionRange.unit}));
    liquidJunctionDeltaVoltages.resize(currentChannelsNum);
    std::fill(liquidJunctionDeltaVoltages.begin(), liquidJunctionDeltaVoltages.end(), 0.0);
    liquidJunctionDeltaCurrents.resize(currentChannelsNum);
    std::fill(liquidJunctionDeltaCurrents.begin(), liquidJunctionDeltaCurrents.end(), 0.0);
    liquidJunctionSmallestCurrentChange.resize(currentChannelsNum);
    std::fill(liquidJunctionSmallestCurrentChange.begin(), liquidJunctionSmallestCurrentChange.end(), 1.0); /*! Start assuming the smallest current change is 1 current LSB */
    liquidJunctionConvergingCount.resize(currentChannelsNum);
    std::fill(liquidJunctionConvergingCount.begin(), liquidJunctionConvergingCount.end(), 0);
    liquidJunctionConvergedCount.resize(currentChannelsNum);
    std::fill(liquidJunctionConvergedCount.begin(), liquidJunctionConvergedCount.end(), 0);
    liquidJunctionSaturationCount.resize(currentChannelsNum);
    std::fill(liquidJunctionSaturationCount.begin(), liquidJunctionSaturationCount.end(), 0);
    liquidJunctionOpenCircuitCount.resize(currentChannelsNum);
    std::fill(liquidJunctionOpenCircuitCount.begin(), liquidJunctionOpenCircuitCount.end(), 0);

    ljResN.resize(currentChannelsNum);
    std::fill(ljResN.begin(), ljResN.end(), 0);
    ljResSx2.resize(currentChannelsNum);
    std::fill(ljResSx2.begin(), ljResSx2.end(), 0.0);
    ljResSx.resize(currentChannelsNum);
    std::fill(ljResSx.begin(), ljResSx.end(), 0.0);
    ljResSy.resize(currentChannelsNum);
    std::fill(ljResSy.begin(), ljResSy.end(), 0.0);
    ljResSxy.resize(currentChannelsNum);
    std::fill(ljResSxy.begin(), ljResSxy.end(), 0.0);
    ljTargetCurrents.resize(currentChannelsNum);
    std::fill(ljTargetCurrents.begin(), ljTargetCurrents.end(), 0.0);

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
    }
    else {
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

void MessageDispatcher::computeDataReadPolicy() {
    return;
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

ErrorCodes_t MessageDispatcher::setCompRanges(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <uint16_t> newRanges, bool applyFlag) {
    if (compValueMatrix.empty()) {
        return ErrorFeatureNotImplemented;
    }

    if (!allLessThan(newRanges, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }

    std::vector <std::vector <double> > localCompValueSubMatrix(channelIndexes.size());
    std::vector <double> newParams(channelIndexes.size());
    for (int chIdx = 0; chIdx < channelIndexes.size(); chIdx++) {
        localCompValueSubMatrix[chIdx] = this->compValueMatrix[channelIndexes[chIdx]];
    }

    switch (paramToUpdate) {
    case U_CpVc:
        if (pipetteCapValCompensationMultiCoders.empty()) {
            return ErrorFeatureNotImplemented;
        }

        for (int chIdx = 0; chIdx < channelIndexes.size(); chIdx++) {
            pipetteCapValCompensationMultiCoders[channelIndexes[chIdx]]->setEncodingRange(newRanges[chIdx]);
            newParams[chIdx] = pipetteCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(localCompValueSubMatrix[chIdx][paramToUpdate], txStatus);
        }
        break;

    case U_Cm:
        if (membraneCapValCompensationMultiCoders.empty()) {
            return ErrorFeatureNotImplemented;
        }

        for (int chIdx = 0; chIdx < channelIndexes.size(); chIdx++) {
            membraneCapValCompensationMultiCoders[channelIndexes[chIdx]]->setEncodingRange(newRanges[chIdx]);
            newParams[chIdx] = membraneCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(localCompValueSubMatrix[chIdx][paramToUpdate], txStatus);
        }
        break;

    case U_CpCc:
        if (pipetteCapCcValCompensationMultiCoders.empty()) {
            return ErrorFeatureNotImplemented;
        }

        for (int chIdx = 0; chIdx < channelIndexes.size(); chIdx++) {
            pipetteCapCcValCompensationMultiCoders[channelIndexes[chIdx]]->setEncodingRange(newRanges[chIdx]);
            newParams[chIdx] = pipetteCapCcValCompensationMultiCoders[channelIndexes[chIdx]]->encode(localCompValueSubMatrix[chIdx][paramToUpdate], txStatus);
        }
        break;

    default:
        return ErrorFeatureNotImplemented;
    }
    return this->setCompValues(channelIndexes, paramToUpdate, newParams, applyFlag);
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

ErrorCodes_t MessageDispatcher::sendSpiCommand(uint32_t command, uint32_t dataLoad) {
    if (spiCommandCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    spiCommandCoder->encode(command, txStatus);
    spiDataLoadCoder->encode(dataLoad, txStatus);

    this->stackOutgoingMessage(txStatus, {TxTriggerSpiSendCommand, ResetIndifferent});
    return Success;
}

ErrorCodes_t MessageDispatcher::setCustomFlag(uint16_t idx, bool flag, bool applyFlag) {
    if (idx >= customFlagsNum) {
        return ErrorValueOutOfRange;
    }
    customFlagsCoders[idx]->encode(flag ? 1 : 0, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCustomOption(uint16_t idx, uint16_t value, bool applyFlag) {
    if (idx >= customOptionsNum) {
        return ErrorValueOutOfRange;
    }
    if (value >= customOptionsDescriptions[idx].size()) {
        return ErrorValueOutOfRange;
    }
    customOptionsCoders[idx]->encode(value, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setCustomDouble(uint16_t idx, double value, bool applyFlag) {
    if (idx >= customDoublesNum) {
        return ErrorValueOutOfRange;
    }
    customDoublesCoders[idx]->encode(value, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher::setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status, bool applyFlag) {
    BoolCoder::CoderConfig_t boolConfig;
    boolConfig.initialWord = wordOffset;
    boolConfig.initialBit = bitOffset;
    boolConfig.bitsNum = 1;
    bitDebugCoder = new BoolArrayCoder(boolConfig);
    bitDebugCoder->encode(status ? 1 : 0, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    delete bitDebugCoder;
    return Success;
}

ErrorCodes_t MessageDispatcher::setDebugWord(uint16_t wordOffset, uint16_t wordValue) {
    BoolCoder::CoderConfig_t boolConfig;
    boolConfig.initialWord = wordOffset;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    wordDebugCoder = new BoolArrayCoder(boolConfig);
    wordDebugCoder->encode(wordValue, txStatus);
    this->stackOutgoingMessage(txStatus);
    delete wordDebugCoder;
    return Success;
}

ErrorCodes_t MessageDispatcher::setDebugTrigger(uint16_t bitOffset) {
    switch (bitOffset) {
    case TxTriggerParameteresUpdated:
    case TxTriggerSw:
    case TxTriggerStartProtocol:
    case TxTriggerStartStateArray:
    case TxTriggerZap:
    case TxTriggerSingleChannelRamp:
    case TxTriggerReadCalEeprom:
    case TxTriggerGetCalRam:
    case TxTriggerSetCalRam:
    case TxTriggerWriteCalEeprom:
    case TxTriggerReadOnTime:
    case TxTriggerSpiSendCommand:
    case TxTriggerDebug1:
    case TxTriggerDebug2:
    case TxTriggerDebug3:
    case TxTriggerDebug4:
        break;

    default:
        return ErrorValueOutOfRange;
    }
    /*! Issue a command which doesn't change anything (copy bit 0 in byte 0 and reapply it) */
    /*! This way the stackOutgoingMessage won't be masked by the lack of messages to be sent */
    this->setDebugBit(0, 0, (txStatus->encodingWords[0] & 0x0001) > 0 ? true : false, false);
    this->stackOutgoingMessage(txStatus, {static_cast <TxTriggerType_t> (bitOffset), ResetIndifferent});
    return Success;
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

void MessageDispatcher::resetCalibRShuntConductance(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibRShuntConductanceCoders.empty()) {
        return;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibRShuntConductanceCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(0.0, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return;
}

void MessageDispatcher::processLiquidJunctionData(RxMessage_t msg) {
    if (computeCurrentOffsetFlag) {
        int offset = 0;
        bool anyCommand;
        switch (liquidJunctionProcessing) {
        case LiquidJunctionProcessingWaitCommandApplied: {
            /*! This is to ensure that the command has been submitted to the FPGA */
            std::unique_lock <std::mutex> txMutexLock(txMutex);
            if (!liquidJunctionControlPending) {
                liquidJunctionProcessing = LiquidJunctionProcessingTransientsStarted;
            }
            txMutexLock.unlock();
        }
        break;

        case LiquidJunctionProcessingTransientsStarted:
            liquidJunctionCurrentEstimatesNum = 0;
            liquidJunctionProcessing = LiquidJunctionProcessingWaitTransients;
            break;

        case LiquidJunctionProcessingWaitTransients:
            liquidJunctionCurrentEstimatesNum += msg.data.size()/totalChannelsNum;
            if (liquidJunctionCurrentEstimatesNum < minLiquidJunctionTransientTimeS*samplingRate.getNoPrefixValue()) {
                break;
            }

            liquidJunctionCurrentEstimatesNum = 0;
            std::fill(liquidJunctionCurrentSums.begin(), liquidJunctionCurrentSums.end(), 0);

            liquidJunctionProcessing = LiquidJunctionProcessingCollectData;
            /*! No break because we can directly execute the next step without waiting for another message */

        case LiquidJunctionProcessingCollectData:
            offset = 0;
            while (offset < msg.data.size()) {
                offset += voltageChannelsNum;
                for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                    liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)((int16_t)msg.data[offset++]);
                }
                liquidJunctionCurrentEstimatesNum++;
            }

            anyCommand = this->computeOffetCorrection();
            anyCommand |= this->computeLiquidJunction();
            if (anyCommand) {
                liquidJunctionProcessing = LiquidJunctionProcessingWaitCommandApplied;
            }
            else if (liquidJunctionCurrentEstimatesNum > minLiquidJunctionEstimationTimeS*samplingRate.getNoPrefixValue()) {
                /*! Move to the next state only if enough data was collected, otherwise it means the process needs to collect more data */
                liquidJunctionProcessing = LiquidJunctionProcessingTransientsStarted;
            }

            computeCurrentOffsetFlag = anyOffsetRecalibrationActive || anyLiquidJunctionActive;
            break;
        }
    }
    else {
        /*! Whenever the computation is not active, reset the starting state */
        liquidJunctionProcessing = LiquidJunctionProcessingWaitCommandApplied;
    }
}

bool MessageDispatcher::computeOffetCorrection() {
    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    if (anyOffsetRecalibrationActive && liquidJunctionCurrentEstimatesNum > minLiquidJunctionEstimationTimeS*samplingRate.getNoPrefixValue()) {
        std::vector <uint16_t> channelIndexes;
        std::vector <Measurement_t> offsetRecalibCorrection;

        int16_t readoutOffsetInt;
        double readoutOffset;
        anyOffsetRecalibrationActive = false;

        for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            switch (offsetRecalibStates[channelIdx]) {
            case OffsetRecalibIdle:
                break;

            case OffsetRecalibStarting:
                anyOffsetRecalibrationActive = true;
                offsetRecalibStates[channelIdx] = OffsetRecalibFirstStep;
                offsetRecalibStatuses[channelIdx] = OffsetRecalibExecuting;
                offsetRecalibrationReadouts[channelIdx] = 32767.0;
                offsetRecalibrationReadCount[channelIdx] = 0;
                break;

            case OffsetRecalibFirstStep:
                anyOffsetRecalibrationActive = true;
                readoutOffsetInt = (int16_t)(((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum);
                this->convertCurrentValue(readoutOffsetInt, readoutOffset);
                offsetRecalibCorrection.push_back(calibrationParams.getValue(CalTypesVcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx));
                offsetRecalibCorrection.back().value -= readoutOffset;
                offsetRecalibStates[channelIdx] = OffsetRecalibCheck;
                channelIndexes.push_back(channelIdx);
                break;

            case OffsetRecalibCheck:
                anyOffsetRecalibrationActive = true;
                readoutOffset = abs((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;

                if (readoutOffset < 5.0) {
                    /*! current offset smaller than 5 LSB */
                    offsetRecalibStates[channelIdx] = OffsetRecalibSuccess;
                }
                else if (readoutOffset < offsetRecalibrationReadouts[channelIdx] && ++offsetRecalibrationReadCount[channelIdx] < 3) {
                    offsetRecalibrationReadouts[channelIdx] = readoutOffset;
                }
                else {
                    /*! current offset greater than 5 LSB */
                    offsetRecalibStates[channelIdx] = OffsetRecalibFail;
                }
                break;

            case OffsetRecalibSuccess:
                anyOffsetRecalibrationActive = true;
                offsetRecalibStates[channelIdx] = OffsetRecalibTerminate;
                offsetRecalibStatuses[channelIdx] = OffsetRecalibSucceded;
                break;

            case OffsetRecalibFail:
                anyOffsetRecalibrationActive = true;
                channelIndexes.push_back(channelIdx);
                offsetRecalibCorrection.push_back(originalCalibrationParams.getValue(CalTypesVcOffsetAdc, selectedClockDividerIdx, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx));
                offsetRecalibStates[channelIdx] = OffsetRecalibTerminate;
                offsetRecalibStatuses[channelIdx] = OffsetRecalibFailed;
                break;

            case OffsetRecalibTerminate:
                anyOffsetRecalibrationActive = true;
                offsetRecalibStates[channelIdx] = OffsetRecalibIdle;
                if (offsetRecalibStatuses[channelIdx] == OffsetRecalibExecuting) {
                    offsetRecalibStatuses[channelIdx] = OffsetRecalibInterrupted;
                }
                break;
            }
        }
        ljMutexLock.unlock();

        liquidJunctionControlPending = true;
        bool anyCommand = !channelIndexes.empty();
        if (anyCommand) {
            this->setCalibVcCurrentOffset(channelIndexes, offsetRecalibCorrection, true);
        }
        return anyCommand;
    }
    return false;
}

bool MessageDispatcher::computeLiquidJunction() {
    bool algoInFw = !(currentTrackingCoders.empty()); /*! \todo FCON il check sul current tracker è perchè al momento se il coder non esiste significa che la liquid junction è fatta in FW. meglio aggiungere una variabile dedicata  */
    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    if (anyLiquidJunctionActive && liquidJunctionCurrentEstimatesNum > minLiquidJunctionEstimationTimeS*samplingRate.getNoPrefixValue()) {
        std::vector <uint16_t> channelIndexes;
        std::vector <Measurement_t> voltages;
        int gain = 0;
        bool posCurr = true;
        bool posDeltaCurr = true;
        double deltaCurr = 0.0;
        double newSatRes = 1.0;
        int newGain = 0.0;

        Measurement_t voltage;
        anyLiquidJunctionActive = false;

        for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            switch (liquidJunctionStates[channelIdx]) {
            case LiquidJunctionIdle:
                gain = 0;
                break;

            case LiquidJunctionStarting:
                /*! Initialization and start data collection */
                anyLiquidJunctionActive = true;
                channelIndexes.push_back(channelIdx);
                liquidJunctionVoltagesBackup[channelIdx] = selectedLiquidJunctionVector[channelIdx];
                selectedLiquidJunctionVector[channelIdx].value = 0.0;
                voltages.push_back(selectedLiquidJunctionVector[channelIdx]);
                liquidJunctionSmallestCurrentChange[channelIdx] = 1.0;
                liquidJunctionResistanceEstimates[channelIdx] = 1.0;
                liquidJunctionSatResistanceEstimates[channelIdx] = 1.0;
                liquidJunctionMinPosSatVoltage[channelIdx] = liquidJunctionRange.max+1.0;
                liquidJunctionMaxNegSatVoltage[channelIdx] = liquidJunctionRange.min-1.0;
                liquidJunctionStates[channelIdx] = LiquidJunctionFirstStep;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionExecuting;
                liquidJunctionConvergingCount[channelIdx] = 0;
                liquidJunctionConvergedCount[channelIdx] = 0;
                liquidJunctionSaturationCount[channelIdx] = 0;
                liquidJunctionOpenCircuitCount[channelIdx] = 0;

                ljResN[channelIdx] = 0;
                ljResSx2[channelIdx] = 0.0;
                ljResSx[channelIdx] = 0.0;
                ljResSy[channelIdx] = 0.0;
                ljResSxy[channelIdx] = 0.0;

                break;

            case LiquidJunctionFirstStep:
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                posCurr = liquidJunctionCurrentEstimates[channelIdx] > 0.0;
                deltaCurr = ljTargetCurrents[channelIdx]-liquidJunctionCurrentEstimates[channelIdx];
                posDeltaCurr = deltaCurr > 0.0;
                if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = posDeltaCurr ? 100 : -100;
                    liquidJunctionStates[channelIdx] = posCurr ? LiquidJunctionOnlyPosSat : LiquidJunctionOnlyNegSat;
                }
                else {
                    /*! not saturating */
                    this->updateLjRes(channelIdx);

                    gain = posDeltaCurr ? 10 : -10;
                    liquidJunctionStates[channelIdx] = LiquidJunctionOnlyOneVal;
                }

                this->updateLjVoltage(channelIdx, gain, channelIndexes, voltages);
                liquidJunctionConvergingCount[channelIdx]++;
                this->updateLjState(channelIdx);
                break;

            case LiquidJunctionOnlyPosSat:
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                posCurr = liquidJunctionCurrentEstimates[channelIdx] > 0.0;
                deltaCurr = ljTargetCurrents[channelIdx]-liquidJunctionCurrentEstimates[channelIdx];
                posDeltaCurr = deltaCurr > 0.0;
                if (liquidJunctionCurrentEstimates[channelIdx] > 30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = -100*(1 << liquidJunctionSaturationCount[channelIdx]);
                    liquidJunctionStates[channelIdx] = LiquidJunctionOnlyPosSat;
                }
                else if (liquidJunctionCurrentEstimates[channelIdx] < -30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = (int)round(deltaCurr*liquidJunctionSatResistanceEstimates[channelIdx]/liquidJunctionResolution);
                    liquidJunctionStates[channelIdx] = LiquidJunctionOnlySat;
                }
                else {
                    /*! not saturating */
                    this->updateLjRes(channelIdx);
                    if (!posCurr) {
                        liquidJunctionSatResistanceEstimates[channelIdx] = (liquidJunctionMinPosSatVoltage[channelIdx]-selectedLiquidJunctionVector[channelIdx].value)/(32767.0-liquidJunctionCurrentEstimates[channelIdx]);
                    }

                    liquidJunctionSaturationCount[channelIdx] = 0;
                    gain = posDeltaCurr ? 10 : -10;
                    liquidJunctionStates[channelIdx] = LiquidJunctionSatAndOneVal;
                }

                this->updateLjVoltage(channelIdx, gain, channelIndexes, voltages);
                liquidJunctionConvergingCount[channelIdx]++;
                this->updateLjState(channelIdx);
                break;

            case LiquidJunctionOnlyNegSat:
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                posCurr = liquidJunctionCurrentEstimates[channelIdx] > 0.0;
                deltaCurr = ljTargetCurrents[channelIdx]-liquidJunctionCurrentEstimates[channelIdx];
                posDeltaCurr = deltaCurr > 0.0;
                if (liquidJunctionCurrentEstimates[channelIdx] > 30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = (int)round(deltaCurr*liquidJunctionSatResistanceEstimates[channelIdx]/liquidJunctionResolution);
                    liquidJunctionStates[channelIdx] = LiquidJunctionOnlySat;
                }
                else if (liquidJunctionCurrentEstimates[channelIdx] < -30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = 100*(1 << liquidJunctionSaturationCount[channelIdx]);
                    liquidJunctionStates[channelIdx] = LiquidJunctionOnlyNegSat;
                }
                else {
                    /*! not saturating */
                    this->updateLjRes(channelIdx);
                    if (posCurr) {
                        liquidJunctionSatResistanceEstimates[channelIdx] = (selectedLiquidJunctionVector[channelIdx].value-liquidJunctionMaxNegSatVoltage[channelIdx])/(liquidJunctionCurrentEstimates[channelIdx]-32768.0);
                    }

                    liquidJunctionSaturationCount[channelIdx] = 0;
                    gain = posDeltaCurr ? 10 : -10;
                    liquidJunctionStates[channelIdx] = LiquidJunctionSatAndOneVal;
                }

                this->updateLjVoltage(channelIdx, gain, channelIndexes, voltages);
                liquidJunctionConvergingCount[channelIdx]++;
                this->updateLjState(channelIdx);
                break;

            case LiquidJunctionOnlySat:
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                posCurr = liquidJunctionCurrentEstimates[channelIdx] > 0.0;
                deltaCurr = ljTargetCurrents[channelIdx]-liquidJunctionCurrentEstimates[channelIdx];
                posDeltaCurr = deltaCurr > 0.0;
                if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = (int)round(deltaCurr*liquidJunctionSatResistanceEstimates[channelIdx]/liquidJunctionResolution);
                    liquidJunctionStates[channelIdx] = LiquidJunctionOnlySat;
                }
                else {
                    /*! not saturating */
                    this->updateLjRes(channelIdx);

                    newSatRes = (liquidJunctionMinPosSatVoltage[channelIdx]-selectedLiquidJunctionVector[channelIdx].value)/(32767.0-liquidJunctionCurrentEstimates[channelIdx]);
                    if (newSatRes > 0.0 && newSatRes < liquidJunctionSatResistanceEstimates[channelIdx]) {
                        liquidJunctionSatResistanceEstimates[channelIdx] = newSatRes;
                    }
                    newSatRes = (selectedLiquidJunctionVector[channelIdx].value-liquidJunctionMaxNegSatVoltage[channelIdx])/(liquidJunctionCurrentEstimates[channelIdx]-32768.0);
                    if (newSatRes > 0.0 && newSatRes < liquidJunctionSatResistanceEstimates[channelIdx]) {
                        liquidJunctionSatResistanceEstimates[channelIdx] = newSatRes;
                    }

                    liquidJunctionSaturationCount[channelIdx] = 0;
                    newGain = (int)round(deltaCurr*liquidJunctionSatResistanceEstimates[channelIdx]/liquidJunctionResolution);
                    if (abs(newGain) > 10) {
                        gain = posDeltaCurr ? 10 : -10;
                    }
                    else {
                        gain = newGain;
                    }
                    liquidJunctionStates[channelIdx] = LiquidJunctionSatAndOneVal;
                }

                this->updateLjVoltage(channelIdx, gain, channelIndexes, voltages);
                liquidJunctionConvergingCount[channelIdx]++;
                this->updateLjState(channelIdx);
                break;

            case LiquidJunctionOnlyOneVal:
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                posCurr = liquidJunctionCurrentEstimates[channelIdx] > 0.0;
                deltaCurr = ljTargetCurrents[channelIdx]-liquidJunctionCurrentEstimates[channelIdx];
                posDeltaCurr = deltaCurr > 0.0;
                if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = posDeltaCurr ? 100 : -100;
                    liquidJunctionStates[channelIdx] = LiquidJunctionSatAndOneVal;
                }
                else {
                    /*! not saturating */
                    if (this->updateLjRes(channelIdx) || algoInFw) {
                        /*! If the resistance estimation is reliable try to use it to find the best votlage value */
                        liquidJunctionSmallestCurrentChange[channelIdx] = liquidJunctionResolution/liquidJunctionResistanceEstimates[channelIdx];
                        gain = (int)round(deltaCurr*liquidJunctionResistanceEstimates[channelIdx]/liquidJunctionResolution);
                        liquidJunctionStates[channelIdx] = LiquidJunctionTwoVals;
                    }
                    else {
                        /*! Otherwise jsut move one step in the right direction */
                        gain = posDeltaCurr ? 1 : -1;
                    }
                }

                this->updateLjVoltage(channelIdx, gain, channelIndexes, voltages);
                liquidJunctionConvergingCount[channelIdx]++;
                this->updateLjState(channelIdx);
                break;

            case LiquidJunctionSatAndOneVal:
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                posCurr = liquidJunctionCurrentEstimates[channelIdx] > 0.0;
                deltaCurr = ljTargetCurrents[channelIdx]-liquidJunctionCurrentEstimates[channelIdx];
                posDeltaCurr = deltaCurr > 0.0;
                if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = (int)round(deltaCurr*liquidJunctionSatResistanceEstimates[channelIdx]/liquidJunctionResolution);
                }
                else {
                    /*! not saturating */
                    newSatRes = (liquidJunctionMinPosSatVoltage[channelIdx]-selectedLiquidJunctionVector[channelIdx].value)/(32767.0-liquidJunctionCurrentEstimates[channelIdx]);
                    if (newSatRes > 0.0 && newSatRes < liquidJunctionSatResistanceEstimates[channelIdx]) {
                        liquidJunctionSatResistanceEstimates[channelIdx] = newSatRes;
                    }
                    newSatRes = (selectedLiquidJunctionVector[channelIdx].value-liquidJunctionMaxNegSatVoltage[channelIdx])/(liquidJunctionCurrentEstimates[channelIdx]-32768.0);
                    if (newSatRes > 0.0 && newSatRes < liquidJunctionSatResistanceEstimates[channelIdx]) {
                        liquidJunctionSatResistanceEstimates[channelIdx] = newSatRes;
                    }

                    liquidJunctionSaturationCount[channelIdx] = 0;

                    if (this->updateLjRes(channelIdx) || algoInFw) {
                        /*! If the resistance estimation is reliable try to use it to find the best votlage value */
                        liquidJunctionSmallestCurrentChange[channelIdx] = liquidJunctionResolution/liquidJunctionResistanceEstimates[channelIdx];
                        gain = (int)round(deltaCurr*liquidJunctionResistanceEstimates[channelIdx]/liquidJunctionResolution);
                        liquidJunctionStates[channelIdx] = LiquidJunctionTwoVals;
                    }
                    else {
                        newGain = (int)round(deltaCurr*liquidJunctionSatResistanceEstimates[channelIdx]/liquidJunctionResolution);
                        if (newGain != 0) {
                            /*! Move one step in the right direction */
                            gain = posDeltaCurr ? 1 : -1;
                        }
                        else {
                            /*! Or stay still */
                            gain = newGain;
                        }
                        liquidJunctionStates[channelIdx] = LiquidJunctionSatAndOneVal;
                    }
                }

                this->updateLjVoltage(channelIdx, gain, channelIndexes, voltages);
                liquidJunctionConvergingCount[channelIdx]++;
                this->updateLjState(channelIdx);
                break;

            case LiquidJunctionTwoVals:
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                posCurr = liquidJunctionCurrentEstimates[channelIdx] > 0.0;
                deltaCurr = ljTargetCurrents[channelIdx]-liquidJunctionCurrentEstimates[channelIdx];
                posDeltaCurr = deltaCurr > 0.0;
                if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 30000.0) {
                    /*! More or less 10% from saturation */
                    this->updateLjSatRes(channelIdx, posCurr);

                    liquidJunctionSaturationCount[channelIdx]++;
                    gain = (int)round(deltaCurr*liquidJunctionSatResistanceEstimates[channelIdx]/liquidJunctionResolution);
                }
                else if (abs(liquidJunctionCurrentEstimates[channelIdx]) < 5.0 * liquidJunctionSmallestCurrentChange[channelIdx]) {
                    gain = (int)round(deltaCurr*liquidJunctionResistanceEstimates[channelIdx]/liquidJunctionResolution);

                    liquidJunctionConvergedCount[channelIdx]++;
                    liquidJunctionSaturationCount[channelIdx] = 0;
                    liquidJunctionOpenCircuitCount[channelIdx] = 0;
                }
                else {
                    auto good = this->updateLjRes(channelIdx) || algoInFw;

                    gain = (int)round(deltaCurr*liquidJunctionResistanceEstimates[channelIdx]/liquidJunctionResolution);

                    liquidJunctionConvergedCount[channelIdx] = 0;
                    liquidJunctionSaturationCount[channelIdx] = 0;
                    if (good) {
                        liquidJunctionOpenCircuitCount[channelIdx] = 0;
                    }
                    else {
                        liquidJunctionOpenCircuitCount[channelIdx]++;
                    }
                }

                this->updateLjVoltage(channelIdx, gain, channelIndexes, voltages);
                liquidJunctionConvergingCount[channelIdx]++;
                this->updateLjState(channelIdx);
                break;

            case LiquidJunctionSuccess:
                anyLiquidJunctionActive = true;
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionSucceded;
                break;

            case LiquidJunctionFailOpenCircuit:
                anyLiquidJunctionActive = true;
                channelIndexes.push_back(channelIdx);
                voltages.push_back(liquidJunctionVoltagesBackup[channelIdx]);
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionFailedOpenCircuit;
                break;

            case LiquidJunctionFailTooManySteps:
                anyLiquidJunctionActive = true;
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionFailedTooManySteps;
                break;

            case LiquidJunctionFailSaturation:
                anyLiquidJunctionActive = true;
                channelIndexes.push_back(channelIdx);
                voltages.push_back(liquidJunctionVoltagesBackup[channelIdx]);
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionFailedSaturation;
                break;

            case LiquidJunctionTerminate:
                anyLiquidJunctionActive = true;
                liquidJunctionSmallestCurrentChange[channelIdx] = 1.0;
                liquidJunctionStates[channelIdx] = LiquidJunctionIdle;
                if (liquidJunctionStatuses[channelIdx] == LiquidJunctionExecuting) {
                    liquidJunctionStatuses[channelIdx] = LiquidJunctionInterrupted;
                }
                break;
            }
        }
        ljMutexLock.unlock();

        liquidJunctionControlPending = true;
        bool anyCommand = !channelIndexes.empty();
        if (anyCommand && !algoInFw) {
            this->setLiquidJunctionVoltage(channelIndexes, voltages, true);
        }
        return anyCommand;
    }
    else {
        ljMutexLock.unlock();
        if (debugLevelEnabled(DebugLevelDigitallOffsetCompensation)) {
            if (anyLiquidJunctionActive) {
                fprintf(ljFid,
                        "not performing: %lld.\n",
                        liquidJunctionCurrentEstimatesNum);
                fflush(ljFid);
            }
        }
        return false;
    }
}

void MessageDispatcher::processTemperatureData(RxMessage msg) {
    double * temperaturesD = new double[temperatureChannelsNum];
    this->convertTemperatureValues((int16_t *)msg.data.data(), temperaturesD);
    std::vector <Measurement_t> temperatures;
    for (uint16_t idx = 0; idx < temperatureChannelsNum; idx++) {
        temperatures.push_back({temperaturesD[idx], temperatureChannelsRanges[idx].prefix, temperatureChannelsRanges[idx].unit});
    }
    this->processTemperatureData(temperatures);
}

uint16_t MessageDispatcher::popUint16FromRxRawBuffer() {
    uint16_t value = (rxRawBuffer[rxRawBufferReadOffset] << 8) + rxRawBuffer[rxRawBufferReadOffset+1];
    rxRawBufferReadOffset = (rxRawBufferReadOffset+RX_WORD_SIZE) & rxRawBufferMask;
    rxRawBytesAvailable -= RX_WORD_SIZE;
    return value;
}

uint32_t MessageDispatcher::popUint32FromRxRawBuffer() {
    uint32_t value = (((uint32_t)rxRawBuffer[rxRawBufferReadOffset]) << 24) +
                     (((uint32_t)rxRawBuffer[rxRawBufferReadOffset+1]) << 16) +
                     (((uint32_t)rxRawBuffer[rxRawBufferReadOffset+2]) << 8) +
                     (uint32_t)rxRawBuffer[rxRawBufferReadOffset+3];

    rxRawBufferReadOffset = (rxRawBufferReadOffset+RX_32WORD_SIZE) & rxRawBufferMask;
    rxRawBytesAvailable -= RX_32WORD_SIZE;
    return value;
}

uint16_t MessageDispatcher::readUint16FromRxRawBuffer(uint32_t n) {
    uint16_t value = (rxRawBuffer[(rxRawBufferReadOffset+n) & rxRawBufferMask] << 8) + rxRawBuffer[(rxRawBufferReadOffset+n+1) & rxRawBufferMask];
    return value;
}

uint32_t MessageDispatcher::readUint32FromRxRawBuffer(uint32_t n) {
    uint32_t value = (((uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n) & rxRawBufferMask]) << 24) +
                     (((uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n+1) & rxRawBufferMask]) << 16) +
                     (((uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n+2) & rxRawBufferMask]) << 8) +
                     (uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n+3) & rxRawBufferMask];
    return value;
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

void MessageDispatcher::forceOutMessage() {
    txStatus->allChanged();
}

void MessageDispatcher::updateVoltageHoldTuner(bool applyFlag) {
    if (vHoldTunerCoders.empty()) {
        return;

    }
    for (uint32_t i = 0; i < currentChannelsNum; i++) {
        vHoldTunerCoders[selectedVcVoltageRangeIdx][i]->encode(selectedVoltageHoldVector[i].value, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
}

void MessageDispatcher::updateCurrentHoldTuner(bool applyFlag) {
    if (cHoldTunerCoders.empty()) {
        return;

    }
    for (uint32_t i = 0; i < voltageChannelsNum; i++) {
        cHoldTunerCoders[selectedCcCurrentRangeIdx][i]->encode(selectedCurrentHoldVector[i].value, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
}

ErrorCodes_t MessageDispatcher::resetProtocol() {
    if (protocolResetCoder == nullptr) {
        return this->stopProtocol();
    }
    protocolResetCoder->encode(1, txStatus);
    this->stackOutgoingMessage(txStatus, {TxTriggerStartProtocol, ResetIndifferent});
    protocolResetFlag = true;
    return Success;
}

void MessageDispatcher::stackOutgoingMessage(CommandStatus * txDataMessage, CommandOptions_t commandOptions) {
    if (txStatus->anyChanged) {
        std::unique_lock <std::mutex> txMutexLock(txMutex);
        while (txMsgBufferReadLength >= TX_MSG_BUFFER_SIZE) {
            txMsgBufferNotFull.wait_for (txMutexLock, std::chrono::milliseconds(100));
        }

        txMsgBuffer[txMsgBufferWriteOffset].clear();
        txMsgToBeSentWords[txMsgBufferWriteOffset].clear();
        for (int idx = 0; idx < txDataMessage->size; idx++) {
            if (txDataMessage->changedWords[idx]) {
                txMsgBuffer[txMsgBufferWriteOffset].push_back(txDataMessage->encodingWords[idx]);
                txMsgToBeSentWords[txMsgBufferWriteOffset].push_back(idx);
            }
        }
        txMsgOption[txMsgBufferWriteOffset] = commandOptions;

        txDataMessage->noneChanged();

        txMsgBufferWriteOffset = (txMsgBufferWriteOffset+1)&TX_MSG_BUFFER_MASK;
        txMsgBufferReadLength++;

        txMutexLock.unlock();
        txMsgBufferNotEmpty.notify_all();
    }
}

ErrorCodes_t MessageDispatcher::updateLiquidJunctionVoltage(uint16_t channelIdx, bool applyFlag) {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    else if (channelIdx >= currentChannelsNum) {
        return ErrorValueOutOfRange;
    }

    if (selectedClampingModality == VOLTAGE_CLAMP) {
        selectedLiquidJunctionVector[channelIdx].convertValue(liquidJunctionRange.prefix);
        selectedLiquidJunctionVector[channelIdx].value = liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIdx]->encode(selectedLiquidJunctionVector[channelIdx].value, txStatus);
    }
    else {
        liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIdx]->encode(0.0, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    return Success;
}

void MessageDispatcher::processTemperatureData(std::vector <Measurement_t> temperaturesRead) {
    return;
}

void MessageDispatcher::updateLjState(int channelIdx) {
    if (liquidJunctionStates[channelIdx] > LiquidJunctionIdle && liquidJunctionStates[channelIdx] < LiquidJunctionTerminate) {
        if (liquidJunctionConvergedCount[channelIdx] > 4 &&
            targetCurrentEnabled == false) {
            liquidJunctionStates[channelIdx] = LiquidJunctionSuccess;
        }
        else if (liquidJunctionConvergingCount[channelIdx] > 20 &&
                 targetCurrentEnabled == false) {
            liquidJunctionStates[channelIdx] = LiquidJunctionFailTooManySteps;
        }
        else if (liquidJunctionOpenCircuitCount[channelIdx] > 5) {
            liquidJunctionStates[channelIdx] = LiquidJunctionFailOpenCircuit;
        }
        else if (liquidJunctionSaturationCount[channelIdx] > 10) {
            liquidJunctionStates[channelIdx] = LiquidJunctionFailSaturation;
        }
    }
}

void MessageDispatcher::updateLjVoltage(int channelIdx, int gain, std::vector <uint16_t> &channelIndexes, std::vector <Measurement_t> &voltages) {
    if (gain != 0) {
        liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*(double)gain;

        /*! Clip the voltage */
        if (selectedLiquidJunctionVector[channelIdx].value + liquidJunctionDeltaVoltages[channelIdx] > liquidJunctionRange.max) {
            liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionRange.max-selectedLiquidJunctionVector[channelIdx].value;
        }
        else if (selectedLiquidJunctionVector[channelIdx].value + liquidJunctionDeltaVoltages[channelIdx] < liquidJunctionRange.min) {
            liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionRange.min-selectedLiquidJunctionVector[channelIdx].value;
        }
        selectedLiquidJunctionVector[channelIdx].value += liquidJunctionDeltaVoltages[channelIdx];

        channelIndexes.push_back(channelIdx);
        voltages.push_back(selectedLiquidJunctionVector[channelIdx]);
    }
}

bool MessageDispatcher::updateLjRes(int channelIdx) {
    double sn = (double)ljResN[channelIdx];
    double x = (double)liquidJunctionCurrentEstimates[channelIdx];
    double sx2 = ljResSx2[channelIdx];
    double sx = ljResSx[channelIdx];
    double sy = ljResSy[channelIdx];
    double sxy = ljResSxy[channelIdx];
    double y = selectedLiquidJunctionVector[channelIdx].value;
    double d = (sn+1.0)*(sx2+x*x)-(sx+x)*(sx+x);
    double n = (sn+1.0)*(sxy+x*y)-(sx+x)*(sy+y);
    if (ljResN[channelIdx] > 0 && d*n > 0 && abs(n) > 0.5/* && n/d < ljMaxRes[channelIdx]*/) {
        /*! at least 2 points, positive estimate and numerator not vanishingly small. Compute everything */
        ljResSxy[channelIdx] += x*y;
        ljResSx[channelIdx] += x;
        ljResSy[channelIdx] += y;
        ljResSx2[channelIdx] += x*x;
        ljResN[channelIdx]++;
        liquidJunctionResistanceEstimates[channelIdx] = n/d;
        return true;
    }
    if (ljResN[channelIdx] == 0/* || n/d >= ljMaxRes[channelIdx]*/) {
        /*! Only one point, update the parameters but do not compute the resistance */
        ljResSxy[channelIdx] += x*y;
        ljResSx[channelIdx] += x;
        ljResSy[channelIdx] += y;
        ljResSx2[channelIdx] += x*x;
        ljResN[channelIdx]++;
    }
    return false;
}

bool MessageDispatcher::updateLjSatRes(int channelIdx, bool posCurr) {
    if (selectedLiquidJunctionVector[channelIdx].value >= liquidJunctionMinPosSatVoltage[channelIdx] || selectedLiquidJunctionVector[channelIdx].value <= liquidJunctionMaxNegSatVoltage[channelIdx]) {
        /*! The computation is valid only if the new votlage value (x) lies between the max voltage for negative saturation (n) and the min voltage for positive saturation (p) */
        return false;
    }
    double t;
    if (posCurr) {
        /*! If the current was in high saturation update the voltage for positive saturation */
        t = (selectedLiquidJunctionVector[channelIdx].value-liquidJunctionMaxNegSatVoltage[channelIdx])/65535.0;
    }
    else {
        /*! If the current was in low saturation update the voltage for negative saturation */
        t = (liquidJunctionMinPosSatVoltage[channelIdx]-selectedLiquidJunctionVector[channelIdx].value)/65535.0;
    }
    if (t >= liquidJunctionSatResistanceEstimates[channelIdx]) {
        /*! If the new resistance estimate is bigger than the previous the votlage values are not closing in on the non saturation zone, so dismiss the result */
        return false;
    }
    if (posCurr) {
        /*! If the current was in high saturation update the voltage for positive saturation */
        liquidJunctionMinPosSatVoltage[channelIdx] = selectedLiquidJunctionVector[channelIdx].value;
    }
    else {
        /*! If the current was in low saturation update the voltage for negative saturation */
        liquidJunctionMaxNegSatVoltage[channelIdx] = selectedLiquidJunctionVector[channelIdx].value;
    }
    liquidJunctionSatResistanceEstimates[channelIdx] = t;
    return true;
}
