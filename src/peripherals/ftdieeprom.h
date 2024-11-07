#ifndef FTDIEEPROM_H
#define FTDIEEPROM_H

#include <string>
#include <vector>

#include "e384commlib_errorcodes.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
using namespace e384CommLib;
#endif

#define NOMINMAX
#ifdef _WIN32 /*! _WIN32 isolates both 32 and 64 bit windows systems, _WIN64 isolates only 64 */
#include <windows.h>
#include "ftd2xx_win.h"

#else /*! Same FTDI header file name also for linux and macOS */
/*! libftdi must be properly set in system folders (/usr/local/lib ... ecc) */
#include "ftd2xx.h"
#endif

#define EEPROM_ENCRYPTION_KEY_LENGTH 8

typedef enum {
    FtdiEepromId56,
    FtdiEepromIdDemo
} FtdiEepromId_t;

typedef enum {
    DeviceVersionTestBoard = 6,
    DeviceVersionEpatch = 7,
    DeviceVersionE4p = 10,
    DeviceVersionDemo = 0xFD,
    DeviceVersionPrototype = 0xFE,
    DeviceVersionUndefined = 0xFF
} DeviceVersion_t;

typedef enum {
    /*! Subversions used for ver = 6 */
    DeviceSubversionTestBoardEL04d = 9,
    DeviceSubversionTestBoardEL04e = 10,
    DeviceSubversionTestBoardEL04f = 11,

    /*! Subversions used for ver = 7 */
    DeviceSubversionEl03D = 1,
    DeviceSubversionEl03F_4D_PCBV02 = 2,
    DeviceSubversionEl03F_4E_PCBV02 = 3,
    DeviceSubversionEl04F = 4,
    DeviceSubversionEl03F_4D_PCBV03 = 5,
    DeviceSubversionEl03F_4E_PCBV03 = 6,
    DeviceSubversionEl03F_4F_PCBV03 = 8,
    DeviceSubversionEl03F_4F_PCBV01_AnalogOut = 9,
    DeviceSubversionEl03F_4F_PCBV02 = 10,
    DeviceSubversionEl03F_4F_PCBV03b = 11,

    /*! Subversions used for ver = 10 */
    DeviceSubversionEl04Fx4 = 1,
    DeviceSubversionEl04Fx4Patch = 2,
    DeviceSubversionEl04Fx8Patch = 3,
    DeviceSubversionEl07ABx4Patch = 5,
    DeviceSubversionEl07ABx8Patch = 6,
    DeviceSubversionEl07ABx8Patch_artix7_PCBV01 = 8,
    DeviceSubversionEl07ABx8Patch_artix7_PCBV02 = 10,
    DeviceSubversionEl07CDx8Patch_artix7_PCBV01 = 12,
    DeviceSubversionEl07CDx8Patch_artix7_PCBV02 = 14,
    DeviceSubversionEl07CDx4Patch_artix7_PCBV01 = 16,
    DeviceSubversionEl07CDx4Patch_artix7_PCBV02 = 17,

    /*! Subversions used for ver = FD */
    DeviceSubversionDemo = 1,
    DeviceSubversionDemox8 = 2,

    /*! Subversions used for ver = FE */
    DeviceSubversionProtoEl04E = 9,
    DeviceSubversionProtoEl03F_4E = 10,

    /*! Subversions used for ver = 0xFF */
    DeviceSubversionUndefined = 0xFF
} DeviceSubversion_t ;

typedef struct DeviceTuple {
    DeviceVersion_t version = DeviceVersionUndefined;
    DeviceSubversion_t subversion = DeviceSubversionUndefined;
    uint32_t fwVersion = 0;
} DeviceTuple_t;

inline bool operator == (const DeviceTuple_t &a, const DeviceTuple_t &b) {
    return ((a.version == b.version) && (a.subversion == b.subversion) && (a.fwVersion == b.fwVersion));
}

inline bool operator != (const DeviceTuple_t &a, const DeviceTuple_t &b) {
    return !(a == b);
}

class FtdiEeprom {
public:
    FtdiEeprom(std::string deviceId);

    static FtdiEepromId_t getFtdiEepromId(std::string deviceId);
    virtual ErrorCodes_t openConnection(char channel = 'A');
    virtual ErrorCodes_t closeConnection();
    DeviceTuple_t getDeviceTuple();
    ErrorCodes_t readEepromWord(DWORD address, LPWORD result);

protected:
    std::string deviceId;
    char communicationChannel;
    std::string communicationSerial;
    uint16_t encryptionKey[EEPROM_ENCRYPTION_KEY_LENGTH];
    DeviceTuple_t deviceTuple;
    bool connectionOpened = false;
    FT_HANDLE handler;

    void calculateEncryptionKey();

    virtual ErrorCodes_t loadData() = 0;
    virtual ErrorCodes_t loadDeviceTuple() = 0;
};

#endif // FTDIEEPROM_H
