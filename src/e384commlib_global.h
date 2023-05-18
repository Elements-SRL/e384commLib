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

#ifndef E384CL_LABVIEW_COMPATIBILITY
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

/*! \def E384CL_DATA_ARRAY_SIZE
 * \brief Size of data array.
 * When calling method readData data corresponding to at most #E384CL_DATA_ARRAY_SIZE samples is returned.
 * When calling method readData provide an array of float with at least #E384CL_DATA_ARRAY_SIZE items.
 */
#define E384CL_DATA_ARRAY_SIZE (65536)

#ifndef E384CL_LABVIEW_COMPATIBILITY
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
    MsgTypeIdAck =                      MsgGroupCommunication+0x0001, /*!< Acknoledge. \note These messages are implicitly handled by the library. */
    MsgTypeIdNack =                     MsgGroupCommunication+0x0002, /*!< Not acknoledge. \note These messages are implicitly handled by the library. */
    MsgTypeIdPing =                     MsgGroupCommunication+0x0003, /*!< Ping. \note These messages are implicitly handled by the library. */
    MsgTypeIdAbort =                    MsgGroupCommunication+0x0004, /*!< Message used to force the device to stop doing anything and sending data. \note Sometimes these messages are implicitly used by the library. */

    /*! Acquisition configuration messages */
    MsgTypeIdSamplingRate =             MsgGroupAcquisitionConfiguration+0x0001, /*!< Message used to set the sampling rate. */
    MsgTypeIdFilterRatio =              MsgGroupAcquisitionConfiguration+0x0002, /*!< Message used to set the ratio between the sampling rate and the bandwidth. */
    MsgTypeIdUpsampling =               MsgGroupAcquisitionConfiguration+0x0003, /*!< Message used to set an hardware upsampling ratio. \note This option increases the sampling rate but not the bandwidth. */

    /*! Hardware configuration messages */
    MsgTypeIdSwitchCtrl =               MsgGroupHwConfiguration+0x0001, /*!< Message used to control the device internal circuitry. */
    MsgTypeIdRegistersCtrl =            MsgGroupHwConfiguration+0x0002, /*!< Message used to control the device internal registers. */
    MsgTypeIdFpgaReset =                MsgGroupHwConfiguration+0x0003, /*!< Message used to reset the FPGA */

    /*! Voltage trials messages */
    MsgTypeIdVoltageProtocolStruct =    MsgGroupVoltageTrials+0x0001, /*!< Message used to describe the struct of a voltage protocol. \note The composition of voltage protocol messages is implicitly handled by the library. */
    MsgTypeIdVoltageStepTimeStep =      MsgGroupVoltageTrials+0x0002, /*!< Message used to apply a protocol item with step voltage and step time. */
    MsgTypeIdVoltageRamp =              MsgGroupVoltageTrials+0x0003, /*!< Message used to apply a protocol item with a voltage ramp. */
    MsgTypeIdVoltageSin =               MsgGroupVoltageTrials+0x0004, /*!< Message used to apply a protocol item with a voltage sinusoidal wave. */
    MsgTypeIdStartProtocol =            MsgGroupVoltageTrials+0x0005, /*!< Message used to start a protocol. */

    /*! Current trials messages */
    MsgTypeIdCurrentProtocolStruct =    MsgGroupCurrentTrials+0x0001, /*!< Message used to describe the struct of a current protocol. \note The composition of voltage protocol messages is implicitly handled by the library. */
    MsgTypeIdCurrentStepTimeStep =      MsgGroupCurrentTrials+0x0002, /*!< Message used to apply a protocol item with step current and step time. */
    MsgTypeIdCurrentRamp =              MsgGroupCurrentTrials+0x0003, /*!< Message used to apply a protocol item with a current ramp. */
    MsgTypeIdCurrentSin =               MsgGroupCurrentTrials+0x0004, /*!< Message used to apply a protocol item with a current sinusoidal wave. */

    /*! Acquired data messages */
    MsgTypeIdAcquisitionHeader =        MsgGroupAcquiredData+0x0001, /*!< Message used to the data messages that will follow. */
    MsgTypeIdAcquisitionData =          MsgGroupAcquiredData+0x0002, /*!< Message containing current and voltage data. */
    MsgTypeIdAcquisitionTail =          MsgGroupAcquiredData+0x0003, /*!< Message that notifies the end of data stream. */
    MsgTypeIdAcquisitionSaturation =    MsgGroupAcquiredData+0x0004, /*!< Message that notifies the saturation of the front-end. */

    /*! Features messages */
    MsgTypeIdDigitalOffsetComp =        MsgGroupFeatures+0x0001, /*!< Message used to apply the digital offset compensation. */
    MsgTypeIdDigitalOffsetCompInquiry = MsgGroupFeatures+0x0002, /*!< Message used to request the liquid junction potential following the digital offset compensation. */
    MsgTypeIdZap =                      MsgGroupFeatures+0x0003, /*!< Message used to generate a cell breaking zap. */
    MsgTypeIdDigitalTriggerOutput =     MsgGroupFeatures+0x0004, /*!< Message used to configure the digital trigger output. */
    MsgTypeIdLockIn =                   MsgGroupFeatures+0x0005, /*!< Message used to enter lock in mode for impendance estimation. */
    MsgTypeIdInvalid =                  MsgGroupFeatures+0x03FF, /*!< Invalid message used only for initiliazation purposes. */

    /*! Device status message*/
    MsgTypeIdDeviceStatus =             MsgGroupDeviceStatus+0x0001, /*!< Message containing the device status. */

} MsgTypeId_t;

/********************\
 *  Other typedefs  *
\********************/

/*! \enum ClampingModality_t
 * \brief Enum that identifies different clamp modality.
 */
typedef enum{
    VOLTAGE_CLAMP = 0,
    ZERO_CURRENT_CLAMP = 1,
    CURRENT_CLAMP = 2,
    DYNAMIC_CLAMP = 3,
} ClampingModality_t;

/*! \enum DeviceTypes_t
 * \brief Enumerates the device types that can be handled by e384CommLib.
 */
typedef enum DeviceTypes {
    Device384Nanopores,         /*!< 384 channels device for nanopores experiments */
    Device384PatchClamp,        /*!< Nanion's Syncropatch */
    Device4x10MHz,              /*!< 4 channels 10MHz nanopore reader */
    Device10MHz,                /*!< channels 10MHz nanopore reader */
#ifdef DEBUG
    Device384Fake,              /*!< Fake nanopore device */
#endif
    DeviceUnknown,              /*!< Invalid item used only for initiliazation purposes. */
    DevicesNum
} DeviceTypes_t;

/*! \def E384CL_OUT_STRUCT_DATA_LEN
 * \brief Max size of double precision numbers returned by method e384CommLib::getNextMessage.
 */
#define E384CL_OUT_STRUCT_DATA_LEN 0x100000

/*! \struct RxOutput_t
 * \brief Structure used to return to the caller data and information received from the device.
 */
typedef struct RxOutput {
    uint16_t msgTypeId = MsgDirectionDeviceToPc+MsgTypeIdInvalid; /*!< Type of message received. */
    uint16_t channelIdx = 0; /*!< For msgTypeId that work channel-wise this field holds the channel index the message refers too, e.g. the compensated channel after digital compensation */
    uint16_t protocolId = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the protocol identifier number */
    uint16_t protocolItemIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the item index of the following data in the current protocol */
    uint16_t protocolRepsIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the iteration of an interal loop of the following data in the current protocol */
    uint16_t protocolSweepIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the sweep index of the following data in the current protocol */
    uint32_t firstSampleOffset = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionData this field holds the data offset wrt the protocol first sample */
    uint32_t dataLen = 0; /*!< Number of data samples available in field data */
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

/*! \enum ProtocolItemTypes_t
 * \brief Enumerates the notifications that can be sent to the FPGA.
 */
typedef enum TxTriggerType_t {
    TxTriggerParameteresUpdated,
    TxTriggerStartProtocol
} TxTriggerType_t;

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

#ifndef E384CL_LABVIEW_COMPATIBILITY
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

#ifdef E384CL_LABVIEW_COMPATIBILITY
#include "extcode.h"
/*! \typedef CharMeasurement_t
 */

/*! \struct CharMeasurement_t
 * \brief Structure used manage physical quantities that define a value with its unit and unit prefix.
 */
typedef struct CharMeasurement {
    double value; /*!< Numerical value. */
    UnitPfx_t prefix; /*!< Unit prefix in the range [femto, Peta]. */
    LStrHandle unit; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */
} CharMeasurement_t;

/*! \typedef CharRangedMeasurement_t
 */

/*! \struct CharRangedMeasurement_t
 * \brief Structure used manage physical ranges that define a range with its unit and unit prefix.
 */
typedef struct CharRangedMeasurement {
    double min; /*!< Minimum value. */
    double max; /*!< Maximum value. */
    double step; /*!< Resolution. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    LStrHandle unit; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */
} CharRangedMeasurement_t;

/*! \struct CompensationControl_t
 * \brief Structure used to return detailed information on a specific compensation implemented by the HW.
 */
typedef struct CharCompensationControl {
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
} CharCompensationControl_t;

#include "lv_prolog.h"
typedef struct {
    int32	cnt;                    /* number of measurements that follow */
    CharMeasurement_t	item[1];	/* cnt measurements */
} LMeas, *LMeasPtr, **LMeasHandle;

typedef struct {
    int32	cnt;                        /* number of ranged measurements that follow */
    CharRangedMeasurement_t	item[1];	/* cnt ranged measurements */
} LRange, *LRangePtr, **LRangeHandle;

typedef struct {
    int32	cnt;                            /* number of compensation controls that follow */
    CharCompensationControl_t	item[1];	/* cnt compensation control */
} LComp, *LCompPtr, **LCompHandle;

#define LVecBuf(sp)	(&((sp))->item[0])				/* pointer to first item of vector */
#define LVecItem(sp, n)	((&((sp))->item[n]))		/* pointer to n-th item of vector */
#define LVecLen(sp)	(((sp))->cnt)					/* # of items in vector */
#include "lv_epilog.h"
#endif

#ifndef E384CL_LABVIEW_COMPATIBILITY
} // namespace e384CommLib
#else
// typedef LStrHandle E384clString_t;
// typedef LStrHandle E384clStringVector_t;
// typedef CharMeasurement_t E384clMeasurement_t;
// typedef LMeasHandle E384clMeasurementVector_t;
// typedef CharRangedMeasurement_t E384clRangedMeasurement_t;
// typedef LRangeHandle E384clRangedMeasurementVector_t;
// typedef CharCompensationControl_t E384clCompensationControl_t;
// typedef uint16_t E384clUint16Vector_t;
// typedef uint32_t E384clUint32Vector_t;
// typedef double E384clDoubleVector_t;
// typedef bool E384clBoolVector_t;
// #define E384CL_OUTPUT_SYMBOL
// #define E384CL_VECTOR_SYMBOL *
#endif

#endif // E384COMMLIB_GLOBAL_H
