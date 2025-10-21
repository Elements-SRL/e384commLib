/*! \file e384commlib_global.h
 * \brief Defines global macros and typedefs.
 * \note \a E384COMMLIB_LIBRARY should be defined only during the library compilation.
 * Defining \a E384COMMLIB_LIBRARY when the library is included in a project will prevent project building.
 */
#ifndef E384COMMLIB_GLOBAL_H
#define E384COMMLIB_GLOBAL_H

#include <vector>
#include <string>
#include <math.h>
#include <limits>

/****************************\
 *  Shared library defines  *
\****************************/

#if defined(_WIN32)

#if defined(E384COMMLIB_STATIC)
#  define E384COMMLIBSHARED_EXPORT
#else
#if defined(E384COMMLIB_LIBRARY)
#  define E384COMMLIBSHARED_EXPORT __declspec(dllexport)
#else
#  define E384COMMLIBSHARED_EXPORT __declspec(dllimport)
#endif
#endif

#elif defined(__APPLE__)

#if defined(E384COMMLIB_STATIC)
#  define E384COMMLIBSHARED_EXPORT
#else
#if defined(E384COMMLIB_LIBRARY)
#  define E384COMMLIBSHARED_EXPORT __attribute__((visibility("default")))
#else
#  define E384COMMLIBSHARED_EXPORT __attribute__((visibility("default")))
#endif
#endif

#else // All posix systems including linux

#if defined(E384COMMLIB_LIBRARY)
#  define E384COMMLIBSHARED_EXPORT
#else
#  define E384COMMLIBSHARED_EXPORT
#endif

#endif

/**************************\
 *  Arguments qualifiers  *
\**************************/

#ifndef E384COMMLIB_LABVIEW_WRAPPER
#define E384COMMLIB_NAME_MANGLING
#else
#define E384COMMLIB_NAME_MANGLING extern "C"
#endif

/*! \def E384CL_ARGIN
 * \brief Dummy define to identify input arguments.
 */
#define E384CL_ARGIN

/*! \def E384CL_ARGOUT
 * \brief Dummy define to identify output arguments.
 */
#define E384CL_ARGOUT

/*! \def E384CL_ARGVOID
 * \brief Dummy define to identify void arguments.
 */
#define E384CL_ARGVOID void

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif

/**********************\
 *  Message type IDs  *
\**********************/

/*! \enum MsgDirection_t
 * \brief Enumerates the directions of the communication channel.
 */
typedef enum MsgDirection {
    MsgDirectionPcToDevice =            0x0000, /*!< Indicates messages going from library caller to the device. */
    MsgDirectionDeviceToPc =            0x8000  /*!< Indicates messages going from library caller to the device. */
} MsgDirection_t;

/*! \enum MsgGroup_t
 * \brief Enumerates groups of messages.
 */
typedef enum MsgGroup {
    MsgGroupCommunication =             0x0000, /*!< Messages regarding communication. */
    MsgGroupAcquisitionConfiguration =  0x0400, /*!< Messages regarding acquisition configuration. */
    MsgGroupHwConfiguration =           0x0800, /*!< Messages regarding hardware configuration. */
    MsgGroupVoltageTrials =             0x0C00, /*!< Messages regarding voltage protocols. */
    MsgGroupCurrentTrials =             0x1000, /*!< Messages regarding current protocols. */
    MsgGroupAcquiredData =              0x1400, /*!< Messages regarding acquired data. */
    MsgGroupFeatures =                  0x1800, /*!< Messages regarding device's features. */
    MsgGroupDeviceStatus =              0x2000, /*!< Messages regarding device's status. */
    MsgGroupCalibrationUtilities =      0x7C00  /*!< Messages used for calibrating the device. */
} MsgGroup_t;

/*! \enum MsgTypeId_t
 * \brief Enumerates the messages IDs.
 */
typedef enum MsgTypeId {
    /*! Communication messages */
    MsgTypeIdAck =                              MsgGroupCommunication+0x0001, /*!< Acknoledge. \note These messages are implicitly handled by the library. */
    MsgTypeIdNack =                             MsgGroupCommunication+0x0002, /*!< Not acknoledge. \note These messages are implicitly handled by the library. */
    MsgTypeIdPing =                             MsgGroupCommunication+0x0003, /*!< Ping. \note These messages are implicitly handled by the library. */
    MsgTypeIdAbort =                            MsgGroupCommunication+0x0004, /*!< Message used to force the device to stop doing anything and sending data. \note Sometimes these messages are implicitly used by the library. */

    /*! Acquisition configuration messages */
    MsgTypeIdSamplingRate =                     MsgGroupAcquisitionConfiguration+0x0001, /*!< Message used to set the sampling rate. */

    /*! Hardware configuration messages */
    MsgTypeIdSwitchCtrl =                       MsgGroupHwConfiguration+0x0001, /*!< Message used to control the device internal circuitry. */
    MsgTypeIdRegistersCtrl =                    MsgGroupHwConfiguration+0x0002, /*!< Message used to control the device internal registers. */
    MsgTypeIdFpgaReset =                        MsgGroupHwConfiguration+0x0003, /*!< Message used to reset the FPGA */

    /*! Voltage trials messages */
    MsgTypeIdVoltageProtocolStruct =            MsgGroupVoltageTrials+0x0001, /*!< Message used to describe the struct of a voltage protocol. \note The composition of voltage protocol messages is implicitly handled by the library. */
    MsgTypeIdVoltageStepTimeStep =              MsgGroupVoltageTrials+0x0002, /*!< Message used to apply a protocol item with step voltage and step time. */
    MsgTypeIdVoltageRamp =                      MsgGroupVoltageTrials+0x0003, /*!< Message used to apply a protocol item with a voltage ramp. */
    MsgTypeIdVoltageSin =                       MsgGroupVoltageTrials+0x0004, /*!< Message used to apply a protocol item with a voltage sinusoidal wave. */
    MsgTypeIdStartProtocol =                    MsgGroupVoltageTrials+0x0005, /*!< Message used to start a protocol. */

    /*! Current trials messages */
    MsgTypeIdCurrentProtocolStruct =            MsgGroupCurrentTrials+0x0001, /*!< Message used to describe the struct of a current protocol. \note The composition of voltage protocol messages is implicitly handled by the library. */
    MsgTypeIdCurrentStepTimeStep =              MsgGroupCurrentTrials+0x0002, /*!< Message used to apply a protocol item with step current and step time. */
    MsgTypeIdCurrentRamp =                      MsgGroupCurrentTrials+0x0003, /*!< Message used to apply a protocol item with a current ramp. */
    MsgTypeIdCurrentSin =                       MsgGroupCurrentTrials+0x0004, /*!< Message used to apply a protocol item with a current sinusoidal wave. */

    /*! Acquired data messages */
    MsgTypeIdAcquisitionHeader =                MsgGroupAcquiredData+0x0001, /*!< Message used to the data messages that will follow. */
    MsgTypeIdAcquisitionData =                  MsgGroupAcquiredData+0x0002, /*!< Message containing current and voltage data. */
    MsgTypeIdAcquisitionTail =                  MsgGroupAcquiredData+0x0003, /*!< Message that notifies the end of data stream. */
    MsgTypeIdAcquisitionSaturation =            MsgGroupAcquiredData+0x0004, /*!< Message that notifies the saturation of the front-end. */
    MsgTypeIdAcquisitionDataLoss =              MsgGroupAcquiredData+0x0005, /*!< Message that notifies the loss of data. */
    MsgTypeIdAcquisitionDataOverflow =          MsgGroupAcquiredData+0x0006, /*!< Message that notifies the overflow of data. */
    MsgTypeIdAcquisitionTemperature =           MsgGroupAcquiredData+0x0007, /*!< Message containing temperature data. */

    /*! Features messages */
    MsgTypeIdLiquidJunctionComp =               MsgGroupFeatures+0x0001, /*!< Message used to apply the liquid junction  compensation. */
    MsgTypeIdLiquidJunctionCompInquiry =        MsgGroupFeatures+0x0002, /*!< Message used to request the liquid junction potential following its compensation. */
    MsgTypeIdDigitalOffsetComp =                MsgGroupFeatures+0x0001, /*!< Message used to apply the liquid junction  compensation.
                                                                              \deprecated Use MsgTypeIdLiquidJunctionComp instead */
    MsgTypeIdDigitalOffsetCompInquiry =         MsgGroupFeatures+0x0002, /*!< Message used to request the liquid junction potential following its compensation.
                                                                              \deprecated Use MsgTypeIdLiquidJunctionCompInquiry instead */
    MsgTypeIdZap =                              MsgGroupFeatures+0x0003, /*!< Message used to generate a cell breaking zap. */
    MsgTypeIdDigitalTriggerOutput =             MsgGroupFeatures+0x0004, /*!< Message used to configure the digital trigger output. */
    MsgTypeIdLockIn =                           MsgGroupFeatures+0x0005, /*!< Message used to enter lock in mode for impendance estimation. */
    MsgTypeIdDigitalRepetitiveTriggerOutput =   MsgGroupFeatures+0x0006, /*!< Message used to configure the digital repetitive trigger output. */
    MsgTypeIdInvalid =                          MsgGroupFeatures+0x03FF, /*!< Invalid message used only for initiliazation purposes. */

    /*! Device status message*/
    MsgTypeIdDeviceStatus =                     MsgGroupDeviceStatus+0x0001, /*!< Message containing the device status. */

} MsgTypeId_t;

/********************\
 *  Other typedefs  *
\********************/

/*! \enum ClampingModality_t
 * \brief Enum that identifies different clamp modality.
 */
typedef enum{
    VOLTAGE_CLAMP,
    ZERO_CURRENT_CLAMP,
    CURRENT_CLAMP,
    DYNAMIC_CLAMP,
    VOLTAGE_CLAMP_VOLTAGE_READ,
    CURRENT_CLAMP_CURRENT_READ,
    UNDEFINED_CLAMP
} ClampingModality_t;

/*! \enum DeviceTypes_t
 * \brief Enumerates the device types that can be handled by e384CommLib.
 */
typedef enum DeviceTypes {
    /*! EMCR devices */
    Device192Blm_el03c_prot_v01_fw_v01,         /*!< 192 channels device for BLM experiments */
    Device192Blm_el03c_mez03_mb04_fw_v01,       /*!< 192 channels device for BLM experiments */
    Device192Blm_el03c_mez03_mb04_fw_v02,       /*!< 192 channels device for BLM experiments */
    Device384Nanopores,                         /*!< 384 channels device for nanopores experiments */
    Device384Nanopores_SR7p5kHz,                /*!< 384 channels device for nanopores experiments */
    Device384PatchClamp_prot_v01_fw_v02,        /*!< 384 channels device for patchclamp experiments */
    Device384PatchClamp_prot_v04_fw_v03,        /*!< 384 channels device for patchclamp experiments */
    Device384PatchClamp_prot_v04_fw_v04,        /*!< 384 channels device for patchclamp experiments */
    Device384PatchClamp_prot_v04_fw_v05,        /*!< 384 channels device for patchclamp experiments */
    Device384PatchClamp_prot_v05_fw_v06,        /*! Header packet */
    Device384PatchClamp_prot_el07c_v06_fw_v02,  /*!< 384 channels device for patchclamp experiments */
    Device384PatchClamp_prot_el07c_v07_fw_v03,  /*!< 384 channels device for patchclamp experiments */
    Device384PatchClamp_prot_el07c_v08_fw_v255, /*!< 384 channels device for patchclamp experiments */
    Device384PatchClamp_el07e_fw_v01,           /*!< 384 channels device for patchclamp experiments */
    Device384VoltageClamp_prot_v04_fw_v03,      /*!< 384 channels device for voltageclamp experiments */
    DeviceTestBoardEL07ab,
    DeviceTestBoardEL07c,
    DeviceTestBoardEL07d,
    DeviceTestBoardQC01a,
    DeviceTestBoardQC01aExtVcm,
    DeviceTestBoardQC02a_PCBV02,
    Device10MHz_SB_V01,                         /*!< 10MHz nanopore reader, single board */
    Device2x10MHz_PCBV01,                       /*!< 2 channels 10MHz nanopore reader */
    Device2x10MHz_PCBV02,                       /*!< 2 channels 10MHz nanopore reader */
    Device4x10MHz_PCBV01,                       /*!< 4 channels 10MHz nanopore reader */
    Device4x10MHz_PCBV03,                       /*!< 4 channels 10MHz nanopore reader */
    Device4x10MHz_SB_PCBV01_FWV01,              /*!< 4 channels 10MHz nanopore reader, single board */
    Device4x10MHz_SB_PCBV01_FWV02,              /*!< 4 channels 10MHz nanopore reader, single board */
    Device2x10MHz_SB_PCBV02_FWV01_FEStim,       /*!< 2 channels 10MHz nanopore reader, single board with stimulus from the front end */
    Device24x10MHz_Only8Ch_PCBV01_EL05c1,       /*!< 24 channels 10MHz nanopore reader, but only 8 active */
    Device24x10MHz_Only8Ch_PCBV01_EL05c2,       /*!< 24 channels 10MHz nanopore reader, but only 8 active */
    Device24x10MHz_Only8Ch_PCBV01_EL05c3,       /*!< 24 channels 10MHz nanopore reader, but only 8 active */
    Device24x10MHz_Only8Ch_PCBV01_EL05c4,       /*!< 24 channels 10MHz nanopore reader, but only 8 active */
    Device24x10MHz_PCBV01_EL05c12,              /*!< 24 channels 10MHz nanopore reader */
    Device24x10MHz_PCBV01_EL05c34,              /*!< 24 channels 10MHz nanopore reader */
    Device4x10MHz_QuadAnalog_PCBV01,            /*!< 4 channels 10MHz nanopore reader, one analog board */
    Device4x10MHz_QuadAnalog_PCBV01_DIGV01,     /*!< 4 channels 10MHz nanopore reader, one analog board, digital board V01 */
    Device2x10MHz_FET,                          /*!< 2 channels 10MHz nanopore reader with controllable reference voltages */
    Device10MHzOld,                             /*!< UDB based 10MHz nanopore reader, old firmware */
    Device10MHzV01,                             /*!< UDB based 10MHz nanopore reader */
    DeviceSuperDuck_PCBV01,
    /*! EZ patch devices */
    DeviceEPatchEL03D_V04,                      /*!< ePatch device with EL03D chip: digital repetitive trigger output. */
    DeviceEPatchEL03D_V03,                      /*!< ePatch device with EL03D chip: increased maximum amount of digital trigger output events. */
    DeviceEPatchEL03D_V02,                      /*!< ePatch device with EL03D chip: digital triggers limited to 21 */
    DeviceEPatchEL03D_V01,                      /*!< ePatch device with EL03D chip: before voltage hold tuner implementation. */
    DeviceEPatchEL03D_V00,                      /*!< ePatch device with EL03D chip: before digital trigger output implementation. */
    DeviceEPatchEL03F_4D_PCBV03_V04,            /*!< ePatch device with EL03F and EL04D chips: digital repetitive trigger output. */
    DeviceEPatchEL03F_4D_PCBV03_V03,            /*!< ePatch device with EL03F and EL04D chips: increased maximum amount of digital trigger output events. */
    DeviceEPatchEL03F_4D_PCBV03_V02,            /*!< ePatch device with EL03F and EL04D chips: PCB V03 for master/slave configurability. */
    DeviceEPatchEL03F_4D_PCBV02_V04,            /*!< ePatch device with EL03F and EL04D chips: increased maximum amount of digital trigger output events */
    DeviceEPatchEL03F_4D_PCBV02_V03,            /*!< ePatch device with EL03F and EL04D chips: increased maximum amount of digital trigger output events */
    DeviceEPatchEL03F_4D_PCBV02_V02,            /*!< ePatch device with EL03F and EL04D chips: voltage and current hold tuner implementation */
    DeviceEPatchEL03F_4D_PCBV02_V01,            /*!< ePatch device with EL03F and EL04D chips: added digital trigger output before. */
    DeviceEPatchEL04E,                          /*!< ePatch device with EL04E chip. */
    DeviceEPatchEL03F_4E_PCBV03_V04,            /*!< ePatch device with EL03F and EL04E chips: digital repetitive trigger output. */
    DeviceEPatchEL03F_4E_PCBV03_V03,            /*!< ePatch device with EL03F and EL04E chips: increased maximum amount of digital trigger output events. */
    DeviceEPatchEL03F_4E_PCBV03_V02,            /*!< ePatch device with EL03F and EL04E chips: PCB V03 for master/slave configurability. */
    DeviceEPatchEL03F_4E_PCBV02_V04,            /*!< ePatch device with EL03F and EL04E chips: digital repetitive trigger output. */
    DeviceEPatchEL03F_4E_PCBV02_V03,            /*!< ePatch device with EL03F and EL04E chips: increased maximum amount of digital trigger output events */
    DeviceEPatchEL03F_4E_PCBV02_V02,            /*!< ePatch device with EL03F and EL04E chips: voltage and current hold tuner implementation */
    DeviceEPatchEL03F_4E_PCBV02_V01,            /*!< ePatch device with EL03F and EL04E chips: added digital trigger output before. */
    DeviceEPatchEL03F_4E_PCBV02_V00,            /*!< ePatch device with EL03F and EL04E chips. */
    DeviceEPatchEL04F,                          /*!< ePatch device with EL04F chip. */
    DeviceEPatchEL03F_4F_PCBV01_AnalogOut,      /*!< ePatch device with EL03F and EL04F chips: added analog output. */
    DeviceEPatchEL03F_4F_PCBV03_V04,            /*!< ePatch device with EL03F and EL04F chips: digital repetitive trigger output. */
    DeviceEPatchEL03F_4F_PCBV02_V04,            /*!< ePatch device with EL03F and EL04F chips: digital repetitive trigger output. */
    DeviceEPatchEL03F_4F_PCBV03_V03,            /*!< ePatch device with EL03F and EL04F chips: increased maximum amount of digital trigger output events. */
    DeviceE4PEL04F,                             /*!< ePatch device with 4 EL04F chips. */
    DeviceE4PPatch,                             /*!< ePatch device with 4 EL04F chips. */
    DeviceE8PPatch,                             /*!< ePatch device with 8 EL04F chips. */
    DeviceE4PPatchEL07AB,                       /*!< ePatch device with 1 EL07AB chips. */
    DeviceE8PPatchEL07AB,                       /*!< ePatch device with 1 EL07AB chips. */
    DeviceE8PPatchEL07AB_artix7_PCBV00_2_V02,   /*!< ePatch device with 1 EL07AB chips PCB V02 (FPGA artix7). Increased protocol items to 256. */
    DeviceE8PPatchEL07AB_artix7_PCBV00_2_V01,   /*!< ePatch device with 1 EL07AB chips PCB V02 (FPGA artix7). */
    DeviceE8PPatchEL07AB_artix7_PCBV00_1,       /*!< ePatch device with 1 EL07AB chips PCB V01 (FPGA artix7). */
    DeviceE8PPatchEL07CD_artix7_PCBV00_2,       /*!< ePatch device with 1 EL07CD chips PCB V02 (FPGA artix7). */
    DeviceE8PPatchEL07CD_artix7_PCBV00_1,       /*!< ePatch device with 1 EL07CD chips PCB V01 (FPGA artix7). */
    DeviceE4PPatchEL07CD_artix7_PCBV00_1,       /*!< ePatch device with 1 EL07CD chips and 4 channels PCB V01 (FPGA artix7). */
    DeviceE4PPatchEL07CD_artix7_PCBV00_2,       /*!< ePatch device with 1 EL07CD chips and 4 channels PCB V01 (FPGA artix7). */
    DeviceE8PPatchEL07CD_artix7_PCBV01_FW2,     /*!< ePatch device with 1 EL07CD chips PCB V01 (FPGA artix7) with fixed protocols generation. */
    DeviceE8PPatchEL07CD_artix7_PCBV01_FW3,     /*!< ePatch device with 1 EL07CD chips PCB V01 (FPGA artix7) with 64 protocol items. */
    DeviceE8nPatchEL07C_artix7_PCBV01_FW1,      /*!< ePatch device with 1 EL07C chip PCB V01 (FPGA artix7). */
    DeviceE8nPatchEL07C_artix7_EL07e_PCBV02_FW1,/*!< ePatch device with 1 EL07C chip PCB V01 (FPGA artix7). */
    DeviceEPatchDlp,                            /*!< ePatch device with dlp fpga. */
    Device384Fake,                              /*!< Fake nanopore device */
    Device384FakePatchClamp,                    /*!< Fake patch clamp device */
    DeviceTbEl07cdFake,                         /*!< Fake test board for EL07ab */
    Device4x10MHzFake,                          /*!< Fake 4x10MHz device */
    Device2x10MHzFake,                          /*!< Fake 2x10MHz device */
    Device10MHzFake,                            /*!< Fake 10MHz device */
    DeviceFakePatch,                            /*!< Fake ePatch device resembling EL04F chip. */
    DeviceFakeP8,                               /*!< Fake 8Patch device resembling EL07AB chip. */
    DeviceUnknown,                              /*!< Invalid item used only for initiliazation purposes. */
    DevicesNum
} DeviceTypes_t;

/*! \def E384CL_OUT_STRUCT_DATA_LEN
 * \brief Max size of unsigned int 16 numbers returned by method e384CommLib::getNextMessage.
 */
#define E384CL_OUT_STRUCT_DATA_LEN 0x100000

/*! \struct RxOutput_t
 * \brief Structure used to return to the caller data and information received from the device.
 */
typedef struct RxOutput {
    uint16_t msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid; /*!< Type of message received. */
    uint16_t channelIdx = 0; /*!< For msgTypeId that work channel-wise this field holds the channel index the message refers too, e.g. the compensated channel after digital compensation */
    uint16_t protocolId = 0; /*!< When #msgTypeId is MsgDirectionDeviceToPc + MsgTypeIdAcquisitionHeader this field holds the protocol identifier number */
    uint16_t protocolItemIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToPc + MsgTypeIdAcquisitionHeader this field holds the item index of the following data in the current protocol */
    uint16_t protocolRepsIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToPc + MsgTypeIdAcquisitionHeader this field holds the iteration of an interal loop of the following data in the current protocol */
    uint16_t protocolSweepIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToPc + MsgTypeIdAcquisitionHeader this field holds the sweep index of the following data in the current protocol */
    uint32_t totalMessages = 0; /*!< When #msgTypeId is MsgDirectionDeviceToPc + MsgTypeIdAcquisitionDataOverflow this field indicates the amount of messages since the last getNextMessage */
    uint32_t firstSampleOffset = 0; /*!< When #msgTypeId is MsgDirectionDeviceToPc + MsgTypeIdAcquisitionData this field holds the data offset wrt the protocol first sample */
    uint32_t dataLen = 0; /*!< Number of data samples available in field data.
                           * \note In case msgTypeId = MsgDirectionDeviceToPc + MsgTypeIdAcquisitionDataLoss this equals 2, and the 2 values in data has to be converted into a single uint32_t value
                           * which equals the unmber of times some data was lost, using the formula uint32_t samplesLost = (uint32_t)(uint16_t)data[0] + (uint32_t)(uint16_t)data[1] << 16.
                           * \note In case msgTypeId = MsgDirectionDeviceToPc + MsgTypeIdAcquisitionDataOverflow this equals 2, and the 2 values in data has to be converted into a single uint32_t value
                           * which equals the amount of samples overwritten due to the buffer overflow, using the formula uint32_t samplesLost = (uint32_t)(uint16_t)data[0] + (uint32_t)(uint16_t)data[1] << 16. */
} RxOutput_t;

/*! \enum ProtocolItemTypes_t
 * \brief Enumerates the items that can be used to build stimulus waveforms.
 */
typedef enum ProtocolItemTypes {
    ProtocolItemStep,
    ProtocolItemRamp,
    ProtocolItemSin,
    ProtocolItemTypesNum
} ProtocolItemTypes_t;

/*! \enum TxTriggerType_t
 * \brief Enumerates the notifications that can be sent to the device.
 */
typedef enum TxTriggerType {
    TxTriggerParameteresUpdated,
    TxTriggerStartProtocol,
    TxTriggerStartStateArray,
    TxTriggerZap
} TxTriggerType_t;

/*! \enum ResetControl_t
 * \brief Enumerates the possible effects of commands over the reset state of the device.
 */
typedef enum ResetControl { /*! \todo FCON Forse servirà qualcosa di più complesso per evitare che il reset ASIC ed FPGA interferiscano,
                             *  e.g. ResetFalse dell'FPGA disabilita un ResetTure dell'ASIC */
    ResetIndifferent,
    ResetTrue,
    ResetFalse
} ResetControl_t;

/*! \struct CommandOptions_t
 * \brief Structure used internally to define the options accompanying commands for the device.
 */
typedef struct CommandOptions {
    TxTriggerType_t triggerType = TxTriggerParameteresUpdated; /*!< Notification sent together with the command */
    ResetControl_t resetControl = ResetIndifferent; /*!< true if the command will put the device in a reset state that stops the incoming data flow */
} CommandOptions_t;

/*! \enum OffsetRecalibStatus_t
 * \brief Enumerates the possible statuses of the readout offset recalibration algorithm.
 */
typedef enum OffsetRecalibStatus {
    OffsetRecalibNotPerformed,
    OffsetRecalibExecuting,
    OffsetRecalibInterrupted,
    OffsetRecalibSucceded,
    OffsetRecalibFailed,
    OffsetRecalibResetted,
    OffsetRecalibStatusesNum
} OffsetRecalibStatus_t;

/*! \enum LiquidJunctionStatus_t
 * \brief Enumerates the possible statuses of the liquid junction compensation (aka digital offset compensation) algorithm.
 */
typedef enum LiquidJunctionStatus {
    LiquidJunctionNotPerformed,
    LiquidJunctionExecuting,
    LiquidJunctionInterrupted,
    LiquidJunctionSucceded,
    LiquidJunctionFailedOpenCircuit,
    LiquidJunctionFailedTooManySteps,
    LiquidJunctionFailedSaturation,
    LiquidJunctionResetted,
    LiquidJunctionStatusesNum
} LiquidJunctionStatus_t;

/*! \enum UnitPfx_t
 * \brief Enumerates the unit prefixes used.
 */
typedef enum UnitPfx {
    UnitPfxFemto    = 0,    /*!< 10^-15 */
    UnitPfxPico     = 1,    /*!< 10^-12 */
    UnitPfxNano     = 2,    /*!< 10^-9 */
    UnitPfxMicro    = 3,    /*!< 10^-6 */
    UnitPfxMilli    = 4,    /*!< 10^-3 */
    UnitPfxNone     = 5,    /*!< 10^0 = 1 */
    UnitPfxKilo     = 6,    /*!< 10^3 */
    UnitPfxMega     = 7,    /*!< 10^6 */
    UnitPfxGiga     = 8,    /*!< 10^9 */
    UnitPfxTera     = 9,    /*!< 10^12 */
    UnitPfxPeta     = 10,   /*!< 10^15 */
    UnitPfxNum              /*!< Invalid item used only for loop purposes. */
} UnitPfx_t;

#ifndef E384COMMLIB_LABVIEW_WRAPPER
/*! Momentarily close namespace because it gets open in e4dccommlib_global_addendum.h */
}  // namespace e384CommLib

#include "e384commlib_global_addendum.h"

namespace e384CommLib {
#endif

/*! \struct ChannelSources_t
 * \brief Structure used to return available data sources for a channel.
 * \note -1 means that the source is not available.
 */
typedef struct ChannelSources {
    int16_t VoltageFromVoltageClamp = -1; /*!< Get voltage applied by voltage clamp front-end. */
    int16_t CurrentFromVoltageClamp = -1; /*!< Get current read by voltage clamp front-end. */
    int16_t VoltageFromCurrentClamp = -1; /*!< Get voltage read by current clamp front-end. */
    int16_t CurrentFromCurrentClamp = -1; /*!< Get current applied by current clamp front-end. */
    int16_t VoltageFromDynamicClamp = -1; /*!< Get voltage computed by dynamic clamp. */
    int16_t CurrentFromDynamicClamp = -1; /*!< Get current computed by current clamp. */
    int16_t VoltageFromVoltagePlusDynamicClamp = -1; /*!< Get voltage applied by voltage clamp front-end plus voltage computed by dynamic clamp. */
    int16_t CurrentFromCurrentPlusDynamicClamp = -1; /*!< Get current applied by current clamp front-end plus current computed by dynamic clamp. */
} ChannelSources_t;

/*! \struct PidParams_t
 * \brief Parameters for pid controls.
 */
typedef struct PidParams {
    double proportionalGain = 1.0;
    double integralGain = 0.0;
    double derivativeGain = 0.0;
    double integralAntiWindUp = 1.0; /*!< The integral error is clipped into the range [-integralAntiWindUp, +integralAntiWindUp] */
} PidParams_t;

#ifdef E384COMMLIB_LABVIEW_WRAPPER
#define _NI_int8_DEFINED_
#include "extcode.h"
/*! \typedef CharMeasurement_t
 */

/*! \struct CharMeasurement_t
 * \brief Structure used manage physical quantities that define a value with its unit and unit prefix.
 */
typedef struct LVMeasurement {
    double value; /*!< Numerical value. */
    UnitPfx_t prefix; /*!< Unit prefix in the range [femto, Peta]. */
    LStrHandle unit; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */
} LVMeasurement_t;

/*! \typedef CharRangedMeasurement_t
 */

/*! \struct CharRangedMeasurement_t
 * \brief Structure used manage physical ranges that define a range with its unit and unit prefix.
 */
typedef struct LVRangedMeasurement {
    double min; /*!< Minimum value. */
    double max; /*!< Maximum value. */
    double step; /*!< Resolution. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    LStrHandle unit; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */
} LVRangedMeasurement_t;

/*! \struct CharCompensationControl_t
 * \brief Structure used to return detailed information on a specific compensation implemented by the HW.
 */
typedef struct LVCompensationControl {
    bool implemented = false; /*!< True if the corresponding compensation is implemented by the device. */
    double min = 0.0; /*!< Minimum compensable value. */
    double max = 1.0; /*!< Maximum compensable value globally. */
    double compensable = 1.0; /*!< Maximum compensable value given also the value of the other compensations. */
    double steps = 2; /*!< Number of steps between #min and #max. */
    double step = 1.0; /*!< Resolution. */
    int decimals = 0; /*!< Decimals to represent the compensated value. */
    double value = 0.0; /*!< Compensated value. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    LStrHandle unit; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */
    LStrHandle name; /*!< Name of the compensation. */
} LVCompensationControl_t;

#include "lv_prolog.h"
typedef struct {
    int32 cnt;                            /* number of measurements that follow */
    LVMeasurement_t item[1];              /* cnt measurements */
} LMeas, *LMeasPtr, **LMeasHandle;

typedef struct {
    int32 cnt[2];                         /* size of matrix of measurements that follow */
    LVMeasurement_t item[1];              /* cnt vector of measurements */
} LVecMeas, *LVecMeasPtr, **LVecMeasHandle;

typedef struct {
    int32 cnt;                              /* number of ranged measurements that follow */
    LVRangedMeasurement_t	item[1];        /* cnt ranged measurements */
} LRange, *LRangePtr, **LRangeHandle;

typedef struct {
    int32 cnt;                            /* number of compensation controls that follow */
    LVCompensationControl_t item[1];      /* cnt compensation control */
} LComp, *LCompPtr, **LCompHandle;

#define LVecBuf(sp)	(&((sp))->item[0])                          /* pointer to first item of vector */
#define LVecItem(sp, n)	((&((sp))->item[n]))                    /* pointer to n-th item of vector */
#define LVecLen(sp)	(((sp))->cnt)                               /* # of items in vector */
#define LMatS1(sp) (((sp))->cnt[0])                             /* # of rows in matrix */
#define LMatS2(sp) (((sp))->cnt[1])                             /* # of cols in matrix */
#define LMatLen(sp)	(LMatS1(sp)*LMatS2(sp))                     /* # of items in matrix */
#define LMatItem(sp, m, n) ((&((sp))->item[m+n*LMatS1(sp)]))    /* pointer to n-th item of vector */
#include "lv_epilog.h"
#endif

#ifndef E384COMMLIB_LABVIEW_WRAPPER
} // namespace e384CommLib
#endif

#endif // E384COMMLIB_GLOBAL_H
