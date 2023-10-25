#ifndef EMCROPALKELLYDEVICE_H
#define EMCROPALKELLYDEVICE_H

#define OKY_RX_PIPE_ADDR 0xA0
#define OKY_RX_BUFFER_SIZE 0x1000000 /*!< Number of bytes. Always use a power of 2 for efficient circular buffer management through index masking */
#define OKY_RX_BUFFER_MASK (OKY_RX_BUFFER_SIZE-1)
#define OKY_RX_TRANSFER_SIZE 0x100000 /*! 1MB. Must match value in FPGA FW */
#define OKY_RX_BLOCK_SIZE 0x4000 /*! 16kB. Must match value in FPGA FW */
#define OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR 0x53
#define OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT 0
#define OKY_START_PROTOCOL_TRIGGER_IN_ADDR 0x53
#define OKY_START_PROTOCOL_TRIGGER_IN_BIT 1
#define OKY_START_STATE_ARRAY_TRIGGER_IN_ADDR 0x53
#define OKY_START_STATE_ARRAY_TRIGGER_IN_BIT 3
#define OKY_MOTHERBOARD_FPGA_BYTES_PER_S 333333

#include "okFrontPanelDLL.h"

#include "messagedispatcher.h"

class EmcrOpalKellyDevice : public MessageDispatcher {
public:
    EmcrOpalKellyDevice(std::string deviceId);
    virtual ~EmcrOpalKellyDevice();

    virtual ErrorCodes_t connect(std::string fwPath) override;
    virtual ErrorCodes_t disconnect() override;

protected:
    typedef enum {
        RxParseLookForHeader,
        RxParseLookForLength,
        RxParseCheckNextHeader
    } RxParsePhase_t;

    /*************\
     *  Methods  *
    \*************/

    virtual void handleCommunicationWithDevice() override;
    virtual void sendCommandsToDevice() override;
    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type);
    virtual uint32_t readDataFromDevice() override;
    virtual void parseDataFromDevice() override;

    ErrorCodes_t initializeBuffers();
    ErrorCodes_t deinitializeBuffers();

    /****************\
     *  Parameters  *
    \****************/

    okCFrontPanel dev;

    std::string fwName = "top.bit";

    /***************\
     *  Variables  *
    \***************/

    int waitingTimeBeforeReadingData = 1;
    int motherboardBootTime_s = 1;
    int fwSize_B = 1000;

    okTRegisterEntries regs;

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */
};

#endif // EMCROPALKELLYDEVICE_H
