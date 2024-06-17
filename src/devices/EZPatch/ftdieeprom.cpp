#include "ftdieeprom.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
using namespace e384CommLib;
#endif

FtdiEeprom::FtdiEeprom(std::string deviceId) :
    deviceId(deviceId) {

}

FtdiEepromId_t FtdiEeprom::getFtdiEepromId(std::string deviceId) {
    if (deviceId == "ePatch Demo") {
        return FtdiEepromIdDemo;
    }

    if (deviceId == "eP8 Demo") {
        return FtdiEepromIdDemo;
    }
    return FtdiEepromId56;
}

ErrorCodes_t FtdiEeprom::openConnection(char channel) {
    ErrorCodes_t ret;
    if (!connectionOpened) {
        /*! Appends the channel to the serial */
        communicationSerial = deviceId+channel;

        /*! Opens the connection with the handler */
        FT_STATUS result = FT_OpenEx((PVOID)communicationSerial.c_str(), FT_OPEN_BY_SERIAL_NUMBER, &handler);
        connectionOpened = (result == FT_OK);

        if (connectionOpened) {
            ret = Success;

        } else {
            ret = ErrorEepromConnectionFailed;
        }

    } else {
        ret = ErrorEepromAlreadyConnected;
    }
    return ret;
}

ErrorCodes_t FtdiEeprom::closeConnection() {
    ErrorCodes_t ret;
    if (connectionOpened) {
        connectionOpened = (FT_Close(handler) != FT_OK);

        if (connectionOpened) {
            ret = ErrorEepromDisconnectionFailed;

        } else {
            ret = Success;
        }

    } else {
        ret = ErrorEepromNotConnected;
    }
    return ret;
}

ErrorCodes_t FtdiEeprom::readEepromWord(DWORD address, LPWORD result) {
    ErrorCodes_t ret;
    if (connectionOpened) {
        FT_STATUS ftRet = FT_ReadEE(handler, address, result);
        if (ftRet != FT_OK) {
            ret = ErrorEepromReadFailed;

        } else {
            ret = Success;
        }

    } else {
        ret = ErrorEepromNotConnected;
    }
    return ret;
}

void FtdiEeprom::calculateEncryptionKey() {
    for (int32_t i = 0; i < 8; i++) {
        encryptionKey[i] = (uint16_t)(deviceId[i]);
    }
}

DeviceTuple_t FtdiEeprom::getDeviceTuple() {
    return deviceTuple;
}
