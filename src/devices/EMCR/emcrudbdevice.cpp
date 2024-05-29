#include "emcrudbdevice.h"

#include <fstream>

#include "emcr10mhz.h"
#ifdef DEBUG
/*! Fake device that generates synthetic data */
#include "emcr10mhzfake.h"
#endif

static UdbProgrammer programmer;

static const std::vector <std::vector <uint32_t>> deviceTupleMapping = {
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 0, Device10MHzOld},                  //   11,  1,  0 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 1, Device10MHzOld},                  //   11,  1,  1 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 2, Device10MHzOld},                  //   11,  1,  2 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 3, Device10MHzOld},                  //   11,  1,  3 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 4, Device10MHzOld},                  //   11,  1,  4 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 5, Device10MHzOld},                  //   11,  1,  5 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 6, Device10MHzOld},                  //   11,  1,  6 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 7, Device10MHzOld},                  //   11,  1,  7 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 8, Device10MHzOld},                  //   11,  1,  8 : UDB V02
    {EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_PCBV02, 9, Device10MHzV01}                   //   11,  1,  9 : UDB V02
    #ifdef DEBUG
    ,{EmcrUdbDevice::DeviceVersion10MHz, EmcrUdbDevice::DeviceSubversionUDB_FAKE, 254, Device10MHzFake}                 //   11,254,254 : UDB FAKE
    #endif
};

static std::unordered_map <DeviceTypes_t, MessageDispatcher::FwUpgradeInfo_t> fwUpgradeInfo = {
    {Device10MHzOld, {true, 9, "10MHz_V09.bin"}},
    {Device10MHzV01, MessageDispatcher::FwUpgradeInfo_t()}
    #ifdef DEBUG
    ,{Device10MHzFake, MessageDispatcher::FwUpgradeInfo_t()}
    #endif
};

EmcrUdbDevice::EmcrUdbDevice(std::string deviceId) :
    EmcrDevice(deviceId) {

    rxRawBufferMask = UDB_RX_BUFFER_MASK;
    startTime = std::chrono::steady_clock::now();
}

EmcrUdbDevice::~EmcrUdbDevice() {
    this->disconnectDevice();
}

ErrorCodes_t EmcrUdbDevice::detectDevices(
        std::vector <std::string> &deviceIds) {
    /*! Gets number of devices */
    int numDevs;
    bool devCountOk = UdbUtils::getDeviceCount(numDevs);
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
        deviceIds.push_back(UdbUtils::getDeviceSerial(i));
    }

#ifdef DEBUG
    numDevs++;
    deviceIds.push_back("FAKE_10MHz");
#endif

    return Success;
}

ErrorCodes_t EmcrUdbDevice::getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion) {
    if (deviceId != "FAKE_10MHz") {
        DeviceTuple_t tuple = getDeviceTuple(UdbUtils::getDeviceIndex(deviceId));
        deviceVersion = tuple.version;
        deviceSubVersion = tuple.subversion;
        fwVersion = tuple.fwVersion;

    } else {
        deviceVersion = DeviceVersion10MHz;
        deviceSubVersion = DeviceSubversionUDB_FAKE;
        fwVersion = 254;
    }
    return Success;
}

ErrorCodes_t EmcrUdbDevice::getDeviceType(std::string deviceId, DeviceTypes_t &type) {
    DeviceTuple_t tuple;
    if (deviceId != "FAKE_10MHz") {
        tuple = getDeviceTuple(UdbUtils::getDeviceIndex(deviceId));

    } else {
        tuple.version = DeviceVersion10MHz;
        tuple.subversion = DeviceSubversionUDB_FAKE;
        tuple.fwVersion = 254;
    }

    bool deviceFound = false;
    for (unsigned int mappingIdx = 0; mappingIdx < deviceTupleMapping.size(); mappingIdx++) {
        if (tuple.version == deviceTupleMapping[mappingIdx][0] &&
                tuple.subversion == deviceTupleMapping[mappingIdx][1] &&
                tuple.fwVersion == deviceTupleMapping[mappingIdx][2]) {
            type = (DeviceTypes_t)deviceTupleMapping[mappingIdx][3];
            deviceFound = true;
            break;
        }
    }

    if (!deviceFound) {
        return ErrorDeviceTypeNotRecognized;
    }
    return Success;
}

ErrorCodes_t EmcrUdbDevice::getUpgradeInfo(DeviceTypes_t type, FwUpgradeInfo_t &info) {
    info = fwUpgradeInfo[type];
    if (!(info.available)) {
        return ErrorDeviceNotUpgradable;
    }
    return Success;
}

ErrorCodes_t EmcrUdbDevice::isDeviceSerialDetected(std::string deviceId) {
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

ErrorCodes_t EmcrUdbDevice::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        return ErrorDeviceAlreadyConnected;
    }

    DeviceTypes_t deviceType;

    ret = EmcrUdbDevice::getDeviceType(deviceId, deviceType);
    if (ret != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    switch (deviceType) {
    case Device10MHzV01:
        messageDispatcher = new Emcr10MHz_V01(deviceId);
        break;

    case Device10MHzOld:
        return ErrorDeviceToBeUpgraded;

#ifdef DEBUG
    case Device10MHzFake:
        messageDispatcher = new Emcr10MHzFake(deviceId);
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

ErrorCodes_t EmcrUdbDevice::isDeviceUpgradable(std::string deviceId) {
    DeviceTypes_t deviceType;

    ErrorCodes_t ret = EmcrUdbDevice::getDeviceType(deviceId, deviceType);
    if (ret != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    FwUpgradeInfo_t info;
    EmcrUdbDevice::getUpgradeInfo(deviceType, info);
    if (!(info.available)) {
        return ErrorDeviceNotUpgradable;
    }
    return Success;
}


ErrorCodes_t EmcrUdbDevice::upgradeDevice(std::string deviceId) {
    DeviceTypes_t deviceType;

    ErrorCodes_t ret = EmcrUdbDevice::getDeviceType(deviceId, deviceType);
    if (ret != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    FwUpgradeInfo_t upgradeInfo;
    EmcrUdbDevice::getUpgradeInfo(deviceType, upgradeInfo);

    int32_t idx = UdbUtils::getDeviceIndex(deviceId);
    if (idx < 0) {
        return ErrorDeviceConnectionFailed;
    }

    programmer.connect(idx, true);

    UdbProgrammer::InfoStruct_t infoStruct;
    programmer.getDeviceInfo(infoStruct);
    if (infoStruct.fx3FwVersion < UTL_DEFAULT_FX3_FW_VERSION) {
        /*! Upgrade FX3 FW and version if needed */
        infoStruct.fx3FwVersion = UTL_DEFAULT_FX3_FW_VERSION;
        std::ifstream file(UTL_DEFAULT_FW_PATH + UTL_DEFAULT_FX3_FW_NAME, std::ios::binary);

        if (!file) {
            return ErrorFwNotFound;
        }

        file.seekg(0, std::ios::end);
        unsigned int fileSize = (unsigned int)file.tellg();
        file.seekg(0, std::ios::beg);

        char * buffer = new char[fileSize];

        file.read(buffer, fileSize);
        file.close();

        programmer.programFlashBlock(UdbUtils::BlockFX3, buffer, fileSize);

        delete [] buffer;
    }

    /*! Read FPGA FW bitstream */
    std::ifstream file(UTL_DEFAULT_FW_PATH + upgradeInfo.fwName, std::ios::binary);
    if (!file) {
        return ErrorFwNotFound;
    }

    /*! Upgrade FPGA version */
    unsigned char originalFwVersion = infoStruct.fpgaFwVersion;
    infoStruct.fpgaFwVersion = upgradeInfo.fwVersion;
    programmer.programFlashBlock(UdbUtils::BlockInfo, (char *)&infoStruct, sizeof(UdbProgrammer::InfoStruct_t));

    /*! Upgrade FPGA FW */
    file.seekg(0, std::ios::end);
    unsigned int fileSize = (unsigned int)file.tellg();
    file.seekg(0, std::ios::beg);

    char * buffer = new char[fileSize+4];
    /*! add image length as leading four bytes... */
    buffer[0] = fileSize & 0x000000FF;
    buffer[1] = (fileSize  >> 8)& 0x000000FF;
    buffer[2] = (fileSize >> 16)& 0x000000FF;
    buffer[3] = (fileSize >> 24)& 0x000000FF;

    file.read(buffer+4, fileSize);
    file.close();

    programmer.programFlashBlock(UdbUtils::BlockFPGA, buffer, fileSize+4);

    if (!(programmer.verifyFlashBlock(UdbUtils::BlockFPGA, buffer, fileSize+4))) {
        /*! Restore original FPGA version */
        infoStruct.fpgaFwVersion = originalFwVersion;
        programmer.programFlashBlock(UdbUtils::BlockInfo, (char *)&infoStruct, sizeof(UdbProgrammer::InfoStruct_t));

        delete [] buffer;
        return ErrorFwUpgradeFailed;
    }

    delete [] buffer;

    /*! disconnect */
    programmer.connect(idx, false);
    return Success;
}


ErrorCodes_t EmcrUdbDevice::getUpgradeProgress(int32_t &progress) {
    progress = programmer.getProgress();
    return Success;
}

ErrorCodes_t EmcrUdbDevice::disconnectDevice() {
    this->deinitialize();
    return Success;
}

ErrorCodes_t EmcrUdbDevice::startCommunication(std::string) {
    int32_t idx = UdbUtils::getDeviceIndex(deviceId);
    if (idx < 0) {
        return ErrorDeviceConnectionFailed;
    }
    dev = new CCyUSBDevice;
    dev->Open(idx);

    UdbUtils::findBulkEndpoints(dev, eptBulkin, eptBulkout);
    UdbUtils::resetBulkEndpoints(eptBulkin, eptBulkout);
    UdbUtils::initEndpoints(readDataTransferSize, eptBulkin, eptBulkout);
    return Success;
}

ErrorCodes_t EmcrUdbDevice::initializeHW() {
    /*! check if FX3 is in FPGA config mode, otherwise switch into it */
    if (UdbUtils::getFwStatus(dev) == UdbUtils::fwStatusConfigMode) {
        UdbUtils::bootFpgafromFLASH(dev);
        Sleep(1000); /*!< wait a bit to be sure that the FPGA write is in progress */

        /*! wait the configuration to finish */
        while (UdbUtils::fpgaLoadBitstreamStatus(dev) == UdbUtils::fpgaLoadingInProgress) {
            Sleep(100); /*!< don't poll too frequently... may lockup */
        }

        if (UdbUtils::fpgaLoadBitstreamStatus(dev) == UdbUtils::fpgaLoadingError) {
            return ErrorDeviceFwLoadingFailed;
        }

        /*! resetBulkEndPoints here might disrupt the FX3 initialization */

        Sleep(1000); /*!< Wait a bit after the FPGA has booted: not 100% sure, but the communication might break if the first commands arrive
                      *   when the FPGA is still executing the starting procedures */
    }
    fwLoadedFlag = true;
    parsingFlag = true;

    this->sendCommands();

    return Success;
}

ErrorCodes_t EmcrUdbDevice::stopCommunication() {
    dev->Close();
    delete dev;
    dev = nullptr;
    return Success;
}

void EmcrUdbDevice::handleCommunicationWithDevice() {
    /*! Header and type do not change for now */
    txRawBulkBuffer[0] = 0x5aa55aa5;
    txRawBulkBuffer[1] = 0x00000000;

    txRawTriggerBuffer[0] = 0x5aa55aa5;
    txRawTriggerBuffer[1] = 0x00000001;
    txRawTriggerBuffer[2] = 0x00000001;

    std::unique_lock <std::mutex> txMutexLock (txMutex);
    txMutexLock.unlock();

    std::unique_lock <std::mutex> rxRawMutexLock (rxRawMutex);
    rxRawMutexLock.unlock();

    bool anyOperationPerformed;

    while (!stopConnectionFlag) {
        anyOperationPerformed = false;

        /*! Avoid communicating too early, communication might break if the fw has not started yet */
        if (fwLoadedFlag) {
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

            rxRawMutexLock.lock();
            if (rxRawBufferReadLength+UDB_RX_TRANSFER_SIZE <= UDB_RX_BUFFER_SIZE) {
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
        }

        if (!anyOperationPerformed) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
}

void EmcrUdbDevice::sendCommandsToDevice() {
    txRawBulkBuffer[2] = txMsgLength[txMsgBufferReadOffset]/2;

    int writeTries = 0;

    bool notSentTxData;

    /*! Moving from 16 bits words to 32 bits registers (+= 2, /2, etc, are due to this conversion) */
    for (uint32_t txDataBufferReadIdx = 0; txDataBufferReadIdx < txMsgLength[txMsgBufferReadOffset]; txDataBufferReadIdx += 2) {
        txRawBulkBuffer[3+txDataBufferReadIdx] = (txMsgOffsetWord[txMsgBufferReadOffset]+txDataBufferReadIdx)/2;
        txRawBulkBuffer[3+txDataBufferReadIdx+1] =
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
        for (uint32_t regIdx = 0; regIdx < txRawBulkBuffer[2]; regIdx++) {
            fprintf(txFid, "%04d:0x%08X ", txRawBulkBuffer[3+regIdx*2], txRawBulkBuffer[3+regIdx*2+1]);
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

bool EmcrUdbDevice::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    if (this->writeRegisters() == false) {
        return false;
    }
    switch (type) {
    case TxTriggerParameteresUpdated:
//        this->activateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
        break;

    case TxTriggerStartProtocol:
//        this->activateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        return this->activateTriggerIn(UDB_START_PROTOCOL_TRIGGER_IN_ADDR, UDB_START_PROTOCOL_TRIGGER_IN_BIT);
        break;

    case TxTriggerStartStateArray:
//        this->activateTriggerIn(OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR, OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT);
//        std::this_thread::sleep_for(std::chrono::milliseconds(5));
//        this->activateTriggerIn(OKY_START_STATE_ARRAY_TRIGGER_IN_ADDR, OKY_START_STATE_ARRAY_TRIGGER_IN_BIT);
        break;
    }
    return true;
}

uint32_t EmcrUdbDevice::readDataFromDevice() {
    /*! Declare variables to manage buffers indexing */
    long bytesRead; /*!< Bytes read during last transfer from Opal Kelly */

    /******************\
     *  Reading part  *
    \******************/

#ifdef DEBUG_RX_PROCESSING_PRINT
    fprintf(rxProcFid, "Entering while loop\n");
    fflush(rxProcFid);
#endif

    /*! Read the data */
    bytesRead = readDataTransferSize;
    if (eptBulkin->XferData((PUCHAR)rxRawBuffer+rxRawBufferWriteOffset, bytesRead, nullptr, false) == false) {
        /*! XferData, apparently, returns false also if the required data is not available before timeout, so check the amount of available data as well */
        if (bytesRead <= 0) {
            return 0;
            /*! \todo eptBulkin->NtStatus controllare per vedere il tipo di fallimento */
        }
    }

//    totalBytesRead += bytesRead;
//    currentTime = std::chrono::steady_clock::now();
//    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
//    if (duration > (1000.0)) {
//        startTime = currentTime;
//        fprintf(rxSpeedFid, "%f64Msps\n", ((double)totalBytesRead)/4.0e6);
//        fflush(rxSpeedFid);
//        totalBytesRead = 0;
//    }

    if (rxRawBufferWriteOffset+bytesRead > UDB_RX_BUFFER_SIZE) {
        for (uint32_t idx = 0; idx < rxRawBufferWriteOffset+bytesRead-UDB_RX_BUFFER_SIZE; idx++) {
            rxRawBuffer[idx] = rxRawBuffer[UDB_RX_BUFFER_SIZE+idx];
        }
    }

#ifdef DEBUG_RX_PROCESSING_PRINT
        fprintf(rxProcFid, "Bytes read %d\n", bytesRead);
        fflush(rxProcFid);
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
        fwrite(rxRawBuffer+rxRawBufferWriteOffset, sizeof(rxRawBuffer[0]), bytesRead, rxRawFid);
        fflush(rxRawFid);
#endif

    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+bytesRead) & UDB_RX_BUFFER_MASK;
    /*! Update buffer writing point */
    return bytesRead;
}

void EmcrUdbDevice::parseDataFromDevice() {
    RxParsePhase_t rxParsePhase = RxParseLookForHeader;

    rxRawBufferReadOffset = 0;
    uint32_t rxSyncWordSize = sizeof(rxSyncWord);
    uint32_t rxOffsetLengthSize = 2*RX_WORD_SIZE;
    uint32_t rxFrameOffset; /*!< Offset of the current frame */
    uint16_t rxWordOffset; /*!< Offset of the first word in the received frame */
    uint16_t rxWordsLength; /*!< Number of words in the received frame */
    uint32_t rxDataBytes; /*!< Number of bytes in the received frame */
    uint32_t rxCandidateHeader;

    bool notEnoughRxData;

    /******************\
     *  Parsing part  *
    \******************/

    std::unique_lock <std::mutex> rxRawMutexLock (rxRawMutex);
    rxRawMutexLock.unlock();

    while (!stopConnectionFlag) {
        rxRawMutexLock.lock();
        /*! Since UDB_RX_TRANSFER_SIZE bytes are obtained each time from the UDB, wait that at least these many are available,
         *  Otherwise it means that no reads from the Opal kelly took place. */
        while (rxRawBufferReadLength < UDB_RX_TRANSFER_SIZE && !stopConnectionFlag) {
            rxRawBufferNotEmpty.wait_for(rxRawMutexLock, std::chrono::milliseconds(3));
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
                    if (popUint32FromRxRawBuffer() == rxSyncWord) {
                        /*! If all the bytes match the sync word move rxSyncWordSize bytes ahead and look for the message length */
                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! If not all the bytes match the sync word restore three of the removed bytes and recheck */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset-3) & UDB_RX_BUFFER_MASK;
                        rxRawBytesAvailable += 3;
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
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & UDB_RX_BUFFER_MASK;
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
                    rxCandidateHeader = readUint32FromRxRawBuffer(rxDataBytes);

                    if (rxCandidateHeader == rxSyncWord) {
                        if (rxWordOffset == rxWordOffsets[RxMessageDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageDataLoad);

                        } else if (rxWordOffset == rxWordOffsets[RxMessageVoltageThenCurrentDataLoad]) {
                            this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageVoltageThenCurrentDataLoad);

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
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxSyncWordSize) & UDB_RX_BUFFER_MASK;
                        rxRawBytesAvailable -= rxSyncWordSize;

                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! Sync word not found, restart looking from the previous sync word */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & UDB_RX_BUFFER_MASK;
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

ErrorCodes_t EmcrUdbDevice::initializeMemory() {
    rxRawBuffer = new (std::nothrow) uint8_t[UDB_RX_EXTENDED_BUFFER_SIZE];
    if (rxRawBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txRawBulkBuffer = new (std::nothrow) uint32_t[3+2*txMaxWords];
    if (txRawBulkBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txRawTriggerBuffer = new (std::nothrow) uint32_t[UDB_TX_TRIGGER_BUFFER_SIZE];
    if (txRawTriggerBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    rxRawBuffer16 = (uint16_t *)rxRawBuffer;
    return EmcrDevice::initializeMemory();
}

void EmcrUdbDevice::deinitializeMemory() {
    if (rxRawBuffer != nullptr) {
        delete [] rxRawBuffer;
        rxRawBuffer = nullptr;
    }

    if (txRawBulkBuffer != nullptr) {
        delete [] txRawBulkBuffer;
        txRawBulkBuffer = nullptr;
    }

    if (txRawTriggerBuffer != nullptr) {
        delete [] txRawTriggerBuffer;
        txRawTriggerBuffer = nullptr;
    }
    rxRawBuffer16 = (uint16_t *)rxRawBuffer;

    EmcrDevice::deinitializeMemory();
}

EmcrUdbDevice::DeviceTuple_t EmcrUdbDevice::getDeviceTuple(uint32_t deviceIdx) {
    DeviceTuple_t tuple;
    CCyUSBDevice * tempDev = new CCyUSBDevice;
    if (tempDev->DeviceCount() > 0) {
        tempDev->Open(deviceIdx);

        CCyControlEndPoint * ctrept;
        long ctrlen = 4;

        unsigned char		buffer[4];

        ctrept = tempDev->ControlEndPt;
        ctrept->Target		= TGT_DEVICE;
        ctrept->ReqType		= REQ_VENDOR;
        ctrept->Direction	= DIR_FROM_DEVICE;
        ctrept->ReqCode		= CYP_CMD_GET_TUPLE;
        ctrept->Value		= 0;
        ctrept->Index		= 0;

        if (ctrept->XferData((PUCHAR)buffer, ctrlen)) {
            tuple.version = (DeviceVersion_t)buffer[0];
            tuple.subversion = (DeviceSubversion_t)buffer[1];
            tuple.fwVersion = buffer[2];
        }

        tempDev->Close();
    }
    delete tempDev;
    return tuple;
}

bool EmcrUdbDevice::writeRegisters() {
    return writeToBulkOut(txRawBulkBuffer);
}

bool EmcrUdbDevice::activateTriggerIn(int address, int bit) {
    txRawTriggerBuffer[3] = (uint32_t)address;
    txRawTriggerBuffer[4] = ((uint32_t)1) << ((uint32_t)bit);
    return writeToBulkOut(txRawTriggerBuffer);
}

bool EmcrUdbDevice::writeToBulkOut(uint32_t * buffer) {
    long bytesNum = sizeof(uint32_t)*(3+2*buffer[2]);
    std::unique_lock <std::mutex> deviceMtxLock (deviceMtx);

    if (!(eptBulkout->XferData((PUCHAR)buffer, bytesNum))) {
        eptBulkout->Abort();
        return false;
    }

//#ifdef DEBUG_PRINT
//    fprintf(fid, "config sent:");
//    for (int i = 0; i < bytesNum/bytesNum; i++) {
//        fprintf(fid, "0x%08x ", *(buffer+i));
//    }
//    fprintf(fid, "\n");
//    fflush(fid);
//#endif

    return true;
}
