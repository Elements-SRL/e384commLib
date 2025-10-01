#include "emcrdevice.h"
#include "tomlcalibrationmanager.h"
#include "csvcalibrationmanager.h"
#include "speed_test.h"

/*****************\
 *  Ctor / Dtor  *
\*****************/

EmcrDevice::EmcrDevice(std::string deviceId) :
    MessageDispatcher(deviceId) {

    /*! Initialize rx word offsets and lengths with default values */
    rxWordOffsets.resize(RxMessageNum);
    rxWordLengths.resize(RxMessageNum);

    fill(rxWordOffsets.begin(), rxWordOffsets.end(), 0xFFFF);
    fill(rxWordLengths.begin(), rxWordLengths.end(), 0x0000);
}

EmcrDevice::~EmcrDevice() {
    /*! \todo FCON probabilmente crasha per via di alcuni coders che vengono sovrascritti. Meglio usare smart pointers */
    // for (auto coder : coders) {
    //     delete coder;
    // }
    coders.clear();
}

ErrorCodes_t EmcrDevice::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    frameManager->enableRxMessageType(messageType, flag);

    return Success;
}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t EmcrDevice::sendCommands() {
    this->forceOutMessage();
    this->stackOutgoingMessage(txStatus);
    return Success;
}

ErrorCodes_t EmcrDevice::startProtocol() {
    if (protocolResetCoder == nullptr) {
        this->forceOutMessage();
        this->stackOutgoingMessage(txStatus, {TxTriggerStartProtocol, ResetIndifferent});

    } else {
        if (protocolResetFlag == false) {
            /*! This is called if the startProtocol is called without protocol strucutre and items, jsut to repeat the previous protocol
              Since the protocol structure would otherwise be responsible for calling the stopProtocol, in this case the startProtocol does it instead */
            this->stopProtocol();
        }
        this->stackOutgoingMessage(txStatus); /*! Make sure the registers are submitted */
        protocolResetCoder->encode(0, txStatus);
        this->stackOutgoingMessage(txStatus); /*! Then take the protocol out of the reset state */
        protocolResetFlag = false;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::stopProtocol() {
    if (protocolResetCoder == nullptr) {
        bool stopProtocolFlag = false; /*! We're already commiting a stop protocol, so commiting another one on the protocol structure will create an infinite recursion */
        if (selectedClampingModality == ClampingModality_t::VOLTAGE_CLAMP || selectedClampingModality == ClampingModality_t::VOLTAGE_CLAMP_VOLTAGE_READ) {
            this->setVoltageProtocolStructure(selectedProtocolId-1, 1, 1, selectedProtocolVrest, stopProtocolFlag);
            this->setVoltageProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "V"}, {0.0, UnitPfxNone, "V"}, {20.0, UnitPfxMilli, "s"}, {0.0, UnitPfxNone, "s"}, false);

        } else {
            this->setCurrentProtocolStructure(selectedProtocolId-1, 1, 1, selectedProtocolIrest, stopProtocolFlag);
            this->setCurrentProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "A"}, {0.0, UnitPfxNone, "A"}, {20.0, UnitPfxMilli, "s"}, {0.0, UnitPfxNone, "s"}, false);
        }
        return this->startProtocol();

    } else {
        protocolResetCoder->encode(1, txStatus);
        this->stackOutgoingMessage(txStatus, {TxTriggerStartProtocol, ResetIndifferent});
        protocolResetFlag = true;
        return Success;
    }
}

ErrorCodes_t EmcrDevice::startStateArray() {
    if (numberOfStatesCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    this->stopProtocol();
    this->forceOutMessage();
    this->stackOutgoingMessage(txStatus, {TxTriggerStartStateArray, ResetIndifferent});
    return Success;
}

ErrorCodes_t EmcrDevice::zap(std::vector <uint16_t> channelIndexes, Measurement_t duration) {
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

ErrorCodes_t EmcrDevice::resetAsic(bool resetFlag, bool applyFlag) {
    if (asicResetCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    asicResetCoder->encode(resetFlag, txStatus);

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus, {TxTriggerParameteresUpdated, resetFlag ? ResetTrue : ResetFalse});
    }
    return Success;
}

ErrorCodes_t EmcrDevice::resetFpga(bool resetFlag, bool applyFlag) {
    if (fpgaResetCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    fpgaResetCoder->encode(resetFlag, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setVoltageHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) {
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP && selectedClampingModality != VOLTAGE_CLAMP_VOLTAGE_READ) {
        return ErrorWrongClampModality;
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

    if (!allInRange(voltages, vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMin(), vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCurrentHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool applyFlag) {
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality == VOLTAGE_CLAMP || selectedClampingModality == VOLTAGE_CLAMP_VOLTAGE_READ) {
        return ErrorWrongClampModality;
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

    if (!allInRange(currents, ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMin(), ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setVoltageHalf(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) {
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP && selectedClampingModality != VOLTAGE_CLAMP_VOLTAGE_READ) {
        return ErrorWrongClampModality;
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

    if (!allInRange(voltages, vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMin(), vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCurrentHalf(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool applyFlag) {
    if (cHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality == VOLTAGE_CLAMP || selectedClampingModality == VOLTAGE_CLAMP_VOLTAGE_READ) {
        return ErrorWrongClampModality;
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

    if (!allInRange(currents, ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMin(), ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMax())) {
        return WarningValueClipped;

    }
    return Success;
}

ErrorCodes_t EmcrDevice::setLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    else if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    else if (selectedClampingModality != VOLTAGE_CLAMP && selectedClampingModality != VOLTAGE_CLAMP_VOLTAGE_READ) {
        return ErrorWrongClampModality;
    }

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        selectedLiquidJunctionVector[channelIndexes[i]] = voltages[i];
        this->updateLiquidJunctionVoltage(channelIndexes[i], false);
        channelModels[channelIndexes[i]]->setLiquidJunctionVoltage(selectedLiquidJunctionVector[channelIndexes[i]]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!allInRange(voltages, liquidJunctionRange.getMin(), liquidJunctionRange.getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::updateLiquidJunctionVoltage(uint16_t channelIdx, bool applyFlag) {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    else if (channelIdx >= currentChannelsNum) {
        return ErrorValueOutOfRange;
    }

    if (selectedClampingModality == VOLTAGE_CLAMP || selectedClampingModality == VOLTAGE_CLAMP_VOLTAGE_READ) {
        if (compensationsEnableFlags[CompRsCorr].empty()) {
            selectedLiquidJunctionVector[channelIdx].convertValue(liquidJunctionRange.prefix);
            selectedLiquidJunctionVector[channelIdx].value = liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIdx]->encode(selectedLiquidJunctionVector[channelIdx].value, txStatus);
        }
        else if (compensationsEnableFlags[CompRsCorr][channelIdx] && !(calibrationParams.types[CalTypesRsCorrOffsetDac].modes.empty())) {
            calibrationParams.convertValue(CalTypesRsCorrOffsetDac, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx, liquidJunctionRange.prefix);
            selectedLiquidJunctionVector[channelIdx].convertValue(liquidJunctionRange.prefix);
            selectedLiquidJunctionVector[channelIdx].value = liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIdx]->encode(
                                                                 selectedLiquidJunctionVector[channelIdx].value+calibrationParams.getValue(CalTypesRsCorrOffsetDac, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx).value,
                                                                 txStatus)-calibrationParams.getValue(CalTypesRsCorrOffsetDac, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx).value;
        }
        else {
            selectedLiquidJunctionVector[channelIdx].convertValue(liquidJunctionRange.prefix);
            selectedLiquidJunctionVector[channelIdx].value = liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIdx]->encode(selectedLiquidJunctionVector[channelIdx].value, txStatus);
        }
    }
    else {
        liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIdx]->encode(0.0, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    return Success;
}

ErrorCodes_t EmcrDevice::setGateVoltages(std::vector <uint16_t> boardIndexes, std::vector <Measurement_t> gateVoltages, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setSourceVoltages(std::vector <uint16_t> boardIndexes, std::vector <Measurement_t> sourceVoltages, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setCalibParams(CalibrationParams_t calibParams) {
    for (int type = 0; type < CalTypesNum; type++) {
        if (!calibParams.types[type].modes.empty() && calibrationParams.types[type].modes.empty()) {
            calibrationParams.types[type] = calibParams.types[type];
        }
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcCurrentGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibVcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibVcCurrentGainRange.prefix);
        calibrationParams.setValue(CalTypesVcGainAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], gains[i]);
    }
    this->updateCalibVcCurrentGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcCurrentGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcGainAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], calibVcCurrentGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesVcGainAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibVcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibVcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        calibrationParams.setValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], offsets[i]);
    }
    this->updateCalibVcCurrentOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        double offset = calibrationParams.getValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibVcCurrentOffsetCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcVoltageGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibCcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibCcVoltageGainRange.prefix);
        calibrationParams.setValue(CalTypesCcGainAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], gains[i]);
    }
    this->updateCalibCcVoltageGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcVoltageGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcGainAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], calibCcVoltageGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesCcGainAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibCcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibCcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx[channelIndexes[i]]].prefix);
        calibrationParams.setValue(CalTypesCcOffsetAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], offsets[i]);
    }
    this->updateCalibCcVoltageOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcOffsetAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i], calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx[channelIndexes[i]]].prefix);
        double offset = calibrationParams.getValue(CalTypesCcOffsetAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibCcVoltageOffsetCoders[selectedCcVoltageRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcVoltageGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibVcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibVcVoltageGainRange.prefix);
        calibrationParams.setValue(CalTypesVcGainDac, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], gains[i]);
    }
    this->updateCalibVcVoltageGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcVoltageGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcGainDac, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], calibVcVoltageGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesVcGainDac, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i]).value;
        calibVcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibVcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        calibrationParams.setValue(CalTypesVcOffsetDac, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], offsets[i]);
    }
    this->updateCalibVcVoltageOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcVoltageOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibVcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesVcOffsetDac, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i], calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        double offset = calibrationParams.getValue(CalTypesVcOffsetDac, selectedSamplingRateIdx, selectedVcVoltageRangeIdx, channelIndexes[i]).value;
        calibVcVoltageOffsetCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcCurrentGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) {
    if (calibCcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        gains[i].convertValue(calibCcCurrentGainRange.prefix);
        calibrationParams.setValue(CalTypesCcGainDac, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], gains[i]);
    }
    this->updateCalibCcCurrentGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcCurrentGain(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcGainDac, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], calibCcCurrentGainRange.prefix);
        double gain = calibrationParams.getValue(CalTypesCcGainDac, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i]).value;
        calibCcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibCcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
        calibrationParams.setValue(CalTypesCcOffsetDac, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], offsets[i]);
    }
    this->updateCalibCcCurrentOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcCurrentOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibCcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesCcOffsetDac, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i], calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
        double offset = calibrationParams.getValue(CalTypesCcOffsetDac, selectedSamplingRateIdx, selectedCcCurrentRangeIdx, channelIndexes[i]).value;
        calibCcCurrentOffsetCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibRsCorrOffsetDac(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (calibRsCorrOffsetDacCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        offsets[i].convertValue(liquidJunctionRangesArray[selectedLiquidJunctionRangeIdx].prefix);
        calibrationParams.setValue(CalTypesRsCorrOffsetDac, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], offsets[i]);
    }
    this->updateCalibRsCorrOffsetDac(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibRsCorrOffsetDac(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibRsCorrOffsetDacCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesRsCorrOffsetDac, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], liquidJunctionRangesArray[selectedLiquidJunctionRangeIdx].prefix);
        double offset = calibrationParams.getValue(CalTypesRsCorrOffsetDac, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibRsCorrOffsetDacCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(offset, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibRShuntConductance(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> conductances, bool applyFlag) {
    if (calibRShuntConductanceCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        conductances[i].convertValue(rRShuntConductanceCalibRange[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        calibrationParams.setValue(CalTypesRShuntConductance, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], conductances[i]);
    }
    this->updateCalibRShuntConductance(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibRShuntConductance(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibRShuntConductanceCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibrationParams.convertValue(CalTypesRShuntConductance, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i], rRShuntConductanceCalibRange[selectedVcCurrentRangeIdx[channelIndexes[i]]].prefix);
        double conductance = calibrationParams.getValue(CalTypesRShuntConductance, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIndexes[i]], channelIndexes[i]).value;
        calibRShuntConductanceCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(conductance, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::resetCalibRShuntConductance(std::vector <uint16_t> channelIndexes, bool applyFlag) {
    if (calibRShuntConductanceCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        calibRShuntConductanceCoders[selectedVcCurrentRangeIdx[channelIndexes[i]]][channelIndexes[i]]->encode(0.0, txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    std::vector <uint16_t> currentRangeIdxs;
    currentRangeIdxs.resize(currentChannelsNum);
    std::fill(currentRangeIdxs.begin(), currentRangeIdxs.end(), currentRangeIdx);
    return setVCCurrentRange(allChannelIndexes, currentRangeIdxs, applyFlag);
}

ErrorCodes_t EmcrDevice::setVCCurrentRange(std::vector <uint16_t> channelIndexes, std::vector <uint16_t> currentRangeIdx, bool applyFlag) {
    if (vcCurrentRangeCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (!allLessThan(currentRangeIdx, (uint16_t)vcCurrentRangesNum)) {
        return ErrorValueOutOfRange;
    }
    for (int idx = 0; idx < channelIndexes.size(); idx++) {
        auto chIdx = channelIndexes[idx];
        if (independentVcCurrentRanges) {
            vcCurrentRangeCoders[chIdx]->encode(currentRangeIdx[idx], txStatus);
        }
        else {
            vcCurrentRangeCoders[0]->encode(currentRangeIdx[idx], txStatus);
        }
        selectedVcCurrentRangeIdx[chIdx] = currentRangeIdx[idx];
        currentRanges[chIdx] = vcCurrentRangesArray[selectedVcCurrentRangeIdx[chIdx]];
        currentResolutions[chIdx] = currentRanges[chIdx].step;
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

ErrorCodes_t EmcrDevice::setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) {
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
    /*! Most of the times the liquid junction (aka digital offset compensation) will be performed by the same DAC that appliese the voltage sitmulus
            Voltage clamp, so by default the same range is selected for the liquid junction
            When this is not the case the boolean variable below is set properly by the corresponding derived class of the messagedispatcher */
    if (liquidJunctionSameRangeAsVcDac) {
        this->setLiquidJunctionRange(voltageRangeIdx);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) {
    std::vector <uint16_t> voltageRangeIdxs;
    voltageRangeIdxs.resize(currentChannelsNum);
    std::fill(voltageRangeIdxs.begin(), voltageRangeIdxs.end(), voltageRangeIdx);
    return setCCVoltageRange(allChannelIndexes, voltageRangeIdxs, applyFlag);
}

ErrorCodes_t EmcrDevice::setCCVoltageRange(std::vector <uint16_t> channelIndexes, std::vector <uint16_t> voltageRangeIdx, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setLiquidJunctionRange(uint16_t idx) {
    if (idx >= liquidJunctionRangesNum) {
        return ErrorValueOutOfRange;
    }
    selectedLiquidJunctionRangeIdx = idx;
    liquidJunctionRange = liquidJunctionRangesArray[selectedLiquidJunctionRangeIdx];
    liquidJunctionResolution = liquidJunctionRange.step;

    this->setLiquidJunctionVoltage(allChannelIndexes, selectedLiquidJunctionVector, true);

    return Success;
}

ErrorCodes_t EmcrDevice::setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::enableStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::turnChannelsOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::turnCalSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::hasCalSw() {
    if (calSwCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::turnVcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::turnCcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setAdcCore(std::vector <uint16_t> channelIndexes, std::vector <ClampingModality_t> clampingModes, bool applyFlag) {
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

        case VOLTAGE_CLAMP_VOLTAGE_READ:
            vcCcSelCoders[channelIndexes[i]]->encode(0, txStatus);
            break;

        case CURRENT_CLAMP_CURRENT_READ:
            vcCcSelCoders[channelIndexes[i]]->encode(1, txStatus);
            break;

        case UNDEFINED_CLAMP:
            return ErrorFeatureNotImplemented;
        }
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::enableCcStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setClampingModality(uint32_t idx, bool applyFlag, bool stopProtocolFlag) {
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

        if (previousClampingModality == VOLTAGE_CLAMP || previousClampingModality == VOLTAGE_CLAMP_VOLTAGE_READ) {
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

    case VOLTAGE_CLAMP_VOLTAGE_READ:
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

        if (previousClampingModality != VOLTAGE_CLAMP_VOLTAGE_READ) {
            this->enableVcCompensations(false, false);
            this->enableCcCompensations(false, false);
        }
        this->turnVoltageReaderOn(true, false);
        this->turnVoltageStimulusOn(true, false);
        this->turnCurrentReaderOn(false, false);
        this->turnCurrentStimulusOn(false, true);

        this->turnVcSwOn(allChannelIndexes, trues, false);

        this->setVCVoltageRange(selectedVcVoltageRangeIdx, false);
        this->setCCVoltageRange(allChannelIndexes, selectedCcVoltageRangeIdx, false);

        this->setSourceForVoltageChannel(1, false);
        this->setSourceForCurrentChannel(1, false);

        break;

    case CURRENT_CLAMP_CURRENT_READ:
        rawDataFilterVoltageFlag = false;
        rawDataFilterCurrentFlag = true;

        if (previousClampingModality != CURRENT_CLAMP_CURRENT_READ) {
            this->enableVcCompensations(false, false);
            this->enableCcCompensations(false, false);
        }
        this->turnCurrentReaderOn(true, false);
        this->turnCurrentStimulusOn(true, false);
        this->turnVoltageReaderOn(false, false);
        this->turnVoltageStimulusOn(false, true);

        this->turnCcSwOn(allChannelIndexes, trues, false);

        this->setCCCurrentRange(selectedCcCurrentRangeIdx, false);
        this->setVCCurrentRange(allChannelIndexes, selectedVcCurrentRangeIdx, false);

        this->setSourceForVoltageChannel(0, false);
        this->setSourceForCurrentChannel(0, false);

        break;
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setSourceForVoltageChannel(uint16_t source, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setSourceForCurrentChannel(uint16_t source, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::readoutOffsetRecalibration(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (liquidJunctionCompensationCoders.empty()) {
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
        liquidJunctionCompensationCoders[chIdx]->encode(onValues[i], txStatus); /*!< Disables protocols and vhold */
        channelModels[chIdx]->setRecalibratingReadoutOffset(onValues[i]);
        if (onValues[i] && (offsetRecalibStates[chIdx] == OffsetRecalibIdle)) {
            offsetRecalibStates[chIdx] = OffsetRecalibStarting;
            offsetRecalibStatuses[chIdx] = OffsetRecalibNotPerformed;

        } else if (!onValues[i] && (offsetRecalibStates[chIdx] != OffsetRecalibIdle)) {
            offsetRecalibStates[chIdx] = OffsetRecalibTerminate;
        }
    }
    anyOffsetRecalibrationActive = true;
    computeCurrentOffsetFlag = true;

    ljMutexLock.unlock();

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::liquidJunctionCompensation(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
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

    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        uint16_t chIdx = channelIndexes[i];
        liquidJunctionCompensationCoders[chIdx]->encode(onValues[i], txStatus); /*!< Disables protocols and vhold */
        channelModels[chIdx]->setCompensatingLiquidJunction(onValues[i]);
        if (onValues[i] && (liquidJunctionStates[chIdx] == LiquidJunctionIdle)) {
            liquidJunctionStates[chIdx] = LiquidJunctionStarting;
            liquidJunctionStatuses[chIdx] = LiquidJunctionNotPerformed;
        }
        else if (!onValues[i] && (liquidJunctionStates[chIdx] != LiquidJunctionIdle)) {
            liquidJunctionStates[chIdx] = LiquidJunctionTerminate;
        }
    }
    anyLiquidJunctionActive = true;
    computeCurrentOffsetFlag = true;

    ljMutexLock.unlock();

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setAdcFilter(bool applyFlag) {
    switch (selectedClampingModality) {
    case VOLTAGE_CLAMP:
    case CURRENT_CLAMP_CURRENT_READ:
        if (vcCurrentFilterCoder != nullptr) {
            vcCurrentFilterCoder->encode(sr2LpfVcCurrentMap[selectedSamplingRateIdx], txStatus);
            selectedVcCurrentFilterIdx = sr2LpfVcCurrentMap[selectedSamplingRateIdx];
        }
        break;

    case CURRENT_CLAMP:
    case ZERO_CURRENT_CLAMP:
    case VOLTAGE_CLAMP_VOLTAGE_READ:
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

ErrorCodes_t EmcrDevice::setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) {
    if (samplingRateCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (samplingRateIdx >= samplingRatesNum) {
        return ErrorValueOutOfRange;
    }
    samplingRateCoder->encode(samplingRateIdx, txStatus);
    selectedSamplingRateIdx = samplingRateIdx;
    samplingRate = realSamplingRatesArray[selectedSamplingRateIdx];
    integrationStep = integrationStepArray[selectedSamplingRateIdx];
    this->setAdcFilter();
    this->computeRawDataFilterCoefficients();
    switch (selectedClampingModality) {
    case VOLTAGE_CLAMP:
    case CURRENT_CLAMP_CURRENT_READ:
        this->updateCalibVcCurrentGain(allChannelIndexes, false);
        this->updateCalibVcCurrentOffset(allChannelIndexes, false);
        break;

    case CURRENT_CLAMP:
    case ZERO_CURRENT_CLAMP:
    case VOLTAGE_CLAMP_VOLTAGE_READ:
        this->updateCalibCcVoltageGain(allChannelIndexes, false);
        this->updateCalibCcVoltageOffset(allChannelIndexes, false);
        break;
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

ErrorCodes_t EmcrDevice::setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status) {
    BoolCoder::CoderConfig_t boolConfig;
    boolConfig.initialWord = wordOffset;
    boolConfig.initialBit = bitOffset;
    boolConfig.bitsNum = 1;
    bitDebugCoder = new BoolArrayCoder(boolConfig);
    bitDebugCoder->encode(status ? 1 : 0, txStatus);
    this->stackOutgoingMessage(txStatus);
    delete bitDebugCoder;
    return Success;
}

ErrorCodes_t EmcrDevice::setDebugWord(uint16_t wordOffset, uint16_t wordValue) {
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

ErrorCodes_t EmcrDevice::setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest, bool stopProtocolFlag) {
    if (voltageProtocolRestCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (itemsNum > protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;
    }
    if (stopProtocolFlag) {
        this->stopProtocol();
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

ErrorCodes_t EmcrDevice::setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) {
    if (!voltageProtocolStepImplemented) {
        return ErrorFeatureNotImplemented;

    } else if (itemIdx >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0) || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(v0Step) ||
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

ErrorCodes_t EmcrDevice::setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) {
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

ErrorCodes_t EmcrDevice::setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag) {
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

ErrorCodes_t EmcrDevice::setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest, bool stopProtocolFlag) {
    if (currentProtocolRestCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (itemsNum > protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;
    }
    if (stopProtocolFlag) {
        this->stopProtocol();
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

ErrorCodes_t EmcrDevice::setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag) {
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

ErrorCodes_t EmcrDevice::setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag) {
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

ErrorCodes_t EmcrDevice::setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag) {
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

ErrorCodes_t EmcrDevice::setCompRanges(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <uint16_t> newRanges, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setStateArrayStructure(int numberOfStates, int initialState, Measurement_t reactionTime) {
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

ErrorCodes_t EmcrDevice::setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, Measurement_t timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag) {
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

ErrorCodes_t EmcrDevice::setStateArrayEnabled(int chIdx, bool enabledFlag) {
    if (enableStateArrayChannelsCoder.empty()) {
        return ErrorFeatureNotImplemented;
    }
    enableStateArrayChannelsCoder[chIdx]->encode(enabledFlag, txStatus);
    return Success;
}

ErrorCodes_t EmcrDevice::setCustomFlag(uint16_t idx, bool flag, bool applyFlag) {
    if (idx >= customFlagsNum) {
        return ErrorValueOutOfRange;
    }
    customFlagsCoders[idx]->encode(flag ? 1 : 0, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCustomOption(uint16_t idx, uint16_t value, bool applyFlag) {
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

ErrorCodes_t EmcrDevice::setCustomDouble(uint16_t idx, double value, bool applyFlag) {
    if (idx >= customDoublesNum) {
        return ErrorValueOutOfRange;
    }
    customDoublesCoders[idx]->encode(value, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::getNextMessage(RxOutput_t &rxOutput, int16_t * data) {
    ErrorCodes_t ret = Success;
    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged or if an error is returned before this can be set to its proper value */

    if (parsingStatus != ParsingParsing) {
        return ErrorDeviceNotConnected;
    }

    uint32_t samplesNum = 0;
    uint32_t sampleIdx = 0; /*! Data index in the read message */
    uint32_t outSampleIdx = 0; /*! Data index in the written buffer */
    uint32_t timeSamplesNum;
    int16_t rawFloat;
    bool keepReading = true;

    while (keepReading) {
        sampleIdx = 0;
        auto msg = frameManager->getNextMessage();
        if (msg.typeId == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
            return ErrorNoDataAvailable;
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
                            outSampleIdx++;
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

        case (MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus):
            // not really managed, ignore it
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTemperature):
            /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
            rxOutput.dataLen = temperatureChannelsNum;
            /*! \todo FCON check sulla lunghezza del messaggio */
            for (uint16_t temperatureChannelIdx = 0; temperatureChannelIdx < temperatureChannelsNum; temperatureChannelIdx++) {
                data[temperatureChannelIdx] = (int16_t)msg.data[sampleIdx++];
            }
            break;
        }
        keepReading = msg.mergeable && (rxOutput.dataLen+frameManager->getMaxDataMessageSize() < E384CL_OUT_STRUCT_DATA_LEN);
    }

#ifdef SPT_LOG_GET_NEXT_MESSAGE
    speedTestLog(SpeedTestGetNextMessage, rxOutput.dataLen*2);
#endif
    return ret;
}

ErrorCodes_t EmcrDevice::purgeData() {
    frameManager->purgeData();

    return Success;
}

ErrorCodes_t EmcrDevice::getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures) {
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageHoldTunerFeatures = vcVoltageRangesArray;
    return Success;
}

ErrorCodes_t EmcrDevice::getVoltageHoldTuner(std::vector <Measurement_t> &voltages) {
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltages = selectedVoltageHoldVector;
    return Success;
}

ErrorCodes_t EmcrDevice::getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTunerFeatures) {
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageHalfTunerFeatures = vcVoltageRangesArray;
    return Success;
}

ErrorCodes_t EmcrDevice::getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures) {
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentHoldTunerFeatures = ccCurrentRangesArray;
    return Success;
}

ErrorCodes_t EmcrDevice::getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTunerFeatures) {
    if (cHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentHalfTunerFeatures = ccCurrentRangesArray;
    return Success;
}

ErrorCodes_t EmcrDevice::getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges) {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    ranges = liquidJunctionRangesArray;
    return Success;
}

ErrorCodes_t EmcrDevice::hasGateVoltages() {
    if (gateVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::hasSourceVoltages() {
    if (sourceVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures) {
    if (gateVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    gateVoltagesFeatures = gateVoltageRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures) {
    if (sourceVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    sourceVoltagesFeatures = sourceVoltageRange;
    return Success;
}

ErrorCodes_t EmcrDevice::hasChannelSwitches() {
    if (turnChannelsOnCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::hasStimulusSwitches() {
    if (enableStimulusCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::hasOffsetCompensation() {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::hasStimulusHalf() {
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::isStateArrayAvailable() {
    if (numberOfStatesCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}


ErrorCodes_t EmcrDevice::getZapFeatures(RangedMeasurement_t &durationRange) {
    if (zapCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    durationRange = zapDurationRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibParams(CalibrationParams_t &calibParams) {
    for (int type = 0; type < CalTypesNum; type++) {
        if (!calibrationParams.types[type].modes.empty()) {
            calibParams.types[type] = calibrationParams.types[type];
        }
    }
    calibParams.sr2srm = calibrationParams.sr2srm;
    return calibrationLoadingError;
}

ErrorCodes_t EmcrDevice::getCalibFileNames(std::vector <std::string> &calibFileNames) {
    calibFileNames = calibrationFileNames;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibFilesFlags(std::vector <std::vector <bool> > &calibFilesFlags) {
    calibFilesFlags = calibrationFilesOkFlags;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibMappingFileDir(std::string &dir) {
    dir = calibrationMappingFileDir;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibMappingFilePath(std::string &path) {
    path = calibrationMappingFilePath;
    return Success;
}

void EmcrDevice::processLiquidJunctionData(RxMessage_t msg) {
    if (computeCurrentOffsetFlag) {
        int offset = 0;
        bool anyCommand;
        switch (liquidJunctionProcessing) {
        case LiquidJunctionProcessingTransientsStarted:
            liquidJunctionTransientsStartTime = std::chrono::steady_clock::now();
            liquidJunctionProcessing = LiquidJunctionProcessingWaitTransients;
            break;

        case LiquidJunctionProcessingWaitTransients:
            if (std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::steady_clock::now()-liquidJunctionTransientsStartTime).count() < minLiquidJunctionTransientTimeS*1000.0) {
                break;
            }
            else {
                liquidJunctionProcessing = LiquidJunctionProcessingResetRequired;
                /*! No break because we can directly execute the next step without waiting for another message */
            }

        case LiquidJunctionProcessingResetRequired:
            liquidJunctionCurrentEstimatesNum = 0;
            std::fill(liquidJunctionCurrentSums.begin(), liquidJunctionCurrentSums.end(), 0);

            liquidJunctionProcessing = LiquidJunctionProcessingCollectData;
            /*! No break because we can directly execute the next step without waiting for another message */

        case LiquidJunctionProcessingCollectData:
            offset = 0;
            while (offset < msg.data.size()) {
                offset += voltageChannelsNum;
                for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                    if (computeCurrentOffsetFlag) {
                        liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)((int16_t)msg.data[offset++]);
                    }
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
                /*! \todo FCON meglio gestire più uscite dalla funzione precedente piuttosto che fare con un booleano ed un conto astruso */
                liquidJunctionProcessing = LiquidJunctionProcessingTransientsStarted;
            }

            computeCurrentOffsetFlag = anyOffsetRecalibrationActive || anyLiquidJunctionActive;
            break;

        case LiquidJunctionProcessingWaitCommandApplied:
            /*! This is to ensure that the command has been submitted to the FPGA */
            std::unique_lock <std::mutex> txMutexLock(txMutex);
            if (!liquidJunctionControlPending) {
                liquidJunctionProcessing = LiquidJunctionProcessingTransientsStarted;
            }
            txMutexLock.unlock();
            break;
        }
    }
}

bool EmcrDevice::computeOffetCorrection() {
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
                break;

            case OffsetRecalibFirstStep:
                anyOffsetRecalibrationActive = true;
                readoutOffsetInt = (int16_t)(((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum);
                this->convertCurrentValue(readoutOffsetInt, readoutOffset);
                offsetRecalibCorrection.push_back(calibrationParams.getValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx));
                offsetRecalibCorrection.back().value -= readoutOffset;
                offsetRecalibStates[channelIdx] = OffsetRecalibCheck;
                channelIndexes.push_back(channelIdx);
                break;

            case OffsetRecalibCheck:
                anyOffsetRecalibrationActive = true;
                readoutOffset = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;

                if (abs(readoutOffset) < 5.0) {
                    /*! current offset smaller than 5 LSB */
                    offsetRecalibStates[channelIdx] = OffsetRecalibSuccess;
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
                offsetRecalibCorrection.push_back(originalCalibrationParams.getValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx[channelIdx], channelIdx));
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
    else {
        return false;
    }
}

bool EmcrDevice::computeLiquidJunction() {
    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    if (anyLiquidJunctionActive && liquidJunctionCurrentEstimatesNum > minLiquidJunctionEstimationTimeS*samplingRate.getNoPrefixValue()) {
        std::vector <uint16_t> channelIndexes;
        std::vector <Measurement_t> voltages;

        Measurement_t voltage;
        double estimatedResistance;
        anyLiquidJunctionActive = false;

        for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            switch (liquidJunctionStates[channelIdx]) {
            case LiquidJunctionIdle:
                break;

            case LiquidJunctionStarting:
                /*! Initialization and start data collection */
                anyLiquidJunctionActive = true;
                channelIndexes.push_back(channelIdx);
                liquidJunctionVoltagesBackup[channelIdx] = selectedLiquidJunctionVector[channelIdx];
                voltages.push_back(selectedLiquidJunctionVector[channelIdx]);
                liquidJunctionStates[channelIdx] = LiquidJunctionFirstStep;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionExecuting;
                liquidJunctionConvergingCount[channelIdx] = 0;
                liquidJunctionConvergedCount[channelIdx] = 0;
                liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                liquidJunctionOpenCircuitCount[channelIdx] = 0;

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                fprintf(ljFid,
                        "%d: starting. "
                        "Liq jun %s, "
                        "conving %d, "
                        "convd %d, "
                        "possat %d, "
                        "negsat %d, "
                        "opencirc %d\n",
                        channelIdx,
                        selectedLiquidJunctionVector[channelIdx].label().c_str(),
                        liquidJunctionConvergingCount[channelIdx],
                        liquidJunctionConvergedCount[channelIdx],
                        liquidJunctionPositiveSaturationCount[channelIdx],
                        liquidJunctionNegativeSaturationCount[channelIdx],
                        liquidJunctionOpenCircuitCount[channelIdx]);
                fflush(ljFid);
#endif
                break;

            case LiquidJunctionFirstStep:
                /*! First impedance estimation */
                anyLiquidJunctionActive = true;
                liquidJunctionCurrentEstimates[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum;
                if (liquidJunctionCurrentEstimates[channelIdx] > 30000.0) {
                    /*! More or less 10% from saturation */
                    liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*100.0;

                }
                else if (liquidJunctionCurrentEstimates[channelIdx] > 0.0) {
                    /*! Positive but not saturating */
                    liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*10.0;

                }
                else if (liquidJunctionCurrentEstimates[channelIdx] < -30000.0) {
                    /*! More or less 10% from saturation */
                    liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*100.0;

                }
                else {
                    /*! Negative but not saturating */
                    liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*10.0;
                }
                liquidJunctionStates[channelIdx] = LiquidJunctionConverge;
                selectedLiquidJunctionVector[channelIdx].value += liquidJunctionDeltaVoltages[channelIdx];
                channelIndexes.push_back(channelIdx);
                voltages.push_back(selectedLiquidJunctionVector[channelIdx]);

                liquidJunctionConvergingCount[channelIdx]++;

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                fprintf(ljFid,
                        "%d: first. "
                        "Curr est %f, "
                        "DV %f, "
                        "Liq jun %s, "
                        "conving %d, "
                        "convd %d, "
                        "possat %d, "
                        "negsat %d, "
                        "opencirc %d\n",
                        channelIdx,
                        liquidJunctionCurrentEstimates[channelIdx],
                        liquidJunctionDeltaVoltages[channelIdx],
                        selectedLiquidJunctionVector[channelIdx].label().c_str(),
                        liquidJunctionConvergingCount[channelIdx],
                        liquidJunctionConvergedCount[channelIdx],
                        liquidJunctionPositiveSaturationCount[channelIdx],
                        liquidJunctionNegativeSaturationCount[channelIdx],
                        liquidJunctionOpenCircuitCount[channelIdx]);
                fflush(ljFid);
#endif
                break;

            case LiquidJunctionConverge:
                /*! Steps towards convergence */
                anyLiquidJunctionActive = true;
                liquidJunctionDeltaCurrents[channelIdx] = ((double)liquidJunctionCurrentSums[channelIdx])/(double)liquidJunctionCurrentEstimatesNum-liquidJunctionCurrentEstimates[channelIdx];
                liquidJunctionCurrentEstimates[channelIdx] += liquidJunctionDeltaCurrents[channelIdx];

                if (liquidJunctionCurrentEstimates[channelIdx] > 30000.0) {
                    /*! More or less 10% from saturation,
                     *  change DAC by 100 LSB in the opposite direction */
                    liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution*100.0;
                    liquidJunctionConvergedCount[channelIdx] = 0;
                    liquidJunctionPositiveSaturationCount[channelIdx]++;
                    liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                    liquidJunctionOpenCircuitCount[channelIdx] = 0;
                }
                else if (liquidJunctionCurrentEstimates[channelIdx] < -30000.0) {
                    /*! More or less 10% from saturation,
                     *  change DAC by 100 LSB in the opposite direction */
                    liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution*100.0;
                    liquidJunctionConvergedCount[channelIdx] = 0;
                    liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                    liquidJunctionNegativeSaturationCount[channelIdx]++;
                    liquidJunctionOpenCircuitCount[channelIdx] = 0;
                }
                else if (abs(liquidJunctionCurrentEstimates[channelIdx]) < 2.0*liquidJunctionSmallestCurrentChange[channelIdx]) {
                    /*! Current very close to 0,
                     *  change DAC by 1 LSB to find the optimal value */
                    if (liquidJunctionDeltaCurrents[channelIdx] > 0.0) {
                        liquidJunctionDeltaVoltages[channelIdx] = -liquidJunctionResolution;
                    }
                    else {
                        liquidJunctionDeltaVoltages[channelIdx] = liquidJunctionResolution;
                    }
                    liquidJunctionConvergedCount[channelIdx]++;
                    liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                    liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                    liquidJunctionOpenCircuitCount[channelIdx] = 0;
                }
                else if (liquidJunctionDeltaCurrents[channelIdx]*liquidJunctionDeltaVoltages[channelIdx] > 0.0) {
                    /*! Current value not small enough, but current change consistent with voltage change,
                     *  estimate resistance and estimate the next DAC value */
                    estimatedResistance = liquidJunctionDeltaVoltages[channelIdx]/liquidJunctionDeltaCurrents[channelIdx];
                    liquidJunctionSmallestCurrentChange[channelIdx] = liquidJunctionResolution/estimatedResistance;
                    liquidJunctionDeltaVoltages[channelIdx] = round(-liquidJunctionCurrentEstimates[channelIdx]*estimatedResistance/liquidJunctionResolution)*liquidJunctionResolution;
                    if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 5.0*liquidJunctionSmallestCurrentChange[channelIdx]) {
                        liquidJunctionConvergedCount[channelIdx] = 0;
                    }
                    liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                    liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                    liquidJunctionOpenCircuitCount[channelIdx] = 0;
                }
                else {
                    /*! Current not so small and delta current inconsistent with delta voltage, probable open circuit,
                     *  Perform another step without updating the estimated resistance value */
                    liquidJunctionDeltaVoltages[channelIdx] = round(-liquidJunctionCurrentEstimates[channelIdx]*estimatedResistance/liquidJunctionResolution)*liquidJunctionResolution;
                    if (abs(liquidJunctionCurrentEstimates[channelIdx]) > 5.0*liquidJunctionSmallestCurrentChange[channelIdx]) {
                        liquidJunctionConvergedCount[channelIdx] = 0;
                    }
                    liquidJunctionPositiveSaturationCount[channelIdx] = 0;
                    liquidJunctionNegativeSaturationCount[channelIdx] = 0;
                    liquidJunctionOpenCircuitCount[channelIdx]++;
                }

                selectedLiquidJunctionVector[channelIdx].value += liquidJunctionDeltaVoltages[channelIdx];
                liquidJunctionConvergingCount[channelIdx]++;

                channelIndexes.push_back(channelIdx);
                voltages.push_back(selectedLiquidJunctionVector[channelIdx]);

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                fprintf(ljFid,
                        "%d: converge. "
                        "DI %f, "
                        "Curr est %f, "
                        "DV %f, "
                        "Liq jun %s, "
                        "conving %d, "
                        "convd %d, "
                        "possat %d, "
                        "negsat %d, "
                        "opencirc %d\n",
                        channelIdx,
                        liquidJunctionDeltaCurrents[channelIdx],
                        liquidJunctionCurrentEstimates[channelIdx],
                        liquidJunctionDeltaVoltages[channelIdx],
                        selectedLiquidJunctionVector[channelIdx].label().c_str(),
                        liquidJunctionConvergingCount[channelIdx],
                        liquidJunctionConvergedCount[channelIdx],
                        liquidJunctionPositiveSaturationCount[channelIdx],
                        liquidJunctionNegativeSaturationCount[channelIdx],
                        liquidJunctionOpenCircuitCount[channelIdx]);
                fflush(ljFid);
#endif

                if (liquidJunctionConvergingCount[channelIdx] > 20) {
                    liquidJunctionStates[channelIdx] = LiquidJunctionFailTooManySteps;
                }
                else if (liquidJunctionConvergedCount[channelIdx] > 2) {
                    liquidJunctionStates[channelIdx] = LiquidJunctionSuccess;
                }
                else if (liquidJunctionOpenCircuitCount[channelIdx] > 5) {
                    liquidJunctionStates[channelIdx] = LiquidJunctionFailOpenCircuit;
                }
                else if (liquidJunctionPositiveSaturationCount[channelIdx] > 10 || liquidJunctionNegativeSaturationCount[channelIdx] > 10) {
                    liquidJunctionStates[channelIdx] = LiquidJunctionFailSaturation;
                }
                break;

            case LiquidJunctionSuccess:
                anyLiquidJunctionActive = true;
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionSucceded;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                fprintf(ljFid,
                        "%d: success.",
                        channelIdx);
                fflush(ljFid);
#endif
                break;

            case LiquidJunctionFailOpenCircuit:
                anyLiquidJunctionActive = true;
                channelIndexes.push_back(channelIdx);
                voltages.push_back(liquidJunctionVoltagesBackup[channelIdx]);
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionFailedOpenCircuit;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                fprintf(ljFid,
                        "%d: open circuit.",
                        channelIdx);
                fflush(ljFid);
#endif
                break;

            case LiquidJunctionFailTooManySteps:
                anyLiquidJunctionActive = true;
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionFailedTooManySteps;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                fprintf(ljFid,
                        "%d: too many steps.",
                        channelIdx);
                fflush(ljFid);
#endif
                break;

            case LiquidJunctionFailSaturation:
                anyLiquidJunctionActive = true;
                channelIndexes.push_back(channelIdx);
                voltages.push_back(liquidJunctionVoltagesBackup[channelIdx]);
                liquidJunctionStates[channelIdx] = LiquidJunctionTerminate;
                liquidJunctionStatuses[channelIdx] = LiquidJunctionFailedSaturation;
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
                fprintf(ljFid,
                        "%d: saturation.",
                        channelIdx);
                fflush(ljFid);
#endif
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
        if (anyCommand) {
            this->setLiquidJunctionVoltage(channelIndexes, voltages, true);
        }
        return anyCommand;
    }
    else {
        ljMutexLock.unlock();
#ifdef DEBUG_LIQUID_JUNCTION_PRINT
        if (anyLiquidJunctionActive) {
            fprintf(ljFid,
                    "not performing: %lld.\n",
                    liquidJunctionCurrentEstimatesNum);
            fflush(ljFid);
        }
#endif
        return false;
    }
}

void EmcrDevice::processTemperatureData(RxMessage_t msg) {
    double * temperaturesD = new double[temperatureChannelsNum];
    this->convertTemperatureValues((int16_t *)msg.data.data(), temperaturesD);
    std::vector <Measurement_t> temperatures;
    for (uint16_t idx = 0; idx < temperatureChannelsNum; idx++) {
        temperatures.push_back({temperaturesD[idx], temperatureChannelsRanges[idx].prefix, temperatureChannelsRanges[idx].unit});
    }
    this->processTemperatureData(temperatures);
}

uint16_t EmcrDevice::popUint16FromRxRawBuffer() {
    uint16_t value = (rxRawBuffer[rxRawBufferReadOffset] << 8) + rxRawBuffer[rxRawBufferReadOffset+1];
    rxRawBufferReadOffset = (rxRawBufferReadOffset+RX_WORD_SIZE) & rxRawBufferMask;
    rxRawBytesAvailable -= RX_WORD_SIZE;
    return value;
}

uint32_t EmcrDevice::popUint32FromRxRawBuffer() {
    uint32_t value = (((uint32_t)rxRawBuffer[rxRawBufferReadOffset]) << 24) +
                     (((uint32_t)rxRawBuffer[rxRawBufferReadOffset+1]) << 16) +
                     (((uint32_t)rxRawBuffer[rxRawBufferReadOffset+2]) << 8) +
                     (uint32_t)rxRawBuffer[rxRawBufferReadOffset+3];

    rxRawBufferReadOffset = (rxRawBufferReadOffset+RX_32WORD_SIZE) & rxRawBufferMask;
    rxRawBytesAvailable -= RX_32WORD_SIZE;
    return value;
}

uint16_t EmcrDevice::readUint16FromRxRawBuffer(uint32_t n) {
    uint16_t value = (rxRawBuffer[(rxRawBufferReadOffset+n) & rxRawBufferMask] << 8) + rxRawBuffer[(rxRawBufferReadOffset+n+1) & rxRawBufferMask];
    return value;
}

uint32_t EmcrDevice::readUint32FromRxRawBuffer(uint32_t n) {
    uint32_t value = (((uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n) & rxRawBufferMask]) << 24) +
                     (((uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n+1) & rxRawBufferMask]) << 16) +
                     (((uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n+2) & rxRawBufferMask]) << 8) +
                     (uint32_t)rxRawBuffer[(rxRawBufferReadOffset+n+3) & rxRawBufferMask];
    return value;
}

/*********************\
 *  Private methods  *
\*********************/

ErrorCodes_t EmcrDevice::initialize(std::string fwPath) {
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

void EmcrDevice::deinitialize() {
    stopConnectionFlag = true;
    this->joinCommunicationThreads();

    this->deinitializeVariables();

    this->deinitializeMemory();

    this->stopCommunication();

    this->closeDebugFiles();
}

ErrorCodes_t EmcrDevice::initializeMemory() {
    txMsgBuffer = new (std::nothrow) std::vector <uint16_t>[TX_MSG_BUFFER_SIZE];
    if (txMsgBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txMsgToBeSentWords.resize(TX_MSG_BUFFER_SIZE);
    txMsgOption.resize(TX_MSG_BUFFER_SIZE);

    selectedVcCurrentRangeIdx.resize(currentChannelsNum);
    std::fill(selectedVcCurrentRangeIdx.begin(), selectedVcCurrentRangeIdx.end(), defaultVcCurrentRangeIdx);

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

void EmcrDevice::initializeVariables() {
    MessageDispatcher::initializeVariables();
    frameManager = new FrameManager(this);
    frameManager->setMaxDataMessageSize(totalChannelsNum*packetsPerFrame);
    frameManager->setRxWordParams(rxWordOffsets, rxWordLengths);
    frameManager->setCurrentBlockLength(rxCurrentBlockLength);
}

ErrorCodes_t EmcrDevice::deviceConfiguration() {
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
    ErrorCodes_t ret = MessageDispatcher::deviceConfiguration();
    return ret;
}

void EmcrDevice::createCommunicationThreads() {
    deviceCommunicationThread = std::thread(&EmcrDevice::handleCommunicationWithDevice, this);
    rxConsumerThread = std::thread(&EmcrDevice::parseDataFromDevice, this);
    liquidJunctionThread = std::thread(&EmcrDevice::computeLiquidJunction, this);

    threadsStarted = true;
}

void EmcrDevice::deinitializeMemory() {
    if (txMsgBuffer != nullptr) {
        delete [] txMsgBuffer;
        txMsgBuffer = nullptr;
    }
}

void EmcrDevice::deinitializeVariables() {
    MessageDispatcher::deinitializeVariables();
    if (frameManager != nullptr) {
        delete frameManager;
        frameManager = nullptr;
    }
}

void EmcrDevice::joinCommunicationThreads() {
    if (threadsStarted) {
        deviceCommunicationThread.join();
        rxConsumerThread.join();
        liquidJunctionThread.join();

        threadsStarted = false;
    }
}

void EmcrDevice::initializeCalibration() {
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

void EmcrDevice::forceOutMessage() {
    txStatus.allChanged();
}

void EmcrDevice::updateVoltageHoldTuner(bool applyFlag) {
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

void EmcrDevice::updateCurrentHoldTuner(bool applyFlag) {
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

void EmcrDevice::stackOutgoingMessage(CommandStatus_t &txDataMessage, CommandOptions_t commandOptions) {
    if (txStatus.anyChanged) {
        std::unique_lock <std::mutex> txMutexLock(txMutex);
        while (txMsgBufferReadLength >= TX_MSG_BUFFER_SIZE) {
            txMsgBufferNotFull.wait_for (txMutexLock, std::chrono::milliseconds(100));
        }

        txMsgBuffer[txMsgBufferWriteOffset].clear();
        txMsgToBeSentWords[txMsgBufferWriteOffset].clear();
        for (int idx = 0; idx < txDataMessage.size; idx++) {
            if (txDataMessage.changedWords[idx]) {
                txMsgBuffer[txMsgBufferWriteOffset].push_back(txDataMessage.encodingWords[idx]);
                txMsgToBeSentWords[txMsgBufferWriteOffset].push_back(idx);
            }
        }
        txMsgOption[txMsgBufferWriteOffset] = commandOptions;

        txDataMessage.noneChanged();

        txMsgBufferWriteOffset = (txMsgBufferWriteOffset+1)&TX_MSG_BUFFER_MASK;
        txMsgBufferReadLength++;

        txMutexLock.unlock();
        txMsgBufferNotEmpty.notify_all();
    }
}

void EmcrDevice::processTemperatureData(std::vector <Measurement_t> temperaturesRead) {
    return;
}
