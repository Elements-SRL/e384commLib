#ifndef EMCRDEVICE_H
#define EMCRDEVICE_H

#include "messagedispatcher.h"
#include "commandcoder.h"

class EmcrDevice : public MessageDispatcher {
public:
    EmcrDevice(std::string deviceId);
    virtual ~EmcrDevice();

    /************************\
     *  Connection methods  *
    \************************/

    virtual ErrorCodes_t connect(std::string fwPath) override;
    virtual ErrorCodes_t disconnect() override;

    ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag) override;

    /****************\
     *  Tx methods  *
    \****************/

    ErrorCodes_t sendCommands() override;
    ErrorCodes_t startProtocol() override;
    ErrorCodes_t startStateArray() override;

    ErrorCodes_t resetAsic(bool resetFlag, bool applyFlagIn = true) override;
    ErrorCodes_t resetFpga(bool resetFlag, bool applyFlagIn = true) override;
    ErrorCodes_t setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn) override;
    ErrorCodes_t setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlagIn) override;
    ErrorCodes_t setVoltageHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn) override;
    ErrorCodes_t setCurrentHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlagIn) override;
    ErrorCodes_t setLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn) override;
    ErrorCodes_t resetLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, bool applyFlagIn) override;
    ErrorCodes_t setGateVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag) override;
    ErrorCodes_t setSourceVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag) override;

    ErrorCodes_t setCalibParams(CalibrationParams_t calibParams) override;
    ErrorCodes_t setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag) override;
    ErrorCodes_t updateCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag) override;
    ErrorCodes_t setCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag) override;
    ErrorCodes_t updateCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag) override;

    ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn) override;
    ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn) override;
    ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn) override;
    ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn) override;
    ErrorCodes_t setLiquidJunctionRange(uint16_t idx) override;

    ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlagIn) override;
    ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlagIn) override;

    ErrorCodes_t enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnChannelsOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnCalSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnVcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnCcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;
    ErrorCodes_t turnVcCcSelOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;
    ErrorCodes_t enableCcStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;

    ErrorCodes_t setClampingModality(uint32_t idx, bool applyFlag) override;
    ErrorCodes_t setClampingModality(ClampingModality_t mode, bool applyFlag) override;
    ErrorCodes_t setSourceForVoltageChannel(uint16_t source, bool applyFlag) override;
    ErrorCodes_t setSourceForCurrentChannel(uint16_t source, bool applyFlag) override;

    ErrorCodes_t digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag) override;

    ErrorCodes_t setAdcFilter() override;
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlagIn) override;

    ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status) override;
    ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue) override;

    ErrorCodes_t setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest) override;
    ErrorCodes_t setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) override;
    ErrorCodes_t setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag) override;
    ErrorCodes_t setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag) override;

    ErrorCodes_t setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest) override;
    ErrorCodes_t setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag) override;
    ErrorCodes_t setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag) override;
    ErrorCodes_t setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag) override;

    ErrorCodes_t setStateArrayStructure(int numberOfStates, int initialState) override;
    ErrorCodes_t setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, double timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag) override;
    ErrorCodes_t setStateArrayEnabled(int chIdx, bool enabledFlag) override;

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t getCalibData(CalibrationData_t &calibData) override;
    ErrorCodes_t getCalibParams(CalibrationParams_t &calibParams) override;
    ErrorCodes_t getCalibFileNames(std::vector<std::string> &calibFileNames) override;
    ErrorCodes_t getCalibFilesFlags(std::vector<std::vector <bool>> &calibFilesFlags) override;
    ErrorCodes_t getCalibMappingFileDir(std::string &dir) override;
    ErrorCodes_t getCalibMappingFilePath(std::string &path) override;

protected:

    /*************\
     *  Methods  *
    \*************/

    ErrorCodes_t init();
    ErrorCodes_t deinit();

    virtual ErrorCodes_t initializeCalibration() override;

    virtual void handleCommunicationWithDevice() = 0;
    virtual uint32_t readDataFromDevice() = 0;
    virtual void parseDataFromDevice() = 0;

    void forceOutMessage();
    void updateVoltageHoldTuner(bool applyFlag);
    void updateCurrentHoldTuner(bool applyFlag);

    void storeFrameData(uint16_t rxMsgTypeId, RxMessageTypes_t rxMessageType);
    void stackOutgoingMessage(std::vector <uint16_t> &txDataMessage, TxTriggerType_t triggerType = TxTriggerParameteresUpdated);
    uint16_t popUint16FromRxRawBuffer();
    uint16_t readUint16FromRxRawBuffer(uint32_t n);

    /************\
     *  Fields  *
    \************/

    /*! Write data buffer management */
    std::vector <uint16_t> * txMsgBuffer; /*!< Buffer of arrays of bytes to communicate to the device */
    std::vector <uint16_t> txMsgOffsetWord; /*!< Buffer of offset word in txMsgBuffer */
    std::vector <uint16_t> txMsgLength; /*!< Buffer of txMsgBuffer length */
    std::vector <TxTriggerType_t> txMsgTrigger; /*!< Buffer of trigger types */
    uint32_t txMsgBufferWriteOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t txMsgBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint16_t txDataWords;
    uint16_t txMaxWords;
    uint16_t txMaxRegs;
    std::vector <uint16_t> txStatus; /*!< Status of the words written */
    uint16_t txModifiedStartingWord;
    uint16_t txModifiedEndingWord;

    std::vector<uint16_t> rxWordOffsets;
    std::vector<uint16_t> rxWordLengths;

    std::vector <bool> rxEnabledTypesMap; /*! key is any message type ID, value tells if the message should be returned by the getNextMessage method */

    ErrorCodes_t calibrationLoadingError = ErrorCalibrationNotLoadedYet;
    CalibrationParams_t calibrationParams;
    std::vector<std::string> calibrationFileNames;
    std::vector<std::vector<bool>> calibrationFilesOkFlags;
    std::string calibrationMappingFileDir;
    std::string calibrationMappingFilePath;

    /************\
     *  Coders  *
    \************/

    std::vector <CommandCoder *> coders;

    BoolCoder * asicResetCoder = nullptr;
    BoolCoder * fpgaResetCoder = nullptr;

    BoolCoder * clampingModeCoder = nullptr;
    BoolCoder * vcCurrentRangeCoder = nullptr;
    BoolCoder * vcVoltageRangeCoder = nullptr;
    BoolCoder * ccCurrentRangeCoder = nullptr;
    BoolCoder * ccVoltageRangeCoder = nullptr;
    BoolCoder * vcCurrentFilterCoder = nullptr;
    BoolCoder * vcVoltageFilterCoder = nullptr;
    BoolCoder * ccCurrentFilterCoder = nullptr;
    BoolCoder * ccVoltageFilterCoder = nullptr;

    std::vector <BoolCoder *> enableStimulusCoders;
    std::vector <BoolCoder *> turnChannelsOnCoders;
    BoolCoder * samplingRateCoder = nullptr;

    std::vector <std::vector <DoubleCoder *>> vHoldTunerCoders;
    std::vector <std::vector <DoubleCoder *>> vHalfTunerCoders;
    std::vector <std::vector <DoubleCoder *>> cHoldTunerCoders;
    std::vector <std::vector <DoubleCoder *>> cHalfTunerCoders;

    std::vector <std::vector <DoubleCoder *>> liquidJunctionVoltageCoders;
    std::vector <DoubleCoder *> calibCcCurrentGainCoders;
    std::vector <std::vector <DoubleCoder *>> calibCcCurrentOffsetCoders;
    std::vector <DoubleCoder *> calibVcVoltageGainCoders;
    std::vector <std::vector <DoubleCoder *>> calibVcVoltageOffsetCoders;
    std::vector <DoubleCoder *> calibVcCurrentGainCoders;
    std::vector <std::vector <DoubleCoder *>> calibVcCurrentOffsetCoders;
    std::vector <DoubleCoder *> calibCcVoltageGainCoders;
    std::vector <std::vector <DoubleCoder *>> calibCcVoltageOffsetCoders;
    std::vector <DoubleCoder *> gateVoltageCoders;
    std::vector <DoubleCoder *> sourceVoltageCoders;

    BoolArrayCoder* voltageChanSourceCoder;
    BoolArrayCoder* currentChanSourceCoder;

    std::vector <BoolCoder *> calSwCoders;
    std::vector <BoolCoder *> vcSwCoders;
    std::vector <BoolCoder *> ccSwCoders;
    std::vector <BoolCoder *> vcCcSelCoders;
    std::vector <BoolCoder *> ccStimEnCoders;
    BoolArrayCoder* sourceForVoltageChannelCoder = nullptr;
    BoolArrayCoder* sourceForCurrentChannelCoder = nullptr;

    std::vector <std::vector <DoubleCoder *>> vcLeakCalibCoders;

    BoolCoder * numberOfStatesCoder = nullptr;
    BoolCoder * initialStateCoder = nullptr;
    std::vector<BoolCoder*> enableStateArrayChannelsCoder;

    std::vector<std::vector<DoubleCoder *>> appliedVoltageCoders;

    std::vector<BoolArrayCoder *> stateTimeoutFlagCoders;
    std::vector<BoolArrayCoder *> stateTriggerFlagCoders;
    std::vector<BoolArrayCoder *> stateTriggerDeltaFlagCoders;
    std::vector<DoubleCoder *> stateTimeoutValueCoders;
    std::vector<BoolCoder *> stateTimeoutNextStateCoders;
    std::vector<std::vector<DoubleCoder *>> stateMinTriggerCurrentCoders;
    std::vector<std::vector<DoubleCoder *>> stateMaxTriggerCurrentCoders;
    std::vector<BoolCoder *> stateTriggerNextStateCoders;

    DoubleCoder * stimRestCoder = nullptr;

    BoolArrayCoder * bitDebugCoder = nullptr;
    BoolArrayCoder * wordDebugCoder = nullptr;

    /*! Protocol coders */
    BoolArrayCoder * protocolIdCoder = nullptr;
    BoolArrayCoder * protocolItemsNumberCoder = nullptr;
    BoolArrayCoder * protocolSweepsNumberCoder = nullptr;
    std::vector <DoubleCoder *> voltageProtocolRestCoders;
    std::vector <DoubleCoder *> currentProtocolRestCoders;

    std::vector <std::vector <DoubleCoder *>> voltageProtocolStim0Coders;
    std::vector <std::vector <DoubleCoder *>> voltageProtocolStim0StepCoders;
    std::vector <std::vector <DoubleCoder *>> voltageProtocolStim1Coders;
    std::vector <std::vector <DoubleCoder *>> voltageProtocolStim1StepCoders;

    std::vector <std::vector <DoubleCoder *>> currentProtocolStim0Coders;
    std::vector <std::vector <DoubleCoder *>> currentProtocolStim0StepCoders;
    std::vector <std::vector <DoubleCoder *>> currentProtocolStim1Coders;
    std::vector <std::vector <DoubleCoder *>> currentProtocolStim1StepCoders;

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

    std::vector <BoolCoder *> digitalOffsetCompensationCoders;

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

};

#endif // EMCRDEVICE_H
