/*! \file e4gcommlib.h
 * \brief Declares class CommLib.
 */
#ifndef E4GCOMMLIB_H
#define E4GCOMMLIB_H

#include <vector>
#include <string>

#include "e4gcommlib_global.h"
#include "e4gcommlib_errorcodes.h"

class MessageDispatcher;

namespace e4gCommLib {

/*! \class CommLib
 * \brief This class implements the interface to 4th generation Elements devices.
 */
class E4GCOMMLIBSHARED_EXPORT CommLib {
public:

    /*****************\
     *  Ctor / Dtor  *
    \*****************/

    /*! \brief CommLib constructor.
     */
    CommLib(
            E4GCL_ARGVOID);

    /*! \brief CommLib destructor.
     */
    virtual ~CommLib(
            E4GCL_ARGVOID);

    /************************\
     *  Connection methods  *
    \************************/

    /*! \brief Detects plugged in devices.
     *
     * \param deviceIds [out] List of plugged in devices IDs.
     * \return Error code.
     */
    ErrorCodes_t detectDevices(
            E4GCL_ARGOUT std::vector <std::string> &deviceIds);

    /*! \brief Connects to a specific device
     * Calling this method if a device is already connected will return an error code.
     *
     * \param deviceId [in] Device ID of the device to connect to.
     * \return Error code.
     */
    ErrorCodes_t connect(
            E4GCL_ARGIN std::string deviceId);

    /*! \brief Filter messages returned by getNextMessage by message type.
     *
     * \param messageType [in] Message type to filter.
     * \param flag [in] True to return the selected message type; false to filter it out.
     * \return Error code.
     */
    ErrorCodes_t enableRxMessageType(
            E4GCL_ARGIN MsgTypeId_t messageType,
            E4GCL_ARGIN bool flag);

    /*! \brief Disconnects from connected device.
     * Calling this method if no device is connected will return an error code.
     *
     * \return Error code.
     */
    ErrorCodes_t disconnect(
            E4GCL_ARGVOID);

    /****************\
     *  Tx methods  *
    \****************/

    /*! \brief Pings the connected device.
     *
     * \return Error code.
     */
    ErrorCodes_t ping(
            E4GCL_ARGVOID);

    /*! \brief Aborts any task the device is currently executing.
     *
     * \return Error code.
     */
    ErrorCodes_t abort(
            E4GCL_ARGVOID);

    /*! \brief Turn on/off the voltage stimulus.
     *
     * \param on [in] True to turn the voltage stimulus on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnVoltageStimulusOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off the current stimulus.
     *
     * \param on [in] True to turn the current stimulus on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnCurrentStimulusOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off the voltage reader.
     *  \note The voltage is read by the current clamp ADC.
     *  \note In some devices the ADC can't be turned on independently of the DAC.
     *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
     *
     * \param on [in] True to turn the voltage reader on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnVoltageReaderOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off the current reader.
     *  \note The current is read by the current clamp ADC.
     *  \note In some devices the ADC can't be turned on independently of the DAC.
     *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
     *
     * \param on [in] True to turn the current reader on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnCurrentReaderOn(
            E4GCL_ARGIN bool on);

    /*! \brief Set the data sources for all channels.
     *
     * \param sourcesIdxs [in] Indexes of the data sources to be set for voltage channels.
     * \param sourcesIdxs [in] Indexes of the data sources to be set for current channels.
     * \return Error code.
     */
    ErrorCodes_t setChannelsSources(
            E4GCL_ARGIN int16_t voltageSourcesIdx,
            E4GCL_ARGIN int16_t currentSourcesIdx);

    /*! \brief Set the holding voltage tuner. This value is added to the whole voltage protocol currently applied and to the following.
     *
     * \param channelIdx [in] Channel to apply holding voltage to.
     * \param voltage [in] Holding voltage that is added to the whole voltage protocol.
     * \return Error code.
     */
    ErrorCodes_t setVoltageHoldTuner(
            E4GCL_ARGIN uint16_t channelIdx,
            E4GCL_ARGIN Measurement_t voltage);

    /*! \brief Set the holding current tuner. This value is added to the whole current protocol currently applied and to the following.
     *
     * \param channelIdx [in] Channel to apply holding current to.
     * \param current [in] Holding current that is added to the whole current protocol.
     * \return Error code.
     */
    ErrorCodes_t setCurrentHoldTuner(
            E4GCL_ARGIN uint16_t channelIdx,
            E4GCL_ARGIN Measurement_t current);

    /*! \brief Turns on/off a uniform noise additive noise with the same range as the LSB on received data.
     *
     * \param flag [in] True to turn on LSB additive noise, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnOnLsbNoise(
            E4GCL_ARGIN bool flag);

    /*! \brief Set the current range for voltage clamp.
     *
     * \param currentRangeIdx [in] Index of the current range to be set.
     * \return Error code.
     */
    ErrorCodes_t setVCCurrentRange(
            E4GCL_ARGIN uint16_t currentRangeIdx);

    /*! \brief Set the current range for current clamp.
     *
     * \param currentRangeIdx [in] Index of the current range to be set.
     * \return Error code.
     */
    ErrorCodes_t setCCCurrentRange(
            E4GCL_ARGIN uint16_t currentRangeIdx);

    /*! \brief Set the voltage range for voltage clamp.
     *
     * \param voltageRangeIdx [in] Index of the voltage range to be set.
     * \return Error code.
     */
    ErrorCodes_t setVCVoltageRange(
            E4GCL_ARGIN uint16_t voltageRangeIdx);

    /*! \brief Set the voltage range for current clamp.
     *
     * \param voltageRangeIdx [in] Index of the voltage range to be set.
     * \return Error code.
     */
    ErrorCodes_t setCCVoltageRange(
            E4GCL_ARGIN uint16_t voltageRangeIdx);

    /*! \brief Set the sampling rate.
     *
     * \param samplingRateIdx [in] Index of the sampling rate to be set.
     * \return Error code.
     */
    ErrorCodes_t setSamplingRate(
            E4GCL_ARGIN uint16_t samplingRateIdx);

    /*! \brief Set the filter ratio.
     * The filter ratio is the ratio between the sampling rate and the signal bandwitdh.
     * e.g. 2 if no filter is applied
     *
     * \param filterRatioIdx [in] Index of the filter ratio to be set.
     * \return Error code.
     */
    ErrorCodes_t setFilterRatio(
            E4GCL_ARGIN uint16_t filterRatioIdx);

    /*! \brief Set the upsampling ratio.
     * The upsampling ratio is the ratio between the real data sampling rate and the
     * one set with method #setSamplingRate.
     * \note The HW structure that performs upsampling preserves the bandwidth,
     * so the upsampling can be used instead of filters to have a bandwidth lower than
     * half sampling rate without filtering.
     *
     * \param upsamplingRatioIdx [in] Index of the upsampling ratio to be set.
     * \return Error code.
     */
    ErrorCodes_t setUpsamplingRatio(
            E4GCL_ARGIN uint16_t upsamplingRatioIdx);

    /*! \brief Set a digital filter.
     *
     * \param cutoffFrequency [in] The cut-off frequency in kHz of the filter.
     * \param lowPassFlag [in] true: set a low pass filter; false: set a high pass filter.
     * \param activeFlag [in] true: enable the filter; false: disable the filter.
     * \return Error code.
     */
    ErrorCodes_t setDigitalFilter(
            E4GCL_ARGIN double cutoffFrequency,
            E4GCL_ARGIN bool lowPassFlag,
            E4GCL_ARGIN bool activeFlag);

    /*! \brief Execute digital offset compensation.
     * Digital offset compensation tunes the offset of the applied voltage so that the
     * acquired current is 0. When the compensation ends the device sends a message with
     * the compensated voltage, that can be used as an estimate of the liquid junction voltage.
     *
     * \param channelIdx [in] Index of the channel to compensate.
     * \return Error code.
     */
    ErrorCodes_t digitalOffsetCompensation(
            E4GCL_ARGIN uint16_t channelIdx);

    /*! \brief Override digital offset compensation value.
     * Override the liquid junction voltage value. This also becomes the starting point of an
     * automated compensation.
     *
     * \param channelIdx [in] Index of the channel to override.
     * \param value [in] Override value.
     * \return Error code.
     */
    ErrorCodes_t digitalOffsetCompensationOverride(
            E4GCL_ARGIN uint16_t channelIdx,
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Request for voltage offset set by digital offset compensation.
     * After this message is received the device will send a message with voltage offset set
     * during the last digital offset compensation. The compensated voltage returned can be
     * used as an estimate of the liquid junction voltage.
     * \note If the given channel has not been compensated the returned voltage will be 0
     *
     * \param channelIdx [in] Index of the channel for which the voltage request.
     * \return Error code.
     */
    ErrorCodes_t digitalOffsetCompensationInquiry(
            E4GCL_ARGIN uint16_t channelIdx);

    /*! \brief Correct the calibration offset of the current acquired in VC.
     *
     * \param channelIdx [in] Index of the channel to correct.
     * \param value [in] Value that will be added to the calibration offset.
     * \return Error code.
     */
    ErrorCodes_t setVcCurrentOffsetDelta(
            E4GCL_ARGIN uint16_t channelIdx,
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Correct the calibration offset of the voltage acquired in CC.
     *
     * \param channelIdx [in] Index of the channel to correct.
     * \param value [in] Value that will be added to the calibration offset.
     * \return Error code.
     */
    ErrorCodes_t setCcVoltageOffsetDelta(
            E4GCL_ARGIN uint16_t channelIdx,
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Zap.
     * A big voltage is applied in order to break the membrane.
     *
     * \param duration [in] Duration of the zap.
     * \param channelIdx [in] Index of the channel to zap.
     * \return Error code.
     */
    ErrorCodes_t zap(
            E4GCL_ARGIN Measurement_t duration,
            E4GCL_ARGIN uint16_t channelIdx);

    /*! \brief Sets the low pass filter on the voltage stimulus.
     *
     * \param opened [in] Index of the filter setting (get available settings with method getVoltageStimulusLpfs).
     * \return Error code.
     */
    ErrorCodes_t setVoltageStimulusLpf(
            E4GCL_ARGIN uint16_t filterIdx);

    /*! \brief Sets the low pass filter on the current stimulus.
     *
     * \param opened [in] Index of the filter setting (get available settings with method getCurrentStimulusLpfs).
     * \return Error code.
     */
    ErrorCodes_t setCurrentStimulusLpf(
            E4GCL_ARGIN uint16_t filterIdx);

    /*! \brief Enable or disable the stimulus on a specific channel.
     *
     * \param channelIdx [in] Index of the channel to control.
     * \param on [in] True to turn the stimulus on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t enableStimulus(
            E4GCL_ARGIN uint16_t channelIdx,
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off a specific LED.
     *
     * \param ledIndex [in] Index of the LED to turn on/off.
     *        See the device documentation for an enumeration of the single LEDs.
     * \param on [in] True to turn the LED on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnLedOn(
            E4GCL_ARGIN uint16_t ledIndex,
            E4GCL_ARGIN bool on);

    /*! \brief Set the device as a slave or a master.
     *
     * \param on [in] True to set the device as a slave, false to set it as a master.
     * \return Error code.
     */
    ErrorCodes_t setSlave(
            E4GCL_ARGIN bool on);

    /*! \brief Sets switches that are not controlled by widget.
     *
     * \return Error code.
     */
    ErrorCodes_t setConstantSwitches(
            E4GCL_ARGVOID);

    /*! \brief Select the channel for compesantions settings.
     *  Call this method before other compensations control methods, in order to select which channel
     *  those methods should apply to.
     *
     * \param channelIdx [in] Channel index that compensations methods will be applied to.
     * \return Error code.
     */
    ErrorCodes_t setCompensationsChannel(
            E4GCL_ARGIN uint16_t channelIdx);

    /*! \brief Turn on/off the voltage compesantions.
     *
     * \param on [in] True to turn the voltage compensations on, false to turn them off.
     * \return Error code.
     */
    ErrorCodes_t turnVoltageCompensationsOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off the current stimulus.
     *
     * \param on [in] True to turn the current compensations on, false to turn them off.
     * \return Error code.
     */
    ErrorCodes_t turnCurrentCompensationsOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off pipette compensation.
     *
     * \param on [in] True to turn the pipette compensation on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnPipetteCompensationOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off pipette compensation for current clamp.
     *
     * \param on [in] True to turn the pipette compensation for current clamp on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnCCPipetteCompensationOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off membrane compensation.
     *
     * \param on [in] True to turn the membrane compensation on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnMembraneCompensationOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off access resistance compensation.
     * \note Resistance compensation includes resistance correction and prediction.
     *
     * \param on [in] True to turn the resistance compensation on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnAccessResistanceCompensationOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off access resistance correction.
     *
     * \param on [in] True to turn the resistance correction on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnAccessResistanceCorrectionOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off access resistance prediction.
     *
     * \param on [in] True to turn the resistance prediction on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnAccessResistancePredictionOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off leak conductance compensation.
     *
     * \param on [in] True to turn the leak conductance compensation on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnLeakConductanceCompensationOn(
            E4GCL_ARGIN bool on);

    /*! \brief Turn on/off bridge balance compensation.
     *
     * \param on [in] True to turn the bridge balance compensation on, false to turn it off.
     * \return Error code.
     */
    ErrorCodes_t turnBridgeBalanceCompensationOn(
            E4GCL_ARGIN bool on);

    /*! \brief Set options for pipette compensation (voltage clamp).
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setPipetteCompensationOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Set options for pipette compensation (current clamp).
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setCCPipetteCompensationOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Set options for membrane compensation.
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setMembraneCompensationOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Set options for resistance compensation.
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistanceCompensationOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Set options for resistance correction.
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistanceCorrectionOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Set options for resistance prediction.
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistancePredictionOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Set options for leak conductance compensation.
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setLeakConductanceCompensationOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Set options for bridge balance compensation.
     *
     * \param optionIdx [in] Option index.
     * \return Error code.
     */
    ErrorCodes_t setBridgeBalanceCompensationOptions(
            E4GCL_ARGIN uint16_t optionIdx);

    /*! \brief Sets the value of the pipette capacitance.
     *
     * \param value [in] Value of the pipette capacitance.
     * \return Error code.
     */
    ErrorCodes_t setPipetteCapacitance(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value of the pipette capacitance for current clamp.
     *
     * \param value [in] Value of the pipette capacitance for current clamp.
     * \return Error code.
     */
    ErrorCodes_t setCCPipetteCapacitance(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value of the membrane capacitance.
     *
     * \param value [in] Value of the membrane capacitance.
     * \return Error code.
     */
    ErrorCodes_t setMembraneCapacitance(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value of the access resistance.
     *
     * \param value [in] Value of the access resistance.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistance(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value for the access resistance correction percentage.
     *
     * \param value [in] Value of the access resistance correction percentage.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistanceCorrectionPercentage(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value for the access resistance correction lag.
     *
     * \param value [in] Value of the access resistance correction lag.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistanceCorrectionLag(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value for the access resistance prediction gain.
     *
     * \param value [in] Value of the access resistance prediction gain.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistancePredictionGain(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value for the access resistance prediction percentage.
     *
     * \param value [in] Value of the access resistance prediction percentage.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistancePredictionPercentage(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value for the access resistance prediction bandwidth gain.
     *
     * \param value [in] Value of the access resistance prediction bandwidth gain.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistancePredictionBandwidthGain(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value for the access resistance prediction tau.
     *
     * \param value [in] Value of the access resistance prediction tau.
     * \return Error code.
     */
    ErrorCodes_t setAccessResistancePredictionTau(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value for the leak conductance.
     *
     * \param value [in] Value of the leak conductance.
     * \return Error code.
     */
    ErrorCodes_t setLeakConductance(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets the value of the access resistance for bridge balance.
     *
     * \param value [in] Value of the access resistance for bridge balance.
     * \return Error code.
     */
    ErrorCodes_t setBridgeBalanceResistance(
            E4GCL_ARGIN Measurement_t value);

    /*! \brief Sets a digital trigger output to be activated during the next protocol.
     *
     * \param triggerIdx [in] Index of the trigger event.
     * \param terminator [in] true for invalid triggers (used as trigger list terminator), false for valid trigger events.
     * \param polarity [in] true for high polarity, false for low polarity.
     * \param triggerId [in] Physical trigger identifier.
     * \param delay [in] Trigger event delay with respect to the protocol start.
     * \return Error code.
     */
    ErrorCodes_t setDigitalTriggerOutput(
            E4GCL_ARGIN uint16_t triggerIdx,
            E4GCL_ARGIN bool terminator,
            E4GCL_ARGIN bool polarity,
            E4GCL_ARGIN uint16_t triggerId,
            E4GCL_ARGIN Measurement_t delay);

    /*! \brief Describes the structure of an incoming voltage protocol.
     *
     * \param protId [in] Protocol identifier number.
     * \param itemsNum [in] Number of protocol items.
     * \param sweepsNum [in] Number of sweeps of the protocol.
     * \param vRest [in] Voltage that will be applied when the protocol ends.
     * \note Each sweep increases the stepped parameter by 1 step.
     * \return Error code.
     */
    ErrorCodes_t setVoltageProtocolStructure(
            E4GCL_ARGIN uint16_t protId,
            E4GCL_ARGIN uint16_t itemsNum,
            E4GCL_ARGIN uint16_t sweepsNum,
            E4GCL_ARGIN Measurement_t vRest);

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
    ErrorCodes_t voltStepTimeStep(
            E4GCL_ARGIN Measurement_t v0,
            E4GCL_ARGIN Measurement_t vStep,
            E4GCL_ARGIN Measurement_t t0,
            E4GCL_ARGIN Measurement_t tStep,
            E4GCL_ARGIN uint16_t currentItem,
            E4GCL_ARGIN uint16_t nextItem,
            E4GCL_ARGIN uint16_t repsNum,
            E4GCL_ARGIN uint16_t applySteps);

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
    ErrorCodes_t voltRamp(
            E4GCL_ARGIN Measurement_t v0,
            E4GCL_ARGIN Measurement_t vFinal,
            E4GCL_ARGIN Measurement_t t,
            E4GCL_ARGIN uint16_t currentItem,
            E4GCL_ARGIN uint16_t nextItem,
            E4GCL_ARGIN uint16_t repsNum,
            E4GCL_ARGIN uint16_t applySteps);

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
    ErrorCodes_t voltSin(
            E4GCL_ARGIN Measurement_t v0,
            E4GCL_ARGIN Measurement_t vAmp,
            E4GCL_ARGIN Measurement_t freq,
            E4GCL_ARGIN uint16_t currentItem,
            E4GCL_ARGIN uint16_t nextItem,
            E4GCL_ARGIN uint16_t repsNum,
            E4GCL_ARGIN uint16_t applySteps);

    /*! \brief Start a protocol.
     *
     * \return Error code.
     */
    ErrorCodes_t startProtocol(
            E4GCL_ARGVOID);

    /*! \brief Describes the structure of an incoming current protocol.
     *
     * \param protId [in] Protocol identifier number.
     * \param itemsNum [in] Number of protocol items.
     * \param sweepsNum [in] Number of sweeps of the protocol.
     * \param iRest [in] Current that will be applied when the protocol ends.
     * \note Each sweep increases the stepped parameters by 1 step.
     * \return Error code.
     */
    ErrorCodes_t setCurrentProtocolStructure(
            E4GCL_ARGIN uint16_t protId,
            E4GCL_ARGIN uint16_t itemsNum,
            E4GCL_ARGIN uint16_t sweepsNum,
            E4GCL_ARGIN Measurement_t iRest);

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
    ErrorCodes_t currStepTimeStep(
            E4GCL_ARGIN Measurement_t i0,
            E4GCL_ARGIN Measurement_t iStep,
            E4GCL_ARGIN Measurement_t t0,
            E4GCL_ARGIN Measurement_t tStep,
            E4GCL_ARGIN uint16_t currentItem,
            E4GCL_ARGIN uint16_t nextItem,
            E4GCL_ARGIN uint16_t repsNum,
            E4GCL_ARGIN uint16_t applySteps);

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
    ErrorCodes_t currRamp(
            E4GCL_ARGIN Measurement_t i0,
            E4GCL_ARGIN Measurement_t iFinal,
            E4GCL_ARGIN Measurement_t t,
            E4GCL_ARGIN uint16_t currentItem,
            E4GCL_ARGIN uint16_t nextItem,
            E4GCL_ARGIN uint16_t repsNum,
            E4GCL_ARGIN uint16_t applySteps);

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
    ErrorCodes_t currSin(
            E4GCL_ARGIN Measurement_t i0,
            E4GCL_ARGIN Measurement_t iAmp,
            E4GCL_ARGIN Measurement_t freq,
            E4GCL_ARGIN uint16_t currentItem,
            E4GCL_ARGIN uint16_t nextItem,
            E4GCL_ARGIN uint16_t repsNum,
            E4GCL_ARGIN uint16_t applySteps);

    /*! \brief Reset the device's chip.
     *
     * \param reset [in] False sets the chip in normal operation state, true sets in reset state.
     * \return Error code.
     */
    ErrorCodes_t resetChip(
            E4GCL_ARGIN bool reset);

    /*! \brief Reset the device's digital offset compensation.
     *
     * \param reset [in] False sets the digital offset compensation in normal operation state, true sets in reset state.
     * \return Error code.
     */
    ErrorCodes_t resetDigitalOffsetCompensation(
            E4GCL_ARGIN bool reset);

    /*! \brief Reset the device's FPGA.
     *
     * \return Error code.
     */
    ErrorCodes_t resetFpga(
            E4GCL_ARGVOID);

    /*! \brief Get calibration configuration structure.
     *
     * \param calibrationConfiguration [out] Calibration configuration structure.
     * \return Error code.
     */
    ErrorCodes_t getCalibrationConfiguration(
            E4GCL_ARGOUT CalibrationConfiguration_t * &calibrationConfiguration);

    /*! \brief Get calibration eeprom size in bytes.
     *
     * \param size [out] Size of the calibration eeprom in bytes.
     * \return Error code.
     */
    ErrorCodes_t getCalibrationEepromSize(
            E4GCL_ARGOUT uint32_t &size);

    /*! \brief Write values on calibration eeprom.
     *
     * \param value [in] Values to be written.
     * \param address [in] Addresses in the eeprom memory of the first byte to be written.
     * \param size [in] Numbers of bytes to be written.
     * \return Error code.
     */
    ErrorCodes_t writeCalibrationEeprom(
            E4GCL_ARGIN std::vector <uint32_t> value,
            E4GCL_ARGIN std::vector <uint32_t> address,
            E4GCL_ARGIN std::vector <uint32_t> size);

    /*! \brief Read values from calibration eeprom.
     *
     * \param value [out] Values to be read.
     * \param address [in] Addresses in the eeprom memory of the first byte to be read.
     * \param size [in] Numbers of bytes to be read.
     * \return Error code.
     */
    ErrorCodes_t readCalibrationEeprom(
            E4GCL_ARGIN std::vector <uint32_t> &value,
            E4GCL_ARGIN std::vector <uint32_t> address,
            E4GCL_ARGIN std::vector <uint32_t> size);

    /*! \brief Gets all HW switches and their names \note This function should be used only for debug purposes.
     *
     * \param words [out] current status of all switches.
     * \param names [out] names of all switches.
     * \return Error code.
     */
    ErrorCodes_t getSwitchesStatus(
            E4GCL_ARGIN std::vector <uint16_t> &words,
            E4GCL_ARGIN std::vector <std::vector <std::string>> &names);

    /*! \brief Sets a single HW switch \note This function should be used only for debug purposes.
     *
     * \param word [in] word of the switch to set.
     * \param bit [in] bit of the switch to set.
     * \param flag [in] true to close the switch, false to open it.
     * \return Error code.
     */
    ErrorCodes_t singleSwitchDebug(
            E4GCL_ARGIN uint16_t word,
            E4GCL_ARGIN uint16_t bit,
            E4GCL_ARGIN bool flag);

    /*! \brief Sets all HW switches \note This function should be used only for debug purposes.
     *
     * \param words [in] status of all switches.
     * \return Error code.
     */
    ErrorCodes_t multiSwitchDebug(
            E4GCL_ARGIN std::vector <uint16_t> words);

    /*! \brief Set a single HW register \note This function should be used only for debug purposes.
     *
     * \param index [in] index of the register to set.
     * \param value [in] value to be written in the register.
     * \return Error code.
     */
    ErrorCodes_t singleRegisterDebug(
            E4GCL_ARGIN uint16_t index,
            E4GCL_ARGIN uint16_t value);

    /****************\
     *  Rx methods  *
    \****************/

    /*! \brief Get notification of possible upgrades for the connected device.
     * Returns Success if there are upgrades available.
     *
     * \param upgradeNotes [out] Notes of the available upgrades.
     * \return Error code.
     */
    ErrorCodes_t isDeviceUpgradable(
            E4GCL_ARGOUT std::string &upgradeNotes,
            E4GCL_ARGOUT std::string &notificationTag);

    /*! \brief Get the device identification information (to be used when the device is already connected).
     *
     * \param deviceId [out] Device identification code (S/N).
     * \param deviceName [out] Device name.
     * \param deviceVersion [out] Device version.
     * \param deviceSubversion [out] Device subversion.
     * \param firmwareVersion [out] Firmware version.
     * \return Error code.
     */
    ErrorCodes_t getDeviceInfo(
            E4GCL_ARGOUT std::string &deviceId,
            E4GCL_ARGOUT std::string &deviceName,
            E4GCL_ARGOUT uint8_t &deviceVersion,
            E4GCL_ARGOUT uint8_t &deviceSubversion,
            E4GCL_ARGOUT uint32_t &firmwareVersion);

    /*! \brief Get the device identification information (to be used when the device is not connected yet).
     *
     * \param deviceId [in] Device identification code (S/N).
     * \param deviceVersion [out] Device version.
     * \param deviceSubversion [out] Device subversion.
     * \param firmwareVersion [out] Firmware version.
     * \return Error code.
     */
    static ErrorCodes_t getDeviceInfo(
            E4GCL_ARGOUT std::string deviceId,
            E4GCL_ARGOUT uint8_t &deviceVersion,
            E4GCL_ARGOUT uint8_t &deviceSubversion,
            E4GCL_ARGOUT uint32_t &firmwareVersion);

    /*! \brief Get the next message from the queue sent by the connected device.
     *
     * \param rxOutput [out] Struct containing info on the received message.
     * \return Error code.
     */
    ErrorCodes_t getNextMessage(
            E4GCL_ARGOUT RxOutput_t &rxOutput);

    /*! \brief Get the number of channels for the device.
     *
     * \param currentChannelsNum [out] Number of current channels.
     * \param voltageChannelsNum [out] Number of voltage channels.
     * \return Error code.
     */
    ErrorCodes_t getChannelsNumber(
            E4GCL_ARGOUT uint32_t &currentChannelsNum,
            E4GCL_ARGOUT uint32_t &voltageChannelsNum);

    /*! \brief Get the available data sources for all channels type.
     * \note Unavailable sources have index -1.
     *
     * \param sourcesIdxs [out] Indexes of the available data sources to be used for voltage channels.
     * \param sourcesIdxs [out] Indexes of the available data sources to be used for current channels.
     * \return Error code.
     */
    ErrorCodes_t getAvailableChannelsSources(
            E4GCL_ARGOUT ChannelSources_t &voltageSourcesIdxs,
            E4GCL_ARGOUT ChannelSources_t &currentSourcesIdxs);

    /*! \brief Check if the device implements the holding voltage tuner.
     *
     * \return Success if the device implements holding voltage tuner.
     */
    ErrorCodes_t hasVoltageHoldTuner(
            E4GCL_ARGVOID);

    /*! \brief Check if the device implements the holding current tuner.
     *
     * \return Success if the device implements holding current tuner.
     */
    ErrorCodes_t hasCurrentHoldTuner(
            E4GCL_ARGVOID);

    /*! \brief Get the current ranges available in voltage clamp for the device.
     *
     * \param currentRanges [out] Array containing all the available current ranges in voltage clamp.
     * \return Error code.
     */
    ErrorCodes_t getVCCurrentRanges(
            E4GCL_ARGOUT std::vector <RangedMeasurement_t> &currentRanges);

    /*! \brief Get the current ranges available in current clamp for the device.
     *
     * \param currentRanges [out] Array containing all the available current ranges in current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCCurrentRanges(
            E4GCL_ARGOUT std::vector <RangedMeasurement_t> &currentRanges);

    /*! \brief Get the current range currently applied for voltage clamp.
     *
     * \param currentRange [out] Current range currently applied for voltage clamp.
     * \return Error code.
     */
    ErrorCodes_t getVCCurrentRange(
            E4GCL_ARGOUT RangedMeasurement_t &currentRange);

    /*! \brief Get the current range currently applied for current clamp.
     *
     * \param currentRange [out] Current range currently applied for current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCCurrentRange(
            E4GCL_ARGOUT RangedMeasurement_t &currentRange);

    /*! \brief Get the voltage ranges available in voltage clamp for the device.
     *
     * \param voltageRanges [out] Array containing all the available voltage ranges in voltage clamp.
     * \return Error code.
     */
    ErrorCodes_t getVCVoltageRanges(
            E4GCL_ARGOUT std::vector <RangedMeasurement_t> &voltageRanges);

    /*! \brief Get the voltage ranges available in current clamp for the device.
     *
     * \param voltageRanges [out] Array containing all the available voltage ranges in current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCVoltageRanges(
            E4GCL_ARGOUT std::vector <RangedMeasurement_t> &voltageRanges);

    /*! \brief Get the voltage range currently applied for voltage clamp.
     *
     * \param voltageRange [out] Voltage range currently applied for voltage clamp.
     * \return Error code.
     */
    ErrorCodes_t getVCVoltageRange(
            E4GCL_ARGOUT RangedMeasurement_t &voltageRange);

    /*! \brief Get the voltage range currently applied for current clamp.
     *
     * \param voltageRange [out] Voltage range currently applied for current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCVoltageRange(
            E4GCL_ARGOUT RangedMeasurement_t &voltageRange);

    /*! \brief Get the sampling rates available for the device.
     *
     * \param samplingRates [out] Array containing all the available sampling rates.
     * \return Error code.
     */
    ErrorCodes_t getSamplingRates(
            E4GCL_ARGOUT std::vector <Measurement_t> &samplingRates);

    /*! \brief Get the real sampling rates available for the device.
     *
     * \param samplingRates [out] Array containing all the available real sampling rates
     *                            (may slightly differ from displayed sampling rates).
     * \return Error code.
     */
    ErrorCodes_t getRealSamplingRates(
            E4GCL_ARGOUT std::vector <Measurement_t> &samplingRates);

    /*! \brief Get the filter ratios available for the device.
     *
     * \param filterRatios [out] Array containing all the available filter ratios.
     * \return Error code.
     */
    ErrorCodes_t getFilterRatios(
            E4GCL_ARGOUT std::vector <Measurement_t> &filterRatios);

    /*! \brief Get the upsampling ratios available for the device.
     *
     * \param upsamplingRatios [out] Array containing all the available upsampling ratios.
     * \return Error code.
     */
    ErrorCodes_t getUpsamplingRatios(
            E4GCL_ARGOUT std::vector <Measurement_t> &upsamplingRatios);

    /*! \brief Get the voltage range for voltage protocols.
     *
     * \param rangeIdx [in] Index of the voltage range to get info about.
     * \param voltageProtocolRange [out] Structure containing min voltage, max voltage and voltage step.
     * \return Error code.
     */
    ErrorCodes_t getVoltageProtocolRange(
            E4GCL_ARGIN unsigned int rangeIdx,
            E4GCL_ARGOUT RangedMeasurement_t &voltageProtocolRange);

    /*! \brief Get the current range for current protocols.
     *
     * \param rangeIdx [in] Index of the current range to get info about.
     * \param currentProtocolRange [out] Structure containing min current, max current and current step.
     * \return Error code.
     */
    ErrorCodes_t getCurrentProtocolRange(
            E4GCL_ARGIN unsigned int rangeIdx,
            E4GCL_ARGOUT RangedMeasurement_t &currentProtocolRange);

    /*! \brief Get the time range for voltage and current protocols.
     *
     * \param timeProtocolRange [out] Structure containing min time, max time and time step.
     * \return Error code.
     */
    ErrorCodes_t getTimeProtocolRange(
            E4GCL_ARGOUT RangedMeasurement_t &timeProtocolRange);

    /*! \brief Get the frequency range for voltage and current sinusoidal protocols.
     *
     * \param frequencyProtocolRange [out] Structure containing min frequency, max frequency and frequency step.
     * \return Error code.
     */
    ErrorCodes_t getFrequencyProtocolRange(
            E4GCL_ARGOUT RangedMeasurement_t &frequencyProtocolRange);

    /*! \brief Tell how many output digital trigger events the device can memorize.
     *
     * \param maxTriggersNum [out] Maximum number of output digital trigger events.
     * \return Error code.
     */
    ErrorCodes_t getMaxOutputTriggers(E4GCL_ARGOUT unsigned int &maxTriggersNum);

    /*! \brief Get the number of physical digital trigger outputs.
     *
     * \param triggersNum [out] Number of output digital triggers.
     * \return Error code.
     */
    ErrorCodes_t getOutputTriggersNum(
            E4GCL_ARGOUT unsigned int &triggersNum);

    /*! \brief Tell how many protocol items the device can memorize.
     *
     * \param maxItemsNum [out] Maximum number of protocol items.
     * \return Error code.
     */
    ErrorCodes_t getMaxProtocolItems(E4GCL_ARGOUT unsigned int &maxItemsNum);

    /*! \brief Tell if the device implements step protocol items.
     *
     * \return Success if the device implements step protocol items.
     */
    ErrorCodes_t hasProtocolStep(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements ramp protocol items.
     *
     * \return Success if the device implements ramp protocol items.
     */
    ErrorCodes_t hasProtocolRamp(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements sin protocol items.
     *
     * \return Success if the device implements sin protocol items.
     */
    ErrorCodes_t hasProtocolSin(
            E4GCL_ARGVOID);

    /*! \brief Get the available options for the voltage stimulus low pass filter.
     *
     * \param opened [out] Available options for the voltage stimulus low pass filter.
     * \return Error code.
     */
    ErrorCodes_t getVoltageStimulusLpfs(
            E4GCL_ARGOUT std::vector <std::string> &filterOptions);

    /*! \brief Get the available options for the current stimulus low pass filter.
     *
     * \param opened [out] Available options for the current stimulus low pass filter.
     * \return Error code.
     */
    ErrorCodes_t getCurrentStimulusLpfs(
            E4GCL_ARGOUT std::vector <std::string> &filterOptions);

    /*! \brief Get the number of LEDs for the device.
     *
     * \param ledsNum [out] Number of LEDs.
     * \return Error code.
     */
    ErrorCodes_t getLedsNumber(
            E4GCL_ARGOUT uint16_t &ledsNum);

    /*! \brief Get the LEDs colors for the device.
     *
     * \param ledsColors [out] Array containing the colors of the LEDs.
     * \return Error code.
     */
    ErrorCodes_t getLedsColors(
            E4GCL_ARGOUT std::vector <uint32_t> &ledsColors);

    /*! \brief Check if the device can work as a slave (triggered by digital input).
     *
     * \return Success if the device implements the feature.
     */
    ErrorCodes_t hasSlaveModality(
            E4GCL_ARGVOID);

    /*! \brief Get the clamping modalities available for the device.
     *
     * \param clampingModalities [out] Array containing all the available clamping modalities.
     * \return Error code.
     */
    ErrorCodes_t getClampingModalities(
            E4GCL_ARGOUT std::vector <uint16_t> &clampingModalities);

    /*! \brief Tells if the device's multimeter can get stuck when it saturates in current clamp.
     *
     * \param stuckFlag [out] true if the device's multimeter can get stuck when it saturates in current clamp, false otherwise.
     * \return Error code.
     */
    ErrorCodes_t multimeterStuckHazard(
            E4GCL_ARGOUT bool &stuckFlag);

    /*! \brief Tell if the device implements pipette compensation.
     *
     * \return Success if the device implements pipette compensation.
     */
    ErrorCodes_t hasPipetteCompensation(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements pipette compensation for current clamp.
     *
     * \return Success if the device implements pipette compensation for current clamp.
     */
    ErrorCodes_t hasCCPipetteCompensation(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements membrane compensation.
     *
     * \return Success if the device implements membrane compensation.
     */
    ErrorCodes_t hasMembraneCompensation(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements access resistance compensation.
     * \note Resistance compensation includes resistance correction and prediction.
     *
     * \return Success if the device implements access resistance compensation.
     */
    ErrorCodes_t hasAccessResistanceCompensation(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements access resistance correction.
     *
     * \return Success if the device implements access resistance correction.
     */
    ErrorCodes_t hasAccessResistanceCorrection(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements access resistance prediction.
     *
     * \return Success if the device implements access resistance prediction.
     */
    ErrorCodes_t hasAccessResistancePrediction(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements leak conductance compensation.
     *
     * \return Success if the device implements leak conductance compensation.
     */
    ErrorCodes_t hasLeakConductanceCompensation(
            E4GCL_ARGVOID);

    /*! \brief Tell if the device implements bridge balance compensation.
     *
     * \return Success if the device implements bridge balance compensation.
     */
    ErrorCodes_t hasBridgeBalanceCompensation(
            E4GCL_ARGVOID);

    /*! \brief Get options for the pipette compensation.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for pipette compensation.
     */
    ErrorCodes_t getPipetteCompensationOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get options for the pipette compensation for current clamp.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for pipette compensation for current clamp.
     */
    ErrorCodes_t getCCPipetteCompensationOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get options for the membrane compensation.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for membrane compensation.
     */
    ErrorCodes_t getMembraneCompensationOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get options for the access resistance compensation.
     * \note Resistance compensation includes resistance correction and prediction.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for access resistance compensation.
     */
    ErrorCodes_t getAccessResistanceCompensationOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get options for the access resistance correction.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for access resistance correction.
     */
    ErrorCodes_t getAccessResistanceCorrectionOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get options for the access resistance prediction.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for access resistance prediction.
     */
    ErrorCodes_t getAccessResistancePredictionOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get options for the leak conductance compensation.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for leak conductance compensation.
     */
    ErrorCodes_t getLeakConductanceCompensationOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get options for the bridge balance compensation.
     *
     * \param option [out]: vector of strings of the available options.
     * \return Success if the device has options for bridge balance compensation.
     */
    ErrorCodes_t getBridgeBalanceCompensationOptions(
            E4GCL_ARGOUT std::vector <std::string> &options);

    /*! \brief Get the specifications of the control for the liquid junction.
     *
     * \param control [in] Specifications of the control for the liquid junction.
     * \return Success if the device implements liquid junction control.
     */
    ErrorCodes_t getLiquidJunctionControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the pipette capacitance.
     *
     * \param control [in] Specifications of the control for the pipette capacitance.
     * \return Success if the device implements pipette capacitance control.
     */
    ErrorCodes_t getPipetteCapacitanceControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the pipette capacitance for current clamp.
     *
     * \param control [in] Specifications of the control for the pipette capacitance for current clamp.
     * \return Success if the device implements pipette capacitance control for current clamp.
     */
    ErrorCodes_t getCCPipetteCapacitanceControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the membrane capacitance.
     *
     * \param control [in] Specifications of the control for the membrane capacitance.
     * \return Success if the device implements membrane capacitance control.
     */
    ErrorCodes_t getMembraneCapacitanceControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the access resistance.
     *
     * \param control [in] Specifications of the control for the access resistance.
     * \return Success if the device implements access resistance control.
     */
    ErrorCodes_t getAccessResistanceControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the resistance correction percentage.
     *
     * \param control [in] Specifications of the control for the resistance correction percentage.
     * \return Success if the device implements resistance correction percentage control.
     */
    ErrorCodes_t getResistanceCorrectionPercentageControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the resistance correction lag.
     *
     * \param control [in] Specifications of the control for the resistance correction lag.
     * \return Success if the device implements resistance correction lag control.
     */
    ErrorCodes_t getResistanceCorrectionLagControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the resistance prediction gain.
     *
     * \param control [in] Specifications of the control for the resistance prediction gain.
     * \return Success if the device implements resistance prediction gain control.
     */
    ErrorCodes_t getResistancePredictionGainControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the resistance prediction percentage.
     *
     * \param control [in] Specifications of the control for the resistance prediction percentage.
     * \return Success if the device implements resistance prediction percentage control.
     */
    ErrorCodes_t getResistancePredictionPercentageControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the resistance prediction bandwidth gain.
     *
     * \param control [in] Specifications of the control for the resistance prediction bandwidth gain.
     * \return Success if the device implements resistance prediction bandwidth gain control.
     */
    ErrorCodes_t getResistancePredictionBandwidthGainControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the resistance prediction tau.
     *
     * \param control [in] Specifications of the control for the resistance prediction tau.
     * \return Success if the device implements resistance prediction tau control.
     */
    ErrorCodes_t getResistancePredictionTauControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the leak conductance.
     *
     * \param control [in] Specifications of the control for the leak conductance.
     * \return Success if the device implements resistance prediction tau control.
     */
    ErrorCodes_t getLeakConductanceControl(
            E4GCL_ARGOUT CompensationControl_t &control);

    /*! \brief Get the specifications of the control for the bridge balance resistance.
     *
     * \param control [in] Specifications of the control for the bridge balance resistance.
     * \return Success if the device implements bridge balance resistance control.
     */
    ErrorCodes_t getBridgeBalanceResistanceControl(
            E4GCL_ARGOUT CompensationControl_t &control);

private:
    MessageDispatcher * messageDispatcher = nullptr;
};
}

#endif // E4GCOMMLIB_H
