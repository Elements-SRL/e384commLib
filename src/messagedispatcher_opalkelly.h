#ifndef MESSAGEDISPATCHER_OPALKELLY_H
#define MESSAGEDISPATCHER_OPALKELLY_H

#define OKY_RX_PIPE_ADDR 0xA0
#define OKY_RX_BUFFER_SIZE 0x400000 /*!< Number of bytes. Always use a power of 2 for efficient circular buffer management through index masking */
#define OKY_RX_BUFFER_MASK (OKY_RX_BUFFER_SIZE-1)
#define OKY_RX_TRANSFER_SIZE 0x100000 /*! 1MB. Must match value in FPGA FW */
#define OKY_RX_BLOCK_SIZE 0x4000 /*! 16kB. Must match value in FPGA FW */

#include "okFrontPanelDLL.h"

#include "messagedispatcher.h"

class MessageDispatcher_OpalKelly : public MessageDispatcher {
public:
    MessageDispatcher_OpalKelly(std::string deviceId);
    virtual ~MessageDispatcher_OpalKelly();

    virtual ErrorCodes_t connect() override;
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

    virtual void sendCommandsToDevice() override;
    virtual void readDataFromDevice() override;

    void storeDataLoadFrame();
    void storeDataHeaderFrame();
    void storeDataTailFrame();
    void storeStatusFrame();
    void storeVoltageOffsetFrame();

    ErrorCodes_t initializeBuffers();
    ErrorCodes_t deinitializeBuffers();

    /****************\
     *  Parameters  *
    \****************/

    okCFrontPanel * dev;

    std::string fwName = "top.bit";

    /***************\
     *  Variables  *
    \***************/

    uint8_t * rxTransferBuffer = nullptr; /*!< Raw data from the device obtained during last transfer */
    uint8_t * rxRawBuffer = nullptr; /*!< Raw incoming data from the device */
    uint32_t rxRawBufferReadOffset = 0; /*!< Device Rx buffer offset position in which data are collected by the outputDataBuffer */
    uint32_t rxRawBufferWriteOffset = 0; /*!< Device Rx buffer offset position in which data are written by FTDI device */

//    uint8_t * txRawBuffer; /*!< Raw outgoing data to the device */

    /********************************************\
     *  Multi-thread synchronization variables  *
    \********************************************/

    std::mutex deviceMutex;
};

#endif // MESSAGEDISPATCHER_OPALKELLY_H
