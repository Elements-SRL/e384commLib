#include "ezpatchftdidevice.h"
#include "utils.h"

#include "libMPSSE_spi.h"

#include "ezpatchepatchel03d.h"
#include "ezpatchepatchel03f_4d.h"
#include "ezpatchepatchel04e.h"
#include "ezpatchepatchel03f_4e.h"
#include "ezpatchepatchel04f.h"
#include "ezpatchepatchel03f_4f.h"
#include "ezpatche4pel04f.h"
#include "ezpatche4ppatchliner.h"
#include "ezpatche8ppatchliner.h"
#include "ezpatche4ppatchliner_el07ab.h"
#include "ezpatche8ppatchliner_el07ab.h"
#ifdef DEBUG
/*! Fake device that generates synthetic data */
#include "ezpatchfakepatch.h"
#include "ezpatchfakep8.h"
#endif

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

static const std::vector <std::vector <uint32_t>> deviceTupleMapping = {
    /*! ePatch EL03D */
    {DeviceVersionEpatch, DeviceSubversionEl03D, 133, DeviceEPatchEL03D_V04},                                               //  7,  1,  133 : digital repetitive trigger
    {DeviceVersionEpatch, DeviceSubversionEl03D, 132, DeviceEPatchEL03D_V03},                                               //  7,  1,  132 : increased digital triggers to 84
    {DeviceVersionEpatch, DeviceSubversionEl03D, 131, DeviceEPatchEL03D_V02},                                               //  7,  1,  131 : implemented holding voltage tuner
    {DeviceVersionEpatch, DeviceSubversionEl03D, 130, DeviceEPatchEL03D_V01},                                               //  7,  1,  130 : implemented trigger digital outputs
    {DeviceVersionEpatch, DeviceSubversionEl03D, 129, DeviceEPatchEL03D_V00},                                               //  7,  1,  129 : reduction of SPI noise
    {DeviceVersionEpatch, DeviceSubversionEl03D, 128, DeviceEPatchEL03D_V00},                                               //  7,  1,  128 : first version
    {DeviceVersionEpatch, 2, 0, DeviceEPatchEL03D_V00},                                                                     //  7,  2,  0   : first version (legacy, to be removed)
    /*! ePatch EL03F-4D */
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV03, 140, DeviceEPatchEL03F_4D_PCBV03_V04},                           //  7,  5,  140 : digital repetitive trigger
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV03, 138, DeviceEPatchEL03F_4D_PCBV03_V03},                           //  7,  5,  138 : increased digital triggers to 84
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV03, 136, DeviceEPatchEL03F_4D_PCBV03_V02},                           //  7,  5,  136 : Slave/Master configurability
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV02, 139, DeviceEPatchEL03F_4D_PCBV02_V04},                           //  7,  2,  139 : digital repetitive trigger
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV02, 137, DeviceEPatchEL03F_4D_PCBV02_V03},                           //  7,  2,  137 : increased digital triggers to 84
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV02, 135, DeviceEPatchEL03F_4D_PCBV02_V02},                           //  7,  5,  135 : improved current clamp noise at low sampling rates
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV02, 135, DeviceEPatchEL03F_4D_PCBV02_V02},                           //  7,  2,  135 : improved current clamp noise at low sampling rates
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV02, 134, DeviceEPatchEL03F_4D_PCBV02_V02},                           //  7,  2,  134 : implemented holding voltage and current tuner
    {DeviceVersionEpatch, DeviceSubversionEl03F_4D_PCBV02, 133, DeviceEPatchEL03F_4D_PCBV02_V01},                           //  7,  2,  133 : implemented trigger digital outputs
    /*! ePatch EL03F-4E */
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV03, 137, DeviceEPatchEL03F_4E_PCBV03_V04},                           //  7,  6,  137 : digital repetitive trigger
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV03, 135, DeviceEPatchEL03F_4E_PCBV03_V03},                           //  7,  6,  135 : increased digital triggers to 84
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV03, 133, DeviceEPatchEL03F_4E_PCBV03_V02},                           //  7,  6,  133 : Slave/Master configurability
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV02, 136, DeviceEPatchEL03F_4E_PCBV02_V04},                           //  7,  3,  136 : digital repetitive trigger
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV02, 134, DeviceEPatchEL03F_4E_PCBV02_V03},                           //  7,  3,  134 : increased digital triggers to 84
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV02, 132, DeviceEPatchEL03F_4E_PCBV02_V02},                           //  7,  6,  132 : improved current clamp noise at low sampling rates
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV02, 132, DeviceEPatchEL03F_4E_PCBV02_V02},                           //  7,  3,  132 : improved current clamp noise at low sampling rates
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV02, 131, DeviceEPatchEL03F_4E_PCBV02_V02},                           //  7,  3,  131 : implemented holding voltage and current tuner
    {DeviceVersionEpatch, DeviceSubversionEl03F_4E_PCBV02, 130, DeviceEPatchEL03F_4E_PCBV02_V01},                           //  7,  3,  130 : implemented trigger digital outputs
    {DeviceVersionPrototype, DeviceSubversionProtoEl03F_4E, 129, DeviceEPatchEL03F_4E_PCBV02_V00},                          //  254,10, 129 : VC-CC prototype 2 chips (EL03F, EL04E) (legacy, to be removed)
    /*! ePatch EL03F-4F */
    {DeviceVersionEpatch, DeviceSubversionEl03F_4F_PCBV01_AnalogOut, 129, DeviceEPatchEL03F_4F_PCBV01_AnalogOut},           //  7,  9,  129 : added analog output.
    {DeviceVersionEpatch, DeviceSubversionEl03F_4F_PCBV03, 130, DeviceEPatchEL03F_4F_PCBV03_V04},                           //  7,  8,  130 : digital repetitive trigger
    {DeviceVersionEpatch, DeviceSubversionEl03F_4F_PCBV02, 131, DeviceEPatchEL03F_4F_PCBV02_V04},                           //  7, 10,  131 : digital repetitive trigger
    {DeviceVersionEpatch, DeviceSubversionEl03F_4F_PCBV03, 129, DeviceEPatchEL03F_4F_PCBV03_V03},                           //  7,  8,  129 : increased digital triggers to 84
    /*! ePatch EL04E */
    {DeviceVersionPrototype, DeviceSubversionProtoEl04E, 128, DeviceEPatchEL04E},                                           //  254,9,  128 : VC-CC prototype 1 chip (EL04E)
    /*! ePatch EL04F */
    {DeviceVersionEpatch, DeviceSubversionEl04F, 129, DeviceEPatchEL04F},                                                   //  7,  4,  129 : VC-CC device with 1 chip (EL04F)
    {DeviceVersionEpatch, DeviceSubversionEl04F, 131, DeviceEPatchEL04F},                                                   //  7,  4,  131 : VC-CC device with 1 chip (EL04F)
    /*! e4P EL04F */
    {DeviceVersionE4p, DeviceSubversionEl04Fx4, 135, DeviceE4PEL04F},                                                       //  10, 1,  135 : VC-CC device with 4 channels (EL04F)
    {DeviceVersionE4p, DeviceSubversionEl04Fx4PatchLiner, 134, DeviceE4PPatchLiner},                                        //  10, 2,  134 : VC-CC device with 4 channels (EL04F) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx4PatchLiner, 135, DeviceE4PPatchLiner},                                        //  10, 2,  135 : VC-CC device with 4 channels (EL04F) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 129, DeviceE8PPatchLiner},                                        //  10, 3,  129 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 135, DeviceE8PPatchLiner},                                        //  10, 3,  135 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner compatible with dynamic clamp both in voltage and current clamp. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 134, DeviceE8PPatchLiner},                                        //  10, 3,  134 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner compatible with dynamic clamp, only in current clamp (worked only in voltage clamp before). */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 133, DeviceE8PPatchLiner},                                        //  10, 3,  133 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner compatible with dynamic clamp, fixed sampling of DC output. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 132, DeviceE8PPatchLiner},                                        //  10, 3,  132 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner compatible with dynamic clamp. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 131, DeviceE8PPatchLiner},                                        //  10, 3,  131 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner fix on digital pins mapping. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 130, DeviceE8PPatchLiner},                                        //  10, 3,  130 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl04Fx8PatchLiner, 129, DeviceE8PPatchLiner},                                        //  10, 3,  129 : VC-CC device with 8 channels (EL04F) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx4PatchLiner, 129, DeviceE4PPatchLinerEL07AB},                                 //  10, 5,  129 : VC-CC device with 4 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 136, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  136 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 135, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  135 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 134, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  134 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 133, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  133 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 132, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  132 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 131, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  131 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 130, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  130 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner, 129, DeviceE8PPatchLinerEL07AB},                                 //  10, 6,  129 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner_artix7_PCBV02, 131, DeviceE8PPatchLinerEL07AB_artix7_PCBV02_V02}, //  10,10,  131 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner (FPGA artix7) PCB V02. increased protocol items to 256. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner_artix7_PCBV02, 129, DeviceE8PPatchLinerEL07AB_artix7_PCBV02_V01}, //  10,10,  129 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner (FPGA artix7) PCB V02. */
    {DeviceVersionE4p, DeviceSubversionEl07ABx8PatchLiner_artix7_PCBV01, 129, DeviceE8PPatchLinerEL07AB_artix7_PCBV01},     //  10, 8,  129 : VC-CC device with 8 channels (EL07AB) for Nanion's Patchliner (FPGA artix7) PCB V01. */
    /*! Test boards */
    {DeviceVersionTestBoard, DeviceSubversionTestBoardEL04d, 129, DeviceEPatchEL04E},                                       //  6,  9,  129 : test board for EL04d chips (only current clamp works)
    {DeviceVersionTestBoard, DeviceSubversionTestBoardEL04e, 129, DeviceEPatchEL04E},                                       //  6, 10,  129 : test board for EL04e chips
    {DeviceVersionTestBoard, DeviceSubversionTestBoardEL04f, 131, DeviceEPatchEL04F},                                       //  6, 11,  131 : test board for EL04f chips
    {DeviceVersionTestBoard, DeviceSubversionTestBoardEL04f, 129, DeviceEPatchEL04F},                                       //  6, 11,  129 : test board for EL04f chips
    /*! ePatch fake */
    {DeviceVersionDemo, DeviceSubversionDemo, 129, DeviceFakePatch},                                                        //  253,1,  129 : fake patch
    /*! eP4 fake */
    {DeviceVersionDemo, DeviceSubversionDemox8, 129, DeviceFakeP8}                                                          //  253,2,  129 : fake patch 8 channels
};

EZPatchFtdiDevice::EZPatchFtdiDevice(std::string deviceId) :
    EZPatchDevice(deviceId) {

    ftdiEepromId = FtdiEepromId56;
    spiChannel = 'A';
    rxChannel = 'B';
    txChannel = 'B';

    rxSyncWord[0] = 0xA5;
    rxSyncWord[1] = 0x5A;

    rxCrcInitialValue = (((uint16_t)rxSyncWord[1]) << 8)+(uint16_t)rxSyncWord[0];

    txSyncWord[0] = 0xA5;
    txSyncWord[1] = 0x5A;

    txCrcInitialValue = (((uint16_t)txSyncWord[1]) << 8)+(uint16_t)txSyncWord[0];
}

EZPatchFtdiDevice::~EZPatchFtdiDevice() {
    this->disconnectDevice();
}

ErrorCodes_t EZPatchFtdiDevice::detectDevices(
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

ErrorCodes_t EZPatchFtdiDevice::getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion) {
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

ErrorCodes_t EZPatchFtdiDevice::getDeviceType(std::string deviceId, DeviceTypes_t &type) {
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

ErrorCodes_t EZPatchFtdiDevice::isDeviceSerialDetected(std::string deviceId) {
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

ErrorCodes_t EZPatchFtdiDevice::connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath) {
    ErrorCodes_t ret = Success;
    if (messageDispatcher != nullptr) {
        return ErrorDeviceAlreadyConnected;
    }

    DeviceTypes_t deviceType;

    ret = EZPatchFtdiDevice::getDeviceType(deviceId, deviceType);
    if (ret != Success) {
        return ErrorDeviceTypeNotRecognized;
    }

    switch (deviceType) {
    case DeviceEPatchEL03D_V04:
        messageDispatcher = new EZPatchePatchEL03D_V04(deviceId);
        break;

    case DeviceEPatchEL03D_V03:
        messageDispatcher = new EZPatchePatchEL03D_V03(deviceId);
        break;

    case DeviceEPatchEL03D_V02:
        messageDispatcher = new EZPatchePatchEL03D_V02(deviceId);
        break;

    case DeviceEPatchEL03D_V01:
        messageDispatcher = new EZPatchePatchEL03D_V01(deviceId);
        break;

    case DeviceEPatchEL03D_V00:
        messageDispatcher = new EZPatchePatchEL03D_V00(deviceId);
        break;

    case DeviceEPatchEL03F_4D_PCBV03_V04:
        messageDispatcher = new EZPatchePatchEL03F_4D_PCBV03_V04(deviceId);
        break;

    case DeviceEPatchEL03F_4D_PCBV03_V03:
        messageDispatcher = new EZPatchePatchEL03F_4D_PCBV03_V03(deviceId);
        break;

    case DeviceEPatchEL03F_4D_PCBV03_V02:
        messageDispatcher = new EZPatchePatchEL03F_4D_PCBV03_V02(deviceId);
        break;

    case DeviceEPatchEL03F_4D_PCBV02_V04:
        messageDispatcher = new EZPatchePatchEL03F_4D_PCBV02_V04(deviceId);
        break;

    case DeviceEPatchEL03F_4D_PCBV02_V03:
        messageDispatcher = new EZPatchePatchEL03F_4D_PCBV02_V03(deviceId);
        break;

    case DeviceEPatchEL03F_4D_PCBV02_V02:
        messageDispatcher = new EZPatchePatchEL03F_4D_PCBV02_V02(deviceId);
        break;

    case DeviceEPatchEL03F_4D_PCBV02_V01:
        messageDispatcher = new EZPatchePatchEL03F_4D_PCBV02_V01(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV03_V04:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV03_V04(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV03_V03:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV03_V03(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV03_V02:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV03_V02(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV02_V04:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV02_V04(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV02_V03:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV02_V03(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV02_V02:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV02_V02(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV02_V01:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV02_V01(deviceId);
        break;

    case DeviceEPatchEL03F_4E_PCBV02_V00:
        messageDispatcher = new EZPatchePatchEL03F_4E_PCBV02_V00(deviceId);
        break;

    case DeviceEPatchEL04E:
        messageDispatcher = new EZPatchePatchEL04E(deviceId);
        break;

    case DeviceEPatchEL03F_4F_PCBV01_AnalogOut:
        messageDispatcher = new EZPatchePatchEL03F_4F_PCBV01_AnalogOut_V01(deviceId);
        break;

    case DeviceEPatchEL03F_4F_PCBV03_V04:
        messageDispatcher = new EZPatchePatchEL03F_4F_PCBV03_V04(deviceId);
        break;

    case DeviceEPatchEL03F_4F_PCBV02_V04:
        messageDispatcher = new EZPatchePatchEL03F_4F_PCBV02_V04(deviceId);
        break;

    case DeviceEPatchEL03F_4F_PCBV03_V03:
        messageDispatcher = new EZPatchePatchEL03F_4F_PCBV03_V03(deviceId);
        break;

    case DeviceEPatchEL04F:
        messageDispatcher = new EZPatchePatchEL04F(deviceId);
        break;

    case DeviceE4PEL04F:
        messageDispatcher = new EZPatche4PEL04F(deviceId);
        break;

    case DeviceE4PPatchLiner:
        messageDispatcher = new EZPatche4PPatchliner(deviceId);
        break;

    case DeviceE8PPatchLiner:
        messageDispatcher = new EZPatche8PPatchliner(deviceId);
        break;

    case DeviceE4PPatchLinerEL07AB:
        messageDispatcher = new EZPatche4PPatchliner_el07ab(deviceId);
        break;

    case DeviceE8PPatchLinerEL07AB:
        messageDispatcher = new EZPatche8PPatchliner_el07ab(deviceId);
        break;

    case DeviceE8PPatchLinerEL07AB_artix7_PCBV01:
        messageDispatcher = new EZPatche8PPatchliner_el07ab_artix7_PCBV01(deviceId);
        break;

    case DeviceE8PPatchLinerEL07AB_artix7_PCBV02_V02:
        messageDispatcher = new EZPatche8PPatchliner_el07ab_artix7_PCBV02_V02(deviceId);
        break;

    case DeviceE8PPatchLinerEL07AB_artix7_PCBV02_V01:
        messageDispatcher = new EZPatche8PPatchliner_el07ab_artix7_PCBV02_V01(deviceId);
        break;

#ifdef DEBUG
    case DeviceFakePatch:
        messageDispatcher = new EZPatchFakePatch(deviceId);
        break;

    case DeviceFakeP8:
        messageDispatcher = new EZPatchFakeP8(deviceId);
        break;
#endif

    case DeviceEPatchDlp:
        messageDispatcher = new EZPatchePatchDlp(deviceId);
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

ErrorCodes_t EZPatchFtdiDevice::pauseConnection(bool pauseFlag) {
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

ErrorCodes_t EZPatchFtdiDevice::disconnectDevice() {
    this->deinitialize();
    return Success;
}

ErrorCodes_t EZPatchFtdiDevice::getCalibrationEepromSize(uint32_t &size) {
    if (calibrationEeprom == nullptr) {
        size = 0;
        return ErrorEepromNotConnected;
    }
    size = calibrationEeprom->getMemorySize();
    return Success;
}

ErrorCodes_t EZPatchFtdiDevice::writeCalibrationEeprom(std::vector <uint32_t> value, std::vector <uint32_t> address, std::vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (calibrationEeprom == nullptr) {
        ret = ErrorEepromNotConnected;
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

    RxOutput_t rxOutput;
    ret = ErrorUnknown;
    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;
    int pingTries = 0;

    while (ret != Success) {
        if (pingTries++ > EZP_MAX_PING_TRIES) {
            return ErrorConnectionPingFailed;
        }

        ret = this->ping();
        if (ret != Success) {
            ret = this->pauseConnection(true);
            calibrationEeprom->openConnection();

            calibrationEeprom->closeConnection();
            this->pauseConnection(false);
        }
    }

    /*! Make a chip reset to force resynchronization of chip states. This is important when the FPGA has just been reset */
    this->resetAsic(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    this->resetAsic(false, true);

    return ret;
}

ErrorCodes_t EZPatchFtdiDevice::readCalibrationEeprom(std::vector <uint32_t> &value, std::vector <uint32_t> address, std::vector <uint32_t> size) {
    ErrorCodes_t ret;
    if (calibrationEeprom == nullptr) {
        ret = ErrorEepromNotConnected;
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

    RxOutput_t rxOutput;
    ret = ErrorUnknown;
    rxOutput.msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;
    int pingTries = 0;

    while (ret != Success) {
        if (pingTries++ > EZP_MAX_PING_TRIES) {
            return ErrorConnectionPingFailed;
        }

        ret = this->ping();
    }

    /*! Make a chip reset to force resynchronization of chip states. This is important when the FPGA has just been reset */
    this->resetAsic(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    this->resetAsic(false, true);

    return ret;
}

int32_t EZPatchFtdiDevice::getDeviceIndex(std::string serial) {
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

std::string EZPatchFtdiDevice::getDeviceSerial(uint32_t index, bool excludeLetter) {
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

bool EZPatchFtdiDevice::getDeviceCount(DWORD &numDevs) {
    /*! Get the number of connected devices */
    numDevs = 0;
    FT_STATUS FT_Result = FT_ListDevices(&numDevs, nullptr, FT_LIST_NUMBER_ONLY);
    if (FT_Result == FT_OK) {
        return true;

    } else {
        return false;
    }
}

ErrorCodes_t EZPatchFtdiDevice::startCommunication(std::string) {
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

ErrorCodes_t EZPatchFtdiDevice::stopCommunication() {
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

    if (ftdiRxHandle != nullptr) {
        delete ftdiRxHandle;
        ftdiRxHandle = nullptr;
        if (txChannel == rxChannel) {
            ftdiTxHandle = nullptr;
        }
    }

    if (ftdiTxHandle != nullptr) {
        delete ftdiTxHandle;
        ftdiTxHandle = nullptr;
    }

    return Success;
}

void EZPatchFtdiDevice::initializeCalibration() {
    calibrationEeprom = new CalibrationEeprom(this->getDeviceIndex(deviceId+spiChannel));
}

void EZPatchFtdiDevice::deinitializeCalibration() {
    if (calibrationEeprom != nullptr) {
        delete calibrationEeprom;
        calibrationEeprom = nullptr;
    }
}

void EZPatchFtdiDevice::readAndParseMessages() {
    FT_STATUS ftRet;
    DWORD ftdiQueuedBytes = 0;
    DWORD ftdiReadBytes;
    uint32_t readTries = 0;
    uint32_t minQueuedBytes = FTD_FRAME_SIZE+rxDataMessageMaxLen*sizeof(uint16_t);
    std::chrono::milliseconds longBytesWait(2);
    std::chrono::milliseconds shortBytesWait(1);

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
    uint16_t rxMsgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid;
    bool rxExpectAck = false;

    /*! Rx data length variables */
    uint16_t rxDataWords = 0;
    uint16_t rxDataBytes = 0;

    /*! Rx message crc variables */
    uint16_t rxReadCrc0 = 0x0000;
    uint16_t rxReadCrc1;
    uint16_t rxComputedCrc = 0x0000;
    bool rxCrcOk;

    parsingFlag = true;

    std::unique_lock <std::mutex> connectionMutexLock (connectionMutex);
    connectionMutexLock.unlock();

    while ((!stopConnectionFlag) || (txWaitingOnAcks > 0)) {

        /******************\
         *  Reading part  *
        \******************/

        /*! Read queue status to check the number of available bytes */
        ftRet = FT_GetQueueStatus(* ftdiRxHandle, &ftdiQueuedBytes);
        connectionMutexLock.lock();
        if (ftRet != FT_OK) {
            connectionMutexLock.unlock();
            continue;
        }

        if (ftdiQueuedBytes == 0) {
            connectionMutexLock.unlock();
            /*! If there are no bytes in the queue skip to next iteration in the while loop */
            std::this_thread::sleep_for (longBytesWait);
            continue;

        } else if ((ftdiQueuedBytes < minQueuedBytes) && (readTries == 0)) {
            connectionMutexLock.unlock();
            readTries++;
            std::this_thread::sleep_for (shortBytesWait);
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
            continue;
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

                    } else {
                        rxParsePhase = RxParseLookForHeader;
#ifdef DEBUG_RX_DATA_PRINT
                        fprintf(rxFid,
                                "crc0 wrong\n"
                                "hb: \t0x%04x\n\n",
                                rxHeartbeat);
                        fflush(rxFid);
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
                        if (rxMsgTypeId == MsgDirectionDeviceToPc+MsgTypeIdAck) {
                            txAckMutex.lock();
                            txAckReceived = true;
                            txAckCv.notify_all();
#ifdef DEBUG_RX_DATA_PRINT
                            fprintf(rxFid,
                                    "ack recd\n"
                                    "hb: \t0x%04x\n\n",
                                    * ((uint16_t *)(rxRawBuffer+rxRawBufferReadOffset)));
                            fflush(rxFid);
#endif
                            txAckMutex.unlock();

                        } else if (rxMsgTypeId == MsgDirectionDeviceToPc+MsgTypeIdNack) {
                            /*! \todo FCON NACK should not be written but used to manage tx, maybe forcing rewriting? hard to implement */
#ifdef DEBUG_RX_DATA_PRINT
                            fprintf(rxFid,
                                    "nack recd\n"
                                    "hb: \t0x%04x\n\n",
                                    * ((uint16_t *)(rxRawBuffer+rxRawBufferReadOffset)));
                            fflush(rxFid);
#endif

                        } else if (rxMsgTypeId == MsgDirectionDeviceToPc+MsgTypeIdPing) {
                            this->ack(rxHeartbeat);

                        } else {
                            std::unique_lock <std::mutex> rxMutexLock (rxMutex);
                            while (rxMsgBufferReadLength >= EZP_RX_MSG_BUFFER_SIZE) { /*! \todo FCON bloccare la ricezione potrebbe essere controproducente */
                                txMsgBufferNotFull.wait(rxMutexLock);
                            }
                            rxMutexLock.unlock();

                            rxMsgBuffer[rxMsgBufferWriteOffset].heartbeat = rxHeartbeat;
                            rxMsgBuffer[rxMsgBufferWriteOffset].typeId = rxMsgTypeId;
                            rxMsgBuffer[rxMsgBufferWriteOffset].startDataPtr = rxDataBufferWriteOffset;

#ifdef DEBUG_RX_DATA_PRINT
                            if (rxEnabledTypesMap[rxMsgTypeId]) {
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
                            }
#endif

                            if (rxMsgTypeId == MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData) {
                                /*! In MsgTypeIdAcquisitionData the last word of the payload contains the number of valid data samples */
                                if (rxDataWords < EZP_RX_MIN_DATA_PACKET_LEN) {
                                    rxParsePhase = RxParseLookForHeader;
#ifdef DEBUG_RX_DATA_PRINT
                                    fprintf(rxFid,
                                            "short data packet\n"
                                            "words: \t%d\n\n",
                                            rxDataWords);
                                    fflush(rxFid);
#endif
                                    continue;

                                } else {

                                    rxDataWords = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*(rxDataWords-1))&FTD_RX_RAW_BUFFER_MASK)));

#ifdef DEBUG_RX_DATA_PRINT
                                    fprintf(rxFid, "vlen:\t0x%04x\n",
                                            rxDataWords);
#endif
                                }
                            }

                            if (rxEnabledTypesMap[rxMsgTypeId]) {
                                /*! Update the message buffer only if the message is not filtered out */
                                rxMsgBuffer[rxMsgBufferWriteOffset].dataLength = rxDataWords;
                                rxMsgBufferWriteOffset = (rxMsgBufferWriteOffset+1)&EZP_RX_MSG_BUFFER_MASK;
                            }

                            for (rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
                                rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx)&EZP_RX_DATA_BUFFER_MASK] = * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*rxDataBufferWriteIdx)&FTD_RX_RAW_BUFFER_MASK)));
#ifdef DEBUG_RX_DATA_PRINT
                                if (rxEnabledTypesMap[rxMsgTypeId]) {
                                    fprintf(rxFid, "data%d:\t0x%04x\n",
                                            rxDataBufferWriteIdx,
                                            rxDataBuffer[(rxDataBufferWriteOffset+rxDataBufferWriteIdx)&EZP_RX_DATA_BUFFER_MASK]);
                                }
#endif
                            }

#ifdef DEBUG_RX_DATA_PRINT
                            if (rxEnabledTypesMap[rxMsgTypeId]) {
                                fprintf(rxFid, "crc1:\t0x%04x\n\n",
                                        rxReadCrc1);
                                fflush(rxFid);
                            }
#endif

                            rxDataBufferWriteOffset = (rxDataBufferWriteOffset+rxDataWords)&EZP_RX_DATA_BUFFER_MASK;

                            if (rxDataBufferWriteOffset <= (uint32_t)rxDataWords) {
                                rxDataBuffer[EZP_RX_DATA_BUFFER_SIZE] = rxDataBuffer[0]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive 16bit words at a time to form a 32bit word,
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

#ifdef DEBUG_RX_DATA_PRINT
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
                            fprintf(rxFid, "data%d:\t0x%04x\n",
                                    rxDataBufferWriteIdx,
                                    * ((uint16_t *)(rxRawBuffer+((rxRawBufferReadOffset+2*rxDataBufferWriteIdx)&FTD_RX_RAW_BUFFER_MASK))));
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
#ifdef DEBUG_RX_DATA_PRINT
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

    if (rxMsgBufferReadLength == 0) {
        std::unique_lock <std::mutex> rxMutexLock (rxMutex);
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
    }
}

void EZPatchFtdiDevice::unwrapAndSendMessages() {
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

    std::unique_lock <std::mutex> txMutexLock (txMutex);
    txMutexLock.unlock();
    std::unique_lock <std::mutex> connectionMutexLock (connectionMutex);
    connectionMutexLock.unlock();

    while ((!stopConnectionFlag) || (txWaitingOnAcks > 0)) {

        /*********************\
         *  Unwrapping part  *
        \*********************/

        txMutexLock.lock();
        while (txMsgBufferReadLength <= 0) {
            txMsgBufferNotEmpty.wait_for (txMutexLock, std::chrono::milliseconds(100));
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

        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txMsgBuffer[txMsgBufferReadOffset].typeId;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;

        txDataBufferReadOffset = txMsgBuffer[txMsgBufferReadOffset].startDataPtr;

        txDataWords = txMsgBuffer[txMsgBufferReadOffset].dataLength;
        txDataBytes = txDataWords*2;
        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txDataWords;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;

        txComputedCrc = txCrc16Ccitt(FTD_TX_SYNC_WORD_SIZE, FTD_TX_HB_TY_LN_SIZE, txCrcInitialValue);
        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txComputedCrc;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;

#ifdef DEBUG_TX_DATA_PRINT
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
            * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txDataBuffer[(txDataBufferReadOffset+txDataBufferReadIdx)&EZP_TX_DATA_BUFFER_MASK];
            txRawBufferReadIdx += FTD_TX_WORD_SIZE;

#ifdef DEBUG_TX_DATA_PRINT
            fprintf(txFid,
                    "data%d:\t0x%02x%02x\n",
                    txDataBufferReadIdx,
                    txRawBuffer[txRawBufferReadIdx-1], txRawBuffer[txRawBufferReadIdx-2]);
#endif
        }

        txComputedCrc = txCrc16Ccitt(FTD_TX_SYNC_WORD_SIZE+FTD_TX_HB_TY_LN_SIZE+FTD_TX_CRC_WORD_SIZE, txDataBytes, txComputedCrc);
        * ((uint16_t *)(txRawBuffer+txRawBufferReadIdx)) = txComputedCrc;
        txRawBufferReadIdx += FTD_TX_WORD_SIZE;

#ifdef DEBUG_TX_DATA_PRINT
        fprintf(txFid,
                "crc1:\t0x%02x%02x\n\n",
                txRawBuffer[txRawBufferReadIdx-1], txRawBuffer[txRawBufferReadIdx-2]);
        fflush(txFid);
#endif

        txMsgBufferReadOffset = (txMsgBufferReadOffset+1)&EZP_TX_MSG_BUFFER_MASK;

        /******************\
         *  Sending part  *
        \******************/

        notSentTxData = true;
        bytesToWrite = ((DWORD)txDataBytes)+FTD_BYTES_TO_WRITE_ALWAYS;
        while (notSentTxData && (writeTries++ < EZP_MAX_WRITE_TRIES)) { /*! \todo FCON prevedere un modo per notificare ad alto livello e all'utente */
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
        if (liquidJunctionControlPending && txMsgBufferReadLength == 0) {
            /*! \todo FCON let the liquid junction procedure know that all commands have been submitted, can be optimized by checking that there are no liquid junction commands pending */
            liquidJunctionControlPending = false;
        }
        txMsgBufferNotFull.notify_all();
        txMutexLock.unlock();
    }
}

void EZPatchFtdiDevice::wrapOutgoingMessage(uint16_t msgTypeId, std::vector <uint16_t> &txDataMessage, uint16_t dataLen) {
    std::unique_lock <std::mutex> txMutexLock (txMutex);
    while (txMsgBufferReadLength >= EZP_TX_MSG_BUFFER_SIZE) {
        txMsgBufferNotFull.wait(txMutexLock);
    }

    txMsgBuffer[txMsgBufferWriteOffset].heartbeat = txHeartbeat;
    txHeartbeat = (txHeartbeat+1)&FTD_TX_HEARTBEAT_MASK;

    txMsgBuffer[txMsgBufferWriteOffset].typeId = msgTypeId;

    txMsgBuffer[txMsgBufferWriteOffset].startDataPtr = txDataBufferWriteOffset;

    txMsgBuffer[txMsgBufferWriteOffset].dataLength = dataLen;

    for (uint16_t idx = 0; idx < dataLen; idx++) {
        txDataBuffer[txDataBufferWriteOffset] = txDataMessage[idx];
        txDataBufferWriteOffset = (txDataBufferWriteOffset+1)&EZP_TX_DATA_BUFFER_MASK;
    }

    if (txDataBufferWriteOffset <= (uint32_t)dataLen) {
        txDataBuffer[EZP_TX_DATA_BUFFER_SIZE] = txDataBuffer[0]; /*!< The last item is a copy of the first one, it used to safely read 2 consecutive 16bit words at a time to form a 32bit word,
                                                                  *   even if the first 16bit word is in position FTD_RX_DATA_BUFFER_SIZE-1 and the following one would go out of range otherwise */
    }

    txMsgBufferWriteOffset = (txMsgBufferWriteOffset+1)&EZP_TX_MSG_BUFFER_MASK;
    txMsgBufferReadLength++;
    txMsgBufferNotEmpty.notify_all();
}

uint16_t EZPatchFtdiDevice::rxCrc16Ccitt(uint32_t offset, uint16_t len, uint16_t crc) {
    for (uint32_t idx = 0; idx < len; idx++) {
        uint16_t tmp = (crc >> 8) ^ (uint16_t)rxRawBuffer[(offset++)&FTD_RX_RAW_BUFFER_MASK];
        crc = ((uint16_t)(crc << 8)) ^ crc16CcittTable[tmp];
    }
    return crc;
}

uint16_t EZPatchFtdiDevice::txCrc16Ccitt(uint32_t offset, uint16_t len, uint16_t crc) {
    for (uint32_t idx = 0; idx < len; idx++) {
        uint16_t tmp = (crc >> 8) ^ (uint16_t)txRawBuffer[offset++];
        crc = ((uint16_t)(crc << 8)) ^ crc16CcittTable[tmp];
    }
    return crc;
}

ErrorCodes_t EZPatchFtdiDevice::initializeMemory() {
    rxRawBuffer = new uint8_t[FTD_RX_RAW_BUFFER_SIZE+1]; /*!< The last item is a copy of the first one, it is used to safely read 2 consecutive bytes at a time to form a 16bit word */
    if (rxRawBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    txRawBuffer = new uint8_t[FTD_TX_RAW_BUFFER_SIZE];
    if (rxRawBuffer == nullptr) {
        this->deinitializeMemory();
        return ErrorMemoryInitialization;
    }

    /*! This will never change so it makes sense to initialize it here */
    for (uint32_t idx = 0; idx < FTD_TX_SYNC_WORD_SIZE; idx++) {
        txRawBuffer[idx] = txSyncWord[idx];
    }
    return EZPatchDevice::initializeMemory();
}

void EZPatchFtdiDevice::deinitializeMemory() {
    if (rxRawBuffer != nullptr) {
        delete [] rxRawBuffer;
        rxRawBuffer = nullptr;
    }

    if (txRawBuffer != nullptr) {
        delete [] txRawBuffer;
        txRawBuffer = nullptr;
    }

    EZPatchDevice::deinitializeMemory();
}

ErrorCodes_t EZPatchFtdiDevice::loadFpgaFw() {
    switch (fpgaLoadType) {
    case FpgaFwLoadAutomatic:
        /*! Nothing to be done, the FPGA will handle itself */
        break;

    case FpgaFwLoadPatchlinerArtix7_V01:
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

        std::this_thread::sleep_for (std::chrono::seconds(35));

        SPI_CloseChannel(spiHandle);
        Cleanup_libMPSSE();

        break;
    }
    return Success;
}

ErrorCodes_t EZPatchFtdiDevice::initFtdiChannel(FT_HANDLE * handle, char channel) {
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
