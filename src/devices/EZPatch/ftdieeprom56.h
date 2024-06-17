#ifndef FTDIEEPROM56_H
#define FTDIEEPROM56_H

#include "ftdieeprom.h"

#define E56_DEVICE_VER_ADDR 0x074
#define E56_FW_VER_ADDR 0x0075

class FtdiEeprom56 : public FtdiEeprom {
public:
    FtdiEeprom56(std::string deviceId);

protected:
    /*! FtdiEeprom interface */
    ErrorCodes_t loadData() override;
    ErrorCodes_t loadDeviceTuple() override;

private:
    /*! If the value is an uint16_t the key values get duplicated also in the MSByte before */
    uint16_t wordEncrypt(uint16_t value, uint16_t * key);
    uint16_t wordDecrypt(uint16_t value, uint16_t * key);
};

#endif // FTDIEEPROM56_H
