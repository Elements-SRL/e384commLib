#ifndef EMCRFTDIDEVICE_H
#define EMCRFTDIDEVICE_H

#define EMF_MIN_QUEUED_BYTES 8 /*! hdr (4) + offset (2) + length (2), assuming no payload */
#define EMF_START_PROTOCOL_TRIGGER_IN_ADDR 0x40
#define EMF_START_PROTOCOL_TRIGGER_IN_BIT 0
#define EMF_ZAP_PULSE_TRIGGER_IN_ADDR 0x40
#define EMF_ZAP_PULSE_TRIGGER_IN_BIT 1
#define EMF_TX_TRIGGER_BUFFER_SIZE 5 // header, type, length = 1, payload[2]
#define EMF_MAX_WRITE_TRIES 3

#include "emcrdevice.h"
#include "calibrationeeprom.h"
#include "ftdiutils.h"

class EmcrFtdiDevice : public EmcrDevice {
public:
    EmcrFtdiDevice(std::string deviceId);
    virtual ~EmcrFtdiDevice();

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion);
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
        RxParseCheckNextHeader
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

    virtual void handleCommunicationWithDevice() override;
    void sendCommandsToDevice();
    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type);
    virtual uint32_t readDataFromDevice() override;
    virtual void parseDataFromDevice() override;

    virtual ErrorCodes_t initializeMemory() override;
    virtual void deinitializeMemory() override;

    ErrorCodes_t loadFpgaFw();
    ErrorCodes_t initFtdiChannel(FT_HANDLE * handle, char channel);

    /****************\
     *  Parameters  *
    \****************/

    FtdiEepromId_t ftdiEepromId = FtdiEepromId56;
    CalibrationEeprom * calibrationEeprom = nullptr;

    FT_HANDLE * ftdiRxHandle = nullptr;
    FT_HANDLE * ftdiTxHandle = nullptr;

    char spiChannel = 'A';
    char rxChannel = 'B';
    char txChannel = 'B';

    uint32_t rxSyncWord;

    /***************\
     *  Variables  *
    \***************/

    FtdiFpgaLoadType_t fpgaLoadType = FtdiFpgaFwLoadAutomatic;

    uint32_t * txRawBulkBuffer = nullptr;
    uint32_t * txRawTriggerBuffer = nullptr;

    bool fwLoadedFlag = true; /*! \todo FCON verificare se va messa a zero e settata dopo o se si può solo togliere, così è inutile */
    uint32_t readTries = 0;

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */

private:
    typedef struct OutputPacket {
        uint32_t header = 0x5aa55aa5;
        uint32_t type = 0;
        uint32_t valuesNum = 0;
        std::vector <uint32_t> addresses;
        std::vector <uint32_t> values;
    } OutputPacket_t;

    /*************\
     *  Methods  *
    \*************/

    bool writeRegisters();
    bool activateTriggerIn(int address, int bit);
    bool writeToBulkOut(uint32_t * buffer);
};

#endif // EMCRFTDIDEVICE_H
