#ifndef EZPATCHFTDIDEVICE_H
#define EZPATCHFTDIDEVICE_H

#define FTD_RX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define FTD_RX_SYNC_WORD_SIZE (FTD_RX_WORD_SIZE) // could be different size
#define FTD_RX_CRC_WORD_SIZE (FTD_RX_WORD_SIZE) // could be different size but requires several modifications, e.g. data type and algo
#define FTD_RX_HB_TY_LN_SIZE (FTD_RX_WORD_SIZE*3) // size of heartbeat + type ID + length, computed here just once for efficience
#define FTD_RX_RAW_BUFFER_SIZE 0x100000 /*! \todo FCON valutare che questo numero sia adeguato */ // ~1M
#define FTD_RX_RAW_BUFFER_MASK (FTD_RX_RAW_BUFFER_SIZE-1) // 0b11...1 for all bits of the buffer indexes
#define FTD_RX_HEARTBEAT_MASK 0xFFFF // mask for 16 bit word

#define FTD_TX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define FTD_TX_SYNC_WORD_SIZE (FTD_TX_WORD_SIZE) // could be different size
#define FTD_TX_CRC_WORD_SIZE (FTD_TX_WORD_SIZE) // could be different size but requires several modifications, e.g. data type and algo
#define FTD_TX_HB_TY_LN_SIZE (FTD_TX_WORD_SIZE*3) // size of heartbeat + type ID + length, computed here just once for efficience
#define FTD_TX_RAW_BUFFER_SIZE 0x1000 /*! \todo FCON valutare che questo numero sia adeguato */ // ~4k
#define FTD_TX_RAW_BUFFER_MASK (FTD_TX_RAW_BUFFER_SIZE-1) // 0b11...1 for all bits of the buffer indexes
#define FTD_TX_HEARTBEAT_MASK 0xFFFF // mask for 16 bit word
#define FTD_BYTES_TO_WRITE_ALWAYS (FTD_TX_SYNC_WORD_SIZE+FTD_TX_HB_TY_LN_SIZE+FTD_TX_CRC_WORD_SIZE*2)

#define FTD_FRAME_SIZE (FTD_RX_SYNC_WORD_SIZE+FTD_RX_HB_TY_LN_SIZE+2*FTD_RX_CRC_WORD_SIZE) // could be different size

#include "ezpatchdevice.h"
#include "utils.h"

class EZPatchFtdiDevice : public EZPatchDevice {
public:
    EZPatchFtdiDevice(std::string deviceId);
    virtual ~EZPatchFtdiDevice();

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = UTL_DEFAULT_FW_PATH);
    ErrorCodes_t disconnectDevice() override;

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

    FtdiEepromId_t ftdiEepromId = FtdiEepromId56;

    char spiChannel;
    char rxChannel;
    char txChannel;

    uint8_t rxSyncWord[FTD_RX_SYNC_WORD_SIZE];

    uint16_t rxCrcInitialValue = 0xFFFF;

    uint8_t txSyncWord[FTD_TX_SYNC_WORD_SIZE];

    uint16_t txCrcInitialValue = 0xFFFF;
};

#endif // EZPATCHFTDIDEVICE_H
