#ifndef EMCROPALKELLYDEVICE_H
#define EMCROPALKELLYDEVICE_H

#define OKY_RX_PIPE_ADDR 0xA0
#define OKY_RX_BUFFER_SIZE 0x1000000 /*!< Number of bytes. Always use a power of 2 for efficient circular buffer management through index masking */
#define OKY_RX_BUFFER_MASK (OKY_RX_BUFFER_SIZE-1)
#define OKY_RX_TRANSFER_TIME_GOAL 0.005 /*!< 5ms. Ideally download data corresponding to this amount of time with each transfer, but no less to keep overhead low */
#define OKY_RX_TRANSFER_SIZE 0x100000 /*!< 1MB. Default value */
#define OKY_RX_BLOCK_SIZE 0x4000 /*!< 16kB. Must match value in FPGA FW */
#define OKY_REGISTERS_CHANGED_TRIGGER_IN_ADDR 0x53
#define OKY_REGISTERS_CHANGED_TRIGGER_IN_BIT 0x0
#define OKY_START_PROTOCOL_TRIGGER_IN_ADDR 0x53
#define OKY_START_PROTOCOL_TRIGGER_IN_BIT 0x1
#define OKY_START_STATE_ARRAY_TRIGGER_IN_ADDR 0x53
#define OKY_START_STATE_ARRAY_TRIGGER_IN_BIT 0x3
#define OKY_ZAP_PULSE_TRIGGER_IN_ADDR 0x53
#define OKY_ZAP_PULSE_TRIGGER_IN_BIT 0x4
#define OKY_SINGLE_CHANNEL_RAMP_TRIGGER_IN_ADDR 0x53
#define OKY_SINGLE_CHANNEL_RAMP_TRIGGER_IN_BIT 0x5
#define OKY_READ_CAL_EEPROM_TRIGGER_IN_ADDR 0x53
#define OKY_READ_CAL_EEPROM_TRIGGER_IN_BIT 0x6
#define OKY_GET_CAL_RAM_TRIGGER_IN_ADDR 0x53
#define OKY_GET_CAL_RAM_TRIGGER_IN_BIT 0x7
#define OKY_SET_CAL_RAM_TRIGGER_IN_ADDR 0x53
#define OKY_SET_CAL_RAM_TRIGGER_IN_BIT 0x8
#define OKY_WRITE_CAL_EEPROM_TRIGGER_IN_ADDR 0x53
#define OKY_WRITE_CAL_EEPROM_TRIGGER_IN_BIT 0x9
#define OKY_READ_ON_TIME_TRIGGER_IN_ADDR 0x53
#define OKY_READ_ON_TIME_TRIGGER_IN_BIT 0xA
#define OKY_SPI_SEND_COMMAND_TRIGGER_IN_ADDR 0x53
#define OKY_SPI_SEND_COMMAND_TRIGGER_IN_BIT 0xB
#define OKY_DEBUG_1_TRIGGER_IN_ADDR 0x53
#define OKY_DEBUG_1_TRIGGER_IN_BIT 0xC
#define OKY_DEBUG_2_TRIGGER_IN_ADDR 0x53
#define OKY_DEBUG_2_TRIGGER_IN_BIT 0xD
#define OKY_DEBUG_3_TRIGGER_IN_ADDR 0x53
#define OKY_DEBUG_3_TRIGGER_IN_BIT 0xE
#define OKY_DEBUG_4_TRIGGER_IN_ADDR 0x53
#define OKY_DEBUG_4_TRIGGER_IN_BIT 0xF
#define OKY_MOTHERBOARD_FPGA_BYTES_PER_S 333333

#include "okFrontPanelDLL.h"

#include "emcrdevice.h"
#include "utils.h"

const uint32_t OKY_RX_TRANSFER_MAX_EXP = LOG2(OKY_RX_TRANSFER_SIZE); /*!< 20. Let's keep the transfer size a power of 2, such that the size doesn't exceed OKY_RX_TRANSFER_SIZE */
const uint32_t OKY_RX_TRANSFER_MIN_EXP = LOG2(OKY_RX_BLOCK_SIZE); /*!< 14. Let's keep the transfer size a power of 2, such that the size doesn't go below OKY_RX_BLOCK_SIZE */
const uint32_t OKY_RX_EXTENDED_BUFFER_SIZE = OKY_RX_BUFFER_SIZE+OKY_RX_TRANSFER_SIZE;  /*!< Add space to be able to always store data from the ReadFromBlockPipeOut */

class OpalKellyDeviceManager : public OpalKelly::FrontPanelManager {
public:
    OpalKellyDeviceManager(std::string deviceId);

    virtual void OnDeviceAdded(const char* serial) override;
    virtual void OnDeviceRemoved(const char* serial) override;

    bool isDeviceRemoved();

private:
    std::string deviceId = "";
    bool deviceRemovedFlag = false;
};

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
        DeviceSubversionTestBoardEl05c5TransSe = 0x17,
        DeviceSubversionTestBoardEl05c5ResSe = 0x18,
        DeviceSubversionTestBoardEl05c5TransDiff = 0x19,
        DeviceSubversionTestBoardEl05c5ResDiff = 0x1A,

        /*! Subversions used for version = 0x0B */
        DeviceSubversion10MHz_SB_EL05a = 0x03,
        DeviceSubversion2x10MHz_PCBV02_EL05a = 0x04,
        DeviceSubversion4x10MHz_SB_EL05a_PCBV01 = 0x09,
        DeviceSubversion4x10MHz_SB_EL05b_PCBV01 = 0x0A,
        DeviceSubversion4x10MHz_SB_EL05a_PCBV02 = 0x0B,
        DeviceSubversion2x10MHz_SB_EL05a_PCBV02_FEStim = 0x0C,
        DeviceSubversion24x10MHz_PCBV01_EL05c34 = 0x0D,
        DeviceSubversion24x10MHz_PCBV01_EL05c12 = 0x0E,
        DeviceSubversion10MHz_SB_EL05a_PCBV03 = 0x0F,
        DeviceSubversion32x10MHz_EL05c2_PCBV01 = 0x10,
        DeviceSubversion24x10MHz_EL05c1_PCBV02 = 0x11,
        DeviceSubversion24x10MHz_EL05c3_PCBV02 = 0x12,
        DeviceSubversion24x10MHz_EL05c4_PCBV02 = 0x13,
        DeviceSubversionOk_FAKE = 0xFE,

        /*! Subversions used for version = 0x0D */
        DeviceSubversion192Blm_EL03c_MB02Mez03 = 0x01,
        DeviceSubversion192Blm_EL03c_MB03Mez04 = 0x02,
        DeviceSubversion8Blm_EL03c_DigitalTester_PCBV01 = 0x03,
        DeviceSubversion8Blm_EL03c_DigitalTester_PCBV01b = 0x04,
        DeviceSubversion192Blm_EL03c_MB03Mez05 = 0x05,

        /*! Subversions used for version = 0x0F */
        DeviceSubversion384Patch_EL07c_FirstProto = 0x01,
        DeviceSubversion384Patch_EL07c_TemperatureControl = 0x02,
        DeviceSubversion384Patch_EL07e_TemperatureControl = 0x03,
        DeviceSubversion384Patch_EL07e_MB03Mez05 = 0x04,
        DeviceSubversion16Patch_EL07e_DigitalTester_PCBV01 = 0x05,
        DeviceSubversion16Patch_EL07e_DigitalTester_PCBV01b = 0x06,
        DeviceSubversion384Patch_EL07e_MB03Mez05An05 = 0x07,

        /*! Subversions used for version = 0xFE */
        DeviceSubversion2x10MHz_FET_PCBV01 = 0x19,
        DeviceSubversion2x10MHz_FET_PCBV02 = 0x1A,
        DeviceSubversion4x10MHz_PCBV02_FEStim_CH12 = 0x1D,

        /*! Subversions used for Version = 0xFF */
        DeviceSubversionUndefined = 0xFF
    } DeviceSubversion_t ;

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwMajor, unsigned int &fwMinor, unsigned int &fwPatch);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t isDeviceRecognized(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = UTL_DEFAULT_FW_PATH);

    ErrorCodes_t setCalibrationMode(bool calibModeFlag) override;

    virtual ErrorCodes_t okMoveCalibrationEepromToRams() override;
    virtual ErrorCodes_t okMoveCalibrationRamsToEeprom() override;
    virtual ErrorCodes_t okSelectCalibrationRam(uint16_t ramIdx) override;
    virtual ErrorCodes_t okWriteCalibrationRam(uint16_t address, uint8_t value) override;
    virtual ErrorCodes_t okReadCalibrationRam() override;

    virtual ErrorCodes_t getDeviceInfo(unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwMajor, unsigned int &fwMinor, unsigned int &fwPatch) override;
    bool isDeviceConnected();

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

    virtual void computeDataReadPolicy() override;
    void monitoringLoop();

    /****************\
     *  Parameters  *
    \****************/

    okCFrontPanel dev;
    OpalKellyDeviceManager * okManager = nullptr;

    BoolCoder * calibrationRamSelectorCoder = nullptr;
    BoolCoder * calibrationRamAddressCoder = nullptr;
    BoolCoder * calibrationRamValueCoder = nullptr;

    /***************\
     *  Variables  *
    \***************/

    int waitingTimeBeforeReadingData = 1;

    okTRegisterEntries regs;

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */

    long okTransferSize = OKY_RX_TRANSFER_SIZE;
    std::thread monitoringThread;
};

#endif // EMCROPALKELLYDEVICE_H
