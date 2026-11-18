#ifndef EMCRDEVICE_H
#define EMCRDEVICE_H

class EmcrDevice : public MessageDispatcher {
public:

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
    ErrorCodes_t setCurrentTracking(std::vector <uint16_t> channelIndexes, std::vector <Measurement_t> currents, bool enable) override;

    ErrorCodes_t setAdcFilter(bool applyFlag = false) override;
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) override;
    ErrorCodes_t setClockDiv(uint16_t clockDivIdx, bool applyFlag) override;

    ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status, bool applyFlag = true) override;
    ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue) override;
    ErrorCodes_t setDebugTrigger(uint16_t bitOffset) override;

    ErrorCodes_t setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest, bool stopProtocolFlag) override;
    ErrorCodes_t setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag, std::vector <uint16_t> activeDigitalOutputs = {}) override;
    ErrorCodes_t setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag, std::vector <uint16_t> activeDigitalOutputs = {}) override;
    ErrorCodes_t setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag, std::vector <uint16_t> activeDigitalOutputs = {}) override;

    ErrorCodes_t setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest, bool stopProtocolFlag) override;
    ErrorCodes_t setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag, std::vector <uint16_t> activeDigitalOutputs = {}) override;
    ErrorCodes_t setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag, std::vector <uint16_t> activeDigitalOutputs = {}) override;
    ErrorCodes_t setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag, std::vector <uint16_t> activeDigitalOutputs = {}) override;

    virtual ErrorCodes_t setCompRanges(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <uint16_t> newRanges, bool applyFlag) override;

    ErrorCodes_t setStateArrayStructure(int numberOfStates, int initialState, Measurement_t reactionTime) override;
    ErrorCodes_t setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, Measurement_t timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag) override;
    ErrorCodes_t setStateArrayEnabled(int chIdx, bool enabledFlag) override;

    /*! Device specific controls */

    virtual ErrorCodes_t sendSpiCommand(uint32_t command, uint32_t dataLoad) override;

    virtual ErrorCodes_t setCustomFlag(uint16_t idx, bool flag, bool applyFlag) override;
    virtual ErrorCodes_t setCustomOption(uint16_t idx, uint16_t value, bool applyFlag) override;
    virtual ErrorCodes_t setCustomDouble(uint16_t idx, double value, bool applyFlag) override;

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t getNextMessage(RxOutput_t &rxOutput, int16_t * data, MsgTypeId_t type) override;
    ErrorCodes_t getStoredMessage(RxOutput_t &rxOutput, int16_t * data, MsgTypeId_t type) override;
    ErrorCodes_t purgeData() override;

    ErrorCodes_t getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures) override;
    ErrorCodes_t getVoltageHoldTuner(std::vector <Measurement_t> &voltages) override;
    ErrorCodes_t getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTunerFeatures) override;
    ErrorCodes_t getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures) override;
    ErrorCodes_t getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTunerFeatures) override;
    ErrorCodes_t getVoltageRampTunerFeatures(std::vector <RangedMeasurement_t> &voltageRanges, RangedMeasurement_t &durationRange) override;
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

    ErrorCodes_t getCalibrationStatus() override;
    ErrorCodes_t getCalibParams(CalibrationParams_t &calibParams) override;
    ErrorCodes_t getCalibFileNames(std::vector <std::string> &calibFileNames) override;
    ErrorCodes_t getCalibFilesFlags(std::vector <std::vector <bool> > &calibFilesFlags) override;
    ErrorCodes_t getCalibMappingFileDir(std::string &dir) override;
    ErrorCodes_t getCalibMappingFilePath(std::string &path) override;

protected:

    /*************\
     *  Methods  *
    \*************/

    virtual ErrorCodes_t initializeMemory() override;
    virtual void initializeVariables() override;
    virtual ErrorCodes_t deviceConfiguration() override;
    virtual void createCommunicationThreads() override;

    virtual void enableOverheatingCounterMeasures() override;
    virtual void deinitializeMemory() override;
    virtual void deinitializeVariables() override;

    virtual void joinCommunicationThreads() override;

    virtual void initializeCalibration() override;
    virtual void initializeLiquidJunction() override;

};

#endif // EMCRDEVICE_H
