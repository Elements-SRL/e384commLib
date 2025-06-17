#include "emcrftdidevice.h"

#include "libMPSSE_spi.h"

#include "emcr8patchclamp_el07cd_artix7.h"
#include "emcr8npatchclamp_el07c_artix7_pcbv01.h"

static const std::vector <std::vector <uint32_t> > deviceTupleMapping = {
    {DeviceVersionE4p, DeviceSubversionEl07CDx8Patch_artix7_PCBV00_2, 4, DeviceE8PPatchEL07CD_artix7_PCBV00_2},     //  10, 14,  4 : VC-CC device with 8 channels (EL07CD) (FPGA artix7) PCB V00.2. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx8Patch_artix7_PCBV00_2, 5, DeviceE8PPatchEL07CD_artix7_PCBV00_2},     //  10, 14,  5 : VC-CC device with 8 channels (EL07CD) (FPGA artix7) PCB V00.2. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx8Patch_artix7_PCBV00_2, 6, DeviceE8PPatchEL07CD_artix7_PCBV00_2},     //  10, 14,  6 : VC-CC device with 8 channels (EL07CD) (FPGA artix7) PCB V00.2. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx8Patch_artix7_PCBV00_1, 4, DeviceE8PPatchEL07CD_artix7_PCBV00_1},     //  10, 12,  4 : VC-CC device with 8 channels (EL07CD) (FPGA artix7) PCB V00.1. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx4Patch_artix7_PCBV00_1, 1, DeviceE4PPatchEL07CD_artix7_PCBV00_1},     //  10, 16,  1 : VC-CC device with 4 channels (EL07CD) (FPGA artix7) PCB V00.1. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx4Patch_artix7_PCBV00_2, 1, DeviceE4PPatchEL07CD_artix7_PCBV00_2},     //  10, 17,  1 : VC-CC device with 4 channels (EL07CD) (FPGA artix7) PCB V00.2. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx4Patch_artix7_PCBV01, 1, DeviceE4PPatchEL07CD_artix7_PCBV00_2},       //  10, 18,  1 : VC-CC device with 4 channels (EL07CD) (FPGA artix7) PCB V01. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx8Patch_artix7_PCBV01, 1, DeviceE8PPatchEL07CD_artix7_PCBV00_2},       //  10, 19,  1 : VC-CC device with 8 channels (EL07CD) (FPGA artix7) PCB V01. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx8Patch_artix7_PCBV01, 2, DeviceE8PPatchEL07CD_artix7_PCBV01_FW2},     //  10, 19,  2 : VC-CC device with 8 channels (EL07CD) (FPGA artix7) PCB V01. */
    {DeviceVersionE4p, DeviceSubversionEl07CDx8Patch_artix7_PCBV01, 3, DeviceE8PPatchEL07CD_artix7_PCBV01_FW3},     //  10, 19,  3 : VC-CC device with 8 channels (EL07CD) (FPGA artix7) PCB V01. */
    {DeviceVersionE8p, DeviceSubversionE8nPatch_artix7_PCBV01, 1, DeviceE8nPatchEL07C_artix7_PCBV01_FW1},           //  16,  1,  1 : VC-CC device with 8 channels (EL07C) (FPGA artix7) PCB V01. */
};

EmcrFtdiDevice::EmcrFtdiDevice(std::string deviceId) :
    EmcrDevice(deviceId) {

    rxRawBufferMask = FTD_RX_RAW_BUFFER_MASK;
}

EmcrFtdiDevice::~EmcrFtdiDevice() {
    this->disconnectDevice();
}

ErrorCodes_t EmcrFtdiDevice::detectDevices(
        std::vector <std::string> &deviceIds) {
    /*! Gets number of devices */
    DWORD numDevs;
    bool devCountOk = getDeviceCount(numDevs);
    if (!devCountOk) {
        return ErrorListDeviceFailed;

    } else if (numDevs == 0) {
        deviceIds.clear();
        return ErrorNoDeviceFound;
    }

    deviceIds.clear();
    std::string deviceName;

    /*! Lists all serial numbers */
    for (uint32_t i = 0; i < numDevs; i++) {
        deviceName = getDeviceSerial(i, true);
        if (find(deviceIds.begin(), deviceIds.end(), deviceName) == deviceIds.end()) {
            /*! Adds only new devices (no distinction between channels A and B creates duplicates) */
            if (deviceName.size() > 0) {
                /*! Devices with an open channel are detected wrongly and their name is an empty std::string */
                deviceIds.push_back(getDeviceSerial(i, true));
            }
        }
    }

    return Success;
}

ErrorCodes_t EmcrFtdiDevice::getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion) {
    FtdiEepromId_t ftdiEepromId = FtdiEeprom::getFtdiEepromId(deviceId);
    DeviceTuple_t tuple;
    switch (ftdiEepromId) {
    case FtdiEepromId56:
        tuple = FtdiEeprom56(deviceId).getDeviceTuple();
        break;

    case FtdiEepromIdDemo:
        tuple = FtdiEepromDemo(deviceId).getDeviceTuple();
        break;
    }
    deviceVersion = tuple.version;
    deviceSubVersion = tuple.subversion;
    fwVersion = tuple.fwVersion;
    return Success;
}

//ErrorCodes_t EmcrFtdiDevice::getDeviceInfo(std::string &deviceId, std::string &deviceName, uint32_t &deviceVersion, uint32_t &deviceSubversion, uint32_t &firmwareVersion) {
//    ErrorCodes_t ret = Success;

//    deviceId = this->deviceId;
//    deviceName = this->deviceName;
//    DeviceTuple_t tuple = FtdiEeprom56(deviceId).getDeviceTuple();
//    deviceVersion = tuple.version;
//    deviceSubversion = tuple.subversion;
//    firmwareVersion = tuple.fwVersion;

//    return ret;
//}

ErrorCodes_t EmcrFtdiDevice::getDeviceType(std::string deviceId, DeviceTypes_t &type) {
    FtdiEepromId_t ftdiEepromId = FtdiEeprom::getFtdiEepromId(deviceId);
    DeviceTuple_t tuple;
    switch (ftdiEepromId) {
    case FtdiEepromId56:
        tuple = FtdiEeprom56(deviceId).getDeviceTuple();
        break;

    case FtdiEepromIdDemo:
        tuple = FtdiEepromDemo(deviceId).getDeviceTuple();
        break;
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

    if (deviceFound) {
        return Success;

    } else {
        return ErrorDeviceTypeNotRecognized;
    }
}

ErrorCodes_t EmcrFtdiDevice::isDeviceSerialDetected(std::string deviceId) {
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

ErrorCodes_t EmcrFtdiDevice::isDeviceRecognized(std::string deviceId) {
    if (isDeviceSerialDetected(deviceId) != Success) {
        return ErrorDeviceNotFound;
    }

    DeviceTypes_t deviceType;

    if (EmcrFtdiDevice::getDeviceType(deviceId, deviceType) != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    return Success;
}

ErrorCodes_t EmcrFtdiDevice::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        return ErrorDeviceAlreadyConnected;
    }

    DeviceTypes_t deviceType;

    ret = EmcrFtdiDevice::getDeviceType(deviceId, deviceType);
    if (ret != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    switch (deviceType) {
    case DeviceE8nPatchEL07C_artix7_PCBV01_FW1:
        messageDispatcher = new Emcr8nPatchClamp_EL07c_artix7_PCBV01_fw_v01(deviceId);
        break;

    case DeviceE8PPatchEL07CD_artix7_PCBV01_FW3:
        messageDispatcher = new Emcr8PatchClamp_EL07c_artix7_PCBV01_fw_v03(deviceId);
        break;

    case DeviceE8PPatchEL07CD_artix7_PCBV01_FW2:
        messageDispatcher = new Emcr8PatchClamp_EL07c_artix7_PCBV01_fw_v02(deviceId);
        break;

    case DeviceE8PPatchEL07CD_artix7_PCBV00_2:
        messageDispatcher = new Emcr8PatchClamp_EL07c_artix7_PCBV00_2_fw_v01(deviceId);
        break;

    case DeviceE8PPatchEL07CD_artix7_PCBV00_1:
        messageDispatcher = new Emcr8PatchClamp_EL07c_artix7_PCBV00_1_fw_v01(deviceId);
        break;

    case DeviceE4PPatchEL07CD_artix7_PCBV00_1:
        messageDispatcher = new Emcr4PatchClamp_EL07c_artix7_PCBV00_1_fw_v01(deviceId);
        break;

    case DeviceE4PPatchEL07CD_artix7_PCBV00_2:
        messageDispatcher = new Emcr4PatchClamp_EL07c_artix7_PCBV00_2_fw_v01(deviceId);
        break;

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

ErrorCodes_t EmcrFtdiDevice::pauseConnection(bool pauseFlag) {
    ErrorCodes_t ret = Success;
    if (pauseFlag) {
        FT_STATUS ftRet;
        ftRet = FT_Close(* ftdiRxHandle);
        if (ftRet != FT_OK) {
            return ErrorDeviceDisconnectionFailed;
        }

        if (rxChannel != txChannel) {
            FT_STATUS ftRet;
            ftRet = FT_Close(* ftdiTxHandle);
            if (ftRet != FT_OK) {
                return ErrorDeviceDisconnectionFailed;
            }
        }

    } else {
        /*! Initialize the ftdi Rx handle */
        ret = this->initFtdiChannel(ftdiRxHandle, rxChannel);
        if (ret != Success) {
            return ret;
        }

        if (rxChannel == txChannel) {
            ftdiTxHandle = ftdiRxHandle;

        } else {
            /*! Initialize the ftdi Tx handle */
            ret = this->initFtdiChannel(ftdiTxHandle, txChannel);
            if (ret != Success) {
                return ret;
            }
        }
    }
    return ret;
}

ErrorCodes_t EmcrFtdiDevice::disconnectDevice() {
    this->deinitialize();
    return Success;
}

ErrorCodes_t EmcrFtdiDevice::getCalibrationEepromSize(uint32_t &size) {
    if (calibrationEeprom == nullptr) {
        size = 0;
        return ErrorEepromNotConnected;
    }
    size = calibrationEeprom->getMemorySize();
    return Success;
}

ErrorCodes_t EmcrFtdiDevice::writeCalibrationEeprom(std::vector <uint32_t> value, std::vector <uint32_t> address, std::vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (calibrationEeprom == nullptr) {
        return ErrorEepromNotConnected;
    }
    std::unique_lock <std::mutex> connectionMutexLock(connectionMutex);

    ret = this->pauseConnection(true);
    calibrationEeprom->openConnection();

    unsigned char eepromBuffer[4];
    for (unsigned int itemIdx = 0; itemIdx < value.size(); itemIdx++) {
        for (uint32_t bufferIdx = 0; bufferIdx < size[itemIdx]; bufferIdx++) {
            eepromBuffer[size[itemIdx]-bufferIdx-1] = value[itemIdx] & 0x000000FF;
            value[itemIdx] >>= 8;
        }

        ret = calibrationEeprom->writeBytes(eepromBuffer, address[itemIdx], size[itemIdx]);
    }

    calibrationEeprom->closeConnection();
    this->pauseConnection(false);

    connectionMutexLock.unlock();

    /*! \todo FCON trovare un altro modo per pingare */
//    RxOutput_t rxOutput;
//    ret = ErrorUnknown;
//    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;
//    int pingTries = 0;

//    while (ret != Success) {
//        if (pingTries++ > EZP_MAX_PING_TRIES) {
//            return ErrorConnectionPingFailed;
//        }

//        ret = this->ping();
//        if (ret != Success) {
//            ret = this->pauseConnection(true);
//            calibrationEeprom->openConnection();

//            calibrationEeprom->closeConnection();
//            this->pauseConnection(false);
//        }
//    }

    /*! Make a chip reset to force resynchronization of chip states. This is important when the FPGA has just been reset */
    this->resetAsic(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    this->resetAsic(false, true);

    return ret;
}

ErrorCodes_t EmcrFtdiDevice::readCalibrationEeprom(std::vector <uint32_t> &value, std::vector <uint32_t> address, std::vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (calibrationEeprom == nullptr) {
        return ErrorEepromNotConnected;
    }
    std::unique_lock <std::mutex> connectionMutexLock(connectionMutex);
    ret = this->pauseConnection(true);
    calibrationEeprom->openConnection();

    if (value.size() != address.size()) {
        value.resize(address.size());
    }

    unsigned char eepromBuffer[4];
    for (unsigned int itemIdx = 0; itemIdx < value.size(); itemIdx++) {
        ret = calibrationEeprom->readBytes(eepromBuffer, address[itemIdx], size[itemIdx]);

        value[itemIdx] = 0;
        for (uint32_t bufferIdx = 0; bufferIdx < size[itemIdx]; bufferIdx++) {
            value[itemIdx] <<= 8;
            value[itemIdx] += static_cast <uint32_t> (eepromBuffer[bufferIdx]);
        }
    }

    calibrationEeprom->closeConnection();
    this->pauseConnection(false);

    connectionMutexLock.unlock();

    /*! \todo FCON trovare un altro modo per pingare */
//    RxOutput_t rxOutput;
//    ret = ErrorUnknown;
//    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;
//    int pingTries = 0;

//    while (ret != Success) {
//        if (pingTries++ > EZP_MAX_PING_TRIES) {
//            return ErrorConnectionPingFailed;
//        }

//        ret = this->ping();
//    }

    /*! Make a chip reset to force resynchronization of chip states. This is important when the FPGA has just been reset */
    this->resetAsic(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    this->resetAsic(false, true);

    return ret;
}

int32_t EmcrFtdiDevice::getDeviceIndex(std::string serial) {
    /*! Gets number of devices */
    DWORD numDevs;
    bool devCountOk = getDeviceCount(numDevs);
    if (!devCountOk) {
        return -1;

    } else if (numDevs == 0) {
        return -1;
    }

    for (int index = 0; index < numDevs; index++) {
        std::string deviceId = getDeviceSerial(index, false);
        if (deviceId == serial) {
            return index;
        }
    }
    return -1;
}

std::string EmcrFtdiDevice::getDeviceSerial(uint32_t index, bool excludeLetter) {
    char buffer[64];
    std::string serial;
    FT_STATUS FT_Result = FT_ListDevices((PVOID)index, buffer, FT_LIST_BY_INDEX);
    if (FT_Result == FT_OK) {
        serial = buffer;
        if (excludeLetter) {
            return serial.substr(0, serial.size()-1); /*!< Removes channel character */

        } else {
            return serial;
        }

    } else {
        return "";
    }
}

bool EmcrFtdiDevice::getDeviceCount(DWORD &numDevs) {
    /*! Get the number of connected devices */
    numDevs = 0;
    FT_STATUS FT_Result = FT_ListDevices(&numDevs, nullptr, FT_LIST_NUMBER_ONLY);
    if (FT_Result == FT_OK) {
        return true;

    } else {
        return false;
    }
}

ErrorCodes_t EmcrFtdiDevice::startCommunication(std::string) {
    ErrorCodes_t ret = this->loadFpgaFw();
    if (ret != Success) {
        return ret;
    }

    /*! Initialize the ftdi Rx handle */
    ftdiRxHandle = new FT_HANDLE;

    ret = this->initFtdiChannel(ftdiRxHandle, rxChannel);
    if (ret != Success) {
        return ret;
    }

    if (rxChannel == txChannel) {
        ftdiTxHandle = ftdiRxHandle;
        return ret;

    }
    /*! Initialize the ftdi Tx handle */
    ftdiTxHandle = new FT_HANDLE;

    return this->initFtdiChannel(ftdiTxHandle, txChannel);
}

ErrorCodes_t EmcrFtdiDevice::stopCommunication() {
    FT_STATUS ftRet;
    if (ftdiRxHandle != nullptr) {
        ftRet = FT_Close(* ftdiRxHandle);
        if (ftRet != FT_OK) {
            return ErrorDeviceDisconnectionFailed;
        }
        delete ftdiRxHandle;
        ftdiRxHandle = nullptr;
        if (txChannel == rxChannel) {
            ftdiTxHandle = nullptr;
        }
    }

    if (ftdiTxHandle != nullptr) {
        if (rxChannel != txChannel) {
            FT_STATUS ftRet;
            ftRet = FT_Close(* ftdiTxHandle);
            if (ftRet != FT_OK) {
                return ErrorDeviceDisconnectionFailed;
            }
        }
        delete ftdiTxHandle;
        ftdiTxHandle = nullptr;
    }

    return Success;
}

void EmcrFtdiDevice::initializeCalibration() {
    calibrationEeprom = new CalibrationEeprom(this->getDeviceIndex(deviceId+spiChannel));
    Measurement_t zeroA = {0.0, UnitPfxNone, "A"};
    calibrationParams.initialize(CalTypesVcOffsetAdc, this->getSamplingRateModesNum(), vcCurrentRangesNum, currentChannelsNum, zeroA);
    Measurement_t zeroV = {0.0, UnitPfxNone, "V"};
    calibrationParams.initialize(CalTypesCcOffsetAdc, this->getSamplingRateModesNum(), ccVoltageRangesNum, voltageChannelsNum, zeroV);
}

void EmcrFtdiDevice::deinitializeCalibration() {
    if (calibrationEeprom != nullptr) {
        delete calibrationEeprom;
        calibrationEeprom = nullptr;
    }
}

void EmcrFtdiDevice::handleCommunicationWithDevice() {
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
    parsingStatus = ParsingParsing;

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

            if (!resetStateFlag) {
                anyOperationPerformed = true;

                uint32_t bytesRead = this->readDataFromDevice();

                if (bytesRead > 0) {
                    rxRawMutexLock.lock();
                    rxRawBufferReadLength += bytesRead;
                    rxRawMutexLock.unlock();
                    rxRawBufferNotEmpty.notify_all();
                }
            }
        }

        if (!anyOperationPerformed) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    }
}

void EmcrFtdiDevice::sendCommandsToDevice() {
    int wordsNum = txMsgToBeSentWords[txMsgBufferReadOffset].size();
    int regsNum = wordsNum/2;
    txRawBulkBuffer[2] = regsNum;

    int writeTries = 0;

    bool notSentTxData;

    /*! Moving from 16 bits words to 32 bits registers (+= 2, /2, etc, are due to this conversion) */
    for (uint32_t txDataBufferReadIdx = 0; txDataBufferReadIdx < wordsNum; txDataBufferReadIdx += 2) {
        txRawBulkBuffer[3+txDataBufferReadIdx] = txMsgToBeSentWords[txMsgBufferReadOffset][txDataBufferReadIdx]/2;
        txRawBulkBuffer[3+txDataBufferReadIdx+1] =
                ((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx] +
                 ((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx+1] << 16)); /*! Little endian */
    }
    TxTriggerType_t type = txMsgOption[txMsgBufferReadOffset].triggerType;
    switch (txMsgOption[txMsgBufferReadOffset].resetControl) {
    case ResetTrue:
        resetStateFlag = true;
        break;

    case ResetFalse:
        resetStateFlag = false;
        break;
    }

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

bool EmcrFtdiDevice::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    if (this->writeRegisters() == false) {
        return false;
    }
    switch (type) {
    case TxTriggerStartProtocol:
        std::this_thread::sleep_for (std::chrono::milliseconds(5));
        return this->activateTriggerIn(EMF_START_PROTOCOL_TRIGGER_IN_ADDR, EMF_START_PROTOCOL_TRIGGER_IN_BIT);
        break;

    case TxTriggerZap:
        std::this_thread::sleep_for (std::chrono::milliseconds(5));
        return this->activateTriggerIn(EMF_ZAP_PULSE_TRIGGER_IN_ADDR, EMF_ZAP_PULSE_TRIGGER_IN_BIT);
        break;
    }
    return true;
}

uint32_t EmcrFtdiDevice::readDataFromDevice() {
    /*! Declare variables to manage buffers indexing */
    DWORD ftdiQueuedBytes = 0;
    std::chrono::milliseconds longBytesWait(2);
    std::chrono::milliseconds shortBytesWait(1);
    DWORD ftdiReadBytes;

    /******************\
     *  Reading part  *
    \******************/

    /*! Read queue status to check the number of available bytes */
    FT_STATUS ftRet = FT_GetQueueStatus(* ftdiRxHandle, &ftdiQueuedBytes);
    std::unique_lock <std::mutex> connectionMutexLock(connectionMutex);
    if (ftRet != FT_OK) {
        connectionMutexLock.unlock();
        return 0;
    }

    if (ftdiQueuedBytes == 0) {
        connectionMutexLock.unlock();
        /*! If there are no bytes in the queue skip to next iteration in the while loop */
        std::this_thread::sleep_for(longBytesWait);
        return 0;

    } else if ((ftdiQueuedBytes < EMF_MIN_QUEUED_BYTES) && (readTries == 0)) {
        connectionMutexLock.unlock();
        readTries++;
        std::this_thread::sleep_for(shortBytesWait);
        return 0;
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
        return 0;
    }

#ifdef DEBUG_RX_RAW_DATA_PRINT
    fwrite(rxRawBuffer+rxRawBufferWriteOffset, sizeof(unsigned char), ftdiReadBytes, rxRawFid);
    fflush(rxRawFid);
#endif
    if (rxRawBufferWriteOffset == 0) {
        rxRawBuffer[FTD_RX_RAW_BUFFER_SIZE] = rxRawBuffer[0]; /*!< The last item is a copy of the first one, it used to safely read 2 consecutive bytes at a time to form a 16bit word,
                                                               *   even if the first byte is in position FTD_RX_RAW_BUFFER_SIZE-1 and the following one would go out of range otherwise */
    }

    /*! Update rxRawBufferWriteOffset to position to be written on next iteration */
    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+ftdiReadBytes)&rxRawBufferMask;
    return ftdiReadBytes;
}

void EmcrFtdiDevice::parseDataFromDevice() {
    RxParsePhase_t rxParsePhase = RxParseLookForHeader;

    rxRawBufferReadOffset = 0;
    uint32_t rxSyncWordSize = sizeof(rxSyncWord);
    uint32_t rxOffsetLengthSize = 2*RX_WORD_SIZE;
    uint32_t rxFrameOffset; /*!< Offset of the current frame */
    uint16_t rxWordOffset; /*!< Offset of the first word in the received frame */
    uint16_t rxWordsLength; /*!< Number of words in the received frame */
    uint32_t rxDataBytes; /*!< Number of bytes in the received frame */
    uint32_t rxCandidateHeader;
    int32_t dataLossCount = INT32_MIN; /*!< No data loss at the start of parsing */

    bool notEnoughRxData;

    /******************\
     *  Parsing part  *
    \******************/

    std::unique_lock <std::mutex> rxRawMutexLock(rxRawMutex);
    rxRawMutexLock.unlock();

    while (!stopConnectionFlag) {
        rxRawMutexLock.lock();
        while (rxRawBufferReadLength == 0 && !stopConnectionFlag) {
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
                        rxRawBufferReadOffset = (rxRawBufferReadOffset-3) & rxRawBufferMask;
                        rxRawBytesAvailable += 3;
                        dataLossCount += 1;
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
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & rxRawBufferMask;
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
                        /*! valid frame data and reset rxDataLoss */
                        frameManager->storeFrameDataLoss(dataLossCount);
                        dataLossCount = 0;

                        frameManager->storeFrameData(rxWordOffset);

                        rxFrameOffset = rxRawBufferReadOffset;
                        /*! remove the bytes that were not popped to read the next header */
                        rxRawBufferReadOffset = (rxRawBufferReadOffset+rxSyncWordSize) & rxRawBufferMask;
                        rxRawBytesAvailable -= rxSyncWordSize;

                        rxParsePhase = RxParseLookForLength;

                    } else {
                        /*! Sync word not found, restart looking from the previous sync word */
                        rxRawBufferReadOffset = (rxFrameOffset+rxSyncWordSize) & rxRawBufferMask;
                        /*! Offset and length are discarded, so add the corresponding bytes back */
                        rxRawBytesAvailable += rxOffsetLengthSize;
                        rxParsePhase = RxParseLookForHeader;
                        dataLossCount += 1;
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
    parsingStatus = ParsingNone;
}

ErrorCodes_t EmcrFtdiDevice::initializeMemory() {
    rxRawBuffer = new (std::nothrow) uint8_t[FTD_RX_RAW_BUFFER_SIZE+1];
    if (rxRawBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txRawBulkBuffer = new (std::nothrow) uint32_t[FTD_TX_RAW_BUFFER_SIZE];
    if (txRawBulkBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txRawTriggerBuffer = new (std::nothrow) uint32_t[EMF_TX_TRIGGER_BUFFER_SIZE];
    if (txRawTriggerBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    rxRawBuffer16 = (uint16_t *)rxRawBuffer;
    return EmcrDevice::initializeMemory();
}

void EmcrFtdiDevice::deinitializeMemory() {
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

ErrorCodes_t EmcrFtdiDevice::loadFpgaFw() {
    switch (fpgaLoadType) {
    case FtdiFpgaFwLoadAutomatic:
        /*! Nothing to be done, the FPGA will handle itself */
        break;

    case FtdiFpgaFwLoad8PatchArtix7_V01:
        /*! Need to set some pins low to select FW loading from FLASH and then pulse Spi_Prog low to start FPGA configuration */
        FT_STATUS status;
        FT_HANDLE spiHandle;
        std::string spiChannelStr = deviceId+spiChannel;

        Init_libMPSSE();

        int idx = getDeviceIndex(spiChannelStr);
        if (idx < 0) {
            return ErrorEepromConnectionFailed;
        }

        status = SPI_OpenChannel(idx, &spiHandle);
        if (status != FT_OK) {
            return ErrorEepromConnectionFailed;
        }

        ChannelConfig spiConfig;
        spiConfig.ClockRate = 3000000; /*! 3MHz */
        spiConfig.LatencyTimer = 1; /*! 1ms */
        spiConfig.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS4 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
        spiConfig.Pin             = 0x00000203; //State of lines after init: xdbus0:clk:out, xdbus1:mosi:out xdbus2:din:miso:in xdbus4:cs:in xdbus6:spi_init:in, clk:0 mosi:1 in idle
        //State of lines after deinitialization: all inputs
        status = SPI_InitChannel(spiHandle, &spiConfig);
        if (status != FT_OK) {
            return ErrorEepromConnectionFailed;
        }

        uint8_t fpgaResetBit = 0x01;
        uint8_t swFtdiInBit = 0x02;
        uint8_t swFtdiMosiBit = 0x04;
        uint8_t progBBit = 0x08;
        uint8_t swFlashFcsBBit = 0x10;
        uint8_t swFpgaModeBit = 0x20;
        uint8_t xCbusDir = fpgaResetBit | swFtdiInBit | swFtdiMosiBit | progBBit | swFlashFcsBBit | swFpgaModeBit;

        FT_WriteGPIO(spiHandle, xCbusDir, fpgaResetBit); // lower bits of the switches to select master mode and prog B to put FPGA in reset
        FT_WriteGPIO(spiHandle, xCbusDir, progBBit); // prog B high to start FPGA configuration (it reads the FLASH)

        std::this_thread::sleep_for(std::chrono::seconds(35));

        SPI_CloseChannel(spiHandle);
        Cleanup_libMPSSE();

        break;
    }
    return Success;
}

ErrorCodes_t EmcrFtdiDevice::initFtdiChannel(FT_HANDLE * handle, char channel) {
    FT_STATUS ftRet;

    std::string communicationSerialNumber = deviceId+channel;

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

        if (ftRet != FT_OK) {
            FT_Close(* handle);
            return ErrorFtdiConfigurationFailed;
        }
    }

    return Success;
}

bool EmcrFtdiDevice::writeRegisters() {
    return writeToBulkOut(txRawBulkBuffer);
}

bool EmcrFtdiDevice::activateTriggerIn(int address, int bit) {
    txRawTriggerBuffer[3] = (uint32_t)address;
    txRawTriggerBuffer[4] = ((uint32_t)1) << ((uint32_t)bit);
    return writeToBulkOut(txRawTriggerBuffer);
}

bool EmcrFtdiDevice::writeToBulkOut(uint32_t * buffer) {
    DWORD bytesToWrite = sizeof(uint32_t)*(3+2*buffer[2]);
    DWORD ftdiWrittenBytes;
    FT_STATUS ftRet;

    bool txDataSent = false;
    int writeTries = 0;
    std::unique_lock <std::mutex> connectionMutexLock(connectionMutex);
    connectionMutexLock.unlock();
    while (!txDataSent && (writeTries++ < EMF_MAX_WRITE_TRIES)) { /*! \todo FCON prevedere un modo per notificare ad alto livello e all'utente */
        connectionMutexLock.lock();
        ftRet = FT_Write(* ftdiTxHandle, (LPVOID)buffer, bytesToWrite, &ftdiWrittenBytes);
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
            txDataSent = true;
            writeTries = 0;
        }
    }
    return txDataSent;
}
