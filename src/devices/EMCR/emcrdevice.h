#ifndef EMCRDEVICE_H
#define EMCRDEVICE_H

#include "messagedispatcher.h"
#include "commandcoder.h"

class EmcrDevice : public MessageDispatcher {
public:
    EmcrDevice(std::string deviceId);
    virtual ~EmcrDevice();

    ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag) override;

    /****************\
     *  Tx methods  *
    \****************/

    ErrorCodes_t sendCommands() override;
    ErrorCodes_t startProtocol() override;
    ErrorCodes_t stopProtocol() override;
    ErrorCodes_t startStateArray() override;
    ErrorCodes_t zap(std::vector <uint16_t> channelIndexes, Measurement_t duration) override;

    ErrorCodes_t resetAsic(bool resetFlag, bool applyFlag = true) override;
    ErrorCodes_t resetFpga(bool resetFlag, bool applyFlag = true) override;

    ErrorCodes_t setVoltageHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) override;
    ErrorCodes_t setCurrentHoldTuner(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool applyFlag) override;
    ErrorCodes_t setVoltageHalf(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) override;
    ErrorCodes_t setCurrentHalf(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool applyFlag) override;
    ErrorCodes_t setLiquidJunctionVoltage(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> voltages, bool applyFlag) override;
    ErrorCodes_t updateLiquidJunctionVoltage(uint16_t channelIdx, bool applyFlag);
    ErrorCodes_t setGateVoltages(std::vector <uint16_t> boardIndexes, std::vector <Measurement_t> gateVoltages, bool applyFlag) override;
    ErrorCodes_t setSourceVoltages(std::vector <uint16_t> boardIndexes, std::vector <Measurement_t> sourceVoltages, bool applyFlag) override;

    ErrorCodes_t setCalibParams(CalibrationParams_t calibParams) override;
    ErrorCodes_t setCalibVcCurrentGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibVcCurrentGain(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibVcCurrentOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcVoltageGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibCcVoltageGain(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibCcVoltageOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibVcVoltageGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibVcVoltageGain(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibVcVoltageOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibVcVoltageOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcCurrentGain(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibCcCurrentGain(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcCurrentOffset(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibCcCurrentOffset(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibRsCorrOffsetDac(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibRsCorrOffsetDac(std::vector <uint16_t> channelIndexes, bool applyFlag)  override;
    ErrorCodes_t setCalibRShuntConductance(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> conductances, bool applyFlag) override;
    ErrorCodes_t updateCalibRShuntConductance(std::vector <uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t resetCalibRShuntConductance(std::vector <uint16_t> channelIndexes, bool applyFlag);

    ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) override;
    ErrorCodes_t setVCCurrentRange(std::vector <uint16_t> channelIndexes, std::vector <uint16_t> currentRangeIdx, bool applyFlag) override;
    ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) override;
    ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) override;
    ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag) override;
    ErrorCodes_t setCCVoltageRange(std::vector <uint16_t> channelIndexes, std::vector <uint16_t> voltageRangeIdx, bool applyFlag) override;
    ErrorCodes_t setLiquidJunctionRange(uint16_t idx) override;

    ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlag) override;
    ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlag) override;

    ErrorCodes_t enableStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnChannelsOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnCalSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;

    ErrorCodes_t turnVcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnCcSwOn(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t setAdcCore(std::vector <uint16_t> channelIndexes, std::vector <ClampingModality_t> clampingModes, bool applyFlag) override;
    ErrorCodes_t enableCcStimulus(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;

    ErrorCodes_t setClampingModality(uint32_t idx, bool applyFlag, bool stopProtocolFlag) override;
    ErrorCodes_t setSourceForVoltageChannel(uint16_t source, bool applyFlag) override;
    ErrorCodes_t setSourceForCurrentChannel(uint16_t source, bool applyFlag) override;

    ErrorCodes_t readoutOffsetRecalibration(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;
    ErrorCodes_t liquidJunctionCompensation(std::vector <uint16_t> channelIndexes, std::vector <bool> onValues, bool applyFlag) override;

    ErrorCodes_t setAdcFilter(bool applyFlag = false) override;
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) override;

    ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status) override;
    ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue) override;

    ErrorCodes_t setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest, bool stopProtocolFlag) override;
    ErrorCodes_t setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) override;
    ErrorCodes_t setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) override;
    ErrorCodes_t setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag) override;

    ErrorCodes_t setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest, bool stopProtocolFlag) override;
    ErrorCodes_t setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag) override;
    ErrorCodes_t setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag) override;
    ErrorCodes_t setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag) override;

    virtual ErrorCodes_t setCompRanges(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <uint16_t> newRanges, bool applyFlag) override;

    ErrorCodes_t setStateArrayStructure(int numberOfStates, int initialState, Measurement_t reactionTime) override;
    ErrorCodes_t setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, Measurement_t timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag) override;
    ErrorCodes_t setStateArrayEnabled(int chIdx, bool enabledFlag) override;

    /*! Device specific controls */

    virtual ErrorCodes_t setCustomFlag(uint16_t idx, bool flag, bool applyFlag) override;
    virtual ErrorCodes_t setCustomOption(uint16_t idx, uint16_t value, bool applyFlag) override;
    virtual ErrorCodes_t setCustomDouble(uint16_t idx, double value, bool applyFlag) override;

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t getNextMessage(RxOutput_t &rxOutput, int16_t * data) override;
    ErrorCodes_t purgeData() override;

    ErrorCodes_t getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures) override;
    ErrorCodes_t getVoltageHoldTuner(std::vector <Measurement_t> &voltages) override;
    ErrorCodes_t getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTunerFeatures) override;
    ErrorCodes_t getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures) override;
    ErrorCodes_t getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTunerFeatures) override;
    ErrorCodes_t getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges) override;
    ErrorCodes_t hasGateVoltages() override;
    ErrorCodes_t hasSourceVoltages() override;
    ErrorCodes_t getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures) override;
    ErrorCodes_t getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures) override;

    ErrorCodes_t hasChannelSwitches() override;
    ErrorCodes_t hasCalSw() override;
    ErrorCodes_t hasStimulusSwitches() override;
    ErrorCodes_t hasOffsetCompensation() override;
    ErrorCodes_t hasStimulusHalf() override;
    ErrorCodes_t isStateArrayAvailable() override;
    ErrorCodes_t getZapFeatures(RangedMeasurement_t &durationRange) override;

    ErrorCodes_t getCalibParams(CalibrationParams_t &calibParams) override;
    ErrorCodes_t getCalibFileNames(std::vector <std::string> &calibFileNames) override;
    ErrorCodes_t getCalibFilesFlags(std::vector <std::vector <bool> > &calibFilesFlags) override;
    ErrorCodes_t getCalibMappingFileDir(std::string &dir) override;
    ErrorCodes_t getCalibMappingFilePath(std::string &path) override;

protected:

    /*************\
     *  Methods  *
    \*************/

    virtual ErrorCodes_t initialize(std::string fwPath) override;
    virtual void deinitialize() override;
    virtual ErrorCodes_t initializeMemory() override;
    virtual void initializeVariables() override;
    virtual ErrorCodes_t deviceConfiguration() override;
    virtual void createCommunicationThreads() override;

    virtual void deinitializeMemory() override;
    virtual void deinitializeVariables() override;

    virtual void joinCommunicationThreads() override;

    virtual void initializeCalibration() override;

    virtual void handleCommunicationWithDevice() = 0;
    virtual uint32_t readDataFromDevice() = 0;
    virtual void parseDataFromDevice() = 0;

    void forceOutMessage();
    void updateVoltageHoldTuner(bool applyFlag);
    void updateCurrentHoldTuner(bool applyFlag);

    void storeFrameData(uint16_t rxMsgTypeId, RxMessageTypes_t rxMessageType);
    void stackOutgoingMessage(std::vector <uint16_t> &txDataMessage, CommandOptions_t commandOptions = CommandOptions_t());
    uint16_t popUint16FromRxRawBuffer();
    uint32_t popUint32FromRxRawBuffer();
    uint16_t readUint16FromRxRawBuffer(uint32_t n);
    uint32_t readUint32FromRxRawBuffer(uint32_t n);

    /************\
     *  Fields  *
    \************/

    unsigned int packetsPerFrame = 1;

    int motherboardBootTime_s = 1;
    int fwSize_B = 1000;

    /*! Read data buffer management */
    uint16_t rxMaxWords;
    uint32_t maxInputDataLoadSize;
    uint8_t * rxRawBuffer = nullptr; /*!< Raw incoming data from the device */
    uint16_t * rxRawBuffer16 = nullptr; /*!< Raw incoming data from the device */
    uint32_t rxRawBufferReadOffset = 0; /*!< Device Rx buffer offset position in which data are collected by the outputDataBuffer */
    uint32_t rxRawBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint32_t maxRxRawBytesRead = 0;
    uint32_t rxRawBytesAvailable = 0;
    uint32_t rxRawBufferWriteOffset = 0; /*!< Device Rx buffer offset position in which data are written by FTDI device */
    uint32_t rxRawBufferMask;
    MsgResume_t * rxMsgBuffer = nullptr; /*!< Buffer of pre-digested messages that contains message's high level info */
    uint32_t rxMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t rxMsgBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint32_t rxMsgBufferWriteOffset = 0;
    uint32_t rxPrevMsgBufferWriteOffset = 0;
    uint32_t rxDataBufferWriteOffset = 0;
    std::vector <uint16_t> voltageDataValues; /*! Store voltage data when current data and voltage data are not sent together in a single packet */
    std::vector <uint16_t> gpDataValues; /*! Store GP data when current data and GP data are not sent together in a single packet */
    bool gettingNextDataFlag = false;

    uint32_t lastParsedMsgType = MsgTypeIdInvalid; /*!< Type of the last parsed message to check for repetitions  */

    uint16_t * rxDataBuffer = nullptr; /*!< Buffer of pre-digested messages that contains message's data */

    /*! Write data buffer management */
    std::vector <uint16_t> * txMsgBuffer = nullptr; /*!< Buffer of arrays of bytes to communicate to the device */
    std::vector <uint16_t> txMsgOffsetWord; /*!< Buffer of offset word in txMsgBuffer */
    std::vector <uint16_t> txMsgLength; /*!< Buffer of txMsgBuffer length */
    std::vector <CommandOptions_t> txMsgOption; /*!< Buffer of commandOptions */
    uint32_t txMsgBufferWriteOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t txMsgBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint16_t txDataWords;
    uint16_t txMaxWords;
    uint16_t txMaxRegs;
    std::vector <uint16_t> txStatus; /*!< Status of the words written */
    uint16_t txModifiedStartingWord;
    uint16_t txModifiedEndingWord;

    std::vector <uint16_t> rxWordOffsets;
    std::vector <uint16_t> rxWordLengths;

    std::vector <bool> rxEnabledTypesMap; /*! key is any message type ID, value tells if the message should be returned by the getNextMessage method */

    // Calibration DAC ranges
    RangedMeasurement_t calibCcCurrentGainRange;
    std::vector <RangedMeasurement_t> calibCcCurrentOffsetRanges;

    RangedMeasurement_t calibVcVoltageGainRange;
    std::vector <RangedMeasurement_t> calibVcVoltageOffsetRanges;

    // Calibration ADC ranges
    RangedMeasurement_t calibVcCurrentGainRange;
    std::vector <RangedMeasurement_t> calibVcCurrentOffsetRanges;

    RangedMeasurement_t calibCcVoltageGainRange;
    std::vector <RangedMeasurement_t> calibCcVoltageOffsetRanges;

    ErrorCodes_t calibrationLoadingError = ErrorCalibrationNotLoadedYet;
    std::vector <std::string> calibrationFileNames = {""};
    std::vector <std::vector <bool> > calibrationFilesOkFlags = {{false}};
    std::string calibrationMappingFileDir = "";
    std::string calibrationMappingFilePath = "";

    /************\
     *  Coders  *
    \************/

    std::vector <CommandCoder *> coders;

    BoolCoder * asicResetCoder = nullptr;
    BoolCoder * fpgaResetCoder = nullptr;

    BoolCoder * clampingModeCoder = nullptr;
    std::vector <BoolCoder *> vcCurrentRangeCoders;
    std::vector <BoolCoder *> vcVoltageRangeCoders;
    std::vector <BoolCoder *> ccCurrentRangeCoders;
    std::vector <BoolCoder *> ccVoltageRangeCoders;
    BoolCoder * vcCurrentFilterCoder = nullptr;
    BoolCoder * vcVoltageFilterCoder = nullptr;
    BoolCoder * ccCurrentFilterCoder = nullptr;
    BoolCoder * ccVoltageFilterCoder = nullptr;

    std::vector <BoolCoder *> enableStimulusCoders;
    std::vector <BoolCoder *> turnChannelsOnCoders;
    BoolCoder * samplingRateCoder = nullptr;

    std::vector <std::vector <DoubleCoder *> > vHoldTunerCoders;
    std::vector <std::vector <DoubleCoder *> > vHalfTunerCoders;
    std::vector <std::vector <DoubleCoder *> > cHoldTunerCoders;
    std::vector <std::vector <DoubleCoder *> > cHalfTunerCoders;

    std::vector <std::vector <DoubleCoder *> > liquidJunctionVoltageCoders;
    std::vector <DoubleCoder *> calibCcCurrentGainCoders;
    std::vector <std::vector <DoubleCoder *> > calibCcCurrentOffsetCoders;
    std::vector <DoubleCoder *> calibVcVoltageGainCoders;
    std::vector <std::vector <DoubleCoder *> > calibVcVoltageOffsetCoders;
    std::vector <DoubleCoder *> calibVcCurrentGainCoders;
    std::vector <std::vector <DoubleCoder *> > calibVcCurrentOffsetCoders;
    std::vector <DoubleCoder *> calibCcVoltageGainCoders;
    std::vector <std::vector <DoubleCoder *> > calibCcVoltageOffsetCoders;
    std::vector <std::vector <DoubleCoder *> > calibRShuntConductanceCoders;
    std::vector <std::vector <DoubleCoder *> > calibRsCorrOffsetDacCoders;
    std::vector <DoubleCoder *> gateVoltageCoders;
    std::vector <DoubleCoder *> sourceVoltageCoders;

    std::vector <BoolCoder *> calSwCoders;
    std::vector <BoolCoder *> vcSwCoders;
    std::vector <BoolCoder *> ccSwCoders;
    std::vector <BoolCoder *> vcCcSelCoders;
    std::vector <BoolCoder *> ccStimEnCoders;
    BoolArrayCoder* sourceForVoltageChannelCoder = nullptr;
    BoolArrayCoder* sourceForCurrentChannelCoder = nullptr;

    BoolCoder * numberOfStatesCoder = nullptr;
    BoolCoder * initialStateCoder = nullptr;
    DoubleCoder * stateArrayMovingAverageLengthCoder = nullptr;
    std::vector <BoolCoder *> enableStateArrayChannelsCoder;

    std::vector <std::vector <DoubleCoder *> > stateAppliedVoltageCoders;

    std::vector <BoolArrayCoder *> stateTimeoutFlagCoders;
    std::vector <BoolArrayCoder *> stateTriggerFlagCoders;
    std::vector <BoolArrayCoder *> stateTriggerDeltaFlagCoders;
    std::vector <DoubleCoder *> stateTimeoutValueCoders;
    std::vector <BoolCoder *> stateTimeoutNextStateCoders;
    std::vector <std::vector <DoubleCoder *> > stateMinTriggerCurrentCoders;
    std::vector <std::vector <DoubleCoder *> > stateMaxTriggerCurrentCoders;
    std::vector <BoolCoder *> stateTriggerNextStateCoders;

    std::vector <BoolArrayCoder *> zapCoders;
    DoubleCoder * zapDurationCoder = nullptr;

    DoubleCoder * stimRestCoder = nullptr;

    BoolArrayCoder * bitDebugCoder = nullptr;
    BoolArrayCoder * wordDebugCoder = nullptr;

    /*! Protocol coders */
    bool protocolResetFlag = true;
    BoolArrayCoder * protocolResetCoder = nullptr;
    BoolArrayCoder * protocolIdCoder = nullptr;
    BoolArrayCoder * protocolItemsNumberCoder = nullptr;
    BoolArrayCoder * protocolSweepsNumberCoder = nullptr;
    std::vector <DoubleCoder *> voltageProtocolRestCoders;
    std::vector <DoubleCoder *> currentProtocolRestCoders;

    std::vector <std::vector <DoubleCoder *> > voltageProtocolStim0Coders;
    std::vector <std::vector <DoubleCoder *> > voltageProtocolStim0StepCoders;
    std::vector <std::vector <DoubleCoder *> > voltageProtocolStim1Coders;
    std::vector <std::vector <DoubleCoder *> > voltageProtocolStim1StepCoders;

    std::vector <std::vector <DoubleCoder *> > currentProtocolStim0Coders;
    std::vector <std::vector <DoubleCoder *> > currentProtocolStim0StepCoders;
    std::vector <std::vector <DoubleCoder *> > currentProtocolStim1Coders;
    std::vector <std::vector <DoubleCoder *> > currentProtocolStim1StepCoders;

    std::vector <DoubleCoder *> protocolTime0Coders;
    std::vector <DoubleCoder *> protocolTime0StepCoders;
    std::vector <DoubleCoder *> protocolFrequency0Coders;
    std::vector <DoubleCoder *> protocolFrequency0StepCoders;

    std::vector <BoolArrayCoder *> protocolItemIdxCoders;
    std::vector <BoolArrayCoder *> protocolNextItemIdxCoders;
    std::vector <BoolArrayCoder *> protocolLoopRepetitionsCoders;

    std::vector <BoolArrayCoder *> protocolApplyStepsCoders;
    std::vector <BoolArrayCoder *> protocolStimHalfCoders;
    std::vector <BoolArrayCoder *> protocolItemTypeCoders;

    std::vector <BoolCoder *> liquidJunctionCompensationCoders;

    /*! Compensations coders (all in asic domain) */
    std::vector <BoolCoder*> pipetteCapEnCompensationCoders;
    std::vector <MultiCoder*> pipetteCapValCompensationMultiCoders;
    std::vector <BoolCoder*> membraneCapEnCompensationCoders;
    std::vector <MultiCoder*> membraneCapValCompensationMultiCoders;
    std::vector <MultiCoder*> membraneCapTauValCompensationMultiCoders; // includes Tau Values and Tau Ranges
    std::vector <BoolCoder*>  rsCorrEnCompensationCoders;
    std::vector <DoubleCoder*>  rsCorrValCompensationCoders;
    std::vector <BoolArrayCoder*>  rsCorrBwCompensationCoders;
    std::vector <BoolCoder*>  rsPredEnCompensationCoders;
    std::vector <DoubleCoder*> rsPredGainCompensationCoders;
    std::vector <DoubleCoder*> rsPredTauCompensationCoders;
    std::vector <BoolCoder*> pipetteCapCcEnCompensationCoders;
    std::vector <MultiCoder*> pipetteCapCcValCompensationMultiCoders;

    std::vector <BoolArrayCoder *> customFlagsCoders;
    std::vector <BoolArrayCoder *> customOptionsCoders;
    std::vector <DoubleCoder *> customDoublesCoders;

    /********************************************\
     *  Multi-thread synchronization variables  *
    \********************************************/

    std::thread deviceCommunicationThread;
    std::thread rxConsumerThread;

    mutable std::mutex rxRawMutex;
    std::condition_variable rxRawBufferNotEmpty;
    std::condition_variable rxRawBufferNotFull;

    mutable std::mutex rxMsgMutex;
    std::condition_variable rxMsgBufferNotEmpty;
    std::condition_variable rxMsgBufferNotFull;
};

#endif // EMCRDEVICE_H
