/*! \file e384commlib_labview.h
 * \brief Declares class CommLib.
 */
#ifndef E384COMMLIB_LABVIEW_H
#define E384COMMLIB_LABVIEW_H
#include "e384commlib_global.h"
#include "e384commlib_errorcodes.h"

class MessageDispatcher;

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
 * \param fwPathIn [in] Path of the Firmware file (empty string if it is in the same folder as the application that calls the library).
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t connectDevice(
        E384CL_ARGIN LStrHandle deviceId,
        E384CL_ARGIN LStrHandle fwPathIn);

/*! \brief Disconnects from connected device.
 * Calling this method if no device is connected will return an error code.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t disconnectDevice(
        E384CL_ARGVOID);

/*! \brief Enables or disables message types, so that disabled messages are not returned by getNextMessage
 *  \note Message types are available in e384comllib_global.h.
 *
 * \param messageType [in] Message type to enable or disable.
 * \param flag [in] true to enable the message type, false to disable it.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t enableRxMessageType(
        E384CL_ARGIN MsgTypeId_t messageType,
        E384CL_ARGIN bool flag);

/****************\
 *  Tx methods  *
\****************/

/*! \brief Turn on/off the voltage stimulus for each channel.
 *
 * \param onValueIn [in] True to turn the voltage stimulus on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnVoltageStimulusOn(
        bool onValueIn,
        bool applyFlagIn);

/*! \brief Turn on/off the current stimulus for each channel.
 *
 * \param onValueIn [in] True to turn the current stimulus on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCurrentStimulusOn(
        bool onValueIn,
        bool applyFlagIn);

/*! \brief Turn on/off the voltage reader for each channel.
 *  \note The voltage is read by the current clamp ADC.
 *  \note In some devices the ADC can't be turned on independently of the DAC.
 *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
 *
 * \param onValueIn [in] True to turn the voltage reader on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnVoltageReaderOn(
        E384CL_ARGIN bool onValueIn,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Turn on/off the current reader for each channel.
 *  \note The current is read by the current clamp ADC.
 *  \note In some devices the ADC can't be turned on independently of the DAC.
 *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
 *
 * \param onValueIn [in] True to turn the current reader on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCurrentReaderOn(
        E384CL_ARGIN bool onValueIn,
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

/*! \brief Set the holding voltage tuner. This value is added to the whole voltage protocol currently applied and to the following.
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
        E384CL_ARGIN LMeasHandle * voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the holding current tuner. This value is added to the whole current protocol currently applied and to the following.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param currentsIn [in] Vector of current offsets.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCurrentHoldTuner(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * currentsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the channel voltage half. This value is added to the voltage protocol items that have the vHalfFlag set.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param voltagesIn [in] Vector of voltage halfs.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVoltageHalf(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the channel current half. This value is added to the current protocol items that have the cHalfFlag set.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param currentsIn [in] Vector of current halfs.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCurrentHalf(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * currentsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the liquid junction voltage.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param voltagesIn [in] Vector of voltage offsets.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setLiquidJunctionVoltage(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * voltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set the liquid junction voltage to 0.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetLiquidJunctionVoltage(
        E384CL_ARGIN uint16_t * channelIndexesIn,
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
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCCCurrentRange(
        E384CL_ARGIN uint16_t currentRangeIdx,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Set the voltage range for voltage clamp.
 *
 * \param voltageRangeIdx [in] Index of the voltage range to be set.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVCVoltageRange(
        E384CL_ARGIN uint16_t voltageRangeIdx,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Set the voltage range for current clamp.
 *
 * \param voltageRangeIdx [in] Index of the voltage range to be set.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCCVoltageRange(
        E384CL_ARGIN uint16_t voltageRangeIdx,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Set the sampling rate.
 *
 * \param samplingRateIdx [in] Index of the sampling rate to be set.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setSamplingRate(
        E384CL_ARGIN uint16_t samplingRateIdx,
        E384CL_ARGIN bool applyFlagIn);

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
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the pipette compensation on, false to turn it off.
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
 * \param filterIdx [in] Index of the desired filtering option.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVoltageStimulusLpf(
        E384CL_ARGIN uint16_t filterIdx,
        E384CL_ARGIN bool applyFlagIn);
    
/*! \brief Sets the low pass filter on the current stimulus.
 *
 * \param filterIdx [in] Index of the desired filtering option.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCurrentStimulusLpf(
        E384CL_ARGIN uint16_t filterIdx,
        E384CL_ARGIN bool applyFlagIn);

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
 * \param onValuesIn [in] Vector of on values: True to turn the channel on (close the IN SWITCH), false to turn it off (open the SWITCH).
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
    
/*! \brief Open and closes the calibration switch for a specific channel.
 *
 * \param channelIndexesIn [in] Vector of indexes of the channel to control.
 * \param onValuesIn [in] Vector of on values: True to close the channel CALIBRATION SWITCH, false to open it.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCalSwOn(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Enables or disables the stimulus in current clamp.
 *  \note The stimulus is disabled via a physical switch, not by digital means, so this allows the I0 current clamp mode
 *
 * \param channelIndexesIn [in] Vector of indexes of the channel to control.
 * \param onValuesIn [in] Vector of on values: True to enable the stimulus, false to disable it.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t enableCcStimulus(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Select the clamping modality.
 *
 * \param clampingModallityIndex [in] Index of the clamping modality to select.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setClampingModality(
        E384CL_ARGIN ClampingModality_t clampingModalityIndex,
        E384CL_ARGIN bool applyFlagIn);

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
        E384CL_ARGIN LMeasHandle * gainsIn,
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
        E384CL_ARGIN LMeasHandle * offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a VC Voltage gain on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param gainsIn [in] Vector of current gains.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibVcVoltageGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a VC voltage offset on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param offsetsIn [in] Vector of current offsets.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibVcVoltageOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * offsetsIn,
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
        E384CL_ARGIN LMeasHandle * gainsIn,
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
        E384CL_ARGIN LMeasHandle * offsetsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a CC current gain on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param gainsIn [in] Vector of voltage gains.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibCcCurrentGain(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * gainsIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Set a CC current offset on a specific channel.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to control.
 * \param offsetsIn [in] Vector of voltage offsets.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibCcCurrentOffset(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * offsetsIn,
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
        E384CL_ARGIN LMeasHandle * gateVoltagesIn,
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
        E384CL_ARGIN LMeasHandle * sourceVoltagesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Turn on/off the voltage clamp compesantions for each channel.
 *
 * \param onValue [in] True to turn the voltage clamp compensations on, false to turn them off.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnVoltageCompensationsOn(
         E384CL_ARGIN bool onValue);

/*! \brief Turn on/off the current clamp compesantions for each channel.
 *
 * \param onValue [in] True to turn the current clamp compensations on, false to turn them off.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCurrentCompensationsOn(
         E384CL_ARGIN bool onValue);

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
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the resistance compensation on, false to turn it off.
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
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the resistance correction on, false to turn it off.
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
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the resistance prediction on, false to turn it off.
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
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the leak conductance compensation on, false to turn it off.
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
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the bridge balance compensation on, false to turn it off.
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
 * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
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
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t vHalfFlag);

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
 * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
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
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t vHalfFlag);

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
 * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
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
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t vHalfFlag);

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
 * \param cHalfFlag [in] 0x0: do not add cHalfFlag to this item; 0x1 add cHalfFlag to this item.
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
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t cHalfFlag);

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
 * \param cHalfFlag [in] 0x0: do not add cHalfFlag to this item; 0x1 add cHalfFlag to this item.
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
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t cHalfFlag);

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
 * \param cHalfFlag [in] 0x0: do not add cHalfFlag to this item; 0x1 add cHalfFlag to this item.
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
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t cHalfFlag);

/*! \brief Reset the device's ASIC.
 *
 * \param reset [in] False sets the ASIC in normal operation state, true sets in reset state.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetAsic(
        E384CL_ARGIN bool reset,
        E384CL_ARGIN bool applyFlagIn = true);

/*! \brief Reset the device's FPGA.
 *
 * \param reset [in] False sets the FPGA in normal operation state, true sets in reset state.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetFpga(
        E384CL_ARGIN bool reset,
        E384CL_ARGIN bool applyFlagIn = true);

/*! \brief Reset the device's digital offset compensation.
 *
 * \param reset [in] False sets the digital offset compensation in normal operation state, true sets in reset state.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetDigitalOffsetCompensation(
        E384CL_ARGIN bool reset);

/****************\
 *  Rx methods  *
\****************/

/*! \brief Get the size of the buffer to be passed to getNextMessage.
 *
 * \param size [out] Size of the buffer to be passed to getNextMessage.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getRxDataBufferSize(
        E384CL_ARGOUT uint32_t &size);

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

/*! \brief Purge buffered data.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t purgeData(
        E384CL_ARGVOID);

/*! \brief Get the voltage currently corrected by liquid junction compensation.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to get the votlage from.
 * \param voltages [out] Array of compensated voltages.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLiquidJunctionVoltages(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGOUT LMeasHandle * voltages,
        E384CL_ARGIN int vectorLengthIn);

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

/*! \brief Get the number of boards for the device.
 *
 * \param boardsNum [out] Number of current channels.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getBoardsNumber(
        E384CL_ARGOUT uint32_t &boardsNum);

/*! \brief Get the voltage hold tuner features, e.g. ranges, step, ...
 *
 * \param voltageHoldTunerFeatures [out] Vector of ranges for VoltageHoldTuner in each stimulus range.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVoltageHoldTunerFeatures(
        E384CL_ARGOUT LRangeHandle * voltageHoldTunerFeaturesOut);

/*! \brief Get the current hold tuner features, e.g. ranges, step, ...
 *
 * \param currentHoldTunerFeatures [out] Vector of ranges for CurrentHoldTuner in each stimulus range.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCurrentHoldTunerFeatures(
        E384CL_ARGOUT LRangeHandle * currentHoldTunerFeaturesOut);

/*! \brief Get the voltage half features, e.g. ranges, step, ...
 *
 * \param voltageHalfFeatures [out] Vector of ranges for VoltageHalf in each stimulus range.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVoltageHalfFeatures(
        E384CL_ARGOUT LRangeHandle * voltageHalfFeaturesOut);

/*! \brief Get the current half features, e.g. ranges, step, ...
 *
 * \param currentHalfFeatures [out] Vector of ranges for CurrentHalf in each stimulus range.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCurrentHalfFeatures(
        E384CL_ARGOUT LRangeHandle * currentHalfFeaturesOut);

/*! \brief Get the clamping modalities available for the device.
 *
 * \param clampingModalities [out] Array containing all the available clamping modalities.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getClampingModalitiesFeatures(
        E384CL_ARGOUT uint16_t * clampingModalities);

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
 * \param rangeOut [out] Current range currently applied for voltage clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVCCurrentRange(
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

/*! \brief Get the current range currently applied for current clamp.
 *
 * \param rangeOut [out] Current range currently applied for current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCCurrentRange(
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

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
 * \param rangeOut [out] Voltage range currently applied for voltage clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVCVoltageRange(
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

/*! \brief Get the voltage range currently applied for current clamp.
 *
 * \param rangeOut [out] Voltage range currently applied for current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCVoltageRange(
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

/*! \brief Get the sampling rates available for the device.
 *
 * \param samplingRates [out] Array containing all the available sampling rates.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getSamplingRates(
        E384CL_ARGOUT LMeasHandle * samplingRates);

/*! \brief Get the real sampling rates available for the device.
 *
 * \param samplingRates [out] Array containing all the available real sampling rates
 *                            (may slightly differ from displayed sampling rates).
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getRealSamplingRates(
        E384CL_ARGOUT LMeasHandle * samplingRates);

/*! \brief Get the voltage range for voltage protocols.
 *
 * \param rangeIdx [in] Index of the voltage range to get info about.
 * \param rangeOut [out] Structure containing min voltage, max voltage and voltage step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVoltageProtocolRange(
        E384CL_ARGIN unsigned int rangeIdx,
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

/*! \brief Get the current range for current protocols.
 *
 * \param rangeIdx [in] Index of the current range to get info about.
 * \param rangeOut [out] Structure containing min current, max current and current step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCurrentProtocolRange(
        E384CL_ARGIN unsigned int rangeIdx,
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

/*! \brief Get the time range for voltage and current protocols.
 *
 * \param rangeOut [out] Structure containing min time, max time and time step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getTimeProtocolRange(
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

/*! \brief Get the frequency range for voltage and current sinusoidal protocols.
 *
 * \param rangeOut [out] Structure containing min frequency, max frequency and frequency step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getFrequencyProtocolRange(
        E384CL_ARGOUT CharRangedMeasurement_t &rangeOut);

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
 * \param filterOptions [out] Available options for the voltage stimulus low pass filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVoltageStimulusLpfs(
        E384CL_ARGOUT LStrHandle filterOptions);

/*! \brief Get the available options for the current stimulus low pass filter.
 *
 * \param filterOptions [out] Available options for the current stimulus low pass filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCurrentStimulusLpfs(
        E384CL_ARGOUT LStrHandle filterOptions);

/*! \brief Get options for the pipette compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for pipette compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getPipetteCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the pipette compensation for current clamp.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for pipette compensation for current clamp.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCPipetteCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the membrane compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for membrane compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMembraneCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the access resistance compensation.
 * \note Resistance compensation includes resistance correction and prediction.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the access resistance correction.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance correction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCorrectionOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the access resistance prediction.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance prediction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistancePredictionOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the leak conductance compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for leak conductance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLeakConductanceCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get options for the bridge balance compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for bridge balance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getBridgeBalanceCompensationOptions(
        E384CL_ARGOUT LStrHandle options);

/*! \brief Get the specifications of the control for the pipette capacitance.
 *
 * \param control [out] Specifications of the control for the pipette capacitance.
 * \return Success if the device implements pipette capacitance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getPipetteCapacitanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the pipette capacitance for current clamp.
 *
 * \param control [out] Specifications of the control for the pipette capacitance for current clamp.
 * \return Success if the device implements pipette capacitance control for current clamp.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCPipetteCapacitanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the membrane capacitance.
 *
 * \param control [out] Specifications of the control for the membrane capacitance.
 * \return Success if the device implements membrane capacitance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMembraneCapacitanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the access resistance.
 *
 * \param control [out] Specifications of the control for the access resistance.
 * \return Success if the device implements access resistance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance correction percentage.
 *
 * \param control [out] Specifications of the control for the resistance correction percentage.
 * \return Success if the device implements resistance correction percentage control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistanceCorrectionPercentageControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance correction lag.
 *
 * \param control [out] Specifications of the control for the resistance correction lag.
 * \return Success if the device implements resistance correction lag control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistanceCorrectionLagControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction gain.
 *
 * \param control [out] Specifications of the control for the resistance prediction gain.
 * \return Success if the device implements resistance prediction gain control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionGainControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction percentage.
 *
 * \param control [out] Specifications of the control for the resistance prediction percentage.
 * \return Success if the device implements resistance prediction percentage control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionPercentageControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction bandwidth gain.
 *
 * \param control [out] Specifications of the control for the resistance prediction bandwidth gain.
 * \return Success if the device implements resistance prediction bandwidth gain control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionBandwidthGainControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction tau.
 *
 * \param control [out] Specifications of the control for the resistance prediction tau.
 * \return Success if the device implements resistance prediction tau control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionTauControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the leak conductance.
 *
 * \param control [out] Specifications of the control for the leak conductance.
 * \return Success if the device implements resistance prediction tau control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLeakConductanceControl(
        E384CL_ARGOUT CharCompensationControl_t &control);

/*! \brief Get the specifications of the control for the bridge balance resistance.
 *
 * \param control [out] Specifications of the control for the bridge balance resistance.
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

/*! \brief Gets all the calibration parameters.
 *
 * \param calibrationParams [out] structure containing all the calibration paramameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCalibParams(
        E384CL_ARGIN CharCalibrationParams_t &calibrationParams);

/*! \brief Set a debug bit
 *
 * \param wordOffset [in] word of the debug bit to be modified.
 * \param bitOffset [in] debug bit to be modified.
 * \param status [in] new status of the debug bit.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDebugBit(
        E384CL_ARGIN uint16_t wordOffset,
        E384CL_ARGIN uint16_t bitOffset,
        E384CL_ARGIN bool status);

/*! \brief something
 *
 * \param something else
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDebugWord(
        E384CL_ARGIN uint16_t wordOffset,
        E384CL_ARGIN uint16_t wordValue);

#endif // E384COMMLIB_LABVIEW_H
