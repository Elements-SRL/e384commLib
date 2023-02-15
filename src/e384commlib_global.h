/*! \file e4gcommlib_global.h
 * \brief Defines global macros and typedefs.
 * \note \a E4GCOMMLIB_LIBRARY should be defined only during the library compilation.
 * Defining \a E4GCOMMLIB_LIBRARY when the library is included in a project will prevent project building.
 */
#ifndef E4GCOMMLIB_GLOBAL_H
#define E4GCOMMLIB_GLOBAL_H

#include <vector>
#include <string>
#include <math.h>
#include <limits>

/****************************\
 *  Shared library defines  *
\****************************/

#if defined(_WIN32)

#if defined(E4GCOMMLIB_STATIC)
#  define E4GCOMMLIBSHARED_EXPORT
#else
#if defined(E4GCOMMLIB_LIBRARY)
#  define E4GCOMMLIBSHARED_EXPORT __declspec(dllexport)
#else
#  define E4GCOMMLIBSHARED_EXPORT __declspec(dllimport)
#endif
#endif

#elif defined(__APPLE__)

#if defined(E4GCOMMLIB_STATIC)
#  define E4GCOMMLIBSHARED_EXPORT
#else
#if defined(E4GCOMMLIB_LIBRARY)
#  define E4GCOMMLIBSHARED_EXPORT __attribute__((visibility("default")))
#else
#  define E4GCOMMLIBSHARED_EXPORT __attribute__((visibility("default")))
#endif
#endif

#else // All posix systems including linux

#if defined(E4GCOMMLIB_LIBRARY)
#  define E4GCOMMLIBSHARED_EXPORT
#else
#  define E4GCOMMLIBSHARED_EXPORT
#endif

#endif

/**************************\
 *  Arguments qualifiers  *
\**************************/

/*! \def E4GCL_ARGIN
 * \brief Dummy define to identify input arguments.
 */
#define E4GCL_ARGIN

/*! \def E4GCL_ARGOUT
 * \brief Dummy define to identify output arguments.
 */
#define E4GCL_ARGOUT

/*! \def E4GCL_ARGVOID
 * \brief Dummy define to identify void arguments.
 */
#define E4GCL_ARGVOID void

namespace e4gCommLib {

/**********************\
 *  Message type IDs  *
\**********************/

/*! \enum MsgDirection_t
 * \brief Enumerates the directions of the communication channel.
 */
typedef enum {
    MsgDirectionEdrToDevice =           0x0000, /*!< Indicates messages going from library caller to the device. */
    MsgDirectionDeviceToEdr =           0x8000  /*!< Indicates messages going from library caller to the device. */
} MsgDirection_t;

/*! \enum MsgGroup_t
 * \brief Enumerates groups of messages.
 */
typedef enum {
    MsgGroupCommunication =             0x0000, /*!< Messages regarding communication. */
    MsgGroupAcquisitionConfiguration =  0x0400, /*!< Messages regarding acquisition configuration. */
    MsgGroupHwConfiguration =           0x0800, /*!< Messages regarding hardware configuration. */
    MsgGroupVoltageTrials =             0x0C00, /*!< Messages regarding voltage protocols. */
    MsgGroupCurrentTrials =             0x1000, /*!< Messages regarding current protocols. */
    MsgGroupAcquiredData =              0x1400, /*!< Messages regarding acquired data. */
    MsgGroupFeatures =                  0x1800, /*!< Messages regarding device's features. */
    MsgGroupCalibrationUtilities =      0x7C00  /*!< Messages used for calibrating the device. */
} MsgGroup_t;

/*! \enum MsgTypeId_t
 * \brief Enumerates the messages IDs.
 */
typedef enum {
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
} MsgTypeId_t;

/********************\
 *  Other typedefs  *
\********************/

/*! \def E4GCL_VOLTAGE_CLAMP_MODE
 * \brief Index that identifies voltage clamp modality.
 */
#define E4GCL_VOLTAGE_CLAMP_MODE 0

/*! \def E4GCL_ZERO_CURRENT_CLAMP_MODE
 * \brief Index that identifies zero current clamp modality.
 */
#define E4GCL_ZERO_CURRENT_CLAMP_MODE 1

/*! \def E4GCL_CURRENT_CLAMP_MODE
 * \brief Index that identifies current clamp modality.
 */
#define E4GCL_CURRENT_CLAMP_MODE 2

/*! \def E4GCL_DYNAMIC_CLAMP_MODE
 * \brief Index that identifies dynamic clamp modality.
 */
#define E4GCL_DYNAMIC_CLAMP_MODE 3

/*! \enum DeviceTypes_t
 * \brief Enumerates the device types that can be handled by e4gCommLib.
 */
typedef enum {
    DeviceEPatchEL03D,          /*!< ePatch device with EL03D chip. */
    DeviceEPatchEL03D_V01,      /*!< ePatch device with EL03D chip: before voltage hold tuner implementation. */
    DeviceEPatchEL03D_V00,      /*!< ePatch device with EL03D chip: before digital trigger output implementation. */
    DeviceEPatchEL03F_4D,       /*!< ePatch device with EL03F and EL04D chips. */
    DeviceEPatchEL03F_4D_V02,   /*!< ePatch device with EL03F and EL04D chips: before master/slave configurability. */
    DeviceEPatchEL03F_4D_V01,   /*!< ePatch device with EL03F and EL04D chips: before voltage and current hold tuner implementation. */
    DeviceEPatchEL03F_4D_V00,   /*!< ePatch device with EL03F and EL04D chips: before digital trigger output implementation. */
    DeviceEPatchEL04E,          /*!< ePatch device with EL04E chip. */
    DeviceEPatchEL03F_4E,       /*!< ePatch device with EL03F and EL04E chips. */
    DeviceEPatchEL03F_4E_V02,   /*!< ePatch device with EL03F and EL04E chips: before master/slave configurability. */
    DeviceEPatchEL03F_4E_V01,   /*!< ePatch device with EL03F and EL04E chips: before voltage and current hold tuner implementation. */
    DeviceEPatchEL03F_4E_V00,   /*!< ePatch device with EL03F and EL04E chips: before digital trigger output implementation. */
    DeviceEPatchEL04F,          /*!< ePatch device with EL04F chip. */
    DeviceE4PEL04F,             /*!< ePatch device with 4 EL04F chips. */
    DeviceE4PPatchLiner,        /*!< ePatch device with 4 EL04F chips for Nanion's Patchliner. */
    DeviceE8PPatchLiner,        /*!< ePatch device with 8 EL04F chips for Nanion's Patchliner. */
    DeviceE4PPatchLinerEL07AB,  /*!< ePatch device with 4 EL07AB chips for Nanion's Patchliner. */
    DeviceE8PPatchLinerEL07AB,  /*!< ePatch device with 8 EL07AB chips for Nanion's Patchliner. */
    DeviceFakePatch,            /*!< Fake ePatch device resembling EL04F chip. */
    DeviceFakeP8,               /*!< Fake patchliner device resembling EL07AB chip. */
    DeviceEPatchDlp,            /*!< ePatch device with dlp fpga. */
    DeviceUnknown,              /*!< Invalid item used only for initiliazation purposes. */
    DevicesNum
} DeviceTypes_t;

/*! \def E4GCL_OUT_STRUCT_UINT_DATA_LEN
 * \brief Max size of unsigned integers returned by struct e4gCommLib::RxOutput_t.
 */
#define E4GCL_OUT_STRUCT_UINT_DATA_LEN 0x10

/*! \def E4GCL_OUT_STRUCT_DATA_LEN
 * \brief Max size of double precision numbers returned by struct e4gCommLib::RxOutput_t.
 */
#define E4GCL_OUT_STRUCT_DATA_LEN 0x1000

/*! \struct RxOutput_t
 * \brief Structure used to return to the caller data and information received from the device.
 */
typedef struct {
    uint16_t msgTypeId = MsgDirectionDeviceToEdr+MsgTypeIdInvalid; /*!< Type of message received. */
    uint16_t channelIdx = 0; /*!< For msgTypeId that work channel-wise this field holds the channel index the message refers too, e.g. the compensated channel after digital compensation */
    uint16_t protocolId = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the protocol identifier number */
    uint16_t protocolItemIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the item index of the following data in the current protocol */
    uint16_t protocolRepsIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the iteration of an interal loop of the following data in the current protocol */
    uint16_t protocolSweepIdx = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionHeader this field holds the sweep index of the following data in the current protocol */
    uint32_t firstSampleOffset = 0; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionData this field holds the data offset wrt the protocol first sample */
    uint16_t dataLen = 0; /*!< Number of data samples available in field data */
    uint16_t uintData[E4GCL_OUT_STRUCT_UINT_DATA_LEN]; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionSaturation this field holds the index of the saturated channels:
                                                        *   Each channel is associated a bit, which is high if the channel is saturated. */
    double data[E4GCL_OUT_STRUCT_DATA_LEN]; /*!< When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdAcquisitionData this field holds the voltage and current data.
                                             *   Data samples are organized in time samples, so if the device has 2 voltage channels V[ch, t] and 3 current channels I[ch, t] data will be:
                                             *   V[0, 0], V[1, 0], I[0, 0], I[1, 0], I[2, 0], V[0, 1], V[1, 1], I[0, 1], ...
                                             *   When #msgTypeId is MsgDirectionDeviceToEdr + MsgTypeIdDigitalOffsetComp or MsgDirectionDeviceToEdr + MsgTypeIdDigitalOffsetCompInquiry, data will contain only one float
                                             *   Corresponding to the voltage of the liquid junction.
                                             *   Data are always expressed in the same unit as the last received CommLib::setVoltageRange(uint16_t) and CommLib::setCurrentRange(uint16_t) commands. */
} RxOutput_t;

/*! \enum UnitPfx_t
 * \brief Enumerates the unit prefixes used.
 */
typedef enum {
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

/*! \brief Increments a prefix by 1.
 *
 * \param value [out] incremented prefix.
 * \return true if the prefix was successfully incremented; false if the prefix has already the maximum possible value.
 */
static inline bool incrementUnit(E4GCL_ARGOUT UnitPfx_t &value);

/*! \brief Increments a prefix by n.
 *
 * \param value [out] incremented prefix.
 * \param n [in] increment.
 * \return true if the prefix was successfully incremented; false if the prefix surpasses the maximum possible value.
 */
static inline bool incrementUnit(E4GCL_ARGOUT UnitPfx_t &value, E4GCL_ARGIN int n);

/*! \brief Decrements a prefix by 1.
 *
 * \param value [out] decremented prefix.
 * \return true if the prefix was successfully decremented; false if the prefix has already the minimum possible value.
 */
static inline bool decrementUnit(E4GCL_ARGOUT UnitPfx_t &value);

/*! \brief Decrements a prefix by n.
 *
 * \param value [out] decremented prefix.
 * \param n [in] decrement.
 * \return true if the prefix was successfully decremented; false if the prefix surpasses the minimum possible value.
 */
static inline bool decrementUnit(E4GCL_ARGOUT UnitPfx_t &value, E4GCL_ARGIN int n);

static inline bool incrementUnit(E4GCL_ARGOUT UnitPfx_t &value) {
    if (value < UnitPfxNum-1) {
        int intValue = static_cast <int> (value) +1;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        return false;
    }
}

static inline bool incrementUnit(E4GCL_ARGOUT UnitPfx_t &value, E4GCL_ARGIN int n) {
    if (value < UnitPfxNum-n) {
        int intValue = static_cast <int> (value) +n;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        value = UnitPfxNum;
        decrementUnit(value);
        return false;
    }
}

static inline bool decrementUnit(E4GCL_ARGOUT UnitPfx_t &value) {
    if (value > 0) {
        int intValue = static_cast <int> (value) -1;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        return false;
    }
}

static inline bool decrementUnit(E4GCL_ARGOUT UnitPfx_t &value, E4GCL_ARGIN int n) {
    if (value > n-1) {
        int intValue = static_cast <int> (value) -n;
        value = static_cast <UnitPfx_t> (intValue);
        return true;

    } else {
        value = static_cast <UnitPfx_t> (0);
        return false;
    }
}

/*! \brief Array with the strings corresponding to the unit prefixes of type UnitPfx_t. */
static const std::string unitPrefixes[UnitPfxNum] = {
    "f",
    "p",
    "n",
    "u",
    "m",
    " ",
    "k",
    "M",
    "G",
    "T",
    "P"
};

/*! \brief Convenience array with the precomputed powers of 1000 used to make unit conversions. */
static const double powersOf1000[UnitPfxNum] = {
    1.0,
    1.0e3,
    1.0e6,
    1.0e9,
    1.0e12,
    1.0e15,
    1.0e18,
    1.0e21,
    1.0e24,
    1.0e27,
    1.0e30
};

/*! \typedef Measurement_t
 */

/*! \struct Measurement_t
 * \brief Structure used manage physical quantities that define a value with its unit and unit prefix.
 */
typedef struct Measurement {
    double value; /*!< Numerical value. */
    UnitPfx_t prefix; /*!< Unit prefix in the range [femto, Peta]. */
    std::string unit; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */

    /*! \brief Returns the value without prefix.
     *
     * \return Value without prefix, e.g. for 1.5nA returns 1.5e-9.
     */
    double getNoPrefixValue(E4GCL_ARGVOID) {
        return value*multiplier();
    }

    /*! \brief Returns the string corresponding to the prefix.
     *
     * \return String corresponding to the prefix.
     */
    std::string getPrefix(E4GCL_ARGVOID) {
        return unitPrefixes[prefix];
    }

    /*! \brief Returns the string corresponding to the unit with the prefix.
     *
     * \return String corresponding to the unit with the prefix.
     */
    std::string getFullUnit(E4GCL_ARGVOID) {
        return unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the prefix multiplier.
     *
     * \return Prefix multiplier, e.g. 1e-6 for micro.
     */
    double multiplier(E4GCL_ARGVOID) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > UnitPfxNone) {
            delta = prefix-UnitPfxNone;
            deltaPositive = true;

        } else {
            delta = UnitPfxNone-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            return powersOf1000[delta];

        } else {
            return 1.0/powersOf1000[delta];
        }
    }

    /*! \brief Returns a string describing the value.
     *
     * \param maxChars [in] maximum number of characters for the returned string.
     * \return String describing the value.
     */
    std::string valueLabel(E4GCL_ARGIN unsigned int maxChars = 8) {
        std::string valueSt = std::to_string(value);
        if (valueSt.length() >= maxChars) {
            valueSt.erase(maxChars);
        }
        size_t dot = valueSt.find_last_of(".");
        size_t notZero = valueSt.find_last_not_of("0");

        if (notZero != std::string::npos) {
            if (dot != std::string::npos) {
                return valueSt.erase(dot < notZero ? notZero+1 : dot);

            } else {
                return valueSt;
            }

        } else {
            return "0";
        }
    }

    /*! \brief Returns a string describing the value with its prefix and unit.
     *
     * \param maxChars [in] maximum number of characters for the returned string.
     * \return String describing the value with its prefix and unit.
     */
    std::string label(E4GCL_ARGIN unsigned int maxChars = 8) {
        return valueLabel(maxChars) + " " + unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the string describing the value with its prefix and unit in a nice fashion.
     *
     * \return String describing the value with its prefix and unit. The value and the prefix are converted so that the value is in the range [1.0, 1000.0[.
     */
    std::string niceLabel(E4GCL_ARGVOID) {
        Measurement temp;
        temp.value = value;
        temp.prefix = prefix;
        temp.unit = unit;
        temp.nice();

        return temp.label(5);
    }

    /*! \brief Converts #value given the input unit prefix.
     *
     * \param newPrefix [in] Desired unit prefix.
     */
    void convertValue(E4GCL_ARGIN UnitPfx_t newPrefix) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > newPrefix) {
            delta = prefix-newPrefix;
            deltaPositive = true;

        } else {
            delta = newPrefix-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            value *= powersOf1000[delta];

        } else {
            value /= powersOf1000[delta];
        }
        prefix = newPrefix;
    }

    /*! \brief Converts #value given the input unit multiplier.
     *
     * \param newMultiplier [in] Desired unit multiplier.
     */
    void convertValue(E4GCL_ARGIN double newMultiplier) {
        double multiplier = this->multiplier();
        double gain;
        bool gainPositive;

        if (multiplier > newMultiplier) {
            gain = multiplier/newMultiplier;
            gainPositive = true;

        } else {
            gain = newMultiplier/multiplier;
            gainPositive = false;
        }

        double diff;
        double minDiff = std::numeric_limits <double>::max();
        unsigned int minDelta = 0;
        for (unsigned int delta = 0; delta < UnitPfxNum; delta++) {
            diff = fabs(powersOf1000[delta]-gain);
            if (diff < minDiff) {
                minDiff = diff;
                minDelta = delta;
            }
        }

        if (gainPositive) {
            value *= powersOf1000[minDelta];
            decrementUnit(prefix, (int)minDelta);

        } else {
            value /= powersOf1000[minDelta];
            incrementUnit(prefix, (int)minDelta);
        }
    }

    /*! \brief Converts #value and #prefix in order to have a final #value in range [1.0, 1000.0[.
     */
    void nice(E4GCL_ARGVOID) {
        if ((value == 0.0) || isinf(value)) {
            prefix = UnitPfxNone;

        } else {
            while (fabs(value) >= 1000.0) {
                if (incrementUnit(prefix)) {
                    value /= 1000.0;

                } else {
                    break;
                }
            }

            while (fabs(value) < 1.0) {
                if (decrementUnit(prefix)) {
                    value *= 1000.0;

                } else {
                    break;
                }
            }
        }
    }
} Measurement_t;

/*! \brief Overloaded equality check for #Measurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have same value, prefix and unit.
*/
inline bool operator == (const Measurement_t &a, const Measurement_t &b) {
    return ((a.value == b.value) && (a.prefix == b.prefix) && (a.unit == b.unit));
}

/*! \brief Overloaded inequality check for #Measurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have different value, prefix or unit.
*/
inline bool operator != (const Measurement_t &a, const Measurement_t &b) {
    return !(a == b);
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is smaller than \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator < (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value < (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) < b.value;
        }
    }
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is smaller than or equal to \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator <= (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value <= (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) <= b.value;
        }
    }
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is greater than \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator > (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value > (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) > b.value;
        }
    }
}

/*! \brief Overloaded inequality check for #Measurement_t.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a is greater than or equal to \p b after they have been converted to have the same prefix. \note Returns false if \p a and \p b have different units.
*/
inline bool operator >= (const Measurement_t &a, const Measurement_t &b) {
    if (a.unit != b.unit) {
        return false;

    } else {
        /*! Not using convertValue method to avoid changing the input structures */
        if (a.prefix < b.prefix) {
            return a.value >= (b.value*powersOf1000[b.prefix-a.prefix]);

        } else {
            return (a.value*powersOf1000[a.prefix-b.prefix]) >= b.value;
        }
    }
}

/*! \brief Overloaded sum for #Measurement_t.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the sum of the values of the operands converted to the prefix of the first operand, and the unit equals the unit of the second operand.
 * \note This method assumes the units are compatible and won't check for the sake of speed.
*/
inline Measurement_t operator + (const Measurement_t &a, const Measurement_t &b) {
    Measurement_t c = b;
    c.convertValue(a.prefix);
    c.value += a.value;
    return c;
}

/*! \brief Overloaded subtraction for #Measurement_t.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the difference of the values of the operands converted to the prefix of the first operand, and the unit equals the unit of the second operand.
 * \note This method assumes the units are compatible and won't check for the sake of speed.
*/
inline Measurement_t operator - (const Measurement_t &a, const Measurement_t &b) {
    Measurement_t c = b;
    c.convertValue(a.prefix);
    c.value = a.value-c.value;
    return c;
}

/*! \brief Overloaded multiplication between #Measurement_t and a constant.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the product of the values and the unit equals the unit of the first operand.
*/
template <class T>
inline Measurement_t operator * (const Measurement_t &a, const T &b) {
    Measurement_t c = a;
    c.value *= (double)b;
    return c;
}

/*! \brief Overloaded multiplication between #Measurement_t and a constant.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the product of the values and the unit equals the unit of the second operand.
*/
template <class T>
inline Measurement_t operator * (const T &a, const Measurement_t &b) {
    return b*a;
}

/*! \brief Overloaded division between #Measurement_t and a constant.
 *
 * \param a [in] First operand.
 * \param b [in] Second operand.
 * \return A #Measurement_t whose value is the ratio of the values and the unit equals the unit of the first operand.
*/
template <class T>
inline Measurement_t operator / (const Measurement_t &a, const T &b) {
    Measurement_t c = a;
    c.value /= (double)b;
    return c;
}

/*! \struct RangedMeasurement_t
 * \brief Structure used manage physical ranges that define a range with its unit and unit prefix.
 */
typedef struct {
    double min; /*!< Minimum value. */
    double max; /*!< Maximum value. */
    double step; /*!< Resolution. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    std::string unit = ""; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */

    /*! \brief Returns the number of steps in the range.
     *
     * \return Number of steps in the range.
     */
    uint32_t steps(E4GCL_ARGVOID) {
        return static_cast <uint32_t> (round(1.0+(max-min)/step));
    }

    /*! \brief Returns the string corresponding to the prefix.
     *
     * \return String corresponding to the prefix.
     */
    std::string getPrefix(E4GCL_ARGVOID) {
        return unitPrefixes[prefix];
    }

    /*! \brief Returns the string corresponding to the unit with the prefix.
     *
     * \return String corresponding to the unit with the prefix.
     */
    std::string getFullUnit(E4GCL_ARGVOID) {
        return unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the prefix multiplier.
     *
     * \return Prefix multiplier, e.g. 10^-6 for micro.
     */
    double multiplier(E4GCL_ARGVOID) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > UnitPfxNone) {
            delta = prefix-UnitPfxNone;
            deltaPositive = true;

        } else {
            delta = UnitPfxNone-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            return powersOf1000[delta];

        } else {
            return 1.0/powersOf1000[delta];
        }
    }

    /*! \brief Returns a string describing the max value.
     *
     * \param maxChars [in] maximum number of characters for the returned string.
     * \return String describing the max value.
     */
    std::string valueLabel(E4GCL_ARGIN unsigned int maxChars = 8) {
        std::string valueSt = std::to_string(max);
        if (valueSt.length() >= maxChars) {
            valueSt.erase(maxChars);
        }
        size_t dot = valueSt.find_last_of(".");
        size_t notZero = valueSt.find_last_not_of("0");

        if (notZero != std::string::npos) {
            if (dot != std::string::npos) {
                return valueSt.erase(dot < notZero ? notZero+1 : dot);

            } else {
                return valueSt;
            }

        } else {
            return "0";
        }
    }

    /*! \brief Returns a string describing the max value with its prefix and unit.
     *
     * \param maxChars [in] maximum number of characters for the returned string.
     * \return String describing the max value with its prefix and unit.
     */
    std::string label(E4GCL_ARGIN unsigned int maxChars = 8) {
        return valueLabel(maxChars) + " " + unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the string describing the max value with its prefix and unit in a nice fashion.
     *
     * \return String describing the max value with its prefix and unit. The max value and the prefix are converted so that the value is in the range [1.0, 1000.0[.
     */
    std::string niceLabel(E4GCL_ARGVOID) {
        Measurement temp;
        temp.value = max;
        temp.prefix = prefix;
        temp.unit = unit;
        temp.nice();

        return temp.label(5);
    }

    /*! \brief Converts #min, #max and #step given the input unit prefix.
     *
     * \param newPrefix [in] Desired unit prefix.
     */
    void convertValues(E4GCL_ARGIN UnitPfx_t newPrefix) {
        unsigned int delta;
        bool deltaPositive;

        if (prefix > newPrefix) {
            delta = prefix-newPrefix;
            deltaPositive = true;

        } else {
            delta = newPrefix-prefix;
            deltaPositive = false;
        }

        if (deltaPositive) {
            min *= powersOf1000[delta];
            max *= powersOf1000[delta];
            step *= powersOf1000[delta];

        } else {
            min /= powersOf1000[delta];
            max /= powersOf1000[delta];
            step /= powersOf1000[delta];
        }
        prefix = newPrefix;
    }

    /*! \brief Converts #min, #max and #step given the input unit multiplier.
     *
     * \param newMultiplier [in] Desired unit multiplier.
     */
    void convertValues(E4GCL_ARGIN double newMultiplier) {
        double multiplier = this->multiplier();
        double gain;
        bool gainPositive;

        if (multiplier > newMultiplier) {
            gain = multiplier/newMultiplier;
            gainPositive = true;

        } else {
            gain = newMultiplier/multiplier;
            gainPositive = false;
        }

        double diff;
        double minDiff = std::numeric_limits <double>::max();
        unsigned int minDelta = 0;
        for (unsigned int delta = 0; delta < UnitPfxNum; delta++) {
            diff = fabs(powersOf1000[delta]-gain);
            if (diff < minDiff) {
                minDiff = diff;
                minDelta = delta;
            }
        }

        if (gainPositive) {
            min *= powersOf1000[minDelta];
            max *= powersOf1000[minDelta];
            step *= powersOf1000[minDelta];
            decrementUnit(prefix, (int)minDelta);

        } else {
            min /= powersOf1000[minDelta];
            max /= powersOf1000[minDelta];
            step /= powersOf1000[minDelta];
            incrementUnit(prefix, (int)minDelta);
        }
    }

    /*! \brief Returns the range width.
     *
     * \return Difference between max and min.
     */
    double delta(E4GCL_ARGVOID) {
        return max-min;
    }

    /*! \brief Returns a reasonable amount of decimals to represent the values in the range.
     *
     * \return A reasonable amount of decimals to represent the values in the range.
     */
    int decimals(E4GCL_ARGVOID) {
        int decimals = 0;
        double temp = step;
        while ((fabs(temp-round(temp)) > 0.05 || temp < 1.0) &&
               decimals < 3) { /*!< \todo Rather than asking 3 decimals better asking for 3 digits */
            decimals++;
            temp *= 10.0;
        }
        return decimals;
    }

    /*! \brief Returns a Measurement_t equivalent to the max value of the range.
     *
     * \return Measurement_t equivalent to the max value of the range.
     */
    Measurement_t getMax(E4GCL_ARGVOID) {
        Measurement_t extreme;
        extreme.value = max;
        extreme.prefix = prefix;
        extreme.unit = unit;
        return extreme;
    }

    /*! \brief Returns a Measurement_t equivalent to the min value of the range.
     *
     * \return Measurement_t equivalent to the min value of the range.
     */
    Measurement_t getMin(E4GCL_ARGVOID) {
        Measurement_t extreme;
        extreme.value = min;
        extreme.prefix = prefix;
        extreme.unit = unit;
        return extreme;
    }

    /*! \brief Returns a Measurement_t equivalent to the x-th step of the range.
     *
     * \return Measurement_t equivalent to the x-th step of the range.
     */
    Measurement_t getXth(E4GCL_ARGIN uint32_t x) {
        Measurement_t xth;
        xth.value = min+step*static_cast <double> (x);
        xth.prefix = prefix;
        xth.unit = unit;
        return xth;
    }
} RangedMeasurement_t;

/*! \brief Overloaded equality check for #RangedMeasurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have same value, prefix and unit.
*/
inline bool operator == (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return ((a.min == b.min) && (a.max == b.max) && (a.step == b.step) && (a.prefix == b.prefix) && (a.unit == b.unit));
}

/*! \brief Overloaded inequality check for #RangedMeasurement_t. \note No conversion is performed, cause the multiplication can introduce rounding errors.
 *
 * \param a [in] First item of the comparison.
 * \param b [in] Second item of the comparison.
 * \return true if \p a and \p b have different value, prefix or unit.
*/
inline bool operator != (const RangedMeasurement_t &a, const RangedMeasurement_t &b) {
    return !(a == b);
}

/*! \struct ChannelSources_t
 * \brief Structure used to return available data sources for a channel.
 * \note -1 means that the source is not available.
 */
typedef struct {
    int16_t VoltageFromVoltageClamp = -1; /*!< Get voltage applied by voltage clamp front-end. */
    int16_t CurrentFromVoltageClamp = -1; /*!< Get current read by voltage clamp front-end. */
    int16_t VoltageFromCurrentClamp = -1; /*!< Get voltage read by current clamp front-end. */
    int16_t CurrentFromCurrentClamp = -1; /*!< Get current applied by current clamp front-end. */
    int16_t VoltageFromDynamicClamp = -1; /*!< Get voltage computed by dynamic clamp. */
    int16_t CurrentFromDynamicClamp = -1; /*!< Get current computed by current clamp. */
    int16_t VoltageFromVoltagePlusDynamicClamp = -1; /*!< Get voltage applied by voltage clamp front-end plus voltage computed by dynamic clamp. */
    int16_t CurrentFromCurrentPlusDynamicClamp = -1; /*!< Get current applied by current clamp front-end plus current computed by dynamic clamp. */
} ChannelSources_t;

/*! \struct CompensationControl_t
 * \brief Structure used to return detailed information on a specific compensation implemented by the HW.
 */
typedef struct {
    bool implemented = false; /*!< True if the corresponding compensation is implemented by the device. */
    double min = 0.0; /*!< Minimum compensable value. */
    double max = 1.0; /*!< Maximum compensable value globally. */
    double compensable = 1.0; /*!< Maximum compensable value given also the value of the other compensations. */
    double steps = 2; /*!< Number of steps between #min and #max. */
    double step = 1.0; /*!< Resolution. */
    int decimals = 0; /*!< Decimals to represent the compensated value. */
    double value = 0.0; /*!< Compensated value. */
    UnitPfx_t prefix = UnitPfxNone; /*!< Unit prefix in the range [femto, Peta]. */
    std::string unit = ""; /*!< Unit. \note Can be any string, the library is not aware of real units meaning. */
    std::string name = ""; /*!< Name of the compensation. */

    /*! \brief Returns the string corresponding to the prefix.
     *
     * \return String corresponding to the prefix.
     */
    std::string getPrefix(E4GCL_ARGVOID) {
        return unitPrefixes[prefix];
    }

    /*! \brief Returns the string corresponding to the unit with the prefix.
     *
     * \return String corresponding to the unit with the prefix.
     */
    std::string getFullUnit(E4GCL_ARGVOID) {
        return unitPrefixes[prefix] + unit;
    }

    /*! \brief Returns the string describing the compensation with its prefix and unit.
     *
     * \return String describing the compensation with its prefix and unit.
     */
    std::string title(E4GCL_ARGVOID) {
        if (unit != "") {
            return name + " [" + unitPrefixes[prefix] + unit + "]";

        } else {
            return name;
        }
    }
} CompensationControl_t;

typedef struct {
    uint32_t address;
    uint32_t bitSize;
    bool signedFlag;
    uint32_t byteSize;
    double lsb;
} CalibrationValueInfo_t;

typedef struct {
    uint32_t adcRangeIdx;
    uint32_t dacRangeIdx;
    uint32_t samplingRateIdx;
    RangedMeasurement_t dacRange;
    Measurement_t resistance;
    bool capacitanceFlag;
    uint32_t csvColumn;
} CharacterizationSetting_t;

typedef struct {
    uint32_t calibrationSamplingRatesNum;
    std::vector <uint32_t> calibrationSamplingRates;
    bool vcVGainRdac;
    uint32_t ccVGainRepeat;
    std::vector <RangedMeasurement_t> vcVGainRanges;
    std::vector <RangedMeasurement_t> vcVOffsetRanges;
    std::vector <RangedMeasurement_t> ccVGainRanges;
    std::vector <RangedMeasurement_t> ccVOffsetRanges;
    std::vector <RangedMeasurement_t> vcIGainRanges;
    std::vector <RangedMeasurement_t> vcIOffsetRanges;
    std::vector <RangedMeasurement_t> ccIGainRanges;
    std::vector <RangedMeasurement_t> ccIOffsetRanges;
    std::vector <Measurement_t> vcVGainResistances;
    std::vector <Measurement_t> vcVOffsetResistances;
    std::vector <Measurement_t> ccVGainResistances;
    std::vector <Measurement_t> ccVOffsetResistances;
    std::vector <Measurement_t> vcIGainResistances;
    std::vector <Measurement_t> vcIOffsetResistances;
    std::vector <Measurement_t> ccIGainResistances;
    std::vector <Measurement_t> ccIOffsetResistances;
    std::vector <CalibrationValueInfo_t> vcVGains;
    std::vector <CalibrationValueInfo_t> vcVOffsets;
    std::vector <CalibrationValueInfo_t> ccVGains;
    std::vector <CalibrationValueInfo_t> ccVOffsets;
    std::vector <CalibrationValueInfo_t> vcIGains;
    std::vector <CalibrationValueInfo_t> vcIOffsets;
    std::vector <CalibrationValueInfo_t> ccIGains;
    std::vector <CalibrationValueInfo_t> ccIOffsets;
    uint32_t characterizationCsvColumns;
    uint32_t vcGainCharacterizationStepsNum;
    uint32_t vcOffsetCharacterizationStepsNum;
    uint32_t ccGainCharacterizationStepsNum;
    uint32_t ccOffsetCharacterizationStepsNum;
    uint32_t i0OffsetCharacterizationStepsNum;
    std::vector <CharacterizationSetting_t> vcGainCharSettings;
    std::vector <CharacterizationSetting_t> vcOffsetCharSettings;
    std::vector <CharacterizationSetting_t> ccGainCharSettings;
    std::vector <CharacterizationSetting_t> ccOffsetCharSettings;
    std::vector <CharacterizationSetting_t> i0OffsetCharSettings;
} CalibrationConfiguration_t;

} // namespace e4gCommLib

#endif // E4GCOMMLIB_GLOBAL_H
