#include "ezpatchdevice.h"

#include <map>

#include "utils.h"

/*****************\
 *  Ctor / Dtor  *
\*****************/

EZPatchDevice::EZPatchDevice(std::string deviceId) :
    MessageDispatcher(deviceId) {

    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAck] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdNack] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdPing] = true;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdFpgaReset] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdLiquidJunctionComp] = true;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail] = true;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation] = false;

    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAck] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdNack] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdPing] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdFpgaReset] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdLiquidJunctionComp] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataOverflow] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation] = true;

    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdAck]  = false;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdNack] = false;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdPing] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdAbort] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdSamplingRate] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdFpgaReset] = false;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdVoltageProtocolStruct] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdVoltageStepTimeStep] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdVoltageRamp] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdVoltageSin] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdStartProtocol] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdCurrentProtocolStruct] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdCurrentStepTimeStep] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdCurrentRamp] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdCurrentSin] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdLiquidJunctionComp] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdLiquidJunctionCompInquiry] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdZap] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdDigitalTriggerOutput] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdInvalid] = false;

    pipetteCompensationOptions.clear();
    ccPipetteCompensationOptions.clear();
    membraneCompensationOptions.clear();
    resistanceCompensationOptions.clear();
    resistanceCorrectionOptions.clear();
    resistancePredictionOptions.clear();
    leakConductanceCompensationOptions.clear();
    bridgeBalanceCompensationOptions.clear();

    /*! All EZ Patch devices were designed this way */
    properHeaderPackets = true;
    canDoEpisodic = true;
}

EZPatchDevice::~EZPatchDevice() {

}

ErrorCodes_t EZPatchDevice::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    uint16_t uType = (uint16_t)messageType;

    if (uType < MsgDirectionDeviceToPc) {
        /*! This method controls only messages going from the device to the SW */
        uType += MsgDirectionDeviceToPc;
    }

    rxEnabledTypesMap[uType] = flag;

    return Success;
}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t EZPatchDevice::ack(uint16_t heartbeat) {
    ErrorCodes_t ret;
    uint16_t dataLength = 1;
    std::vector <uint16_t> txDataMessage(dataLength);
    txDataMessage[0] = heartbeat;

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdAck, txDataMessage, dataLength);
    return ret;
}

ErrorCodes_t EZPatchDevice::nack(uint16_t heartbeat) {
    ErrorCodes_t ret;
    uint16_t dataLength = 1;
    std::vector <uint16_t> txDataMessage(dataLength);
    txDataMessage[0] = heartbeat;

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdNack, txDataMessage, dataLength);
    return ret;
}

ErrorCodes_t EZPatchDevice::ping() {
    ErrorCodes_t ret;
    uint16_t dataLength = 0;
    std::vector <uint16_t> txDataMessage(dataLength);

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdPing, txDataMessage, dataLength);
    if (ret != Success) {
        ret = ErrorConnectionPingFailed;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::resetAsic(bool resetFlag, bool applyFlag) {
    /*! \todo FCON qui bisogna eventualmente settare resetFlag, se mai sarà operativo quel genere di controllo per questi device */
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    unsigned int resetIdx = ResetIndexChip;

    if (resetFlag) {
        txDataMessage[resetWord[resetIdx]] |= resetByte[resetIdx];

    } else {
        txDataMessage[resetWord[resetIdx]] &= ~resetByte[resetIdx];
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::resetFpga(bool resetFlag, bool applyFlag) {
    if (resetFlag) {
        return this->resetFpga();
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::resetFpga() {
    ErrorCodes_t ret;
    uint16_t dataLength = 0;
    std::vector <uint16_t> txDataMessage(dataLength);
    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdFpgaReset, txDataMessage, dataLength);
    voltageOffsetCorrected = 0.0;
    this->selectVoltageOffsetResolution();
    return ret;
}

ErrorCodes_t EZPatchDevice::setLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) {
    ErrorCodes_t ret = Success;
    for (unsigned int idx = 0; idx < channelIndexes.size(); idx++) {
        if (ret == Success) {
            ret = this->setLiquidJunctionVoltage(channelIndexes[idx], voltages[idx]);
        }
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setLiquidJunctionVoltage(uint16_t channelIdx, Measurement_t voltage) {
    if (!voltageHoldTunerImplemented) {
        return ErrorFeatureNotImplemented;
    }

    if (channelIdx >= currentChannelsNum) {
        return ErrorValueOutOfRange;
    }

    selectedLiquidJunctionVector[channelIdx] = voltage;
    if (this->updateLiquidJunctionVoltage(channelIdx, true) == Success) {
        channelModels[channelIdx]->setLiquidJunctionVoltage(selectedLiquidJunctionVector[channelIdx]); /*! \todo FCON Qui andrebbe controllato chi fallisce */
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::updateLiquidJunctionVoltage(uint16_t channelIdx, bool applyFlag) {
    if (channelIdx >= currentChannelsNum) {
        return ErrorValueOutOfRange;
    }

    /*! \todo FCON, ogni canale inviato da solo, si potrebbe fare di meglio */
    ErrorCodes_t ret;
    selectedLiquidJunctionVector[channelIdx].convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);

    uint16_t dataLength = 4;
    std::vector <uint16_t> txDataMessage(dataLength);
    if (channelModels[channelIdx]->isCompensatingLiquidJunction()) {
        this->int322uint16((int32_t)round((selectedLiquidJunctionVector[channelIdx].value)/voltageCommandResolution), txDataMessage, 0);

    } else {
        this->int322uint16((int32_t)round((selectedVoltageHoldVector[channelIdx].value+selectedLiquidJunctionVector[channelIdx].value)/voltageCommandResolution), txDataMessage, 0);
    }
    txDataMessage[3] = txDataMessage[1];
    txDataMessage[1] = txDataMessage[0];
    txDataMessage[0] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
    txDataMessage[2] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

    return ret;
}

ErrorCodes_t EZPatchDevice::setVoltageHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool) {
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (!allInRange(voltages, vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMin(), vcVoltageRangesArray[selectedVcVoltageRangeIdx].getMax())) {
        return ErrorValueOutOfRange;
    }
    std::map <uint16_t, Measurement_t> m;
    for (size_t i = 0; i < channelIndexes.size(); ++i) {
        m[channelIndexes[i]] = voltages[i];
    }

    uint16_t dataLength = 4*channelIndexes.size();
    std::vector <uint16_t> txDataMessage(dataLength);
    std::vector <uint16_t> uint16Value(2);
    for (auto e : m) {
        uint16_t chIdx = e.first;
        Measurement_t voltage = e.second;

        selectedVoltageHoldVector[chIdx] = voltage;
        selectedVoltageHoldVector[chIdx].convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);

        if (channelModels[chIdx]->isCompensatingLiquidJunction()) {
            this->int322uint16((int32_t)round((selectedLiquidJunctionVector[chIdx].value)/voltageCommandResolution), uint16Value, 0);

        } else {
            this->int322uint16((int32_t)round((selectedVoltageHoldVector[chIdx].value+selectedLiquidJunctionVector[chIdx].value)/voltageCommandResolution), uint16Value, 0);
        }
        txDataMessage[0+chIdx*4] = vcHoldTunerHwRegisterOffset+chIdx*coreSpecificRegistersNum;
        txDataMessage[1+chIdx*4] = uint16Value[0];
        txDataMessage[2+chIdx*4] = vcHoldTunerHwRegisterOffset+chIdx*coreSpecificRegistersNum+1;
        txDataMessage[3+chIdx*4] = uint16Value[1];
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

    if (ret != Success) {
        return ret;
    }
    for (auto e : m) {
        uint16_t chIdx = e.first;
        voltageTunerCorrection[chIdx] = selectedVoltageHoldVector[chIdx].value;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::setCurrentHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool) {
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (!allInRange(currents, ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMin(), ccCurrentRangesArray[selectedCcCurrentRangeIdx].getMax())) {
        return ErrorValueOutOfRange;
    }
    std::map <uint16_t, Measurement_t> m;
    for (size_t i = 0; i < channelIndexes.size(); ++i) {
        m[channelIndexes[i]] = currents[i];
    }

    uint16_t dataLength = 4*channelIndexes.size();
    std::vector <uint16_t> txDataMessage(dataLength);
    std::vector <uint16_t> uint16Value(2);
    for (auto e : m) {
        uint16_t chIdx = e.first;
        Measurement_t current = e.second;

        selectedCurrentHoldVector[chIdx] = current;
        selectedCurrentHoldVector[chIdx].convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
        this->int322uint16((int32_t)round(selectedCurrentHoldVector[chIdx].value/currentCommandResolution), uint16Value, 0);
        txDataMessage[0+chIdx*4] = ccHoldTunerHwRegisterOffset+chIdx*coreSpecificRegistersNum;
        txDataMessage[1+chIdx*4] = uint16Value[0];
        txDataMessage[2+chIdx*4] = ccHoldTunerHwRegisterOffset+chIdx*coreSpecificRegistersNum+1;
        txDataMessage[3+chIdx*4] = uint16Value[1];
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

    if (ret != Success) {
        return ret;
    }
    for (auto e : m) {
        uint16_t chIdx = e.first;
        currentTunerCorrection[chIdx] = selectedCurrentHoldVector[chIdx].value;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::turnChannelsOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    ErrorCodes_t ret;

    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (inputSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    for (unsigned int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
        unsigned int ch = channelIndexes[channelIdx];
        for (unsigned int switchIdx = 0; switchIdx < inputSwitchesNum; switchIdx++) {
            if (inputSwitchesLut[switchIdx] == onValues[channelIdx]) {
                txDataMessage[inputSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] |=
                        inputSwitchesByte[switchIdx]; // 1

            } else {
                txDataMessage[inputSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] &=
                        ~inputSwitchesByte[switchIdx]; // 0
            }
        }
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnCalSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (calSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    for (unsigned int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
        unsigned int ch = channelIndexes[channelIdx];
        for (unsigned int switchIdx = 0; switchIdx < calSwitchesNum; switchIdx++) {
            if (calSwitchesLut[switchIdx] == onValues[channelIdx]) {
                txDataMessage[calSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] |=
                        calSwitchesByte[switchIdx]; // 1

            } else {
                txDataMessage[calSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] &=
                        ~calSwitchesByte[switchIdx]; // 0
            }
        }
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnVcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (vcSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    for (unsigned int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
        unsigned int ch = channelIndexes[channelIdx];
        for (unsigned int switchIdx = 0; switchIdx < vcSwitchesNum; switchIdx++) {
            if (vcSwitchesLut[switchIdx] == onValues[channelIdx]) {
                txDataMessage[vcSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] |=
                        vcSwitchesByte[switchIdx]; // 1

            } else {
                txDataMessage[vcSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] &=
                        ~vcSwitchesByte[switchIdx]; // 0
            }
        }
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnCcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (ccSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    for (unsigned int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
        unsigned int ch = channelIndexes[channelIdx];
        for (unsigned int switchIdx = 0; switchIdx < ccSwitchesNum; switchIdx++) {
            if (ccSwitchesLut[switchIdx] == onValues[channelIdx]) {
                txDataMessage[ccSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] |=
                        ccSwitchesByte[switchIdx]; // 1

            } else {
                txDataMessage[ccSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] &=
                        ~ccSwitchesByte[switchIdx]; // 0
            }
        }
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setAdcCore(std::vector <uint16_t> channelIndexes, std::vector <ClampingModality_t> clampingModes, bool applyFlag) {
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (adcCoreSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);
    bool ctrl;

    if (adcCoreSwitchChannelIndependent) {
        for (unsigned int channelIdx = 0; channelIdx < channelIndexes.size(); channelIdx++) {
            unsigned int ch = channelIndexes[channelIdx];
            switch (clampingModes[ch]) {
            case VOLTAGE_CLAMP:
                ctrl = true;
                break;

            case ZERO_CURRENT_CLAMP:
                ctrl = false;
                break;

            case CURRENT_CLAMP:
                ctrl = false;
                break;

            case DYNAMIC_CLAMP:
                return ErrorFeatureNotImplemented;

            case UNDEFINED_CLAMP:
                return ErrorFeatureNotImplemented;
            }
            for (unsigned int switchIdx = 0; switchIdx < adcCoreSwitchesNum; switchIdx++) {
                if (adcCoreSwitchesLut[switchIdx] == ctrl) {
                    txDataMessage[adcCoreSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] |=
                            adcCoreSwitchesByte[switchIdx]; // 1

                } else {
                    txDataMessage[adcCoreSwitchesWord[switchIdx]+ch*coreSpecificSwitchesWordsNum] &=
                            ~adcCoreSwitchesByte[switchIdx]; // 0
                }
            }
        }

    } else {
        unsigned int ch = channelIndexes[0]; // Use the first item for all channels
        switch (clampingModes[ch]) {
        case VOLTAGE_CLAMP:
            ctrl = true;
            break;

        case ZERO_CURRENT_CLAMP:
            ctrl = false;
            break;

        case CURRENT_CLAMP:
            ctrl = false;
            break;

        case DYNAMIC_CLAMP:
            return ErrorFeatureNotImplemented;

        case UNDEFINED_CLAMP:
            return ErrorFeatureNotImplemented;
        }
        for (unsigned int switchIdx = 0; switchIdx < adcCoreSwitchesNum; switchIdx++) {
            if (adcCoreSwitchesLut[switchIdx] == ctrl) {
                txDataMessage[adcCoreSwitchesWord[switchIdx]] |=
                        adcCoreSwitchesByte[switchIdx]; // 1

            } else {
                txDataMessage[adcCoreSwitchesWord[switchIdx]] &=
                        ~adcCoreSwitchesByte[switchIdx]; // 0
            }
        }
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnVoltageStimulusOn(bool on, bool applyFlag) {
    if (vcStimulusSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    if (vcStimulusSwitchChannelIndependent) {
        for (unsigned int channelIdx = 0; channelIdx < voltageChannelsNum; channelIdx++) {
            for (unsigned int stimulusSwitchIdx = 0; stimulusSwitchIdx < vcStimulusSwitchesNum; stimulusSwitchIdx++) {
                if (vcStimulusSwitchesLut[stimulusSwitchIdx] == on) {
                    txDataMessage[vcStimulusSwitchesWord[stimulusSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] |=
                            vcStimulusSwitchesByte[stimulusSwitchIdx]; // 1

                } else {
                    txDataMessage[vcStimulusSwitchesWord[stimulusSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] &=
                            ~vcStimulusSwitchesByte[stimulusSwitchIdx]; // 0
                }
            }
        }

    } else {
        for (unsigned int stimulusSwitchIdx = 0; stimulusSwitchIdx < vcStimulusSwitchesNum; stimulusSwitchIdx++) {
            if (vcStimulusSwitchesLut[stimulusSwitchIdx] == on) {
                txDataMessage[vcStimulusSwitchesWord[stimulusSwitchIdx]] |=
                        vcStimulusSwitchesByte[stimulusSwitchIdx]; // 1

            } else {
                txDataMessage[vcStimulusSwitchesWord[stimulusSwitchIdx]] &=
                        ~vcStimulusSwitchesByte[stimulusSwitchIdx]; // 0
            }
        }
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnCurrentStimulusOn(bool on, bool applyFlag) {
    ErrorCodes_t ret;

    if (ccStimulusSwitchesNum > 0) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (ccStimulusSwitchChannelIndependent) {
            for (unsigned int channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
                for (unsigned int stimulusSwitchIdx = 0; stimulusSwitchIdx < ccStimulusSwitchesNum; stimulusSwitchIdx++) {
                    if (ccStimulusSwitchesLut[stimulusSwitchIdx] == on) {
                        txDataMessage[ccStimulusSwitchesWord[stimulusSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] |=
                                ccStimulusSwitchesByte[stimulusSwitchIdx]; // 1

                    } else {
                        txDataMessage[ccStimulusSwitchesWord[stimulusSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] &=
                                ~ccStimulusSwitchesByte[stimulusSwitchIdx]; // 0
                    }
                }
            }

        } else {
            for (unsigned int stimulusSwitchIdx = 0; stimulusSwitchIdx < ccStimulusSwitchesNum; stimulusSwitchIdx++) {
                if (ccStimulusSwitchesLut[stimulusSwitchIdx] == on) {
                    txDataMessage[ccStimulusSwitchesWord[stimulusSwitchIdx]] |=
                            ccStimulusSwitchesByte[stimulusSwitchIdx]; // 1

                } else {
                    txDataMessage[ccStimulusSwitchesWord[stimulusSwitchIdx]] &=
                            ~ccStimulusSwitchesByte[stimulusSwitchIdx]; // 0
                }
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnVoltageReaderOn(bool on, bool applyFlag) {
    ErrorCodes_t ret;

    /*! Voltage is read by current clamp ADC */
    if (ccReaderSwitchesNum > 0) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (ccReaderSwitchChannelIndependent) {
            for (unsigned int channelIdx = 0; channelIdx < voltageChannelsNum; channelIdx++) {
                for (unsigned int readerSwitchIdx = 0; readerSwitchIdx < ccReaderSwitchesNum; readerSwitchIdx++) {
                    if (ccReaderSwitchesLut[readerSwitchIdx] == on) {
                        txDataMessage[ccReaderSwitchesWord[readerSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] |=
                                ccReaderSwitchesByte[readerSwitchIdx]; // 1

                    } else {
                        txDataMessage[ccReaderSwitchesWord[readerSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] &=
                                ~ccReaderSwitchesByte[readerSwitchIdx]; // 0
                    }
                }
            }

        } else {
            for (unsigned int readerSwitchIdx = 0; readerSwitchIdx < ccReaderSwitchesNum; readerSwitchIdx++) {
                if (ccReaderSwitchesLut[readerSwitchIdx] == on) {
                    txDataMessage[ccReaderSwitchesWord[readerSwitchIdx]] |=
                            ccReaderSwitchesByte[readerSwitchIdx]; // 1

                } else {
                    txDataMessage[ccReaderSwitchesWord[readerSwitchIdx]] &=
                            ~ccReaderSwitchesByte[readerSwitchIdx]; // 0
                }
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnCurrentReaderOn(bool on, bool applyFlag) {
    ErrorCodes_t ret;

    /*! Current is read by voltage clamp ADC */
    if (vcReaderSwitchesNum > 0) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (vcReaderSwitchChannelIndependent) {
            for (unsigned int channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
                for (unsigned int readerSwitchIdx = 0; readerSwitchIdx < vcReaderSwitchesNum; readerSwitchIdx++) {
                    if (vcReaderSwitchesLut[readerSwitchIdx] == on) {
                        txDataMessage[vcReaderSwitchesWord[readerSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] |=
                                vcReaderSwitchesByte[readerSwitchIdx]; // 1

                    } else {
                        txDataMessage[vcReaderSwitchesWord[readerSwitchIdx]+channelIdx*coreSpecificSwitchesWordsNum] &=
                                ~vcReaderSwitchesByte[readerSwitchIdx]; // 0
                    }
                }
            }

        } else {
            for (unsigned int readerSwitchIdx = 0; readerSwitchIdx < vcReaderSwitchesNum; readerSwitchIdx++) {
                if (vcReaderSwitchesLut[readerSwitchIdx] == on) {
                    txDataMessage[vcReaderSwitchesWord[readerSwitchIdx]] |=
                            vcReaderSwitchesByte[readerSwitchIdx]; // 1

                } else {
                    txDataMessage[vcReaderSwitchesWord[readerSwitchIdx]] &=
                            ~vcReaderSwitchesByte[readerSwitchIdx]; // 0
                }
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setClampingModality(uint32_t idx, bool applyFlag, bool stopProtocolFlag) {
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

    switch (selectedClampingModality) {
    case VOLTAGE_CLAMP:
        rawDataFilterVoltageFlag = false;
        rawDataFilterCurrentFlag = true;

        /*! Restore liquid junction and remove it from the voltage reading */
        for (uint32_t i = 0; i < currentChannelsNum; i++) {
            this->updateLiquidJunctionVoltage(i, false);
            ccLiquidJunctionVector[i] = 0;
        }

        this->enableCcCompensations(false, false);
        this->turnCurrentReaderOn(true, false);
        this->turnVoltageStimulusOn(true, true);
        /*! Apply on previous command to turn the voltage clamp on first */
        this->turnCurrentStimulusOn(false, false);
        this->turnVoltageReaderOn(false, false);
        if ((previousClampingModality == CURRENT_CLAMP) ||
                (previousClampingModality == ZERO_CURRENT_CLAMP)) {
            this->setVCCurrentRange(storedVcCurrentRangeIdx, false);
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
            selectedLiquidJunctionVector[i].convertValue(ccVoltageRangesArray[selectedCcVoltageRangeIdx].prefix);
            ccLiquidJunctionVector[i] = (int16_t)(selectedLiquidJunctionVector[i].value/ccVoltageRangesArray[selectedCcVoltageRangeIdx].step);
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
        this->setCCVoltageRange(selectedCcVoltageRangeIdx, false);
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
            selectedLiquidJunctionVector[i].convertValue(ccVoltageRangesArray[selectedCcVoltageRangeIdx].prefix);
            ccLiquidJunctionVector[i] = (int16_t)(selectedLiquidJunctionVector[i].value/ccVoltageRangesArray[selectedCcVoltageRangeIdx].step);
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
        this->setCCVoltageRange(selectedCcVoltageRangeIdx, false);
        this->enableCcCompensations(true, false);

        this->setSourceForVoltageChannel(1, false);
        this->setSourceForCurrentChannel(1, false);

        break;

    case DYNAMIC_CLAMP:
        rawDataFilterVoltageFlag = false;
        rawDataFilterCurrentFlag = false;
        break;
    }

    return Success;
}

ErrorCodes_t EZPatchDevice::setSourceForVoltageChannel(uint16_t source, bool applyFlag) {
    if (selectableVoltageChannelsNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = selectableVoltageChannelsNum*2;
    std::vector <uint16_t> txDataMessage(dataLength);

    for (uint16_t channelIdx = 0; channelIdx < selectableVoltageChannelsNum; channelIdx++) {
        if (find(availableVoltageSourcesIdxsArray.begin(), availableVoltageSourcesIdxsArray.end(), source) == availableVoltageSourcesIdxsArray.end()) {
            return ErrorValueOutOfRange;
        }
        txDataMessage[2*channelIdx] = voltageChannelSourcesRegisters[channelIdx];
        txDataMessage[2*channelIdx+1] = source;
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
    selectedSourceForVoltageChannelIdx = source;
    this->selectChannelsResolutions();

    return ret;
}

ErrorCodes_t EZPatchDevice::setSourceForCurrentChannel(uint16_t source, bool applyFlag) {
    if (selectableCurrentChannelsNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    uint16_t dataLength = selectableCurrentChannelsNum*2;
    std::vector <uint16_t> txDataMessage(dataLength);

    for (uint16_t channelIdx = 0; channelIdx < selectableCurrentChannelsNum; channelIdx++) {
        if (find(availableCurrentSourcesIdxsArray.begin(), availableCurrentSourcesIdxsArray.end(), source) == availableCurrentSourcesIdxsArray.end()) {
            return ErrorValueOutOfRange;
        }
        txDataMessage[2*channelIdx] = currentChannelSourcesRegisters[channelIdx];
        txDataMessage[2*channelIdx+1] = source;
    }

    ErrorCodes_t ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
    selectedSourceForVoltageChannelIdx = source;
    this->selectChannelsResolutions();

    return ret;
}

ErrorCodes_t EZPatchDevice::setCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (!vcCurrentOffsetDeltaImplemented) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    RangedMeasurement_t range = vcCurrentRangesArray[selectedVcCurrentRangeIdx];
    if (!allInRange(offsets, range.getMin(), range.getMax())) {
        return ErrorValueOutOfRange;
    }
    std::map <uint16_t, Measurement_t> m;
    for (size_t i = 0; i < channelIndexes.size(); ++i) {
        m[channelIndexes[i]] = offsets[i];
    }
    for (auto e : m) {
        uint16_t chIdx = e.first;
        Measurement_t current = e.second;

        current.convertValue(range.prefix);
        calibrationParams.setValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx, chIdx, current);
    }
    return this->updateCalibVcCurrentOffset(channelIndexes, applyFlag);
}

ErrorCodes_t EZPatchDevice::updateCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, bool) {
    if (!vcCurrentOffsetDeltaImplemented) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    RangedMeasurement_t range = vcCurrentRangesArray[selectedVcCurrentRangeIdx];
    uint16_t dataLength = 2*channelIndexes.size();
    std::vector <uint16_t> txDataMessage(dataLength);
    for (auto chIdx : channelIndexes) {
        calibrationParams.convertValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx, chIdx, range.prefix);

        txDataMessage[0+chIdx*2] = vcCurrentOffsetDeltaRegisterOffset+chIdx*coreSpecificRegistersNum;
        txDataMessage[1+chIdx*2] = (uint16_t)((int16_t)round(calibrationParams.getValue(CalTypesVcOffsetAdc, selectedSamplingRateIdx, selectedVcCurrentRangeIdx, chIdx).value*SHORT_MAX/range.max));
    }

    return this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
}

ErrorCodes_t EZPatchDevice::setCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) {
    if (!ccVoltageOffsetDeltaImplemented) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    RangedMeasurement_t range = ccVoltageRangesArray[selectedCcVoltageRangeIdx];
    if (!allInRange(offsets, range.getMin(), range.getMax())) {
        return ErrorValueOutOfRange;
    }
    std::map <uint16_t, Measurement_t> m;
    for (size_t i = 0; i < channelIndexes.size(); ++i) {
        m[channelIndexes[i]] = offsets[i];
    }
    for (auto e : m) {
        uint16_t chIdx = e.first;
        Measurement_t voltage = e.second;

        voltage.convertValue(range.prefix);
        calibrationParams.setValue(CalTypesCcOffsetAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx, chIdx, voltage);
    }
    return this->updateCalibCcVoltageOffset(channelIndexes, applyFlag);
}

ErrorCodes_t EZPatchDevice::updateCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, bool) {
    if (!ccVoltageOffsetDeltaImplemented) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, voltageChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    RangedMeasurement_t range = ccVoltageRangesArray[selectedCcVoltageRangeIdx];
    uint16_t dataLength = 2*channelIndexes.size();
    std::vector <uint16_t> txDataMessage(dataLength);
    for (auto chIdx : channelIndexes) {
        calibrationParams.convertValue(CalTypesCcOffsetAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx, chIdx, range.prefix);

        txDataMessage[0+chIdx*2] = ccVoltageOffsetDeltaRegisterOffset+chIdx*coreSpecificRegistersNum;
        txDataMessage[1+chIdx*2] = (uint16_t)((int16_t)round(calibrationParams.getValue(CalTypesCcOffsetAdc, selectedSamplingRateIdx, selectedCcVoltageRangeIdx, chIdx).value*SHORT_MAX/range.max));
    }

    return this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
}

ErrorCodes_t EZPatchDevice::setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (currentRangeIdx < vcCurrentRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        for (unsigned int currentRangesSwitchIdx = 0; currentRangesSwitchIdx < vcCurrentRangesSwitchesNum; currentRangesSwitchIdx++) {
            if (vcCurrentRangesSwitchesLut[currentRangeIdx][currentRangesSwitchIdx]) {
                txDataMessage[vcCurrentRangesSwitchesWord[currentRangesSwitchIdx]] |=
                        vcCurrentRangesSwitchesByte[currentRangesSwitchIdx]; // 1

            } else {
                txDataMessage[vcCurrentRangesSwitchesWord[currentRangesSwitchIdx]] &=
                        ~vcCurrentRangesSwitchesByte[currentRangesSwitchIdx]; // 0
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            currentRange = vcCurrentRangesArray[currentRangeIdx];
            selectedVcCurrentRangeIdx = currentRangeIdx;
            currentResolution = currentRange.step;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (currentRangeIdx < ccCurrentRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        for (unsigned int currentRangesSwitchIdx = 0; currentRangesSwitchIdx < ccCurrentRangesSwitchesNum; currentRangesSwitchIdx++) {
            if (ccCurrentRangesSwitchesLut[currentRangeIdx][currentRangesSwitchIdx]) {
                txDataMessage[ccCurrentRangesSwitchesWord[currentRangesSwitchIdx]] |=
                        ccCurrentRangesSwitchesByte[currentRangesSwitchIdx]; // 1

            } else {
                txDataMessage[ccCurrentRangesSwitchesWord[currentRangesSwitchIdx]] &=
                        ~ccCurrentRangesSwitchesByte[currentRangesSwitchIdx]; // 0
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            currentRange = ccCurrentRangesArray[currentRangeIdx];
            selectedCcCurrentRangeIdx = currentRangeIdx;
            currentResolution = currentRange.step;
            currentCommandResolution = currentResolution*ccCurrentCommandResolutionGain;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (voltageRangeIdx < vcVoltageRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        for (unsigned int voltageRangesSwitchIdx = 0; voltageRangesSwitchIdx < vcVoltageRangesSwitchesNum; voltageRangesSwitchIdx++) {
            if (vcVoltageRangesSwitchesLut[voltageRangeIdx][voltageRangesSwitchIdx]) {
                txDataMessage[vcVoltageRangesSwitchesWord[voltageRangesSwitchIdx]] |=
                        vcVoltageRangesSwitchesByte[voltageRangesSwitchIdx]; // 1

            } else {
                txDataMessage[vcVoltageRangesSwitchesWord[voltageRangesSwitchIdx]] &=
                        ~vcVoltageRangesSwitchesByte[voltageRangesSwitchIdx]; // 0
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            voltageRange = vcVoltageRangesArray[voltageRangeIdx];
            selectedVcVoltageRangeIdx = voltageRangeIdx;
            voltageResolution = voltageRange.step;
            voltageCommandResolution = voltageResolution*vcVoltageCommandResolutionGain;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (voltageRangeIdx < ccVoltageRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        for (unsigned int voltageRangesSwitchIdx = 0; voltageRangesSwitchIdx < ccVoltageRangesSwitchesNum; voltageRangesSwitchIdx++) {
            if (ccVoltageRangesSwitchesLut[voltageRangeIdx][voltageRangesSwitchIdx]) {
                txDataMessage[ccVoltageRangesSwitchesWord[voltageRangesSwitchIdx]] |=
                        ccVoltageRangesSwitchesByte[voltageRangesSwitchIdx]; // 1

            } else {
                txDataMessage[ccVoltageRangesSwitchesWord[voltageRangesSwitchIdx]] &=
                        ~ccVoltageRangesSwitchesByte[voltageRangesSwitchIdx]; // 0
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            voltageRange = ccVoltageRangesArray[voltageRangeIdx];
            selectedCcVoltageRangeIdx = voltageRangeIdx;
            voltageResolution = voltageRange.step;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (samplingRateIdx < samplingRatesNum) {
        uint16_t dataLength = 1;
        std::vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = samplingRateIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSamplingRate, txDataMessage, dataLength);

        /*! Make a chip reset to force resynchronization of chip states. This is important when the clock changes (SR slow to SR fast or vice versa) */
        this->resetAsic(true, true);
        std::this_thread::sleep_for (std::chrono::milliseconds(resetDurationMs));
        this->resetAsic(false, true);

        selectedSamplingRateIdx = samplingRateIdx;
        samplingRate = realSamplingRatesArray[samplingRateIdx];
        integrationStep = integrationStepArray[selectedSamplingRateIdx];

        /*! Used for the digital filter */
        this->computeRawDataFilterCoefficients();

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::readoutOffsetRecalibration(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (!vcCurrentOffsetDeltaImplemented) {
        return ErrorFeatureNotImplemented;
    }
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (anyLiquidJunctionActive) {
        return ErrorLiquidJunctionAndRecalibration;
    }

    std::unique_lock <std::mutex> ljMutexLock (ljMutex);
    bool zeroProtocolFlag = false;
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        uint16_t chIdx = channelIndexes[i];
        channelModels[chIdx]->setRecalibratingReadoutOffset(onValues[i]);
        if (onValues[i] && (offsetRecalibStates[chIdx] == OffsetRecalibIdle)) {
            offsetRecalibStates[chIdx] = OffsetRecalibStarting;
            offsetRecalibStatuses[chIdx] = OffsetRecalibNotPerformed;
            zeroProtocolFlag = true;

        } else if (!onValues[i] && (offsetRecalibStates[chIdx] != OffsetRecalibIdle)) {
            offsetRecalibStates[chIdx] = OffsetRecalibTerminate;
        }
    }
    anyOffsetRecalibrationActive = true;
    computeCurrentOffsetFlag = true;
    ljMutexLock.unlock();

    if (zeroProtocolFlag) {
        this->zeroProtocol();
    }

    return Success;
}

ErrorCodes_t EZPatchDevice::liquidJunctionCompensation(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
    if (!allLessThan(channelIndexes, currentChannelsNum)) {
        return ErrorValueOutOfRange;
    }
    if (anyOffsetRecalibrationActive) {
        return ErrorLiquidJunctionAndRecalibration;
    }

    std::unique_lock <std::mutex> ljMutexLock(ljMutex);
    bool zeroProtocolFlag = false;
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        uint16_t chIdx = channelIndexes[i];
        channelModels[chIdx]->setCompensatingLiquidJunction(onValues[i]);
        if (onValues[i] && (liquidJunctionStates[chIdx] == LiquidJunctionIdle)) {
            liquidJunctionStates[chIdx] = LiquidJunctionStarting;
            liquidJunctionStatuses[chIdx] = LiquidJunctionNotPerformed;
            zeroProtocolFlag = true;

        } else if (!onValues[i]) {
            liquidJunctionStates[chIdx] = LiquidJunctionTerminate;
        }
    }
    anyLiquidJunctionActive = true;
    computeCurrentOffsetFlag = true;
    ljMutexLock.unlock();

    if (zeroProtocolFlag) {
        this->zeroProtocol();
    }

    return Success;
}

ErrorCodes_t EZPatchDevice::liquidJunctionCompensation(uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        this->setLiquidJunctionCompensationOverrideSwitch(false);
        uint16_t dataLength = 1;
        std::vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = channelIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdLiquidJunctionComp, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::liquidJunctionCompensationOverride(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    if (liquidJunctionCompensationOverrideImplemented) {
        if (channelIdx < currentChannelsNum) {
            ret = this->setLiquidJunctionCompensationOverrideSwitch(true);
            if (ret == Success) {
                ret = this->setLiquidJunctionCompensationOverrideValue(channelIdx, value);

                if (ret == Success) {
                    uint16_t dataLength = 1;
                    std::vector <uint16_t> txDataMessage(dataLength);
                    txDataMessage[0] = channelIdx;

                    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdLiquidJunctionComp, txDataMessage, dataLength);
                }
            }

        } else {
            ret = ErrorValueOutOfRange;
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::liquidJunctionCompensationInquiry(uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        uint16_t dataLength = 1;
        std::vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = channelIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdLiquidJunctionCompInquiry, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::zap(Measurement_t duration, uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        duration.convertValue(positiveProtocolTimeRange.prefix);

        uint16_t dataLength = 4;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->int322uint16((int32_t)round(duration.value/positiveProtocolTimeRange.step), txDataMessage, 0);
        txDataMessage[3] = txDataMessage[1];
        txDataMessage[1] = txDataMessage[0];
        txDataMessage[0] = zapDurationHwRegisterOffset;
        txDataMessage[2] = zapDurationHwRegisterOffset+1;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            dataLength = 1;
            txDataMessage[0] = channelIdx;

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdZap, txDataMessage, dataLength);
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (filterIdx < vcStimulusLpfOptionsNum) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        for (unsigned int vcStimulusLpfSwitchIdx = 0; vcStimulusLpfSwitchIdx < vcStimulusLpfSwitchesNum; vcStimulusLpfSwitchIdx++) {
            if (vcStimulusLpfSwitchesLut[filterIdx][vcStimulusLpfSwitchIdx]) {
                txDataMessage[vcStimulusLpfSwitchesWord[vcStimulusLpfSwitchIdx]] |=
                        vcStimulusLpfSwitchesByte[vcStimulusLpfSwitchIdx]; // 1

            } else {
                txDataMessage[vcStimulusLpfSwitchesWord[vcStimulusLpfSwitchIdx]] &=
                        ~vcStimulusLpfSwitchesByte[vcStimulusLpfSwitchIdx]; // 0
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            selectedVcVoltageFilterIdx = filterIdx;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (filterIdx < ccStimulusLpfOptionsNum) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        for (unsigned int ccStimulusLpfSwitchIdx = 0; ccStimulusLpfSwitchIdx < ccStimulusLpfSwitchesNum; ccStimulusLpfSwitchIdx++) {
            if (ccStimulusLpfSwitchesLut[filterIdx][ccStimulusLpfSwitchIdx]) {
                txDataMessage[ccStimulusLpfSwitchesWord[ccStimulusLpfSwitchIdx]] |=
                        ccStimulusLpfSwitchesByte[ccStimulusLpfSwitchIdx]; // 1

            } else {
                txDataMessage[ccStimulusLpfSwitchesWord[ccStimulusLpfSwitchIdx]] &=
                        ~ccStimulusLpfSwitchesByte[ccStimulusLpfSwitchIdx]; // 0
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            selectedCcCurrentFilterIdx = filterIdx;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::enableStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) {
//    if (!allLessThan(channelIndexes, currentChannelsNum)) {
//        return ErrorValueOutOfRange;
//    }
    for (uint32_t i = 0; i < channelIndexes.size(); i++) {
        this->enableStimulus(channelIndexes[i], onValues[i]);
        channelModels[channelIndexes[i]]->setStimActive(onValues[i]);
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::enableStimulus(uint16_t channelIdx, bool on) {
    ErrorCodes_t ret;

    if (!stimulusEnableImplemented) {
        ret  = ErrorFeatureNotImplemented;

    } else {
        if (channelIdx < currentChannelsNum) {
            uint16_t dataLength = switchesStatusLength;
            std::vector <uint16_t> txDataMessage(dataLength);
            this->switches2DataMessage(txDataMessage);

            if (on) {
                txDataMessage[stimulusEnableSwitchWord+channelIdx*coreSpecificSwitchesWordsNum] |=
                        stimulusEnableSwitchByte; // 1

            } else {
                txDataMessage[stimulusEnableSwitchWord+channelIdx*coreSpecificSwitchesWordsNum] &=
                        ~stimulusEnableSwitchByte; // 0
            }

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
            if (ret == Success) {
                this->dataMessage2Switches(txDataMessage);
            }

        } else {
            ret = ErrorValueOutOfRange;
        }
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnLedOn(uint16_t ledIndex, bool on) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    if (on) {
        txDataMessage[ledsWord[ledIndex]] &= ~ledsByte[ledIndex];

    } else {
        txDataMessage[ledsWord[ledIndex]] |= ledsByte[ledIndex];
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setAnalogOut(bool on) {
    ErrorCodes_t ret;
    if (analogOutImplementedFlag) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (on) {
            txDataMessage[analogOutWord] |= analogOutByte;

        } else {
            txDataMessage[analogOutWord] &= ~analogOutByte;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setSlave(bool on) {
    ErrorCodes_t ret;
    if (slaveImplementedFlag) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (on) {
            txDataMessage[slaveWord] |= slaveByte;

        } else {
            txDataMessage[slaveWord] &= ~slaveByte;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setConstantSwitches() {
    if (constantSwitchesNum == 0) {
        return Success;
    }
    ErrorCodes_t ret = Success;

    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    for (unsigned int constantSwitchIdx = 0; constantSwitchIdx < constantSwitchesNum; constantSwitchIdx++) {
        if (constantSwitchesLut[constantSwitchIdx]) {
            txDataMessage[constantSwitchesWord[constantSwitchIdx]] |=
                    constantSwitchesByte[constantSwitchIdx]; // 1

        } else {
            txDataMessage[constantSwitchesWord[constantSwitchIdx]] &=
                    ~constantSwitchesByte[constantSwitchIdx]; // 0
        }
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setConstantRegisters() {
    if (constantRegistersNum == 0) {
        return Success;
    }
    ErrorCodes_t ret = Success;
    uint16_t dataLength = 2;
    std::vector <uint16_t> txDataMessage(dataLength);

    for (uint16_t idx = 0; idx < constantRegistersNum; idx++) {
        txDataMessage[0] = constantRegistersWord[idx];
        txDataMessage[1] = constantRegistersValues[idx];

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
        if (ret != Success) {
            return ret;
        }
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setCompensationsChannel(uint16_t channelIdx) {
    compensationsSettingChannel = channelIdx;
    /*! Needed to update compensable field in all CompensationControl_t */
    this->checkCompensationsValues();
    return Success;
}

ErrorCodes_t EZPatchDevice::enableCompensation(std::vector <uint16_t> channelIndexes, CompensationTypes_t compTypeToEnable, std::vector <bool> onValues, bool applyFlag) {
    int tempCompensationSettingChannel = compensationsSettingChannel;
    switch (compTypeToEnable) {
    case CompCfast:
        if (!pipetteCompensationImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->turnPipetteCompensationOn(onValues[i]);
            channelModels[compensationsSettingChannel]->setCompensatingCfast(onValues[i]);
        }
        break;

    case CompCslow:
        if (!membraneCompensationImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->turnMembraneCompensationOn(onValues[i]);
            channelModels[compensationsSettingChannel]->setCompensatingCslowRs(onValues[i]);
        }
        break;

    case CompRsCorr:
        if (!resistanceCorrectionImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->turnResistanceCorrectionOn(onValues[i]);
            channelModels[compensationsSettingChannel]->setCompensatingRsCp(onValues[i]);
        }
        break;

    case CompRsPred:
        if (!resistancePredictionImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->turnResistancePredictionOn(onValues[i]);
            channelModels[compensationsSettingChannel]->setCompensatingRsPg(onValues[i]);
        }
        break;

    case CompCcCfast:
        if (!ccPipetteCompensationImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!ccCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->turnCCPipetteCompensationOn(onValues[i]);
            channelModels[compensationsSettingChannel]->setCompensatingCcCfast(onValues[i]);
        }
        break;

    default:
        return ErrorFeatureNotImplemented;
    }
    compensationsSettingChannel = tempCompensationSettingChannel;
    return Success;
}

ErrorCodes_t EZPatchDevice::enableVcCompensations(bool enable, bool applyFlag) {
    int tempCompensationSettingChannel = compensationsSettingChannel;
    bool flags[6];
    ErrorCodes_t ret = Success;

    for (int compensationsSettingChannel = 0; compensationsSettingChannel < currentChannelsNum; compensationsSettingChannel++) {
        flags[0] = compensationsEnabledArray[CompCfast][compensationsSettingChannel];
        flags[1] = compensationsEnabledArray[CompCslow][compensationsSettingChannel];
        flags[2] = compensationsEnabledArray[CompRsComp][compensationsSettingChannel];
        flags[3] = compensationsEnabledArray[CompRsCorr][compensationsSettingChannel];
        flags[4] = compensationsEnabledArray[CompRsPred][compensationsSettingChannel];
        flags[5] = compensationsEnabledArray[CompGLeak][compensationsSettingChannel];

        compensationsEnabledArray[CompCfast][compensationsSettingChannel] = enable & compensationsEnableFlags[CompCfast][compensationsSettingChannel];
        compensationsEnabledArray[CompCslow][compensationsSettingChannel] = enable & compensationsEnableFlags[CompCslow][compensationsSettingChannel];
        compensationsEnabledArray[CompRsComp][compensationsSettingChannel] = enable & compensationsEnableFlags[CompRsComp][compensationsSettingChannel];
        compensationsEnabledArray[CompRsCorr][compensationsSettingChannel] = enable & compensationsEnableFlags[CompRsCorr][compensationsSettingChannel];
        compensationsEnabledArray[CompRsPred][compensationsSettingChannel] = enable & compensationsEnableFlags[CompRsPred][compensationsSettingChannel];
        compensationsEnabledArray[CompGLeak][compensationsSettingChannel] = enable & compensationsEnableFlags[CompGLeak][compensationsSettingChannel];

        ErrorCodes_t tempRet = this->turnCompensationsOn(vcCompensationsActivated, enable);
        if (tempRet != Success) {
            ret = tempRet;
            compensationsEnabledArray[CompCfast][compensationsSettingChannel] = flags[0];
            compensationsEnabledArray[CompCslow][compensationsSettingChannel] = flags[1];
            compensationsEnabledArray[CompRsComp][compensationsSettingChannel] = flags[2];
            compensationsEnabledArray[CompRsCorr][compensationsSettingChannel] = flags[3];
            compensationsEnabledArray[CompRsPred][compensationsSettingChannel] = flags[4];
            compensationsEnabledArray[CompGLeak][compensationsSettingChannel] = flags[5];
        }
    }
    compensationsSettingChannel = tempCompensationSettingChannel;
    return ret;
}

ErrorCodes_t EZPatchDevice::enableCcCompensations(bool enable, bool applyFlag) {
    int tempCompensationSettingChannel = compensationsSettingChannel;
    bool flags[2];
    ErrorCodes_t ret = Success;

    for (int compensationsSettingChannel = 0; compensationsSettingChannel < currentChannelsNum; compensationsSettingChannel++) {
        flags[0] = compensationsEnabledArray[CompCcCfast][compensationsSettingChannel];
        flags[1] = compensationsEnabledArray[CompBridgeRes][compensationsSettingChannel];

        compensationsEnabledArray[CompCcCfast][compensationsSettingChannel] = enable & compensationsEnableFlags[CompCcCfast][compensationsSettingChannel];
        compensationsEnabledArray[CompBridgeRes][compensationsSettingChannel] = enable & compensationsEnableFlags[CompBridgeRes][compensationsSettingChannel];

        ErrorCodes_t tempRet = this->turnCompensationsOn(ccCompensationsActivated, enable);
        if (tempRet != Success) {
            ret = tempRet;
            compensationsEnabledArray[CompCcCfast][compensationsSettingChannel] = flags[0];
            compensationsEnabledArray[CompBridgeRes][compensationsSettingChannel] = flags[1];
        }
    }
    compensationsSettingChannel = tempCompensationSettingChannel;
    return ret;
}

ErrorCodes_t EZPatchDevice::turnPipetteCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompCfast][compensationsSettingChannel];
    compensationsEnabledArray[CompCfast][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompCfast], on);
    if (ret != Success) {
        compensationsEnabledArray[CompCfast][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnCCPipetteCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompCcCfast][compensationsSettingChannel];
    compensationsEnabledArray[CompCcCfast][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompCcCfast], on);
    if (ret != Success) {
        compensationsEnabledArray[CompCcCfast][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnMembraneCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompCslow][compensationsSettingChannel];
    compensationsEnabledArray[CompCslow][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompCslow], on);
    if (ret != Success) {
        compensationsEnabledArray[CompCslow][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnResistanceCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompRsComp][compensationsSettingChannel];
    compensationsEnabledArray[CompRsComp][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompRsComp], on);
    if (ret != Success) {
        compensationsEnabledArray[CompRsComp][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnResistanceCorrectionOn(bool on) {
    bool flag = compensationsEnabledArray[CompRsCorr][compensationsSettingChannel];
    compensationsEnabledArray[CompRsCorr][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompRsCorr], on);
    if (ret != Success) {
        compensationsEnabledArray[CompRsCorr][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnResistancePredictionOn(bool on) {
    bool flag = compensationsEnabledArray[CompRsPred][compensationsSettingChannel];
    compensationsEnabledArray[CompRsPred][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompRsPred], on);
    if (ret != Success) {
        compensationsEnabledArray[CompRsPred][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnLeakConductanceCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompGLeak][compensationsSettingChannel];
    compensationsEnabledArray[CompGLeak][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompGLeak], on);
    if (ret != Success) {
        compensationsEnabledArray[CompGLeak][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnBridgeBalanceCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompBridgeRes][compensationsSettingChannel];
    compensationsEnabledArray[CompBridgeRes][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(compensationsEnableFlags[CompBridgeRes], on);
    if (ret != Success) {
        compensationsEnabledArray[CompBridgeRes][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setPipetteCompensationOptions(uint16_t optionIdx) {
    if (optionIdx < pipetteCompensationOptions.size()) {
        uint16_t originalReg = pipetteCompensationOptionReg[compensationsSettingChannel];
        pipetteCompensationOptionReg[compensationsSettingChannel] = (optionIdx << pipetteCompensationOptionOffset) & pipetteCompensationOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            pipetteCompensationOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setCompValues(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <double> newParamValues, bool) {
    ErrorCodes_t err;
    int tempCompensationSettingChannel = compensationsSettingChannel;
    for (int i = 0; i < channelIndexes.size(); i++) {
        compensationsSettingChannel = channelIndexes[i];
        switch (paramToUpdate) {
        case U_CpVc:
            err = this->setPipetteCapacitance({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_Cm:
            err = this->setMembraneCapacitance({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_Rs:
            err = this->setAccessResistance({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_RsCp:
            err = this->setResistanceCorrectionPercentage({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_RsCl:
            err = this->setResistanceCorrectionLag({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_RsPg:
            err = this->setResistancePredictionGain({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_RsPp:
            err = this->setResistancePredictionPercentage({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_RsPt:
            err = this->setResistancePredictionTau({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_LkG:
            err = this->setLeakConductance({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_CpCc:
            err = this->setCCPipetteCapacitance({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case U_BrB:
            err = this->setBridgeBalanceResistance({newParamValues[i], compensationControls[paramToUpdate][compensationsSettingChannel].prefix, compensationControls[paramToUpdate][compensationsSettingChannel].unit});
            break;

        case CompensationUserParamsNum:
            return ErrorValueOutOfRange;
        }

        if (err == Success) {
            compValueMatrix[compensationsSettingChannel][paramToUpdate] = newParamValues[i];
        }
    }
    compensationsSettingChannel = tempCompensationSettingChannel;
    return Success;
}

ErrorCodes_t EZPatchDevice::setCompOptions(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <uint16_t> options, bool) {
    int tempCompensationSettingChannel = compensationsSettingChannel;
    switch (type) {
    case CompCfast:
        if (!pipetteCompensationImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->setPipetteCompensationOptions(options[i]);
        }
        break;

    case CompCslow:
        if (!membraneCompensationImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->setMembraneCompensationOptions(options[i]);
        }
        break;

    case CompRsCorr:
        if (!resistanceCorrectionImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->setResistanceCorrectionOptions(options[i]);
        }
        break;

    case CompRsPred:
        if (!resistancePredictionImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->setResistancePredictionOptions(options[i]);
        }
        break;

    case CompCcCfast:
        if (!ccPipetteCompensationImplemented) {
            return ErrorFeatureNotImplemented;
        }

        if (!ccCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsSettingChannel = channelIndexes[i];
            this->setCCPipetteCompensationOptions(options[i]);
        }
        break;

    default:
        return ErrorFeatureNotImplemented;
    }
    compensationsSettingChannel = tempCompensationSettingChannel;
    return Success;
}

ErrorCodes_t EZPatchDevice::setCCPipetteCompensationOptions(uint16_t optionIdx) {
    if (optionIdx < ccPipetteCompensationOptions.size()) {
        uint16_t originalReg = ccPipetteCompensationOptionReg[compensationsSettingChannel];
        ccPipetteCompensationOptionReg[compensationsSettingChannel] = (optionIdx << ccPipetteCompensationOptionOffset) & ccPipetteCompensationOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            ccPipetteCompensationOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setMembraneCompensationOptions(uint16_t optionIdx) {
    if (optionIdx < membraneCompensationOptions.size()) {
        uint16_t originalReg = membraneCompensationOptionReg[compensationsSettingChannel];
        membraneCompensationOptionReg[compensationsSettingChannel] = (optionIdx << membraneCompensationOptionOffset) & membraneCompensationOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            membraneCompensationOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setResistanceCompensationOptions(uint16_t optionIdx) {
    if (optionIdx < resistanceCompensationOptions.size()) {
        uint16_t originalReg = resistanceCompensationOptionReg[compensationsSettingChannel];
        resistanceCompensationOptionReg[compensationsSettingChannel] = (optionIdx << resistanceCompensationOptionOffset) & resistanceCompensationOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            resistanceCompensationOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setResistanceCorrectionOptions(uint16_t optionIdx) {
    if (optionIdx < resistanceCorrectionOptions.size()) {
        uint16_t originalReg = resistanceCorrectionOptionReg[compensationsSettingChannel];
        resistanceCorrectionOptionReg[compensationsSettingChannel] = (optionIdx << resistanceCorrectionOptionOffset) & resistanceCorrectionOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            resistanceCorrectionOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setResistancePredictionOptions(uint16_t optionIdx) {
    if (optionIdx < resistancePredictionOptions.size()) {
        uint16_t originalReg = resistancePredictionOptionReg[compensationsSettingChannel];
        resistancePredictionOptionReg[compensationsSettingChannel] = (optionIdx << resistancePredictionOptionOffset) & resistancePredictionOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            resistancePredictionOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setLeakConductanceCompensationOptions(uint16_t optionIdx) {
    if (optionIdx < leakConductanceCompensationOptions.size()) {
        uint16_t originalReg = leakConductanceCompensationOptionReg[compensationsSettingChannel];
        leakConductanceCompensationOptionReg[compensationsSettingChannel] = (optionIdx << leakConductanceCompensationOptionOffset) & leakConductanceCompensationOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            leakConductanceCompensationOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setBridgeBalanceCompensationOptions(uint16_t optionIdx) {
    if (optionIdx < bridgeBalanceCompensationOptions.size()) {
        uint16_t originalReg = bridgeBalanceCompensationOptionReg[compensationsSettingChannel];
        bridgeBalanceCompensationOptionReg[compensationsSettingChannel] = (optionIdx << bridgeBalanceCompensationOptionOffset) & bridgeBalanceCompensationOptionMask;
        ErrorCodes_t ret = this->setCompensationsOptions();
        if (ret != Success) {
            bridgeBalanceCompensationOptionReg[compensationsSettingChannel] = originalReg;
        }
        return ret;

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchDevice::setPipetteCapacitance(Measurement_t capacitance) {
    return this->setCompensationValue(compensationControls[U_CpVc][compensationsSettingChannel], capacitance);
}

ErrorCodes_t EZPatchDevice::setCCPipetteCapacitance(Measurement_t capacitance) {
    return this->setCompensationValue(compensationControls[U_CpCc][compensationsSettingChannel], capacitance);
}

ErrorCodes_t EZPatchDevice::setMembraneCapacitance(Measurement_t capacitance) {
    return this->setCompensationValue(compensationControls[U_Cm][compensationsSettingChannel], capacitance);
}

ErrorCodes_t EZPatchDevice::setAccessResistance(Measurement_t resistance) {
    return this->setCompensationValue(compensationControls[U_Rs][compensationsSettingChannel], resistance);
}

ErrorCodes_t EZPatchDevice::setResistanceCorrectionPercentage(Measurement_t percentage) {
    return this->setCompensationValue(compensationControls[U_RsCp][compensationsSettingChannel], percentage);
}

ErrorCodes_t EZPatchDevice::setResistanceCorrectionLag(Measurement_t lag) {
    return this->setCompensationValue(compensationControls[U_RsCl][compensationsSettingChannel], lag);
}

ErrorCodes_t EZPatchDevice::setResistancePredictionGain(Measurement_t gain) {
    return this->setCompensationValue(compensationControls[U_RsPg][compensationsSettingChannel], gain);
}

ErrorCodes_t EZPatchDevice::setResistancePredictionPercentage(Measurement_t percentage) {
    return this->setCompensationValue(compensationControls[U_RsPp][compensationsSettingChannel], percentage);
}

ErrorCodes_t EZPatchDevice::setResistancePredictionTau(Measurement_t tau) {
    return this->setCompensationValue(compensationControls[U_RsPt][compensationsSettingChannel], tau);
}

ErrorCodes_t EZPatchDevice::setLeakConductance(Measurement_t conductance) {
    return this->setCompensationValue(compensationControls[U_LkG][compensationsSettingChannel], conductance);
}

ErrorCodes_t EZPatchDevice::setBridgeBalanceResistance(Measurement_t resistance) {
    return this->setCompensationValue(compensationControls[U_BrB][compensationsSettingChannel], resistance);
}

ErrorCodes_t EZPatchDevice::setDigitalTriggerOutput(uint16_t triggerIdx, bool terminator, bool polarity, uint16_t triggerId, Measurement_t delay) {
    if (maxDigitalTriggerOutputEvents == 0) {
        return ErrorFeatureNotImplemented;
    }
    ErrorCodes_t ret;

    if ((triggerIdx < maxDigitalTriggerOutputEvents) &&
            triggerId < digitalTriggersNum) {

        delay.convertValue(positiveProtocolTimeRange.prefix);

        uint16_t dataLength = 4;
        std::vector <uint16_t> txDataMessage(dataLength);

        txDataMessage[0] = triggerIdx;
        txDataMessage[1] = (terminator ? 0x8000 : 0x0000) + (polarity ? 0x0100 : 0x0000) + triggerId;
        this->int322uint16((int32_t)round(delay.value/positiveProtocolTimeRange.step), txDataMessage, 2);

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdDigitalTriggerOutput, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setDigitalRepetitiveTriggerOutput(uint16_t triggersNum, bool valid, bool infinite, uint16_t triggerId,
                                                                  Measurement_t delay, Measurement_t duration, Measurement_t period, Measurement_t deltaPeriod) {
    ErrorCodes_t ret;

    if (repetitiveTriggerAvailableFlag &&
            (triggerId < digitalTriggersNum)) {

        delay.convertValue(positiveProtocolTimeRange.prefix);
        duration.convertValue(positiveProtocolTimeRange.prefix);
        period.convertValue(positiveProtocolTimeRange.prefix);
        deltaPeriod.convertValue(positiveProtocolTimeRange.prefix);

        if (period.value <= duration.value) {
            duration.value = period.value-1.0*positiveProtocolTimeRange.step;
        }

        if (deltaPeriod.value < -0.5*positiveProtocolTimeRange.step) { /*! Loose condition for checking if the number is negative: due to rounding it can be negative and very small when it should be zero */
            infinite = false;
            triggersNum = fmin(triggersNum, (uint16_t)floor((period.value-duration.value)/deltaPeriod.value));
        }

        uint16_t dataLength = 10;
        std::vector <uint16_t> txDataMessage(dataLength);

        txDataMessage[0] = triggersNum;
        txDataMessage[1] = (valid ? 0x0000 : 0x8000) + (infinite ? 0x0100 : 0x0000) + triggerId;
        this->int322uint16((int32_t)round(delay.value/positiveProtocolTimeRange.step), txDataMessage, 2);
        this->int322uint16((int32_t)round((period.value-duration.value)/positiveProtocolTimeRange.step), txDataMessage, 4);
        this->int322uint16((int32_t)round(deltaPeriod.value/positiveProtocolTimeRange.step), txDataMessage, 6);
        this->int322uint16((int32_t)round(duration.value/positiveProtocolTimeRange.step), txDataMessage, 8);

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdDigitalRepetitiveTriggerOutput, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest, bool stopProtocolFlag) {
    if (itemsNum > protocolMaxItemsNum) {
        return ErrorValueOutOfRange;
    }
    if (stopProtocolFlag) {
        this->stopProtocol();
    }

    Measurement zeroSeconds = {0, UnitPfx::UnitPfxNone, "s"};
    this->setDigitalTriggerOutput(0, true, true, 0, zeroSeconds);
    this->setDigitalRepetitiveTriggerOutput(0, false, false, 0, zeroSeconds, zeroSeconds, zeroSeconds, zeroSeconds);

    selectedProtocolId = protId;
    selectedProtocolItemsNum = itemsNum;
    selectedProtocolVrest = vRest;
    vRest.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);

    uint16_t dataLength = 5;
    std::vector <uint16_t> txDataMessage(dataLength);
    txDataMessage[0] = protId;
    txDataMessage[1] = itemsNum;
    txDataMessage[2] = sweepsNum;
    this->int322uint16((int32_t)round(vRest.value/voltageCommandResolution), txDataMessage, 3);

    stepsOnLastSweep = (double)(sweepsNum-1);
    protocolItemsNum = itemsNum;
    protocolItemIndex = 0;

    return this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdVoltageProtocolStruct, txDataMessage, dataLength);
}

ErrorCodes_t EZPatchDevice::voltStepTimeStep(Measurement_t v0, Measurement_t vStep, Measurement_t t0, Measurement_t tStep,
                                                 uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    v0.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
    vStep.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
    t0.convertValue(positiveProtocolTimeRange.prefix);
    tStep.convertValue(positiveProtocolTimeRange.prefix);

    if ((this->checkVoltStepTimeStepParameters(v0.value, vStep.value, t0.value, tStep.value, repsNum, applySteps) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            std::vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(v0.value/voltageCommandResolution), txDataMessage, 0);
            this->int322uint16((int32_t)round(vStep.value/voltageCommandResolution), txDataMessage, 2);
            this->int322uint16((int32_t)round(t0.value/positiveProtocolTimeRange.step), txDataMessage, 4);
            this->int322uint16((int32_t)round(tStep.value/positiveProtocolTimeRange.step), txDataMessage, 6);
            txDataMessage[8] = 0xF00D;
            txDataMessage[9] = 0xBAAD;
            txDataMessage[10] = 0xF00D;
            txDataMessage[11] = 0xBAAD;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdVoltageStepTimeStep, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::voltRamp(Measurement_t v0, Measurement_t vFinal, Measurement_t t,
                                         uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    v0.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
    vFinal.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
    t.convertValue(positiveProtocolTimeRange.prefix);

    if ((this->checkVoltRampParameters(v0.value, vFinal.value, t.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            std::vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(v0.value/voltageCommandResolution), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(vFinal.value/voltageCommandResolution), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(t.value/positiveProtocolTimeRange.step), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdVoltageRamp, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::voltSin(Measurement_t v0, Measurement_t vAmp, Measurement_t freq,
                                        uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    v0.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
    vAmp.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);
    freq.convertValue(positiveProtocolFrequencyRange.prefix);

    if ((this->checkVoltSinParameters(v0.value, vAmp.value, freq.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            std::vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(v0.value/voltageCommandResolution), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(vAmp.value/voltageCommandResolution), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(freq.value/positiveProtocolFrequencyRange.step), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdVoltageSin, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) {
    return this->voltStepTimeStep(v0, v0Step, t0, t0Step, itemIdx, nextItemIdx, loopReps, applyStepsFlag);
}

ErrorCodes_t EZPatchDevice::setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) {
    return this->voltRamp(v0, vFinal, t0, itemIdx, nextItemIdx, loopReps, applyStepsFlag);
}

ErrorCodes_t EZPatchDevice::setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag) {
    return this->voltSin(v0, vAmp, f0, itemIdx, nextItemIdx, loopReps, applyStepsFlag);
}

ErrorCodes_t EZPatchDevice::startProtocol() {
    ErrorCodes_t ret;

    if (startProtocolCommandImplemented) {
        uint16_t dataLength = 0;
        std::vector <uint16_t> txDataMessage(dataLength);

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdStartProtocol, txDataMessage, dataLength);

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::stopProtocol() {
    bool stopProtocolFlag = false; /*! We're already commiting a stop protocol, so commiting another one on the protocol structure will create an infinite recursion */
    if (selectedClampingModality == ClampingModality_t::VOLTAGE_CLAMP) {
        this->setVoltageProtocolStructure(selectedProtocolId-1, 2, 1, selectedProtocolVrest, stopProtocolFlag);
        this->setVoltageProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "V"}, {0.0, UnitPfxNone, "V"}, {2.0, UnitPfxMilli, "s"}, {0.0, UnitPfxNone, "s"}, false);
        this->setVoltageProtocolStep(1, 2, 1, false, {0.0, UnitPfxNone, "V"}, {0.0, UnitPfxNone, "V"}, {2.0, UnitPfxMilli, "s"}, {0.0, UnitPfxNone, "s"}, false);

    } else {
        this->setCurrentProtocolStructure(selectedProtocolId-1, 2, 1, selectedProtocolIrest, stopProtocolFlag);
        this->setCurrentProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "A"}, {0.0, UnitPfxNone, "A"}, {2.0, UnitPfxMilli, "s"}, {0.0, UnitPfxNone, "s"}, false);
        this->setCurrentProtocolStep(1, 2, 1, false, {0.0, UnitPfxNone, "A"}, {0.0, UnitPfxNone, "A"}, {2.0, UnitPfxMilli, "s"}, {0.0, UnitPfxNone, "s"}, false);
    }
    return this->startProtocol();
}

ErrorCodes_t EZPatchDevice::zeroProtocol() {
    bool stopProtocolFlag = false; /*! We're want to apply this instead of the stop protocol */
    if (selectedClampingModality == ClampingModality_t::VOLTAGE_CLAMP) {
        this->setVoltageProtocolStructure(selectedProtocolId-1, 2, 1, selectedProtocolVrest, stopProtocolFlag);
        this->setVoltageProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "V"}, {0.0, UnitPfxNone, "V"}, {100.0, UnitPfxKilo, "s"}, {0.0, UnitPfxNone, "s"}, false);
        this->setVoltageProtocolStep(1, 0, 1, false, {0.0, UnitPfxNone, "V"}, {0.0, UnitPfxNone, "V"}, {100.0, UnitPfxKilo, "s"}, {0.0, UnitPfxNone, "s"}, false);

    } else {
        this->setCurrentProtocolStructure(selectedProtocolId-1, 2, 1, selectedProtocolIrest, stopProtocolFlag);
        this->setCurrentProtocolStep(0, 1, 1, false, {0.0, UnitPfxNone, "A"}, {0.0, UnitPfxNone, "A"}, {100.0, UnitPfxKilo, "s"}, {0.0, UnitPfxNone, "s"}, false);
        this->setCurrentProtocolStep(1, 0, 1, false, {0.0, UnitPfxNone, "A"}, {0.0, UnitPfxNone, "A"}, {100.0, UnitPfxKilo, "s"}, {0.0, UnitPfxNone, "s"}, false);
    }
    return this->startProtocol();
}

ErrorCodes_t EZPatchDevice::setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest, bool stopProtocolFlag) {
    if (itemsNum > protocolMaxItemsNum) {
        return ErrorValueOutOfRange;
    }
    if (stopProtocolFlag) {
        this->stopProtocol();
    }

    selectedProtocolId = protId;
    selectedProtocolItemsNum = itemsNum;
    selectedProtocolIrest = iRest;

    iRest.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);

    uint16_t dataLength = 5;
    std::vector <uint16_t> txDataMessage(dataLength);
    txDataMessage[0] = protId;
    txDataMessage[1] = itemsNum;
    txDataMessage[2] = sweepsNum;
    this->int322uint16((int32_t)round(iRest.value/currentCommandResolution), txDataMessage, 3);

    stepsOnLastSweep = (double)(sweepsNum-1);
    protocolItemsNum = itemsNum;
    protocolItemIndex = 0;

    return this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdCurrentProtocolStruct, txDataMessage, dataLength);
}

ErrorCodes_t EZPatchDevice::currStepTimeStep(Measurement_t i0, Measurement_t iStep, Measurement_t t0, Measurement_t tStep,
                                                 uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    i0.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
    iStep.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
    t0.convertValue(positiveProtocolTimeRange.prefix);
    tStep.convertValue(positiveProtocolTimeRange.prefix);

    if ((this->checkCurrStepTimeStepParameters(i0.value, iStep.value, t0.value, tStep.value, repsNum, applySteps) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            std::vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(i0.value/currentCommandResolution), txDataMessage, 0);
            this->int322uint16((int32_t)round(iStep.value/currentCommandResolution), txDataMessage, 2);
            this->int322uint16((int32_t)round(t0.value/positiveProtocolTimeRange.step), txDataMessage, 4);
            this->int322uint16((int32_t)round(tStep.value/positiveProtocolTimeRange.step), txDataMessage, 6);
            txDataMessage[8] = 0xF00D;
            txDataMessage[9] = 0xBAAD;
            txDataMessage[10] = 0xF00D;
            txDataMessage[11] = 0xBAAD;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdCurrentStepTimeStep, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::currRamp(Measurement_t i0, Measurement_t iFinal, Measurement_t t,
                                         uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    i0.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
    iFinal.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
    t.convertValue(positiveProtocolTimeRange.prefix);

    if ((this->checkCurrRampParameters(i0.value, iFinal.value, t.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            std::vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(i0.value/currentCommandResolution), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(iFinal.value/currentCommandResolution), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(t.value/positiveProtocolTimeRange.step), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdCurrentRamp, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::currSin(Measurement_t i0, Measurement_t iAmp, Measurement_t freq,
                                        uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    i0.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
    iAmp.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);
    freq.convertValue(positiveProtocolFrequencyRange.prefix);

    if ((this->checkCurrSinParameters(i0.value, iAmp.value, freq.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            std::vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(i0.value/currentCommandResolution), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(iAmp.value/currentCommandResolution), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(freq.value/positiveProtocolFrequencyRange.step), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdCurrentSin, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool iHalfFlag) {
    return this->currStepTimeStep(i0, i0Step, t0, t0Step, itemIdx, nextItemIdx, loopReps, applyStepsFlag);
}

ErrorCodes_t EZPatchDevice::setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool iHalfFlag) {
    return this->currRamp(i0, iFinal, t0, itemIdx, nextItemIdx, loopReps, applyStepsFlag);
}

ErrorCodes_t EZPatchDevice::setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool iHalfFlag) {
    return this->currSin(i0, iAmp, f0, itemIdx, nextItemIdx, loopReps, applyStepsFlag);
}

ErrorCodes_t EZPatchDevice::resetLiquidJunctionCompensation(bool reset) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    unsigned int resetIdx = ResetIndexLiquidJunctionCompensation;

    if (reset) {
        txDataMessage[resetWord[resetIdx]] |= resetByte[resetIdx];

    } else {
        txDataMessage[resetWord[resetIdx]] &= ~resetByte[resetIdx];
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status) {
    if (wordOffset >= switchesStatusLength) {
        return ErrorValueOutOfRange;
    }
    return this->singleSwitchDebug(wordOffset, bitOffset, status);
}

ErrorCodes_t EZPatchDevice::setDebugWord(uint16_t wordOffset, uint16_t wordValue) {
    return this->singleRegisterDebug(wordOffset, wordValue);
}

ErrorCodes_t EZPatchDevice::getSwitchesStatus(std::vector <uint16_t> &words, std::vector <std::vector <std::string> > &names) {
    words.resize(switchesStatusLength);
    names.resize(switchesStatusLength);
    switches2DataMessage(words);
    for (unsigned int wordIdx = 0; wordIdx < switchesStatusLength; wordIdx++) {
        names[wordIdx].resize(16);
        for (unsigned int stringIdx = 0; stringIdx < 16; stringIdx++) {
            names[wordIdx][stringIdx] = switchesNames[wordIdx][stringIdx];
        }
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::singleSwitchDebug(uint16_t word, uint16_t bit, bool flag) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    if (flag) {
        txDataMessage[word] |= (0x0001 << bit);

    } else {
        txDataMessage[word] &= ~(0x0001 << bit);
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::multiSwitchDebug(std::vector <uint16_t> words) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    /*! std::vector <uint16_t> txDataMessage(dataLength); not needed, because it is identical to words */

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, words, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(words);
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::singleRegisterDebug(uint16_t index, uint16_t value) {
    ErrorCodes_t ret;
    uint16_t dataLength = 2;
    std::vector <uint16_t> txDataMessage(dataLength);

    txDataMessage[0] = index;
    txDataMessage[1] = value;

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
    return ret;
}

/****************\
 *  Rx methods  *
\****************/

ErrorCodes_t EZPatchDevice::isDeviceUpgradable(std::string &upgradeNotes, std::string &notificationTag) {
    ErrorCodes_t ret = Success;

    upgradeNotes = this->upgradeNotes;
    notificationTag = this->notificationTag;
    if (upgradeNotes == "") {
        ret = ErrorUpgradesNotAvailable;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::getNextMessage(RxOutput_t &rxOutput, int16_t * data) {
    ErrorCodes_t ret = Success;
    double xFlt;

    std::unique_lock <std::mutex> rxMutexLock (rxMutex);
    if (rxMsgBufferReadLength == 0) {
        if (parsingStatus == ParsingNone) {
            gettingNextDataFlag = false;
            return ErrorDeviceNotConnected;
        }
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point currentTime = startTime;
        while (((std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count()) < EZP_NO_DATA_WAIT_TIME_MS) &&
               (rxMsgBufferReadLength == 0)) {
            rxMsgBufferNotEmpty.wait_for (rxMutexLock, std::chrono::milliseconds(EZP_NO_DATA_WAIT_TIME_MS));
            currentTime = std::chrono::steady_clock::now();
        }
        if (rxMsgBufferReadLength == 0) {
            return ErrorNoDataAvailable;
        }
    }
    uint32_t maxMsgRead = rxMsgBufferReadLength;
    if (maxMsgRead > RX_MSG_BUFFER_SIZE && rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataOverflow]) {
        rxOutput.dataLen = 2;
        rxOutput.msgTypeId = MsgDirectionDeviceToPc + MsgTypeIdAcquisitionDataOverflow;
        rxOutput.totalMessages = maxMsgRead;
        uint32_t skipped = maxMsgRead - RX_MSG_BUFFER_SIZE;
        rxMsgBufferReadOffset = (rxMsgBufferReadOffset + skipped) & RX_MSG_BUFFER_MASK;
        rxMsgBufferReadLength -= skipped;
        maxMsgRead = rxMsgBufferReadLength;
        data[0] = (int16_t)(skipped & (0xFFFF));
        data[1] = (int16_t)((skipped >> 16) & (0xFFFF));
        return Success;
    }
    gettingNextDataFlag = true;
    rxMutexLock.unlock();

    uint32_t msgReadCount = 0;
    bool headerSet = false;
    bool tailSet = false;

    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged */
    bool exitLoop = false;

    while (msgReadCount < maxMsgRead) {
        if (rxOutput.msgTypeId == rxMsgBuffer[rxMsgBufferReadOffset].typeId) {
            switch (rxOutput.msgTypeId) {
            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData):
                if ((msgReadCount+1)*rxDataMessageMaxLen > E384CL_OUT_STRUCT_DATA_LEN) {
                    /*! Exit if the merge may exceed the data field length */
                    exitLoop = true;

                } else {
                    exitLoop = false;
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdFpgaReset):
            case (MsgDirectionDeviceToPc+MsgTypeIdLiquidJunctionComp):
            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss):
            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataOverflow):
            default:
                /*! Messages of these types can't be merged. */
                if (msgReadCount == 0) {
                    /*! If no messages have been read yet process it... */
                    exitLoop = false;

                } else {
                    /*! otherwise exit and force the beginning of another packet */
                    exitLoop = true;
                }
                break;
            }

        } else {
            /*! If new message type is different from previous one stay and read only if no other messages have been read yet */
            if (msgReadCount > 0) {
                /*! Otherwise exit or messages of different types will be merged */
                exitLoop = true;
            }
        }

        if (exitLoop) {
            break;

        } else {
            rxOutput.msgTypeId = rxMsgBuffer[rxMsgBufferReadOffset].typeId;
            uint32_t dataOffset = rxMsgBuffer[rxMsgBufferReadOffset].startDataPtr;
            uint16_t samplesNum;
            uint16_t sampleIdx = 0;
            uint16_t timeSamplesNum;
            uint16_t rawFloat;
            uint16_t dataWritten;

            switch (rxOutput.msgTypeId) {
            case (MsgDirectionDeviceToPc+MsgTypeIdFpgaReset):
                lastParsedMsgType = MsgTypeIdFpgaReset;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdLiquidJunctionComp):
                rxOutput.dataLen = 1;
                rxOutput.channelIdx = * (rxDataBuffer+dataOffset);
                rawFloat = * (rxDataBuffer+((dataOffset+1)&EZP_RX_DATA_BUFFER_MASK));
                voltageOffsetCorrected = (((double)rawFloat)-liquidJunctionOffsetBinary)*liquidJunctionResolution;
                this->selectVoltageOffsetResolution();
                data[0] = (int16_t)(rawFloat-liquidJunctionOffsetBinary);
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;

//                this->setLiquidJunctionCompensationOverrideValue(rxOutput.channelIdx, {voltageOffsetCorrected, liquidJunctionControl.prefix, "V"}); /*! \todo FCON capire perchè stato messo questo comando durante il porting su e384commlib */
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
                if (lastParsedMsgType != MsgTypeIdAcquisitionHeader) {
                    /*! Evaluate only if it's the first repetition */
                    rxOutput.dataLen = 0;
                    rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                    rxOutput.protocolItemIdx = * (rxDataBuffer+((dataOffset+1)&EZP_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolRepsIdx = * (rxDataBuffer+((dataOffset+2)&EZP_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolSweepIdx = * (rxDataBuffer+((dataOffset+3)&EZP_RX_DATA_BUFFER_MASK));
                    lastParsedMsgType = MsgTypeIdAcquisitionHeader;
                    headerSet = true;

                } else {
                    if (!headerSet) {
                        /*! This message is a repetition and should not be passed to SW only if the first header message was not processed (headerSet) during this call */
                        ret = ErrorRepeatedHeader;
                    }
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData):
                if (rxMsgBuffer[rxMsgBufferReadOffset].dataLength < EZP_RX_MIN_DATA_PACKET_VALID_LEN) {
                    ret = ErrorIllFormedMessage;

                } else {
                    samplesNum = rxMsgBuffer[rxMsgBufferReadOffset].dataLength-2; /*!< The first 2 samples are used for firstSampleOffset */
                    dataWritten = rxOutput.dataLen;
                    rxOutput.dataLen += samplesNum;
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    if (msgReadCount == 0) {
                        /*! Update firstSampleOffset only if it is not merging messages */
                        rxOutput.firstSampleOffset = * ((uint32_t *)(rxDataBuffer+dataOffset));
                    }
                    dataOffset = (dataOffset+2)&EZP_RX_DATA_BUFFER_MASK;

                    for (uint16_t idx = 0; idx < timeSamplesNum; idx++) {
                        /*! \todo FCON questo doppio ciclo va modificato per raccogliere i dati di impedenza in modalità lock-in */
                        for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyRawDataFilter(channelIdx, (((double)rawFloat)-SHORT_OFFSET_BINARY)+(voltageOffsetCorrection+voltageTunerCorrection[channelIdx])/voltageResolution, iirVNum, iirVDen);
                            xFlt = iirY[channelIdx][iirOff];
                            data[dataWritten+sampleIdx] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;

                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyRawDataFilter(channelIdx+voltageChannelsNum, (((double)rawFloat)-SHORT_OFFSET_BINARY)+currentTunerCorrection[channelIdx]/currentResolution, iirINum, iirIDen);
                            xFlt = iirY[channelIdx+voltageChannelsNum][iirOff];
                            data[dataWritten+sampleIdx+voltageChannelsNum] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;

                            if (computeCurrentOffsetFlag) {
                                liquidJunctionCurrentSums[channelIdx] += (int64_t)data[dataWritten+sampleIdx+voltageChannelsNum];
                            }

                            sampleIdx++;
                        }

                        if (computeCurrentOffsetFlag) {
                            liquidJunctionCurrentEstimatesNum++;
                        }
                        sampleIdx += currentChannelsNum;

                        if (iirOff < 1) {
                            iirOff = IIR_ORD;

                        } else {
                            iirOff--;
                        }
                    }
                }
                lastParsedMsgType = MsgTypeIdAcquisitionData;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
                if (lastParsedMsgType != MsgTypeIdAcquisitionTail) {
                    /*! Evaluate only if it's the first repetition */
                    rxOutput.dataLen = 0;
                    rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                    lastParsedMsgType = MsgTypeIdAcquisitionTail;
                    tailSet = true;

                } else {
                    if (!tailSet) {
                        /*! This message is a repetition and should not be passed to SW only if the first tail message was not processed (tailSet) during this call */
                        ret = ErrorRepeatedTail;
                    }
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
                rxOutput.dataLen = rxMsgBuffer[rxMsgBufferReadOffset].dataLength;
                for (uint16_t dataIdx = 0; dataIdx < rxOutput.dataLen; dataIdx++) {
                    rxOutput.dataLen = 1;
//                    rxOutput.uintData[dataIdx] = * (rxDataBuffer+dataOffset); /* \todo FCON al momento della saturazione non frega niente a nessuno */
                    dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;
                }
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss):
                if (lastParsedMsgType == MsgDirectionDeviceToPc+MsgTypeIdInvalid) {
                    lastParsedMsgType = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss;
                    rxOutput.dataLen = 2;
                    data[0] = (int16_t)rxDataBuffer[dataOffset];
                    data[1] = (int16_t)rxDataBuffer[(dataOffset+1) & EZP_RX_DATA_BUFFER_MASK];
                    dataOffset = (dataOffset+2)&EZP_RX_DATA_BUFFER_MASK;

                    lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;
                }
                break;

            default:
                lastParsedMsgType = MsgTypeIdInvalid;
                break;
            }

            msgReadCount++;
            rxMsgBufferReadOffset = (rxMsgBufferReadOffset+1)&EZP_RX_MSG_BUFFER_MASK;
        }
    }

    rxMutexLock.lock();
    rxMsgBufferReadLength -= msgReadCount;
    gettingNextDataFlag = false;
    rxMsgBufferNotFull.notify_all();
    rxMutexLock.unlock();

    return ret;
}

ErrorCodes_t EZPatchDevice::purgeData() {
    ErrorCodes_t ret = Success;

    std::unique_lock <std::mutex> rxMutexLock(rxMutex);
    while (gettingNextDataFlag) {
        /*! Wait for the getNextMessage method to be outside of its loop, because the rxMsgBufferReadLength and rxMsgBufferReadOffset variables must not change during its execution */
        rxMsgBufferNotFull.wait_for (rxMutexLock, std::chrono::milliseconds(1));
    }
    rxMsgBufferReadOffset = (rxMsgBufferReadOffset+rxMsgBufferReadLength) & RX_MSG_BUFFER_MASK;
    rxMsgBufferReadLength = 0;
    rxMutexLock.unlock();
    rxMsgBufferNotFull.notify_all();

    return ret;
}

ErrorCodes_t EZPatchDevice::hasChannelSwitches() {
    if (inputSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::hasCalSw() {
    if (calSwitchesNum == 0) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::hasVoltageHoldTuner() {
    if (!voltageHoldTunerImplemented) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageRanges) {
    if (!voltageHoldTunerImplemented) {
        return ErrorFeatureNotImplemented;
    }
    voltageRanges = vcVoltageRangesArray;
    return Success;
}

ErrorCodes_t EZPatchDevice::hasCurrentHoldTuner() {
    if (!currentHoldTunerImplemented) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentRanges) {
    if (!currentHoldTunerImplemented) {
        return ErrorFeatureNotImplemented;
    }
    currentRanges = ccCurrentRangesArray;
    return Success;
}

ErrorCodes_t EZPatchDevice::getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges) {
    if (!voltageHoldTunerImplemented) {
        return ErrorFeatureNotImplemented;
    }
    ranges = vcVoltageRangesArray;
    return Success;
}

ErrorCodes_t EZPatchDevice::getMaxOutputTriggers(unsigned int &maxTriggersNum) {
    maxTriggersNum = maxDigitalTriggerOutputEvents;
    return Success;
}

ErrorCodes_t EZPatchDevice::getOutputTriggersNum(unsigned int &triggersNum) {
    triggersNum = digitalTriggersNum;
    return Success;
}

ErrorCodes_t EZPatchDevice::hasRepetitiveTrigger() {
    if (repetitiveTriggerAvailableFlag) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::multimeterStuckHazard(bool &stuckFlag) {
    stuckFlag = multimeterStuckHazardFlag;
    return Success;
}

ErrorCodes_t EZPatchDevice::getLedsNumber(uint16_t &ledsNumber) {
    ledsNumber = ledsNum;
    return Success;
}

ErrorCodes_t EZPatchDevice::getLedsColors(std::vector <uint32_t> &ledsColors) {
    ledsColors = ledsColorsArray;
    return Success;
}

ErrorCodes_t EZPatchDevice::hasAnalogOut() {
    if (analogOutImplementedFlag) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::hasSlaveModality() {
    if (!slaveImplementedFlag) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::hasOffsetCompensation() {
    if (!voltageHoldTunerImplemented) {
        return ErrorFeatureNotImplemented;
    }
    return Success;
}

ErrorCodes_t EZPatchDevice::hasPipetteCompensation() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (pipetteCompensationImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::hasCCPipetteCompensation() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (ccPipetteCompensationImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::hasMembraneCompensation() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (membraneCompensationImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::hasResistanceCompensation() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistanceCompensationImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::hasResistanceCorrection() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistanceCorrectionImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::hasResistancePrediction() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistancePredictionImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::hasLeakConductanceCompensation() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (leakConductanceCompensationImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::hasBridgeBalanceCompensation() {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (bridgeBalanceCompensationImplemented) {
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getCompOptionsFeatures(CompensationTypes_t type, std::vector <std::string> &options) {
    switch (type) {
    case CompCfast:
        if (pipetteCompensationOptions.size() > 0) {
            options = pipetteCompensationOptions;
            return Success;
        }
        break;

    case CompCslow:
        if (membraneCompensationOptions.size() > 0) {
            options = membraneCompensationOptions;
            return Success;
        }
        break;

    case CompRsComp:
        if (resistanceCompensationOptions.size() > 0) {
            options = resistanceCompensationOptions;
            return Success;
        }
        break;

    case CompRsCorr:
        if (resistanceCorrectionOptions.size() > 0) {
            options = resistanceCorrectionOptions;
            return Success;
        }
        break;

    case CompRsPred:
        if (resistancePredictionOptions.size() > 0) {
            options = resistancePredictionOptions;
            return Success;
        }
        break;

    case CompGLeak:
        if (leakConductanceCompensationOptions.size() > 0) {
            options = leakConductanceCompensationOptions;
            return Success;
        }
        break;

    case CompCcCfast:
        if (ccPipetteCompensationOptions.size() > 0) {
            options = ccPipetteCompensationOptions;
            return Success;

        }
        break;

    case CompBridgeRes:
        if (bridgeBalanceCompensationOptions.size() > 0) {
            options = bridgeBalanceCompensationOptions;
            return Success;
        }
        break;
    }
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t EZPatchDevice::getLiquidJunctionControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (liquidJunctionControl.implemented) {
        control = liquidJunctionControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (compensationControls[param][compensationsSettingChannel].implemented) {
        control = compensationControls[param][compensationsSettingChannel];
        ret = Success;
    }
    return ret;
}

/*********************\
 *  Private methods  *
\*********************/

ErrorCodes_t EZPatchDevice::initialize(std::string fwPath) {
    this->createDebugFiles();

    ErrorCodes_t ret = this->startCommunication(fwPath);
    if (ret != Success) {
        return ret;
    }

    ret = this->initializeMemory();
    if (ret != Success) {
        return ret;
    }

    stopConnectionFlag = false;
    this->createCommunicationThreads();

    this->initializeVariables();

    ret = this->deviceConfiguration();
    if (ret != Success) {
        return ret;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    ret = this->initializeHW();
    if (ret != Success) {
        return ret;
    }

//    send a protocol to force the device to send data
    const Measurement m0 = {0, UnitPfx::UnitPfxMilli, "V"};
    const Measurement t0 = {100, UnitPfx::UnitPfxKilo, "s"};
    const Measurement t1 = {0, UnitPfx::UnitPfxTera, "s"};
    setVoltageProtocolStructure(-1, 2, 1, m0, true);
    setVoltageProtocolStep(0, 1, 0, false, m0, m0, t0, t1, false);
    ret = setVoltageProtocolStep(1, 0, 0, false, m0, m0, t0, t1, false);
    if (startProtocolCommandImplemented) {
        return startProtocol();

    } else {
        return ret;
    }
}

void EZPatchDevice::deinitialize() {
    stopConnectionFlag = true;
    this->joinCommunicationThreads();

    this->deinitializeVariables();

    this->deinitializeMemory();

    this->stopCommunication();

    this->closeDebugFiles();
}

ErrorCodes_t EZPatchDevice::initializeMemory() {
    rxMsgBuffer = new (std::nothrow) MsgResume_t[EZP_RX_MSG_BUFFER_SIZE];
    if (rxMsgBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    rxDataBuffer = new (std::nothrow) uint16_t[EZP_RX_DATA_BUFFER_SIZE+1]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word */
    if (rxDataBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txMsgBuffer = new (std::nothrow) MsgResume_t[EZP_TX_MSG_BUFFER_SIZE];
    if (txMsgBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txDataBuffer = new (std::nothrow) uint16_t[EZP_TX_DATA_BUFFER_SIZE+1]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word */
    if (txDataBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    /*! Allocate memory for holding values to correct readout from device */
    voltageTunerCorrection.resize(currentChannelsNum);
    std::fill(voltageTunerCorrection.begin(), voltageTunerCorrection.end(), 0.0);
    currentTunerCorrection.resize(voltageChannelsNum);
    std::fill(currentTunerCorrection.begin(), currentTunerCorrection.end(), 0.0);

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

void EZPatchDevice::initializeVariables() {
    MessageDispatcher::initializeVariables();

    /*! Allocate memory for compensations */
    this->initializeCompensations();
}

ErrorCodes_t EZPatchDevice::deviceConfiguration() {
    std::this_thread::sleep_for (std::chrono::milliseconds(10));
    ErrorCodes_t ret = MessageDispatcher::deviceConfiguration();
    if (ret != Success) {
        return ret;
    }

    ret = this->setConstantSwitches();
    if (ret != Success) {
        return ret;
    }

    this->setSecondaryDeviceswitch(false);

    std::vector <uint16_t> ch(currentChannelsNum);
    std::vector <Measurement_t> offsetsA(currentChannelsNum);
    std::vector <Measurement_t> offsetsV(currentChannelsNum);
    Measurement_t zeroA = {0.0, UnitPfxNone, "A"};
    Measurement_t zeroV = {0.0, UnitPfxNone, "V"};

    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        ch[idx] = idx;
        offsetsA[idx] = zeroA;
        offsetsV[idx] = zeroV;
    }
    this->setCalibVcCurrentOffset(ch, offsetsA, true);
    this->setCalibCcVoltageOffset(ch, offsetsV, true);

    return this->setConstantRegisters();
}

void EZPatchDevice::createCommunicationThreads() {
    rxThread = std::thread(&EZPatchDevice::readAndParseMessages, this);
    txThread = std::thread(&EZPatchDevice::unwrapAndSendMessages, this);
    liquidJunctionThread = std::thread(&EZPatchDevice::computeLiquidJunction, this);

    threadsStarted = true;
}

ErrorCodes_t EZPatchDevice::initializeHW() {
    RxOutput_t rxOutput;
    ErrorCodes_t ret = ErrorUnknown;
    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;
    int pingTries = 0;
    int fpgaResetTries = 0;

    int16_t * datain;
    this->allocateRxDataBuffer(datain);

    while (ret != Success) {
        if (pingTries++ > EZP_MAX_PING_TRIES) {
            return ErrorConnectionPingFailed;
        }

        this->resetFpga();
        while (rxOutput.msgTypeId != MsgDirectionDeviceToPc+MsgTypeIdFpgaReset) {
            ret = this->getNextMessage(rxOutput, datain);
            if (ret == ErrorNoDataAvailable) { /*! If another valid packet is obtained it doesn't count as a fail, only if no packets are available it is in fact a retry */
                if (fpgaResetTries++ > EZP_MAX_FPGA_RESET_TRIES) {
                    return ErrorConnectionFpgaResetFailed;
                }

#ifdef DEBUG_RX_DATA_PRINT
                fprintf(rxFid,
                        "fpga reset\n"
                        "try %d: %s\n\n",
                        fpgaResetTries,
                        ((rxOutput.msgTypeId == MsgDirectionDeviceToPc+MsgTypeIdFpgaReset) && (ret == Success) ? "success" : "fail"));
                fflush(rxFid);
#endif

            }
        }
        fpgaResetTries = 0;

        ret = this->ping();

#ifdef DEBUG_RX_DATA_PRINT
        fprintf(rxFid,
                "ping\n"
                "try %d: %s\n\n",
                pingTries,
                (ret == Success ? "success" : "fail"));
        fflush(rxFid);
#endif
    }
    this->deallocateRxDataBuffer(datain);

    std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeBeforeAsicResetMs));

    ret = this->resetAsic(true, true);
    if (ret != Success) {
        return ErrorConnectionChipResetFailed;
    }

    this->stopProtocol(); /*! Force a write of the ASIC's SPI, so that the reset is sent to the secondary board in case of multi board devices */

    std::this_thread::sleep_for(std::chrono::milliseconds(resetDurationMs));

    ret = this->resetAsic(false, true);
    if (ret != Success) {
        return ErrorConnectionChipResetFailed;
    }

    this->stopProtocol(); /*! Force a write of the ASIC's SPI, so that the reset is sent to the secondary board in case of multi board devices */

    return ret;
}

void EZPatchDevice::deinitializeMemory() {
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

    if (txDataBuffer != nullptr) {
        delete [] txDataBuffer;
        txDataBuffer = nullptr;
    }
}

void EZPatchDevice::deinitializeVariables() {
    /*! Nothing to be done */
    MessageDispatcher::deinitializeVariables();
    this->deinitializeCalibration();
}

void EZPatchDevice::joinCommunicationThreads() {
    if (threadsStarted) {
        rxThread.join();
        txThread.join();
        liquidJunctionThread.join();

        threadsStarted = false;
    }
}

void EZPatchDevice::initializeCompensations() {
    for (uint16_t compensationIdx = 0; compensationIdx < CompensationTypesNum; compensationIdx++) {
        /*! Assuming patch devices always have the same number of voltage and current channels */
        compensationsEnabledArray[compensationIdx].resize(currentChannelsNum);
        for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            compensationsEnabledArray[compensationIdx][channelIdx] = false;
        }
    }

    vcCompensationsActivated = false;
    ccCompensationsActivated = false;

    compensationControls[U_CpVc].resize(currentChannelsNum);
    compensationControls[U_CpCc].resize(currentChannelsNum);
    compensationControls[U_Cm].resize(currentChannelsNum);
    compensationControls[U_Rs].resize(currentChannelsNum);
    compensationControls[U_RsCp].resize(currentChannelsNum);
    compensationControls[U_RsCl].resize(currentChannelsNum);
    compensationControls[U_RsPg].resize(currentChannelsNum);
    compensationControls[U_RsPp].resize(currentChannelsNum);
    compensationControls[U_RsPt].resize(currentChannelsNum);
    compensationControls[U_LkG].resize(currentChannelsNum);
    compensationControls[U_BrB].resize(currentChannelsNum);

    compensationsEnableFlags[CompCfast].resize(currentChannelsNum);
    compensationsEnableFlags[CompCcCfast].resize(currentChannelsNum);
    compensationsEnableFlags[CompCslow].resize(currentChannelsNum);
    compensationsEnableFlags[CompRsComp].resize(currentChannelsNum);
    compensationsEnableFlags[CompRsCorr].resize(currentChannelsNum);
    compensationsEnableFlags[CompRsPred].resize(currentChannelsNum);
    compensationsEnableFlags[CompGLeak].resize(currentChannelsNum);
    compensationsEnableFlags[CompBridgeRes].resize(currentChannelsNum);

    std::fill(compensationsEnableFlags[CompCfast].begin(), compensationsEnableFlags[CompCfast].end(), false);
    std::fill(compensationsEnableFlags[CompCcCfast].begin(), compensationsEnableFlags[CompCcCfast].end(), false);
    std::fill(compensationsEnableFlags[CompCslow].begin(), compensationsEnableFlags[CompCslow].end(), false);
    std::fill(compensationsEnableFlags[CompRsComp].begin(), compensationsEnableFlags[CompRsComp].end(), false);
    std::fill(compensationsEnableFlags[CompRsCorr].begin(), compensationsEnableFlags[CompRsCorr].end(), false);
    std::fill(compensationsEnableFlags[CompRsPred].begin(), compensationsEnableFlags[CompRsPred].end(), false);
    std::fill(compensationsEnableFlags[CompGLeak].begin(), compensationsEnableFlags[CompGLeak].end(), false);
    std::fill(compensationsEnableFlags[CompBridgeRes].begin(), compensationsEnableFlags[CompBridgeRes].end(), false);

    pipetteCompensationOptionReg.resize(currentChannelsNum);
    ccPipetteCompensationOptionReg.resize(currentChannelsNum);
    membraneCompensationOptionReg.resize(currentChannelsNum);
    resistanceCompensationOptionReg.resize(currentChannelsNum);
    resistanceCorrectionOptionReg.resize(currentChannelsNum);
    resistancePredictionOptionReg.resize(currentChannelsNum);
    leakConductanceCompensationOptionReg.resize(currentChannelsNum);
    bridgeBalanceCompensationOptionReg.resize(currentChannelsNum);

    std::fill(pipetteCompensationOptionReg.begin(), pipetteCompensationOptionReg.end(), 0);
    std::fill(ccPipetteCompensationOptionReg.begin(), ccPipetteCompensationOptionReg.end(), 0);
    std::fill(membraneCompensationOptionReg.begin(), membraneCompensationOptionReg.end(), 0);
    std::fill(resistanceCompensationOptionReg.begin(), resistanceCompensationOptionReg.end(), 0);
    std::fill(resistanceCorrectionOptionReg.begin(), resistanceCorrectionOptionReg.end(), 0);
    std::fill(resistancePredictionOptionReg.begin(), resistancePredictionOptionReg.end(), 0);
    std::fill(leakConductanceCompensationOptionReg.begin(), leakConductanceCompensationOptionReg.end(), 0);
    std::fill(bridgeBalanceCompensationOptionReg.begin(), bridgeBalanceCompensationOptionReg.end(), 0);
}

ErrorCodes_t EZPatchDevice::manageOutgoingMessageLife(uint16_t msgTypeId, std::vector <uint16_t> txDataMessage, uint16_t dataLen) {
    while (dataLen > txDataMessageMaxLen) {
        std::vector <uint16_t> txDataMessageMax(txDataMessage.begin(), txDataMessage.begin()+txDataMessageMaxLen);
        this->manageOutgoingMessageLife(msgTypeId, txDataMessageMax, txDataMessageMaxLen);
        txDataMessage.erase(txDataMessage.begin(), txDataMessage.begin()+txDataMessageMaxLen);
        dataLen -= txDataMessageMaxLen;
    }

    ErrorCodes_t ret = ErrorSendMessageFailed;
    bool txExpectAck = txExpectAckMap[msgTypeId];
    if (txExpectAck) {
        txWaitingOnAcks++;
    }
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;

    if (!txExpectAck) {
        this->wrapOutgoingMessage(msgTypeId, txDataMessage, dataLen);
        ret = Success;

    } else if (threadsStarted) {
        std::unique_lock <std::mutex> ackLock(txAckMutex);
        txAckReceived = false;
        int resendTry = 0;
        while ((!txAckReceived) && (resendTry < EZP_MAX_RESEND_TRIES)) {
            ackLock.unlock();

            this->wrapOutgoingMessage(msgTypeId, txDataMessage, dataLen);

            startTime = std::chrono::steady_clock::now();
            currentTime = startTime;
            ackLock.lock();
            while (!txAckReceived && (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count()) < EZP_ACK_WAIT_TIME_MS) {
                txAckCv.wait_for (ackLock, std::chrono::milliseconds(EZP_ACK_WAIT_TIME_MS));
                currentTime = std::chrono::steady_clock::now();
            }
            if (txAckReceived) {
                txWaitingOnAcks--;
                ret = Success;

            } else {
                resendTry++;
#ifdef DEBUG_TX_DATA_PRINT
                fprintf(txFid, "resend\n\n");
                fflush(txFid);
#endif
            }
        }

        if (!txAckReceived) {
            txWaitingOnAcks--;
        }
        ackLock.unlock();

    } else {
        this->wrapOutgoingMessage(msgTypeId, txDataMessage, dataLen);
        txWaitingOnAcks--;
        ret = Success;
    }
    return ret;
}

void EZPatchDevice::int322uint16(int32_t from, std::vector <uint16_t> &to, size_t offset) {
    to[offset] = from & 0xFFFF;
    to[offset+1] = (from >> 16) & 0xFFFF;
}

void EZPatchDevice::uint322uint16(uint32_t from, std::vector <uint16_t> &to, size_t offset) {
    to[offset] = from & 0xFFFF;
    to[offset+1] = (from >> 16) & 0xFFFF;
}

ErrorCodes_t EZPatchDevice::setLiquidJunctionCompensationOverrideSwitch (bool flag) {
    ErrorCodes_t ret;

    if (liquidJunctionCompensationOverrideImplemented) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (flag) {
            txDataMessage[liquidJunctionCompensationOverrideSwitchWord] |= liquidJunctionCompensationOverrideSwitchByte;

        } else {
            txDataMessage[liquidJunctionCompensationOverrideSwitchWord] &= ~liquidJunctionCompensationOverrideSwitchByte;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setSecondaryDeviceswitch (bool flag) {
    ErrorCodes_t ret;

    if (startProtocolCommandImplemented) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (flag) {
            txDataMessage[startProtocolSwitchWord] |= startProtocolSwitchByte;

        } else {
            txDataMessage[startProtocolSwitchWord] &= ~startProtocolSwitchByte;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setLiquidJunctionCompensationOverrideValue(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    value.convertValue(liquidJunctionControl.prefix);
    uint16_t dataLength = 2;
    std::vector <uint16_t> txDataMessage(dataLength);

    txDataMessage[0] = liquidJunctionCompensationOverrideRegisterOffset+channelIdx*coreSpecificRegistersNum;
    /*! In some devices (EL04e/f based devices) we need to keep some LSBs to 0, because the resolution of the lj is just too small, a few uV */
    uint16_t uintValue = (uint16_t)((int16_t)round(-value.value/liquidJunctionResolution/liquidJunctionRounding)*liquidJunctionRounding);
    txDataMessage[1] = uintValue;

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

    return ret;
}

bool EZPatchDevice::checkVoltStepTimeStepParameters(double v0, double vStep, double t0, double tStep, uint16_t repsNum, uint16_t applySteps) {
    double minVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].min-voltageCommandResolution*0.5;
    double maxVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].max+voltageCommandResolution*0.5;
    double minTWithMargin = positiveProtocolTimeRange.min-positiveProtocolTimeRange.step*0.5;
    double maxTWithMargin = positiveProtocolTimeRange.max+positiveProtocolTimeRange.step*0.5;
    double factor = stepsOnLastSweep+(applySteps == 1 ? (double)(repsNum-1) : 0.0);
    if (v0 < minVWithMargin ||
            v0+factor*vStep < minVWithMargin ||
            v0 > maxVWithMargin ||
            v0+factor*vStep > maxVWithMargin ||
            t0 > maxTWithMargin ||
            t0+factor*tStep > maxTWithMargin ||
            t0 < minTWithMargin ||
            t0+factor*tStep < minTWithMargin) {
        return false;

    } else {
        return true;
    }
}

bool EZPatchDevice::checkVoltRampParameters(double v0, double vFinal, double t) {
    double minVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].min-voltageCommandResolution*0.5;
    double maxVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].max+voltageCommandResolution*0.5;
    double minTWithMargin = positiveProtocolTimeRange.min-positiveProtocolTimeRange.step*0.5;
    double maxTWithMargin = positiveProtocolTimeRange.max+positiveProtocolTimeRange.step*0.5;
    if (v0 < minVWithMargin ||
            vFinal < minVWithMargin ||
            v0 > maxVWithMargin ||
            vFinal > maxVWithMargin ||
            t > maxTWithMargin ||
            t < minTWithMargin) {
        return false;

    } else {
        return true;
    }
}

bool EZPatchDevice::checkVoltSinParameters(double v0, double vAmp, double freq) {
    double minVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].min-voltageCommandResolution*0.5;
    double maxVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].max+voltageCommandResolution*0.5;
    double minFWithMargin = positiveProtocolFrequencyRange.min-positiveProtocolFrequencyRange.step*0.5;
    double maxFWithMargin = positiveProtocolFrequencyRange.max+positiveProtocolFrequencyRange.step*0.5;
    if (v0-vAmp < minVWithMargin ||
            v0+vAmp > maxVWithMargin ||
            vAmp < 0.0 ||
            freq < minFWithMargin ||
            freq > maxFWithMargin) {
        return false;

    } else {
        return true;
    }
}

bool EZPatchDevice::checkCurrStepTimeStepParameters(double i0, double iStep, double t0, double tStep, uint16_t repsNum, uint16_t applySteps) {
    double minIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].min-currentCommandResolution*0.5;
    double maxIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].max+currentCommandResolution*0.5;
    double minTWithMargin = positiveProtocolTimeRange.min-positiveProtocolTimeRange.step*0.5;
    double maxTWithMargin = positiveProtocolTimeRange.max+positiveProtocolTimeRange.step*0.5;
    double factor = stepsOnLastSweep+(applySteps == 1 ? (double)(repsNum-1) : 0.0);
    if (i0 < minIWithMargin ||
            i0+factor*iStep < minIWithMargin ||
            i0 > maxIWithMargin ||
            i0+factor*iStep > maxIWithMargin ||
            t0 > maxTWithMargin ||
            t0+factor*tStep > maxTWithMargin ||
            t0 < minTWithMargin ||
            t0+factor*tStep < minTWithMargin) {
        return false;

    } else {
        return true;
    }
}

bool EZPatchDevice::checkCurrRampParameters(double i0, double iFinal, double t) {
    double minIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].min-currentCommandResolution*0.5;
    double maxIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].max+currentCommandResolution*0.5;
    double minTWithMargin = positiveProtocolTimeRange.min-positiveProtocolTimeRange.step*0.5;
    double maxTWithMargin = positiveProtocolTimeRange.max+positiveProtocolTimeRange.step*0.5;
    if (i0 < minIWithMargin ||
            iFinal < minIWithMargin ||
            i0 > maxIWithMargin ||
            iFinal > maxIWithMargin ||
            t > maxTWithMargin ||
            t < minTWithMargin) {
        return false;

    } else {
        return true;
    }
}

bool EZPatchDevice::checkCurrSinParameters(double i0, double iAmp, double freq) {
    double minIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].min-currentCommandResolution*0.5;
    double maxIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].max+currentCommandResolution*0.5;
    double minFWithMargin = positiveProtocolFrequencyRange.min-positiveProtocolFrequencyRange.step*0.5;
    double maxFWithMargin = positiveProtocolFrequencyRange.max+positiveProtocolFrequencyRange.step*0.5;
    if (i0-iAmp < minIWithMargin ||
            i0+iAmp > maxIWithMargin ||
            iAmp < 0.0 ||
            freq < minFWithMargin ||
            freq > maxFWithMargin) {
        return false;

    } else {
        return true;
    }
}

void EZPatchDevice::switches2DataMessage(std::vector <uint16_t> &txDataMessage) {
    for (uint16_t idx = 0; idx < switchesStatusLength; idx++) {
        txDataMessage[idx] = switchesStatus[idx];
    }
}

void EZPatchDevice::dataMessage2Switches(std::vector <uint16_t> &txDataMessage) {
    for (uint16_t idx = 0; idx < switchesStatusLength; idx++) {
        switchesStatus[idx] = txDataMessage[idx];
    }
}

void EZPatchDevice::updateCompensations() {
    uint16_t dataLength = compensationsRegistersNum*2;
    std::vector <uint16_t> txDataMessage(dataLength);

    this->checkCompensationsValues();
    if (this->fillCompensationsRegistersTxData(txDataMessage)) {
        if (this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength) == Success) {
            this->updateWrittenCompensationValues(txDataMessage);
        }

        /*! \todo FCON qui volendo si potrebbe fare il backtrace anche del comando degli switch/current range
                       nel caso in cui l'ultima scrittura non vada a buon fine */
    }
}

ErrorCodes_t EZPatchDevice::turnCompensationsOn(bool &flag, bool on) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    bool originalFlag = flag;
    flag = on;
    this->compensationsFlags2Switches(txDataMessage);

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
        this->updateCompensations();

    } else {
        flag = originalFlag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnCompensationOn(std::vector <bool> &flag, bool on) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    bool originalFlag = flag[compensationsSettingChannel];
    flag[compensationsSettingChannel] = on;
    this->compensationsFlags2Switches(txDataMessage);

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
        this->updateCompensations();

    } else {
        flag[compensationsSettingChannel] = originalFlag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setCompensationsOptions() {
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);
    uint16_t msgIdx;

    if (pipetteCompensationOptions.size() > 0) {
        msgIdx = pipetteCompensationOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~pipetteCompensationOptionMask) | pipetteCompensationOptionReg[compensationsSettingChannel];
    }

    if (ccPipetteCompensationOptions.size() > 0) {
        msgIdx = ccPipetteCompensationOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~ccPipetteCompensationOptionMask) | ccPipetteCompensationOptionReg[compensationsSettingChannel];
    }

    if (membraneCompensationOptions.size() > 0) {
        msgIdx = membraneCompensationOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~membraneCompensationOptionMask) | membraneCompensationOptionReg[compensationsSettingChannel];
    }

    if (resistanceCompensationOptions.size() > 0) {
        msgIdx = resistanceCompensationOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~resistanceCompensationOptionMask) | resistanceCompensationOptionReg[compensationsSettingChannel];
    }

    if (resistanceCorrectionOptions.size() > 0) {
        msgIdx = resistanceCorrectionOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~resistanceCorrectionOptionMask) | resistanceCorrectionOptionReg[compensationsSettingChannel];
    }

    if (resistancePredictionOptions.size() > 0) {
        msgIdx = resistancePredictionOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~resistancePredictionOptionMask) | resistancePredictionOptionReg[compensationsSettingChannel];
    }

    if (leakConductanceCompensationOptions.size() > 0) {
        msgIdx = leakConductanceCompensationOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~leakConductanceCompensationOptionMask) | leakConductanceCompensationOptionReg[compensationsSettingChannel];
    }

    if (bridgeBalanceCompensationOptions.size() > 0) {
        msgIdx = bridgeBalanceCompensationOptionWord+compensationsSettingChannel*coreSpecificSwitchesWordsNum;
        txDataMessage[msgIdx] = (txDataMessage[msgIdx] & ~bridgeBalanceCompensationOptionMask) | bridgeBalanceCompensationOptionReg[compensationsSettingChannel];
    }

    return this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
}

ErrorCodes_t EZPatchDevice::setCompensationValue(CompensationControl_t &control, Measurement_t newValue) {
    ErrorCodes_t ret;

    if (!control.implemented) {
        return ErrorFeatureNotImplemented;
    }
    newValue.convertValue(control.prefix);
    double originalValue = control.value;
    control.value = newValue.value;

    if (!(this->checkCompensationsValues())) {
        return ErrorValueOutOfRange;
    }
    uint16_t dataLength = compensationsRegistersNum*2;
    std::vector <uint16_t> txDataMessage(dataLength);
    if (!(this->fillCompensationsRegistersTxData(txDataMessage))) {
        /*! Uncheanged value */
        return Success;
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->updateWrittenCompensationValues(txDataMessage);

    } else {
        control.value = originalValue;
    }

    return ret;
}

void EZPatchDevice::compensationsFlags2Switches(std::vector <uint16_t> &txDataMessage) {
    if (vcCompensationsActivated) {
        /*! In voltage clamp apply CC compensations first, because if there's any flag in common between VC and CC (e.g. pipette compensation) CC disables it for sure, while VC might overwrite it and enable it if needed */
        for (unsigned int compensationSwitchIdx = 0; compensationSwitchIdx < compensationsSwitchesNum; compensationSwitchIdx++) {
            if (compensationsSwitchesLut[compensationSwitchIdx] >= VCCompensationsNum) {
                if (compensationsEnabledArray[compensationsSwitchesLut[compensationSwitchIdx]][compensationsSettingChannel] == compensationsSwitchesEnableSignArray[compensationSwitchIdx]) {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] |=
                            compensationsSwitchesByte[compensationSwitchIdx]; // 1

                } else {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] &=
                            ~compensationsSwitchesByte[compensationSwitchIdx]; // 0
                }
            }
        }

        for (unsigned int compensationSwitchIdx = 0; compensationSwitchIdx < compensationsSwitchesNum; compensationSwitchIdx++) {
            if (compensationsSwitchesLut[compensationSwitchIdx] < VCCompensationsNum) {
                if (compensationsEnabledArray[compensationsSwitchesLut[compensationSwitchIdx]][compensationsSettingChannel] == compensationsSwitchesEnableSignArray[compensationSwitchIdx]) {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] |=
                            compensationsSwitchesByte[compensationSwitchIdx]; // 1

                } else {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] &=
                            ~compensationsSwitchesByte[compensationSwitchIdx]; // 0
                }
            }
        }

    } else {
        for (unsigned int compensationSwitchIdx = 0; compensationSwitchIdx < compensationsSwitchesNum; compensationSwitchIdx++) {
            if (compensationsSwitchesLut[compensationSwitchIdx] < VCCompensationsNum) {
                if (compensationsEnabledArray[compensationsSwitchesLut[compensationSwitchIdx]][compensationsSettingChannel] == compensationsSwitchesEnableSignArray[compensationSwitchIdx]) {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] |=
                            compensationsSwitchesByte[compensationSwitchIdx]; // 1

                } else {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] &=
                            ~compensationsSwitchesByte[compensationSwitchIdx]; // 0
                }
            }
        }

        for (unsigned int compensationSwitchIdx = 0; compensationSwitchIdx < compensationsSwitchesNum; compensationSwitchIdx++) {
            if (compensationsSwitchesLut[compensationSwitchIdx] >= VCCompensationsNum) {
                if (compensationsEnabledArray[compensationsSwitchesLut[compensationSwitchIdx]][compensationsSettingChannel] == compensationsSwitchesEnableSignArray[compensationSwitchIdx]) {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] |=
                            compensationsSwitchesByte[compensationSwitchIdx]; // 1

                } else {
                    txDataMessage[compensationsSwitchesWord[compensationSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] &=
                            ~compensationsSwitchesByte[compensationSwitchIdx]; // 0
                }
            }
        }
    }
}
