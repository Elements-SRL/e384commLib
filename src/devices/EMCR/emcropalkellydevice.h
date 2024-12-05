#ifndef EMCROPALKELLYDEVICE_H
#define EMCROPALKELLYDEVICE_H

#define OKY_RX_PIPE_ADDR 0xA0
#define OKY_RX_BUFFER_SIZE 0x1000000 /*!< Number of bytes. Always use a power of 2 for efficient circular buffer management through index masking */
#define OKY_RX_BUFFER_MASK (OKY_RX_BUFFER_SIZE-1)
#define OKY_RX_TRANSFER_SIZE 0x100000 /*! 1MB. Default value */
#define OKY_RX_BLOCK_SIZE 0x4000 /*! 16kB. Must match value in FPGA FW */
#define OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR 0x53
#define OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT 0
#define OKY_START_PROTOCOL_TRIGGER_IN_ADDR 0x53
#define OKY_START_PROTOCOL_TRIGGER_IN_BIT 1
#define OKY_START_STATE_ARRAY_TRIGGER_IN_ADDR 0x53
#define OKY_START_STATE_ARRAY_TRIGGER_IN_BIT 3
#define OKY_ZAP_PULSE_TRIGGER_IN_ADDR 0x53
#define OKY_ZAP_PULSE_TRIGGER_IN_BIT 4
#define OKY_MOTHERBOARD_FPGA_BYTES_PER_S 333333

#include "okFrontPanelDLL.h"

#include "emcrdevice.h"
#include "utils.h"

class EmcrOpalKellyDevice : public EmcrDevice {
public:
    EmcrOpalKellyDevice(std::string deviceId);
    virtual ~EmcrOpalKellyDevice();

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t isDeviceRecognized(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = UTL_DEFAULT_FW_PATH);
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
    virtual ErrorCodes_t stopCommunication() override;

    virtual void handleCommunicationWithDevice() override;
    void sendCommandsToDevice();
    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type);
    virtual uint32_t readDataFromDevice() override;
    virtual void parseDataFromDevice() override;

    virtual ErrorCodes_t initializeMemory() override;
    virtual void deinitializeMemory() override;

    /****************\
     *  Parameters  *
    \****************/

    uint16_t rxSyncWord;
    okCFrontPanel dev;

    std::string fwName = "top.bit";

    /***************\
     *  Variables  *
    \***************/

    int waitingTimeBeforeReadingData = 1;

    okTRegisterEntries regs;

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */

    long okTransferSize = OKY_RX_TRANSFER_SIZE;
};

#endif // EMCROPALKELLYDEVICE_H
