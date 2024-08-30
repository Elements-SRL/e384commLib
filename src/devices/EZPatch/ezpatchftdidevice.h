#ifndef EZPATCHFTDIDEVICE_H
#define EZPATCHFTDIDEVICE_H

#include "ezpatchdevice.h"
#include "utils.h"
#include "ftdiutils.h"

class EZPatchFtdiDevice : public EZPatchDevice {
public:
    EZPatchFtdiDevice(std::string deviceId);
    virtual ~EZPatchFtdiDevice();

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion);
    ErrorCodes_t getDeviceInfo(std::string &deviceId, std::string &deviceName, uint32_t &deviceVersion, uint32_t &deviceSubversion, uint32_t &firmwareVersion);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t isDeviceRecognized(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = UTL_DEFAULT_FW_PATH);
    virtual ErrorCodes_t pauseConnection(bool pauseFlag);
    ErrorCodes_t disconnectDevice() override;
    ErrorCodes_t getCalibrationEepromSize(uint32_t &size) override;
    ErrorCodes_t writeCalibrationEeprom(std::vector <uint32_t> value, std::vector <uint32_t> address, std::vector <uint32_t> size) override;
    ErrorCodes_t readCalibrationEeprom(std::vector <uint32_t> &value, std::vector <uint32_t> address, std::vector <uint32_t> size) override;

protected:
    typedef enum {
        RxParseLookForHeader,
        RxParseLookForLength,
        RxParseLookForCrc
    } RxParsePhase_t;

    /*************\
     *  Methods  *
    \*************/

    static int32_t getDeviceIndex(std::string serial);
    static std::string getDeviceSerial(uint32_t index, bool excludeLetter);
    static bool getDeviceCount(DWORD &numDevs);

    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual ErrorCodes_t stopCommunication() override;
    virtual void initializeCalibration() override;
    virtual void deinitializeCalibration() override;

    virtual void readAndParseMessages() override;
    virtual void unwrapAndSendMessages() override;
    void wrapOutgoingMessage(uint16_t msgTypeId, std::vector <uint16_t> &txDataMessage, uint16_t dataLen) override;

    uint16_t rxCrc16Ccitt(uint32_t offset, uint16_t len, uint16_t crc);
    uint16_t txCrc16Ccitt(uint32_t offset, uint16_t len, uint16_t crc);

    virtual ErrorCodes_t initializeMemory() override;
    virtual void deinitializeMemory() override;

    ErrorCodes_t loadFpgaFw();
    ErrorCodes_t initFtdiChannel(FT_HANDLE * handle, char channel);

    /****************\
     *  Parameters  *
    \****************/

    FtdiFpgaLoadType_t fpgaLoadType = FtdiFpgaFwLoadAutomatic;

    FtdiEepromId_t ftdiEepromId = FtdiEepromId56;
    CalibrationEeprom * calibrationEeprom = nullptr;

    FT_HANDLE * ftdiRxHandle = nullptr;
    FT_HANDLE * ftdiTxHandle = nullptr;

    char spiChannel = 'A';
    char rxChannel = 'B';
    char txChannel = 'B';

    uint8_t rxSyncWord[FTD_RX_SYNC_WORD_SIZE] = {0xA5, 0x5A};

    uint16_t rxCrcInitialValue = 0xFFFF;

    uint8_t txSyncWord[FTD_TX_SYNC_WORD_SIZE] = {0xA5, 0x5A};

    uint16_t txCrcInitialValue = 0xFFFF;
};

#endif // EZPATCHFTDIDEVICE_H
