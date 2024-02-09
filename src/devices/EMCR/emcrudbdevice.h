#ifndef EMCRUDBDEVICE_H
#define EMCRUDBDEVICE_H

#define UDB_RX_BUFFER_SIZE 0x1000000 /*!< Number of bytes. Always use a power of 2 for efficient circular buffer management through index masking */
#define UDB_RX_BUFFER_MASK (UDB_RX_BUFFER_SIZE-1)
#define UDB_RX_TRANSFER_SIZE 0x100000  /*! 1MB, must be lower than 4MB */
//#define UDB_RX_BLOCK_SIZE 0x4000 /*! 16kB. Must match value in FPGA FW */
#define UDB_BULKIN_ENDPOINT_TIMEOUT ((int)2000)
#define UDB_BULKOUT_ENDPOINT_TIMEOUT ((int)2000)
#define UDB_PACKETS_PER_TRANSFER ((int)64)

#include "stdafx.h"
#ifdef _WIN32
#include <windows.h> /*! This has to be included before CyAPI.h, otherwise it won't be correctly interpreted */
#endif
#include "CyAPI.h"

#include "emcrdevice.h"

typedef enum {
    DeviceVersion10MHz = 0x0B,
    DeviceVersionUndefined = 0xFF
} DeviceVersion_t;

typedef enum {
    /*! Subversions used for version = 0x0B */
    DeviceSubversionUDB_PCBV02 = 0x01,
    DeviceSubversionUDB_PCBV03 = 0x02,

    /*! Subversions used for Version = 0xFF */
    DeviceSubversionUndefined = 0xFF
} DeviceSubversion_t ;

class EmcrUdbDevice : public EmcrDevice {
public:
    EmcrUdbDevice(std::string deviceId);
    virtual ~EmcrUdbDevice();

    typedef struct DeviceTuple {
        DeviceVersion_t version = DeviceVersionUndefined;
        DeviceSubversion_t subversion = DeviceSubversionUndefined;
        uint32_t fwVersion = 0;
    } DeviceTuple_t;

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = "");
    ErrorCodes_t disconnectDevice() override;

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
    static std::string getDeviceSerial(uint32_t index);
    static bool getDeviceCount(int &numDevs);

    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual ErrorCodes_t initializeHW() override;
    virtual ErrorCodes_t stopCommunication() override;

    virtual void handleCommunicationWithDevice() override;
    void sendCommandsToDevice();
    virtual bool writeRegisters();
    virtual uint32_t readDataFromDevice() override;
    virtual void parseDataFromDevice() override;

    virtual ErrorCodes_t initializeMemory() override;
    virtual void deinitializeMemory() override;

    /****************\
     *  Parameters  *
    \****************/

    CCyUSBDevice * dev = nullptr;

    /*! endpoints */
    CCyUSBEndPoint * eptBulkin;
    CCyUSBEndPoint * eptBulkout;

    /*! Rx consts */
    uint32_t readDataTransferSize = UDB_RX_TRANSFER_SIZE;

    mutable std::mutex deviceMtx;

    /***************\
     *  Variables  *
    \***************/

    int waitingTimeBeforeReadingData = 1;

//    okTRegisterEntries regs;

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */

private:
    enum fpgaLoadingStatus {
        fpgaLoadingInProgress = 0,
        fpgaLoadingSuccess = 1,
        fpgaLoadingError = 2
    };

    enum fwStatus {
        fwStatusConfigMode = 0,
        fwStatusSlaveFifo = 1,
        fwStatusError = 2
    };

    /*************\
     *  Methods  *
    \*************/

    static DeviceTuple_t getDeviceTuple(uint32_t deviceIdx);
    void findBulkEndpoints();
    bool resetBulkEndpoints();
    void initEndpoints();
    bool bootFpgafromFLASH();
    unsigned char fpgaLoadBitstreamStatus();
    unsigned char getFwStatus();

    /***************\
     *  Variables  *
    \***************/

    DeviceTuple_t deviceTuple;
};

#endif // EMCRUDBDEVICE_H
