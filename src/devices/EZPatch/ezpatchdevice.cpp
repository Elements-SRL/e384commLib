#include "ezpatchdevice.h"

#include <random>

/*****************\
 *  Ctor / Dtor  *
\*****************/

EZPatchDevice::EZPatchDevice(std::string deviceId) :
    MessageDispatcher(deviceId) {

    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAck] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdNack] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdPing] = true;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdFpgaReset] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp] = true;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData] = false;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail] = true;
    rxExpectAckMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation] = false;

    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAck] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdNack] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdPing] = false;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdFpgaReset] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData] = true;
    rxEnabledTypesMap[MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail] = true;
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
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdDigitalOffsetComp] = true;
    txExpectAckMap[MsgDirectionPcToDevice+MsgTypeIdDigitalOffsetCompInquiry] = true;
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

ErrorCodes_t EZPatchDevice::setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag) {
    ErrorCodes_t ret = Success;
    for (unsigned int idx; idx < channelIndexes.size(); idx++) {
        if (ret == Success) {
            ret = this->setVoltageHoldTuner(channelIndexes[idx], voltages[idx]);
        }
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag) {
    ErrorCodes_t ret = Success;
    for (unsigned int idx; idx < channelIndexes.size(); idx++) {
        if (ret == Success) {
            ret = this->setCurrentHoldTuner(channelIndexes[idx], currents[idx]);
        }
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::setVoltageHoldTuner(uint16_t channelIdx, Measurement_t voltage) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        voltageTuner[channelIdx] = voltage;
        voltageTuner[channelIdx].convertValue(vHoldRange[selectedVcVoltageRangeIdx].prefix);

        uint16_t dataLength = 4;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->int322uint16((int32_t)round(voltageTuner[channelIdx].value/vHoldRange[selectedVcVoltageRangeIdx].step), txDataMessage, 0);
        txDataMessage[3] = txDataMessage[1];
        txDataMessage[1] = txDataMessage[0];
        txDataMessage[0] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
        txDataMessage[2] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            voltageTuner[channelIdx].convertValue(voltageRange.prefix);
            voltageTunerCorrection[channelIdx] = voltageTuner[channelIdx].value;
        }

    } else if (channelIdx == currentChannelsNum) {
        uint16_t dataLength = 4*currentChannelsNum;
        std::vector <uint16_t> txDataMessage(dataLength);
        for (channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            voltageTuner[channelIdx] = voltage;
            voltageTuner[channelIdx].convertValue(vHoldRange[selectedVcVoltageRangeIdx].prefix);
            this->int322uint16((int32_t)round(voltageTuner[channelIdx].value/vHoldRange[selectedVcVoltageRangeIdx].step), txDataMessage, channelIdx*4);
            txDataMessage[3+channelIdx*4] = txDataMessage[1+channelIdx*4];
            txDataMessage[1+channelIdx*4] = txDataMessage[0+channelIdx*4];
        }

        for (channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            txDataMessage[0+channelIdx*4] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[2+channelIdx*4] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            for (channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
                voltageTuner[channelIdx].convertValue(voltageRange.prefix);
                voltageTunerCorrection[channelIdx] = voltageTuner[channelIdx].value;
            }
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setCurrentHoldTuner(uint16_t channelIdx, Measurement_t current) {
    ErrorCodes_t ret;

    if (channelIdx < voltageChannelsNum) {
        currentTuner[channelIdx] = current;
        currentTuner[channelIdx].convertValue(cHoldRange[selectedCcCurrentRangeIdx].prefix);

        uint16_t dataLength = 4;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->int322uint16((int32_t)round(currentTuner[channelIdx].value/cHoldRange[selectedCcCurrentRangeIdx].step), txDataMessage, 0);
        txDataMessage[3] = txDataMessage[1];
        txDataMessage[1] = txDataMessage[0];
        txDataMessage[0] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
        txDataMessage[2] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            currentTuner[channelIdx].convertValue(currentRange.prefix);
            currentTunerCorrection[channelIdx] = currentTuner[channelIdx].value;
        }

    } else if (channelIdx == voltageChannelsNum) {
        uint16_t dataLength = 4*voltageChannelsNum;
        std::vector <uint16_t> txDataMessage(dataLength);
        for (channelIdx = 0; channelIdx < voltageChannelsNum; channelIdx++) {
            currentTuner[channelIdx] = current;
            currentTuner[channelIdx].convertValue(cHoldRange[selectedCcCurrentRangeIdx].prefix);
            this->int322uint16((int32_t)round(currentTuner[channelIdx].value/cHoldRange[selectedCcCurrentRangeIdx].step), txDataMessage, channelIdx*4);
            txDataMessage[3+channelIdx*4] = txDataMessage[1+channelIdx*4];
            txDataMessage[1+channelIdx*4] = txDataMessage[0+channelIdx*4];
        }

        for (channelIdx = 0; channelIdx < voltageChannelsNum; channelIdx++) {
            txDataMessage[0+channelIdx*4] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[2+channelIdx*4] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            for (channelIdx = 0; channelIdx < voltageChannelsNum; channelIdx++) {
                currentTuner[channelIdx].convertValue(currentRange.prefix);
                currentTunerCorrection[channelIdx] = currentTuner[channelIdx].value;
            }
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnVoltageStimulusOn(bool on, bool applyFlag) {
    ErrorCodes_t ret;

    if (vcStimulusSwitchesNum > 0) {
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

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        return ErrorFeatureNotImplemented;
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

ErrorCodes_t EZPatchDevice::setSourceForVoltageChannel(uint16_t source, bool applyFlag) {
    ErrorCodes_t ret;
    if ((ret = this->setChannelsSources((int16_t)source, selectedSourceForCurrentChannelIdx)) != Success) {
        return ret;
    }
    selectedSourceForVoltageChannelIdx = (int16_t)source;
    return Success;
}

ErrorCodes_t EZPatchDevice::setSourceForCurrentChannel(uint16_t source, bool applyFlag) {
    ErrorCodes_t ret;
    if ((ret = this->setChannelsSources(selectedSourceForVoltageChannelIdx, (int16_t)source)) != Success) {
        return ret;
    }
    selectedSourceForCurrentChannelIdx = (int16_t)source;
    return Success;
}

ErrorCodes_t EZPatchDevice::setChannelsSources(int16_t voltageSourcesIdx, int16_t currentSourcesIdx) {
    ErrorCodes_t ret = Success;

    uint16_t dataLength = selectableTotalChannelsNum*2;
    std::vector <uint16_t> txDataMessage(dataLength);

    unsigned short channelIdx = 0;
    for (; channelIdx < selectableVoltageChannelsNum; channelIdx++) {
        if (find(availableVoltageSourcesIdxsArray.begin(), availableVoltageSourcesIdxsArray.end(), voltageSourcesIdx) != availableVoltageSourcesIdxsArray.end()) {
            txDataMessage[2*channelIdx] = channelSourcesRegisters[channelIdx];
            txDataMessage[2*channelIdx+1] = (uint16_t)voltageSourcesIdx;

        } else {
            ret = ErrorValueOutOfRange;
            break;
        }
    }

    for (; channelIdx < selectableVoltageChannelsNum+selectableCurrentChannelsNum; channelIdx++) {
        if (find(availableCurrentSourcesIdxsArray.begin(), availableCurrentSourcesIdxsArray.end(), currentSourcesIdx) != availableCurrentSourcesIdxsArray.end()) {
            txDataMessage[2*channelIdx] = channelSourcesRegisters[channelIdx];
            txDataMessage[2*channelIdx+1] = (uint16_t)currentSourcesIdx;

        } else {
            ret = ErrorValueOutOfRange;
            break;
        }
    }

    if (ret == Success) {
        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
        selectedVoltageSourceIdx = voltageSourcesIdx;
        selectedCurrentSourceIdx = currentSourcesIdx;
        this->selectChannelsResolutions();
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::turnOnLsbNoise(bool flag) {
    this->initializeLsbNoise(!flag);
    return Success;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(resetDuration));
        this->resetAsic(false, true);

        samplingRate = realSamplingRatesArray[samplingRateIdx];
        integrationStep = integrationStepArray[selectedSamplingRateIdx];

        /*! Used for the digital filter */
        this->computeRawDataFilterCoefficients();

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
//    if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
//        return ErrorValueOutOfRange;
//    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        uint16_t chIdx = channelIndexes[i];
        this->enableStimulus(chIdx, !onValues[i]);
        channelModels[chIdx]->setCompensatingLiquidJunction(onValues[i]);
        if (onValues[i] && (liquidJunctionStates[chIdx] == LiquidJunctionIdle)) {
            liquidJunctionStates[chIdx] = LiquidJunctionStarting;

        } else if (!onValues[i]) {
            liquidJunctionStates[chIdx] = LiquidJunctionTerminate;
        }
    }

    anyLiquidJuctionActive = true;
    return Success;
}

ErrorCodes_t EZPatchDevice::digitalOffsetCompensation(uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        this->setDigitalOffsetCompensationOverrideSwitch(false);
        uint16_t dataLength = 1;
        std::vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = channelIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdDigitalOffsetComp, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::digitalOffsetCompensationOverride(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    if (digitalOffsetCompensationOverrideImplemented) {
        if (channelIdx < currentChannelsNum) {
            ret = this->setDigitalOffsetCompensationOverrideSwitch(true);
            if (ret == Success) {
                ret = this->setDigitalOffsetCompensationOverrideValue(channelIdx, value);

                if (ret == Success) {
                    uint16_t dataLength = 1;
                    std::vector <uint16_t> txDataMessage(dataLength);
                    txDataMessage[0] = channelIdx;

                    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdDigitalOffsetComp, txDataMessage, dataLength);
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

ErrorCodes_t EZPatchDevice::setVcCurrentOffsetDelta(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    if (vcCurrentOffsetDeltaImplemented) {
        if (channelIdx < currentChannelsNum) {
            value.convertValue(currentRange.prefix);
            uint16_t dataLength = 2;
            std::vector <uint16_t> txDataMessage(dataLength);

            txDataMessage[0] = vcCurrentOffsetDeltaRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[1] = (uint16_t)((int16_t)round(value.value*SHORT_MAX/currentRange.max));

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

            return ret;

        } else {
            ret = ErrorValueOutOfRange;
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::setCcVoltageOffsetDelta(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    if (vcCurrentOffsetDeltaImplemented) {
        if (channelIdx < currentChannelsNum) {
            value.convertValue(voltageRange.prefix);
            uint16_t dataLength = 2;
            std::vector <uint16_t> txDataMessage(dataLength);

            txDataMessage[0] = ccVoltageOffsetDeltaRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[1] = (uint16_t)((int16_t)round(value.value*SHORT_MAX/voltageRange.max));

            ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

            return ret;

        } else {
            ret = ErrorValueOutOfRange;
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t EZPatchDevice::digitalOffsetCompensationInquiry(uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        uint16_t dataLength = 1;
        std::vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = channelIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdDigitalOffsetCompInquiry, txDataMessage, dataLength);

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

ErrorCodes_t EZPatchDevice::enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) {
//    if (!areAllTheVectorElementsLessThan(channelIndexes, currentChannelsNum)) {
//        return ErrorValueOutOfRange;
//    }
    for(uint32_t i = 0; i < channelIndexes.size(); i++){
        this->enableStimulus(channelIndexes[i], onValues[i]);
        channelModels[channelIndexes[i]]->setInStimActive(onValues[i]);
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
    ErrorCodes_t ret;

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

ErrorCodes_t EZPatchDevice::setCompensationsChannel(uint16_t channelIdx) {
    compensationsSettingChannel = channelIdx;
    /*! Needed to update compensable field in all CompensationControl_t */
    this->checkCompensationsValues();
    return Success;
}

ErrorCodes_t EZPatchDevice::turnVoltageCompensationsOn(bool on) {
    bool flags[6];
    flags[0] = compensationsEnabledArray[CompensationPipette][compensationsSettingChannel];
    flags[1] = compensationsEnabledArray[CompensationMembrane][compensationsSettingChannel];
    flags[2] = compensationsEnabledArray[CompensationResistance][compensationsSettingChannel];
    flags[3] = compensationsEnabledArray[CompensationResistanceCorrection][compensationsSettingChannel];
    flags[4] = compensationsEnabledArray[CompensationResistancePrediction][compensationsSettingChannel];
    flags[5] = compensationsEnabledArray[CompensationLeakConductance][compensationsSettingChannel];

    compensationsEnabledArray[CompensationPipette][compensationsSettingChannel] = on & pipetteCompensationFlag[compensationsSettingChannel];
    compensationsEnabledArray[CompensationMembrane][compensationsSettingChannel] = on & membraneCompensationFlag[compensationsSettingChannel];
    compensationsEnabledArray[CompensationResistance][compensationsSettingChannel] = on & resistanceCompensationFlag[compensationsSettingChannel];
    compensationsEnabledArray[CompensationResistanceCorrection][compensationsSettingChannel] = on & resistanceCorrectionFlag[compensationsSettingChannel];
    compensationsEnabledArray[CompensationResistancePrediction][compensationsSettingChannel] = on & resistancePredictionFlag[compensationsSettingChannel];
    compensationsEnabledArray[CompensationLeakConductance][compensationsSettingChannel] = on & leakConductanceCompensationFlag[compensationsSettingChannel];

    ErrorCodes_t ret = this->turnCompensationsOn(voltageCompensationsFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationPipette][compensationsSettingChannel] = flags[0];
        compensationsEnabledArray[CompensationMembrane][compensationsSettingChannel] = flags[1];
        compensationsEnabledArray[CompensationResistance][compensationsSettingChannel] = flags[2];
        compensationsEnabledArray[CompensationResistanceCorrection][compensationsSettingChannel] = flags[3];
        compensationsEnabledArray[CompensationResistancePrediction][compensationsSettingChannel] = flags[4];
        compensationsEnabledArray[CompensationLeakConductance][compensationsSettingChannel] = flags[5];
    }
    vcCompensationsActivated = on;
    return ret;
}

ErrorCodes_t EZPatchDevice::turnCurrentCompensationsOn(bool on) {
    bool flags[5];
    flags[0] = compensationsEnabledArray[CompensationCCPipette][compensationsSettingChannel];
    flags[1] = compensationsEnabledArray[CompensationBridgeBalance][compensationsSettingChannel];

    compensationsEnabledArray[CompensationCCPipette][compensationsSettingChannel] = on & ccPipetteCompensationFlag[compensationsSettingChannel];
    compensationsEnabledArray[CompensationBridgeBalance][compensationsSettingChannel] = on & bridgeBalanceCompensationFlag[compensationsSettingChannel];

    ErrorCodes_t ret = this->turnCompensationsOn(currentCompensationsFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationCCPipette][compensationsSettingChannel] = flags[0];
        compensationsEnabledArray[CompensationBridgeBalance][compensationsSettingChannel] = flags[1];
    }
    ccCompensationsActivated = on;
    return ret;
}

ErrorCodes_t EZPatchDevice::turnPipetteCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationPipette][compensationsSettingChannel];
    compensationsEnabledArray[CompensationPipette][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(pipetteCompensationFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationPipette][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnCCPipetteCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationCCPipette][compensationsSettingChannel];
    compensationsEnabledArray[CompensationCCPipette][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(ccPipetteCompensationFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationCCPipette][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnMembraneCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationMembrane][compensationsSettingChannel];
    compensationsEnabledArray[CompensationMembrane][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(membraneCompensationFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationMembrane][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnResistanceCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationResistance][compensationsSettingChannel];
    compensationsEnabledArray[CompensationResistance][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(resistanceCompensationFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationResistance][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnResistanceCorrectionOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationResistanceCorrection][compensationsSettingChannel];
    compensationsEnabledArray[CompensationResistanceCorrection][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(resistanceCorrectionFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationResistanceCorrection][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnResistancePredictionOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationResistancePrediction][compensationsSettingChannel];
    compensationsEnabledArray[CompensationResistancePrediction][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(resistancePredictionFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationResistancePrediction][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnLeakConductanceCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationLeakConductance][compensationsSettingChannel];
    compensationsEnabledArray[CompensationLeakConductance][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(leakConductanceCompensationFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationLeakConductance][compensationsSettingChannel] = flag;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::turnBridgeBalanceCompensationOn(bool on) {
    bool flag = compensationsEnabledArray[CompensationBridgeBalance][compensationsSettingChannel];
    compensationsEnabledArray[CompensationBridgeBalance][compensationsSettingChannel] = on;

    ErrorCodes_t ret = this->turnCompensationOn(bridgeBalanceCompensationFlag, on);
    if (ret != Success) {
        compensationsEnabledArray[CompensationBridgeBalance][compensationsSettingChannel] = flag;
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
    return this->setCompensationValue(pipetteCapacitance[compensationsSettingChannel], pipetteCapacitanceControl, capacitance);
}

ErrorCodes_t EZPatchDevice::setCCPipetteCapacitance(Measurement_t capacitance) {
    return this->setCompensationValue(ccPipetteCapacitance[compensationsSettingChannel], ccPipetteCapacitanceControl, capacitance);
}

ErrorCodes_t EZPatchDevice::setMembraneCapacitance(Measurement_t capacitance) {
    return this->setCompensationValue(membraneCapacitance[compensationsSettingChannel], membraneCapacitanceControl, capacitance);
}

ErrorCodes_t EZPatchDevice::setAccessResistance(Measurement_t resistance) {
    return this->setCompensationValue(accessResistance[compensationsSettingChannel], accessResistanceControl, resistance);
}

ErrorCodes_t EZPatchDevice::setResistanceCorrectionPercentage(Measurement_t percentage) {
    return this->setCompensationValue(resistanceCorrectionPercentage[compensationsSettingChannel], resistanceCorrectionPercentageControl, percentage);
}

ErrorCodes_t EZPatchDevice::setResistanceCorrectionLag(Measurement_t lag) {
    return this->setCompensationValue(resistanceCorrectionLag[compensationsSettingChannel], resistanceCorrectionLagControl, lag);
}

ErrorCodes_t EZPatchDevice::setResistancePredictionGain(Measurement_t gain) {
    return this->setCompensationValue(resistancePredictionGain[compensationsSettingChannel], resistancePredictionGainControl, gain);
}

ErrorCodes_t EZPatchDevice::setResistancePredictionPercentage(Measurement_t percentage) {
    return this->setCompensationValue(resistancePredictionPercentage[compensationsSettingChannel], resistancePredictionPercentageControl, percentage);
}

ErrorCodes_t EZPatchDevice::setResistancePredictionBandwidthGain(Measurement_t gain) {
    return this->setCompensationValue(resistancePredictionBandwidthGain[compensationsSettingChannel], resistancePredictionBandwidthGainControl, gain);
}

ErrorCodes_t EZPatchDevice::setResistancePredictionTau(Measurement_t tau) {
    return this->setCompensationValue(resistancePredictionTau[compensationsSettingChannel], resistancePredictionTauControl, tau);
}

ErrorCodes_t EZPatchDevice::setLeakConductance(Measurement_t conductance) {
    return this->setCompensationValue(leakConductance[compensationsSettingChannel], leakConductanceControl, conductance);
}

ErrorCodes_t EZPatchDevice::setBridgeBalanceResistance(Measurement_t resistance) {
    return this->setCompensationValue(bridgeBalanceResistance[compensationsSettingChannel], bridgeBalanceResistanceControl, resistance);
}

ErrorCodes_t EZPatchDevice::setDigitalTriggerOutput(uint16_t triggerIdx, bool terminator, bool polarity, uint16_t triggerId, Measurement_t delay) {
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

ErrorCodes_t EZPatchDevice::setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest) {
    ErrorCodes_t ret;

    if (itemsNum <= protocolMaxItemsNum) {
        vRest.convertValue(vcVoltageRangesArray[selectedVcVoltageRangeIdx].prefix);

        uint16_t dataLength = 5;
        std::vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = protId;
        txDataMessage[1] = itemsNum;
        txDataMessage[2] = sweepsNum;
        this->int322uint16((int32_t)round(vRest.value/vcVoltageRangesArray[selectedVcVoltageRangeIdx].step), txDataMessage, 3);

        stepsOnLastSweep = (double)(sweepsNum-1);
        protocolItemsNum = itemsNum;
        protocolItemIndex = 0;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdVoltageProtocolStruct, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
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
            this->int322uint16((int32_t)round(v0.value/vcVoltageRangesArray[selectedVcVoltageRangeIdx].step), txDataMessage, 0);
            this->int322uint16((int32_t)round(vStep.value/vcVoltageRangesArray[selectedVcVoltageRangeIdx].step), txDataMessage, 2);
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
            this->int322uint16((int32_t)round(v0.value/vcVoltageRangesArray[selectedVcVoltageRangeIdx].step), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(vFinal.value/vcVoltageRangesArray[selectedVcVoltageRangeIdx].step), txDataMessage, 4);
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
            this->int322uint16((int32_t)round(v0.value/vcVoltageRangesArray[selectedVcVoltageRangeIdx].step), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(vAmp.value/vcVoltageRangesArray[selectedVcVoltageRangeIdx].step), txDataMessage, 4);
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

ErrorCodes_t EZPatchDevice::setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest) {
    ErrorCodes_t ret;

    if (itemsNum <= protocolMaxItemsNum) {
        iRest.convertValue(ccCurrentRangesArray[selectedCcCurrentRangeIdx].prefix);

        uint16_t dataLength = 5;
        std::vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = protId;
        txDataMessage[1] = itemsNum;
        txDataMessage[2] = sweepsNum;
        this->int322uint16((int32_t)round(iRest.value/ccCurrentRangesArray[selectedCcCurrentRangeIdx].step), txDataMessage, 3);

        stepsOnLastSweep = (double)(sweepsNum-1);
        protocolItemsNum = itemsNum;
        protocolItemIndex = 0;

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdCurrentProtocolStruct, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
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
            this->int322uint16((int32_t)round(i0.value/ccCurrentRangesArray[selectedCcCurrentRangeIdx].step), txDataMessage, 0);
            this->int322uint16((int32_t)round(iStep.value/ccCurrentRangesArray[selectedCcCurrentRangeIdx].step), txDataMessage, 2);
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
            this->int322uint16((int32_t)round(i0.value/ccCurrentRangesArray[selectedCcCurrentRangeIdx].step), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(iFinal.value/ccCurrentRangesArray[selectedCcCurrentRangeIdx].step), txDataMessage, 4);
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
            this->int322uint16((int32_t)round(i0.value/ccCurrentRangesArray[selectedCcCurrentRangeIdx].step), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(iAmp.value/ccCurrentRangesArray[selectedCcCurrentRangeIdx].step), txDataMessage, 4);
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

ErrorCodes_t EZPatchDevice::resetDigitalOffsetCompensation(bool reset) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    std::vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    unsigned int resetIdx = ResetIndexDigitalOffsetCompensation;

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

ErrorCodes_t EZPatchDevice::getSwitchesStatus(std::vector <uint16_t> &words, std::vector <std::vector <std::string>> &names) {
    ErrorCodes_t ret = Success;
    words.resize(switchesStatusLength);
    names.resize(switchesStatusLength);
    switches2DataMessage(words);
    for (unsigned int wordIdx = 0; wordIdx < switchesStatusLength; wordIdx++) {
        names[wordIdx].resize(16);
        for (unsigned int stringIdx = 0; stringIdx < 16; stringIdx++) {
            names[wordIdx][stringIdx] = switchesNames[wordIdx][stringIdx];
        }
    }
    return ret;
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

ErrorCodes_t EZPatchDevice::getDeviceInfo(std::string &deviceId, std::string &deviceName, uint8_t &deviceVersion, uint8_t &deviceSubversion, uint32_t &firmwareVersion) {
    ErrorCodes_t ret = Success;

    deviceId = this->deviceId;
    deviceName = this->deviceName;
    DeviceTuple_t tuple = FtdiEeprom56(deviceId).getDeviceTuple();
    deviceVersion = tuple.version;
    deviceSubversion = tuple.subversion;
    firmwareVersion = tuple.fwVersion;

    return ret;
}

ErrorCodes_t EZPatchDevice::getNextMessage(RxOutput_t &rxOutput, int16_t * data) {
    ErrorCodes_t ret = Success;
    double xFlt;

    std::unique_lock <std::mutex> rxMutexLock (rxMutex);
    if (rxMsgBufferReadLength <= 0) {
        rxMsgBufferNotEmpty.wait_for(rxMutexLock, std::chrono::milliseconds(1000));
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
            case (MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp):
            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
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

            case (MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp):
                rxOutput.dataLen = 1;
                rxOutput.channelIdx = * (rxDataBuffer+dataOffset);
                rawFloat = * (rxDataBuffer+((dataOffset+1)&EZP_RX_DATA_BUFFER_MASK));
                voltageOffsetCorrected = (((double)rawFloat)-liquidJunctionOffsetBinary)*liquidJunctionResolution;
                this->selectVoltageOffsetResolution();
                data[0] = (int16_t)(rawFloat-liquidJunctionOffsetBinary);
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;

//                this->setDigitalOffsetCompensationOverrideValue(rxOutput.channelIdx, {voltageOffsetCorrected, liquidJunctionControl.prefix, "V"}); /*! \todo FCON capire perchè stato messo questo comando durante il porting su e384commlib */
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
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyRawDataFilter(voltageChannelIdx, (((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx])+(voltageOffsetCorrection+voltageTunerCorrection[voltageChannelIdx])/voltageResolution, iirVNum, iirVDen);
                            xFlt = iirY[voltageChannelIdx][iirOff];
                            data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&EZP_LSB_NOISE_ARRAY_MASK;
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyRawDataFilter(currentChannelIdx+voltageChannelsNum, (((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx])+currentTunerCorrection[currentChannelIdx]/currentResolution, iirINum, iirIDen);
                            xFlt = iirY[currentChannelIdx+voltageChannelsNum][iirOff];
                            data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&EZP_LSB_NOISE_ARRAY_MASK;
                        }

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
//                    rxOutput.uintData[dataIdx] = * (rxDataBuffer+dataOffset); /* \todo FCON al momento della saturazione non frega niente a nessuno */
                    dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;
                }
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;
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
    rxMsgBufferNotFull.notify_all();
    rxMutexLock.unlock();

    return ret;
}

ErrorCodes_t EZPatchDevice::hasVoltageHoldTuner() {
    if (voltageHoldTunerImplemented) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::hasCurrentHoldTuner() {
    if (currentHoldTunerImplemented) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
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
    if (slaveImplementedFlag) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
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

ErrorCodes_t EZPatchDevice::getPipetteCompensationOptions(std::vector <std::string> &options) {
    if (pipetteCompensationOptions.size() > 0) {
        options = pipetteCompensationOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getCCPipetteCompensationOptions(std::vector <std::string> &options) {
    if (ccPipetteCompensationOptions.size() > 0) {
        options = ccPipetteCompensationOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getMembraneCompensationOptions(std::vector <std::string> &options) {
    if (membraneCompensationOptions.size() > 0) {
        options = membraneCompensationOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getResistanceCompensationOptions(std::vector <std::string> &options) {
    if (resistanceCompensationOptions.size() > 0) {
        options = resistanceCompensationOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getResistanceCorrectionOptions(std::vector <std::string> &options) {
    if (resistanceCorrectionOptions.size() > 0) {
        options = resistanceCorrectionOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getResistancePredictionOptions(std::vector <std::string> &options) {
    if (resistancePredictionOptions.size() > 0) {
        options = resistancePredictionOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getLeakConductanceCompensationOptions(std::vector <std::string> &options) {
    if (leakConductanceCompensationOptions.size() > 0) {
        options = leakConductanceCompensationOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getBridgeBalanceCompensationOptions(std::vector <std::string> &options) {
    if (bridgeBalanceCompensationOptions.size() > 0) {
        options = bridgeBalanceCompensationOptions;
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t EZPatchDevice::getLiquidJunctionControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (liquidJunctionControl.implemented) {
        control = liquidJunctionControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getPipetteCapacitanceControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (pipetteCapacitanceControl.implemented) {
        control = pipetteCapacitanceControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getCCPipetteCapacitanceControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (ccPipetteCapacitanceControl.implemented) {
        control = ccPipetteCapacitanceControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getMembraneCapacitanceControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (membraneCapacitanceControl.implemented) {
        control = membraneCapacitanceControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getAccessResistanceControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (accessResistanceControl.implemented) {
        control = accessResistanceControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getResistanceCorrectionPercentageControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistanceCorrectionPercentageControl.implemented) {
        control = resistanceCorrectionPercentageControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getResistanceCorrectionLagControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistanceCorrectionLagControl.implemented) {
        control = resistanceCorrectionLagControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getResistancePredictionGainControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistancePredictionGainControl.implemented) {
        control = resistancePredictionGainControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getResistancePredictionPercentageControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistancePredictionPercentageControl.implemented) {
        control = resistancePredictionPercentageControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getResistancePredictionBandwidthGainControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistancePredictionBandwidthGainControl.implemented) {
        control = resistancePredictionBandwidthGainControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getResistancePredictionTauControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (resistancePredictionTauControl.implemented) {
        control = resistancePredictionTauControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getLeakConductanceControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (leakConductanceControl.implemented) {
        control = leakConductanceControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t EZPatchDevice::getBridgeBalanceResistanceControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (bridgeBalanceResistanceControl.implemented) {
        control = bridgeBalanceResistanceControl;
        ret = Success;
    }
    return ret;
}

/*********************\
 *  Private methods  *
\*********************/

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

    lsbNoiseArray = new (std::nothrow) double[EZP_LSB_NOISE_ARRAY_SIZE];
    if (lsbNoiseArray == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    /*! Allocate memory for holding values to correct readout from device */
    voltageTuner.resize(currentChannelsNum);
    std::fill(voltageTuner.begin(), voltageTuner.end(), Measurement_t({0.0, UnitPfxNone, "V"}));
    currentTuner.resize(voltageChannelsNum);
    std::fill(currentTuner.begin(), currentTuner.end(), Measurement_t({0.0, UnitPfxNone, "A"}));
    voltageTunerCorrection.resize(currentChannelsNum);
    std::fill(voltageTunerCorrection.begin(), voltageTunerCorrection.end(), 0.0);
    currentTunerCorrection.resize(voltageChannelsNum);
    std::fill(currentTunerCorrection.begin(), currentTunerCorrection.end(), 0.0);

    return Success;
}

void EZPatchDevice::initializeVariables() {
    MessageDispatcher::initializeVariables();
    /*! Calculate the LSB noise std::vector */
    this->initializeLsbNoise();

    /*! Allocate memory for compensations */
    this->initializeCompensations();
}

ErrorCodes_t EZPatchDevice::deviceConfiguration() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ErrorCodes_t ret = MessageDispatcher::deviceConfiguration();
    if (ret != Success) {
        return ret;
    }

    return this->setConstantSwitches();
}

void EZPatchDevice::createCommunicationThreads() {
    rxThread = std::thread(&EZPatchDevice::readAndParseMessages, this);
    txThread = std::thread(&EZPatchDevice::unwrapAndSendMessages, this);

    threadsStarted = true;
}

ErrorCodes_t EZPatchDevice::initializeHW() {
    RxOutput_t rxOutput;
    ErrorCodes_t ret = ErrorUnknown;
    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;
    int pingTries = 0;
    int fpgaResetTries = 0;

    while (ret != Success) {
        if (pingTries++ > EZP_MAX_PING_TRIES) {
            return ErrorConnectionPingFailed;
        }

        this->resetFpga();
        while ((rxOutput.msgTypeId != MsgDirectionDeviceToPc+MsgTypeIdFpgaReset) || (ret != Success)) {
            if (fpgaResetTries++ > EZP_MAX_FPGA_RESET_TRIES) {
                return ErrorConnectionFpgaResetFailed;
            }
            int16_t * datain;
            this->allocateRxDataBuffer(datain);
            ret = this->getNextMessage(rxOutput, datain);
            this->deallocateRxDataBuffer(datain);

#ifdef DEBUGPRINT
            fprintf(rxFid,
                    "fpga reset\n"
                    "try %d: %s\n\n",
                    fpgaResetTries,
                    ((rxOutput.msgTypeId == MsgDirectionDeviceToPc+MsgTypeIdFpgaReset) && (ret == Success) ? "success" : "fail"));
            fflush(rxFid);
#endif

        }
        fpgaResetTries = 0;

        ret = this->ping();

#ifdef DEBUGPRINT
        fprintf(rxFid,
                "ping\n"
                "try %d: %s\n\n",
                pingTries,
                (ret == Success ? "success" : "fail"));
        fflush(rxFid);
#endif
    }

    ret = this->resetAsic(true, true);
    if (ret != Success) {
        return ErrorConnectionChipResetFailed;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(resetDuration));

    ret = this->resetAsic(false, true);
    if (ret != Success) {
        return ErrorConnectionChipResetFailed;
    }

    this->setSecondaryDeviceSwitch(false); /*! \todo FCON questo va propriamente settato per gestire device main e secondary */
    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        this->setVcCurrentOffsetDelta(idx, {0.0, UnitPfxNone, "A"});
    }

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

    if (lsbNoiseArray != nullptr) {
        delete [] lsbNoiseArray;
        lsbNoiseArray = nullptr;
    }
}

void EZPatchDevice::deinitializeVariables() {
    /*! Nothing to be done */
    MessageDispatcher::deinitializeVariables();
}

void EZPatchDevice::joinCommunicationThreads() {
    if (threadsStarted) {
        rxThread.join();
        txThread.join();

        threadsStarted = false;
    }
}

void EZPatchDevice::initializeLsbNoise(bool nullValues) {
    if (nullValues) {
        /*! By default there is no added noise  */
        for (int32_t i = 0; i < EZP_LSB_NOISE_ARRAY_SIZE; i++) {
            lsbNoiseArray[i] = 0.0;
        }

    } else {
        std::mt19937 mtRng((uint32_t)time(nullptr));
        double den = (double)0xFFFFFFFF;
        for (int32_t i = 0; i < EZP_LSB_NOISE_ARRAY_SIZE; i++) {
            lsbNoiseArray[i] = ((double)mtRng())/den-0.5;
        }
    }
}

void EZPatchDevice::initializeCompensations() {
    for (uint16_t compensationIdx = 0; compensationIdx < CompensationsNum; compensationIdx++) {
        /*! Assuming patch devices always have the same number of voltage and current channels */
        compensationsEnabledArray[compensationIdx].resize(currentChannelsNum);
        for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            compensationsEnabledArray[compensationIdx][channelIdx] = false;
        }
    }

    pipetteCapacitance.resize(currentChannelsNum);
    ccPipetteCapacitance.resize(currentChannelsNum);
    membraneCapacitance.resize(currentChannelsNum);
    accessResistance.resize(currentChannelsNum);
    resistanceCorrectionPercentage.resize(currentChannelsNum);
    resistanceCorrectionLag.resize(currentChannelsNum);
    resistancePredictionGain.resize(currentChannelsNum);
    resistancePredictionPercentage.resize(currentChannelsNum);
    resistancePredictionBandwidthGain.resize(currentChannelsNum);
    resistancePredictionTau.resize(currentChannelsNum);
    leakConductance.resize(currentChannelsNum);
    bridgeBalanceResistance.resize(currentChannelsNum);

    voltageCompensationsFlag.resize(currentChannelsNum);
    currentCompensationsFlag.resize(currentChannelsNum);
    pipetteCompensationFlag.resize(currentChannelsNum);
    ccPipetteCompensationFlag.resize(currentChannelsNum);
    membraneCompensationFlag.resize(currentChannelsNum);
    resistanceCompensationFlag.resize(currentChannelsNum);
    resistanceCorrectionFlag.resize(currentChannelsNum);
    resistancePredictionFlag.resize(currentChannelsNum);
    leakConductanceCompensationFlag.resize(currentChannelsNum);
    bridgeBalanceCompensationFlag.resize(currentChannelsNum);

    pipetteCompensationOptionReg.resize(currentChannelsNum);
    ccPipetteCompensationOptionReg.resize(currentChannelsNum);
    membraneCompensationOptionReg.resize(currentChannelsNum);
    resistanceCompensationOptionReg.resize(currentChannelsNum);
    resistanceCorrectionOptionReg.resize(currentChannelsNum);
    resistancePredictionOptionReg.resize(currentChannelsNum);
    leakConductanceCompensationOptionReg.resize(currentChannelsNum);
    bridgeBalanceCompensationOptionReg.resize(currentChannelsNum);

    for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        pipetteCapacitance[channelIdx] = 1.0;
        ccPipetteCapacitance[channelIdx] = 1.0;
        membraneCapacitance[channelIdx] = 1.0;
        accessResistance[channelIdx] = 1.0;
        resistanceCorrectionPercentage[channelIdx] = 1.0;
        resistanceCorrectionLag[channelIdx] = 1.0;
        resistancePredictionGain[channelIdx] = 1.0;
        resistancePredictionPercentage[channelIdx] = 1.0;
        resistancePredictionBandwidthGain[channelIdx] = 1.0;
        resistancePredictionTau[channelIdx] = 1.0;
        leakConductance[channelIdx] = 1.0;
        bridgeBalanceResistance[channelIdx] = 1.0;

        voltageCompensationsFlag[channelIdx] = false;
        currentCompensationsFlag[channelIdx] = false;
        pipetteCompensationFlag[channelIdx] = false;
        ccPipetteCompensationFlag[channelIdx] = false;
        membraneCompensationFlag[channelIdx] = false;
        resistanceCompensationFlag[channelIdx] = false;
        resistanceCorrectionFlag[channelIdx] = false;
        resistancePredictionFlag[channelIdx] = false;
        leakConductanceCompensationFlag[channelIdx] = false;
        bridgeBalanceCompensationFlag[channelIdx] = false;

        pipetteCompensationOptionReg[channelIdx] = 0;
        ccPipetteCompensationOptionReg[channelIdx] = 0;
        membraneCompensationOptionReg[channelIdx] = 0;
        resistanceCompensationOptionReg[channelIdx] = 0;
        resistanceCorrectionOptionReg[channelIdx] = 0;
        resistancePredictionOptionReg[channelIdx] = 0;
        leakConductanceCompensationOptionReg[channelIdx] = 0;
        bridgeBalanceCompensationOptionReg[channelIdx] = 0;
    }
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
    uint32_t waitTime = 50;

    if (!txExpectAck) {
        this->wrapOutgoingMessage(msgTypeId, txDataMessage, dataLen);
        ret = Success;

    } else {
        std::unique_lock <std::mutex> ackLock(txAckMutex);
        txAckReceived = false;
        int resendTry = 0;
        while ((!txAckReceived) && (resendTry < EZP_MAX_RESEND_TRIES)) {
            ackLock.unlock();

            this->wrapOutgoingMessage(msgTypeId, txDataMessage, dataLen);

            ackLock.lock();
            txAckCv.wait_for(ackLock, std::chrono::milliseconds(waitTime));
            if (txAckReceived) {
                txWaitingOnAcks--;
                ret = Success;

            } else {
                resendTry++;
#ifdef RX_PACKETS_FLOW_PRINT
                cout << "resend" << endl;
#endif
#ifdef DEBUGPRINT
                fprintf(txFid, "resend\n\n");
                fflush(txFid);
#endif
            }
        }

        if (!txAckReceived) {
            txWaitingOnAcks--;
        }
        ackLock.unlock();
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

ErrorCodes_t EZPatchDevice::setDigitalOffsetCompensationOverrideSwitch(bool flag) {
    ErrorCodes_t ret;

    if (digitalOffsetCompensationOverrideImplemented) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (flag) {
            txDataMessage[digitalOffsetCompensationOverrideSwitchWord] |= digitalOffsetCompensationOverrideSwitchByte;

        } else {
            txDataMessage[digitalOffsetCompensationOverrideSwitchWord] &= ~digitalOffsetCompensationOverrideSwitchByte;
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

ErrorCodes_t EZPatchDevice::setSecondaryDeviceSwitch(bool flag) {
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

ErrorCodes_t EZPatchDevice::setDigitalOffsetCompensationOverrideValue(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    value.convertValue(liquidJunctionControl.prefix);
    uint16_t dataLength = 2;
    std::vector <uint16_t> txDataMessage(dataLength);

    txDataMessage[0] = digitalOffsetCompensationOverrideRegisterOffset+channelIdx*coreSpecificRegistersNum;
    uint16_t uintValue = (uint16_t)((int16_t)round(-value.value/liquidJunctionResolution/16.0)*16.0); /*! \todo FCON sta schifezza serve a tenere i 4 bit meno significativi a zero */
    txDataMessage[1] = uintValue;

    ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

    return ret;
}

bool EZPatchDevice::checkVoltStepTimeStepParameters(double v0, double vStep, double t0, double tStep, uint16_t repsNum, uint16_t applySteps) {
    double minVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].min-vcVoltageRangesArray[selectedVcVoltageRangeIdx].step*0.5;
    double maxVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].max+vcVoltageRangesArray[selectedVcVoltageRangeIdx].step*0.5;
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
    double minVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].min-vcVoltageRangesArray[selectedVcVoltageRangeIdx].step*0.5;
    double maxVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].max+vcVoltageRangesArray[selectedVcVoltageRangeIdx].step*0.5;
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
    double minVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].min-vcVoltageRangesArray[selectedVcVoltageRangeIdx].step*0.5;
    double maxVWithMargin = vcVoltageRangesArray[selectedVcVoltageRangeIdx].max+vcVoltageRangesArray[selectedVcVoltageRangeIdx].step*0.5;
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
    double minIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].min-ccCurrentRangesArray[selectedCcCurrentRangeIdx].step*0.5;
    double maxIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].max+ccCurrentRangesArray[selectedCcCurrentRangeIdx].step*0.5;
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
    double minIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].min-ccCurrentRangesArray[selectedCcCurrentRangeIdx].step*0.5;
    double maxIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].max+ccCurrentRangesArray[selectedCcCurrentRangeIdx].step*0.5;
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
    double minIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].min-ccCurrentRangesArray[selectedCcCurrentRangeIdx].step*0.5;
    double maxIWithMargin = ccCurrentRangesArray[selectedCcCurrentRangeIdx].max+ccCurrentRangesArray[selectedCcCurrentRangeIdx].step*0.5;
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
            this->updateWrittenCompesantionValues(txDataMessage);
        }

        /*! \todo FCON qui volendo si potrebbe fare il backtrace anche del comando degli switch/current range
                       nel caso in cui l'ultima scrittura non vada a buon fine */
    }
}

ErrorCodes_t EZPatchDevice::turnCompensationsOn(std::vector <bool> &flag, bool on) {
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

ErrorCodes_t EZPatchDevice::setCompensationValue(double &compensatedQuantity, CompensationControl_t &control, Measurement_t newValue) {
    ErrorCodes_t ret;

    if (control.implemented) {
        newValue.convertValue(control.prefix);
        double originalValue = compensatedQuantity;
        compensatedQuantity = newValue.value;

        if (this->checkCompensationsValues()) {
            uint16_t dataLength = compensationsRegistersNum*2;
            std::vector <uint16_t> txDataMessage(dataLength);
            if (this->fillCompensationsRegistersTxData(txDataMessage)) {
                ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
                if (ret == Success) {
                    this->updateWrittenCompesantionValues(txDataMessage);

                } else {
                    compensatedQuantity = originalValue;
                }

            } else {
                ret = ErrorUnchangedValue;
            }

        } else {
            ret = ErrorValueOutOfRange;
        }

    } else {
        ret = ErrorFeatureNotImplemented;
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
