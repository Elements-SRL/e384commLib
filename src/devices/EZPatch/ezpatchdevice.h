#ifndef EZPATCHDEVICE_H
#define EZPATCHDEVICE_H

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>

#include "messagedispatcher.h"
#include "ftdicalibrationeeprom.h"

#define EZP_RX_MSG_BUFFER_SIZE 0x10000 /*! \todo FCON valutare che questo numero sia adeguato */ // ~65k
#define EZP_RX_MSG_BUFFER_MASK (EZP_RX_MSG_BUFFER_SIZE-1)
#define EZP_RX_DATA_BUFFER_SIZE 0x1000000 /*! \todo FCON valutare che questo numero sia adeguato */ // 16M
#define EZP_RX_DATA_BUFFER_MASK (EZP_RX_DATA_BUFFER_SIZE-1)
#define EZP_RX_MIN_DATA_PACKET_LEN 3
#define EZP_RX_MIN_DATA_PACKET_VALID_LEN (EZP_RX_MIN_DATA_PACKET_LEN-1)

#define RX_MSG_BUFFER_SIZE 0x10000 // ~64k
#define RX_MSG_BUFFER_MASK (RX_MSG_BUFFER_SIZE-1)
#define RX_DATA_BUFFER_SIZE 0x10000000 /*! ~256M The biggest data frame possible has a dataload of 1024 words (4 x 10MHz current frame)
                                           So this buffer has to be at least 1024 times bigger than RX_MSG_BUFFER_SIZE */
#define RX_DATA_BUFFER_MASK (RX_DATA_BUFFER_SIZE-1)

#define EZP_TX_MSG_BUFFER_SIZE 0x1000 /*! \todo FCON valutare che questo numero sia adeguato */ // 4096
#define EZP_TX_MSG_BUFFER_MASK (EZP_TX_MSG_BUFFER_SIZE-1)
#define EZP_TX_DATA_BUFFER_SIZE 0x100000 /*! \todo FCON valutare che questo numero sia adeguato */ // 1M
#define EZP_TX_DATA_BUFFER_MASK (EZP_TX_DATA_BUFFER_SIZE-1)

#define EZP_MAX_RESEND_TRIES 5
#define EZP_MAX_PING_TRIES 5
#define EZP_MAX_FPGA_RESET_TRIES 5
#define EZP_MAX_WRITE_TRIES 3
#define EZP_ACK_WAIT_TIME_MS 200
#define EZP_NO_DATA_WAIT_TIME_MS 200

typedef enum {
    ResetIndexChip,
    ResetIndexLiquidJunctionCompensation,
    ResetIndexNum
} ResetIndex_t;

class EZPatchDevice : public MessageDispatcher {
public:

    /*****************\
     *  Ctor / Dtor  *
    \*****************/

    EZPatchDevice(std::string deviceId);
    virtual ~EZPatchDevice();

    /************************\
     *  Connection methods  *
    \************************/

    virtual ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag) override;

    /****************\
     *  Tx methods  *
    \****************/

    ErrorCodes_t ack(uint16_t heartbeat);
    ErrorCodes_t nack(uint16_t heartbeat);
    ErrorCodes_t ping();

    ErrorCodes_t resetAsic(bool resetFlag, bool applyFlag) override;
    virtual ErrorCodes_t resetFpga(bool resetFlag, bool applyFlag) override;
    virtual ErrorCodes_t resetFpga();

    ErrorCodes_t setLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) override;
    ErrorCodes_t setLiquidJunctionVoltage(uint16_t channelIdx, Measurement_t voltage);
    ErrorCodes_t updateLiquidJunctionVoltage(uint16_t channelIdx, bool applyFlag);

    ErrorCodes_t setVoltageHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) override;
    ErrorCodes_t setCurrentHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool applyFlag) override;

    ErrorCodes_t turnChannelsOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnCalSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;

    ErrorCodes_t turnVcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnCcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t setAdcCore(std::vector <uint16_t> channelIndexes, std::vector <ClampingModality_t> clampingModes, bool applyFlag) override;

    ErrorCodes_t turnVoltageStimulusOn(bool on, bool applyFlag) override;
    ErrorCodes_t turnCurrentStimulusOn(bool on, bool applyFlag) override;
    ErrorCodes_t turnVoltageReaderOn(bool on, bool applyFlag) override;
    ErrorCodes_t turnCurrentReaderOn(bool on, bool applyFlag) override;

    ErrorCodes_t setClampingModality(uint32_t idx, bool applyFlag, bool stopProtocolFlag) override;
    ErrorCodes_t setSourceForVoltageChannel(uint16_t source, bool applyFlag) override;
    ErrorCodes_t setSourceForCurrentChannel(uint16_t source, bool applyFlag) override;

    ErrorCodes_t setCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) override;

    virtual ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) override;
    virtual ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) override;
    ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) override;
    ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) override;
    virtual ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) override;

    ErrorCodes_t readoutOffsetRecalibration(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    virtual ErrorCodes_t liquidJunctionCompensation(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    virtual ErrorCodes_t liquidJunctionCompensation(uint16_t channelIdx);
    ErrorCodes_t liquidJunctionCompensationOverride(uint16_t channelIdx, Measurement_t value);
    ErrorCodes_t liquidJunctionCompensationInquiry(uint16_t channelIdx);
    ErrorCodes_t zap(Measurement_t duration, uint16_t channelIdx);
    ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlag) override;
    ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlag) override;
    ErrorCodes_t enableStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t enableStimulus(uint16_t channelIdx, bool on);
    ErrorCodes_t turnLedOn(uint16_t ledIndex, bool on);
    ErrorCodes_t setAnalogOut(bool on);
    ErrorCodes_t setSlave(bool on);
    ErrorCodes_t setConstantSwitches();
    ErrorCodes_t setConstantRegisters();

    ErrorCodes_t setCompensationsChannel(uint16_t channelIdx);
    ErrorCodes_t enableCompensation(std::vector <uint16_t> channelIndexes, CompensationTypes_t compTypeToEnable, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t enableVcCompensations(bool enable, bool applyFlag) override;
    ErrorCodes_t enableCcCompensations(bool enable, bool applyFlag) override;
    ErrorCodes_t turnPipetteCompensationOn(bool on);
    ErrorCodes_t turnCCPipetteCompensationOn(bool on);
    ErrorCodes_t turnMembraneCompensationOn(bool on);
    ErrorCodes_t turnResistanceCompensationOn(bool on);
    ErrorCodes_t turnResistanceCorrectionOn(bool on);
    ErrorCodes_t turnResistancePredictionOn(bool on);
    ErrorCodes_t turnLeakConductanceCompensationOn(bool on);
    ErrorCodes_t turnBridgeBalanceCompensationOn(bool on);

    ErrorCodes_t setCompOptions(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <uint16_t> options, bool applyFlag) override;
    ErrorCodes_t setPipetteCompensationOptions(uint16_t optionIdx);
    ErrorCodes_t setCCPipetteCompensationOptions(uint16_t optionIdx);
    ErrorCodes_t setMembraneCompensationOptions(uint16_t optionIdx);
    ErrorCodes_t setResistanceCompensationOptions(uint16_t optionIdx);
    ErrorCodes_t setResistanceCorrectionOptions(uint16_t optionIdx);
    virtual ErrorCodes_t setResistancePredictionOptions(uint16_t optionIdx);
    ErrorCodes_t setLeakConductanceCompensationOptions(uint16_t optionIdx);
    ErrorCodes_t setBridgeBalanceCompensationOptions(uint16_t optionIdx);

    ErrorCodes_t setCompValues(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <double> newParamValues, bool applyFlag) override;
    ErrorCodes_t setPipetteCapacitance(Measurement_t capacitance);
    ErrorCodes_t setCCPipetteCapacitance(Measurement_t capacitance);
    ErrorCodes_t setMembraneCapacitance(Measurement_t capacitance);
    ErrorCodes_t setAccessResistance(Measurement_t resistance);
    ErrorCodes_t setResistanceCorrectionPercentage(Measurement_t percentage);
    ErrorCodes_t setResistanceCorrectionLag(Measurement_t lag);
    ErrorCodes_t setResistancePredictionGain(Measurement_t gain);
    ErrorCodes_t setResistancePredictionPercentage(Measurement_t percentage);
    ErrorCodes_t setResistancePredictionTau(Measurement_t tau);
    ErrorCodes_t setLeakConductance(Measurement_t conductance);
    ErrorCodes_t setBridgeBalanceResistance(Measurement_t resistance); /*! \todo FCON sono arrivato qui */

    ErrorCodes_t setDigitalTriggerOutput(uint16_t triggerIdx, bool terminator, bool polarity, uint16_t triggerId, Measurement_t delay);
    ErrorCodes_t setDigitalRepetitiveTriggerOutput(uint16_t triggersNum, bool valid, bool infinite, uint16_t triggerId,
                                                   Measurement_t delay, Measurement_t duration, Measurement_t period, Measurement_t deltaPeriod);

    ErrorCodes_t setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest, bool stopProtocolFlag) override;
    ErrorCodes_t voltStepTimeStep(Measurement_t v0, Measurement_t vStep, Measurement_t t0, Measurement_t tStep,
                                  uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps);
    ErrorCodes_t voltRamp(Measurement_t v0, Measurement_t vFinal, Measurement_t t,
                          uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps);
    ErrorCodes_t voltSin(Measurement_t v0, Measurement_t vAmp, Measurement_t freq,
                         uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps);
    ErrorCodes_t setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) override;
    ErrorCodes_t setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) override;
    ErrorCodes_t setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag) override;
    ErrorCodes_t startProtocol() override;
    ErrorCodes_t stopProtocol() override;
    ErrorCodes_t zeroProtocol();

    ErrorCodes_t setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest, bool stopProtocolFlag) override;
    ErrorCodes_t currStepTimeStep(Measurement_t i0, Measurement_t iStep, Measurement_t t0, Measurement_t tStep,
                                  uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps);
    ErrorCodes_t currRamp(Measurement_t i0, Measurement_t iFinal, Measurement_t t,
                          uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps);
    ErrorCodes_t currSin(Measurement_t i0, Measurement_t iAmp, Measurement_t freq,
                         uint16_t currentItem, uint16_t nextItem, uint16_t repsNum, uint16_t applySteps);
    ErrorCodes_t setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) override;
    ErrorCodes_t setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool iHalfFlag) override;
    ErrorCodes_t setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool iHalfFlag) override;

    ErrorCodes_t resetLiquidJunctionCompensation(bool reset);

    ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status, bool applyFlag = true) override;
    ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue) override;

    ErrorCodes_t getSwitchesStatus(std::vector <uint16_t> &words, std::vector <std::vector <std::string> > &names);
    ErrorCodes_t singleSwitchDebug(uint16_t word, uint16_t bit, bool flag);
    ErrorCodes_t multiSwitchDebug(std::vector <uint16_t> words);
    ErrorCodes_t singleRegisterDebug(uint16_t index, uint16_t value);

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t isDeviceUpgradable(std::string &upgradeNotes, std::string &notificationTag);
    virtual ErrorCodes_t getNextMessage(RxOutput_t &rxOutput, int16_t * data) override;
    ErrorCodes_t purgeData() override;

    ErrorCodes_t hasChannelSwitches() override;
    ErrorCodes_t hasCalSw() override;

    ErrorCodes_t hasVoltageHoldTuner();
    ErrorCodes_t getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageRanges) override;
    ErrorCodes_t hasCurrentHoldTuner();
    ErrorCodes_t getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentRanges) override;
    ErrorCodes_t getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges) override;

    ErrorCodes_t getMaxOutputTriggers(unsigned int &maxTriggersNum);
    ErrorCodes_t getOutputTriggersNum(unsigned int &triggersNum);
    ErrorCodes_t hasRepetitiveTrigger();

    ErrorCodes_t multimeterStuckHazard(bool &stuckFlag);

    ErrorCodes_t getLedsNumber(uint16_t &ledsNumber);
    ErrorCodes_t getLedsColors(std::vector <uint32_t> &ledsColors);
    ErrorCodes_t hasAnalogOut();
    ErrorCodes_t hasSlaveModality();
    ErrorCodes_t hasOffsetCompensation() override;

    virtual ErrorCodes_t hasPipetteCompensation();
    virtual ErrorCodes_t hasCCPipetteCompensation();
    virtual ErrorCodes_t hasMembraneCompensation();
    virtual ErrorCodes_t hasResistanceCompensation();
    virtual ErrorCodes_t hasResistanceCorrection();
    virtual ErrorCodes_t hasResistancePrediction();
    virtual ErrorCodes_t hasLeakConductanceCompensation();
    virtual ErrorCodes_t hasBridgeBalanceCompensation();

    virtual ErrorCodes_t getCompOptionsFeatures(CompensationTypes_t type, std::vector <std::string> &options) override;

    ErrorCodes_t getLiquidJunctionControl(CompensationControl_t &control);
    virtual ErrorCodes_t getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) override;

protected:

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

    virtual ErrorCodes_t initialize(std::string fwPath) override;
    virtual void deinitialize() override;
    virtual ErrorCodes_t initializeMemory() override;
    virtual void initializeVariables() override;
    virtual ErrorCodes_t deviceConfiguration() override;
    virtual void createCommunicationThreads() override;
    virtual ErrorCodes_t initializeHW() override;

    virtual void deinitializeMemory() override;

    void joinCommunicationThreads() override;
    void computeLiquidJunction();

    virtual void initializeCompensations();
    virtual void initializeLiquidJunction() override;

    virtual void readAndParseMessages() = 0;
    virtual void unwrapAndSendMessages() = 0;

    ErrorCodes_t manageOutgoingMessageLife(uint16_t msgTypeId, std::vector <uint16_t> txDataMessage, uint16_t dataLen);
    virtual void wrapOutgoingMessage(uint16_t msgTypeId, std::vector <uint16_t> &txDataMessage, uint16_t dataLen) = 0;

    inline void int322uint16(int32_t from, std::vector <uint16_t> &to, size_t offset);
    inline void uint322uint16(uint32_t from, std::vector <uint16_t> &to, size_t offset);

    virtual void selectChannelsResolutions() = 0;
    virtual void selectVoltageOffsetResolution() = 0;

    ErrorCodes_t setSecondaryDeviceswitch (bool flag);

    ErrorCodes_t setLiquidJunctionCompensationOverrideSwitch (bool flag);
    ErrorCodes_t setLiquidJunctionCompensationOverrideValue(uint16_t channelIdx, Measurement_t value);

    bool checkVoltStepTimeStepParameters(double v0, double vStep, double t0, double tStep, uint16_t repsNum, uint16_t applySteps);
    bool checkVoltRampParameters(double v0, double vFinal, double t);
    bool checkVoltSinParameters(double v0, double vAmp, double freq);

    bool checkCurrStepTimeStepParameters(double i0, double iStep, double t0, double tStep, uint16_t repsNum, uint16_t applySteps);
    bool checkCurrRampParameters(double i0, double iFinal, double t);
    bool checkCurrSinParameters(double i0, double iAmp, double freq);

    void switches2DataMessage(std::vector <uint16_t> &txDataMessage);
    void dataMessage2Switches(std::vector <uint16_t> &txDataMessage);

    void updateCompensations();
    virtual bool checkCompensationsValues() = 0;
    virtual bool fillCompensationsRegistersTxData(std::vector <uint16_t> &txDataMessage) = 0;
    virtual void updateWrittenCompensationValues(std::vector <uint16_t> &txDataMessage) = 0;
    ErrorCodes_t turnCompensationsOn(bool &flag, bool on);
    ErrorCodes_t turnCompensationOn(std::vector <bool> &flag, bool on);
    virtual ErrorCodes_t setCompensationsOptions();
    ErrorCodes_t setCompensationValue(CompensationControl_t &control, Measurement_t newValue);
    virtual void compensationsFlags2Switches(std::vector <uint16_t> &txDataMessage);

    /****************\
     *  Parameters  *
    \****************/

    std::unordered_map <uint16_t, bool> rxExpectAckMap; /*!< key is any message type ID, value tells if the message requires ACK from EDR */

    std::unordered_map <uint16_t, bool> rxEnabledTypesMap; /*! key is any message type ID, value tells if the message should be returned by the getNextMessage method */

    std::unordered_map <uint16_t, bool> txExpectAckMap; /*!< key is any message type ID, value tells if the message requires ACK from the device */

    uint16_t selectableCurrentChannelsNum = 1;
    uint16_t selectableVoltageChannelsNum = 1;
    uint16_t selectableTotalChannelsNum = 2;

    std::vector <uint16_t> availableVoltageSourcesIdxsArray;
    std::vector <uint16_t> availableCurrentSourcesIdxsArray;
    std::vector <uint16_t> voltageChannelSourcesRegisters;
    std::vector <uint16_t> currentChannelSourcesRegisters;

    bool voltageHoldTunerImplemented = false;
    bool currentHoldTunerImplemented = false;

    bool vcCurrentOffsetDeltaImplemented = false;
    bool ccVoltageOffsetDeltaImplemented = false;

    double voltageCommandResolution = 1.0;
    double vcVoltageCommandResolutionGain = 1.0;
    double currentCommandResolution = 1.0;
    double ccCurrentCommandResolutionGain = 1.0;

    unsigned int maxDigitalTriggerOutputEvents = 0;
    unsigned int digitalTriggersNum = 0;
    bool repetitiveTriggerAvailableFlag = false;

    CompensationControl_t liquidJunctionControl;
    double liquidJunctionOffsetBinary = 0.0;
    double liquidJunctionRounding = 1.0; /*! In some case the 4 LSBs are fixed at 0. Since the automatic compensation does not set these bits in EL04e/f device,
                                             this bits must not be set, otherwise the FW fails to check against the upper and lower limits */

    uint16_t zapDurationHwRegisterOffset = 0;
    uint16_t vcHoldTunerHwRegisterOffset = 0;
    uint16_t ccHoldTunerHwRegisterOffset = 0;
    uint16_t liquidJunctionCompensationOverrideRegisterOffset = 0;
    uint16_t vcCurrentOffsetDeltaRegisterOffset = 0;
    uint16_t ccVoltageOffsetDeltaRegisterOffset = 0;

    uint16_t switchesStatusLength = 0;
    std::vector <uint16_t> switchesStatus;
    std::vector <std::vector <std::string> > switchesNames;

    uint16_t ledsNum = 0;
    std::vector <uint16_t> ledsWord;
    std::vector <uint16_t> ledsByte;
    std::vector <uint32_t> ledsColorsArray;

    bool analogOutImplementedFlag = false;
    uint16_t analogOutWord;
    uint16_t analogOutByte;

    bool slaveImplementedFlag = false;
    uint16_t slaveWord;
    uint16_t slaveByte;

    bool multimeterStuckHazardFlag = false;

    uint16_t inputSwitchesNum = 0;
    std::vector <uint16_t> inputSwitchesWord;
    std::vector <uint16_t> inputSwitchesByte;
    std::vector <bool> inputSwitchesLut;

    uint16_t calSwitchesNum = 0;
    std::vector <uint16_t> calSwitchesWord;
    std::vector <uint16_t> calSwitchesByte;
    std::vector <bool> calSwitchesLut;

    uint16_t vcSwitchesNum = 0;
    std::vector <uint16_t> vcSwitchesWord;
    std::vector <uint16_t> vcSwitchesByte;
    std::vector <bool> vcSwitchesLut;

    uint16_t ccSwitchesNum = 0;
    std::vector <uint16_t> ccSwitchesWord;
    std::vector <uint16_t> ccSwitchesByte;
    std::vector <bool> ccSwitchesLut;

    uint16_t adcCoreSwitchesNum = 0;
    std::vector <uint16_t> adcCoreSwitchesWord;
    std::vector <uint16_t> adcCoreSwitchesByte;
    std::vector <bool> adcCoreSwitchesLut;
    bool adcCoreSwitchChannelIndependent = false;

    uint16_t vcStimulusSwitchesNum = 0;
    std::vector <uint16_t> vcStimulusSwitchesWord;
    std::vector <uint16_t> vcStimulusSwitchesByte;
    std::vector <bool> vcStimulusSwitchesLut;
    bool vcStimulusSwitchChannelIndependent = true;

    uint16_t vcReaderSwitchesNum = 0;
    std::vector <uint16_t> vcReaderSwitchesWord;
    std::vector <uint16_t> vcReaderSwitchesByte;
    std::vector <bool> vcReaderSwitchesLut;
    bool vcReaderSwitchChannelIndependent = true;

    uint16_t ccStimulusSwitchesNum = 0;
    std::vector <uint16_t> ccStimulusSwitchesWord;
    std::vector <uint16_t> ccStimulusSwitchesByte;
    std::vector <bool> ccStimulusSwitchesLut;
    bool ccStimulusSwitchChannelIndependent = true;

    uint16_t ccReaderSwitchesNum = 0;
    std::vector <uint16_t> ccReaderSwitchesWord;
    std::vector <uint16_t> ccReaderSwitchesByte;
    std::vector <bool> ccReaderSwitchesLut;
    bool ccReaderSwitchChannelIndependent = true;

    uint16_t vcStimulusLpfSwitchesNum = 0;
    std::vector <uint16_t> vcStimulusLpfSwitchesWord;
    std::vector <uint16_t> vcStimulusLpfSwitchesByte;
    std::vector <std::vector <bool> > vcStimulusLpfSwitchesLut;
    uint16_t vcStimulusLpfOptionsNum = 0;

    uint16_t ccStimulusLpfSwitchesNum = 0;
    std::vector <uint16_t> ccStimulusLpfSwitchesWord;
    std::vector <uint16_t> ccStimulusLpfSwitchesByte;
    std::vector <std::vector <bool> > ccStimulusLpfSwitchesLut;
    uint16_t ccStimulusLpfOptionsNum = 0;

    bool stimulusEnableImplemented = false;
    uint16_t stimulusEnableSwitchWord;
    uint16_t stimulusEnableSwitchByte;

    uint16_t constantSwitchesNum = 0;
    std::vector <uint16_t> constantSwitchesWord;
    std::vector <uint16_t> constantSwitchesByte;
    std::vector <bool> constantSwitchesLut;

    uint16_t constantRegistersNum = 0;
    std::vector <uint16_t> constantRegistersWord;
    std::vector <uint16_t> constantRegistersValues;

    uint16_t vcCurrentRangesSwitchesNum = 0;
    std::vector <uint16_t> vcCurrentRangesSwitchesWord;
    std::vector <uint16_t> vcCurrentRangesSwitchesByte;
    std::vector <std::vector <bool> > vcCurrentRangesSwitchesLut;

    uint16_t ccCurrentRangesSwitchesNum = 0;
    std::vector <uint16_t> ccCurrentRangesSwitchesWord;
    std::vector <uint16_t> ccCurrentRangesSwitchesByte;
    std::vector <std::vector <bool> > ccCurrentRangesSwitchesLut;

    uint16_t vcVoltageRangesSwitchesNum = 0;
    std::vector <uint16_t> vcVoltageRangesSwitchesWord;
    std::vector <uint16_t> vcVoltageRangesSwitchesByte;
    std::vector <std::vector <bool> > vcVoltageRangesSwitchesLut;

    uint16_t ccVoltageRangesSwitchesNum = 0;
    std::vector <uint16_t> ccVoltageRangesSwitchesWord;
    std::vector <uint16_t> ccVoltageRangesSwitchesByte;
    std::vector <std::vector <bool> > ccVoltageRangesSwitchesLut;

    bool liquidJunctionCompensationOverrideImplemented = false;
    uint16_t liquidJunctionCompensationOverrideSwitchWord;
    uint16_t liquidJunctionCompensationOverrideSwitchByte;

    bool startProtocolCommandImplemented = false;
    uint16_t startProtocolSwitchWord;
    uint16_t startProtocolSwitchByte;

    uint16_t compensationsSettingChannel = 0;
    uint16_t coreSpecificSwitchesWordsNum = 0;
    uint16_t compensationsRegistersNum = 0;
    uint16_t coreSpecificRegistersNum = 0;

    uint16_t resetWord[ResetIndexNum];
    uint16_t resetByte[ResetIndexNum];
    int waitTimeBeforeAsicResetMs = 50;
    int resetDurationMs = 10;

    /***************\
     *  Variables  *
    \***************/

    uint32_t rxMsgOffset;

    uint16_t rxMsgBytes;

    uint16_t txHeartbeat = 0x0000;

    bool txAckReceived = false;

    int32_t txWaitingOnAcks = 0;

    uint8_t * rxRawBuffer = nullptr; /*!< Raw incoming data from the device */

    MsgResume_t * rxMsgBuffer = nullptr; /*!< Buffer of pre-digested messages that contains message's high level info */
    uint32_t rxMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t rxMsgBufferReadLength = 0; /*!< Lenght of the part of the buffer to be processed */
    uint32_t lastParsedMsgType = MsgTypeIdInvalid; /*!< Type of the last parsed message to check for repetitions  */
    bool gettingNextDataFlag = false;

    uint16_t * rxDataBuffer = nullptr; /*!< Buffer of pre-digested messages that contains message's data */

    uint32_t rxDataMessageMaxLen = 1; /*!< Max payload length */

    uint8_t * txRawBuffer = nullptr; /*!< Raw outgoing data to the device */

    MsgResume_t * txMsgBuffer = nullptr; /*!< Buffer of pre-digested messages that contains message's high level info */
    uint32_t txMsgBufferWriteOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t txMsgBufferReadLength = 0; /*!< Lenght of the part of the buffer to be processed */

    uint16_t * txDataBuffer = nullptr; /*!< Buffer of pre-digested messages that contains message's data */
    uint32_t txDataBufferWriteOffset = 0; /*!< Offset of the part of buffer to be written */

    uint16_t txDataMessageMaxLen = 1; /*!< Max payload length */

    double voltageOffsetCorrected = 0.0; /*!< Value currently corrected in applied voltages by the device (expressed in the unit of the liquid junction control) */
    double voltageOffsetCorrection = 0.0; /*!< Value to be used to correct the measured voltage values (expressed in the unit of current voltage range) */

    std::vector <double> voltageTunerCorrection; /*!< Value to be used to correct the measured voltage values (expressed in the unit of current voltage range) */
    std::vector <double> currentTunerCorrection; /*!< Value to be used to correct the measured current values (expressed in the unit of current current range) */

    double stepsOnLastSweep;
    uint16_t protocolItemsNum;
    uint16_t protocolItemIndex;

    /*******************\
     *  Compensations  *
    \*******************/

    uint16_t compensationsSwitchesNum = 0;
    std::vector <uint16_t> compensationsSwitchesWord;
    std::vector <uint16_t> compensationsSwitchesByte;
    std::vector <uint16_t> compensationsSwitchesLut;
    std::vector <char> compensationsSwitchesLutStrings;
    std::vector <bool> compensationsSwitchesEnableSignArray; /*! true means that enabling the compensation closes a switch (write a 1) */

    std::vector <bool> compensationsEnabledArray[CompensationTypesNum]; /*! Compensations actually enabled on device (the compensation is active and we are in the correct clamping modality) */

    bool pipetteCompensationImplemented = false;
    bool ccPipetteCompensationImplemented = false;
    bool membraneCompensationImplemented = false;
    bool resistanceCompensationImplemented = false;
    bool resistanceCorrectionImplemented = false;
    bool resistancePredictionImplemented = false;
    bool leakConductanceCompensationImplemented = false;
    bool bridgeBalanceCompensationImplemented = false;

    std::vector <std::string> pipetteCompensationOptions;
    std::vector <std::string> ccPipetteCompensationOptions;
    std::vector <std::string> membraneCompensationOptions;
    std::vector <std::string> resistanceCompensationOptions;
    std::vector <std::string> resistanceCorrectionOptions;
    std::vector <std::string> resistancePredictionOptions;
    std::vector <std::string> leakConductanceCompensationOptions;
    std::vector <std::string> bridgeBalanceCompensationOptions;

    std::vector <uint16_t> pipetteCompensationOptionReg;
    std::vector <uint16_t> ccPipetteCompensationOptionReg;
    std::vector <uint16_t> membraneCompensationOptionReg;
    std::vector <uint16_t> resistanceCompensationOptionReg;
    std::vector <uint16_t> resistanceCorrectionOptionReg;
    std::vector <uint16_t> resistancePredictionOptionReg;
    std::vector <uint16_t> leakConductanceCompensationOptionReg;
    std::vector <uint16_t> bridgeBalanceCompensationOptionReg;

    uint16_t pipetteCompensationOptionWord;
    uint16_t ccPipetteCompensationOptionWord;
    uint16_t membraneCompensationOptionWord;
    uint16_t resistanceCompensationOptionWord;
    uint16_t resistanceCorrectionOptionWord;
    uint16_t resistancePredictionOptionWord;
    uint16_t leakConductanceCompensationOptionWord;
    uint16_t bridgeBalanceCompensationOptionWord;

    uint16_t pipetteCompensationOptionMask;
    uint16_t ccPipetteCompensationOptionMask;
    uint16_t membraneCompensationOptionMask;
    uint16_t resistanceCompensationOptionMask;
    uint16_t resistanceCorrectionOptionMask;
    uint16_t resistancePredictionOptionMask;
    uint16_t leakConductanceCompensationOptionMask;
    uint16_t bridgeBalanceCompensationOptionMask;

    uint16_t pipetteCompensationOptionOffset;
    uint16_t ccPipetteCompensationOptionOffset;
    uint16_t membraneCompensationOptionOffset;
    uint16_t resistanceCompensationOptionOffset;
    uint16_t resistanceCorrectionOptionOffset;
    uint16_t resistancePredictionOptionOffset;
    uint16_t leakConductanceCompensationOptionOffset;
    uint16_t bridgeBalanceCompensationOptionOffset;

    std::vector <LiquidJunctionState_t> liquidJunctionStates;
    std::vector <int64_t> liquidJunctionCurrentSums;
    std::vector <double> liquidJunctionCurrentEstimates;
    std::vector <double> liquidJunctionResistanceEstimates;
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

    /********************************************\
     *  Multi-thread synchronization variables  *
    \********************************************/

    std::thread rxThread;
    std::thread txThread;

    mutable std::mutex rxMutex;
    std::condition_variable rxMsgBufferNotEmpty;
    std::condition_variable rxMsgBufferNotFull;

    mutable std::mutex txMutex;
    std::condition_variable txMsgBufferNotEmpty;
    std::condition_variable txMsgBufferNotFull;

    mutable std::mutex txAckMutex;
    std::condition_variable txAckCv;
};

#endif // EZPATCHDEVICE_H
