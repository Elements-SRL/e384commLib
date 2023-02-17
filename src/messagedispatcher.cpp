#include "messagedispatcher.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <thread>
#include <math.h>
#include <random>
#include <algorithm>

static const uint16_t crc16CcittTable[256] = {
    0x0000U, 0x1021U, 0x2042U, 0x3063U, 0x4084U, 0x50A5U, 0x60C6U, 0x70E7U,
    0x8108U, 0x9129U, 0xA14AU, 0xB16BU, 0xC18CU, 0xD1ADU, 0xE1CEU, 0xF1EFU,
    0x1231U, 0x0210U, 0x3273U, 0x2252U, 0x52B5U, 0x4294U, 0x72F7U, 0x62D6U,
    0x9339U, 0x8318U, 0xB37BU, 0xA35AU, 0xD3BDU, 0xC39CU, 0xF3FFU, 0xE3DEU,
    0x2462U, 0x3443U, 0x0420U, 0x1401U, 0x64E6U, 0x74C7U, 0x44A4U, 0x5485U,
    0xA56AU, 0xB54BU, 0x8528U, 0x9509U, 0xE5EEU, 0xF5CFU, 0xC5ACU, 0xD58DU,
    0x3653U, 0x2672U, 0x1611U, 0x0630U, 0x76D7U, 0x66F6U, 0x5695U, 0x46B4U,
    0xB75BU, 0xA77AU, 0x9719U, 0x8738U, 0xF7DFU, 0xE7FEU, 0xD79DU, 0xC7BCU,
    0x48C4U, 0x58E5U, 0x6886U, 0x78A7U, 0x0840U, 0x1861U, 0x2802U, 0x3823U,
    0xC9CCU, 0xD9EDU, 0xE98EU, 0xF9AFU, 0x8948U, 0x9969U, 0xA90AU, 0xB92BU,
    0x5AF5U, 0x4AD4U, 0x7AB7U, 0x6A96U, 0x1A71U, 0x0A50U, 0x3A33U, 0x2A12U,
    0xDBFDU, 0xCBDCU, 0xFBBFU, 0xEB9EU, 0x9B79U, 0x8B58U, 0xBB3BU, 0xAB1AU,
    0x6CA6U, 0x7C87U, 0x4CE4U, 0x5CC5U, 0x2C22U, 0x3C03U, 0x0C60U, 0x1C41U,
    0xEDAEU, 0xFD8FU, 0xCDECU, 0xDDCDU, 0xAD2AU, 0xBD0BU, 0x8D68U, 0x9D49U,
    0x7E97U, 0x6EB6U, 0x5ED5U, 0x4EF4U, 0x3E13U, 0x2E32U, 0x1E51U, 0x0E70U,
    0xFF9FU, 0xEFBEU, 0xDFDDU, 0xCFFCU, 0xBF1BU, 0xAF3AU, 0x9F59U, 0x8F78U,
    0x9188U, 0x81A9U, 0xB1CAU, 0xA1EBU, 0xD10CU, 0xC12DU, 0xF14EU, 0xE16FU,
    0x1080U, 0x00A1U, 0x30C2U, 0x20E3U, 0x5004U, 0x4025U, 0x7046U, 0x6067U,
    0x83B9U, 0x9398U, 0xA3FBU, 0xB3DAU, 0xC33DU, 0xD31CU, 0xE37FU, 0xF35EU,
    0x02B1U, 0x1290U, 0x22F3U, 0x32D2U, 0x4235U, 0x5214U, 0x6277U, 0x7256U,
    0xB5EAU, 0xA5CBU, 0x95A8U, 0x8589U, 0xF56EU, 0xE54FU, 0xD52CU, 0xC50DU,
    0x34E2U, 0x24C3U, 0x14A0U, 0x0481U, 0x7466U, 0x6447U, 0x5424U, 0x4405U,
    0xA7DBU, 0xB7FAU, 0x8799U, 0x97B8U, 0xE75FU, 0xF77EU, 0xC71DU, 0xD73CU,
    0x26D3U, 0x36F2U, 0x0691U, 0x16B0U, 0x6657U, 0x7676U, 0x4615U, 0x5634U,
    0xD94CU, 0xC96DU, 0xF90EU, 0xE92FU, 0x99C8U, 0x89E9U, 0xB98AU, 0xA9ABU,
    0x5844U, 0x4865U, 0x7806U, 0x6827U, 0x18C0U, 0x08E1U, 0x3882U, 0x28A3U,
    0xCB7DU, 0xDB5CU, 0xEB3FU, 0xFB1EU, 0x8BF9U, 0x9BD8U, 0xABBBU, 0xBB9AU,
    0x4A75U, 0x5A54U, 0x6A37U, 0x7A16U, 0x0AF1U, 0x1AD0U, 0x2AB3U, 0x3A92U,
    0xFD2EU, 0xED0FU, 0xDD6CU, 0xCD4DU, 0xBDAAU, 0xAD8BU, 0x9DE8U, 0x8DC9U,
    0x7C26U, 0x6C07U, 0x5C64U, 0x4C45U, 0x3CA2U, 0x2C83U, 0x1CE0U, 0x0CC1U,
    0xEF1FU, 0xFF3EU, 0xCF5DU, 0xDF7CU, 0xAF9BU, 0xBFBAU, 0x8FD9U, 0x9FF8U,
    0x6E17U, 0x7E36U, 0x4E55U, 0x5E74U, 0x2E93U, 0x3EB2U, 0x0ED1U, 0x1EF0U
};

using namespace std;

static unordered_map <string, DeviceTypes_t> deviceIdMapping = {
    {"opp", Device384Nanopores},
    {"pup", Device384PatchClamp}}; /*! \todo FCON queste info dovrebbero risiedere nel DB */

/********************************************************************************************\
 *                                                                                          *
 *                                 MessageDispatcher                                        *
 *                                                                                          *
\********************************************************************************************/

/*****************\
 *  Ctor / Dtor  *
\*****************/

MessageDispatcher::MessageDispatcher(string deviceId) :
    deviceId(deviceId) {

}

MessageDispatcher::~MessageDispatcher() {

}

/************************\
 *  Connection methods  *
\************************/

ErrorCodes_t MessageDispatcher::connect() {
    if (connected) {
        return ErrorDeviceAlreadyConnected;
    }

    ErrorCodes_t ret;
    connected = true;

    /*! Calculate the LSB noise vector */
    this->initializeLsbNoise();

    /*! Allocate memory for compensations */
    this->initializeCompensations();

    stopConnectionFlag = false;

#ifdef DEBUGPRINT
    if (rxFid == nullptr) {
#ifdef _WIN32
        string path = string(getenv("HOMEDRIVE"))+string(getenv("HOMEPATH"));
#else
        string path = string(getenv("HOME"));
#endif
        stringstream ss;

        for (size_t i = 0; i < path.length(); ++i) {
            if (path[i] == '\\') {
                ss << "\\\\";

            } else {
                ss << path[i];
            }
        }
#ifdef _WIN32
        ss << "\\\\rx.txt";
#else
        ss << "/rx.txt";
#endif

        rxFid = fopen(ss.str().c_str(), "wb");
    }

    if (txFid == nullptr) {
#ifdef _WIN32
        string path = string(getenv("HOMEDRIVE"))+string(getenv("HOMEPATH"));
#else
        string path = string(getenv("HOME"));
#endif
        stringstream ss;

        for (size_t i = 0; i < path.length(); ++i) {
            if (path[i] == '\\') {
                ss << "\\\\";

            } else {
                ss << path[i];
            }
        }
#ifdef _WIN32
        ss << "\\\\tx.txt";
#else
        ss << "/tx.txt";
#endif

        txFid = fopen(ss.str().c_str(), "wb");
    }

    startPrintfTime = std::chrono::steady_clock::now();
#endif
#ifdef RAW_DEBUGPRINT
    if (rawRxFid == nullptr) {
#ifdef _WIN32
        string path = string(getenv("HOMEDRIVE"))+string(getenv("HOMEPATH"));
#else
        string path = string(getenv("HOME"));
#endif
        stringstream ss;

        for (int i = 0; i < path.length(); ++i) {
            if (path[i] == '\\') {
                ss << "\\\\";

            } else {
                ss << path[i];
            }
        }
#ifdef _WIN32
        ss << "\\\\rawRx.txt";
#else
        ss << "/rawRx.txt";
#endif

        rawRxFid = fopen(ss.str().c_str(), "wb");
    }
#endif

    rxThread = thread(&MessageDispatcher::readDataFromDevice, this);

    txThread = thread(&MessageDispatcher::sendCommandsToDevice, this);

    threadsStarted = true;

    this->resetFpga();

    this->resetChip(true);
    this_thread::sleep_for(chrono::milliseconds(100));
    ret = this->resetChip(false);
    if (ret != Success) {
        return ErrorConnectionChipResetFailed;
    }

    this->setSecondaryDeviceSwitch(false); /*! \todo FCON questo va propriamente settato per gestire device main e secondary */
    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        this->setVcCurrentOffsetDelta(idx, {0.0, UnitPfxNone, nullptr});
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    if (!connected) {
        return ErrorDeviceNotConnected;
    }

    uint16_t uType = (uint16_t)messageType;

    if (uType < MsgDirectionDeviceToEdr) {
        /*! This method controls only messages going from the device to the SW */
        uType += MsgDirectionDeviceToEdr;
    }

    rxEnabledTypesMap[uType] = flag;

    return Success;
}

ErrorCodes_t MessageDispatcher::disconnect() {
    if (!connected) {
        return ErrorDeviceNotConnected;
    }

    if (!stopConnectionFlag) {
        stopConnectionFlag = true;

        if (threadsStarted) {
            rxThread.join();
            txThread.join();
        }

        connected = false;

#ifdef DEBUGPRINT
        if (rxFid != nullptr) {
            fclose(rxFid);
            rxFid = nullptr;
        }

        if (txFid != nullptr) {
            fclose(txFid);
            txFid = nullptr;
        }
#endif
#ifdef RAW_DEBUGPRINT
        if (rawRxFid != nullptr) {
            fclose(rawRxFid);
            rawRxFid = nullptr;
        }
#endif

        return Success;

    } else {
        return ErrorDeviceNotConnected;
    }
}

ErrorCodes_t MessageDispatcher::getDeviceType(string deviceId, DeviceTypes_t &type) {
    if (deviceIdMapping.count(deviceId) > 0) {
        type = deviceIdMapping[deviceId];
        return Success;

    } else {
        return ErrorDeviceTypeNotRecognized;
    }
}

/****************\
 *  Tx methods  *
\****************/

ErrorCodes_t MessageDispatcher::ack(uint16_t heartbeat) {
    ErrorCodes_t ret;
    uint16_t dataLength = 1;
    vector <uint16_t> txDataMessage(dataLength);
    txDataMessage[0] = heartbeat;

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdAck, txDataMessage, dataLength);
    return ret;
}

ErrorCodes_t MessageDispatcher::nack(uint16_t heartbeat) {
    ErrorCodes_t ret;
    uint16_t dataLength = 1;
    vector <uint16_t> txDataMessage(dataLength);
    txDataMessage[0] = heartbeat;

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdNack, txDataMessage, dataLength);
    return ret;
}

ErrorCodes_t MessageDispatcher::ping() {
    ErrorCodes_t ret;
    uint16_t dataLength = 0;
    vector <uint16_t> txDataMessage(dataLength);

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdPing, txDataMessage, dataLength);
    if (ret != Success) {
        ret = ErrorConnectionPingFailed;
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::abort() {
    /*! \todo FCON non implementato in FW */
    return ErrorCommandNotImplemented;
//    uint16_t dataLength = 0;
//    vector <uint16_t> txDataMessage(dataLength);

//    return this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdAbort, txDataMessage, dataLength);
}

ErrorCodes_t MessageDispatcher::turnVoltageStimulusOn(bool on) {
    ErrorCodes_t ret;

    if (vcStimulusSwitchesNum > 0) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::turnCurrentStimulusOn(bool on) {
    ErrorCodes_t ret;

    if (ccStimulusSwitchesNum > 0) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::turnVoltageReaderOn(bool on) {
    ErrorCodes_t ret;

    /*! Voltage is read by current clamp ADC */
    if (ccReaderSwitchesNum > 0) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::turnCurrentReaderOn(bool on) {
    ErrorCodes_t ret;

    /*! Current is read by voltage clamp ADC */
    if (vcReaderSwitchesNum > 0) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setChannelsSources(int16_t voltageSourcesIdx, int16_t currentSourcesIdx) {
    ErrorCodes_t ret = Success;

    uint16_t dataLength = selectableTotalChannelsNum*2;
    vector <uint16_t> txDataMessage(dataLength);

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
        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
        selectedVoltageSourceIdx = voltageSourcesIdx;
        if (selectedVoltageSourceIdx == 1) {
            voltageTunerCorrection = 0.0;
        }
        selectedCurrentSourceIdx = currentSourcesIdx;
        if (selectedCurrentSourceIdx == 0) {
            currentTunerCorrection = 0.0;
        }
        this->selectChannelsResolutions();
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setVoltageHoldTuner(uint16_t channelIdx, Measurement_t voltage) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        voltageTuner = voltage;
        voltageTuner.convertValue(UnitPfxNone);

        uint16_t dataLength = 4;
        vector <uint16_t> txDataMessage(dataLength);
        this->int322uint16((int32_t)round(voltageTuner.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 0);
        txDataMessage[3] = txDataMessage[1];
        txDataMessage[1] = txDataMessage[0];
        txDataMessage[0] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
        txDataMessage[2] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            /*! \todo FCON qui bisognerebbe avere un array di valori e settare solo quello corrispondente */
            if (selectedVoltageSourceIdx == 0) {
                voltageTuner.convertValue(vcVoltageRange.prefix);
                voltageTunerCorrection = voltageTuner.value;

            } else {
                voltageTunerCorrection = 0.0;
            }
        }

    } else if (channelIdx == currentChannelsNum) {
        voltageTuner = voltage;
        voltageTuner.convertValue(UnitPfxNone);

        uint16_t dataLength = 4*currentChannelsNum;
        vector <uint16_t> txDataMessage(dataLength);
        for (channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            this->int322uint16((int32_t)round(voltageTuner.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, channelIdx*4);
            txDataMessage[3+channelIdx*4] = txDataMessage[1+channelIdx*4];
            txDataMessage[1+channelIdx*4] = txDataMessage[0+channelIdx*4];
        }

        for (channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            txDataMessage[0+channelIdx*4] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[2+channelIdx*4] = vcHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            /*! \todo FCON qui bisognerebbe avere un array di valori e settare solo quello corrispondente */
            voltageTuner.convertValue(vcVoltageRange.prefix);
            voltageTunerCorrection = voltageTuner.value;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setCurrentHoldTuner(uint16_t channelIdx, Measurement_t current) {
    ErrorCodes_t ret;

    if (channelIdx < voltageChannelsNum) {
        currentTuner = current;
        currentTuner.convertValue(UnitPfxNone);

        uint16_t dataLength = 4;
        vector <uint16_t> txDataMessage(dataLength);
        this->int322uint16((int32_t)round(currentTuner.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 0);
        txDataMessage[3] = txDataMessage[1];
        txDataMessage[1] = txDataMessage[0];
        txDataMessage[0] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
        txDataMessage[2] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            /*! \todo FCON qui bisognerebbe avere un array di valori e settare solo quello corrispondente */
            currentTuner.convertValue(ccCurrentRange.prefix);
            currentTunerCorrection = currentTuner.value;
        }

    } else if (channelIdx == voltageChannelsNum) {
        currentTuner = current;
        currentTuner.convertValue(UnitPfxNone);

        uint16_t dataLength = 4*voltageChannelsNum;
        vector <uint16_t> txDataMessage(dataLength);
        for (channelIdx = 0; channelIdx < voltageChannelsNum; channelIdx++) {
            this->int322uint16((int32_t)round(currentTuner.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, channelIdx*4);
            txDataMessage[3+channelIdx*4] = txDataMessage[1+channelIdx*4];
            txDataMessage[1+channelIdx*4] = txDataMessage[0+channelIdx*4];
        }

        for (channelIdx = 0; channelIdx < voltageChannelsNum; channelIdx++) {
            txDataMessage[0+channelIdx*4] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[2+channelIdx*4] = ccHoldTunerHwRegisterOffset+channelIdx*coreSpecificRegistersNum+1;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

        if (ret == Success) {
            /*! \todo FCON qui bisognerebbe avere un array di valori e settare solo quello corrispondente */
            if (selectedCurrentSourceIdx == 1) {
                currentTuner.convertValue(ccCurrentRange.prefix);
                currentTunerCorrection = currentTuner.value;

            } else {
                currentTunerCorrection = 0.0;
            }
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::turnOnLsbNoise(bool flag) {
    this->initializeLsbNoise(!flag);
    return Success;
}

ErrorCodes_t MessageDispatcher::setVCCurrentRange(uint16_t currentRangeIdx) {
    ErrorCodes_t ret;

    if (currentRangeIdx < vcCurrentRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            vcCurrentRange = vcCurrentRangesArray[currentRangeIdx];
            selectedVCCurrentRangeIdx = currentRangeIdx;
            vcCurrentResolution = vcCurrentRangesArray[selectedVCCurrentRangeIdx].step;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setCCCurrentRange(uint16_t currentRangeIdx) {
    ErrorCodes_t ret;

    if (currentRangeIdx < ccCurrentRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            ccCurrentRange = ccCurrentRangesArray[currentRangeIdx];
            selectedCCCurrentRangeIdx = currentRangeIdx;
            ccCurrentResolution = ccCurrentRangesArray[selectedCCCurrentRangeIdx].step;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setVCVoltageRange(uint16_t voltageRangeIdx) {
    ErrorCodes_t ret;

    if (voltageRangeIdx < vcVoltageRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            vcVoltageRange = vcVoltageRangesArray[voltageRangeIdx];
            selectedVCVoltageRangeIdx = voltageRangeIdx;
            vcVoltageResolution = vcVoltageRangesArray[selectedVCVoltageRangeIdx].step;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setCCVoltageRange(uint16_t voltageRangeIdx) {
    ErrorCodes_t ret;

    if (voltageRangeIdx < ccVoltageRangesNum) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            ccVoltageRange = ccVoltageRangesArray[voltageRangeIdx];
            selectedCCVoltageRangeIdx = voltageRangeIdx;
            ccVoltageResolution = ccVoltageRangesArray[selectedCCVoltageRangeIdx].step;
            this->selectChannelsResolutions();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setSamplingRate(uint16_t samplingRateIdx) {
    ErrorCodes_t ret;

    if (samplingRateIdx < samplingRatesNum) {
        uint16_t dataLength = 1;
        vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = samplingRateIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSamplingRate, txDataMessage, dataLength);

        /*! Make a chip reset to force resynchronization of chip states. This is important when the clock changes (SR slow to SR fast or vice versa) */
        this->resetChip(true);
        this_thread::sleep_for(chrono::milliseconds(resetDuration));
        this->resetChip(false);

        samplingRate = realSamplingRatesArray[samplingRateIdx];

        /*! Used for the digital filter */
        samplingRateKhz = samplingRate;
        samplingRateKhz.convertValue(UnitPfxKilo);
        if (filterActiveFlag) {
            /*! If the filter is active update the coefficients */
            this->computeFilterCoefficients();
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setFilterRatio(uint16_t filterRatioIdx) {
    ErrorCodes_t ret;

    if (filterRatioIdx < filterRatiosNum) {
        uint16_t dataLength = 1;
        vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = filterRatioIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdFilterRatio, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setUpsamplingRatio(uint16_t upsamplingRatioIdx) {
    ErrorCodes_t ret;

    if (upsamplingRatioIdx < upsamplingRatiosNum) {
        uint16_t dataLength = 1;
        vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = upsamplingRatioIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdUpsampling, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setDigitalFilter(double cutoffFrequency, bool lowPassFlag, bool activeFlag) {
    ErrorCodes_t ret;

    if ((cutoffFrequency > 0.0) || (cutoffFrequency < samplingRateKhz.value*0.5)) {
        filterCutoffFrequency = cutoffFrequency;
        filterLowPassFlag = lowPassFlag;
        filterActiveFlag = activeFlag;
        this->computeFilterCoefficients();

        ret = Success;

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::digitalOffsetCompensation(uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        this->setDigitalOffsetCompensationOverrideSwitch(false);
        uint16_t dataLength = 1;
        vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = channelIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdDigitalOffsetComp, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::digitalOffsetCompensationOverride(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    if (digitalOffsetCompensationOverrideImplemented) {
        if (channelIdx < currentChannelsNum) {
            ret = this->setDigitalOffsetCompensationOverrideSwitch(true);
            if (ret == Success) {
                ret = this->setDigitalOffsetCompensationOverrideValue(channelIdx, value);

                if (ret == Success) {
                    uint16_t dataLength = 1;
                    vector <uint16_t> txDataMessage(dataLength);
                    txDataMessage[0] = channelIdx;

                    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdDigitalOffsetComp, txDataMessage, dataLength);
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

ErrorCodes_t MessageDispatcher::setVcCurrentOffsetDelta(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    if (vcCurrentOffsetDeltaImplemented) {
        if (channelIdx < currentChannelsNum) {
            value.convertValue(vcCurrentRange.prefix);
            uint16_t dataLength = 2;
            vector <uint16_t> txDataMessage(dataLength);

            txDataMessage[0] = vcCurrentOffsetDeltaRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[1] = (uint16_t)((int16_t)round(value.value*SHORT_MAX/vcCurrentRange.max));

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

            return ret;

        } else {
            ret = ErrorValueOutOfRange;
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setCcVoltageOffsetDelta(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    if (vcCurrentOffsetDeltaImplemented) {
        if (channelIdx < currentChannelsNum) {
            value.convertValue(ccVoltageRange.prefix);
            uint16_t dataLength = 2;
            vector <uint16_t> txDataMessage(dataLength);

            txDataMessage[0] = ccVoltageOffsetDeltaRegisterOffset+channelIdx*coreSpecificRegistersNum;
            txDataMessage[1] = (uint16_t)((int16_t)round(value.value*SHORT_MAX/ccVoltageRange.max));

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

            return ret;

        } else {
            ret = ErrorValueOutOfRange;
        }

    } else {
        return ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::digitalOffsetCompensationInquiry(uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        uint16_t dataLength = 1;
        vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = channelIdx;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdDigitalOffsetCompInquiry, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::zap(Measurement_t duration, uint16_t channelIdx) {
    ErrorCodes_t ret;

    if (channelIdx < currentChannelsNum) {
        duration.convertValue(UnitPfxNone);

        uint16_t dataLength = 4;
        vector <uint16_t> txDataMessage(dataLength);
        this->int322uint16((int32_t)round(duration.value/protocolItemsTimeStep), txDataMessage, 0);
        txDataMessage[3] = txDataMessage[1];
        txDataMessage[1] = txDataMessage[0];
        txDataMessage[0] = zapDurationHwRegisterOffset;
        txDataMessage[2] = zapDurationHwRegisterOffset+1;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            dataLength = 1;
            txDataMessage[0] = channelIdx;

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdZap, txDataMessage, dataLength);
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setVoltageStimulusLpf(uint16_t filterIdx) {
    ErrorCodes_t ret;

    if (filterIdx < vcStimulusLpfOptionsNum) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            vcStimulusSelectedLpfOption = filterIdx;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setCurrentStimulusLpf(uint16_t filterIdx) {
    ErrorCodes_t ret;

    if (filterIdx < ccStimulusLpfOptionsNum) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
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

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            ccStimulusSelectedLpfOption = filterIdx;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::enableStimulus(uint16_t channelIdx, bool on) {
    ErrorCodes_t ret;

    if (!stimulusEnableImplemented) {
        ret  = ErrorFeatureNotImplemented;

    } else {
        if (channelIdx < currentChannelsNum) {
            uint16_t dataLength = switchesStatusLength;
            vector <uint16_t> txDataMessage(dataLength);
            this->switches2DataMessage(txDataMessage);

            if (on) {
                txDataMessage[stimulusEnableSwitchWord+channelIdx*coreSpecificSwitchesWordsNum] |=
                        stimulusEnableSwitchByte; // 1

            } else {
                txDataMessage[stimulusEnableSwitchWord+channelIdx*coreSpecificSwitchesWordsNum] &=
                        ~stimulusEnableSwitchByte; // 0
            }

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
            if (ret == Success) {
                this->dataMessage2Switches(txDataMessage);
            }

        } else {
            ret = ErrorValueOutOfRange;
        }
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::turnLedOn(uint16_t ledIndex, bool on) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    if (on) {
        txDataMessage[ledsWord[ledIndex]] &= ~ledsByte[ledIndex];

    } else {
        txDataMessage[ledsWord[ledIndex]] |= ledsByte[ledIndex];
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::setSlave(bool on) {
    ErrorCodes_t ret;
    if (slaveImplementedFlag) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (on) {
            txDataMessage[slaveWord] |= slaveByte;

        } else {
            txDataMessage[slaveWord] &= ~slaveByte;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setConstantSwitches() {
    ErrorCodes_t ret;

    uint16_t dataLength = switchesStatusLength;
    vector <uint16_t> txDataMessage(dataLength);
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

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setCompensationsChannel(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnVoltageCompensationsOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCurrentCompensationsOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnPipetteCompensationOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnCCPipetteCompensationOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnMembraneCompensationOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnResistanceCompensationOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnResistanceCorrectionOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnResistancePredictionOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnLeakConductanceCompensationOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::turnBridgeBalanceCompensationOn(bool) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setPipetteCompensationOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCCPipetteCompensationOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setMembraneCompensationOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCompensationOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCorrectionOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setLeakConductanceCompensationOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setBridgeBalanceCompensationOptions(uint16_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setPipetteCapacitance(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setCCPipetteCapacitance(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setMembraneCapacitance(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setAccessResistance(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCorrectionPercentage(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistanceCorrectionLag(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionGain(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionPercentage(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionBandwidthGain(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setResistancePredictionTau(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setLeakConductance(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setBridgeBalanceResistance(Measurement_t) {
    return ErrorCommandNotImplemented;
}

ErrorCodes_t MessageDispatcher::setDigitalTriggerOutput(uint16_t triggerIdx, bool terminator, bool polarity, uint16_t triggerId, Measurement_t delay) {
    ErrorCodes_t ret;

    if ((triggerIdx < maxDigitalTriggerOutputEvents) &&
            triggerId < digitalTriggersNum) {

        delay.convertValue(UnitPfxNone);

        uint16_t dataLength = 4;
        vector <uint16_t> txDataMessage(dataLength);

        txDataMessage[0] = triggerIdx;
        txDataMessage[1] = (terminator ? 0x8000 : 0x0000) + (polarity ? 0x0100 : 0x0000) + triggerId;
        this->int322uint16((int32_t)round(delay.value/protocolItemsTimeStep), txDataMessage, 2);

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdDigitalTriggerOutput, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest) {
    ErrorCodes_t ret;

    if (itemsNum <= maxProtocolItemsNum) {
        vRest.convertValue(UnitPfxNone);

        uint16_t dataLength = 5;
        vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = protId;
        txDataMessage[1] = itemsNum;
        txDataMessage[2] = sweepsNum;
        this->int322uint16((int32_t)round(vRest.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 3);

        stepsOnLastSweep = (double)(sweepsNum-1);
        protocolItemsNum = itemsNum;
        protocolItemIndex = 0;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdVoltageProtocolStruct, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::voltStepTimeStep(Measurement_t v0, Measurement_t vStep, Measurement_t t0, Measurement_t tStep,
                                                 uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    v0.convertValue(UnitPfxNone);
    vStep.convertValue(UnitPfxNone);
    t0.convertValue(UnitPfxNone);
    tStep.convertValue(UnitPfxNone);

    if ((this->checkVoltStepTimeStepParameters(v0.value, vStep.value, t0.value, tStep.value, repsNum, applySteps) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(v0.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 0);
            this->int322uint16((int32_t)round(vStep.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 2);
            this->int322uint16((int32_t)round(t0.value/protocolItemsTimeStep), txDataMessage, 4);
            this->int322uint16((int32_t)round(tStep.value/protocolItemsTimeStep), txDataMessage, 6);
            txDataMessage[8] = 0xF00D;
            txDataMessage[9] = 0xBAAD;
            txDataMessage[10] = 0xF00D;
            txDataMessage[11] = 0xBAAD;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdVoltageStepTimeStep, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::voltRamp(Measurement_t v0, Measurement_t vFinal, Measurement_t t,
                                         uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    v0.convertValue(UnitPfxNone);
    vFinal.convertValue(UnitPfxNone);
    t.convertValue(UnitPfxNone);

    if ((this->checkVoltRampParameters(v0.value, vFinal.value, t.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(v0.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(vFinal.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(t.value/protocolItemsTimeStep), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdVoltageRamp, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::voltSin(Measurement_t v0, Measurement_t vAmp, Measurement_t freq,
                                        uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    v0.convertValue(UnitPfxNone);
    vAmp.convertValue(UnitPfxNone);
    freq.convertValue(UnitPfxNone);

    if ((this->checkVoltSinParameters(v0.value, vAmp.value, freq.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(v0.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(vAmp.value/protocolItemsVoltageStep[selectedVCVoltageRangeIdx]), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(freq.value/protocolItemsFrequencyStep), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdVoltageSin, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::startProtocol() {
    ErrorCodes_t ret;

    if (startProtocolCommandImplemented) {
        uint16_t dataLength = 0;
        vector <uint16_t> txDataMessage(dataLength);

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdStartProtocol, txDataMessage, dataLength);

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest) {
    ErrorCodes_t ret;

    if (itemsNum <= maxProtocolItemsNum) {
        iRest.convertValue(UnitPfxNone);

        uint16_t dataLength = 5;
        vector <uint16_t> txDataMessage(dataLength);
        txDataMessage[0] = protId;
        txDataMessage[1] = itemsNum;
        txDataMessage[2] = sweepsNum;
        this->int322uint16((int32_t)round(iRest.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 3);

        stepsOnLastSweep = (double)(sweepsNum-1);
        protocolItemsNum = itemsNum;
        protocolItemIndex = 0;

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdCurrentProtocolStruct, txDataMessage, dataLength);

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::currStepTimeStep(Measurement_t i0, Measurement_t iStep, Measurement_t t0, Measurement_t tStep,
                                                 uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    i0.convertValue(UnitPfxNone);
    iStep.convertValue(UnitPfxNone);
    t0.convertValue(UnitPfxNone);
    tStep.convertValue(UnitPfxNone);

    if ((this->checkCurrStepTimeStepParameters(i0.value, iStep.value, t0.value, tStep.value, repsNum, applySteps) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(i0.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 0);
            this->int322uint16((int32_t)round(iStep.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 2);
            this->int322uint16((int32_t)round(t0.value/protocolItemsTimeStep), txDataMessage, 4);
            this->int322uint16((int32_t)round(tStep.value/protocolItemsTimeStep), txDataMessage, 6);
            txDataMessage[8] = 0xF00D;
            txDataMessage[9] = 0xBAAD;
            txDataMessage[10] = 0xF00D;
            txDataMessage[11] = 0xBAAD;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdCurrentStepTimeStep, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::currRamp(Measurement_t i0, Measurement_t iFinal, Measurement_t t,
                                         uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    i0.convertValue(UnitPfxNone);
    iFinal.convertValue(UnitPfxNone);
    t.convertValue(UnitPfxNone);

    if ((this->checkCurrRampParameters(i0.value, iFinal.value, t.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(i0.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(iFinal.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(t.value/protocolItemsTimeStep), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdCurrentRamp, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::currSin(Measurement_t i0, Measurement_t iAmp, Measurement_t freq,
                                        uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps) {
    ErrorCodes_t ret;

    i0.convertValue(UnitPfxNone);
    iAmp.convertValue(UnitPfxNone);
    freq.convertValue(UnitPfxNone);

    if ((this->checkCurrSinParameters(i0.value, iAmp.value, freq.value) && (currentItem < protocolItemsNum))) {
        if (nextItem <= ++protocolItemIndex) {
            uint16_t dataLength = 16;
            vector <uint16_t> txDataMessage(dataLength);
            this->int322uint16((int32_t)round(i0.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 0);
            txDataMessage[2] = 0;
            txDataMessage[3] = 0;
            this->int322uint16((int32_t)round(iAmp.value/protocolItemsCurrentStep[selectedCCCurrentRangeIdx]), txDataMessage, 4);
            txDataMessage[6] = 0;
            txDataMessage[7] = 0;
            this->int322uint16((int32_t)round(freq.value/protocolItemsFrequencyStep), txDataMessage, 8);
            txDataMessage[10] = 0;
            txDataMessage[11] = 0;
            txDataMessage[12] = currentItem;
            txDataMessage[13] = nextItem;
            txDataMessage[14] = repsNum;
            txDataMessage[15] = applySteps;

            ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdCurrentSin, txDataMessage, dataLength);

        } else {
            ret = ErrorBadlyFormedProtocolLoop;
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::resetChip(bool reset) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    unsigned int resetIdx = ResetIndexChip;

    if (reset) {
        txDataMessage[resetWord[resetIdx]] |= resetByte[resetIdx];

    } else {
        txDataMessage[resetWord[resetIdx]] &= ~resetByte[resetIdx];
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::resetDigitalOffsetCompensation(bool reset) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    unsigned int resetIdx = ResetIndexDigitalOffsetCompensation;

    if (reset) {
        txDataMessage[resetWord[resetIdx]] |= resetByte[resetIdx];

    } else {
        txDataMessage[resetWord[resetIdx]] &= ~resetByte[resetIdx];
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::resetFpga() {
    ErrorCodes_t ret;
    uint16_t dataLength = 0;
    vector <uint16_t> txDataMessage(dataLength);
    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdFpgaReset, txDataMessage, dataLength);
    voltageOffsetCorrected = 0.0;
    this->selectVoltageOffsetResolution();
    return ret;
}

//ErrorCodes_t MessageDispatcher::getCalibrationConfiguration(CalibrationConfiguration_t * &calibrationConfiguration) {
//    calibrationConfiguration = calConf;
//    return Success;
//}

ErrorCodes_t MessageDispatcher::getCalibrationEepromSize(uint32_t &size) {
    ErrorCodes_t ret;
    if (calEeprom != nullptr) {
        size = calEeprom->getMemorySize();
        ret = Success;

    } else {
        size = 0;
        ret = ErrorEepromNotConnected;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::writeCalibrationEeprom(vector <uint32_t> value, vector <uint32_t> address, vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (calEeprom != nullptr) {
        unique_lock <mutex> connectionMutexLock(connectionMutex);

        ret = this->pauseConnection(true);
        calEeprom->openConnection();

        unsigned char eepromBuffer[4];
        for (unsigned int itemIdx = 0; itemIdx < value.size(); itemIdx++) {
            for (uint32_t bufferIdx = 0; bufferIdx < size[itemIdx]; bufferIdx++) {
                eepromBuffer[size[itemIdx]-bufferIdx-1] = value[itemIdx] & 0x000000FF;
                value[itemIdx] >>= 8;
            }

            ret = calEeprom->writeBytes(eepromBuffer, address[itemIdx], size[itemIdx]);
        }

        calEeprom->closeConnection();
        this->pauseConnection(false);

        connectionMutexLock.unlock();

        RxOutput_t rxOutput;
        ret = ErrorUnknown;
        rxOutput.msgTypeId = MsgDirectionDeviceToEdr+MsgTypeIdInvalid;
        int pingTries = 0;

        while (ret != Success) {
            if (pingTries++ > FTD_MAX_PING_TRIES) {
                return ErrorConnectionPingFailed;
            }

            ret = this->ping();
            if (ret != Success) {
                ret = this->pauseConnection(true);
                calEeprom->openConnection();

                calEeprom->closeConnection();
                this->pauseConnection(false);
            }
        }

        /*! Make a chip reset to force resynchronization of chip states. This is important when the FPGA has just been reset */
        this->resetChip(true);
        this_thread::sleep_for(chrono::milliseconds(1));
        this->resetChip(false);

    } else {
        ret = ErrorEepromNotConnected;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::readCalibrationEeprom(vector <uint32_t> &value, vector <uint32_t> address, vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (calEeprom != nullptr) {
        unique_lock <mutex> connectionMutexLock(connectionMutex);

        ret = this->pauseConnection(true);
        calEeprom->openConnection();

        if (value.size() != address.size()) {
            value.resize(address.size());
        }

        unsigned char eepromBuffer[4];
        for (unsigned int itemIdx = 0; itemIdx < value.size(); itemIdx++) {
            ret = calEeprom->readBytes(eepromBuffer, address[itemIdx], size[itemIdx]);

            value[itemIdx] = 0;
            for (uint32_t bufferIdx = 0; bufferIdx < size[itemIdx]; bufferIdx++) {
                value[itemIdx] <<= 8;
                value[itemIdx] += static_cast <uint32_t> (eepromBuffer[bufferIdx]);
            }
        }

        calEeprom->closeConnection();
        this->pauseConnection(false);

        connectionMutexLock.unlock();

        RxOutput_t rxOutput;
        ret = ErrorUnknown;
        rxOutput.msgTypeId = MsgDirectionDeviceToEdr+MsgTypeIdInvalid;
        int pingTries = 0;

        while (ret != Success) {
            if (pingTries++ > FTD_MAX_PING_TRIES) {
                return ErrorConnectionPingFailed;
            }

            ret = this->ping();
        }

        /*! Make a chip reset to force resynchronization of chip states. This is important when the FPGA has just been reset */
        this->resetChip(true);
        this_thread::sleep_for(chrono::milliseconds(1));
        this->resetChip(false);

    } else {
        ret = ErrorEepromNotConnected;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::getSwitchesStatus(vector <uint16_t> &words, vector <vector <string>> &names) {
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

ErrorCodes_t MessageDispatcher::singleSwitchDebug(uint16_t word, uint16_t bit, bool flag) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    vector <uint16_t> txDataMessage(dataLength);
    this->switches2DataMessage(txDataMessage);

    if (flag) {
        txDataMessage[word] |= (0x0001 << bit);

    } else {
        txDataMessage[word] &= ~(0x0001 << bit);
    }

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(txDataMessage);
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::multiSwitchDebug(vector <uint16_t> words) {
    ErrorCodes_t ret;
    uint16_t dataLength = switchesStatusLength;
    /*! vector <uint16_t> txDataMessage(dataLength); not needed, because it is identical to words */

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, words, dataLength);
    if (ret == Success) {
        this->dataMessage2Switches(words);
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::singleRegisterDebug(uint16_t index, uint16_t value) {
    ErrorCodes_t ret;
    uint16_t dataLength = 2;
    vector <uint16_t> txDataMessage(dataLength);

    txDataMessage[0] = index;
    txDataMessage[1] = value;

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);
    return ret;
}

/****************\
 *  Rx methods  *
\****************/

ErrorCodes_t MessageDispatcher::isDeviceUpgradable(string &upgradeNotes, string &notificationTag) {
    ErrorCodes_t ret = Success;

    upgradeNotes = this->upgradeNotes;
    notificationTag = this->notificationTag;
    if (upgradeNotes == "") {
        ret = ErrorUpgradesNotAvailable;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::getDeviceInfo(string &deviceId, string &deviceName, uint8_t &deviceVersion, uint8_t &deviceSubversion, uint32_t &firmwareVersion) {
    ErrorCodes_t ret = Success;

    deviceId = this->deviceId;
    deviceName = this->deviceName;
    DeviceTuple_t tuple = ftdiEeprom->getDeviceTuple();
    deviceVersion = tuple.version;
    deviceSubversion = tuple.subversion;
    firmwareVersion = tuple.fwVersion;

    return ret;
}

ErrorCodes_t MessageDispatcher::getNextMessage(RxOutput_t &rxOutput) {
    ErrorCodes_t ret = Success;
    double xFlt;

    unique_lock <mutex> rxMutexLock (rxMutex);
    if (rxMsgBufferReadLength <= 0) {
        rxMsgBufferNotEmpty.wait_for(rxMutexLock, chrono::milliseconds(1000));
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
            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionHeader):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionData):
                if ((msgReadCount+1)*rxDataMessageMaxLen > E4GCL_OUT_STRUCT_DATA_LEN) {
                    /*! Exit if the merge may exceed the data field length */
                    exitLoop = true;

                } else {
                    exitLoop = false;
                }
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionTail):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdFpgaReset):
            case (MsgDirectionDeviceToEdr+MsgTypeIdDigitalOffsetComp):
            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionSaturation):
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
            case (MsgDirectionDeviceToEdr+MsgTypeIdFpgaReset):
                lastParsedMsgType = MsgTypeIdFpgaReset;
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdDigitalOffsetComp):
                rxOutput.dataLen = 1;
                rxOutput.channelIdx = * (rxDataBuffer+dataOffset);
                rawFloat = * (rxDataBuffer+((dataOffset+1)&FTD_RX_DATA_BUFFER_MASK));
                voltageOffsetCorrected = (((double)rawFloat)-liquidJunctionOffsetBinary)*liquidJunctionResolution;
                this->selectVoltageOffsetResolution();
                rxOutput.data[0] = (int16_t)(rawFloat-liquidJunctionOffsetBinary);
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToEdr;

                this->setDigitalOffsetCompensationOverrideValue(rxOutput.channelIdx, {voltageOffsetCorrected, liquidJunctionControl.prefix, nullptr});
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionHeader):
                if (lastParsedMsgType != MsgTypeIdAcquisitionHeader) {
                    /*! Evaluate only if it's the first repetition */
                    rxOutput.dataLen = 0;
                    rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                    rxOutput.protocolItemIdx = * (rxDataBuffer+((dataOffset+1)&FTD_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolRepsIdx = * (rxDataBuffer+((dataOffset+2)&FTD_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolSweepIdx = * (rxDataBuffer+((dataOffset+3)&FTD_RX_DATA_BUFFER_MASK));
                    lastParsedMsgType = MsgTypeIdAcquisitionHeader;
                    headerSet = true;

                } else {
                    if (!headerSet) {
                        /*! This message is a repetition and should not be passed to SW only if the first header message was not processed (headerSet) during this call */
                        ret = ErrorRepeatedHeader;
                    }
                }
                break;

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionData):
                if (rxMsgBuffer[rxMsgBufferReadOffset].dataLength < FTD_RX_MIN_DATA_PACKET_VALID_LEN) {
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
                    dataOffset = (dataOffset+2)&FTD_RX_DATA_BUFFER_MASK;

                    for (uint16_t idx = 0; idx < timeSamplesNum; idx++) {
                        /*! \todo FCON questo doppio ciclo va modificato per raccogliere i dati di impedenza in modalità lock-in */
                        for (uint16_t voltageChannelIdx = 0; voltageChannelIdx < voltageChannelsNum; voltageChannelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyVoltageFilter(voltageChannelIdx, ((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx]);
                            xFlt = iirY[voltageChannelIdx][iirOff];
                            rxOutput.data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&FTD_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&LSB_NOISE_ARRAY_MASK;
                        }

                        for (uint16_t currentChannelIdx = 0; currentChannelIdx < currentChannelsNum; currentChannelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyCurrentFilter(currentChannelIdx+voltageChannelsNum, ((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx]);
                            xFlt = iirY[currentChannelIdx+voltageChannelsNum][iirOff];
                            rxOutput.data[dataWritten+sampleIdx++] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&FTD_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&LSB_NOISE_ARRAY_MASK;
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

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionTail):
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

            case (MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionSaturation):
                rxOutput.dataLen = rxMsgBuffer[rxMsgBufferReadOffset].dataLength;
                for (uint16_t dataIdx = 0; dataIdx < rxOutput.dataLen; dataIdx++) {
                    rxOutput.uintData[dataIdx] = * (rxDataBuffer+dataOffset);
                    dataOffset = (dataOffset+1)&FTD_RX_DATA_BUFFER_MASK;
                }
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToEdr;
                break;

            default:
                lastParsedMsgType = MsgTypeIdInvalid;
                break;
            }

            msgReadCount++;
            rxMsgBufferReadOffset = (rxMsgBufferReadOffset+1)&FTD_RX_MSG_BUFFER_MASK;
        }
    }

    rxMutexLock.lock();
    rxMsgBufferReadLength -= msgReadCount;
    rxMsgBufferNotFull.notify_all();
    rxMutexLock.unlock();

    return ret;
}

ErrorCodes_t MessageDispatcher::getChannelsNumber(uint32_t &currentChannelsNumber, uint32_t &voltageChannelsNumber) {
    currentChannelsNumber = currentChannelsNum;
    voltageChannelsNumber = voltageChannelsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getAvailableChannelsSources(ChannelSources_t &voltageSourcesIdxs, ChannelSources_t &currentSourcesIdxs) {
    voltageSourcesIdxs = availableVoltageSourcesIdxs;
    currentSourcesIdxs = availableCurrentSourcesIdxs;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasVoltageHoldTuner() {
    if (voltageHoldTunerImplemented) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::hasCurrentHoldTuner() {
    if (currentHoldTunerImplemented) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::getVCCurrentRanges(vector <RangedMeasurement_t> &currentRanges) {
    currentRanges = vcCurrentRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentRanges(vector <RangedMeasurement_t> &currentRanges) {
    currentRanges = ccCurrentRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCCurrentRange(RangedMeasurement_t &currentRange) {
    currentRange = vcCurrentRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCCurrentRange(RangedMeasurement_t &currentRange) {
    currentRange = ccCurrentRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageRanges(vector <RangedMeasurement_t> &voltageRanges) {
    voltageRanges = vcVoltageRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRanges(vector <RangedMeasurement_t> &voltageRanges) {
    voltageRanges = ccVoltageRangesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVCVoltageRange(RangedMeasurement_t &voltageRange) {
    voltageRange = vcVoltageRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCCVoltageRange(RangedMeasurement_t &voltageRange) {
    voltageRange = ccVoltageRange;
    return Success;
}

ErrorCodes_t MessageDispatcher::getSamplingRates(vector <Measurement_t> &samplingRates) {
    samplingRates = samplingRatesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getRealSamplingRates(vector <Measurement_t> &samplingRates) {
    samplingRates = realSamplingRatesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getFilterRatios(vector <Measurement_t> &filterRatios) {
    filterRatios = filterRatiosArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getUpsamplingRatios(vector <Measurement_t> &upsamplingRatios) {
    upsamplingRatios = upsamplingRatiosArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageProtocolRange(unsigned int rangeIdx, RangedMeasurement_t &voltageProtocolRange) {
    voltageProtocolRange.min = protocolItemsMinVoltage[rangeIdx];
    voltageProtocolRange.max = protocolItemsMaxVoltage[rangeIdx];
    voltageProtocolRange.step = protocolItemsVoltageGuiStep[rangeIdx];
    voltageProtocolRange.prefix = UnitPfxNone;
//    voltageProtocolRange.unit = "V";
    voltageProtocolRange.convertValues(vcVoltageRangesArray[rangeIdx].prefix);
    return Success;
}

ErrorCodes_t MessageDispatcher::getCurrentProtocolRange(unsigned int rangeIdx, RangedMeasurement_t &currentProtocolRange) {
    currentProtocolRange.min = protocolItemsMinCurrent[rangeIdx];
    currentProtocolRange.max = protocolItemsMaxCurrent[rangeIdx];
    currentProtocolRange.step = protocolItemsCurrentGuiStep[rangeIdx];
    currentProtocolRange.prefix = UnitPfxNone;
//    currentProtocolRange.unit = "A";
    currentProtocolRange.convertValues(ccCurrentRangesArray[rangeIdx].prefix);
    return Success;
}

ErrorCodes_t MessageDispatcher::getTimeProtocolRange(RangedMeasurement_t &timeProtocolRange) {
    timeProtocolRange.min = protocolItemsMinTime;
    timeProtocolRange.max = protocolItemsMaxTime;
    timeProtocolRange.step = protocolItemsTimeStep;
    timeProtocolRange.prefix = UnitPfxNone;
//    timeProtocolRange.unit = "s";
    return Success;
}

ErrorCodes_t MessageDispatcher::getFrequencyProtocolRange(RangedMeasurement_t &frequencyProtocolRange) {
    frequencyProtocolRange.min = protocolItemsMinFrequency;
    frequencyProtocolRange.max = protocolItemsMaxFrequency;
    frequencyProtocolRange.step = protocolItemsFrequencyRangeStep;
    frequencyProtocolRange.prefix = UnitPfxNone;
//    frequencyProtocolRange.unit = "Hz";
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxOutputTriggers(unsigned int &maxTriggersNum) {
    maxTriggersNum = maxDigitalTriggerOutputEvents;
    return Success;
}

ErrorCodes_t MessageDispatcher::getOutputTriggersNum(unsigned int &triggersNum) {
    triggersNum = digitalTriggersNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getMaxProtocolItems(unsigned int &maxItemsNum) {
    maxItemsNum = maxProtocolItemsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasProtocolStep() {
    if (protocolStepFlag) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::hasProtocolRamp() {
    if (protocolRampFlag) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::hasProtocolSin() {
    if (protocolSinFlag) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::getClampingModalities(vector <uint16_t> &clampingModalities) {
    clampingModalities = clampingModalitiesArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::multimeterStuckHazard(bool &stuckFlag) {
    stuckFlag = multimeterStuckHazardFlag;
    return Success;
}

ErrorCodes_t MessageDispatcher::getVoltageStimulusLpfs(vector <string> &filterOptions) {
    filterOptions = vcStimulusLpfOptions;
    return Success;
}

ErrorCodes_t MessageDispatcher::getCurrentStimulusLpfs(vector <string> &filterOptions) {
    filterOptions = ccStimulusLpfOptions;
    return Success;
}

ErrorCodes_t MessageDispatcher::getLedsNumber(uint16_t &ledsNumber) {
    ledsNumber = ledsNum;
    return Success;
}

ErrorCodes_t MessageDispatcher::getLedsColors(vector <uint32_t> &ledsColors) {
    ledsColors = ledsColorsArray;
    return Success;
}

ErrorCodes_t MessageDispatcher::hasSlaveModality() {
    if (slaveImplementedFlag) {
        return Success;

    } else {
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t MessageDispatcher::hasPipetteCompensation() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasCCPipetteCompensation() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasMembraneCompensation() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasResistanceCompensation() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasResistanceCorrection() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasResistancePrediction() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasLeakConductanceCompensation() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::hasBridgeBalanceCompensation() {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getPipetteCompensationOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCCPipetteCompensationOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getMembraneCompensationOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCompensationOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCorrectionOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductanceCompensationOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceCompensationOptions(char *) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLiquidJunctionControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (liquidJunctionControl.implemented) {
        control = liquidJunctionControl;
        ret = Success;
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::getPipetteCapacitanceControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getCCPipetteCapacitanceControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getMembraneCapacitanceControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getAccessResistanceControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCorrectionPercentageControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistanceCorrectionLagControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionGainControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionPercentageControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionBandwidthGainControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getResistancePredictionTauControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getLeakConductanceControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t MessageDispatcher::getBridgeBalanceResistanceControl(CompensationControl_t &) {
    return ErrorFeatureNotImplemented;
}

/*********************\
 *  Private methods  *
\*********************/

ErrorCodes_t MessageDispatcher::checkExpirationDate() {
    ErrorCodes_t ret;

    time_t t = time(nullptr);
    struct tm * now = localtime(&t);
    vector <uint16_t> * expireDate = ftdiEeprom->getExpirationDate();

    int32_t expirationYear = (int32_t)(expireDate->at(DateYearIdx));
    int32_t expirationMonth = (int32_t)(expireDate->at(DateMonthIdx));
    int32_t expirationDay = (int32_t)(expireDate->at(DateDayIdx));

    if (now->tm_year+1900 > expirationYear) {
        ret = ErrorExpiredDevice;

    } else if (now->tm_year+1900 < expirationYear) {
        ret = Success;

    } else {
        if (now->tm_mon+1 > expirationMonth) {
            ret = ErrorExpiredDevice;

        } else if (now->tm_mon+1 < expirationMonth) {
            ret = Success;

        } else {
            if (now->tm_mday+1 > expirationDay) {
                ret = ErrorExpiredDevice;

            } else {
                ret = Success;
            }
        }
    }
    return ret;
}

ErrorCodes_t MessageDispatcher::initFtdiChannel(FT_HANDLE * handle, char channel) {
    FT_STATUS ftRet;

    string communicationSerialNumber = deviceId+channel;

    /*! Opens the device */
    ftRet = FT_OpenEx((PVOID)communicationSerialNumber.c_str(), FT_OPEN_BY_SERIAL_NUMBER, handle);
    if (ftRet != FT_OK) {
        return ErrorDeviceConnectionFailed;
    }

    /*! Sets latency */
    ftRet = FT_SetLatencyTimer(* handle, 2); /*!< ms */
    if (ftRet != FT_OK) {
        FT_Close(* handle);
        return ErrorFtdiConfigurationFailed;
    }

    /*! Sets transfers size to */
    ftRet = FT_SetUSBParameters(* handle, 4096, 4096);
    if (ftRet != FT_OK) {
        FT_Close(* handle);
        return ErrorFtdiConfigurationFailed;
    }

    /*! Purges buffers */
    ftRet = FT_Purge(* handle, FT_PURGE_RX | FT_PURGE_TX);
    if (ftRet != FT_OK) {
        FT_Close(* handle);
        return ErrorFtdiConfigurationFailed;
    }

    if (channel == rxChannel) {
        /*! Set a notification for the received byte event */
        DWORD EventMask;
        EventMask = FT_EVENT_RXCHAR;

#ifdef FTD_POLLING
#else
#if defined(_WIN32)
        rxQueuedByteEvent = CreateEventA(nullptr, false, false, "ftdiRxEvent");
        ftRet = FT_SetEventNotification(* ftdiRxHandle, EventMask, rxQueuedByteEvent);

#elif defined(__APPLE__)
        pthread_mutex_init(&rxQueuedByteEventHandle.eMutex, nullptr);
        pthread_cond_init(&rxQueuedByteEventHandle.eCondVar, nullptr);
        ftRet = FT_SetEventNotification(* ftdiRxHandle, EventMask, (PVOID)&rxQueuedByteEventHandle);
#endif
#endif
        if (ftRet != FT_OK) {
            FT_Close(* handle);
            return ErrorFtdiConfigurationFailed;
        }
    }

    return Success;
}

void MessageDispatcher::initializeLsbNoise(bool nullValues) {
    if (nullValues) {
        /*! By default there is no added noise  */
        for (int32_t i = 0; i < LSB_NOISE_ARRAY_SIZE; i++) {
            lsbNoiseArray[i] = 0.0;
        }

    } else {
        mt19937 mtRng((uint32_t)time(nullptr));
        double den = (double)0xFFFFFFFF;
        for (int32_t i = 0; i < LSB_NOISE_ARRAY_SIZE; i++) {
            lsbNoiseArray[i] = ((double)mtRng())/den-0.5;
        }
    }
}

void MessageDispatcher::initializeCompensations() {
    /*! Nothing to be done for no patch clamp devices */
}

void MessageDispatcher::readAndParseMessages() {
    FT_STATUS ftRet;
    DWORD ftdiQueuedBytes = 0;
    DWORD ftdiReadBytes;
    uint32_t readTries = 0;
    uint32_t minQueuedBytes = FTD_FRAME_SIZE+rxDataMessageMaxLen*sizeof(uint16_t);
    chrono::milliseconds longBytesWait(2);
    chrono::milliseconds shortBytesWait(1);

    RxParsePhase_t rxParsePhase = RxParseLookForHeader;

    /*! Variables used to access the rx raw buffer */
    uint32_t rxRawBufferWriteOffset = 0;
    uint32_t rxRawBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed  */
    uint32_t rxRawBufferReadLength = 0; /*!< Lenght of the part of the buffer to be processed */
    uint32_t rxRawBufferReadIdx = 0; /*!< Index being processed wrt rxRawBufferReadOffset */

    /*! Variables used to access the rx msg buffer */
    uint32_t rxMsgBufferWriteOffset = 0;

    /*! Variables used to access the rx data buffer */
    uint32_t rxDataBufferWriteOffset = 0;
    uint32_t rxDataBufferWriteIdx = 0; /*!< Index being written wrt rxDataBufferWriteOffset */

    bool notEnoughRxData;

    /*! Rx sync word variables */
    bool rxSyncOk;

    /*! Rx heartbeat variables */
    uint16_t rxHeartbeat = 0x0000;

    /*! Rx message type ID variables */
    uint16_t rxMsgTypeId = MsgDirectionDeviceToEdr+MsgTypeIdInvalid;
    bool rxExpectAck = false;

    /*! Rx data length variables */
    uint16_t rxDataWords = 0;
#ifdef CHECK_DATA_PACKET_LENGTH
    uint16_t rxDataWordsRemoved = 0;
    uint16_t rxDataWordsAdded = 0;
#endif
    uint16_t rxDataBytes = 0;

    /*! Rx message crc variables */
    uint16_t rxReadCrc0 = 0x0000;
    uint16_t rxReadCrc1;
    uint16_t rxComputedCrc = 0x0000;
    bool rxCrcOk;

    parsingFlag = true;

    unique_lock <mutex> connectionMutexLock (connectionMutex);
    connectionMutexLock.unlock();

    while ((!stopConnectionFlag) || (txWaitingOnAcks > 0)) {

        /******************\
         *  Reading part  *
        \******************/

#ifndef FTD_POLLING
        if (ftdiQueuedBytes == 0) {
#if defined(_WIN32)
            WaitForSingleObject(rxQueuedByteEvent, 1000);

#elif defined(__APPLE__)
            pthread_mutex_lock(&rxQueuedByteEventHandle.eMutex);
            clock_gettime(CLOCK_REALTIME, &rxWaitForByteTimeout);
            rxWaitForByteTimeout.tv_sec += 1;
            pthread_cond_timedwait(&rxQueuedByteEventHandle.eCondVar, &rxQueuedByteEventHandle.eMutex, &rxWaitForByteTimeout);
            pthread_mutex_unlock(&rxQueuedByteEventHandle.eMutex);
#endif

        } else {
#if defined(_WIN32)
            WaitForSingleObject(rxQueuedByteEvent, 2);

#elif defined(__APPLE__)
            pthread_mutex_lock(&rxQueuedByteEventHandle.eMutex);
            clock_gettime(CLOCK_REALTIME, &rxWaitForByteTimeout);
            rxWaitForByteTimeout.tv_nsec += 2000000;
            pthread_cond_timedwait(&rxQueuedByteEventHandle.eCondVar, &rxQueuedByteEventHandle.eMutex, &rxWaitForByteTimeout);
            pthread_mutex_unlock(&rxQueuedByteEventHandle.eMutex);
#endif
        }
#endif

        /*! Read queue status to check the number of available bytes */
        connectionMutexLock.lock();
        ftRet = FT_GetQueueStatus(* ftdiRxHandle, &ftdiQueuedBytes);
        if (ftRet != FT_OK) {
            connectionMutexLock.unlock();
            continue;
        }

        if (ftdiQueuedBytes == 0) {
            connectionMutexLock.unlock();
            /*! If there are no bytes in the queue skip to next iteration in the while loop */
#ifdef FTD_POLLING
            this_thread::sleep_for(longBytesWait);
#endif
            continue;

        } else if ((ftdiQueuedBytes < minQueuedBytes) && (readTries == 0)) {
            connectionMutexLock.unlock();
            readTries++;
#ifdef FTD_POLLING
            this_thread::sleep_for(shortBytesWait);
#endif
            continue;
        }
        readTries = 0;

        /*! Reads the data */
        uint32_t bytesToEnd = FTD_RX_RAW_BUFFER_SIZE-rxRawBufferWriteOffset;
        if (ftdiQueuedBytes >= bytesToEnd) {
            ftRet = FT_Read(* ftdiRxHandle, rxRawBuffer+rxRawBufferWriteOffset, bytesToEnd, &ftdiReadBytes);

        } else {
            ftRet = FT_Read(* ftdiRxHandle, rxRawBuffer+rxRawBufferWriteOffset, ftdiQueuedBytes, &ftdiReadBytes);
        }
        connectionMutexLock.unlock();

        if ((ftRet != FT_OK) || (ftdiReadBytes == 0)) {
            if (ftRet != FT_OK) {
                cout << "OOOOOOOOPPPPPPPPPSSSSS" << endl; /*! \todo FCON togliere/sostituire */
            }
            continue;
        }

#ifdef RAW_DEBUGPRINT
        fwrite(rxRawBuffer+rxRawBufferWriteOffset, sizeof(unsigned char), ftdiReadBytes, rawRxFid);
        fflush(rawRxFid);
#endif
        if (rxRawBufferWriteOffset == 0) {
            rxRawBuffer[FTD_RX_RAW_BUFFER_SIZE] = rxRawBuffer[0]; /*!< The last item is a copy of the first one, it used to safely read 2 consecutive bytes at a time to form a 16bit word,
                                                                   *   even if the first byte is in position FTD_RX_RAW_BUFFER_SIZE-1 and the following one would go out of range otherwise */
        }

        /*! Update rxRawBufferWriteOffset to position to be written on next iteration */
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+ftdiReadBytes)&FTD_RX_RAW_BUFFER_MASK;

        /******************\
         *  Parsing part  *
        \******************/

        rxRawBufferReadLength += ftdiReadBytes;
        notEnoughRxData = false;

        while (!notEnoughRxData) {
            switch (rxParsePhase) {
            case RxParseLookForHeader:
                /*! Look for header */
                if (rxRawBufferReadLength < FTD_RX_SYNC_WORD_SIZE) {
                    notEnoughRxData = true;

                } else {
                    rxSyncOk = true;
                    for (rxRawBufferReadIdx = 0; rxRawBufferReadIdx < FTD_RX_SYNC_WORD_SIZE; rxRawBufferReadIdx++) {
                        if (rxRawBuffer[(rxRawBufferReadOffset+rxRawBufferReadIdx)&FTD_RX_RAW_BUFFER_MASK] != rxSyncWord[rxRawBufferReadIdx]) {
                            rxSyncOk = false;
                            break;
                        }
                    }

                    if (rxSyncOk) {
                        rxMsgOffset = rxRawBufferReadOffset;
                        rxParsePhase = RxParseLookForLength;
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxRawBufferReadIdx)&FTD_RX_RAW_BUFFER_MASK;
                        rxRawBufferReadLength -= rxRawBufferReadIdx;

                    } else {
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxRawBufferReadIdx+1)&FTD_RX_RAW_BUFFER_MASK;
                        rxRawBufferReadLength -= rxRawBufferReadIdx+1;
                    }
                }
                break;

            case RxParseLookForLength:
                /*! Look for length */
                if (rxRawBufferReadLength < FTD_RX_HB_TY_LN_SIZE+FTD_RX_CRC_WORD_SIZE) {
                    notEnoughRxData = true;

                } else {
                    rxRawBufferReadIdx = 0;
                    rxHeartbeat = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+rxRawBufferReadIdx)&FTD_RX_RAW_BUFFER_MASK)));
                    rxRawBufferReadIdx += 2;

                    rxMsgTypeId = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+rxRawBufferReadIdx)&FTD_RX_RAW_BUFFER_MASK)));
                    rxRawBufferReadIdx += 2;
                    rxExpectAck = rxExpectAckMap[rxMsgTypeId];

                    rxDataWords = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+rxRawBufferReadIdx)&FTD_RX_RAW_BUFFER_MASK)));
                    rxRawBufferReadIdx += 2;

                    rxReadCrc0 = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+rxRawBufferReadIdx)&FTD_RX_RAW_BUFFER_MASK)));
                    rxRawBufferReadIdx += 2;

                    rxComputedCrc = this->rxCrc16Ccitt(rxRawBufferReadOffset, FTD_RX_HB_TY_LN_SIZE, rxCrcInitialValue);
                    rxCrcOk = (rxReadCrc0 == rxComputedCrc);

                    if (rxCrcOk) {
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+FTD_RX_HB_TY_LN_SIZE+FTD_RX_CRC_WORD_SIZE)&FTD_RX_RAW_BUFFER_MASK;
                        rxRawBufferReadLength -= FTD_RX_HB_TY_LN_SIZE+FTD_RX_CRC_WORD_SIZE;

                        rxDataBytes = rxDataWords*FTD_RX_WORD_SIZE;
                        rxMsgBytes = FTD_RX_SYNC_WORD_SIZE+FTD_RX_HB_TY_LN_SIZE+FTD_RX_CRC_WORD_SIZE+rxDataBytes+FTD_RX_CRC_WORD_SIZE;

                        rxParsePhase = RxParseLookForCrc;
#ifdef RX_PACKETS_FLOW_PRINT
                        cout << "crc0 right" << endl;
#endif

                    } else {
                        rxParsePhase = RxParseLookForHeader;
#ifdef DEBUGPRINT
                        fprintf(rxFid,
                                "crc0 wrong\n"
                                "hb: \t0x%04x\n\n",
                                rxHeartbeat);
                        fflush(rxFid);
#endif
#ifdef RX_PACKETS_FLOW_PRINT
                        cout << "crc0 wrong" << endl;
#endif
                    }
                }
                break;

            case RxParseLookForCrc:
                /*! Look for CRC */
                if (rxRawBufferReadLength < rxDataBytes+FTD_RX_CRC_WORD_SIZE) {
                    notEnoughRxData = true;

                } else {
                    rxReadCrc1 = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+rxDataBytes)&FTD_RX_RAW_BUFFER_MASK)));
                    rxComputedCrc = this->rxCrc16Ccitt(rxRawBufferReadOffset, rxDataBytes, rxComputedCrc);
                    rxCrcOk = (rxReadCrc1 == rxComputedCrc);

                    if (rxCrcOk) {
#ifdef RX_PACKETS_FLOW_PRINT
                        cout << "crc1 right" << endl;
#endif
                        if (rxMsgTypeId == MsgDirectionDeviceToEdr+MsgTypeIdAck) {
                            txAckMutex.lock();
                            txAckReceived = true;
                            txAckCv.notify_all();
#ifdef RX_PACKETS_FLOW_PRINT
                            cout << "ack" << endl;
#endif
#ifdef DEBUGPRINT
                            fprintf(rxFid,
                                    "ack recd\n"
                                    "hb: \t0x%04x\n\n",
                                    * ((uint16_t *)(rxRawBuffer+rxRawBufferReadOffset)));
                            fflush(rxFid);
#endif
                            txAckMutex.unlock();

                        } else if (rxMsgTypeId == MsgDirectionDeviceToEdr+MsgTypeIdNack) {
                            /*! \todo FCON NACK should not be written but used to manage tx, maybe forcing rewriting? hard to implement */
#ifdef RX_PACKETS_FLOW_PRINT
                            cout << "nack" << endl;
#endif
#ifdef DEBUGPRINT
                            fprintf(rxFid,
                                    "nack recd\n"
                                    "hb: \t0x%04x\n\n",
                                    * ((uint16_t *)(rxRawBuffer+rxRawBufferReadOffset)));
                            fflush(rxFid);
#endif

                        } else if (rxMsgTypeId == MsgDirectionDeviceToEdr+MsgTypeIdPing) {
                            this->ack(rxHeartbeat);

                        } else {
                            unique_lock <mutex> rxMutexLock (rxMutex);
                            while (rxMsgBufferReadLength >= FTD_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
                                txMsgBufferNotFull.wait(rxMutexLock);
                            }
                            rxMutexLock.unlock();

                            rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat;
                            rxMsgBuffer[rxMsgBufferWriteOffset].typeId = rxMsgTypeId;
                            rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;

#ifdef DEBUGPRINT
                            currentPrintfTime = std::chrono::steady_clock::now();
                            fprintf(rxFid,
                                    "%d us\n"
                                    "recd message\n"
                                    "crc1 ok\n"
                                    "hb: \t0x%04x\n"
                                    "typeID:\t0x%04x\n"
                                    "length:\t0x%04x\n"
                                    "crc0:\t0x%04x\n",
                                    (int)(std::chrono::duration_cast <std::chrono::microseconds> (currentPrintfTime-startPrintfTime).count()),
                                    rxHeartbeat,
                                    rxMsgTypeId,
                                    rxDataWords,
                                    rxReadCrc0);
#endif

                            if (rxMsgTypeId == MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionData) {
                                /*! In MsgTypeIdAcquisitionData the last word of the payload contains the number of valid data samples */
                                if (rxDataWords < FTD_RX_MIN_DATA_PACKET_LEN) {
                                    rxParsePhase = RxParseLookForHeader;
#ifdef DEBUGPRINT
                                    fprintf(rxFid,
                                            "short data packet\n"
                                            "words: \t%d\n\n",
                                            rxDataWords);
                                    fflush(rxFid);
#endif
                                    continue;

                                } else {

#ifdef CHECK_DATA_PACKET_LENGTH
                                    rxDataWords = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*(rxDataWords-1))&FTD_RX_RAW_BUFFER_MASK)))+rxDataWordsAdded;
                                    rxDataWordsRemoved = (rxDataWords-2) % totalChannelsNum;
                                    rxDataWords -= rxDataWordsRemoved;
#else
                                    rxDataWords = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*(rxDataWords-1))&FTD_RX_RAW_BUFFER_MASK)));
#endif

#ifdef DEBUGPRINT
#ifdef CHECK_DATA_PACKET_LENGTH
                                    fprintf(rxFid,
                                            "vlen:\t0x%04x\n"
                                            "vlen+:\t0x%04x\n"
                                            "vlen-:\t0x%04x\n",
                                            rxDataWords,
                                            rxDataWordsAdded,
                                            rxDataWordsRemoved);
#else
                                    fprintf(rxFid, "vlen:\t0x%04x\n",
                                            rxDataWords);
#endif
#endif
                                }
                            }

                            if (rxDataWords >= E4GCL_OUT_STRUCT_DATA_LEN) {
                                rxParsePhase = RxParseLookForHeader;
#ifdef DEBUGPRINT
                                fprintf(rxFid,
                                        "long data packet\n"
                                        "words: \t%d\n\n",
                                        rxDataWords);
                                fflush(rxFid);
#endif
                                continue;
                            }

                            if (rxEnabledTypesMap[rxMsgTypeId]) {
                                /*! Update the message buffer only if the message is not filtered out */
                                rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = rxDataWords;
                                rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&FTD_RX_MSG_BUFFER_MASK;
                            }

#ifdef CHECK_DATA_PACKET_LENGTH
                            /*! It seems to work better if we just forget about the additional data */
//                            if (rxMsgTypeId == MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionData) {
//                                for (rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWordsAdded; rxDataBufferWriteIdx++) {
//                                    rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx)&FTD_RX_DATA_BUFFER_MASK] = movedWords[rxDataBufferWriteIdx];
//                                }
//                                rxDataBufferWriteOffset = (rxDataBufferWriteOffset+rxDataWordsAdded)&FTD_RX_DATA_BUFFER_MASK;
//                                rxDataWordsAdded = 0;
//                            }
#endif

                            for (rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
                                rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx)&FTD_RX_DATA_BUFFER_MASK] = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*rxDataBufferWriteIdx)&FTD_RX_RAW_BUFFER_MASK)));
#ifdef DEBUGPRINT
#ifndef RX_DATA_PRINT
                                if (rxDataBufferWriteIdx < 4) {
#endif
                                    fprintf(rxFid, "data%d:\t0x%04x\n",
                                            rxDataBufferWriteIdx,
                                            rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx)&FTD_RX_DATA_BUFFER_MASK]);
#ifndef RX_DATA_PRINT
                                }
#endif
#endif
                            }

#ifdef CHECK_DATA_PACKET_LENGTH
//                            if (rxMsgTypeId == MsgDirectionDeviceToEdr+MsgTypeIdAcquisitionData) {
//                                for (rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWordsRemoved; rxDataBufferWriteIdx++) {
//                                    movedWords[rxDataBufferWriteIdx] = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*rxDataBufferWriteIdx)&FTD_RX_RAW_BUFFER_MASK)));;
//                                }
//                                rxDataWordsAdded = rxDataWordsRemoved; /*!< Remember to add these values with the next packet */
//                            }
#endif

#ifdef DEBUGPRINT
                            fprintf(rxFid, "crc1:\t0x%04x\n\n",
                                    rxReadCrc1);
                            fflush(rxFid);
#endif

                            rxDataBufferWriteOffset = (rxDataBufferWriteOffset+rxDataWords)&FTD_RX_DATA_BUFFER_MASK;

                            if (rxDataBufferWriteOffset <= (uint32_t)rxDataWords) {
                                rxDataBuffer[FTD_RX_DATA_BUFFER_SIZE] = rxDataBuffer[0]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word,
                                                                                          *   even if the first 16bit word is in position FTD_RX_DATA_BUFFER_SIZE-1 and the following one would go out of range otherwise */
                            }

                            rxRawBufferReadOffset = (rxRawBufferReadOffset+rxDataBytes+FTD_RX_CRC_WORD_SIZE)&FTD_RX_RAW_BUFFER_MASK;
                            rxRawBufferReadLength -= rxDataBytes+FTD_RX_CRC_WORD_SIZE;

                            if (rxExpectAck) {
                                this->ack(rxHeartbeat);
                            }

                            if (rxEnabledTypesMap[rxMsgTypeId]) {
                                /*! change the message buffer length only if the message is not filtered out */
                                rxMutexLock.lock();
                                rxMsgBufferReadLength++;
                                rxMsgBufferNotEmpty.notify_all();
                                rxMutexLock.unlock();
                            }
                        }

                    } else {

#ifdef DEBUGPRINT
                        currentPrintfTime = std::chrono::steady_clock::now();
                        fprintf(rxFid,
                                "%d us\n"
                                "recd message\n"
                                "crc1 wrong\n"
                                "hb: \t0x%04x\n"
                                "typeID:\t0x%04x\n"
                                "length:\t0x%04x\n"
                                "crc0:\t0x%04x\n",
                                (int)(std::chrono::duration_cast <std::chrono::microseconds> (currentPrintfTime-startPrintfTime).count()),
                                rxHeartbeat,
                                rxMsgTypeId,
                                rxDataWords,
                                rxReadCrc0);

                        for (rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
#ifndef RX_DATA_PRINT
                            if (rxDataBufferWriteIdx < 4) {
#endif
                                fprintf(rxFid, "data%d:\t0x%04x\n",
                                        rxDataBufferWriteIdx,
                                        * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*rxDataBufferWriteIdx)&FTD_RX_RAW_BUFFER_MASK))));
#ifndef RX_DATA_PRINT
                            }
#endif
                        }

                        fprintf(rxFid,
                                "crc1:\t0x%04x\n"
                                "crc2:\t0x%04x\n\n",
                                rxReadCrc1,
                                rxComputedCrc);
                        fflush(rxFid);
#endif

                        rxRawBufferReadOffset = (rxMsgOffset+FTD_RX_SYNC_WORD_SIZE)&FTD_RX_RAW_BUFFER_MASK;
                        rxRawBufferReadLength += FTD_RX_HB_TY_LN_SIZE;
#ifdef RX_PACKETS_FLOW_PRINT
                        cout << "crc1 wrong" << endl;
#endif
#ifdef DEBUGPRINT
                        fprintf(rxFid,
                                "crc1 wrong\n"
                                "hb: \t0x%04x\n\n",
                                rxHeartbeat);
                        fflush(rxFid);
#endif
                        if (rxExpectAck) {
                            this->nack(rxHeartbeat);
                        }
                    }
                    rxParsePhase = RxParseLookForHeader;
                }
                break;
            }
        }
    }

    if (rxMsgBufferReadLength <= 0) {
        rxMutex.lock();
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
        rxMutex.unlock();
    }
}

void MessageDispatcher::unwrapAndSendMessages() {
    FT_STATUS ftRet;
    DWORD bytesToWrite;
    DWORD ftdiWrittenBytes;
    int writeTries = 0;

    /*! Variables used to access the tx raw buffer */
    uint32_t txRawBufferReadIdx = 0; /*!< Index being processed wrt buffer  */

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed  */

    /*! Variables used to access the tx data buffer */
    uint32_t txDataBufferReadOffset = 0;
    uint32_t txDataBufferReadIdx; /*!< Index being read wrt txDataBufferReadOffset */

    bool notSentTxData;

    /*! Tx data length variables */
    uint16_t txDataWords;
    uint16_t txDataBytes;

    /*! Tx message crc variables */
    uint16_t txComputedCrc;

    unique_lock <mutex> txMutexLock (txMutex);
    unique_lock <mutex> connectionMutexLock (connectionMutex);
    txMutexLock.unlock();
    connectionMutexLock.unlock();

    while ((!stopConnectionFlag) || (txWaitingOnAcks > 0)) {

        /*********************\
         *  Unwrapping part  *
        \*********************/

        txMutexLock.lock();
        while (txMsgBufferReadLength <= 0) {
            txMsgBufferNotEmpty.wait_for(txMutexLock, chrono::milliseconds(100));
            if (stopConnectionFlag && (txWaitingOnAcks == 0)) {
                break;
            }
        }
        txMutexLock.unlock();
        if (stopConnectionFlag && (txWaitingOnAcks == 0)) {
            continue;
        }

        txRawBufferReadIdx = FTD_TX_SYNC_WORD_SIZE; /*! Sync word has already been written during initialization */
        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txMsgBuffer[txMsgBufferReadOffset].heartbeat;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;
        unsigned short tempHb = txMsgBuffer[txMsgBufferReadOffset].heartbeat;

        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txMsgBuffer[txMsgBufferReadOffset].typeId;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;
        unsigned short tempId = txMsgBuffer[txMsgBufferReadOffset].typeId;

        txDataBufferReadOffset = txMsgBuffer[txMsgBufferReadOffset].startDataPtr;

        txDataWords = txMsgBuffer[txMsgBufferReadOffset].dataLength;
        txDataBytes = txDataWords*2;
        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txDataWords;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;

        txComputedCrc = txCrc16Ccitt(FTD_TX_SYNC_WORD_SIZE, FTD_TX_HB_TY_LN_SIZE, txCrcInitialValue);
        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txComputedCrc;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;

#ifdef DEBUGPRINT
        currentPrintfTime = std::chrono::steady_clock::now();
        fprintf(txFid,
                "%d us\n"
                "sent message\n"
                "sync:\t0x%02x%02x\n"
                "hb: \t0x%02x%02x\n"
                "typeID:\t0x%02x%02x\n"
                "length:\t0x%02x%02x\n"
                "crc0:\t0x%02x%02x\n",
                (int)(std::chrono::duration_cast <std::chrono::microseconds> (currentPrintfTime-startPrintfTime).count()),
                txRawBuffer[1], txRawBuffer[0],
                txRawBuffer[3], txRawBuffer[2],
                txRawBuffer[5], txRawBuffer[4],
                txRawBuffer[7], txRawBuffer[6],
                txRawBuffer[9], txRawBuffer[8]);
#endif

        for (txDataBufferReadIdx = 0; txDataBufferReadIdx < txDataWords; txDataBufferReadIdx++) {
            * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txDataBuffer[(txDataBufferReadOffset+txDataBufferReadIdx)&FTD_TX_DATA_BUFFER_MASK];
            txRawBufferReadIdx += FTD_TX_WORD_SIZE;

#ifdef DEBUGPRINT
#ifndef RX_DATA_PRINT
            if (txDataBufferReadIdx < 4) {
#endif
                fprintf(txFid,
                        "data%d:\t0x%02x%02x\n",
                        txDataBufferReadIdx,
                        txRawBuffer[txRawBufferReadIdx-1], txRawBuffer[txRawBufferReadIdx-2]);

#ifndef RX_DATA_PRINT
            }
#endif
#endif
        }

        txComputedCrc = txCrc16Ccitt(FTD_TX_SYNC_WORD_SIZE+FTD_TX_HB_TY_LN_SIZE+FTD_TX_CRC_WORD_SIZE, txDataBytes, txComputedCrc);
        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txComputedCrc;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;

#ifdef DEBUGPRINT
        fprintf(txFid,
                "crc1:\t0x%02x%02x\n\n",
                txRawBuffer[txRawBufferReadIdx-1], txRawBuffer[txRawBufferReadIdx-2]);
        fflush(txFid);
#endif

        txMsgBufferReadOffset = (txMsgBufferReadOffset+1)&FTD_TX_MSG_BUFFER_MASK;

        /******************\
         *  Sending part  *
        \******************/

        notSentTxData = true;
        bytesToWrite = ((DWORD)txDataBytes)+FTD_BYTES_TO_WRITE_ALWAYS;
        while (notSentTxData && (writeTries++ < FTD_MAX_WRITE_TRIES)) { /*! \todo FCON prevedere un modo per notificare ad alto livello e all'utente */
            connectionMutexLock.lock();
            ftRet = FT_Write(* ftdiTxHandle, txRawBuffer, bytesToWrite, &ftdiWrittenBytes);
            connectionMutexLock.unlock();

            if (ftRet != FT_OK) {
                continue;
            }

            /*! If less bytes than need are sent purge the buffer and retry */
            if (ftdiWrittenBytes < bytesToWrite) {
                /*! Cleans TX buffer */
                connectionMutexLock.lock();
                ftRet = FT_Purge(* ftdiTxHandle, FT_PURGE_TX);
                connectionMutexLock.unlock();

            } else {
                notSentTxData = false;
                writeTries = 0;
            }
        }

        txMutexLock.lock();
        txMsgBufferReadLength--;
        txMsgBufferNotFull.notify_all();
        txMutexLock.unlock();
    }
}

ErrorCodes_t MessageDispatcher::manageOutgoingMessageLife(uint16_t msgTypeId, vector <uint16_t> &txDataMessage, uint16_t dataLen) {
    while (dataLen > txDataMessageMaxLen) {
        vector <uint16_t> txDataMessageMax(txDataMessage.begin(), txDataMessage.begin()+txDataMessageMaxLen);
        this->manageOutgoingMessageLife(msgTypeId, txDataMessageMax, txDataMessageMaxLen);
        txDataMessage.erase(txDataMessage.begin(), txDataMessage.begin()+txDataMessageMaxLen);
        dataLen -= txDataMessageMaxLen;
    }

    ErrorCodes_t ret = ErrorSendMessageFailed;
    bool txExpectAck = txExpectAckMap[msgTypeId];
    if (txExpectAck) {
        txWaitingOnAcks++;
    }
    uint32_t waitTime = 1000;

    if (!txExpectAck) {
        this->wrapOutgoingMessage(msgTypeId, txDataMessage, dataLen);
        ret = Success;

    } else {
        unique_lock <mutex> ackLock(txAckMutex);
        txAckReceived = false;
        int resendTry = 0;
        while ((!txAckReceived) && (resendTry < FTD_MAX_RESEND_TRIES)) {
            ackLock.unlock();

            this->wrapOutgoingMessage(msgTypeId, txDataMessage, dataLen);

            ackLock.lock();
            txAckCv.wait_for(ackLock, chrono::milliseconds(waitTime));
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
    }
    return ret;
}

void MessageDispatcher::wrapOutgoingMessage(uint16_t msgTypeId, vector <uint16_t> &txDataMessage, uint16_t dataLen) {
    unique_lock <mutex> txMutexLock (txMutex);
    while (txMsgBufferReadLength >= FTD_TX_MSG_BUFFER_SIZE) {
        txMsgBufferNotFull.wait(txMutexLock);
    }

    txMsgBuffer[txMsgBufferWriteOffset].heartbeat = txHeartbeat;
    txHeartbeat = (txHeartbeat+1)&FTD_TX_HEARTBEAT_MASK;

    txMsgBuffer[txMsgBufferWriteOffset].typeId = msgTypeId;

    txMsgBuffer[txMsgBufferWriteOffset].startDataPtr = txDataBufferWriteOffset;

    txMsgBuffer[txMsgBufferWriteOffset].dataLength = dataLen;

    for (uint16_t idx = 0; idx < dataLen; idx++) {
        txDataBuffer[txDataBufferWriteOffset] = txDataMessage[idx];
        txDataBufferWriteOffset = (txDataBufferWriteOffset+1)&FTD_TX_DATA_BUFFER_MASK;
    }

    if (txDataBufferWriteOffset <= (uint32_t)dataLen) {
        txDataBuffer[FTD_TX_DATA_BUFFER_SIZE] = txDataBuffer[0]; /*!< The last item is a copy of the first one, it used to safely read 2 consecutive 16bit words at a time to form a 32bit word,
                                                                  *   even if the first 16bit word is in position FTD_RX_DATA_BUFFER_SIZE-1 and the following one would go out of range otherwise */
    }

    txMsgBufferWriteOffset = (txMsgBufferWriteOffset+1)&FTD_TX_MSG_BUFFER_MASK;
    txMsgBufferReadLength++;
    txMsgBufferNotEmpty.notify_all();
}

uint16_t MessageDispatcher::rxCrc16Ccitt(uint32_t offset, uint16_t len, uint16_t crc) {
    for (uint32_t idx = 0; idx < len; idx++) {
        uint16_t tmp = (crc >> 8) ^ (uint16_t)rxRawBuffer[(offset++)&FTD_RX_RAW_BUFFER_MASK];
        crc = ((uint16_t)(crc << 8)) ^ crc16CcittTable[tmp];
    }
    return crc;
}

uint16_t MessageDispatcher::txCrc16Ccitt(uint32_t offset, uint16_t len, uint16_t crc) {
    for (uint32_t idx = 0; idx < len; idx++) {
        uint16_t tmp = (crc >> 8) ^ (uint16_t)txRawBuffer[offset++];
        crc = ((uint16_t)(crc << 8)) ^ crc16CcittTable[tmp];
    }
    return crc;
}

void MessageDispatcher::int322uint16(int32_t from, vector <uint16_t> &to, size_t offset) {
    to[offset] = from & 0xFFFF;
    to[offset+1] = (from >> 16) & 0xFFFF;
}

void MessageDispatcher::uint322uint16(uint32_t from, vector <uint16_t> &to, size_t offset) {
    to[offset] = from & 0xFFFF;
    to[offset+1] = (from >> 16) & 0xFFFF;
}

ErrorCodes_t MessageDispatcher::setDigitalOffsetCompensationOverrideSwitch(bool flag) {
    ErrorCodes_t ret;

    if (digitalOffsetCompensationOverrideImplemented) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (flag) {
            txDataMessage[digitalOffsetCompensationOverrideSwitchWord] |= digitalOffsetCompensationOverrideSwitchByte;

        } else {
            txDataMessage[digitalOffsetCompensationOverrideSwitchWord] &= ~digitalOffsetCompensationOverrideSwitchByte;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setSecondaryDeviceSwitch(bool flag) {
    ErrorCodes_t ret;

    if (startProtocolCommandImplemented) {
        uint16_t dataLength = switchesStatusLength;
        vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        if (flag) {
            txDataMessage[startProtocolSwitchWord] |= startProtocolSwitchByte;

        } else {
            txDataMessage[startProtocolSwitchWord] &= ~startProtocolSwitchByte;
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
        }

    } else {
        ret = ErrorFeatureNotImplemented;
    }

    return ret;
}

ErrorCodes_t MessageDispatcher::setDigitalOffsetCompensationOverrideValue(uint16_t channelIdx, Measurement_t value) {
    ErrorCodes_t ret;

    value.convertValue(liquidJunctionControl.prefix);
    uint16_t dataLength = 2;
    vector <uint16_t> txDataMessage(dataLength);

    txDataMessage[0] = digitalOffsetCompensationOverrideRegisterOffset+channelIdx*coreSpecificRegistersNum;
    uint16_t uintValue = (uint16_t)((int16_t)round(-value.value/liquidJunctionResolution/16.0)*16.0); /*! \todo FCON sta schifezza serve a tenere i 4 bit meno significativi a zero */
    txDataMessage[1] = uintValue;

    ret = this->manageOutgoingMessageLife(MsgDirectionEdrToDevice+MsgTypeIdRegistersCtrl, txDataMessage, dataLength);

    return ret;
}

bool MessageDispatcher::checkVoltStepTimeStepParameters(double v0, double vStep, double t0, double tStep, uint16_t repsNum, uint16_t applySteps) {
    double minVWithMargin = protocolItemsMinVoltage[selectedVCVoltageRangeIdx]-protocolItemsVoltageStep[selectedVCVoltageRangeIdx]*0.5;
    double maxVWithMargin = protocolItemsMaxVoltage[selectedVCVoltageRangeIdx]+protocolItemsVoltageStep[selectedVCVoltageRangeIdx]*0.5;
    double minTWithMargin = protocolItemsMinTime-protocolItemsTimeStep*0.5;
    double maxTWithMargin = protocolItemsMaxTime+protocolItemsTimeStep*0.5;
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

bool MessageDispatcher::checkVoltRampParameters(double v0, double vFinal, double t) {
    double minVWithMargin = protocolItemsMinVoltage[selectedVCVoltageRangeIdx]-protocolItemsVoltageStep[selectedVCVoltageRangeIdx]*0.5;
    double maxVWithMargin = protocolItemsMaxVoltage[selectedVCVoltageRangeIdx]+protocolItemsVoltageStep[selectedVCVoltageRangeIdx]*0.5;
    double minTWithMargin = protocolItemsMinTime-protocolItemsTimeStep*0.5;
    double maxTWithMargin = protocolItemsMaxTime+protocolItemsTimeStep*0.5;
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

bool MessageDispatcher::checkVoltSinParameters(double v0, double vAmp, double freq) {
    double minVWithMargin = protocolItemsMinVoltage[selectedVCVoltageRangeIdx]-protocolItemsVoltageStep[selectedVCVoltageRangeIdx]*0.5;
    double maxVWithMargin = protocolItemsMaxVoltage[selectedVCVoltageRangeIdx]+protocolItemsVoltageStep[selectedVCVoltageRangeIdx]*0.5;
    double minFWithMargin = protocolItemsMinFrequency-protocolItemsFrequencyStep*0.5;
    double maxFWithMargin = protocolItemsMaxFrequency+protocolItemsFrequencyStep*0.5;
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

bool MessageDispatcher::checkCurrStepTimeStepParameters(double i0, double iStep, double t0, double tStep, uint16_t repsNum, uint16_t applySteps) {
    double minIWithMargin = protocolItemsMinCurrent[selectedCCCurrentRangeIdx]-protocolItemsCurrentStep[selectedCCCurrentRangeIdx]*0.5;
    double maxIWithMargin = protocolItemsMaxCurrent[selectedCCCurrentRangeIdx]+protocolItemsCurrentStep[selectedCCCurrentRangeIdx]*0.5;
    double minTWithMargin = protocolItemsMinTime-protocolItemsTimeStep*0.5;
    double maxTWithMargin = protocolItemsMaxTime+protocolItemsTimeStep*0.5;
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

bool MessageDispatcher::checkCurrRampParameters(double i0, double iFinal, double t) {
    double minIWithMargin = protocolItemsMinCurrent[selectedCCCurrentRangeIdx]-protocolItemsCurrentStep[selectedCCCurrentRangeIdx]*0.5;
    double maxIWithMargin = protocolItemsMaxCurrent[selectedCCCurrentRangeIdx]+protocolItemsCurrentStep[selectedCCCurrentRangeIdx]*0.5;
    double minTWithMargin = protocolItemsMinTime-protocolItemsTimeStep*0.5;
    double maxTWithMargin = protocolItemsMaxTime+protocolItemsTimeStep*0.5;
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

bool MessageDispatcher::checkCurrSinParameters(double i0, double iAmp, double freq) {
    double minIWithMargin = protocolItemsMinCurrent[selectedCCCurrentRangeIdx]-protocolItemsCurrentStep[selectedCCCurrentRangeIdx]*0.5;
    double maxIWithMargin = protocolItemsMaxCurrent[selectedCCCurrentRangeIdx]+protocolItemsCurrentStep[selectedCCCurrentRangeIdx]*0.5;
    double minFWithMargin = protocolItemsMinFrequency-protocolItemsFrequencyStep*0.5;
    double maxFWithMargin = protocolItemsMaxFrequency+protocolItemsFrequencyStep*0.5;
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

void MessageDispatcher::switches2DataMessage(vector <uint16_t> &txDataMessage) {
    for (uint16_t idx = 0; idx < switchesStatusLength; idx++) {
        txDataMessage[idx] = switchesStatus[idx];
    }
}

void MessageDispatcher::dataMessage2Switches(vector <uint16_t> &txDataMessage) {
    for (uint16_t idx = 0; idx < switchesStatusLength; idx++) {
        switchesStatus[idx] = txDataMessage[idx];
    }
}

void MessageDispatcher::initializeFilterVariables() {
    iirX = new double * [totalChannelsNum];
    iirU = new double * [totalChannelsNum];
    iirY = new double * [totalChannelsNum];
    for (unsigned int channelIdx = 0; channelIdx < totalChannelsNum; channelIdx++) {
        iirX[channelIdx] = new double[IIR_ORD+1];
        iirU[channelIdx] = new double[IIR_ORD+1];
        iirY[channelIdx] = new double[IIR_ORD+1];
        for (int tapIdx = 0; tapIdx < IIR_ORD+1; tapIdx++) {
            iirX[channelIdx][tapIdx] = 0.0;
            iirU[channelIdx][tapIdx] = 0.0;
            iirY[channelIdx][tapIdx] = 0.0;
        }
    }
}

void MessageDispatcher::computeFilterCoefficients() {
    if (filterActiveFlag && (filterCutoffFrequency < samplingRateKhz.value*0.5)) {
        if (filterVoltageFlag) {
            double k1 = tan(M_PI*filterCutoffFrequency/samplingRateKhz.value); /*!< pre-warp coefficient */
            double k12 = k1*k1;
            double k2 = -2+2*k12; /*!< frequently used expression */
            double d1 = 1.0/(1.0+k1*IIR_2_SIN_3_PI_8+k12); /*! denominator of first biquad */
            double d2 = 1.0/(1.0+k1*IIR_2_SIN_PI_8+k12); /*! denominator of second biquad */

            /*! Denominators */
            // iir1Den[0] = 1.0; not used
            // iir2Den[0] = 1.0; not used
            iir1VDen[1] = k2*d1;
            iir2VDen[1] = k2*d2;
            iir1VDen[2] = (-1.0-k2+k12*k12+k12*IIR_2_COS_3_PI_8_2)*d1*d1;
            iir2VDen[2] = (-1.0-k2+k12*k12+k12*IIR_2_COS_PI_8_2)*d2*d2;

            /*! Gains and numerators */
            double iir1G;
            double iir2G;
            if (filterLowPassFlag) {
                iir1G = (1.0+iir1VDen[1]+iir1VDen[2])*0.25;
                iir2G = (1.0+iir2VDen[1]+iir2VDen[2])*0.25;

                iir1VNum[1] = 2.0*iir1G;
                iir2VNum[1] = 2.0*iir2G;

            } else {
                iir1G = (1.0-iir1VDen[1]+iir1VDen[2])*0.25;
                iir2G = (1.0-iir2VDen[1]+iir2VDen[2])*0.25;

                iir1VNum[1] = -2.0*iir1G;
                iir2VNum[1] = -2.0*iir2G;
            }

            iir1VNum[0] = iir1G;
            iir2VNum[0] = iir2G;
            iir1VNum[2] = iir1G;
            iir2VNum[2] = iir2G;

        } else {
            /*! Voltage is not filtered */
            iir1VNum[0] = 1.0;
            iir2VNum[0] = 1.0;
            iir1VNum[1] = 0.0;
            iir2VNum[1] = 0.0;
            iir1VNum[2] = 0.0;
            iir2VNum[2] = 0.0;
            // iir1VDen[0] = 1.0; not used
            // iir2VDen[0] = 1.0; not used
            iir1VDen[1] = 0.0;
            iir2VDen[1] = 0.0;
            iir1VDen[2] = 0.0;
            iir2VDen[2] = 0.0;
        }

        if (filterCurrentFlag) {
            double k1 = tan(M_PI*filterCutoffFrequency/samplingRateKhz.value); /*!< pre-warp coefficient */
            double k12 = k1*k1;
            double k2 = -2+2*k12; /*!< frequently used expression */
            double d1 = 1.0/(1.0+k1*IIR_2_SIN_3_PI_8+k12); /*! denominator of first biquad */
            double d2 = 1.0/(1.0+k1*IIR_2_SIN_PI_8+k12); /*! denominator of second biquad */

            /*! Denominators */
            // iir1Den[0] = 1.0; not used
            // iir2Den[0] = 1.0; not used
            iir1IDen[1] = k2*d1;
            iir2IDen[1] = k2*d2;
            iir1IDen[2] = (-1.0-k2+k12*k12+k12*IIR_2_COS_3_PI_8_2)*d1*d1;
            iir2IDen[2] = (-1.0-k2+k12*k12+k12*IIR_2_COS_PI_8_2)*d2*d2;

            /*! Gains and numerators */
            double iir1G;
            double iir2G;
            if (filterLowPassFlag) {
                iir1G = (1.0+iir1IDen[1]+iir1IDen[2])*0.25;
                iir2G = (1.0+iir2IDen[1]+iir2IDen[2])*0.25;

                iir1INum[1] = 2.0*iir1G;
                iir2INum[1] = 2.0*iir2G;

            } else {
                iir1G = (1.0-iir1IDen[1]+iir1IDen[2])*0.25;
                iir2G = (1.0-iir2IDen[1]+iir2IDen[2])*0.25;

                iir1INum[1] = -2.0*iir1G;
                iir2INum[1] = -2.0*iir2G;
            }

            iir1INum[0] = iir1G;
            iir2INum[0] = iir2G;
            iir1INum[2] = iir1G;
            iir2INum[2] = iir2G;

        } else {
            /*! Current is not filtered */
            iir1INum[0] = 1.0;
            iir2INum[0] = 1.0;
            iir1INum[1] = 0.0;
            iir2INum[1] = 0.0;
            iir1INum[2] = 0.0;
            iir2INum[2] = 0.0;
            // iir1IDen[0] = 1.0; not used
            // iir2IDen[0] = 1.0; not used
            iir1IDen[1] = 0.0;
            iir2IDen[1] = 0.0;
            iir1IDen[2] = 0.0;
            iir2IDen[2] = 0.0;
        }

    } else {
        /*! Delta impulse response with no autoregressive part */
        iir1VNum[0] = 1.0;
        iir2VNum[0] = 1.0;
        iir1VNum[1] = 0.0;
        iir2VNum[1] = 0.0;
        iir1VNum[2] = 0.0;
        iir2VNum[2] = 0.0;
        // iir1VDen[0] = 1.0; not used
        // iir2VDen[0] = 1.0; not used
        iir1VDen[1] = 0.0;
        iir2VDen[1] = 0.0;
        iir1VDen[2] = 0.0;
        iir2VDen[2] = 0.0;

        iir1INum[0] = 1.0;
        iir2INum[0] = 1.0;
        iir1INum[1] = 0.0;
        iir2INum[1] = 0.0;
        iir1INum[2] = 0.0;
        iir2INum[2] = 0.0;
        // iir1IDen[0] = 1.0; not used
        // iir2IDen[0] = 1.0; not used
        iir1IDen[1] = 0.0;
        iir2IDen[1] = 0.0;
        iir1IDen[2] = 0.0;
        iir2IDen[2] = 0.0;
    }

    /*! reset FIFOs */
    for (uint16_t channelIdx = 0; channelIdx < totalChannelsNum; channelIdx++) {
        for (int tapIdx = 0; tapIdx < IIR_ORD+1; tapIdx++) {
            iirX[channelIdx][tapIdx] = 0.0;
            iirU[channelIdx][tapIdx] = 0.0;
            iirY[channelIdx][tapIdx] = 0.0;
        }
    }
}

void MessageDispatcher::applyVoltageFilter(uint16_t channelIdx, double x) {
    /*! 4th order Butterworth filter with cascaded biquad structure */
    int tapIdx;

    /*! 1st biquad section */
    int coeffIdx = 0;
    iirX[channelIdx][iirOff] = x;
    double u = x*iir1VNum[coeffIdx++];

    for (tapIdx = iirOff+1; tapIdx <= IIR_ORD; tapIdx++) {
        u += iirX[channelIdx][tapIdx]*iir1VNum[coeffIdx]-iirU[channelIdx][tapIdx]*iir1VDen[coeffIdx];
        coeffIdx++;
    }

    for (tapIdx = 0; tapIdx < iirOff; tapIdx++) {
        u += iirX[channelIdx][tapIdx]*iir1VNum[coeffIdx]-iirU[channelIdx][tapIdx]*iir1VDen[coeffIdx];
        coeffIdx++;
    }

    /*! 2nd biquad section */
    coeffIdx = 0;
    iirU[channelIdx][iirOff] = u;
    double y = u*iir2VNum[coeffIdx++];

    for (tapIdx = iirOff+1; tapIdx <= IIR_ORD; tapIdx++) {
        y += iirU[channelIdx][tapIdx]*iir2VNum[coeffIdx]-iirY[channelIdx][tapIdx]*iir2VDen[coeffIdx];
        coeffIdx++;
    }

    for (tapIdx = 0; tapIdx < iirOff; tapIdx++) {
        y += iirU[channelIdx][tapIdx]*iir2VNum[coeffIdx]-iirY[channelIdx][tapIdx]*iir2VDen[coeffIdx];
        coeffIdx++;
    }

    iirY[channelIdx][iirOff] = y;
}

void MessageDispatcher::applyCurrentFilter(uint16_t channelIdx, double x) {
    /*! 4th order Butterworth filter with cascaded biquad structure */
    int tapIdx;

    /*! 1st biquad section */
    int coeffIdx = 0;
    iirX[channelIdx][iirOff] = x;
    double u = x*iir1INum[coeffIdx++];

    for (tapIdx = iirOff+1; tapIdx <= IIR_ORD; tapIdx++) {
        u += iirX[channelIdx][tapIdx]*iir1INum[coeffIdx]-iirU[channelIdx][tapIdx]*iir1IDen[coeffIdx];
        coeffIdx++;
    }

    for (tapIdx = 0; tapIdx < iirOff; tapIdx++) {
        u += iirX[channelIdx][tapIdx]*iir1INum[coeffIdx]-iirU[channelIdx][tapIdx]*iir1IDen[coeffIdx];
        coeffIdx++;
    }

    /*! 2nd biquad section */
    coeffIdx = 0;
    iirU[channelIdx][iirOff] = u;
    double y = u*iir2INum[coeffIdx++];

    for (tapIdx = iirOff+1; tapIdx <= IIR_ORD; tapIdx++) {
        y += iirU[channelIdx][tapIdx]*iir2INum[coeffIdx]-iirY[channelIdx][tapIdx]*iir2IDen[coeffIdx];
        coeffIdx++;
    }

    for (tapIdx = 0; tapIdx < iirOff; tapIdx++) {
        y += iirU[channelIdx][tapIdx]*iir2INum[coeffIdx]-iirY[channelIdx][tapIdx]*iir2IDen[coeffIdx];
        coeffIdx++;
    }

    iirY[channelIdx][iirOff] = y;
}
