#include "emcrdevice.h"
#include "calibrationmanager.h"

EmcrDevice::EmcrDevice(std::string deviceId) :
    MessageDispatcher(deviceId) {

    rxEnabledTypesMap.resize(MsgDirectionDeviceToPc*2);
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAck] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdNack] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdPing] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdFpgaReset] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdInvalid] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus] = false;

    /*! Initialize rx word offsets and lengths with default values */
    rxWordOffsets.resize(RxMessageNum);
    rxWordLengths.resize(RxMessageNum);

    fill(rxWordOffsets.begin(), rxWordOffsets.end(), 0xFFFF);
    fill(rxWordLengths.begin(), rxWordLengths.end(), 0x0000);
}

EmcrDevice::~EmcrDevice() {
    for (auto coder : coders) {
        delete coder;
    }
}

ErrorCodes_t EmcrDevice::connect(std::string fwPath) {
    if (connected) {
        return ErrorDeviceAlreadyConnected;
    }

    connected = true;

    this->init();

    stopConnectionFlag = false;

#ifdef DEBUG_TX_DATA_PRINT
    if (txFid == nullptr) {
        createDebugFile(txFid, "e384CommLib_tx");
    }
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
    if (rxRawFid == nullptr) {
        createDebugFile(rxRawFid, "e384CommLib_rxRaw");
    }
#endif

#ifdef DEBUG_RX_PROCESSING_PRINT
    if (rxProcFid == nullptr) {
        createDebugFile(rxProcFid, "e384CommLib_rxProcessing");
    }
#endif

#ifdef DEBUG_RX_DATA_PRINT
    if (rxFid == nullptr) {
        createDebugFile(rxFid, "e384CommLib_rx");
    }
#endif

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
    if (ljFid == nullptr) {
        createDebugFile(ljFid, "e384CommLib_lj");
    }
#endif

    /*! Initialize device */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    this->initializeDevice();

    deviceCommunicationThread = std::thread(&EmcrDevice::handleCommunicationWithDevice, this);
    rxConsumerThread = std::thread(&EmcrDevice::parseDataFromDevice, this);
    liquidJunctionThread = std::thread(&EmcrDevice::computeLiquidJunction, this);

    threadsStarted = true;

    return Success;
}

ErrorCodes_t EmcrDevice::disconnect() {
    if (!connected || stopConnectionFlag) {
        return ErrorDeviceNotConnected;
    }

    stopConnectionFlag = true;

    if (threadsStarted) {
        deviceCommunicationThread.join();
        rxConsumerThread.join();
        liquidJunctionThread.join();
    }

    this->deinit();
    this->flushBoardList();

    connected = false;

    return Success;
}

ErrorCodes_t EmcrDevice::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
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

ErrorCodes_t EmcrDevice::sendCommands() {
    this->forceOutMessage();
    this->stackOutgoingMessage(txStatus);
    return Success;
}

ErrorCodes_t EmcrDevice::startProtocol() {
    this->forceOutMessage();
    this->stackOutgoingMessage(txStatus, TxTriggerStartProtocol);
    return Success;
}

ErrorCodes_t EmcrDevice::startStateArray() {
    this->forceOutMessage();
    this->stackOutgoingMessage(txStatus, TxTriggerStartStateArray);
    return Success;
}

ErrorCodes_t EmcrDevice::resetAsic(bool resetFlag, bool applyFlagIn) {
    if (asicResetCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    asicResetCoder->encode(resetFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::resetFpga(bool resetFlag, bool applyFlagIn) {
    if (fpgaResetCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    fpgaResetCoder->encode(resetFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag){
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }

    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        voltages[i].convertValue(vHoldRange[selectedVcVoltageRangeIdx].prefix);
        voltages[i].value = vHoldTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedVoltageHoldVector[channelIndexes[i]] = voltages[i];
        channelModels[channelIndexes[i]]->setVhold(voltages[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!areAllTheVectorElementsInRange(voltages, vHoldRange[selectedVcVoltageRangeIdx].getMin(), vHoldRange[selectedVcVoltageRangeIdx].getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag){
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality == VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        currents[i].convertValue(cHoldRange[selectedCcCurrentRangeIdx].prefix);
        currents[i].value = cHoldTunerCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(currents[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedCurrentHoldVector[channelIndexes[i]] = currents[i];
        channelModels[channelIndexes[i]]->setChold(currents[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!areAllTheVectorElementsInRange(currents, cHoldRange[selectedCcCurrentRangeIdx].getMin(), cHoldRange[selectedCcCurrentRangeIdx].getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setVoltageHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag){
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        voltages[i].convertValue(vHalfRange[selectedVcVoltageRangeIdx].prefix);
        voltages[i].value = vHalfTunerCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedVoltageHalfVector[channelIndexes[i]] = voltages[i];
        channelModels[channelIndexes[i]]->setVhalf(voltages[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!areAllTheVectorElementsInRange(voltages, vHalfRange[selectedVcVoltageRangeIdx].getMin(), vHalfRange[selectedVcVoltageRangeIdx].getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCurrentHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag){
    if (cHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality == VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        currents[i].convertValue(cHalfRange[selectedCcCurrentRangeIdx].prefix);
        currents[i].value = cHalfTunerCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(currents[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedCurrentHalfVector[channelIndexes[i]] = currents[i];
        channelModels[channelIndexes[i]]->setChalf(currents[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!areAllTheVectorElementsInRange(currents, cHalfRange[selectedCcCurrentRangeIdx].getMin(), cHalfRange[selectedCcCurrentRangeIdx].getMax())) {
        return WarningValueClipped;

    }
    return Success;
}

ErrorCodes_t EmcrDevice::setLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag){
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;

    } else if (selectedClampingModality != VOLTAGE_CLAMP) {
        return ErrorWrongClampModality;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        voltages[i].convertValue(liquidJunctionRange.prefix);
        voltages[i].value = liquidJunctionVoltageCoders[selectedLiquidJunctionRangeIdx][channelIndexes[i]]->encode(voltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedLiquidJunctionVector[channelIndexes[i]] = voltages[i];
        channelModels[channelIndexes[i]]->setLiquidJunctionVoltage(voltages[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!areAllTheVectorElementsInRange(voltages, liquidJunctionRange.getMin(), liquidJunctionRange.getMax())) {
        return WarningValueClipped;

    }
    return Success;
}

ErrorCodes_t EmcrDevice::resetLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, bool applyFlagIn) {
    std::vector<Measurement_t> voltages(channelIndexes.size(), {0.0, liquidJunctionRange.prefix, "V"});
    return setLiquidJunctionVoltage(channelIndexes, voltages, applyFlagIn);
}

ErrorCodes_t EmcrDevice::setGateVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag){
    if (gateVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < boardIndexes.size(); i++){
        gateVoltages[i].convertValue(gateVoltageRange.prefix);
        gateVoltages[i].value = gateVoltageCoders[boardIndexes[i]]->encode(gateVoltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedGateVoltageVector[boardIndexes[i]] = gateVoltages[i];
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!areAllTheVectorElementsInRange(gateVoltages, gateVoltageRange.getMin(), gateVoltageRange.getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setSourceVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag){
    if (sourceVoltageCoders.size() == 0) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(boardIndexes, totalBoardsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < boardIndexes.size(); i++){
        sourceVoltages[i].convertValue(sourceVoltageRange.prefix);
        sourceVoltages[i].value = sourceVoltageCoders[boardIndexes[i]]->encode(sourceVoltages[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        selectedSourceVoltageVector[boardIndexes[i]] = sourceVoltages[i];
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }

    if (!areAllTheVectorElementsInRange(sourceVoltages, sourceVoltageRange.getMin(), sourceVoltageRange.getMax())) {
        return WarningValueClipped;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibParams(CalibrationParams_t calibParams){
    if (!calibParams.allGainAdcMeas.empty() && calibrationParams.allGainAdcMeas.empty()){
        calibrationParams.allGainAdcMeas = calibParams.allGainAdcMeas;
    }

    if (!calibParams.allOffsetAdcMeas.empty() && !calibrationParams.allOffsetAdcMeas.empty()){
        calibrationParams.allOffsetAdcMeas = calibParams.allOffsetAdcMeas;
    }

    if (!calibParams.allGainDacMeas.empty() && !calibrationParams.allGainDacMeas.empty()){
        calibrationParams.allGainDacMeas = calibParams.allGainDacMeas;
    }

    if (!calibParams.allOffsetDacMeas.empty() && !calibrationParams.allOffsetDacMeas.empty()){
        calibrationParams.allOffsetDacMeas = calibParams.allOffsetDacMeas;
    }

    if (!calibParams.ccAllGainAdcMeas.empty() && !calibrationParams.ccAllGainAdcMeas.empty()){
        calibrationParams.ccAllGainAdcMeas = calibParams.ccAllGainAdcMeas;
    }

    if (!calibParams.ccAllOffsetAdcMeas.empty() && !calibrationParams.ccAllOffsetAdcMeas.empty()){
        calibrationParams.ccAllOffsetAdcMeas = calibParams.ccAllOffsetAdcMeas;
    }

    if (!calibParams.ccAllGainDacMeas.empty() && !calibrationParams.ccAllGainDacMeas.empty()){
        calibrationParams.ccAllGainDacMeas = calibParams.ccAllGainDacMeas;
    }

    if (!calibParams.ccAllOffsetDacMeas.empty() && !calibrationParams.ccAllOffsetDacMeas.empty()){
        calibrationParams.ccAllOffsetDacMeas = calibParams.ccAllOffsetDacMeas;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibVcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        gains[i].convertValue(calibVcCurrentGainRange.prefix);
        calibrationParams.allGainAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]] = gains[i];
    }
    this->updateCalibVcCurrentGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.allGainAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].convertValue(calibVcCurrentGainRange.prefix);
        double gain = calibrationParams.allGainAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].value;
        calibVcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibVcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        offsets[i].convertValue(calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].prefix);
        calibrationParams.allOffsetAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]] = offsets[i];
    }
    this->updateCalibVcCurrentOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.allOffsetAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].convertValue(calibVcCurrentOffsetRanges[selectedVcCurrentRangeIdx].prefix);
        double offset = calibrationParams.allOffsetAdcMeas[selectedVcCurrentRangeIdx][channelIndexes[i]].value;
        calibVcCurrentOffsetCoders[selectedVcCurrentRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibCcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        gains[i].convertValue(calibCcVoltageGainRange.prefix);
        calibrationParams.ccAllGainAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]] = gains[i];
    }
    this->updateCalibCcVoltageGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.ccAllGainAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].convertValue(calibCcVoltageGainRange.prefix);
        double gain = calibrationParams.ccAllGainAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].value;
        calibCcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibCcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        offsets[i].convertValue(calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].prefix);
        calibrationParams.ccAllOffsetAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]] = offsets[i];
    }
    this->updateCalibCcVoltageOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.ccAllOffsetAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].convertValue(calibCcVoltageOffsetRanges[selectedCcVoltageRangeIdx].prefix);
        double offset = calibrationParams.ccAllOffsetAdcMeas[selectedCcVoltageRangeIdx][channelIndexes[i]].value;
        calibCcVoltageOffsetCoders[selectedCcVoltageRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibVcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        gains[i].convertValue(calibVcVoltageGainRange.prefix);
        calibrationParams.allGainDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]] = gains[i];
    }
    this->updateCalibVcVoltageGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcVoltageGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.allGainDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].convertValue(calibVcVoltageGainRange.prefix);
        double gain = calibrationParams.allGainDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].value;
        calibVcVoltageGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibVcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        offsets[i].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        calibrationParams.allOffsetDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]] = offsets[i];
    }
    this->updateCalibVcVoltageOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibVcVoltageOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.allOffsetDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].convertValue(calibVcVoltageOffsetRanges[selectedVcVoltageRangeIdx].prefix);
        double offset = calibrationParams.allOffsetDacMeas[selectedVcVoltageRangeIdx][channelIndexes[i]].value;
        calibVcVoltageOffsetCoders[selectedVcVoltageRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag){
    if (calibCcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        gains[i].convertValue(calibCcCurrentGainRange.prefix);
        calibrationParams.ccAllGainDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]] = gains[i];
    }
    this->updateCalibCcCurrentGain(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcCurrentGainCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.ccAllGainDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].convertValue(calibCcCurrentGainRange.prefix);
        double gain = calibrationParams.ccAllGainDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].value;
        calibCcCurrentGainCoders[channelIndexes[i]]->encode(gain, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag){
    if (calibCcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        offsets[i].convertValue(calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
        calibrationParams.ccAllOffsetDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]] = offsets[i];
    }
    this->updateCalibCcCurrentOffset(channelIndexes, applyFlag);

    return Success;
}

ErrorCodes_t EmcrDevice::updateCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag){
    if (calibCcCurrentOffsetCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calibrationParams.ccAllOffsetDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].convertValue(calibCcCurrentOffsetRanges[selectedCcCurrentRangeIdx].prefix);
        double offset = calibrationParams.ccAllOffsetDacMeas[selectedCcCurrentRangeIdx][channelIndexes[i]].value;
        calibCcCurrentOffsetCoders[selectedCcCurrentRangeIdx][channelIndexes[i]]->encode(offset, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn) {
    if (vcCurrentRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (currentRangeIdx >= vcCurrentRangesNum) {
        return ErrorValueOutOfRange;
    }
    vcCurrentRangeCoder->encode(currentRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedVcCurrentRangeIdx = currentRangeIdx;
    currentRange = vcCurrentRangesArray[selectedVcCurrentRangeIdx];
    currentResolution = currentRange.step;

    this->updateCalibVcCurrentGain(allChannelIndexes, false);
    this->updateCalibVcCurrentOffset(allChannelIndexes, false);

    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn) {
    if (vcVoltageRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (voltageRangeIdx >= vcVoltageRangesNum) {
        return ErrorValueOutOfRange;
    }
    vcVoltageRangeCoder->encode(voltageRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedVcVoltageRangeIdx = voltageRangeIdx;
    voltageRange = vcVoltageRangesArray[selectedVcVoltageRangeIdx];
    voltageResolution = voltageRange.step;

    this->updateCalibVcVoltageGain(allChannelIndexes, false);
    this->updateCalibVcVoltageOffset(allChannelIndexes, false);
    this->updateVoltageHoldTuner(false);

    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    /*! Most of the times the liquid junction (aka digital offset compensation) will be performed by the same DAC that appliese the voltage sitmulus
            Voltage clamp, so by default the same range is selected for the liquid junction
            When this isnot the case the boolean variable below is set properly by the corresponding derived class of the messagedispatcher */
    if (liquidJunctionSameRangeAsVcDac) {
        this->setLiquidJunctionRange(voltageRangeIdx);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn) {
    if (ccCurrentRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (currentRangeIdx >= ccCurrentRangesNum) {
        return ErrorValueOutOfRange;
    }
    ccCurrentRangeCoder->encode(currentRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedCcCurrentRangeIdx = currentRangeIdx;
    currentRange = ccCurrentRangesArray[selectedCcCurrentRangeIdx];
    currentResolution = currentRange.step;

    this->updateCalibCcCurrentGain(allChannelIndexes, false);
    this->updateCalibCcCurrentOffset(allChannelIndexes, false);
    this->updateCurrentHoldTuner(false);

    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn) {
    if (ccVoltageRangeCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (voltageRangeIdx >= ccVoltageRangesNum) {
        return ErrorValueOutOfRange;
    }
    ccVoltageRangeCoder->encode(voltageRangeIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedCcVoltageRangeIdx = voltageRangeIdx;
    voltageRange = ccVoltageRangesArray[selectedCcVoltageRangeIdx];
    voltageResolution = voltageRange.step;

    this->updateCalibCcVoltageGain(allChannelIndexes, false);
    this->updateCalibCcVoltageOffset(allChannelIndexes, false);

    if (applyFlagIn) {
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

ErrorCodes_t EmcrDevice::setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlagIn){
    if (vcVoltageFilterCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (filterIdx >= vcVoltageFiltersNum) {
        return ErrorValueOutOfRange;
    }
    vcVoltageFilterCoder->encode(filterIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedVcVoltageFilterIdx = filterIdx;
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlagIn){
    if (ccCurrentFilterCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (filterIdx >= ccCurrentFiltersNum) {
        return ErrorValueOutOfRange;
    }
    ccCurrentFilterCoder->encode(filterIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedCcCurrentFilterIdx = filterIdx;
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (enableStimulusCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        enableStimulusCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        channelModels[channelIndexes[i]]->setInStimActive(onValues[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::turnChannelsOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (turnChannelsOnCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        turnChannelsOnCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        channelModels[channelIndexes[i]]->setOn(onValues[i]);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::turnCalSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (calSwCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        calSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::turnVcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (vcSwCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        vcSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::turnCcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (ccSwCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        ccSwCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::turnVcCcSelOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (vcCcSelCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        vcCcSelCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::enableCcStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag){
    if (ccStimEnCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        ccStimEnCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setClampingModality(uint32_t idx, bool applyFlag) {
    if (idx >= clampingModalitiesNum) {
        return ErrorValueOutOfRange;
    }
    selectedClampingModalityIdx = idx;
    selectedClampingModality = clampingModalitiesArray[selectedClampingModalityIdx];
    if (clampingModeCoder != nullptr) {
        clampingModeCoder->encode(selectedClampingModalityIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    switch (selectedClampingModality) {
    case VOLTAGE_CLAMP:
        rawDataFilterVoltageFlag = false;
        rawDataFilterCurrentFlag = true;
        break;

    case ZERO_CURRENT_CLAMP:
    case CURRENT_CLAMP:
        rawDataFilterVoltageFlag = true;
        rawDataFilterCurrentFlag = false;
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

ErrorCodes_t EmcrDevice::setClampingModality(ClampingModality_t mode, bool applyFlag) {
    auto iter = std::find(clampingModalitiesArray.begin(), clampingModalitiesArray.end(), mode);
    if (iter == clampingModalitiesArray.end()) {
        return ErrorValueOutOfRange;
    }
    return this->setClampingModality((uint32_t)(iter-clampingModalitiesArray.begin()), applyFlag);
}

ErrorCodes_t EmcrDevice::setSourceForVoltageChannel(uint16_t source, bool applyFlag){
    if (sourceForVoltageChannelCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    sourceForVoltageChannelCoder->encode(source, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedSourceForVoltageChannelIdx = source;
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setSourceForCurrentChannel(uint16_t source, bool applyFlag){
    if (sourceForCurrentChannelCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    sourceForCurrentChannelCoder->encode(source, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedSourceForCurrentChannelIdx = source;
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
    if (digitalOffsetCompensationCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        uint16_t chIdx = channelIndexes[i];
        digitalOffsetCompensationCoders[chIdx]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        channelModels[chIdx]->setCompensatingLiquidJunction(onValues[i]);
        if (onValues[i] && (liquidJunctionStates[chIdx] == LiquidJunctionIdle)) {
            liquidJunctionStates[chIdx] = LiquidJunctionStarting;

        } else if (!onValues[i]) {
            liquidJunctionStates[chIdx] = LiquidJunctionTerminate;
        }
    }

    anyLiquidJuctionActive = true;

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setAdcFilter() {
    // Still to be properly implemented
    if (selectedClampingModality == VOLTAGE_CLAMP){
        if (vcCurrentFilterCoder != nullptr) {
            vcCurrentFilterCoder->encode(sr2LpfVcCurrentMap[selectedSamplingRateIdx], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedVcCurrentFilterIdx = sr2LpfVcCurrentMap[selectedSamplingRateIdx];
        }

    } else {
        if (ccVoltageFilterCoder != nullptr) {
            ccVoltageFilterCoder->encode(sr2LpfCcVoltageMap[selectedSamplingRateIdx], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            selectedCcVoltageFilterIdx = sr2LpfCcVoltageMap[selectedSamplingRateIdx];
        }
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setSamplingRate(uint16_t samplingRateIdx, bool applyFlagIn) {
    if (samplingRateCoder == nullptr) {
        return ErrorFeatureNotImplemented;

    } else if (samplingRateIdx >= samplingRatesNum) {
        return ErrorValueOutOfRange;
    }
    samplingRateCoder->encode(samplingRateIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    selectedSamplingRateIdx = samplingRateIdx;
    samplingRate = realSamplingRatesArray[selectedSamplingRateIdx];
    integrationStep = integrationStepArray[selectedSamplingRateIdx];
    this->setAdcFilter();
    this->computeRawDataFilterCoefficients();
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t EmcrDevice::setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status) {
    BoolCoder::CoderConfig_t boolConfig;
    boolConfig.initialWord = wordOffset;
    boolConfig.initialBit = bitOffset;
    boolConfig.bitsNum = 1;
    bitDebugCoder = new BoolArrayCoder(boolConfig);
    bitDebugCoder->encode(status ? 1 : 0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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
    wordDebugCoder->encode(wordValue, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);
    delete wordDebugCoder;
    return Success;
}

ErrorCodes_t EmcrDevice::setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest) {
    if (voltageProtocolRestCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (itemsNum >= protocolMaxItemsNum || !vcVoltageRangesArray[selectedVcVoltageRangeIdx].includes(vRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;
    }
    selectedProtocolItemsNum = itemsNum;
    UnitPfx_t voltagePrefix = vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix;
    protocolIdCoder->encode(protId, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolItemsNumberCoder->encode(itemsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolSweepsNumberCoder->encode(sweepsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    vRest.convertValue(voltagePrefix);
    voltageProtocolRestCoders[selectedVcVoltageRangeIdx]->encode(vRest.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);

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
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(vHalfFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemStep, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    v0.convertValue(voltagePrefix);
    voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    v0Step.convertValue(voltagePrefix);
    voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(vHalfFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemRamp, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    v0.convertValue(voltagePrefix);
    voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    v0Step.convertValue(voltagePrefix);
    voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    vFinal.convertValue(voltagePrefix);
    voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(vFinal.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    vFinalStep.convertValue(voltagePrefix);
    voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(vFinalStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(vHalfFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemSin, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    v0.convertValue(voltagePrefix);
    voltageProtocolStim0Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    v0Step.convertValue(voltagePrefix);
    voltageProtocolStim0StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(v0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    vAmp.convertValue(voltagePrefix);
    voltageProtocolStim1Coders[selectedVcVoltageRangeIdx][itemIdx]->encode(vAmp.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    vAmpStep.convertValue(voltagePrefix);
    voltageProtocolStim1StepCoders[selectedVcVoltageRangeIdx][itemIdx]->encode(vAmpStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    f0.convertValue(freqPrefix);
    protocolFrequency0Coders[itemIdx]->encode(f0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    f0Step.convertValue(freqPrefix);
    protocolFrequency0StepCoders[itemIdx]->encode(f0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

ErrorCodes_t EmcrDevice::setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest) {
    if (currentProtocolRestCoders.empty()) {
        return ErrorFeatureNotImplemented;

    } else if (itemsNum >= protocolMaxItemsNum || !ccCurrentRangesArray[selectedCcCurrentRangeIdx].includes(iRest)) { /*! \todo FCON sommare i valori sommati con l'holder o altri meccanismi */
        return ErrorValueOutOfRange;
    }
    selectedProtocolItemsNum = itemsNum;
    UnitPfx_t currentPrefix = ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix;
    protocolIdCoder->encode(protId, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolItemsNumberCoder->encode(itemsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolSweepsNumberCoder->encode(sweepsNum, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    iRest.convertValue(currentPrefix);
    currentProtocolRestCoders[selectedCcCurrentRangeIdx]->encode(iRest.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);

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
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(cHalfFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemStep, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    i0.convertValue(currentPrefix);
    currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    i0Step.convertValue(currentPrefix);
    currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(0.0, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(cHalfFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemRamp, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    i0.convertValue(currentPrefix);
    currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    i0Step.convertValue(currentPrefix);
    currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    iFinal.convertValue(currentPrefix);
    currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(iFinal.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    iFinalStep.convertValue(currentPrefix);
    currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(iFinalStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0.convertValue(timePrefix);
    protocolTime0Coders[itemIdx]->encode(t0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    t0Step.convertValue(timePrefix);
    protocolTime0StepCoders[itemIdx]->encode(t0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
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
    protocolItemIdxCoders[itemIdx]->encode(itemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolNextItemIdxCoders[itemIdx]->encode(nextItemIdx, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolLoopRepetitionsCoders[itemIdx]->encode(loopReps, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    protocolApplyStepsCoders[itemIdx]->encode(applyStepsFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    if (!protocolStimHalfCoders.empty()) {
        protocolStimHalfCoders[itemIdx]->encode(cHalfFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }
    protocolItemTypeCoders[itemIdx]->encode(ProtocolItemSin, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    i0.convertValue(currentPrefix);
    currentProtocolStim0Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    i0Step.convertValue(currentPrefix);
    currentProtocolStim0StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(i0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    iAmp.convertValue(currentPrefix);
    currentProtocolStim1Coders[selectedCcCurrentRangeIdx][itemIdx]->encode(iAmp.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    iAmpStep.convertValue(currentPrefix);
    currentProtocolStim1StepCoders[selectedCcCurrentRangeIdx][itemIdx]->encode(iAmpStep.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    f0.convertValue(freqPrefix);
    protocolFrequency0Coders[itemIdx]->encode(f0.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    f0Step.convertValue(freqPrefix);
    protocolFrequency0StepCoders[itemIdx]->encode(f0Step.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

ErrorCodes_t EmcrDevice::setStateArrayStructure(int numberOfStates, int initialState){
    if (numberOfStatesCoder == nullptr ) {
        return ErrorFeatureNotImplemented;
    }
    if ((unsigned int)numberOfStates > stateMaxNum || initialState >= numberOfStates){
        return ErrorValueOutOfRange;
    }
    numberOfStatesCoder->encode(numberOfStates, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    initialStateCoder->encode(initialState, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

ErrorCodes_t EmcrDevice::setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, double timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag){
    if (appliedVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltage.convertValue(vHoldRange[selectedVcVoltageRangeIdx].prefix);
    appliedVoltageCoders[selectedVcVoltageRangeIdx][stateIdx]->encode(voltage.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTimeoutFlagCoders[stateIdx]->encode(timeoutStateFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTriggerFlagCoders[stateIdx]->encode(triggerFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTriggerDeltaFlagCoders[stateIdx]->encode(deltaFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTimeoutValueCoders[stateIdx]->encode(timeout, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTimeoutNextStateCoders[stateIdx]->encode(timeoutState, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    minTriggerValue.convertValue(vcCurrentRangesArray[selectedVcCurrentRangeIdx].prefix);
    maxTriggerValue.convertValue(vcCurrentRangesArray[selectedVcCurrentRangeIdx].prefix);
    stateMinTriggerCurrentCoders[selectedVcCurrentRangeIdx][stateIdx]->encode(minTriggerValue.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateMaxTriggerCurrentCoders[selectedVcCurrentRangeIdx][stateIdx]->encode(maxTriggerValue.value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stateTriggerNextStateCoders[stateIdx]->encode(triggerState, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

ErrorCodes_t EmcrDevice::setStateArrayEnabled(int chIdx, bool enabledFlag) {
    if (enableStateArrayChannelsCoder.empty()) {
        return ErrorFeatureNotImplemented;
    }
    enableStateArrayChannelsCoder[chIdx]->encode(enabledFlag, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    return Success;
}

ErrorCodes_t EmcrDevice::getNextMessage(RxOutput_t &rxOutput, int16_t * data) {
    ErrorCodes_t ret = Success;
    double xFlt;

    std::unique_lock <std::mutex> rxMutexLock (rxMsgMutex);
    if (rxMsgBufferReadLength <= 0) {
        rxMsgBufferNotEmpty.wait_for(rxMutexLock, std::chrono::milliseconds(3));
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

    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged */
    lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdInvalid;

    uint32_t dataOffset;
    uint32_t samplesNum;
    uint32_t sampleIdx;
    uint32_t timeSamplesNum;
    int16_t rawFloat;
    uint32_t dataWritten;
    bool exitLoop = false;
    bool messageReadFlag = false;

    while (msgReadCount < maxMsgRead) {
        dataOffset = rxMsgBuffer[rxMsgBufferReadOffset].startDataPtr;
        sampleIdx = 0;
        switch (rxMsgBuffer[rxMsgBufferReadOffset].typeId) {
        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid) */
                rxOutput.dataLen = 0;
                rxOutput.protocolId = rxDataBuffer[dataOffset];
                rxOutput.protocolItemIdx = rxDataBuffer[(dataOffset+1) & RX_DATA_BUFFER_MASK];
                rxOutput.protocolRepsIdx = rxDataBuffer[(dataOffset+2) & RX_DATA_BUFFER_MASK];
                rxOutput.protocolSweepIdx = rxDataBuffer[(dataOffset+3) & RX_DATA_BUFFER_MASK];

                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader;

                /*! This message cannot be merged, but stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader) {
                /*! If there are more of this kind in a sequence ignore subsequent ones */
                exitLoop = false;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData):
            samplesNum = rxMsgBuffer[rxMsgBufferReadOffset].dataLength;
            dataWritten = rxOutput.dataLen;
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid ||
                    (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData && dataWritten+samplesNum < E384CL_OUT_STRUCT_DATA_LEN)) {
                /*! process the message if it is the first message to be processed during this call (lastParsedMsgType == MsgTypeIdInvalid)
                    OR if we are merging successive acquisition data messages that do not overflow the available memory */

                std::unique_lock <std::mutex> ljMutexLock (ljMutex);
                if (downsamplingFlag) {
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    uint32_t downsamplingCount = 0;
                    for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                        if (++downsamplingOffset >= selectedDownsamplingRatio) {
                            downsamplingOffset = 0;
                            downsamplingCount++;
                            for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
#ifdef FILTER_CLIP_NEEDED
                                xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                                data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                                data[dataWritten+sampleIdx++] = (int16_t)round(this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen));
#endif
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }

                            for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
#ifdef FILTER_CLIP_NEEDED
                                xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                                data[dataWritten+sampleIdx] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                                data[dataWritten+sampleIdx] = (int16_t)round(this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen));
#endif
                                if (anyLiquidJuctionActive) {
                                    liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)data[dataWritten+sampleIdx];
                                }
                                sampleIdx++;
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }
                            liquidJunctionCurrentEstimatesNum++;

                        } else {
                            for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
                                xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }

                            for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                                rawFloat = (int16_t)rxDataBuffer[dataOffset];
                                xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                                if (anyLiquidJuctionActive) {
                                    liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)round(xFlt);
                                }
                                dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                            }
                            liquidJunctionCurrentEstimatesNum++;
                        }

                        if (iirOff < 1) {
                            iirOff = IIR_ORD;

                        } else {
                            iirOff--;
                        }
                    }

                    rxOutput.dataLen += downsamplingCount*totalChannelsNum;

                } else if (rawDataFilterActiveFlag) {
                    rxOutput.dataLen += samplesNum;
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            rawFloat = (int16_t)rxDataBuffer[dataOffset];
#ifdef FILTER_CLIP_NEEDED
                            xFlt = this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen);
                            data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                            data[dataWritten+sampleIdx++] = (int16_t)round(this->applyRawDataFilter(voltageChannelIdx, (double)rawFloat, iirVNum, iirVDen));
#endif
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            rawFloat = (int16_t)rxDataBuffer[dataOffset];
#ifdef FILTER_CLIP_NEEDED
                            xFlt = this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen);
                            data[dataWritten+sampleIdx] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
#else
                            data[dataWritten+sampleIdx] = (int16_t)round(this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (double)rawFloat, iirINum, iirIDen));
#endif
                            if (anyLiquidJuctionActive) {
                                liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)data[dataWritten+sampleIdx];
                            }
                            sampleIdx++;
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }
                        liquidJunctionCurrentEstimatesNum++;

                        if (iirOff < 1) {
                            iirOff = IIR_ORD;

                        } else {
                            iirOff--;
                        }
                    }

                } else {
                    rxOutput.dataLen += samplesNum;
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    for (uint32_t idx = 0; idx < timeSamplesNum; idx++) {
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            data[dataWritten+sampleIdx++] = rxDataBuffer[dataOffset];
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            data[dataWritten+sampleIdx] = rxDataBuffer[dataOffset];
                            if (anyLiquidJuctionActive) {
                                liquidJunctionCurrentSums[currentChannelIdx] += (int64_t)data[dataWritten+sampleIdx];
                            }
                            sampleIdx++;
                            dataOffset = (dataOffset+1) & RX_DATA_BUFFER_MASK;
                        }
                        if (anyLiquidJuctionActive) {
                            liquidJunctionCurrentEstimatesNum++;
                        }
                    }
                }

                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData;

                /*! This message can be merged, stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }
            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                /*! Evaluate only if it's the first repetition */
                rxOutput.dataLen = 0;
                rxOutput.protocolId = rxDataBuffer[dataOffset];

                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail;

                /*! This message cannot be merged, but stay in the loop in case there are more to read */
                exitLoop = false;
                messageReadFlag = true;

            } else if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail) {
                /*! If there are more of this kind in a sequence ignore subsequent ones */
                exitLoop = false;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }

            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation;

                /*! This message cannot be merged, leave anyway */
                exitLoop = true;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }

            break;

        case (MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus):
            //            not really managed, ignore it
            if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus;
                /*! This message cannot be merged, leave anyway */
                exitLoop = true;
                messageReadFlag = true;

            } else {
                /*! Exit the loop in case this message type is different from the previous one */
                exitLoop = true;
                messageReadFlag = false;
            }

            break;

        default:
            lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdInvalid;

            /*! Leave and look for following messages */
            exitLoop = true;
            messageReadFlag = true;
            break;
        }
        rxOutput.msgTypeId = lastParsedMsgType;
        if (messageReadFlag) {
            msgReadCount++;
            rxMsgBufferReadOffset = (rxMsgBufferReadOffset+1) & RX_MSG_BUFFER_MASK;
        }

        if (exitLoop) {
            break;
        }
    }

    rxMutexLock.lock();
    rxMsgBufferReadLength -= msgReadCount;
    rxMutexLock.unlock();
    rxMsgBufferNotFull.notify_all();

    return ret;
}

ErrorCodes_t EmcrDevice::purgeData() {
    ErrorCodes_t ret = Success;

    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    rxMsgBufferReadOffset = (rxMsgBufferReadOffset+rxMsgBufferReadLength) & RX_MSG_BUFFER_MASK;
    rxMsgBufferReadLength = 0;
    rxMutexLock.unlock();
    rxMsgBufferNotFull.notify_all();

    return ret;
}

ErrorCodes_t EmcrDevice::getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures){
    if (vHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageHoldTunerFeatures = vHoldRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTunerFeatures){
    if (vHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    voltageHalfTunerFeatures = vHalfRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures){
    if (cHoldTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentHoldTunerFeatures = cHoldRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTunerFeatures){
    if (cHalfTunerCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    currentHalfTunerFeatures = cHalfRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges) {
    if (liquidJunctionVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    ranges = liquidJunctionRangesArray;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibVcCurrentGainFeatures(RangedMeasurement_t &calibVcCurrentGainFeatures){
    if (calibVcCurrentGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    }
    calibVcCurrentGainFeatures = calibVcCurrentGainRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibVcCurrentOffsetFeatures(std::vector<RangedMeasurement_t> &calibVcCurrentOffsetFeatures){
    if (calibVcCurrentOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    }
    calibVcCurrentOffsetFeatures = calibVcCurrentOffsetRanges;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibCcVoltageGainFeatures(RangedMeasurement_t &calibCcVoltageGainFeatures){
    if (calibCcVoltageGainCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    }
    calibCcVoltageGainFeatures = calibCcVoltageGainRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibCcVoltageOffsetFeatures(std::vector<RangedMeasurement_t> &calibCcVoltageOffsetFeatures){
    if (calibCcVoltageOffsetCoders.size() == 0) {
        return ErrorFeatureNotImplemented;
    }
    calibCcVoltageOffsetFeatures = calibCcVoltageOffsetRanges;
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

ErrorCodes_t EmcrDevice::getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures){
    if (gateVoltageCoders.empty()) {
        return ErrorFeatureNotImplemented;
    }
    gateVoltagesFeatures = gateVoltageRange;
    return Success;
}

ErrorCodes_t EmcrDevice::getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures){
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

ErrorCodes_t EmcrDevice::isStateArrayAvailable() {
    if (numberOfStatesCoder == nullptr) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibData(CalibrationData_t &calibData){
    if(calibrationData.vcCalibResArray.empty()){
        return ErrorFeatureNotImplemented;
    }
    calibData = calibrationData;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibParams(CalibrationParams_t &calibParams) {
    if (!calibrationParams.allGainAdcMeas.empty()) {
        calibParams.allGainAdcMeas = calibrationParams.allGainAdcMeas;
    }

    if (!calibrationParams.allOffsetAdcMeas.empty()) {
        calibParams.allOffsetAdcMeas = calibrationParams.allOffsetAdcMeas;
    }

    if (!calibrationParams.allGainDacMeas.empty()) {
        calibParams.allGainDacMeas = calibrationParams.allGainDacMeas;
    }

    if (!calibrationParams.allOffsetDacMeas.empty()) {
        calibParams.allOffsetDacMeas = calibrationParams.allOffsetDacMeas;
    }

    if (!calibrationParams.ccAllGainAdcMeas.empty()) {
        calibParams.ccAllGainAdcMeas = calibrationParams.ccAllGainAdcMeas;
    }

    if (!calibrationParams.ccAllOffsetAdcMeas.empty()) {
        calibParams.ccAllOffsetAdcMeas = calibrationParams.ccAllOffsetAdcMeas;
    }

    if (!calibrationParams.ccAllGainDacMeas.empty()) {
        calibParams.ccAllGainDacMeas = calibrationParams.ccAllGainDacMeas;
    }

    if (!calibrationParams.ccAllOffsetDacMeas.empty()) {
        calibParams.ccAllOffsetDacMeas = calibrationParams.ccAllOffsetDacMeas;
    }
    return calibrationLoadingError;
}

ErrorCodes_t EmcrDevice::getCalibFileNames(std::vector<std::string> &calibFileNames){
    calibFileNames = calibrationFileNames;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibFilesFlags(std::vector<std::vector <bool>> &calibFilesFlags) {
    calibFilesFlags = calibrationFilesOkFlags;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibMappingFileDir(std::string &dir){
    dir = calibrationMappingFileDir;
    return Success;
}

ErrorCodes_t EmcrDevice::getCalibMappingFilePath(std::string &path){
    path = calibrationMappingFilePath;
    return Success;
}

ErrorCodes_t EmcrDevice::init() {
    rxMsgBuffer = new (std::nothrow) MsgResume_t[RX_MSG_BUFFER_SIZE];
    if (rxMsgBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    rxDataBuffer = new (std::nothrow) uint16_t[RX_DATA_BUFFER_SIZE+1]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word */
    if (rxDataBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    txMsgBuffer = new (std::nothrow) std::vector <uint16_t>[TX_MSG_BUFFER_SIZE];
    if (txMsgBuffer == nullptr) {
        return ErrorMemoryInitialization;
    }

    txMsgOffsetWord.resize(TX_MSG_BUFFER_SIZE);
    txMsgLength.resize(TX_MSG_BUFFER_SIZE);
    txMsgTrigger.resize(TX_MSG_BUFFER_SIZE);

    /*! Allocate memory for raw data filters */
    this->initializeRawDataFilterVariables();

    /*! Allocate memory for voltage values for devices that send only data current in standard data frames */
    voltageDataValues.resize(voltageChannelsNum);
    std::fill(voltageDataValues.begin(), voltageDataValues.end(), 0);

    return Success;
}

ErrorCodes_t EmcrDevice::deinit() {
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

#ifdef DEBUG_TX_DATA_PRINT
    fclose(txFid);
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
    fclose(rxRawFid);
#endif

#ifdef DEBUG_RX_PROCESSING_PRINT
    fclose(rxProcFid);
#endif

#ifdef DEBUG_RX_DATA_PRINT
    fclose(rxFid);
#endif

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
    fclose(ljFid);
#endif

    return Success;
}

ErrorCodes_t EmcrDevice::initializeCalibration() {
    CalibrationManager calibrationManager(deviceId, currentChannelsNum, totalBoardsNum, vcCurrentRangesNum, vcVoltageRangesNum, ccVoltageRangesNum, ccCurrentRangesNum);

    calibrationParams = calibrationManager.getCalibrationParams(calibrationLoadingError);
    calibrationFileNames = calibrationManager.getCalibrationFileNames();
    calibrationFilesOkFlags = calibrationManager.getCalibrationFilesOkFlags();
    calibrationMappingFileDir = calibrationManager.getMappingFileDir();
    calibrationMappingFilePath = calibrationManager.getMappingFilePath();
}

void EmcrDevice::forceOutMessage() {
    txModifiedStartingWord = 0;
    txModifiedEndingWord = txDataWords;
}

void EmcrDevice::updateVoltageHoldTuner(bool applyFlag) {
    if (vHoldTunerCoders.empty()) {
        return;

    } else {
        for (uint32_t i = 0; i < currentChannelsNum; i++) {
            vHoldTunerCoders[selectedVcVoltageRangeIdx][i]->encode(selectedVoltageHoldVector[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
    }
}

void EmcrDevice::updateCurrentHoldTuner(bool applyFlag) {
    if (cHoldTunerCoders.empty()) {
        return;

    } else {
        for (uint32_t i = 0; i < voltageChannelsNum; i++) {
            cHoldTunerCoders[selectedCcCurrentRangeIdx][i]->encode(selectedCurrentHoldVector[i].value, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        }

        if (applyFlag) {
            this->stackOutgoingMessage(txStatus);
        }
    }
}

void EmcrDevice::storeFrameData(uint16_t rxMsgTypeId, RxMessageTypes_t rxMessageType) {
    uint32_t rxDataWords = rxWordLengths[rxMessageType];

#ifdef DEBUG_RX_PROCESSING_PRINT
    fprintf(rxProcFid, "Store data frame: %x\n", rxMessageType);
    fflush(rxProcFid);
#endif

    rxMsgBuffer[rxMsgBufferWriteOffset].typeId = rxMsgTypeId;
    rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = rxDataWords;
    rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;

    if (rxMessageType == RxMessageCurrentDataLoad) {
        /*! Data frame with only current */
        uint32_t packetsNum = rxDataWords/currentChannelsNum;
        uint32_t rxDataBufferWriteIdx = 0;

#ifdef DEBUG_RX_PROCESSING_PRINT
        fprintf(rxProcFid, "rxDataWords: %d\n", rxDataWords);
        fprintf(rxProcFid, "packetsNum: %d\n", packetsNum);
        fflush(rxProcFid);
#endif

        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! For each packet retrieve the last recevied voltage values */
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++) & RX_DATA_BUFFER_MASK] = voltageDataValues[idx];
            }

            /*! The store the new current values */
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx++) & RX_DATA_BUFFER_MASK] = this->popUint16FromRxRawBuffer();
            }
        }

        /* The size of the data returned by the message dispatcher is different from the size of the fram returned by the FPGA */
        rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = rxDataBufferWriteIdx;

    } else if (rxMessageType == RxMessageVoltageDataLoad) {
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            voltageDataValues[idx] = this->popUint16FromRxRawBuffer();
        }

    } else {
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx) & RX_DATA_BUFFER_MASK] = this->popUint16FromRxRawBuffer();
        }
    }

    rxDataBufferWriteOffset = (rxDataBufferWriteOffset+rxMsgBuffer[rxMsgBufferWriteOffset].dataLength) & RX_DATA_BUFFER_MASK;

    if (rxDataBufferWriteOffset <= rxMsgBuffer[rxMsgBufferWriteOffset].dataLength) {
        rxDataBuffer[RX_DATA_BUFFER_SIZE] = rxDataBuffer[0]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word,
                                                              *   even if the first 16bit word is in position FTD_RX_DATA_BUFFER_SIZE-1 and the following one would go out of range otherwise */
    }

    if (rxEnabledTypesMap[rxMsgTypeId]) {
        /*! Update the message buffer only if the message is not filtered out */
        rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1) & RX_MSG_BUFFER_MASK;
        /*! change the message buffer length */
        std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
        rxMsgBufferReadLength++;
        rxMutexLock.unlock();
        rxMsgBufferNotEmpty.notify_all();
    }
}

void EmcrDevice::stackOutgoingMessage(std::vector <uint16_t> &txDataMessage, TxTriggerType_t triggerType) {
    if (txModifiedEndingWord > txModifiedStartingWord) {
        std::unique_lock <std::mutex> txMutexLock(txMutex);
        while (txMsgBufferReadLength >= TX_MSG_BUFFER_SIZE) {
            txMsgBufferNotFull.wait_for(txMutexLock, std::chrono::milliseconds(100));
        }

        /*! The next 2 lines ensure that words are written in blocks of 32 bits in case any device libraries require it */
        txModifiedStartingWord = (txModifiedStartingWord/2)*2; /*! Round to the biggest smaller even number */
        txModifiedEndingWord = ((txModifiedEndingWord+1)/2)*2; /*! +1 : if odd, makes the number even and /2 followed by *2 doesn't change a thing. If even, makes the number odd and /2 truncates to the smaller integer and *2 drives you to the correct ending word. */

        txMsgBuffer[txMsgBufferWriteOffset] = {txDataMessage.begin()+txModifiedStartingWord, txDataMessage.begin()+txModifiedEndingWord};
        txMsgOffsetWord[txMsgBufferWriteOffset] = txModifiedStartingWord;
        txMsgLength[txMsgBufferWriteOffset] = txModifiedEndingWord-txModifiedStartingWord;
        txMsgTrigger[txMsgBufferWriteOffset] = triggerType;

        txModifiedStartingWord = txDataWords;
        txModifiedEndingWord = 0;

        txMsgBufferWriteOffset = (txMsgBufferWriteOffset+1)&TX_MSG_BUFFER_MASK;
        txMsgBufferReadLength++;

        txMutexLock.unlock();
        txMsgBufferNotEmpty.notify_all();
    }
}

uint16_t EmcrDevice::popUint16FromRxRawBuffer() {
    uint16_t value = (rxRawBuffer[rxRawBufferReadOffset] << 8) + rxRawBuffer[rxRawBufferReadOffset+1];
    rxRawBufferReadOffset = (rxRawBufferReadOffset+RX_WORD_SIZE) & rxRawBufferMask;
    rxRawBytesAvailable -= RX_WORD_SIZE;
    return value;
}

uint16_t EmcrDevice::readUint16FromRxRawBuffer(uint32_t n) {
    uint16_t value = (rxRawBuffer[(rxRawBufferReadOffset+n) & rxRawBufferMask] << 8) + rxRawBuffer[(rxRawBufferReadOffset+n+1) & rxRawBufferMask];
    return value;
}
