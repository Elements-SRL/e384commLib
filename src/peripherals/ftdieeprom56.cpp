#include "ftdieeprom56.h"

FtdiEeprom56::FtdiEeprom56(std::string deviceId) :
    FtdiEeprom(deviceId) {

    this->loadData();
}

ErrorCodes_t FtdiEeprom56::loadData() {
    ErrorCodes_t ret;
    ret = this->openConnection();
    if ((ret != Success) && (ret != ErrorEepromAlreadyConnected)) {
        return ret;
    }

    this->calculateEncryptionKey();
    ret = this->loadDeviceTuple();
    if (ret != Success) {
        return ret;
    }

    ret = this->closeConnection();
    if (ret != Success) {
        return ret;
    }

    return ret;
}

ErrorCodes_t FtdiEeprom56::loadDeviceTuple() {
    ErrorCodes_t ret;
    WORD firmware;
    WORD versionSubversion;

    ret = this->readEepromWord(E56_DEVICE_VER_ADDR, &versionSubversion);
    if (ret != Success) {
        return ret;
    }

    ret = this->readEepromWord(E56_FW_VER_ADDR, &firmware);
    if (ret != Success) {
        return ret;
    }

    deviceTuple.version = (DeviceVersion_t)((versionSubversion >> 8) & 0xFF);
    deviceTuple.subversion = (DeviceSubversion_t)(versionSubversion & 0xFF);
    deviceTuple.fwVersion = firmware;

    return ret;
}

uint16_t FtdiEeprom56::wordEncrypt(uint16_t value, uint16_t * key) {
    uint16_t temp = 0;
    for (int32_t idx = 0; idx < EEPROM_ENCRYPTION_KEY_LENGTH; idx++) {
        temp = key[idx] + (key[idx] << 8);
        value = value ^ temp;
    }
    return value;
}

uint16_t FtdiEeprom56::wordDecrypt(uint16_t value, uint16_t * key) {
    uint16_t temp = 0;
    for (int32_t idx = EEPROM_ENCRYPTION_KEY_LENGTH-1; idx >= 0; idx--) {
        temp = key[idx] + (key[idx] << 8);
        value = value ^ temp;
    }
    return value;
}
