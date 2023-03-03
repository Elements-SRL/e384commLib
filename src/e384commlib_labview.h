/*! \file e384commlib_labview.h
 * \brief Declares class CommLib.
 */
#ifndef E384COMMLIB_LABVIEW_H
#define E384COMMLIB_LABVIEW_H

#ifndef E384CL_LABVIEW_COMPATIBILITY
#include <vector>
#include <string>
#endif

#include "e384commlib_global.h"
#include "e384commlib_errorcodes.h"

class MessageDispatcher;

#ifndef E384CL_LABVIEW_COMPATIBILITY
namespace e384CommLib {
#endif

E384COMMLIBSHARED_EXPORT

/*******************\
 *  Init / Deinit  *
\*******************/

/*! \brief Initialize the communication library.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t init(
        E384CL_ARGVOID);

/*! \brief Deinitialize the communication library.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t deinit(
        E384CL_ARGVOID);

/************************\
 *  Connection methods  *
\************************/

/*! \brief Detects plugged in devices.
 *
 * \param deviceIds [out] List of plugged in devices IDs.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t detectDevices(
        E384CL_ARGOUT LStrHandle deviceIds);

/*! \brief Connects to a specific device
 * Calling this method if a device is already connected will return an error code.
 *
 * \param deviceId [in] Device ID of the device to connect to.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t connectDevice(
        E384CL_ARGIN LStrHandle deviceId);

/*! \brief Disconnects from connected device.
 * Calling this method if no device is connected will return an error code.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t disconnectDevice(
        E384CL_ARGVOID);

/****************\
 *  Tx methods  *
\****************/

/*! \brief Pings the connected device.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t ping(
        E384CL_ARGVOID);

/*! \brief Turn on/off the voltage stimulus for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the voltage stimulus on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnVoltageStimulusOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off the current stimulus for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the current stimulus on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCurrentStimulusOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off the voltage reader for each channel.
 *  \note The voltage is read by the current clamp ADC.
 *  \note In some devices the ADC can't be turned on independently of the DAC.
 *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
 *
 * \param  onValueIn [in] Boolean, one for each channel: True to turn the voltage reader on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnVoltageReaderOn(
        E384CL_ARGIN bool * onValueIn,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Turn on/off the current reader for each channel.
 *  \note The current is read by the current clamp ADC.
 *  \note In some devices the ADC can't be turned on independently of the DAC.
 *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
 *
 * \param onValueIn [in] Boolean, one for each channel: True to turn the current reader on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCurrentReaderOn(
        E384CL_ARGIN bool * onValueIn,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Set the data sources for all channels.
 *
 * \param sourcesIdxs [in] Indexes of the data sources to be set for voltage channels.
 * \param sourcesIdxs [in] Indexes of the data sources to be set for current channels.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setChannelsSources(
        E384CL_ARGIN int16_t voltageSourcesIdx,
        E384CL_ARGIN int16_t currentSourcesIdx);

/*! \brief Set the holding current tuner. This value is added to the whole current protocol currently applied and to the following.
 *
 * \param channelIdx [in] Channel to apply holding current to.
 * \param current [in] Holding current that is added to the whole current protocol.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCurrentHoldTuner(
        E384CL_ARGIN uint16_t channelIdx,
        E384CL_ARGIN CharMeasurement_t current);

/*! \brief Turns on/off a uniform noise additive noise with the same range as the LSB on received data.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  flagValuesIn [in] Array of booleans, one for each channel: True to turn on LSB additive noise, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnOnLsbNoise(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * flagValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the current range for voltage clamp.
 *
 * \param currentRangeIdx [in] Index of the current range to be set.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVCCurrentRange(
        E384CL_ARGIN uint16_t currentRangeIdx,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Set the current range for current clamp.
 *
 * \param currentRangeIdx [in] Index of the current range to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCCCurrentRange(
        E384CL_ARGIN uint16_t currentRangeIdx);

/*! \brief Set the voltage range for voltage clamp.
 *
 * \param voltageRangeIdx [in] Index of the voltage range to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVCVoltageRange(
        E384CL_ARGIN uint16_t voltageRangeIdx);

/*! \brief Set the voltage range for current clamp.
 *
 * \param voltageRangeIdx [in] Index of the voltage range to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCCVoltageRange(
        E384CL_ARGIN uint16_t voltageRangeIdx);

/*! \brief Set the sampling rate.
 *
 * \param samplingRateIdx [in] Index of the sampling rate to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setSamplingRate(
        E384CL_ARGIN uint16_t samplingRateIdx);

/*! \brief Set a digital filter.
 *
 * \param cutoffFrequency [in] The cut-off frequency in kHz of the filter.
 * \param lowPassFlag [in] true: set a low pass filter; false: set a high pass filter.
 * \param activeFlag [in] true: enable the filter; false: disable the filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDigitalFilter(
        E384CL_ARGIN double cutoffFrequency,
        E384CL_ARGIN bool lowPassFlag,
        E384CL_ARGIN bool activeFlag);

/*! \brief Execute digital offset compensation.
 * Digital offset compensation tunes the offset of the applied voltage so that the
 * acquired current is 0. When the compensation ends the device sends a message with
 * the compensated voltage, that can be used as an estimate of the liquid junction voltage.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the pipette compensation on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t digitalOffsetCompensation(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Override digital offset compensation value.
 * Override the liquid junction voltage value. This also becomes the starting point of an
 * automated compensation.
 *
 * \param channelIdx [in] Index of the channel to override.
 * \param value [in] Override value.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t digitalOffsetCompensationOverride(
        E384CL_ARGIN uint16_t channelIdx,
        E384CL_ARGIN CharMeasurement_t value);

/*! \brief Request for voltage offset set by digital offset compensation.
 * After this message is received the device will send a message with voltage offset set
 * during the last digital offset compensation. The compensated voltage returned can be
 * used as an estimate of the liquid junction voltage.
 * \note If the given channel has not been compensated the returned voltage will be 0
 *
 * \param channelIdx [in] Index of the channel for which the voltage request.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t digitalOffsetCompensationInquiry(
        E384CL_ARGIN uint16_t channelIdx);

/*! \brief Correct the calibration offset of the current acquired in VC.
 *
 * \param channelIdx [in] Index of the channel to correct.
 * \param value [in] Value that will be added to the calibration offset.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVcCurrentOffsetDelta(
        E384CL_ARGIN uint16_t channelIdx,
        E384CL_ARGIN CharMeasurement_t value);

/*! \brief Correct the calibration offset of the voltage acquired in CC.
 *
 * \param channelIdx [in] Index of the channel to correct.
 * \param value [in] Value that will be added to the calibration offset.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCcVoltageOffsetDelta(
        E384CL_ARGIN uint16_t channelIdx,
        E384CL_ARGIN CharMeasurement_t value);

/*! \brief Zap.
 * A big voltage is applied in order to break the membrane.
 *
 * \param duration [in] Duration of the zap.
 * \param channelIdx [in] Index of the channel to zap.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t zap(
        E384CL_ARGIN CharMeasurement_t duration,
        E384CL_ARGIN uint16_t channelIdx);

/*! \brief Sets the low pass filter on the voltage stimulus.
 *
 * \param opened [in] Index of the filter setting (get available settings with method getVoltageStimulusLpfs).
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVoltageStimulusLpf(
        E384CL_ARGIN uint16_t filterIdx);

/*! \brief Sets the low pass filter on the current stimulus.
 *
 * \param opened [in] Index of the filter setting (get available settings with method getCurrentStimulusLpfs).
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCurrentStimulusLpf(
        E384CL_ARGIN uint16_t filterIdx);

/*! \brief Enable or disable the stimulus on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of indexes of the channel to control.
 * \param onValuesIn [in] Vector of on values: True to turn the stimulus on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t enableStimulus(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turns on and off a specific channel.
 *
 * \param channelIndexesIn [in] Vector of indexes of the channel to control.
 * \param onValuesIn [in] Vector of on values: True to turn the channel on (close the SWITCH), false to turn it off (open the SWITCH).
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnChannelsOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);


/*! \brief Set a channel voltage offset on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param voltagesIn [in] Vector of voltage offsets.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVoltageHoldTuner(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a VC current gain on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param gainsIn [in] Vector of current gains.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibVcCurrentGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a VC current offset on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param offsetsIn [in] Vector of current offsets.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibVcCurrentOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a CC voltage gain on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param gainsIn [in] Vector of voltage gains.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibCcVoltageGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a CC voltage offset on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param offsetsIn [in] Vector of voltage offsets.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibCcVoltageOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the gate voltage on a specific board.
 *
 * \param boardIndexesIn [in] Vector of Indexes for the boards to control.
 * \param gateVoltagesIn [in] Vector of gate voltages.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setGateVoltage(
        E384CL_ARGIN uint16_t * boardIndexesIn,
        E384CL_ARGIN LMeasHandle gateVoltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the source voltage on a specific board.
 *
 * \param boardIndexesIn [in] Vector of Indexes for the boards to control.
 * \param sourceVoltagesIn [in] Vector of gate voltages.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setSourceVoltage(
        E384CL_ARGIN uint16_t * boardIndexesIn,
        E384CL_ARGIN LMeasHandle sourceVoltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off a specific LED.
 *
 * \param ledIndex [in] Index of the LED to turn on/off.
 *        See the device documentation for an enumeration of the single LEDs.
 * \param on [in] True to turn the LED on, false to turn it off.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnLedOn(
        E384CL_ARGIN uint16_t ledIndex,
        E384CL_ARGIN bool on);

/*! \brief Set the device as a slave or a master.
 *
 * \param on [in] True to set the device as a slave, false to set it as a master.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setSlave(
        E384CL_ARGIN bool on);

/*! \brief Turn on/off the voltage compesantions for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the voltage compensations on, false to turn them off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnVoltageCompensationsOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off the current stimulus for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the current compensations on, false to turn them off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCurrentCompensationsOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off pipette compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the pipette compensation on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnPipetteCompensationOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off pipette compensation for current clamp for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the pipette compensation for current clamp on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCCPipetteCompensationOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off membrane compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the membrane compensation on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnMembraneCompensationOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn);

/*! \brief Turn on/off access resistance compensation for each channel.
 * \note Resistance compensation includes resistance correction and prediction.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the resistance compensation on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnAccessResistanceCompensationOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off access resistance correction for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the resistance correction on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnAccessResistanceCorrectionOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off access resistance prediction for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the resistance prediction on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnAccessResistancePredictionOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off leak conductance compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the leak conductance compensation on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnLeakConductanceCompensationOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off bridge balance compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param  onValuesIn [in] Array of booleans, one for each channel: True to turn the bridge balance compensation on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnBridgeBalanceCompensationOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for pipette compensation (voltage clamp) for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setPipetteCompensationOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for pipette compensation (current clamp) for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCCPipetteCompensationOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for membrane compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setMembraneCompensationOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for resistance compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistanceCompensationOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for resistance correction for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistanceCorrectionOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for resistance prediction for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for leak conductance compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setLeakConductanceCompensationOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set options for bridge balance compensation for each channel.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param optionIndexesIn [in] Option indexes.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setBridgeBalanceCompensationOptions(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * optionIndexesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value of the pipette capacitance for voltage clamp for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of pipette capacitances.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setPipetteCapacitance(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value of the pipette capacitance for current clamp for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of pipette capacitances for current clamp.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCCPipetteCapacitance(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value of the membrane capacitance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of membrane capacitances.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setMembraneCapacitance(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value of the access resistance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of access resistances.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistance(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value for the access resistance correction percentage for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of resistance correction percentages.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistanceCorrectionPercentage(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value for the access resistance correction lag for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of access resistance correction lags.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistanceCorrectionLag(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value for the access resistance prediction gain for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of access resistance prediction gains.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value for the access resistance prediction percentage for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of access resistance prediction percentages.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionPercentage(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value for the access resistance prediction bandwidth gain for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of access resistance prediction bandwiths gains.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionBandwidthGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value for the access resistance prediction tau for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of access resistance prediction taus.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionTau(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value for the leak conductance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of leak conductances.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setLeakConductance(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the value of the access resistance for bridge balance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesIn [in] Array/vector of bridge balances.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setBridgeBalanceResistance(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN double * channelValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets a digital trigger output to be activated during the next protocol.
 *
 * \param triggerIdx [in] Index of the trigger event.
 * \param terminator [in] true for invalid triggers (used as trigger list terminator), false for valid trigger events.
 * \param polarity [in] true for high polarity, false for low polarity.
 * \param triggerId [in] Physical trigger identifier.
 * \param delay [in] Trigger event delay with respect to the protocol start.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDigitalTriggerOutput(
        E384CL_ARGIN uint16_t triggerIdx,
        E384CL_ARGIN bool terminator,
        E384CL_ARGIN bool polarity,
        E384CL_ARGIN uint16_t triggerId,
        E384CL_ARGIN CharMeasurement_t delay);

/*! \brief Describes the structure of an incoming voltage protocol.
 *
 * \param protId [in] Protocol identifier number.
 * \param itemsNum [in] Number of protocol items.
 * \param sweepsNum [in] Number of sweeps of the protocol.
 * \param vRest [in] Voltage that will be applied when the protocol ends.
 * \note Each sweep increases the stepped parameter by 1 step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVoltageProtocolStructure(
        E384CL_ARGIN uint16_t protId,
        E384CL_ARGIN uint16_t itemsNum,
        E384CL_ARGIN uint16_t sweepsNum,
        E384CL_ARGIN CharMeasurement_t vRest);

/*! \brief Commits a voltage protocol item consisting of a constant voltage.
 *  Steps can be defined for both voltage and duration to make them change at any iteration.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param v0 [in] Initial voltage.
 * \param vStep [in] Voltage step.
 * \param t0 [in] Initial duration.
 * \param tStep [in] Duration step.
 * \param currentItem [in] Index of the current protocol item.
 * \param nextItem [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
 * \param repsNum [in] Number of loop repetitions before moving on.
 * \param applySteps [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
 * \note Items that do not end a loop must have \p nextItem = \<actual item\> + 1 and \p repsNum = 1.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t voltStepTimeStep(
        E384CL_ARGIN CharMeasurement_t v0,
        E384CL_ARGIN CharMeasurement_t vStep,
        E384CL_ARGIN CharMeasurement_t t0,
        E384CL_ARGIN CharMeasurement_t tStep,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps);

/*! \brief Commits a voltage protocol item consisting of a voltage ramp.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param v0 [in] Initial voltage.
 * \param vFinal [in] Final voltage.
 * \param t [in] Duration.
 * \param currentItem [in] Index of the current protocol item.
 * \param nextItem [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
 * \param repsNum [in] Number of loop repetitions before moving on.
 * \param applySteps [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
 * \note Items that do not end a loop must have \p nextItem = \<actual item\> + 1 and \p repsNum = 1.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t voltRamp(
        E384CL_ARGIN CharMeasurement_t v0,
        E384CL_ARGIN CharMeasurement_t vFinal,
        E384CL_ARGIN CharMeasurement_t t,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps);

/*! \brief Commits a voltage protocol item consisting of a sinusoidal wave.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param v0 [in] Voltage offset.
 * \param vAmp [in] Voltage amplitude.
 * \param freq [in] Oscillation frequency.
 * \param currentItem [in] Index of the current protocol item.
 * \param nextItem [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
 * \param repsNum [in] Number of loop repetitions before moving on.
 * \param applySteps [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
 * \note Items that do not end a loop must have \p nextItem = \<actual item\> + 1 and \p repsNum = 1.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t voltSin(
        E384CL_ARGIN CharMeasurement_t v0,
        E384CL_ARGIN CharMeasurement_t vAmp,
        E384CL_ARGIN CharMeasurement_t freq,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps);

/*! \brief Start a protocol.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t startProtocol(
        E384CL_ARGVOID);

/*! \brief Describes the structure of an incoming current protocol.
 *
 * \param protId [in] Protocol identifier number.
 * \param itemsNum [in] Number of protocol items.
 * \param sweepsNum [in] Number of sweeps of the protocol.
 * \param iRest [in] Current that will be applied when the protocol ends.
 * \note Each sweep increases the stepped parameters by 1 step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCurrentProtocolStructure(
        E384CL_ARGIN uint16_t protId,
        E384CL_ARGIN uint16_t itemsNum,
        E384CL_ARGIN uint16_t sweepsNum,
        E384CL_ARGIN CharMeasurement_t iRest);

/*! \brief Commits a current protocol item consisting of a constant current.
 *  Steps can be defined for both current and duration to make them change at any iteration.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param i0 [in] Initial current.
 * \param iStep [in] current step.
 * \param t0 [in] Initial duration.
 * \param tStep [in] Duration step.
 * \param currentItem [in] Index of the current protocol item.
 * \param nextItem [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
 * \param repsNum [in] Number of loop repetitions before moving on.
 * \param applySteps [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
 * \note Items that do not end a loop must have \p nextItem = \<actual item\> + 1 and \p repsNum = 1.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t currStepTimeStep(
        E384CL_ARGIN CharMeasurement_t i0,
        E384CL_ARGIN CharMeasurement_t iStep,
        E384CL_ARGIN CharMeasurement_t t0,
        E384CL_ARGIN CharMeasurement_t tStep,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps);

/*! \brief Commits a current protocol item consisting of a current ramp.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param i0 [in] Initial current.
 * \param iFinal [in] Final current.
 * \param t [in] Duration.
 * \param currentItem [in] Index of the current protocol item.
 * \param nextItem [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
 * \param repsNum [in] Number of loop repetitions before moving on.
 * \param applySteps [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
 * \note Items that do not end a loop must have \p nextItem = \<actual item\> + 1 and \p repsNum = 1.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t currRamp(
        E384CL_ARGIN CharMeasurement_t i0,
        E384CL_ARGIN CharMeasurement_t iFinal,
        E384CL_ARGIN CharMeasurement_t t,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps);

/*! \brief Commits a current protocol item consisting of a sinusoidal wave.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param i0 [in] Current offset.
 * \param iAmp [in] Current amplitude.
 * \param freq [in] Oscillation frequency.
 * \param currentItem [in] Index of the current protocol item.
 * \param nextItem [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
 * \param repsNum [in] Number of loop repetitions before moving on.
 * \param applySteps [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
 * \note Items that do not end a loop must have \p nextItem = \<actual item\> + 1 and \p repsNum = 1.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t currSin(
        E384CL_ARGIN CharMeasurement_t i0,
        E384CL_ARGIN CharMeasurement_t iAmp,
        E384CL_ARGIN CharMeasurement_t freq,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps);

/*! \brief Reset the device's ASIC.
 *
 * \param reset [in] False sets the ASIC in normal operation state, true sets in reset state.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetAsic(
        E384CL_ARGIN bool reset);

/*! \brief Reset the device's FPGA.
 *
 * \param reset [in] False sets the FPGA in normal operation state, true sets in reset state.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetFpga(
        E384CL_ARGIN bool reset);

/*! \brief Reset the device's digital offset compensation.
 *
 * \param reset [in] False sets the digital offset compensation in normal operation state, true sets in reset state.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetDigitalOffsetCompensation(
        E384CL_ARGIN bool reset);

/*! \brief Get calibration eeprom size in bytes.
 *
 * \param size [out] Size of the calibration eeprom in bytes.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCalibrationEepromSize(
        E384CL_ARGOUT uint32_t &size);

/*! \brief Write values on calibration eeprom.
 *
 * \param value [in] Values to be written.
 * \param address [in] Addresses in the eeprom memory of the first byte to be written.
 * \param size [in] Numbers of bytes to be written.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t writeCalibrationEeprom(
        E384CL_ARGIN std::vector <uint32_t> value,
        E384CL_ARGIN std::vector <uint32_t> address,
        E384CL_ARGIN std::vector <uint32_t> size);

/*! \brief Read values from calibration eeprom.
 *
 * \param value [out] Values to be read.
 * \param address [in] Addresses in the eeprom memory of the first byte to be read.
 * \param size [in] Numbers of bytes to be read.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t readCalibrationEeprom(
        E384CL_ARGOUT std::vector <uint32_t> &value,
        E384CL_ARGIN std::vector <uint32_t> address,
        E384CL_ARGIN std::vector <uint32_t> size);

/****************\
 *  Rx methods  *
\****************/

/*! \brief Get the next message from the queue sent by the connected device.
 *
 * \param rxOutput [out] Struct containing info on the received message.
 * \param data [out] array of output data.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getNextMessage(
        E384CL_ARGOUT RxOutput_t &rxOutput,
        E384CL_ARGOUT int16_t * data);

/*! \brief Get the number of channels for the device.
 *
 * \param currentChannelsNum [out] Number of current channels.
 * \param voltageChannelsNum [out] Number of voltage channels.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getChannelsNumber(
        E384CL_ARGOUT uint32_t &currentChannelsNum,
        E384CL_ARGOUT uint32_t &voltageChannelsNum);

/*! \brief Get the available data sources for all channels type.
 * \note Unavailable sources have index -1.
 *
 * \param sourcesIdxs [out] Indexes of the available data sources to be used for voltage channels.
 * \param sourcesIdxs [out] Indexes of the available data sources to be used for current channels.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAvailableChannelsSources(
        E384CL_ARGOUT ChannelSources_t &voltageSourcesIdxs,
        E384CL_ARGOUT ChannelSources_t &currentSourcesIdxs);

/*! \brief Check if the device implements the holding voltage tuner.
 *
 * \return Success if the device implements holding voltage tuner.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasVoltageHoldTuner(
        E384CL_ARGVOID);

/*! \brief Check if the device implements the holding current tuner.
 *
 * \return Success if the device implements holding current tuner.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasCurrentHoldTuner(
        E384CL_ARGVOID);


//------------------------------------------------------------------------
/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
/*! \brief Get the voltage hold tuner features, e.g. ranges, step, ...
 *
 * \param voltageHoldTunerFeatures [out] Structure containing the VoltageHoldTuner features.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVoltageHoldTunerFeatures(
        E384CL_ARGOUT LRange voltageHoldTunerFeaturesOut);

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
/*! \brief Get the VC calibration current gain features, e.g. ranges, step, ...
 *
 * \param calibVcCurrentGainFeatures [out] Structure containing the VC calibration current gain  features.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCalibVcCurrentGainFeatures(
        E384CL_ARGOUT LRange calibVcCurrentGainFeaturesOut);

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
/*! \brief Get the VC calibration current offset features, e.g. ranges, step, ...
 *
 * \param calibVcCurrentOffsetFeatures [out] Vector of structures containing the VC calibration current offset features, one element for each option.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCalibVcCurrentOffsetFeatures(
        E384CL_ARGOUT LRangeHandle * calibVcCurrentOffsetFeaturesOut);

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
/*! \brief Get the VC calibration current gain features, e.g. ranges, step, ...
 *
 * \param calibVcCurrentGainFeatures [out] Structure containing the VC calibration current gain  features.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCalibCcVoltageGainFeatures(
        E384CL_ARGOUT LRange calibVcCurrentGainFeaturesOut);

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
/*! \brief Get the VC calibration current offset features, e.g. ranges, step, ...
 *
 * \param calibVcCurrentOffsetFeatures [out] Vector of structures containing the VC calibration current offset features, one element for each option.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCalibCcVoltageOffsetFeatures(
        E384CL_ARGOUT LRangeHandle * calibVcCurrentOffsetFeaturesOut);

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
/*! \brief Get the gate voltage features, e.g. ranges, step, ...
 *
 * \param gateVoltagesFeatures [out] Structure containing the gate voltage features.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getGateVoltagesFeatures(
        E384CL_ARGOUT LRange gateVoltagesFeaturesOut);

/*! \todo Discuss with patrick the output type (pointer, vector with 1 element, ...). This is just a stub in the e384commlib */
/*! \brief Get the source voltage features, e.g. ranges, step, ...
 *
 * \param sourceVoltagesFeatures [out] Structure containing the source voltage features.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getSourceVoltagesFeatures(
        E384CL_ARGOUT LRange sourceVoltagesFeaturesOut);

//------------------------------------------------------------------------

/*! \brief Get the current ranges available in voltage clamp for the device.
 *
 * \param currentRanges [out] Array containing all the available current ranges in voltage clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVCCurrentRanges(
        E384CL_ARGOUT LRangeHandle * currentRanges);

/*! \brief Get the current ranges available in current clamp for the device.
 *
 * \param currentRanges [out] Array containing all the available current ranges in current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCCurrentRanges(
        E384CL_ARGOUT LRangeHandle * currentRanges);

/*! \brief Get the current range currently applied for voltage clamp.
 *
 * \param currentRange [out] Current range currently applied for voltage clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVCCurrentRange(
        E384CL_ARGOUT RangedMeasurement_t &currentRange);

/*! \brief Get the current range currently applied for current clamp.
 *
 * \param currentRange [out] Current range currently applied for current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCCurrentRange(
        E384CL_ARGOUT RangedMeasurement_t &currentRange);

/*! \brief Get the voltage ranges available in voltage clamp for the device.
 *
 * \param voltageRanges [out] Array containing all the available voltage ranges in voltage clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVCVoltageRanges(
        E384CL_ARGOUT LRangeHandle * voltageRanges);

/*! \brief Get the voltage ranges available in current clamp for the device.
 *
 * \param voltageRanges [out] Array containing all the available voltage ranges in current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCVoltageRanges(
        E384CL_ARGOUT LRangeHandle * voltageRanges);

/*! \brief Get the voltage range currently applied for voltage clamp.
 *
 * \param voltageRange [out] Voltage range currently applied for voltage clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVCVoltageRange(
        E384CL_ARGOUT RangedMeasurement_t &voltageRange);

/*! \brief Get the voltage range currently applied for current clamp.
 *
 * \param voltageRange [out] Voltage range currently applied for current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCVoltageRange(
        E384CL_ARGOUT RangedMeasurement_t &voltageRange);

/*! \brief Get the sampling rates available for the device.
 *
 * \param samplingRates [out] Array containing all the available sampling rates.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getSamplingRates(
        E384CL_ARGOUT LRangeHandle * samplingRates);

/*! \brief Get the real sampling rates available for the device.
 *
 * \param samplingRates [out] Array containing all the available real sampling rates
 *                            (may slightly differ from displayed sampling rates).
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getRealSamplingRates(
        E384CL_ARGOUT LRangeHandle * samplingRates);

/*! \brief Get the voltage range for voltage protocols.
 *
 * \param rangeIdx [in] Index of the voltage range to get info about.
 * \param voltageProtocolRange [out] Structure containing min voltage, max voltage and voltage step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVoltageProtocolRange(
        E384CL_ARGIN unsigned int rangeIdx,
        E384CL_ARGOUT RangedMeasurement_t &voltageProtocolRange);

/*! \brief Get the current range for current protocols.
 *
 * \param rangeIdx [in] Index of the current range to get info about.
 * \param currentProtocolRange [out] Structure containing min current, max current and current step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCurrentProtocolRange(
        E384CL_ARGIN unsigned int rangeIdx,
        E384CL_ARGOUT RangedMeasurement_t &currentProtocolRange);

/*! \brief Get the time range for voltage and current protocols.
 *
 * \param timeProtocolRange [out] Structure containing min time, max time and time step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getTimeProtocolRange(
        E384CL_ARGOUT RangedMeasurement_t &timeProtocolRange);

/*! \brief Get the frequency range for voltage and current sinusoidal protocols.
 *
 * \param frequencyProtocolRange [out] Structure containing min frequency, max frequency and frequency step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getFrequencyProtocolRange(
        E384CL_ARGOUT RangedMeasurement_t &frequencyProtocolRange);

/*! \brief Tell how many output digital trigger events the device can memorize.
 *
 * \param maxTriggersNum [out] Maximum number of output digital trigger events.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMaxOutputTriggers(E384CL_ARGOUT unsigned int &maxTriggersNum);

/*! \brief Get the number of physical digital trigger outputs.
 *
 * \param triggersNum [out] Number of output digital triggers.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getOutputTriggersNum(
        E384CL_ARGOUT unsigned int &triggersNum);

/*! \brief Tell how many protocol items the device can memorize.
 *
 * \param maxItemsNum [out] Maximum number of protocol items.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMaxProtocolItems(E384CL_ARGOUT unsigned int &maxItemsNum);

/*! \brief Tell if the device implements step protocol items.
 *
 * \return Success if the device implements step protocol items.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasProtocolStep(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements ramp protocol items.
 *
 * \return Success if the device implements ramp protocol items.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasProtocolRamp(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements sin protocol items.
 *
 * \return Success if the device implements sin protocol items.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasProtocolSin(
        E384CL_ARGVOID);

/*! \brief Get the available options for the voltage stimulus low pass filter.
 *
 * \param opened [out] Available options for the voltage stimulus low pass filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVoltageStimulusLpfs(
        E384CL_ARGOUT LStrHandle filterOptions);

/*! \brief Get the available options for the current stimulus low pass filter.
 *
 * \param opened [out] Available options for the current stimulus low pass filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCurrentStimulusLpfs(
        E384CL_ARGOUT LStrHandle filterOptions);

/*! \brief Get the number of LEDs for the device.
 *
 * \param ledsNum [out] Number of LEDs.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLedsNumber(
        E384CL_ARGOUT uint16_t &ledsNum);

/*! \brief Get the LEDs colors for the device.
 *
 * \param ledsColors [out] Array containing the colors of the LEDs.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLedsColors(
        E384CL_ARGOUT uint32_t * ledsColors);

/*! \brief Check if the device can work as a slave (triggered by digital input).
 *
 * \return Success if the device implements the feature.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasSlaveModality(
        E384CL_ARGVOID);

/*! \brief Get the clamping modalities available for the device.
 *
 * \param clampingModalities [out] Array containing all the available clamping modalities.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getClampingModalities(
        E384CL_ARGOUT std::vector <uint16_t> &clampingModalities);

/*! \brief Tell if the device implements pipette compensation.
 *
 * \return Success if the device implements pipette compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasPipetteCompensation(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements pipette compensation for current clamp.
 *
 * \return Success if the device implements pipette compensation for current clamp.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasCCPipetteCompensation(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements membrane compensation.
 *
 * \return Success if the device implements membrane compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasMembraneCompensation(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements access resistance compensation.
 * \note Resistance compensation includes resistance correction and prediction.
 *
 * \return Success if the device implements access resistance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasAccessResistanceCompensation(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements access resistance correction.
 *
 * \return Success if the device implements access resistance correction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasAccessResistanceCorrection(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements access resistance prediction.
 *
 * \return Success if the device implements access resistance prediction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasAccessResistancePrediction(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements leak conductance compensation.
 *
 * \return Success if the device implements leak conductance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasLeakConductanceCompensation(
        E384CL_ARGVOID);

/*! \brief Tell if the device implements bridge balance compensation.
 *
 * \return Success if the device implements bridge balance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t hasBridgeBalanceCompensation(
        E384CL_ARGVOID);

/*! \brief Get options for the pipette compensation.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for pipette compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getPipetteCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the pipette compensation for current clamp.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for pipette compensation for current clamp.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCPipetteCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the membrane compensation.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for membrane compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMembraneCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the access resistance compensation.
 * \note Resistance compensation includes resistance correction and prediction.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the access resistance correction.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance correction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCorrectionOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the access resistance prediction.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance prediction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistancePredictionOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the leak conductance compensation.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for leak conductance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLeakConductanceCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the bridge balance compensation.
 *
 * \param option [out]: vector of strings of the available options.
 * \return Success if the device has options for bridge balance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getBridgeBalanceCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get the specifications of the control for the liquid junction.
 *
 * \param control [in] Specifications of the control for the liquid junction.
 * \return Success if the device implements liquid junction control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLiquidJunctionControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the pipette capacitance.
 *
 * \param control [in] Specifications of the control for the pipette capacitance.
 * \return Success if the device implements pipette capacitance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getPipetteCapacitanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the pipette capacitance for current clamp.
 *
 * \param control [in] Specifications of the control for the pipette capacitance for current clamp.
 * \return Success if the device implements pipette capacitance control for current clamp.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCPipetteCapacitanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the membrane capacitance.
 *
 * \param control [in] Specifications of the control for the membrane capacitance.
 * \return Success if the device implements membrane capacitance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMembraneCapacitanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the access resistance.
 *
 * \param control [in] Specifications of the control for the access resistance.
 * \return Success if the device implements access resistance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance correction percentage.
 *
 * \param control [in] Specifications of the control for the resistance correction percentage.
 * \return Success if the device implements resistance correction percentage control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistanceCorrectionPercentageControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance correction lag.
 *
 * \param control [in] Specifications of the control for the resistance correction lag.
 * \return Success if the device implements resistance correction lag control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistanceCorrectionLagControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction gain.
 *
 * \param control [in] Specifications of the control for the resistance prediction gain.
 * \return Success if the device implements resistance prediction gain control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionGainControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction percentage.
 *
 * \param control [in] Specifications of the control for the resistance prediction percentage.
 * \return Success if the device implements resistance prediction percentage control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionPercentageControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction bandwidth gain.
 *
 * \param control [in] Specifications of the control for the resistance prediction bandwidth gain.
 * \return Success if the device implements resistance prediction bandwidth gain control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionBandwidthGainControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction tau.
 *
 * \param control [in] Specifications of the control for the resistance prediction tau.
 * \return Success if the device implements resistance prediction tau control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionTauControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the leak conductance.
 *
 * \param control [in] Specifications of the control for the leak conductance.
 * \return Success if the device implements resistance prediction tau control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLeakConductanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the bridge balance resistance.
 *
 * \param control [in] Specifications of the control for the bridge balance resistance.
 * \return Success if the device implements bridge balance resistance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getBridgeBalanceResistanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

// NEW MICHELANGELO'S GETS
/*! \brief Gets the value of the pipette capacitance for voltage clamp for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of pipette capacitances.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getPipetteCapacitance(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the pipette capacitance for current clamp for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of pipette capacitances.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCPipetteCapacitance(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the mebrane capacitance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of membrane capacitances.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMembraneCapacitance(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of theaccess resistance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of access resistances.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistance(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the resistance correction percentage for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of resistance correction percentages.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCorrectionPercentage(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the resistance correction lag for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of resistance correction lags.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCorrectionLag(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the access resistance prediction gain for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of access reisstance prediction gains.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistancePredictionGain(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the access resistance prediction percentage for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of access resistance prediction percentages.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistancePredictionPercentage(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the access resistance prediction bandwidth gain for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of access resistance prediction bandwidth gains.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistancePredictionBandwidthGain(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn);

/*! \brief Gets the value of the access resistance prediction tau for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of access resistance prediction taus.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistancePredictionTau(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the leak conductance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector of leask conductances.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLeakConductance(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the value of the bridge balance resistance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelValuesOut [out] Array/vector ofbridge balance resistances.
 * \param activeNotActiveOut [out] Array/vector of flags for each channel activity state.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getBridgeBalanceResistance(
        E384CL_ARGIN  uint16_t * channelIndexesIn,
        E384CL_ARGOUT double * channelValuesOut,
        E384CL_ARGOUT bool * activeNotActiveOut,
        E384CL_ARGIN  int vectorLengthIn = 0);

// END NEW MICHELANGELO'S GETS

#ifndef E384CL_LABVIEW_COMPATIBILITY
}
#endif


#endif // E384COMMLIB_LABVIEW_H