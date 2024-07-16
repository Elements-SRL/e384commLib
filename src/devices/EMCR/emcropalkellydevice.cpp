#include "emcropalkellydevice.h"

#include <fstream>

#include "emcr384nanopores.h"
#include "emcr384nanopores_sr7p5khz_v01.h"
#include "emcr384patchclamp_prot_v01_fw_v02.h"
#include "emcr384patchclamp_prot_v04_fw_v03.h"
#include "emcr384patchclamp_prot_v04_fw_v04.h"
#include "emcr384patchclamp_prot_v04_fw_v05.h"
#include "emcr384patchclamp_prot_v05_fw_v06.h"
#include "emcr384patchclamp_el07cd_prot_v06_fw_v01.h"
#include "emcr384voltageclamp_prot_v04_fw_v03.h"
#include "emcrtestboardel07ab.h"
#include "emcrtestboardel07cd.h"
#include "emcr4x10mhz.h"
#include "emcr2x10mhz.h"
#ifdef DEBUG
/*! Fake device that generates synthetic data */
#include "emcr384nanoporesfake.h"
#include "emcr384patchclampfake.h"
#include "emcrtestboardel07abfake.h"
#include "emcr4x10mhzfake.h"
#include "emcr2x10mhzfake.h"
#endif

static std::unordered_map <std::string, DeviceTypes_t> deviceIdMapping = {
    {"221000107S", Device384Nanopores_SR7p5kHz},
    {"221000108T", Device384Nanopores_SR7p5kHz},
    {"22510013B4", Device384Nanopores},
    {"23210014U9", Device384Nanopores},
    {"23210014UP", Device384Nanopores},
    {"23190014UX", Device384PatchClamp_prot_v04_fw_v05},
    {"2210001076", Device384PatchClamp_prot_el07c_v06_fw_v01},
    {"221000106C", Device384PatchClamp_prot_v01_fw_v02},
    {"23210014UF", Device384PatchClamp_prot_v01_fw_v02},
    {"221000106B", Device384VoltageClamp_prot_v04_fw_v03},
    {"22370012CB", Device2x10MHz_PCBV02},
    {"224800131L", Device2x10MHz_PCBV02},
    {"233600161X", Device4x10MHz_PCBV03},
    {"224800130Y", Device4x10MHz_PCBV03},
    {"224800130X", Device4x10MHz_QuadAnalog_PCBV01},
    {"22370012CI", Device4x10MHz_QuadAnalog_PCBV01_DIGV01},
    {"23230014TO", Device4x10MHz_SB_PCBV01},
    {"23230014TE", Device4x10MHz_SB_PCBV01},
    {"2336001642", DeviceTestBoardEL07c},
    {"233600165Q", DeviceTestBoardEL07d}
    #ifdef DEBUG
    ,{"DEMO_384_SSN", Device384Fake},
    {"DEMO_384_Patch", Device384FakePatchClamp},
    {"DEMO_TB_EL07ab", DeviceTbEl07abFake},
    {"DEMO_4x10MHz", Device4x10MHzFake},
    {"DEMO_2x10MHz", Device2x10MHzFake}
    #endif
}; /*! \todo FCON queste info dovrebbero risiedere nel DB */

EmcrOpalKellyDevice::EmcrOpalKellyDevice(std::string deviceId) :
    EmcrDevice(deviceId) {

    rxRawBufferMask = OKY_RX_BUFFER_MASK;
}

EmcrOpalKellyDevice::~EmcrOpalKellyDevice() {
    this->disconnectDevice();
}

ErrorCodes_t EmcrOpalKellyDevice::detectDevices(
        std::vector <std::string> &deviceIds) {
    /*! Gets number of devices */
    int numDevs;
    bool devCountOk = getDeviceCount(numDevs);
    if (!devCountOk) {
        return ErrorListDeviceFailed;

#ifndef DEBUG
    } else if (numDevs == 0) {
        deviceIds.clear();
        return ErrorNoDeviceFound;
#endif
    }

    deviceIds.clear();

    /*! Lists all serial numbers */
    for (int i = 0; i < numDevs; i++) {
        deviceIds.push_back(getDeviceSerial(i));
    }

#ifdef DEBUG
    numDevs++;
    deviceIds.push_back("DEMO_384_SSN");
    numDevs++;
    deviceIds.push_back("DEMO_384_Patch");
    numDevs++;
    deviceIds.push_back("DEMO_TB_EL07ab");
    numDevs++;
    deviceIds.push_back("DEMO_4x10MHz");
    numDevs++;
    deviceIds.push_back("DEMO_2x10MHz");
#endif

    return Success;
}

ErrorCodes_t EmcrOpalKellyDevice::getDeviceType(std::string deviceId, DeviceTypes_t &type) {
    if (deviceIdMapping.count(deviceId) == 0) {
        return ErrorDeviceTypeNotRecognized;
    }
    type = deviceIdMapping[deviceId];
    return Success;
}

ErrorCodes_t EmcrOpalKellyDevice::isDeviceSerialDetected(std::string deviceId) {
    ErrorCodes_t ret = Success;
    std::vector <std::string> deviceIds;
    ret = detectDevices(deviceIds);

    if (ret != Success) {
        return ret;
    }

    uint32_t deviceIdx = distance(deviceIds.begin(), find(deviceIds.begin(), deviceIds.end(), deviceId));

    if (deviceIdx == deviceIds.size()) {
        return ErrorDeviceNotFound;
    }
    return ret;
}

ErrorCodes_t EmcrOpalKellyDevice::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        return ErrorDeviceAlreadyConnected;
    }

    DeviceTypes_t deviceType;
    ret = EmcrOpalKellyDevice::getDeviceType(deviceId, deviceType);
    if (ret != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    messageDispatcher = nullptr;

    switch (deviceType) {
    case Device384Nanopores:
        messageDispatcher = new Emcr384NanoPores_V01(deviceId);
        break;

    case Device384Nanopores_SR7p5kHz:
        messageDispatcher = new Emcr384NanoPores_SR7p5kHz_V01(deviceId);
        break;

    case Device384PatchClamp_prot_v01_fw_v02:
        messageDispatcher = new Emcr384PatchClamp_prot_v01_fw_v02(deviceId);
        break;

    case Device384PatchClamp_prot_v04_fw_v03:
        messageDispatcher = new Emcr384PatchClamp_prot_v04_fw_v03(deviceId);
        break;

    case Device384PatchClamp_prot_v04_fw_v04:
        messageDispatcher = new Emcr384PatchClamp_prot_v04_fw_v04(deviceId);
        break;

    case Device384PatchClamp_prot_v04_fw_v05:
        messageDispatcher = new Emcr384PatchClamp_prot_v04_fw_v05(deviceId);
        break;

    case Device384PatchClamp_prot_v05_fw_v06:
        messageDispatcher = new Emcr384PatchClamp_prot_v05_fw_v06(deviceId);
        break;

    case Device384PatchClamp_prot_el07c_v06_fw_v01:
        messageDispatcher = new Emcr384PatchClamp_EL07c_prot_v06_fw_v01(deviceId);
        break;

    case Device384VoltageClamp_prot_v04_fw_v03:
        messageDispatcher = new Emcr384VoltageClamp_prot_v04_fw_v03(deviceId);
        break;

    case DeviceTestBoardEL07ab:
        messageDispatcher = new EmcrTestBoardEl07ab(deviceId);
        break;

    case DeviceTestBoardEL07c:
        messageDispatcher = new EmcrTestBoardEl07c(deviceId);
        break;

    case DeviceTestBoardEL07d:
        messageDispatcher = new EmcrTestBoardEl07d(deviceId);
        break;

    case Device2x10MHz_PCBV01:
        messageDispatcher = new Emcr2x10MHz_PCBV01_V02(deviceId);
        break;

    case Device2x10MHz_PCBV02:
        messageDispatcher = new Emcr2x10MHz_PCBV02_V02(deviceId);
        break;

    case Device4x10MHz_PCBV01:
        messageDispatcher = new Emcr4x10MHz_PCBV01_V03(deviceId);
        break;

    case Device4x10MHz_PCBV03:
        messageDispatcher = new Emcr4x10MHz_PCBV03_V04(deviceId);
        break;

    case Device4x10MHz_SB_PCBV01:
        messageDispatcher = new Emcr4x10MHz_SB_PCBV01_V05(deviceId);
        break;

    case Device4x10MHz_QuadAnalog_PCBV01:
        messageDispatcher = new Emcr4x10MHz_QuadAnalog_PCBV01_V05(deviceId);
        break;

    case Device4x10MHz_QuadAnalog_PCBV01_DIGV01:
        messageDispatcher = new Emcr4x10MHz_QuadAnalog_PCBV01_DIGV01_V05(deviceId);
        break;

#ifdef DEBUG
    case Device384Fake:
        messageDispatcher = new Emcr384FakeNanopores(deviceId);
        break;

    case Device384FakePatchClamp:
        messageDispatcher = new Emcr384FakePatchClamp(deviceId);
        break;

    case DeviceTbEl07abFake:
        messageDispatcher = new EmcrTestBoardEl07abFake(deviceId);
        break;

    case Device4x10MHzFake:
        messageDispatcher = new Emcr4x10MHzFake(deviceId);
        break;

    case Device2x10MHzFake:
        messageDispatcher = new Emcr2x10MHzFake(deviceId);
        break;
#endif

    default:
        return ErrorDeviceTypeNotRecognized;
    }

    if (messageDispatcher != nullptr) {
        ret = messageDispatcher->initialize(fwPath);

        if (ret != Success) {
            messageDispatcher->deinitialize();
            delete messageDispatcher;
            messageDispatcher = nullptr;
        }
    }

    return ret;
}

ErrorCodes_t EmcrOpalKellyDevice::disconnectDevice() {
    this->deinitialize();
    return Success;
}

int32_t EmcrOpalKellyDevice::getDeviceIndex(std::string serial) {
    /*! Gets number of devices */
    int numDevs;
    bool devCountOk = getDeviceCount(numDevs);
    if (!devCountOk) {
        return -1;

    } else if (numDevs == 0) {
        return -1;
    }

    for (int index = 0; index < numDevs; index++) {
        std::string deviceId = getDeviceSerial(index);
        if (deviceId == serial) {
            return index;
        }
    }
    return -1;
}

std::string EmcrOpalKellyDevice::getDeviceSerial(uint32_t index) {
    std::string serial;
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

bool EmcrOpalKellyDevice::getDeviceCount(int &numDevs) {
    okCFrontPanel okDev;
    numDevs = okDev.GetDeviceCount();
    return true;
}

ErrorCodes_t EmcrOpalKellyDevice::startCommunication(std::string fwPath) {
    okCFrontPanel::ErrorCode error = dev.OpenBySerial(deviceId);

    if (error != okCFrontPanel::NoError) {
        return ErrorDeviceConnectionFailed;
    }
    fwPath = (!fwPath.empty() && fwPath.back() == '\\')?fwPath : fwPath + '\\';

    std::ifstream file(fwPath + fwName);
    if (file.good()) {
        error = dev.ConfigureFPGA(fwPath + fwName);

    } else {
        std::ifstream file(fwName);
        if (file.good()) {
            error = dev.ConfigureFPGA(fwName);

        } else {
            return ErrorFwNotFound;
        }
    }

    if (error != okCFrontPanel::NoError) {
        return ErrorDeviceFwLoadingFailed;
    }

    if (!(dev.IsFrontPanelEnabled())) {
        return ErrorDeviceFwLoadingFailed;
    }
    return Success;
}

ErrorCodes_t EmcrOpalKellyDevice::stopCommunication() {
    dev.Close();
    return Success;
}

void EmcrOpalKellyDevice::handleCommunicationWithDevice() {
    regs.reserve(txMaxRegs);

    std::unique_lock <std::mutex> txMutexLock (txMutex);
    txMutexLock.unlock();

    std::unique_lock <std::mutex> rxRawMutexLock (rxRawMutex);
    rxRawMutexLock.unlock();

    std::chrono::steady_clock::time_point startWhileTime = std::chrono::steady_clock::now();

    bool waitingTimeForReadingPassed = false;
    bool anyOperationPerformed;

    while (!stopConnectionFlag) {
        anyOperationPerformed = false;

        /***********************\
         *  Data copying part  *
        \***********************/

        txMutexLock.lock();
        while (txMsgBufferReadLength > 0) {
            anyOperationPerformed = true;
            this->sendCommandsToDevice();
            txMsgBufferReadLength--;
            if (liquidJunctionControlPending && txMsgBufferReadLength == 0) {
                /*! \todo FCON let the liquid junction procedure know that all commands have been submitted, can be optimized by checking that there are no liquid junction commands pending */
                liquidJunctionControlPending = false;
            }
        }
        txMutexLock.unlock();
        if (anyOperationPerformed) {
            txMsgBufferNotFull.notify_all();
        }

        /*! Avoid performing reads too early, might trigger Opal Kelly's API timeout, which appears to be a non escapable condition */
        if (waitingTimeForReadingPassed) {
            rxRawMutexLock.lock();
            if (rxRawBufferReadLength+okTransferSize <= OKY_RX_BUFFER_SIZE) {
                anyOperationPerformed = true;
                rxRawMutexLock.unlock();

                uint32_t bytesRead = this->readDataFromDevice();

                if (bytesRead <= INT32_MAX) {
                    rxRawMutexLock.lock();
                    rxRawBufferReadLength += bytesRead;
                    rxRawMutexLock.unlock();
                    rxRawBufferNotEmpty.notify_all();
                }

            } else {
                rxRawMutexLock.unlock();
            }

        } else {
            long long t = std::chrono::duration_cast <std::chrono::microseconds> (std::chrono::steady_clock::now()-startWhileTime).count();
            if (t > waitingTimeBeforeReadingData*1e6) {
                waitingTimeForReadingPassed = true;
                parsingFlag = true;
            }
        }

        if (!anyOperationPerformed) {
            std::this_thread::sleep_for (std::chrono::microseconds(1));
        }
    }
}

void EmcrOpalKellyDevice::sendCommandsToDevice() {
    int writeTries = 0;

    bool notSentTxData;

    /*! Moving from 16 bits words to 32 bits registers (+= 2, /2, etc, are due to this conversion) */
    regs.resize(txMsgLength[txMsgBufferReadOffset]/2);
    for (uint32_t txDataBufferReadIdx = 0; txDataBufferReadIdx < txMsgLength[txMsgBufferReadOffset]; txDataBufferReadIdx += 2) {
        regs[txDataBufferReadIdx/2].address = (txMsgOffsetWord[txMsgBufferReadOffset]+txDataBufferReadIdx)/2;
        regs[txDataBufferReadIdx/2].data =
                ((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx] +
                 ((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx+1] << 16)); /*! Little endian */
    }
    TxTriggerType_t type = txMsgTrigger[txMsgBufferReadOffset];

    txMsgBufferReadOffset = (txMsgBufferReadOffset+1) & TX_MSG_BUFFER_MASK;

    /******************\
     *  Sending part  *
    \******************/

    notSentTxData = true;
    writeTries = 0;
    while (notSentTxData && (writeTries++ < TX_MAX_WRITE_TRIES)) { /*! \todo FCON prevedere un modo per notificare ad alto livello e all'utente */
        if (!this->writeRegistersAndActivateTriggers(type)) {
            continue;
        }

#ifdef DEBUG_TX_DATA_PRINT
        for (uint16_t regIdx = 0; regIdx < regs.size(); regIdx++) {
            fprintf(txFid, "%04d:0x%08X ", regs[regIdx].address, regs[regIdx].data);
            if (regIdx % 16 == 15) {
                fprintf(txFid, "\n");
            }
        }
        fprintf(txFid, "\n");
        fflush(txFid);
#endif

        notSentTxData = false;
    }
}

bool EmcrOpalKellyDevice::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    if (dev.WriteRegisters(regs) != okCFrontPanel::NoError) {
        return false;
    }
    switch (type) {
    case TxTriggerParameteresUpdated:
        dev.ActivateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
        break;

    case TxTriggerStartProtocol:
        dev.ActivateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
        std::this_thread::sleep_for (std::chrono::milliseconds(5));
        dev.ActivateTriggerIn(OKY_START_PROTOCOL_TRIGGER_IN_ADDR, OKY_START_PROTOCOL_TRIGGER_IN_BIT);
        break;

    case TxTriggerStartStateArray:
        dev.ActivateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
        std::this_thread::sleep_for (std::chrono::milliseconds(5));
        dev.ActivateTriggerIn(OKY_START_STATE_ARRAY_TRIGGER_IN_ADDR, OKY_START_STATE_ARRAY_TRIGGER_IN_BIT);
        break;
    }
    return true;
}

uint32_t EmcrOpalKellyDevice::readDataFromDevice() {
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead; /*!< Bytes read during last transfer from Opal Kelly */

    /******************\
     *  Reading part  *
    \******************/

#ifdef DEBUG_RX_PROCESSING_PRINT
    fprintf(rxProcFid, "Entering while loop\n");
    fflush(rxProcFid);
#endif

//    int okWrites = 0;
//    std::chrono::steady_clock::time_point startPrintfTime;
//    std::chrono::steady_clock::time_point currentPrintfTime;
//    startPrintfTime = std::chrono::steady_clock::now();
    /*! Read the data */
    bytesRead = dev.ReadFromBlockPipeOut(OKY_RX_PIPE_ADDR, OKY_RX_BLOCK_SIZE, okTransferSize, rxRawBuffer+rxRawBufferWriteOffset);

    if (bytesRead > INT32_MAX) {
        if (bytesRead == ok_Timeout || bytesRead == ok_Failed) {
            /*! The device cannot recover from timeout condition */
            dev.Close();
            std::this_thread::sleep_for (std::chrono::milliseconds(1000));
            txModifiedStartingWord = 0;
            txModifiedEndingWord = txMaxWords;
            this->sendCommands();
            dev.OpenBySerial(deviceId);
        }
        std::this_thread::sleep_for (std::chrono::milliseconds(100));
#ifdef DEBUG_RX_PROCESSING_PRINT
        fprintf(rxProcFid, "Error %x\n", bytesRead);
        fflush(rxProcFid);
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
        fprintf(rxRawFid, "Error %x\n", bytesRead);
        fflush(rxRawFid);
#endif

    } else {

#ifdef DEBUG_RX_PROCESSING_PRINT
        fprintf(rxProcFid, "Bytes read %d\n", bytesRead);
        fflush(rxProcFid);
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
        fprintf(rxRawFid, "Bytes read %d\n", bytesRead);
        fflush(rxRawFid);
#endif
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+bytesRead) & OKY_RX_BUFFER_MASK;
    }
    /*! Update buffer writing point */
    return bytesRead;
}

void EmcrOpalKellyDevice::parseDataFromDevice() {
    RxParsePhase_t rxParsePhase = RxParseLookForHeader;

    rxRawBufferReadOffset = 0;
    uint32_t rxSyncWordSize = sizeof(rxSyncWord);
    uint32_t rxOffsetLengthSize = 2*RX_WORD_SIZE;
    uint32_t rxFrameOffset; /*!< Offset of the current frame */
    uint16_t rxWordOffset; /*!< Offset of the first word in the received frame */
    uint16_t rxWordsLength; /*!< Number of words in the received frame */
    uint32_t rxDataBytes; /*!< Number of bytes in the received frame */
    uint16_t rxCandidateHeader;

    bool notEnoughRxData;

    /******************\
     *  Parsing part  *
    \******************/

    std::unique_lock <std::mutex> rxRawMutexLock (rxRawMutex);
    rxRawMutexLock.unlock();

    while (!stopConnectionFlag) {
        rxRawMutexLock.lock();
        /*! Since OKY_RX_TRANSFER_SIZE bytes are obtained each time from the opal kelly, wait that at least these many are available,
         *  Otherwise it means that no reads from the Opal kelly took place. */
        while (rxRawBufferReadLength < okTransferSize && !stopConnectionFlag) {
            rxRawBufferNotEmpty.wait_for (rxRawMutexLock, std::chrono::milliseconds(3));
        }
        maxRxRawBytesRead = rxRawBufferReadLength;
        rxRawBytesAvailable = maxRxRawBytesRead;
        rxRawMutexLock.unlock();
        if (stopConnectionFlag) {
            break;
        }

        /*! Compute the approximate number of available packets */
        notEnoughRxData = false;

        while (!notEnoughRxData) {
            switch (rxParsePhase) {
            case RxParseLookForHeader:

#ifdef DEBUG_RX_PROCESSING_PRINT
                fprintf(rxProcFid, "Look for header: %x\n", rxRawBufferReadOffset);
                fflush(rxProcFid);
#endif

                /*! Look for header */
                if (rxRawBytesAvailable < rxSyncWordSize) {
                    notEnoughRxData = true;

                } else {
                    rxFrameOffset = rxRawBufferReadOffset;
                    /*! Check byte by byte if the buffer contains a sync word (frame header) */
                    if (popUint16FromRxRawBuffer() == rxSyncWord) {
                        /*! If all the bytes match the sync word move rxSyncWordSize bytes ahead and look for the message length */
                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! If not all the bytes match the sync word restore one of the removed bytes and recheck */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset-1) & OKY_RX_BUFFER_MASK;
                        rxRawBytesAvailable++;
                    }
                }
                break;

            case RxParseLookForLength:

#ifdef DEBUG_RX_PROCESSING_PRINT
                fprintf(rxProcFid, "Look for length: %x\n", rxRawBufferReadOffset);
                fflush(rxProcFid);
#endif

                /*! Look for length */
                if (rxRawBytesAvailable < rxOffsetLengthSize) {
                    notEnoughRxData = true;

                } else {
                    /*! Offset of the words received */
                    rxWordOffset = popUint16FromRxRawBuffer();

                    /*! Number of the words received */
                    rxWordsLength = popUint16FromRxRawBuffer();

                    rxDataBytes = rxWordsLength*RX_WORD_SIZE;

                    if (rxDataBytes > maxInputDataLoadSize) {
                        /*! Too many bytes to be read, restarting looking for a sync word from the previous one */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBytesAvailable += rxOffsetLengthSize;
#ifdef DEBUG_RX_DATA_PRINT
                        /*! aggiungere printata di debug se serve */
#endif
                        rxParsePhase = RxParseLookForHeader;

                    } else {
                        rxParsePhase = RxParseCheckNextHeader;
                    }
                }
                break;

            case RxParseCheckNextHeader:

#ifdef DEBUG_RX_PROCESSING_PRINT
                fprintf(rxProcFid, "Check next header: %x\n", rxRawBufferReadOffset);
                fflush(rxProcFid);
#endif

                /*! Check that after the frame end there is a valid header */
                if (rxRawBytesAvailable < rxDataBytes+rxSyncWordSize) {
                    notEnoughRxData = true;

                } else {
                    rxCandidateHeader = readUint16FromRxRawBuffer(rxDataBytes);

                    if (rxCandidateHeader == rxSyncWord) {
                        if (rxWordOffset == rxWordOffsets[RxMessageDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageCurrentDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageCurrentDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageVoltageDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdInvalid, RxMessageVoltageDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageDataHeader]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader, RxMessageDataHeader);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageDataTail]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail, RxMessageDataTail);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageStatus]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus, RxMessageStatus);
                        }

                        rxFrameOffset = rxRawBufferReadOffset;
                        /*! remove the bytes that were not popped to read the next header */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        rxRawBytesAvailable -= rxSyncWordSize;

                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! Sync word not found, restart looking from the previous sync word */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & OKY_RX_BUFFER_MASK;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBytesAvailable += rxOffsetLengthSize;
                        rxParsePhase = RxParseLookForHeader;
                    }
                }
                break;
            }
        }

        rxRawMutexLock.lock();
        rxRawBufferReadLength -= maxRxRawBytesRead-rxRawBytesAvailable;
        rxRawMutexLock.unlock();
        rxRawBufferNotFull.notify_all();
    }

    if (rxMsgBufferReadLength <= 0) {
        std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMutexLock.unlock();
        rxMsgBufferNotEmpty.notify_all();
    }
}

ErrorCodes_t EmcrOpalKellyDevice::initializeMemory() {
    rxRawBuffer = new (std::nothrow) uint8_t[OKY_RX_BUFFER_SIZE];
    if (rxRawBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    rxRawBuffer16 = (uint16_t *)rxRawBuffer;
    return EmcrDevice::initializeMemory();
}

void EmcrOpalKellyDevice::deinitializeMemory() {
    if (rxRawBuffer != nullptr) {
        delete [] rxRawBuffer;
        rxRawBuffer = nullptr;
    }
    rxRawBuffer16 = (uint16_t *)rxRawBuffer;

    EmcrDevice::deinitializeMemory();
}
