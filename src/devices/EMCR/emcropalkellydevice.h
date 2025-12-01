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
#define OKY_SINGLE_CHANNEL_RAMP_TRIGGER_IN_ADDR 0x53
#define OKY_SINGLE_CHANNEL_RAMP_TRIGGER_IN_BIT 5
#define OKY_MOTHERBOARD_FPGA_BYTES_PER_S 333333

#include "okFrontPanelDLL.h"

#include "emcrdevice.h"
#include "utils.h"

class EmcrOpalKellyDevice : public EmcrDevice {
public:
    EmcrOpalKellyDevice(std::string deviceId);
    virtual ~EmcrOpalKellyDevice();

    typedef enum {
        DeviceVersionTestBoard = 0x06,
        DeviceVersion10MHz = 0x0B,
        DeviceVersion192Blm = 0x0D,
        DeviceVersion384Patch = 0x0F,
        DeviceVersionPrototype = 0xFE,
        DeviceVersionUndefined = 0xFF
    } DeviceVersion_t;

    typedef enum {
        /*! Subversions used for version = 0x06 */
        DeviceSubversionTestBoardQC01a = 0x0D,
        DeviceSubversionTestBoardQC01aExtVcm = 0x0E,
        DeviceSubversionTestBoardEL07a = 0x11,
        DeviceSubversionTestBoardEL07b = 0x12,
        DeviceSubversionTestBoardEL07c = 0x13,
        DeviceSubversionTestBoardEL07d = 0x14,
        DeviceSubversionTestBoardEL07e = 0x15,

        /*! Subversions used for version = 0x0B */
        DeviceSubversion10MHz_SB_EL05a = 0x03,
        DeviceSubversion4x10MHz_SB_EL05a_PCBV01 = 0x09,
        DeviceSubversion4x10MHz_SB_EL05b_PCBV01 = 0x0A,
        DeviceSubversion4x10MHz_SB_EL05a_PCBV02 = 0x0B,
        DeviceSubversion2x10MHz_SB_EL05a_PCBV02_FEStim = 0x0C,
        DeviceSubversion24x10MHz_PCBV01_EL05c34 = 0x0D,
        DeviceSubversion24x10MHz_PCBV01_EL05c12 = 0x0E,
        DeviceSubversionOk_FAKE = 0xFE,

        /*! Subversions used for version = 0x0D */
        DeviceSubversion192Blm_EL03c_MB02Mez03 = 0x01,
        DeviceSubversion192Blm_EL03c_MB03Mez04 = 0x02,

        /*! Subversions used for version = 0x0F */
        DeviceSubversion384Patch_EL07c_FirstProto = 0x01,
        DeviceSubversion384Patch_EL07c_TemperatureControl = 0x02,
        DeviceSubversion384Patch_EL07e_TemperatureControl = 0x03,

        /*! Subversions used for version = 0xFE */
        DeviceSubversion2x10MHz_FET_PCBV01 = 0x19,
        DeviceSubversion2x10MHz_FET_PCBV02 = 0x1A,

        /*! Subversions used for Version = 0xFF */
        DeviceSubversionUndefined = 0xFF
    } DeviceSubversion_t ;

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t isDeviceRecognized(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = UTL_DEFAULT_FW_PATH);
    ErrorCodes_t disconnectDevice() override;

    ErrorCodes_t setCalibrationMode(bool calibModeFlag) override;
    virtual ErrorCodes_t getDeviceInfo(unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion) override;

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

    okCFrontPanel dev;

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
