/*! \file e384commlib_labview.h
 * \brief Declares class CommLib.
 */
#ifndef E384COMMLIB_LABVIEW_H
#define E384COMMLIB_LABVIEW_H

#include "e384commlib_global.h"
#include "e384commlib_errorcodes.h"

/**********************\
 *  Helper functions  *
\**********************/

/*! \todo Find a way to return the content of the vectors of measurement and rangeMeasurement */

/*! \brief Get the size of the internal vector of Measurement_t.
 * \note Increases size by 1
 *
 * \param size [out] Size of the vector.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMeasVecSize(
        E384CL_ARGOUT uint16_t &size);

/*! \brief Resize the internal vector of Measurement_t.
 *
 * \param size [in] New size of the vector.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t measVecResize(
        E384CL_ARGIN uint16_t &size);

/*! \brief Clears the internal vector of Measurement_t.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t measVecClear(
        E384CL_ARGVOID);

/*! \brief Append a Measurment_t to the internal vector.
 * \note Increases size by 1
 *
 * \param value [in] Value of the measurement.
 * \param prefix [in] Prefic of the measurement unit.
 * \param unit [in] Unit of the measurement.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t appendMeasToVec(
        E384CL_ARGIN double value,
        E384CL_ARGIN UnitPfx_t prefix,
        E384CL_ARGIN char * unitIn);

/*! \brief Set a Measurment_t in the internal vector.
 * \note The size of the vector must be greater than idx.
 *
 * \param idx [in] Index of the vector to set.
 * \param value [in] Value of the measurement.
 * \param prefix [in] Prefic of the measurement unit.
 * \param unit [in] Unit of the measurement.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setMeasInVec(
        E384CL_ARGIN uint16_t idx,
        E384CL_ARGIN double value,
        E384CL_ARGIN UnitPfx_t prefix,
        E384CL_ARGIN char * unitIn);

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
        E384CL_ARGOUT LStrHandle * deviceIds);

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
        E384CL_ARGIN bool onValueIn,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Turn on/off the current stimulus for each channel.
 *
 * \param onValueIn [in] True to turn the current stimulus on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t turnCurrentStimulusOn(
        E384CL_ARGIN bool onValueIn,
        E384CL_ARGIN bool applyFlagIn);

/*! \brief Turn on/off the voltage reader for each channel.
 *  \note The voltage is read by the current clamp ADC.
 *  \note In some devices the ADC can't be turned on independently of the DAC.
 *  \note This only activates the circuitry: in order to have the device return the desired channels use #setSourceForVoltageChannel.
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
 *  \note This only activates the circuitry: in order to have the device return the desired channels use #setSourceForCurrentChannel.
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
        E384CL_ARGIN LMeasHandle * voltagesIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * currentsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * voltagesIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * currentsIn, /*!< Can use internal vector */
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Activate or deactivate the automatic subtraction of the liquid junction compensated in VC from the CC readout.
 *
 * \param flag [in] true: the liquid junction potential is subtracted from the CC readout; false: the CC readout is unaffected.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t subtractLiquidJunctionFromCc(
        E384CL_ARGIN bool flag);

/*! \brief Set the current offset to the default value.
 *
 * \param channelIndexes [in] Vector of Indexes for the channels to control.
 * \param applyFlagIn [in] true: immediately submit the command to the device; false: submit together with the next command.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetOffsetRecalibration(
        E384CL_ARGIN uint16_t * channelIndexesIn,
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
        E384CL_ARGIN LMeasHandle * voltagesIn, /*!< Can use internal vector */
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Reset the liquid junction voltage.
 *
 * \param channelIndexesIn [in] Vector of Indexes for the channels to reset.
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

/*! \brief Set the downsampling ratio, so that the final sampling rate is the value selected by the setSamplingRate method, divided by the downsampling ratio.
 *  \note In order to avoid aliasing, any downsampling ratio other than 1 will automatically activate a low pass filter with cut off frequency SR/4,
 *  were SR is the final sampling rate, after decimation.
 *
 * \param ratio [in] Decimation ratio.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDownsamplingRatio(
        E384CL_ARGIN uint32_t ratio);

/*! \brief Set a digital filter.
 *
 * \param cutoffFrequencyIn [in] The cut-off frequency in kHz of the filter.
 * \param lowPassFlag [in] true: set a low pass filter; false: set a high pass filter.
 * \param activeFlag [in] true: enable the filter; false: disable the filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDigitalFilter(
        E384CL_ARGIN LVMeasurement_t cutoffFrequencyIn,
        E384CL_ARGIN bool lowPassFlag,
        E384CL_ARGIN bool activeFlag);

/*! \brief Set a digital filter.
 *
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: The cut-off frequency in kHz of the filter.
 *                 Can use internal vector
 * \param lowPassFlag [in] true: set a low pass filter; false: set a high pass filter.
 * \param activeFlag [in] true: enable the filter; false: disable the filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDigitalFilterVec(
        E384CL_ARGIN LMeasHandle * vec,
        E384CL_ARGIN bool lowPassFlag,
        E384CL_ARGIN bool activeFlag);

/*! \brief Execute the readout offset recalibration.
 * \note The readout offset recalibration needs to be run in open circuit in voltage clamp and in short circuit in current clamp.
 * This way the readout is guaranteed to be zero and the recalibration can be performed.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param Array of booleans, one for each channel: True to turn the recalibration on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t readoutOffsetRecalibration(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Execute liquid junction compensation.
 * \note The liquid junction compensation tunes the offset of the applied voltage so that the acquired current is 0.
 * \note Do not use in open circuit: if there's a current offset in open circuit use the readoutOffsetRecalibration to fix it.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the algorithm compensation on, false to turn it off.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t liquidJunctionCompensation(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN bool * onValuesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Execute liquid junction compensation.
 * \note The liquid junction compensation tunes the offset of the applied voltage so that the acquired current is 0.
 * \note Do not use in open circuit: if there's a current offset in open circuit use the readoutOffsetRecalibration to fix it.
 * \deprecated Use liquidJunctionCompensation instead.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param onValuesIn [in] Array of booleans, one for each channel: True to turn the algorithm compensation on, false to turn it off.
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
 * \param channelIndexesIn [in] Channel indexes.
 * \param duration [in] Duration of the zap.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t zap(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LVMeasurement_t duration,
        E384CL_ARGIN int vectorLengthIn);

/*! \brief Zap.
 * A big voltage is applied in order to break the membrane.
 *
 * \param channelIndexesIn [in] Channel indexes.
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Duration of the zap.
 *                 Can use internal vector
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t zapVec(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN LMeasHandle * vec,
        E384CL_ARGIN int vectorLengthIn);

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
 * \param stopProtocolFlag [in] true, to automatically stop the currently applied stimulation before changing modality
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
        E384CL_ARGIN LMeasHandle * gainsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * offsetsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * gainsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * offsetsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * gainsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * offsetsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * gainsIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * offsetsIn, /*!< Can use internal vector */
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Configure the device to enable read/write of the calibration eeprom.
 *
 * \param calibModeFlag [in] true to enable the calibration mode; false to return to normal operation mode.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCalibrationMode(
        E384CL_ARGIN bool calibModeFlag);

/*! \brief Write values on calibration eeprom.
 *
 * \param value [in] Values to be written.
 * \param address [in] Addresses in the eeprom memory of the first byte to be written.
 * \param size [in] Numbers of bytes to be written (max 4 if all the 32bits of argument value are used).
 * \param vectorLength [in] Number of values to be written (it's the size of the arrays value, address and size)
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t writeCalibrationEeprom(
        E384CL_ARGIN uint32_t * value,
        E384CL_ARGIN uint32_t * address,
        E384CL_ARGIN uint32_t * size,
        E384CL_ARGIN uint32_t vectorLength);

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
        E384CL_ARGIN LMeasHandle * gateVoltagesIn, /*!< Can use internal vector */
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
        E384CL_ARGIN LMeasHandle * sourceVoltagesIn, /*!< Can use internal vector */
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
        E384CL_ARGIN int vectorLengthIn = 0);

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

/*! \brief Sets the range of the pipette capacitance for voltage clamp for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of pipette capacitance ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setPipetteCapacitanceRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range of the pipette capacitance for current clamp for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of pipette capacitance ranges for current clamp.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCCPipetteCapacitanceRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range of the membrane capacitance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of membrane capacitance ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setMembraneCapacitanceRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range of the access resistance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of access resistance ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistanceRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range for the access resistance correction percentage for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of resistance correction percentage ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistanceCorrectionPercentageRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range for the access resistance correction lag for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of access resistance correction lag ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistanceCorrectionLagRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range for the access resistance prediction gain for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of access resistance prediction gain ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionGainRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range for the access resistance prediction percentage for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of access resistance prediction percentage ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionPercentageRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range for the access resistance prediction tau for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of access resistance prediction tau ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setAccessResistancePredictionTauRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range for the leak conductance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of leak conductance ranges.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setLeakConductanceRange(
        E384CL_ARGIN uint16_t * channelIndexesIn,
        E384CL_ARGIN uint16_t * channelRangesIn,
        E384CL_ARGIN bool applyFlagIn,
        E384CL_ARGIN int vectorLengthIn = 0);

/*! \brief Sets the range of the access resistance for bridge balance for each channel.
 *
 * \param channelIndexesIn [in] Array/vector of channel indexes.
 * \param channelRangesIn [in] Array/vector of bridge balance rangeSs.
 * \param applyFlagIn [in] Flag for instant application of this setting.
 * \param vectorLengthIn [in] Length of the array/vector of channels to be set.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setBridgeBalanceResistanceRange(
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
        E384CL_ARGIN LVMeasurement_t vRest);

/*! \brief Describes the structure of an incoming voltage protocol.
 *
 * \param protId [in] Protocol identifier number.
 * \param itemsNum [in] Number of protocol items.
 * \param sweepsNum [in] Number of sweeps of the protocol.
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Voltage that will be applied when the protocol ends.
 *                 Can use internal vector
 * \note Each sweep increases the stepped parameter by 1 step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setVoltageProtocolStructureVec(
        E384CL_ARGIN uint16_t protId,
        E384CL_ARGIN uint16_t itemsNum,
        E384CL_ARGIN uint16_t sweepsNum,
        E384CL_ARGIN LMeasHandle * vec);

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
        E384CL_ARGIN LVMeasurement_t v0,
        E384CL_ARGIN LVMeasurement_t vStep,
        E384CL_ARGIN LVMeasurement_t t0,
        E384CL_ARGIN LVMeasurement_t tStep,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t vHalfFlag);

/*! \brief Commits a voltage protocol item consisting of a constant voltage.
 *  Steps can be defined for both voltage and duration to make them change at any iteration.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Initial voltage.
 *                 vec[1]: Voltage step.
 *                 vec[2]: Initial duration.
 *                 vec[3]: Duration step.
 *                 Can use internal vector
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
ErrorCodes_t voltStepTimeStepVec(
        E384CL_ARGIN LMeasHandle * vec,
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
        E384CL_ARGIN LVMeasurement_t v0,
        E384CL_ARGIN LVMeasurement_t vFinal,
        E384CL_ARGIN LVMeasurement_t t,
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
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Initial voltage.
 *                 vec[1]: Final voltage.
 *                 vec[2]: Duration.
 *                 Can use internal vector
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
ErrorCodes_t voltRampVec(
        E384CL_ARGIN LMeasHandle * vec,
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
        E384CL_ARGIN LVMeasurement_t v0,
        E384CL_ARGIN LVMeasurement_t vAmp,
        E384CL_ARGIN LVMeasurement_t freq,
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
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Voltage offset.
 *                 vec[1]: Voltage amplitude.
 *                 vec[2]: Oscillation frequency.
 *                 Can use internal vector
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
ErrorCodes_t voltSinVec(
        E384CL_ARGIN LMeasHandle * vec,
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

/*! \brief Stop a protocol.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t stopProtocol(
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
        E384CL_ARGIN LVMeasurement_t iRest);

/*! \brief Describes the structure of an incoming current protocol.
 *
 * \param protId [in] Protocol identifier number.
 * \param itemsNum [in] Number of protocol items.
 * \param sweepsNum [in] Number of sweeps of the protocol.
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Current that will be applied when the protocol ends.
 *                 Can use internal vector
 * \note Each sweep increases the stepped parameters by 1 step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCurrentProtocolStructureVec(
        E384CL_ARGIN uint16_t protId,
        E384CL_ARGIN uint16_t itemsNum,
        E384CL_ARGIN uint16_t sweepsNum,
        E384CL_ARGIN LMeasHandle * vec);

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
        E384CL_ARGIN LVMeasurement_t i0,
        E384CL_ARGIN LVMeasurement_t iStep,
        E384CL_ARGIN LVMeasurement_t t0,
        E384CL_ARGIN LVMeasurement_t tStep,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t cHalfFlag);

/*! \brief Commits a current protocol item consisting of a constant current.
 *  Steps can be defined for both current and duration to make them change at any iteration.
 *  Loops can also be defined to repeat a given sequence of items more than once,
 *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
 *  repsNum set to 3
 *
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Initial current.
 *                 vec[1]: Current step.
 *                 vec[2]: Initial duration.
 *                 vec[3]: Duration step.
 *                 Can use internal vector
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
ErrorCodes_t currStepTimeStepVec(
        E384CL_ARGIN LMeasHandle * vec,
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
        E384CL_ARGIN LVMeasurement_t i0,
        E384CL_ARGIN LVMeasurement_t iFinal,
        E384CL_ARGIN LVMeasurement_t t,
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
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Initial Current.
 *                 vec[1]: Final current.
 *                 vec[2]: Duration.
 *                 Can use internal vector
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
ErrorCodes_t currRampVec(
        E384CL_ARGIN LMeasHandle * vec,
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
        E384CL_ARGIN LVMeasurement_t i0,
        E384CL_ARGIN LVMeasurement_t iAmp,
        E384CL_ARGIN LVMeasurement_t freq,
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
 * \param vec [in] Vector of input measurements:
 *                 vec[0]: Current offset.
 *                 vec[1]: Current amplitude.
 *                 vec[2]: Oscillation frequency.
 *                 Can use internal vector
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
ErrorCodes_t currSinVec(
        E384CL_ARGIN LMeasHandle * vec,
        E384CL_ARGIN uint16_t currentItem,
        E384CL_ARGIN uint16_t nextItem,
        E384CL_ARGIN uint16_t repsNum,
        E384CL_ARGIN uint16_t applySteps,
        E384CL_ARGIN uint16_t cHalfFlag);

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

/*! \brief Reset the device's liquid junction compensation.
 *
 * \param reset [in] False sets the liquid junction compensation in normal operation state, true sets in reset state.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t resetLiquidJunctionCompensation(
        E384CL_ARGIN bool reset);

/*! \brief Set cooling fans speed.
 *
 * \param speed [in] Desired speed.
 * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCoolingFansSpeed(
        E384CL_ARGIN LVMeasurement_t speed,
        E384CL_ARGIN bool applyFlag);

/*! \brief Set and/or activate temperature control.
 *
 * \param temperature [in] Desired temperature.
 * \param enabled [in] true: temperature control active; false: temperature control disabled.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setTemperatureControl(
        E384CL_ARGIN LVMeasurement_t temperature,
        E384CL_ARGIN bool enabled);

/*! \brief Set temperature control PID parameters.
 *
 * \param params [in] PID parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setTemperatureControlPid(
        E384CL_ARGIN PidParams_t params);

/*! \brief Set a custom flag.
 *
 * \param idx [in] Index of the flag to be set.
 * \param flag [in] Value for the flag.
 * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCustomFlag(
        E384CL_ARGIN uint16_t idx,
        E384CL_ARGIN bool flag,
        E384CL_ARGIN bool applyFlag);

/*! \brief Set a custom multivalued option.
 *
 * \param idx [in] Index of the option to be set.
 * \param value [in] Value for the option.
 * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCustomOption(
        E384CL_ARGIN uint16_t idx,
        E384CL_ARGIN uint16_t value,
        E384CL_ARGIN bool applyFlag);

/*! \brief Set a custom double value.
 *
 * \param idx [in] Index of the value to be set.
 * \param value [in] Value to be set.
 * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setCustomDouble(
        E384CL_ARGIN uint16_t idx,
        E384CL_ARGIN double value,
        E384CL_ARGIN bool applyFlag);

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

/*! \brief Set a debug word
 *
 * \param wordOffset [in] word of the debug bit to be modified.
 * \param word value [in] new value for the debug word.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t setDebugWord(
        E384CL_ARGIN uint16_t wordOffset,
        E384CL_ARGIN uint16_t wordValue);

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
        E384CL_ARGIN int vectorLengthIn = 0);

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
 * \param voltageSourcesIdxs [out] Indexes of the available data sources to be used for voltage channels.
 * \param currentSourcesIdxs [out] Indexes of the available data sources to be used for current channels.
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
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

/*! \brief Get the current range currently applied for current clamp.
 *
 * \param rangeOut [out] Current range currently applied for current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCCurrentRange(
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

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
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

/*! \brief Get the voltage range currently applied for current clamp.
 *
 * \param rangeOut [out] Voltage range currently applied for current clamp.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCVoltageRange(
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

/*! \brief Get information on the temperature channels.
 *
 * \param names [out] String containing the name of each temperature channel, separated by commas.
 * \param ranges [out] Array containing the range for each temperature channel.
 *
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getTemperatureChannelsInfo(
        E384CL_ARGOUT LStrHandle * names,
        E384CL_ARGOUT LRangeHandle * ranges);

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
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

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
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

/*! \brief Get the time range for voltage and current protocols.
 *
 * \param rangeOut [out] Structure containing min time, max time and time step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getTimeProtocolRange(
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

/*! \brief Get the frequency range for voltage and current sinusoidal protocols.
 *
 * \param rangeOut [out] Structure containing min frequency, max frequency and frequency step.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getFrequencyProtocolRange(
        E384CL_ARGOUT LVRangedMeasurement_t &rangeOut);

/*! \brief Tell how many protocol items the device can memorize.
 *
 * \param maxItemsNum [out] Maximum number of protocol items.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMaxProtocolItems(
        E384CL_ARGOUT unsigned int &maxItemsNum);

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
        E384CL_ARGOUT LStrHandle * filterOptions);

/*! \brief Get the available options for the current stimulus low pass filter.
 *
 * \param filterOptions [out] Available options for the current stimulus low pass filter.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCurrentStimulusLpfs(
        E384CL_ARGOUT LStrHandle * filterOptions);

/*! \brief Get options for the pipette compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for pipette compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getPipetteCompensationOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get options for the pipette compensation for current clamp.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for pipette compensation for current clamp.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCPipetteCompensationOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get options for the membrane compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for membrane compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMembraneCompensationOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get options for the access resistance compensation.
 * \note Resistance compensation includes resistance correction and prediction.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCompensationOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get options for the access resistance correction.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance correction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceCorrectionOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get options for the access resistance prediction.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for access resistance prediction.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistancePredictionOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get options for the leak conductance compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for leak conductance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLeakConductanceCompensationOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get options for the bridge balance compensation.
 *
 * \param options [out]: vector of strings of the available options.
 * \return Success if the device has options for bridge balance compensation.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getBridgeBalanceCompensationOptions(
        E384CL_ARGOUT LStrHandle * options);

/*! \brief Get the specifications of the control for the pipette capacitance.
 *
 * \param control [out] Specifications of the control for the pipette capacitance.
 * \return Success if the device implements pipette capacitance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getPipetteCapacitanceControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the pipette capacitance for current clamp.
 *
 * \param control [out] Specifications of the control for the pipette capacitance for current clamp.
 * \return Success if the device implements pipette capacitance control for current clamp.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCCPipetteCapacitanceControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the membrane capacitance.
 *
 * \param control [out] Specifications of the control for the membrane capacitance.
 * \return Success if the device implements membrane capacitance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getMembraneCapacitanceControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the access resistance.
 *
 * \param control [out] Specifications of the control for the access resistance.
 * \return Success if the device implements access resistance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getAccessResistanceControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance correction percentage.
 *
 * \param control [out] Specifications of the control for the resistance correction percentage.
 * \return Success if the device implements resistance correction percentage control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistanceCorrectionPercentageControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance correction lag.
 *
 * \param control [out] Specifications of the control for the resistance correction lag.
 * \return Success if the device implements resistance correction lag control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistanceCorrectionLagControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction gain.
 *
 * \param control [out] Specifications of the control for the resistance prediction gain.
 * \return Success if the device implements resistance prediction gain control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionGainControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction percentage.
 *
 * \param control [out] Specifications of the control for the resistance prediction percentage.
 * \return Success if the device implements resistance prediction percentage control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionPercentageControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the resistance prediction tau.
 *
 * \param control [out] Specifications of the control for the resistance prediction tau.
 * \return Success if the device implements resistance prediction tau control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getResistancePredictionTauControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the leak conductance.
 *
 * \param control [out] Specifications of the control for the leak conductance.
 * \return Success if the device implements resistance prediction tau control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getLeakConductanceControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

/*! \brief Get the specifications of the control for the bridge balance resistance.
 *
 * \param control [out] Specifications of the control for the bridge balance resistance.
 * \return Success if the device implements bridge balance resistance control.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getBridgeBalanceResistanceControl(
        E384CL_ARGOUT LVCompensationControl_t &control);

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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
    E384CL_ARGIN  int vectorLengthIn = 0);

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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
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
    E384CL_ARGOUT LDoubleHandle * channelValuesOut,
    E384CL_ARGOUT LUint8Handle * activeNotActiveOut,
    E384CL_ARGIN  int vectorLengthIn = 0);

/*! \brief Gets the gain of the voltage clamp current ADC.
 *
 * \param samplingRateIdx [in] sampling rate index (different sampling rates may have different ADC calibrations).
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVcAdcGainCalibration(
        E384CL_ARGIN uint16_t samplingRateIdx,
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the offset of the voltage clamp current ADC.
 *
 * \param samplingRateIdx [in] sampling rate index (different sampling rates may have different ADC calibrations).
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVcAdcOffsetCalibration(
        E384CL_ARGIN uint16_t samplingRateIdx,
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the gain of the voltage clamp voltage DAC.
 *
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVcDacGainCalibration(
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the offset of the voltage clamp voltage DAC.
 *
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getVcDacOffsetCalibration(
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the gain of the current clamp voltage ADC.
 *
 * \param samplingRateIdx [in] sampling rate index (different sampling rates may have different ADC calibrations).
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCcAdcGainCalibration(
        E384CL_ARGIN uint16_t samplingRateIdx,
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the offset of the current clamp voltage ADC.
 *
 * \param samplingRateIdx [in] sampling rate index (different sampling rates may have different ADC calibrations).
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCcAdcOffsetCalibration(
        E384CL_ARGIN uint16_t samplingRateIdx,
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the gain of the current clamp current DAC.
 *
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCcDacGainCalibration(
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the offset of the current clamp current DAC.
 *
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCcDacOffsetCalibration(
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the offset of the voltage clamp voltage DAC due to Rs correction.
 *
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getRsCorrDacOffsetCalibration(
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Gets the conductance due to the shunt resistance.
 *
 * \param meas [out] calibration parameters.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getRsShuntConductanceCalibration(
        E384CL_ARGOUT LVecMeasHandle * meas);

/*! \brief Get calibration eeprom size in bytes.
 *
 * \param size [out] Size of the calibration eeprom in bytes.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCalibrationEepromSize(
        E384CL_ARGOUT uint32_t &size);

/*! \brief Read values from calibration eeprom.
 *
 * \param value [out] Values to be read.
 * \param address [in] Addresses in the eeprom memory of the first byte to be read.
 * \param size [in] Numbers of bytes to be read (max 4 if the all the 32bits of argument value are used).
 * \param vectorLength [in] Number of values to be written (it's the size of the arrays value, address and size).
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t readCalibrationEeprom(
        E384CL_ARGOUT uint32_t * value,
        E384CL_ARGIN uint32_t * address,
        E384CL_ARGIN uint32_t * size,
        E384CL_ARGIN uint32_t vectorLength);

/*! \brief Get cooling fans speed.
 *
 * \param range [out] Speed Range.
 * \return Error code.
 */
E384COMMLIB_NAME_MANGLING
E384COMMLIBSHARED_EXPORT
ErrorCodes_t getCoolingFansSpeedRange(
        E384CL_ARGOUT LVRangedMeasurement_t &range);

#endif // E384COMMLIB_LABVIEW_H
