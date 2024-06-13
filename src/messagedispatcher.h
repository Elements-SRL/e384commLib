#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#define _USE_MATH_DEFINES

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <cmath>

#include "e384commlib_errorcodes.h"
#include "e384commlib_global.h"
#include "boardmodel.h"
#include "utils.h"

#ifdef E384COMMLIB_LABVIEW_WRAPPER
#include "e384commlib_global_addendum.h"
#endif

#define SHORT_OFFSET_BINARY (static_cast <double> (0x8000))
#define SHORT_MAX (static_cast <double> (0x7FFF))
#define SHORT_MIN (-SHORT_MAX-1.0)
#define USHORT_MAX (static_cast <double> (0xFFFF))
#define UINT10_MAX (static_cast <double> (0x3FF))
#define UINT13_MAX (static_cast <double> (0x1FFF))
#define INT13_MAX (static_cast <double> (0x0FFF))
#define INT14_MAX (static_cast <double> (0x1FFF))
#define UINT14_MAX (static_cast <double> (0x3FFF))
#define INT18_MAX (static_cast <double> (0x1FFFF))
#define INT24_MAX (static_cast <double> (0x7FFFFF))
#define INT24_MIN (-INT24_MAX-1.0)
#define UINT24_MAX (static_cast <double> (0xFFFFFF))
#define UINT28_MAX (static_cast <double> (0xFFFFFFF))
#define INT28_MAX (static_cast <double> (0x7FFFFFF))
#define INT28_MIN (-INT28_MAX-1.0)
#define LUINT32_MAX (static_cast <double> (0xFFFFFFFF))
#define LINT32_MAX (static_cast <double> (0x7FFFFFFF))
#define LINT32_MIN (-LINT32_MAX-1.0)

#ifdef USE_2ND_ORDER_BUTTERWORTH
#define IIR_ORD 2
#define IIR_2_SIN_PI_4 (2.0*sin(M_PI/4.0))
#define IIR_2_COS_PI_4 (2.0*cos(M_PI/4.0))
#define IIR_2_COS_PI_4_2 (IIR_2_COS_PI_4*IIR_2_COS_PI_4)
#define FILTER_CLIP_NEEDED
#else /*! 1st order iir */
#define IIR_ORD 1
#endif

#define RX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define RX_32WORD_SIZE (sizeof(uint32_t)) // 16 bit word
#define RX_FEW_PACKETS_COEFF 0.01 /*!< = 10.0/1000.0: 10.0 because I want to get data once every 10ms, 1000 to convert sampling rate from Hz to kHz */
#define RX_MAX_BYTES_TO_WAIT_FOR 16384
#define RX_MSG_BUFFER_SIZE 0x10000 // ~64k
#define RX_MSG_BUFFER_MASK (RX_MSG_BUFFER_SIZE-1)
#define RX_DATA_BUFFER_SIZE 0x10000000 /*! ~256M The biggest data frame possible has a dataload of 1024 words (4 x 10MHz current frame)
                                           So this buffer has to be at least 1024 times bigger than RX_MSG_BUFFER_SIZE */
#define RX_DATA_BUFFER_MASK (RX_DATA_BUFFER_SIZE-1)

#define TX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define TX_MSG_BUFFER_SIZE 0x100 /*!< Number of messages. Always use a power of 2 for efficient circular buffer management through index masking */
#define TX_MSG_BUFFER_MASK (TX_MSG_BUFFER_SIZE-1)
#define TX_MAX_WRITE_TRIES 10

#ifndef E384COMMLIB_LABVIEW_WRAPPER
using namespace e384CommLib;
#endif

typedef struct MsgResume {
    uint16_t typeId;
    uint16_t heartbeat;
    uint32_t dataLength;
    uint32_t startDataPtr;
} MsgResume_t;

class E384COMMLIBSHARED_EXPORT MessageDispatcher {
public:

    /*****************\
     *  Ctor / Dtor  *
    \*****************/

    MessageDispatcher(std::string deviceId);
    virtual ~MessageDispatcher();

    typedef enum CompensationTypes {
        CompCfast = 0,      // pipette voltage clamp
        CompCslow = 1,      // membrane
        CompRsComp = 2,     // rseries compensation
        CompRsCorr = 3,     // rseries correction
        CompRsPred = 4,     // rseries prediction
        CompGLeak = 5,      // leakage conductance
        VCCompensationsNum = 6,
        CompCcCfast = 6,    // pipette current clamp
        CompBridgeRes = 7,  // bridge balance
        CompensationTypesNum = 8
    } CompensationTypes_t;

    typedef enum CompensationUserParams {
        U_CpVc,     // VCPipetteCapacitance
        U_Cm,       // MembraneCapacitance
        U_Rs,       // SeriesResistance
        U_RsCp,     // SeriesCorrectionPerc
        U_RsCl,     // SeriesCorrectionLag
        U_RsPg,     // SeriesPredictionGain
        U_RsPp,     // SeriesPredictionPerc
        U_RsPt,     // SeriesPredictionTau
        U_LkG,      // LeakConductance
        U_CpCc,     // CCPipetteCapacitance
        U_BrB,      // CCBridgeBalance
        CompensationUserParamsNum
    } CompensationUserParams_t;

    typedef struct FwUpgradeInfo { /*! Defaults to "no upgrades available" */
        bool available = false;
        unsigned char fwVersion = 0xFF;
        std::string fwName = "";
    } FwUpgradeInfo_t;

    /************************\
     *  Connection methods  *
    \************************/

    /*! \brief Detects plugged in devices.
     *
     * \param deviceIds [out] List of plugged in devices IDs.
     * \return Error code.
     */
    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);

    /*! \brief Get information about a connected device.
     *
     * \param deviceId [in] Serial number of the device.
     * \param deviceVersion [out] Version of the device (device family). -1 if not available.
     * \param deviceSubVersion [out] Subversion of the device (increases with PCB changes). -1 if not available.
     * \param fwVersion [out] Version of the firmware (increases with device's firmware). -1 if not available.
     * \note The available device versions with the corresponding devices sub versions are found
     *       as enums in some header files, more specifically, devices/EMCR/emcrudbdevice.h and
     *       devices/EZPatch/ftdieeprom.h
     * \return Error code.
     */
    static ErrorCodes_t getDeviceInfo(std::string deviceId, unsigned int &deviceVersion, unsigned int &deviceSubVersion, unsigned int &fwVersion);

    /*! \brief Connects to a specific device
     * Calling this method if a device is already connected will return an error code.
     *
     * \param deviceId [in] Device ID of the device to connect to.
     * \param messageDispatcher [out] Class to control the device.
     * \param fwPathIn [in] Path of the Firmware file (empty string if it is in the same folder as the application that calls the library).
     * \return Error code.
     */
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = UTL_DEFAULT_FW_PATH);
    static ErrorCodes_t isDeviceUpgradable(std::string deviceId);
    static ErrorCodes_t upgradeDevice(std::string deviceId);
    virtual ErrorCodes_t initialize(std::string fwPath) = 0;
    virtual void deinitialize() = 0;

    /*! \brief Disconnects from connected device.
     * Calling this method if no device is connected will return an error code.
     *
     * \return Error code.
     */
    virtual ErrorCodes_t disconnectDevice() = 0;

    /*! \brief Enables or disables message types, so that disabled messages are not returned by getNextMessage
     *  \note Message types are available in e384comllib_global.h.
     *
     * \param messageType [in] Message type to enable or disable.
     * \param flag [in] true to enable the message type, false to disable it.
     * \return Error code.
     */
    virtual ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag) = 0;

    /***************************\
     *  Configuration methods  *
    \***************************/

    ErrorCodes_t setChannelSelected(uint16_t chIdx, bool newState);
    ErrorCodes_t setBoardSelected(uint16_t brdIdx, bool newState);
    ErrorCodes_t setRowSelected(uint16_t rowIdx, bool newState);
    ErrorCodes_t getChannelsOnRow(uint16_t rowIdx, std::vector<ChannelModel *> &channels);
    ErrorCodes_t setAllChannelsSelected(bool newState);
    ErrorCodes_t getChannelsOnBoard(uint16_t boardIdx, std::vector <ChannelModel *> &channels);

    /****************\
     *  Tx methods  *
    \****************/

    /*! \brief Forces the commands to be sent to the device.
     *
     * \return Error code.
     */
    virtual ErrorCodes_t sendCommands();

    /*! \brief Start the protocol defined with the setVoltageProtocolStructure or setCurrentProtocolStructure method.
     *
     * \return Error code.
     */
    virtual ErrorCodes_t startProtocol();

    /*! \brief Stops the currently applied protocol.
     *
     * \return Error code.
     */
    virtual ErrorCodes_t stopProtocol();
    virtual ErrorCodes_t startStateArray();

    /*! \brief Reset the device's ASIC.
     *
     * \param reset [in] False sets the ASIC in normal operation state, true sets in reset state.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t resetAsic(bool resetFlag, bool applyFlag = true);

    /*! \brief Reset the device's FPGA.
     *
     * \param reset [in] False sets the FPGA in normal operation state, true sets in reset state.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t resetFpga(bool resetFlag, bool applyFlag = true);

    /*! \brief Set the holding voltage tuner. This value is added to the whole voltage protocol currently applied and to the following.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param voltages [in] Vector of voltage offsets.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag);

    /*! \brief Set the holding current tuner. This value is added to the whole current protocol currently applied and to the following.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param currents [in] Vector of current offsets.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag);

    /*! \brief Set the channel voltage half. This value is added to the voltage protocol items that have the vHalfFlag set.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param voltages [in] Vector of voltage halfs.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setVoltageHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag);

    /*! \brief Set the channel current half. This value is added to the current protocol items that have the cHalfFlag set.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param currents [in] Vector of current halfs.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCurrentHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag);

    /*! \brief Set the liquid junction voltage. Contrarily to the voltage hold tuner, this voltage contribute is not reflected in the voltage readout
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param voltages [in] Vector of voltage offsets.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag);

    /*! \brief Set the liquid junction voltage to 0.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t resetLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set the gate voltage on a specific board.
     *
     * \param boardIndexes [in] Vector of Indexes for the boards to control.
     * \param gateVoltages [in] Vector of gate voltages.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setGateVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag);

    /*! \brief Set the source voltage on a specific board.
     *
     * \param boardIndexes [in] Vector of Indexes for the boards to control.
     * \param sourceVoltages [in] Vector of gate voltages.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setSourceVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag);

    virtual ErrorCodes_t setCalibParams(CalibrationParams_t calibParams);

    /*! \brief Set a VC current gain on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param gains [in] Vector of current gains.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set a VC current offset on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param offsets [in] Vector of current offsets.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set a VC Voltage gain on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param gains [in] Vector of current gains.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set a VC voltage offset on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param offsets [in] Vector of current offsets.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set a CC voltage gain on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param gains [in] Vector of voltage gains.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set a CC voltage offset on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param offsets [in] Vector of voltage offsets.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set a CC current gain on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param gains [in] Vector of voltage gains.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set a CC current offset on a specific channel.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to control.
     * \param offsets [in] Vector of voltage offsets.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibRShuntConductance(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> conductances, bool applyFlag);
    virtual ErrorCodes_t updateCalibRShuntConductance(std::vector<uint16_t> channelIndexes, bool applyFlag);

    /*! \brief Set the current range for voltage clamp.
     *
     * \param currentRangeIdx [in] Index of the current range to be set.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag);

    /*! \brief Set the voltage range for voltage clamp.
     *
     * \param voltageRangeIdx [in] Index of the voltage range to be set.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag);

    /*! \brief Set the current range for current clamp.
     *
     * \param currentRangeIdx [in] Index of the current range to be set.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag);

    /*! \brief Set the voltage range for current clamp.
     *
     * \param voltageRangeIdx [in] Index of the voltage range to be set.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag);
    virtual ErrorCodes_t setLiquidJunctionRange(uint16_t idx);

    /*! \brief Sets the low pass filter on the voltage stimulus.
     *
     * \param filterIdx [in] Index of the desired filtering option.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlag);

    /*! \brief Sets the low pass filter on the current stimulus.
     *
     * \param filterIdx [in] Index of the desired filtering option.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlag);

    /*! \brief Enable or disable the stimulus on a specific channel.
     *
     * \param channelIndexes [in] Vector of indexes of the channel to control.
     * \param onValues [in] Vector of on values: True to turn the stimulus on, false to turn it off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    /*! \brief Turns on and off a specific channel.
     *
     * \param channelIndexes [in] Vector of indexes of the channel to control.
     * \param onValues [in] Vector of on values: True to turn the channel on (close the IN SWITCH), false to turn it off (open the SWITCH).
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t turnChannelsOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    /*! \brief Open and closes the calibration switch for a specific channel.
     *
     * \param channelIndexes [in] Vector of indexes of the channel to control.
     * \param onValues [in] Vector of on values: True to close the channel CALIBRATION SWITCH, false to open it.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t turnCalSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t hasCalSw();
    virtual ErrorCodes_t turnVcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t turnCcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t turnVcCcSelOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    /*! \brief Enables or disables the stimulus in current clamp.
     *  \note The stimulus is disabled via a physical switch, not by digital means, so this allows the I0 current clamp mode
     *
     * \param channelIndexes [in] Vector of indexes of the channel to control.
     * \param onValues [in] Vector of on values: True to enable the stimulus, false to disable it.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t enableCcStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    virtual ErrorCodes_t setClampingModality(uint32_t idx, bool applyFlag, bool stopProtocolFlag);

    /*! \brief Select the clamping modality.
     *
     * \param mode [in] Index of the clamping modality to select.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \param stopProtocolFlag [in] true, to automatically stop the currently applied stimulation before changing modality.
     * \return Error code.
     */
    ErrorCodes_t setClampingModality(ClampingModality_t mode, bool applyFlag, bool stopProtocolFlag = true);

    /*! \brief Set the data sources for voltage channels.
     *
     * \param source [in] Index of the data source to be set for voltage channels.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setSourceForVoltageChannel(uint16_t source, bool applyFlag);

    /*! \brief Set the data sources for current channels.
     *
     * \param source [in] Index of the data source to be set for current channels.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setSourceForCurrentChannel(uint16_t source, bool applyFlag);

    /*! \brief Execute digital offset compensation.
     * Digital offset compensation tunes the offset of the applied voltage so that the
     * acquired current is 0.
     *
     * \param channelIndexes [in] Channel indexes.
     * \param onValues [in] Array of booleans, one for each channel: True to turn the offset compensation on, false to turn it off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t expandTraces(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues);

    virtual ErrorCodes_t setAdcFilter();

    /*! \brief Set the sampling rate.
     *
     * \param samplingRateIdx [in] Index of the sampling rate to be set.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag);

    /*! \brief Set the downsampling ratio.
     *
     * \param ratio [in] Decimation ratio.
     * \return Error code.
     */
    ErrorCodes_t setDownsamplingRatio(uint32_t ratio);

    /*! \brief Set a debug bit
     *
     * \param wordOffset [in] word of the debug bit to be modified.
     * \param bitOffset [in] debug bit to be modified.
     * \param status [in] new status of the debug bit.
     * \return Error code.
     */
    virtual ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status);

    /*! \brief Set a debug word
     *
     * \param wordOffset [in] word of the debug bit to be modified.
     * \param word value [in] new value for the debug word.
     * \return Error code.
     */
    virtual ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue);

    /*! \brief Turn on/off the voltage reader for each channel.
     *  \note The voltage is read by the current clamp ADC.
     *  \note In some devices the ADC can't be turned on independently of the DAC.
     *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
     *
     * \param onValue [in] True to turn the voltage reader on, false to turn it off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlag);

    /*! \brief Turn on/off the current reader for each channel.
     *  \note The current is read by the current clamp ADC.
     *  \note In some devices the ADC can't be turned on independently of the DAC.
     *  \note This only activates the circuitry: in order to have the device return the desired channels use #setChannelsSources.
     *
     * \param onValue [in] True to turn the current reader on, false to turn it off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlag);

    /*! \brief Turn on/off the voltage stimulus for each channel.
     *
     * \param onValue [in] True to turn the voltage stimulus on, false to turn it off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t turnVoltageStimulusOn(bool onValue, bool applyFlag);

    /*! \brief Turn on/off the current stimulus for each channel.
     *
     * \param onValue [in] True to turn the current stimulus on, false to turn it off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t turnCurrentStimulusOn(bool onValue, bool applyFlag);

    /*! \brief Describes the structure of an incoming voltage protocol.
     *
     * \param protId [in] Protocol identifier number.
     * \param itemsNum [in] Number of protocol items.
     * \param sweepsNum [in] Number of sweeps of the protocol.
     * \param vRest [in] Voltage that will be applied when the protocol ends.
     * \param stopProtocolFlag [in] true, to automatically stop the currently applied stimulation before applying the new one.
     * \note Each sweep increases the stepped parameter by 1 step.
     * \return Error code.
     */
    virtual ErrorCodes_t setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest, bool stopProtocolFlag = true);

    /*! \brief Commits a voltage protocol item consisting of a constant voltage.
     *  Steps can be defined for both voltage and duration to make them change at any iteration.
     *  Loops can also be defined to repeat a given sequence of items more than once,
     *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
     *  repsNum set to 3
     *
     * \param itemIdx [in] Index of the current protocol item.
     * \param nextItemIdx [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
     * \param loopReps [in] Number of loop repetitions before moving on.
     * \param applyStepsFlag [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
     * \param v0 [in] Initial voltage.
     * \param v0Step [in] Voltage step.
     * \param t0 [in] Initial duration.
     * \param t0Step [in] Duration step.
     * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
     * \note Items that do not end a loop must have \p nextItemIdx = \<actual item\> + 1 and \p loopReps = 1.
     * \return Error code.
     */
    virtual ErrorCodes_t setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag);

    /*! \brief Commits a voltage protocol item consisting of a voltage ramp.
     *  Loops can also be defined to repeat a given sequence of items more than once,
     *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
     *  repsNum set to 3
     *
     * \param itemIdx [in] Index of the current protocol item.
     * \param nextItemIdx [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
     * \param loopReps [in] Number of loop repetitions before moving on.
     * \param applyStepsFlag [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
     * \param v0 [in] Initial voltage.
     * \param v0Step [in] Voltage step.
     * \param vFinal [in] Final voltage.
     * \param vFinalStep [in] Voltage step.
     * \param t0 [in] Duration.
     * \param t0Step [in] Duration step.
     * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
     * \note Items that do not end a loop must have \p nextItemIdx = \<actual item\> + 1 and \p loopReps = 1.
     * \return Error code.
     */
    virtual ErrorCodes_t setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag);

    /*! \brief Commits a voltage protocol item consisting of a sinusoidal wave.
     *  Loops can also be defined to repeat a given sequence of items more than once,
     *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
     *  repsNum set to 3
     *
     * \param itemIdx [in] Index of the current protocol item.
     * \param nextItemIdx [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
     * \param loopReps [in] Number of loop repetitions before moving on.
     * \param applyStepsFlag [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
     * \param v0 [in] Voltage offset.
     * \param v0Step [in] Voltage step.
     * \param vAmp [in] Voltage amplitude.
     * \param vAmpStep [in] Voltage amplitude step.
     * \param f0 [in] Oscillation frequency.
     * \param f0Step [in] Oscillation frequency step.
     * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
     * \note Items that do not end a loop must have \p nextItemIdx = \<actual item\> + 1 and \p loopReps = 1.
     * \return Error code.
     */
    virtual ErrorCodes_t setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag);

    /*! \brief Describes the structure of an incoming current protocol.
     *
     * \param protId [in] Protocol identifier number.
     * \param itemsNum [in] Number of protocol items.
     * \param sweepsNum [in] Number of sweeps of the protocol.
     * \param iRest [in] Current that will be applied when the protocol ends.
     * \param stopProtocolFlag [in] true, to automatically stop the currently applied stimulation before applying the new one.
     * \note Each sweep increases the stepped parameter by 1 step.
     * \return Error code.
     */
    virtual ErrorCodes_t setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest, bool stopProtocolFlag);

    /*! \brief Commits a current protocol item consisting of a constant current.
     *  Steps can be defined for both current and duration to make them change at any iteration.
     *  Loops can also be defined to repeat a given sequence of items more than once,
     *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
     *  repsNum set to 3
     *
     * \param itemIdx [in] Index of the current protocol item.
     * \param nextItemIdx [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
     * \param loopReps [in] Number of loop repetitions before moving on.
     * \param applyStepsFlag [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
     * \param i0 [in] Initial current.
     * \param i0Step [in] Current step.
     * \param t0 [in] Initial duration.
     * \param t0Step [in] Duration step.
     * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
     * \note Items that do not end a loop must have \p nextItemIdx = \<actual item\> + 1 and \p loopReps = 1.
     * \return Error code.
     */
    virtual ErrorCodes_t setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag);

    /*! \brief Commits a current protocol item consisting of a current ramp.
     *  Loops can also be defined to repeat a given sequence of items more than once,
     *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
     *  repsNum set to 3
     *
     * \param itemIdx [in] Index of the current protocol item.
     * \param nextItemIdx [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
     * \param loopReps [in] Number of loop repetitions before moving on.
     * \param applyStepsFlag [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
     * \param i0 [in] Initial current.
     * \param i0Step [in] Current step.
     * \param iFinal [in] Final current.
     * \param iFinalStep [in] Current step.
     * \param t0 [in] Duration.
     * \param t0Step [in] Duration step.
     * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
     * \note Items that do not end a loop must have \p nextItemIdx = \<actual item\> + 1 and \p loopReps = 1.
     * \return Error code.
     */
    virtual ErrorCodes_t setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag);

    /*! \brief Commits a current protocol item consisting of a sinusoidal wave.
     *  Loops can also be defined to repeat a given sequence of items more than once,
     *  e.g. sequence 0123232345 can be reduced to 012345 where item 3 has \p nextItem set to 2 and
     *  repsNum set to 3
     *
     * \param itemIdx [in] Index of the current protocol item.
     * \param nextItemIdx [in] Index of the protocol item that will follow, i.e. used as a goto to create loops.
     * \param loopReps [in] Number of loop repetitions before moving on.
     * \param applyStepsFlag [in] 0x0: each repetition is a replica; 0x1 each repetition increases stepped parameters by 1 step.
     * \param i0 [in] Current offset.
     * \param i0Step [in] Current step.
     * \param iAmp [in] Current amplitude.
     * \param iAmpStep [in] Current amplitude step.
     * \param f0 [in] Oscillation frequency.
     * \param f0Step [in] Oscillation frequency step.
     * \param vHalfFlag [in] 0x0: do not add vHalfFlag to this item; 0x1 add vHalfFlag to this item.
     * \note Items that do not end a loop must have \p nextItemIdx = \<actual item\> + 1 and \p loopReps = 1.
     * \return Error code.
     */
    virtual ErrorCodes_t setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag);

    virtual ErrorCodes_t setStateArrayStructure(int numberOfStates, int initialState, Measurement_t reactionTime);
    virtual ErrorCodes_t setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, double timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag);
    virtual ErrorCodes_t setStateArrayEnabled(int chIdx, bool enabledFlag);

    /*! \brief Turn on/off a specific compensation.
     *
     * \param channelIndexes [in] Channel indexes.
     * \param type [in] compensation type to enable.
     * \param onValues [in] Array of booleans, one for each channel: True to turn the compensation on, false to turn it off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t enableCompensation(std::vector<uint16_t> channelIndexes, CompensationTypes_t type, std::vector<bool> onValues, bool applyFlag);

    /*! \brief Turn on/off the voltage clamp compesantions for each channel.
     *
     * \param onValue [in] True to turn the voltage clamp compensations on, false to turn them off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t enableVcCompensations(bool enable, bool applyFlag);

    /*! \brief Turn on/off the current clamp compesantions for each channel.
     *
     * \param onValue [in] True to turn the current clamp compensations on, false to turn them off.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t enableCcCompensations(bool enable, bool applyFlag);

    /*! \brief Sets the value for a specific compensation.
     *
     * \param channelIndexes [in] Array/vector of channel indexes.
     * \param paramToUpdate [in] Compensation parameter to be updated.
     * \param newParamValues [in] Array/vector of compensation values.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCompValues(std::vector<uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <double> newParamValues, bool applyFlag);

    /*! \brief Sets the range for a specific compensation when more than one is available.
     *  \note If an invalide range index is provided than the range is selected automatically based on the compensation value.
     *
     * \param channelIndexes [in] Array/vector of channel indexes.
     * \param paramToUpdate [in] Compensation parameter to be updated.
     * \param newRanges [in] Array/vector of compensation range indexes.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCompRanges(std::vector<uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <uint16_t> newRanges, bool applyFlag);

    /*! \brief Set options for a specific compensation.
     *
     * \param channelIndexes [in] Channel indexes.
     * \param type [in] compensation type to configure.
     * \param optionIndexes [in] Option indexes.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCompOptions(std::vector<uint16_t> channelIndexes, CompensationTypes_t type, std::vector <uint16_t> options, bool applyFlag);

    /*! Device specific controls */

    /*! \brief Set a custom flag.
     *
     * \param idx [in] Index of the flag to be set.
     * \param flag [in] Value for the flag.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCustomFlag(uint16_t idx, bool flag, bool applyFlag);

    /*! \brief Set a custom multivalued option.
     *
     * \param idx [in] Index of the option to be set.
     * \param value [in] Value for the option.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCustomOption(uint16_t idx, uint16_t value, bool applyFlag);

    /*! \brief Set a custom double value.
     *
     * \param idx [in] Index of the value to be set.
     * \param value [in] Value to be set.
     * \param applyFlag [in] true: immediately submit the command to the device; false: submit together with the next command.
     * \return Error code.
     */
    virtual ErrorCodes_t setCustomDouble(uint16_t idx, double value, bool applyFlag);

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t getSerialNumber(std::string &serialNumber);

    ErrorCodes_t getBoards(std::vector <BoardModel *> &boards);
    ErrorCodes_t getChannels(std::vector <ChannelModel *> &channels);
    ErrorCodes_t getSelectedChannels(std::vector <bool> &selected);
    ErrorCodes_t getSelectedChannelsIndexes(std::vector <uint16_t> &indexes);

    /*! \brief Get the size of the buffer to be passed to getNextMessage.
     *
     * \param size [out] Size of the buffer to be passed to getNextMessage.
     * \return Error code.
     */
    ErrorCodes_t getRxDataBufferSize(uint32_t &size);
    ErrorCodes_t allocateRxDataBuffer(int16_t * &data);
    ErrorCodes_t deallocateRxDataBuffer(int16_t * &data);

    /*! \brief Get the next message from the queue sent by the connected device.
     *
     * \param rxOutput [out] Struct containing info on the received message.
     * \param data [out] array of output data.
     * \return Error code.
     */
    virtual ErrorCodes_t getNextMessage(RxOutput_t &rxOutput, int16_t * data);

    /*! \brief Purge buffered data.
     *
     * \return Error code.
     */
    virtual ErrorCodes_t purgeData();
    ErrorCodes_t convertVoltageValue(int16_t intValue, double &fltValue);
    ErrorCodes_t convertCurrentValue(int16_t intValue, double &fltValue);
    ErrorCodes_t convertVoltageValues(int16_t * intValue, double * fltValue, int valuesNum);
    ErrorCodes_t convertCurrentValues(int16_t * intValue, double * fltValue, int valuesNum);

    /*! \brief Get the voltage currently corrected by liquid junction compensation.
     *
     * \param channelIndexes [in] Vector of Indexes for the channels to get the votlage from.
     * \param voltages [out] Array of compensated voltages.
     * \return Error code.
     */
    ErrorCodes_t getLiquidJunctionVoltages(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> &voltages);
    ErrorCodes_t getLiquidJunctionStatuses(std::vector<uint16_t> channelIndexes, std::vector<LiquidJunctionStatus_t> &statuses);

    /*! \brief Get the voltage hold tuner features, e.g. ranges, step, ...
     *
     * \param voltageHoldTuner [out] Vector of ranges for VoltageHoldTuner in each stimulus range.
     * \return Error code.
     */
    virtual ErrorCodes_t getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTuner);

    /*! \brief Get the voltage half features, e.g. ranges, step, ...
     *
     * \param voltageHalf [out] Vector of ranges for VoltageHalf in each stimulus range.
     * \return Error code.
     */
    virtual ErrorCodes_t getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTuner);

    /*! \brief Get the current hold tuner features, e.g. ranges, step, ...
     *
     * \param currentHoldTuner [out] Vector of ranges for CurrentHoldTuner in each stimulus range.
     * \return Error code.
     */
    virtual ErrorCodes_t getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTuner);

    /*! \brief Get the current half features, e.g. ranges, step, ...
     *
     * \param currentHalfTuner [out] Vector of ranges for CurrentHalf in each stimulus range.
     * \return Error code.
     */
    virtual ErrorCodes_t getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTuner);
    virtual ErrorCodes_t getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges);
    virtual ErrorCodes_t getCalibVcCurrentGainFeatures(RangedMeasurement_t &calibVcCurrentGainFeatures);
    virtual ErrorCodes_t getCalibVcCurrentOffsetFeatures(std::vector<RangedMeasurement_t> &calibVcCurrentOffsetFeatures);
    virtual ErrorCodes_t getCalibCcVoltageGainFeatures(RangedMeasurement_t &calibCcVoltageGainFeatures);
    virtual ErrorCodes_t getCalibCcVoltageOffsetFeatures(std::vector<RangedMeasurement_t> &calibCcVoltageOffsetFeatures);
    virtual ErrorCodes_t hasGateVoltages();
    virtual ErrorCodes_t hasSourceVoltages();
    virtual ErrorCodes_t getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures);
    virtual ErrorCodes_t getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures);

    /*! \brief Get the number of channels for the device.
     *
     * \param voltageChannelNumber [out] Number of voltage channels.
     * \param currentChannelNumber [out] Number of current channels.
     * \return Error code.
     */
    ErrorCodes_t getChannelNumberFeatures(uint16_t &voltageChannelNumber, uint16_t &currentChannelNumber);

    /*! \brief Get the number of channels for the device.
     *
     * \param voltageChannelNumber [out] Number of voltage channels.
     * \param currentChannelNumber [out] Number of current channels.
     * \return Error code.
     */
    ErrorCodes_t getChannelNumberFeatures(int &voltageChannelNumber, int &currentChannelNumber);

    /*! \brief Get the available data sources for all channels type.
     * \note Unavailable sources have index -1.
     *
     * \param voltageSourcesIdxs [out] Indexes of the available data sources to be used for voltage channels.
     * \param currentSourcesIdxs [out] Indexes of the available data sources to be used for current channels.
     * \return Error code.
     */
    ErrorCodes_t getAvailableChannelsSourcesFeatures(ChannelSources_t &voltageSourcesIdxs, ChannelSources_t &currentSourcesIdxs);

    /*! \brief Get the number of boards for the device.
     *
     * \param boardsNumber [out] Number of current channels.
     * \return Error code.
     */
    ErrorCodes_t getBoardsNumberFeatures(uint16_t &boardsNumber);

    /*! \brief Get the number of boards for the device.
     *
     * \param boardsNumber [out] Number of current channels.
     * \return Error code.
     */
    ErrorCodes_t getBoardsNumberFeatures(int &boardsNumberFeatures);

    /*! \brief Get the clamping modalities available for the device.
     *
     * \param clampingModalities [out] Array containing all the available clamping modalities.
     * \return Error code.
     */
    ErrorCodes_t getClampingModalitiesFeatures(std::vector<ClampingModality_t> &clampingModalities);
    ErrorCodes_t getClampingModality(ClampingModality_t &clampingModality);
    ErrorCodes_t getClampingModalityIdx(uint32_t &idx);

    /*! \brief Get the current ranges available in voltage clamp for the device.
     *
     * \param currentRanges [out] Array containing all the available current ranges in voltage clamp.
     * \param defaultVcCurrRangeIdx [out] Default index
     * \return Error code.
     */
    ErrorCodes_t getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges, uint16_t &defaultVcCurrRangeIdx);

    /*! \brief Get the voltage ranges available in voltage clamp for the device.
     *
     * \param voltageRanges [out] Array containing all the available voltage ranges in voltage clamp.
     * \return Error code.
     */
    ErrorCodes_t getVCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges);

    /*! \brief Get the current ranges available in current clamp for the device.
     *
     * \param currentRanges [out] Array containing all the available current ranges in current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges);

    /*! \brief Get the voltage ranges available in current clamp for the device.
     *
     * \param voltageRanges [out] Array containing all the available voltage ranges in current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges);

    /*! \brief Get the current range currently applied for voltage clamp.
     *
     * \param range [out] Current range currently applied for voltage clamp.
     * \return Error code.
     */
    ErrorCodes_t getVCCurrentRange(RangedMeasurement_t &range);

    /*! \brief Get the voltage range currently applied for voltage clamp.
     *
     * \param range [out] Voltage range currently applied for voltage clamp.
     * \return Error code.
     */
    ErrorCodes_t getVCVoltageRange(RangedMeasurement_t &range);
    ErrorCodes_t getLiquidJunctionRange(RangedMeasurement_t &range);

    /*! \brief Get the current range currently applied for current clamp.
     *
     * \param range [out] Current range currently applied for current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCCurrentRange(RangedMeasurement_t &range);

    /*! \brief Get the voltage range currently applied for current clamp.
     *
     * \param range [out] Voltage range currently applied for current clamp.
     * \return Error code.
     */
    ErrorCodes_t getCCVoltageRange(RangedMeasurement_t &range);

    ErrorCodes_t getVCCurrentRangeIdx(uint32_t &idx);
    ErrorCodes_t getVCVoltageRangeIdx(uint32_t &idx);
    ErrorCodes_t getCCCurrentRangeIdx(uint32_t &idx);
    ErrorCodes_t getCCVoltageRangeIdx(uint32_t &idx);

    // Get the voltage in use (could be any clamping modality)
    ErrorCodes_t getVoltageRange(RangedMeasurement_t &range);
    // Get the current range in use (could be any clamping modality)
    ErrorCodes_t getCurrentRange(RangedMeasurement_t &range);

    ErrorCodes_t getMaxVCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinVCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMaxVCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinVCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMaxCCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinCCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMaxCCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinCCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);

    /*! \brief Get the sampling rates available for the device.
     *
     * \param samplingRates [out] Array containing all the available sampling rates.
     * \return Error code.
     */
    ErrorCodes_t getSamplingRatesFeatures(std::vector <Measurement_t> &samplingRates);
    ErrorCodes_t getSamplingRate(Measurement_t &samplingRate);
    ErrorCodes_t getSamplingRateIdx(uint32_t &idx);

    /*! \brief Get the real sampling rates available for the device.
     *
     * \param samplingRates [out] Array containing all the available real sampling rates
     *                            (may slightly differ from displayed sampling rates).
     * \return Error code.
     */
    ErrorCodes_t getRealSamplingRatesFeatures(std::vector <Measurement_t> &realSamplingRates);
    ErrorCodes_t getMaxDownsamplingRatioFeature(uint32_t &ratio);
    ErrorCodes_t getDownsamplingRatio(uint32_t &ratio);

    /*! \brief Get the available options for the voltage stimulus low pass filter.
     *
     * \param filters [out] Available options for the voltage stimulus low pass filter.
     * \return Error code.
     */
    ErrorCodes_t getVCVoltageFilters(std::vector <Measurement_t> &filters);
    ErrorCodes_t getVCCurrentFilters(std::vector <Measurement_t> &filters);
    ErrorCodes_t getCCVoltageFilters(std::vector <Measurement_t> &filters);

    /*! \brief Get the available options for the current stimulus low pass filter.
     *
     * \param filters [out] Available options for the current stimulus low pass filter.
     * \return Error code.
     */
    ErrorCodes_t getCCCurrentFilters(std::vector <Measurement_t> &filters);

    ErrorCodes_t getVCVoltageFilter(Measurement_t &filter);
    ErrorCodes_t getVCCurrentFilter(Measurement_t &filter);
    ErrorCodes_t getCCVoltageFilter(Measurement_t &filter);
    ErrorCodes_t getCCCurrentFilter(Measurement_t &filter);

    ErrorCodes_t getVCVoltageFilterIdx(uint32_t &idx);
    ErrorCodes_t getVCCurrentFilterIdx(uint32_t &idx);
    ErrorCodes_t getCCVoltageFilterIdx(uint32_t &idx);
    ErrorCodes_t getCCCurrentFilterIdx(uint32_t &idx);

    virtual ErrorCodes_t hasChannelSwitches();
    virtual ErrorCodes_t hasStimulusSwitches();
    virtual ErrorCodes_t hasOffsetCompensation();
    virtual ErrorCodes_t hasStimulusHalf();

    /*! \brief Get the voltage range for voltage protocols.
     *
     * \param rangeIdx [in] Index of the voltage range to get info about.
     * \param range [out] Structure containing min voltage, max voltage and voltage step.
     * \return Error code.
     */
    ErrorCodes_t getVoltageProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range);

    /*! \brief Get the current range for current protocols.
     *
     * \param rangeIdx [in] Index of the current range to get info about.
     * \param range [out] Structure containing min current, max current and current step.
     * \return Error code.
     */
    ErrorCodes_t getCurrentProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range);

    /*! \brief Get the time range for voltage and current protocols.
     *
     * \param range [out] Structure containing min time, max time and time step.
     * \return Error code.
     */
    ErrorCodes_t getTimeProtocolRangeFeature(RangedMeasurement_t &range);

    /*! \brief Get the frequency range for voltage and current sinusoidal protocols.
     *
     * \param range [out] Structure containing min frequency, max frequency and frequency step.
     * \return Error code.
     */
    ErrorCodes_t getFrequencyProtocolRangeFeature(RangedMeasurement_t &range);

    /*! \brief Tell how many protocol items the device can memorize.
     *
     * \param num [out] Maximum number of protocol items.
     * \return Error code.
     */
    ErrorCodes_t getMaxProtocolItemsFeature(uint32_t &num);
    ErrorCodes_t hasProtocols();

    /*! \brief Tell if the device implements step protocol items.
     *
     * \return Success if the device implements step protocol items.
     */
    ErrorCodes_t hasProtocolStepFeature();

    /*! \brief Tell if the device implements ramp protocol items.
     *
     * \return Success if the device implements ramp protocol items.
     */
    ErrorCodes_t hasProtocolRampFeature();

    /*! \brief Tell if the device implements sin protocol items.
     *
     * \return Success if the device implements sin protocol items.
     */
    ErrorCodes_t hasProtocolSinFeature();
    virtual ErrorCodes_t isStateArrayAvailable();

    /*! \brief Get a structure containing the calibration parameters.
     *
     * \param calibParams [out] calibration parameters.
     * \return Error code.
     */
    virtual ErrorCodes_t getCalibParams(CalibrationParams_t &calibParams);
    virtual ErrorCodes_t getCalibFileNames(std::vector<std::string> &calibFileNames);
    virtual ErrorCodes_t getCalibFilesFlags(std::vector<std::vector <bool>> &calibFilesFlags);
    virtual ErrorCodes_t getCalibMappingFileDir(std::string &dir);
    virtual ErrorCodes_t getCalibMappingFilePath(std::string &path);

    ErrorCodes_t hasCompFeature(CompensationUserParams_t feature);
    virtual ErrorCodes_t getCompFeatures(CompensationUserParams_t feature, std::vector<RangedMeasurement_t> &compensationFeatures, double &defaultParamValue);

    /*! \brief Get options for the selected compensation type.
     *
     * \param type [in]: Compensation type, e.g. pipette compensation.
     * \param options [out]: vector of strings of the available options.
     * \return Success if the device has options for the selected compensation type.
     */
    virtual ErrorCodes_t getCompOptionsFeatures(CompensationTypes_t type, std::vector <std::string> &compOptionsArray);
    ErrorCodes_t getCompValueMatrix(std::vector<std::vector<double>> &matrix);
    virtual ErrorCodes_t getCompensationEnables(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> &onValues);

    /*! \brief Get the specifications of the control for the selected compensation parameter.
     *
     * \param param [in] Compensation parameter, e.g. pipette capacitance.
     * \param control [out] Specifications of the control for the selected parameter.
     * \return Success if the device implements the selected parameter control.
     */
    virtual ErrorCodes_t getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control);

    virtual ErrorCodes_t getAccessResistanceCorrectionLag(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getAccessResistancePredictionPercentage(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getAccessResistancePredictionTau(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getLeakConductance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getBridgeBalanceResistance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);

    /*! Device specific controls */

    ErrorCodes_t getCustomFlags(std::vector <std::string> &customFlags, std::vector <bool> &customFlagsDefault);
    ErrorCodes_t getCustomOptions(std::vector <std::string> &customOptions, std::vector <std::vector <std::string>> &customOptionsDescriptions, std::vector <uint16_t> &customOptionsDefault);
    ErrorCodes_t getCustomDoubles(std::vector <std::string> &customDoubles, std::vector <RangedMeasurement_t> &customDoublesRanges, std::vector <double> &customDoublesDefault);

protected:
    typedef enum RxMessageTypes {
        RxMessageDataLoad,
        RxMessageVoltageThenCurrentDataLoad,
        RxMessageCurrentDataLoad,
        RxMessageVoltageDataLoad,
        RxMessageDataHeader,
        RxMessageDataTail,
        RxMessageStatus,
        RxMessageNum
    } RxMessageTypes_t;

    typedef enum LiquidJunctionState {
        LiquidJunctionIdle,
        LiquidJunctionStarting,
        LiquidJunctionFirstStep,
        LiquidJunctionConverge,
        LiquidJunctionSuccess,
        LiquidJunctionFailOpenCircuit,
        LiquidJunctionFailTooManySteps,
        LiquidJunctionFailSaturation,
        LiquidJunctionTerminate,
        LiquidJunctionStatesNum
    } LiquidJunctionState_t;

    /*************\
     *  Methods  *
    \*************/

    void createDebugFiles();
    virtual ErrorCodes_t startCommunication(std::string fwPath) = 0;
    virtual ErrorCodes_t initializeMemory() = 0;
    virtual void initializeVariables();
    virtual ErrorCodes_t deviceConfiguration();
    virtual void createCommunicationThreads() = 0;
    virtual ErrorCodes_t initializeHW() = 0;

    virtual ErrorCodes_t stopCommunication() = 0;
    virtual void deinitializeMemory() = 0;
    virtual void deinitializeVariables();

    void closeDebugFiles();
    virtual void joinCommunicationThreads() = 0;

    void computeLiquidJunction();
    virtual void initializeCalibration();
    void initializeLiquidJunction();

    bool checkProtocolValidity(std::string &message);

    void initializeRawDataFilterVariables();
    void deInitializeRawDataFilterVariables();
    void computeRawDataFilterCoefficients();
    double applyRawDataFilter(uint16_t channelIdx, double x, double * iirNum, double * iirDen);

    virtual std::vector<double> user2AsicDomainTransform(int chIdx, std::vector<double> userDomainParams);
    virtual std::vector<double> asic2UserDomainTransform(int chIdx, std::vector<double> asicDomainParams, double oldUCpVc, double oldUCpCc);
    virtual ErrorCodes_t asic2UserDomainCompensable(int chIdx, std::vector<double> asicDomainParams, std::vector<double> userDomainParams);

    void fillBoardList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard);
    void fillChannelList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard);

    void flushBoardList();

    /************\
     *  Fields  *
    \************/

    std::string upgradeNotes = "";
    std::string notificationTag = "UNDEFINED";

    uint16_t voltageChannelsNum = 1;
    uint16_t currentChannelsNum = 1;
    uint16_t totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    ChannelSources_t availableVoltageSourcesIdxs;
    ChannelSources_t availableCurrentSourcesIdxs;

    uint16_t totalBoardsNum = 1;
    uint16_t channelsPerBoard = 1;

    /*! 20230531 MPAC: state array params*/
    unsigned int stateMaxNum;
    unsigned int stateWordOffset;
    unsigned int stateWordsNum;
    RangedMeasurement_t stateArrayReactionTimeRange = {0.0, 1.0, 1.0, UnitPfxNone, "s"};

    /*! Protocol's parameters */
    unsigned int protocolMaxItemsNum = 0;
    unsigned int protocolWordOffset;
    unsigned int protocolItemsWordsNum;
    double protocolFpgaClockFrequencyHz = 10.0e6;

    bool voltageProtocolStepImplemented = false;
    bool voltageProtocolRampImplemented = false;
    bool voltageProtocolSinImplemented = false;

    bool currentProtocolStepImplemented = false;
    bool currentProtocolRampImplemented = false;
    bool currentProtocolSinImplemented = false;

    RangedMeasurement_t protocolTimeRange;
    RangedMeasurement_t positiveProtocolTimeRange;
    RangedMeasurement_t protocolFrequencyRange;
    RangedMeasurement_t positiveProtocolFrequencyRange;

    uint16_t selectedProtocolItemsNum = 0;
    uint16_t selectedProtocolId = 0;
    Measurement_t selectedProtocolVrest = {0.0, UnitPfxNone, "V"};
    Measurement_t selectedProtocolIrest = {0.0, UnitPfxNone, "A"};

    uint32_t clampingModalitiesNum = 0;
    uint32_t selectedClampingModalityIdx = 0;
    bool clampingModalitySetFlag = false;
    uint32_t selectedClampingModality = VOLTAGE_CLAMP;
    uint32_t previousClampingModality = VOLTAGE_CLAMP;
    std::vector <ClampingModality_t> clampingModalitiesArray;
    uint16_t defaultClampingModalityIdx = 0;

    uint32_t vcCurrentRangesNum = 0;
    uint32_t selectedVcCurrentRangeIdx = 0;
    uint32_t storedVcCurrentRangeIdx = 0;
    std::vector <RangedMeasurement_t> vcCurrentRangesArray;
    uint16_t defaultVcCurrentRangeIdx = 0;

    uint32_t vcVoltageRangesNum = 0;
    uint32_t selectedVcVoltageRangeIdx = 0;
    std::vector <RangedMeasurement_t> vcVoltageRangesArray;
    uint16_t defaultVcVoltageRangeIdx = 0;

    uint32_t liquidJunctionRangesNum = 0;
    uint32_t selectedLiquidJunctionRangeIdx = 0;
    std::vector <RangedMeasurement_t> liquidJunctionRangesArray;
    uint16_t defaultLiquidJunctionRangeIdx = 0;

    uint32_t ccCurrentRangesNum = 0;
    uint32_t selectedCcCurrentRangeIdx = 0;
    std::vector <RangedMeasurement_t> ccCurrentRangesArray;
    uint16_t defaultCcCurrentRangeIdx = 0;

    uint32_t ccVoltageRangesNum = 0;
    uint32_t selectedCcVoltageRangeIdx = 0;
    std::vector <RangedMeasurement_t> ccVoltageRangesArray;
    uint16_t defaultCcVoltageRangeIdx = 0;

    uint32_t vcCurrentFiltersNum = 0;
    uint32_t selectedVcCurrentFilterIdx = 0;
    std::vector <Measurement_t> vcCurrentFiltersArray;
    uint16_t defaultVcCurrentFilterIdx = 0;

    uint32_t vcVoltageFiltersNum = 0;
    uint32_t selectedVcVoltageFilterIdx = 0;
    std::vector <Measurement_t> vcVoltageFiltersArray;
    uint16_t defaultVcVoltageFilterIdx = 0;

    uint32_t ccCurrentFiltersNum = 0;
    uint32_t selectedCcCurrentFilterIdx = 0;
    std::vector <Measurement_t> ccCurrentFiltersArray;
    uint16_t defaultCcCurrentFilterIdx = 0;

    uint32_t ccVoltageFiltersNum = 0;
    uint32_t selectedCcVoltageFilterIdx = 0;
    std::vector <Measurement_t> ccVoltageFiltersArray;
    uint16_t defaultCcVoltageFilterIdx = 0;

    uint32_t samplingRatesNum;
    std::vector <Measurement_t> samplingRatesArray;
    std::vector <Measurement_t> realSamplingRatesArray;
    std::vector <Measurement_t> integrationStepArray;
    unsigned int defaultSamplingRateIdx = 0;
    std::unordered_map <uint16_t, uint16_t> sr2LpfVcCurrentMap;
    std::unordered_map <uint16_t, uint16_t> sr2LpfCcVoltageMap;
    std::unordered_map <uint16_t, uint16_t> vcCurrRange2CalibResMap;

    std::vector <Measurement_t> selectedVoltageHoldVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector <Measurement_t> selectedCurrentHoldVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector <Measurement_t> selectedVoltageHalfVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector <Measurement_t> selectedCurrentHalfVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */

    Measurement_t defaultVoltageHoldTuner = {0.0, UnitPfxNone, "V"};
    Measurement_t defaultCurrentHoldTuner = {0.0, UnitPfxNone, "A"};
    Measurement_t defaultVoltageHalfTuner = {0.0, UnitPfxNone, "V"};
    Measurement_t defaultCurrentHalfTuner = {0.0, UnitPfxNone, "A"};

    std::vector <Measurement_t> selectedLiquidJunctionVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector <int16_t> ccLiquidJunctionVector;

    RangedMeasurement_t gateVoltageRange;
    std::vector <Measurement_t> selectedGateVoltageVector;

    RangedMeasurement_t sourceVoltageRange;
    std::vector <Measurement_t> selectedSourceVoltageVector;
    uint16_t selectedSourceForVoltageChannelIdx;
    uint16_t selectedSourceForCurrentChannelIdx;

    std::vector <RangedMeasurement_t> rRShuntConductanceCalibRange;

    /*! Compensation options*/
    std::vector <uint16_t> selectedRsCorrBws;
    std::vector <Measurement_t> rsCorrBwArray;
    uint16_t defaultRsCorrBwIdx;

    /*! Features in ASIC domain, depend on asic*/
    std::vector <RangedMeasurement> pipetteCapacitanceRange;
    std::vector <RangedMeasurement> membraneCapValueRange;
    std::vector <RangedMeasurement> membraneCapTauValueRange;
    RangedMeasurement_t rsCorrValueRange;
    RangedMeasurement_t rsPredGainRange;
    RangedMeasurement_t rsPredTauRange;

    std::vector <CompensationControl_t> compensationControls[CompensationUserParamsNum];

    /*! Default paramter values in USER domain*/
    std::vector <double> defaultUserDomainParams;

    std::vector <double> membraneCapValueInjCapacitance;
    std::vector <std::vector <std::string>> compensationOptionStrings;

    std::vector <std::vector <double>> compValueMatrix;
    std::vector <bool> compensationsEnableFlags[CompensationTypesNum];
    bool vcCompensationsActivated = false;
    bool ccCompensationsActivated = false;

    bool anyLiquidJunctionActive = false;

    std::vector <LiquidJunctionStatus_t> liquidJunctionStatuses;
    std::vector <LiquidJunctionState_t> liquidJunctionStates;
    std::vector <int64_t> liquidJunctionCurrentSums;
    std::vector <double> liquidJunctionCurrentEstimates;
    int64_t liquidJunctionCurrentEstimatesNum;
    std::vector <Measurement_t> liquidJunctionVoltagesBackup;
    std::vector <double> liquidJunctionDeltaVoltages;
    std::vector <double> liquidJunctionDeltaCurrents;
    std::vector <double> liquidJunctionSmallestCurrentChange;
    std::vector <uint16_t> liquidJunctionConvergingCount;
    std::vector <uint16_t> liquidJunctionConvergedCount;
    std::vector <uint16_t> liquidJunctionPositiveSaturationCount;
    std::vector <uint16_t> liquidJunctionNegativeSaturationCount;
    std::vector <uint16_t> liquidJunctionOpenCircuitCount;

    std::string deviceId;
    std::string deviceName;

    bool threadsStarted = false;
    bool stopConnectionFlag = false;
    bool parsingFlag = false;

    std::vector <BoardModel *> boardModels;
    std::vector <ChannelModel *> channelModels;

    uint16_t selectedSamplingRateIdx = 0;

    double currentResolution = 1.0;
    double voltageResolution = 1.0;
    double liquidJunctionResolution = 1.0;
    bool liquidJunctionSameRangeAsVcDac = true;

    RangedMeasurement_t voltageRange;
    RangedMeasurement_t currentRange;
    RangedMeasurement_t liquidJunctionRange;

    Measurement_t samplingRate = {200.0, UnitPfxKilo, "Hz"};
    Measurement_t integrationStep = {5.0, UnitPfxMicro, "s"};
    Measurement_t stateArrayReactionTime = {0.0, UnitPfxMicro, "s"};

    std::vector <uint16_t> allChannelIndexes;

    uint16_t customFlagsNum = 0;
    std::vector <std::string> customFlagsNames;
    std::vector <bool> customFlagsDefault;

    uint16_t customOptionsNum = 0;
    std::vector <std::string> customOptionsNames;
    std::vector <std::vector <std::string>> customOptionsDescriptions;
    std::vector <uint16_t> customOptionsDefault;

    uint16_t customDoublesNum = 0;
    std::vector <std::string> customDoublesNames;
    std::vector <RangedMeasurement_t> customDoublesRanges;
    std::vector <double> customDoublesDefault;

    /***********************\
     *  Filters variables  *
    \***********************/

    Measurement_t rawDataFilterCutoffFrequency = {30.0, UnitPfxKilo, "Hz"};
    bool rawDataFilterLowPassFlag = true;
    bool rawDataFilterActiveFlag = false;
    bool rawDataFilterVoltageFlag = false;
    bool rawDataFilterCurrentFlag = false;

    uint32_t maxDownSamplingRatio = 1000;
    uint32_t selectedDownsamplingRatio = 1;
    bool downsamplingFlag = false;
    uint32_t downsamplingOffset = 0;
    Measurement_t rawDataFilterCutoffFrequencyOverride = {30.0, UnitPfxKilo, "Hz"};

    double iirVNum[IIR_ORD+1];
    double iirVDen[IIR_ORD+1];
    double iirINum[IIR_ORD+1];
    double iirIDen[IIR_ORD+1];

    double ** iirX = nullptr;
    double ** iirY = nullptr;

    uint16_t iirOff = 0;

    /********************************************\
     *  Multi-thread synchronization variables  *
    \********************************************/

    /*! \todo FCON non mi piace sta roba qui, dovrebbe stare inemcrdevice.h, però lo usa la procedura per la liquid junction */
    mutable std::mutex txMutex;
    std::condition_variable txMsgBufferNotEmpty;
    std::condition_variable txMsgBufferNotFull;

    mutable std::mutex ljMutex;
    bool liquidJunctionControlPending = false;

    std::thread liquidJunctionThread;

#if defined(DEBUG_TX_DATA_PRINT) || defined(DEBUG_RX_DATA_PRINT)
    std::chrono::steady_clock::time_point startPrintfTime;
    std::chrono::steady_clock::time_point currentPrintfTime;
#endif

#ifdef DEBUG_TX_DATA_PRINT
    FILE * txFid = nullptr;
#endif

#ifdef DEBUG_RX_RAW_DATA_PRINT
    FILE * rxRawFid = nullptr;
#endif

#ifdef DEBUG_RX_PROCESSING_PRINT
    FILE * rxProcFid = nullptr;
#endif

#ifdef DEBUG_RX_DATA_PRINT
    FILE * rxFid = nullptr;
#endif

#ifdef DEBUG_LIQUID_JUNCTION_PRINT
    FILE * ljFid = nullptr;
#endif
};

#endif // MESSAGEDISPATCHER_H
