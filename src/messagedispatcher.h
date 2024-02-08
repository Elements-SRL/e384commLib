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

class MessageDispatcher {
public:

    /*****************\
     *  Ctor / Dtor  *
    \*****************/

    MessageDispatcher(std::string deviceId);
    virtual ~MessageDispatcher();

    enum CompensationTypes {
        CompCfast, // pipette
        CompCslow, // membrane
        CompRsCorr,
        CompRsPred,
        CompCcCfast,
        CompensationTypesNum
    };

    enum CompensationUserParams {
        U_CpVc,     // VCPipetteCapacitance
        U_Cm,       // MembraneCapacitance
        U_Rs,       // SeriesResistance
        U_RsCp,     // SeriesCorrectionPerc
        U_RsPg,     // SeriesPredictionGain
        U_CpCc,     // CCPipetteCapacitance
        CompensationUserParamsNum
    };

    /************************\
     *  Connection methods  *
    \************************/

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = "");
    virtual ErrorCodes_t initialize(std::string fwPath) = 0;
    virtual void deinitialize() = 0;
    virtual ErrorCodes_t disconnectDevice() = 0;
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

    virtual ErrorCodes_t sendCommands();
    virtual ErrorCodes_t startProtocol();
    ErrorCodes_t stopProtocol();
    virtual ErrorCodes_t startStateArray();

    virtual ErrorCodes_t resetAsic(bool resetFlag, bool applyFlag = true);
    virtual ErrorCodes_t resetFpga(bool resetFlag, bool applyFlag = true);

    virtual ErrorCodes_t setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag);
    virtual ErrorCodes_t setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag);
    virtual ErrorCodes_t setVoltageHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag);
    virtual ErrorCodes_t setCurrentHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlag);
    virtual ErrorCodes_t setLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlag);
    virtual ErrorCodes_t resetLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setGateVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag);
    virtual ErrorCodes_t setSourceVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag);

    virtual ErrorCodes_t setCalibParams(CalibrationParams_t calibParams);
    virtual ErrorCodes_t setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    virtual ErrorCodes_t setCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    virtual ErrorCodes_t updateCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);

    virtual ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag);
    virtual ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag);
    virtual ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag);
    virtual ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag);
    virtual ErrorCodes_t setLiquidJunctionRange(uint16_t idx);

    virtual ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlag);
    virtual ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlag);

    virtual ErrorCodes_t enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t turnChannelsOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t turnCalSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t turnVcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t turnCcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t turnVcCcSelOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t enableCcStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    virtual ErrorCodes_t setClampingModality(uint32_t idx, bool applyFlag, bool stopProtocolFlag);
    virtual ErrorCodes_t setClampingModality(ClampingModality_t mode, bool applyFlag, bool stopProtocolFlag);
    virtual ErrorCodes_t setSourceForVoltageChannel(uint16_t source, bool applyFlag);
    virtual ErrorCodes_t setSourceForCurrentChannel(uint16_t source, bool applyFlag);

    virtual ErrorCodes_t digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t expandTraces(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues);

    virtual ErrorCodes_t setAdcFilter();
    virtual ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag);
    ErrorCodes_t setDownsamplingRatio(uint32_t ratio);

    virtual ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status);
    virtual ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue);

    virtual ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlag);
    virtual ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlag);
    virtual ErrorCodes_t turnVoltageStimulusOn(bool onValue, bool applyFlag);
    virtual ErrorCodes_t turnCurrentStimulusOn(bool onValue, bool applyFlag);

    virtual ErrorCodes_t setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest);
    virtual ErrorCodes_t setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag);
    virtual ErrorCodes_t setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag);
    virtual ErrorCodes_t setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag);

    virtual ErrorCodes_t setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest);
    virtual ErrorCodes_t setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag);
    virtual ErrorCodes_t setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag);
    virtual ErrorCodes_t setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag);

    virtual ErrorCodes_t setStateArrayStructure(int numberOfStates, int initialState, Measurement_t reactionTime);
    virtual ErrorCodes_t setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, double timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag);
    virtual ErrorCodes_t setStateArrayEnabled(int chIdx, bool enabledFlag);

    virtual ErrorCodes_t enableCompensation(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> onValues, bool applyFlag);
    virtual ErrorCodes_t enableVcCompensations(bool enable, bool applyFlag);
    virtual ErrorCodes_t enableCcCompensations(bool enable, bool applyFlag);
    virtual ErrorCodes_t setCompValues(std::vector<uint16_t> channelIndexes, CompensationUserParams paramToUpdate, std::vector<double> newParamValues, bool applyFlag);
    virtual ErrorCodes_t setCompOptions(std::vector<uint16_t> channelIndexes, CompensationTypes type, std::vector<uint16_t> options, bool applyFlag);

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t getSerialNumber(std::string &serialNumber);

    ErrorCodes_t getBoards(std::vector <BoardModel *> &boards);
    ErrorCodes_t getChannels(std::vector <ChannelModel *> &channels);
    ErrorCodes_t getSelectedChannels(std::vector <bool> &selected);
    ErrorCodes_t getSelectedChannelsIndexes(std::vector <uint16_t> &indexes);

    ErrorCodes_t getRxDataBufferSize(uint32_t &size);
    ErrorCodes_t allocateRxDataBuffer(int16_t * &data);
    ErrorCodes_t deallocateRxDataBuffer(int16_t * &data);
    virtual ErrorCodes_t getNextMessage(RxOutput_t &rxOutput, int16_t * data);
    virtual ErrorCodes_t purgeData();
    ErrorCodes_t convertVoltageValue(int16_t intValue, double &fltValue);
    ErrorCodes_t convertCurrentValue(int16_t intValue, double &fltValue);
    ErrorCodes_t convertVoltageValues(int16_t * intValue, double * fltValue, int valuesNum);
    ErrorCodes_t convertCurrentValues(int16_t * intValue, double * fltValue, int valuesNum);

    ErrorCodes_t getLiquidJunctionVoltages(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> &voltages);

    virtual ErrorCodes_t getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures);
    virtual ErrorCodes_t getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTunerFeatures);
    virtual ErrorCodes_t getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures);
    virtual ErrorCodes_t getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTunerFeatures);
    virtual ErrorCodes_t getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges);
    virtual ErrorCodes_t getCalibVcCurrentGainFeatures(RangedMeasurement_t &calibVcCurrentGainFeatures);
    virtual ErrorCodes_t getCalibVcCurrentOffsetFeatures(std::vector<RangedMeasurement_t> &calibVcCurrentOffsetFeatures);
    virtual ErrorCodes_t getCalibCcVoltageGainFeatures(RangedMeasurement_t &calibCcVoltageGainFeatures);
    virtual ErrorCodes_t getCalibCcVoltageOffsetFeatures(std::vector<RangedMeasurement_t> &calibCcVoltageOffsetFeatures);
    virtual ErrorCodes_t hasGateVoltages();
    virtual ErrorCodes_t hasSourceVoltages();
    virtual ErrorCodes_t getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures);
    virtual ErrorCodes_t getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures);

    ErrorCodes_t getChannelNumberFeatures(uint16_t &voltageChannelNumberFeatures, uint16_t &currentChannelNumberFeatures);
    ErrorCodes_t getChannelNumberFeatures(int &voltageChannelNumberFeatures, int &currentChannelNumberFeatures);
    ErrorCodes_t getAvailableChannelsSourcesFeatures(ChannelSources_t &voltageSourcesIdxs, ChannelSources_t &currentSourcesIdxs);
    ErrorCodes_t getBoardsNumberFeatures(uint16_t &boardsNumberFeatures);
    ErrorCodes_t getBoardsNumberFeatures(int &boardsNumberFeatures);
    ErrorCodes_t getClampingModalitiesFeatures(std::vector<ClampingModality_t> &clampingModalitiesFeatures);
    ErrorCodes_t getClampingModality(ClampingModality_t &clampingModality);
    ErrorCodes_t getClampingModalityIdx(uint32_t &idx);

    ErrorCodes_t getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges, uint16_t &defaultVcCurrRangeIdx);
    ErrorCodes_t getVCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges);
    ErrorCodes_t getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges);
    ErrorCodes_t getCCVoltageRanges(std::vector <RangedMeasurement_t> &voltageRanges);

    ErrorCodes_t getVCCurrentRange(RangedMeasurement_t &range);
    ErrorCodes_t getVCVoltageRange(RangedMeasurement_t &range);
    ErrorCodes_t getLiquidJunctionRange(RangedMeasurement_t &range);
    ErrorCodes_t getCCCurrentRange(RangedMeasurement_t &range);
    ErrorCodes_t getCCVoltageRange(RangedMeasurement_t &range);

    ErrorCodes_t getVCCurrentRangeIdx(uint32_t &idx);
    ErrorCodes_t getVCVoltageRangeIdx(uint32_t &idx);
    ErrorCodes_t getCCCurrentRangeIdx(uint32_t &idx);
    ErrorCodes_t getCCVoltageRangeIdx(uint32_t &idx);

    ErrorCodes_t getMaxVCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinVCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMaxVCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinVCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMaxCCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinCCCurrentRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMaxCCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);
    ErrorCodes_t getMinCCVoltageRange(RangedMeasurement_t &range, uint32_t &idx);

    ErrorCodes_t getSamplingRatesFeatures(std::vector <Measurement_t> &samplingRates);
    ErrorCodes_t getSamplingRate(Measurement_t &samplingRate);
    ErrorCodes_t getSamplingRateIdx(uint32_t &idx);
    ErrorCodes_t getRealSamplingRatesFeatures(std::vector <Measurement_t> &realSamplingRates);
    ErrorCodes_t getMaxDownsamplingRatioFeature(uint32_t &ratio);
    ErrorCodes_t getDownsamplingRatio(uint32_t &ratio);

    ErrorCodes_t getVCVoltageFilters(std::vector <Measurement_t> &filters);
    ErrorCodes_t getVCCurrentFilters(std::vector <Measurement_t> &filters);
    ErrorCodes_t getCCVoltageFilters(std::vector <Measurement_t> &filters);
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

    ErrorCodes_t getVoltageProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range);
    ErrorCodes_t getCurrentProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range);
    ErrorCodes_t getTimeProtocolRangeFeature(RangedMeasurement_t &range);
    ErrorCodes_t getFrequencyProtocolRangeFeature(RangedMeasurement_t &range);

    ErrorCodes_t getMaxProtocolItemsFeature(uint32_t &num);
    ErrorCodes_t hasProtocols();
    ErrorCodes_t hasProtocolStepFeature();
    ErrorCodes_t hasProtocolRampFeature();
    ErrorCodes_t hasProtocolSinFeature();
    virtual ErrorCodes_t isStateArrayAvailable();

    virtual ErrorCodes_t getCalibData(CalibrationData_t &calibData);
    virtual ErrorCodes_t getCalibParams(CalibrationParams_t &calibParams);
    virtual ErrorCodes_t getCalibFileNames(std::vector<std::string> &calibFileNames);
    virtual ErrorCodes_t getCalibFilesFlags(std::vector<std::vector <bool>> &calibFilesFlags);
    virtual ErrorCodes_t getCalibMappingFileDir(std::string &dir);
    virtual ErrorCodes_t getCalibMappingFilePath(std::string &path);

    virtual ErrorCodes_t hasCompFeature(uint16_t feature);
    virtual ErrorCodes_t getCompFeatures(uint16_t paramToExtractFeatures, std::vector<RangedMeasurement_t> &compensationFeatures, double &defaultParamValue);
    virtual ErrorCodes_t getCompOptionsFeatures(CompensationTypes type ,std::vector <std::string> &compOptionsArray);
    virtual ErrorCodes_t getCompValueMatrix(std::vector<std::vector<double>> &compValueMatrix);
    virtual ErrorCodes_t getCompensationEnables(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> &onValues);

    virtual ErrorCodes_t getPipetteCompensationOptions(std::vector <std::string> &options);
    virtual ErrorCodes_t getCCPipetteCompensationOptions(std::vector <std::string> &options);
    virtual ErrorCodes_t getMembraneCompensationOptions(std::vector <std::string> &options);
    virtual ErrorCodes_t getResistanceCompensationOptions(std::vector <std::string> &options);
    virtual ErrorCodes_t getResistanceCorrectionOptions(std::vector <std::string> &options);
    virtual ErrorCodes_t getResistancePredictionOptions(std::vector <std::string> &options);
    virtual ErrorCodes_t getLeakConductanceCompensationOptions(std::vector <std::string> &options);
    virtual ErrorCodes_t getBridgeBalanceCompensationOptions(std::vector <std::string> &options);

    virtual ErrorCodes_t getPipetteCapacitanceControl(CompensationControl_t &control);
    virtual ErrorCodes_t getCCPipetteCapacitanceControl(CompensationControl_t &control);
    virtual ErrorCodes_t getMembraneCapacitanceControl(CompensationControl_t &control);
    virtual ErrorCodes_t getAccessResistanceControl(CompensationControl_t &control);
    virtual ErrorCodes_t getResistanceCorrectionPercentageControl(CompensationControl_t &control);
    virtual ErrorCodes_t getResistanceCorrectionLagControl(CompensationControl_t &control);
    virtual ErrorCodes_t getResistancePredictionGainControl(CompensationControl_t &control);
    virtual ErrorCodes_t getResistancePredictionPercentageControl(CompensationControl_t &control);
    virtual ErrorCodes_t getResistancePredictionBandwidthGainControl(CompensationControl_t &control);
    virtual ErrorCodes_t getResistancePredictionTauControl(CompensationControl_t &control);
    virtual ErrorCodes_t getLeakConductanceControl(CompensationControl_t &control);
    virtual ErrorCodes_t getBridgeBalanceResistanceControl(CompensationControl_t &control);

    virtual ErrorCodes_t getAccessResistanceCorrectionLag(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getAccessResistancePredictionPercentage(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getAccessResistancePredictionBandwidthGain(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getAccessResistancePredictionTau(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getLeakConductance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);
    virtual ErrorCodes_t getBridgeBalanceResistance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, std::vector<bool> activeNotActive);

protected:
    typedef enum RxMessageTypes {
        RxMessageDataLoad,
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

    std::vector <RangedMeasurement_t> vcLeakCalibRange;

    /*! Compensation options*/
    std::vector <uint16_t> selectedRsCorrBws;
    std::vector <Measurement_t> rsCorrBwArray;
    uint16_t defaultRsCorrBwIdx;

    /*! Features in ASIC domain, depend on asic*/
    std::vector <RangedMeasurement> pipetteCapacitanceRange_pF;
    std::vector <RangedMeasurement> membraneCapValueRange_pF;
    std::vector <RangedMeasurement> membraneCapTauValueRange_us;
    RangedMeasurement_t rsCorrValueRange;
    RangedMeasurement_t rsPredGainRange;
    RangedMeasurement_t rsPredTauRange;

    /*! Features in USER domain, depend on the asic parameters*/
    std::vector <RangedMeasurement> uCpVcCompensable;
    std::vector <RangedMeasurement> uCmCompensable;
    std::vector <RangedMeasurement> uRsCompensable;
    std::vector <RangedMeasurement> uRsCpCompensable;
    std::vector <RangedMeasurement> uRsPgCompensable;
    std::vector <RangedMeasurement> uCpCcCompensable;

    /*! Default paramter values in USER domain*/
    std::vector <double> defaultUserDomainParams;

    std::vector <double> membraneCapValueInjCapacitance;
    std::vector <std::vector<std::string>> compensationOptionStrings;

    bool anyLiquidJuctionActive = false;

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

    std::vector<std::vector<double>> compValueMatrix;
    std::vector<bool> compCfastEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compCslowEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compRsCompEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compRsCorrEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compRsPredEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compLeakEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compCcCfastEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compCcBridgeEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    bool vcCompensationsActivated = false;
    bool ccCompensationsActivated = false;

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
