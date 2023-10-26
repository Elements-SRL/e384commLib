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
#include "commandcoder.h"
#include "boardmodel.h"

#ifdef E384COMMLIB_LABVIEW_WRAPPER
#include "e384commlib_global_addendum.h"
#endif

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
    virtual ErrorCodes_t disconnectDevice();
    virtual ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag);

    virtual ErrorCodes_t connect(std::string fwPath);
    virtual ErrorCodes_t disconnect();

    /****************\
     *  Tx methods  *
    \****************/

    ErrorCodes_t initializeDevice();

    ErrorCodes_t setChannelSelected(uint16_t chIdx, bool newState);
    ErrorCodes_t setBoardSelected(uint16_t brdIdx, bool newState);
    ErrorCodes_t setRowSelected(uint16_t rowIdx, bool newState);
    ErrorCodes_t setAllChannelsSelected(bool newState);

    ErrorCodes_t sendCommands();
    ErrorCodes_t resetAsic(bool resetFlag, bool applyFlagIn = true);
    ErrorCodes_t resetFpga(bool resetFlag, bool applyFlagIn = true);
    ErrorCodes_t setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn);
    ErrorCodes_t setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlagIn);
    ErrorCodes_t setVoltageHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn);
    ErrorCodes_t setCurrentHalf(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> currents, bool applyFlagIn);
    ErrorCodes_t setLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn);
    ErrorCodes_t resetLiquidJunctionVoltage(std::vector<uint16_t> channelIndexes, bool applyFlagIn);
    ErrorCodes_t setGateVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag);
    ErrorCodes_t setSourceVoltages(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag);

    ErrorCodes_t setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    ErrorCodes_t updateCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    ErrorCodes_t setCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    ErrorCodes_t updateCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);
    ErrorCodes_t setCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    ErrorCodes_t updateCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    ErrorCodes_t setCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    ErrorCodes_t updateCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);
    ErrorCodes_t setCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    ErrorCodes_t updateCalibVcVoltageGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    ErrorCodes_t setCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    ErrorCodes_t updateCalibVcVoltageOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);
    ErrorCodes_t setCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    ErrorCodes_t updateCalibCcCurrentGain(std::vector<uint16_t> channelIndexes, bool applyFlag);
    ErrorCodes_t setCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    ErrorCodes_t updateCalibCcCurrentOffset(std::vector<uint16_t> channelIndexes, bool applyFlag);

    ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn);
    ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn);
    ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn);
    ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn);
    virtual ErrorCodes_t setLiquidJunctionRange(uint16_t idx);

    ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlagIn);
    ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlagIn);

    ErrorCodes_t enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t turnChannelsOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t turnCalSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t turnVcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t turnCcSwOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t turnVcCcSelOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t enableCcStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    ErrorCodes_t setClampingModality(uint32_t idx, bool applyFlag);
    ErrorCodes_t setClampingModality(ClampingModality_t mode, bool applyFlag);
    ErrorCodes_t setSourceForVoltageChannel(uint16_t source, bool applyFlag);
    ErrorCodes_t setSourceForCurrentChannel(uint16_t source, bool applyFlag);

    ErrorCodes_t digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t expandTraces(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues);

    ErrorCodes_t setAdcFilter();
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlagIn);
    ErrorCodes_t setDownsamplingRatio(uint32_t ratio);

    ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status);
    ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue);

    virtual ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlagIn);
    virtual ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlagIn);
    virtual ErrorCodes_t turnVoltageStimulusOn(bool onValue, bool applyFlag);
    virtual ErrorCodes_t turnCurrentStimulusOn(bool onValue, bool applyFlag);

    ErrorCodes_t setCalibParams(CalibrationParams_t calibParams);

    virtual ErrorCodes_t turnResistanceCompensationOn(std::vector<uint16_t> channelIndexes,std::vector<bool> onValues, bool applyFlagIn);
    virtual ErrorCodes_t turnLeakConductanceCompensationOn(std::vector<uint16_t> channelIndexes,std::vector<bool> onValues, bool applyFlagIn);
    virtual ErrorCodes_t turnBridgeBalanceCompensationOn(std::vector<uint16_t> channelIndexes,std::vector<bool> onValues, bool applyFlagIn);
    virtual ErrorCodes_t enableCompensation(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> onValues, bool applyFlagIn);
    virtual ErrorCodes_t enableVcCompensations(bool enable);
    virtual ErrorCodes_t enableCcCompensations(bool enable);
    virtual ErrorCodes_t setCompValues(std::vector<uint16_t> channelIndexes, CompensationUserParams paramToUpdate, std::vector<double> newParamValues, bool applyFlagIn);
    virtual ErrorCodes_t setCompOptions(std::vector<uint16_t> channelIndexes, CompensationTypes type, std::vector<uint16_t> options, bool applyFlagIn);
    virtual ErrorCodes_t setLeakConductance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn);
    virtual ErrorCodes_t setBridgeBalanceResistance(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn);
    
    virtual ErrorCodes_t setPipetteCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn);
    virtual ErrorCodes_t setMembraneCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn);
    virtual ErrorCodes_t setResistanceCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn);
    virtual ErrorCodes_t setResistancePredictionOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn);
    virtual ErrorCodes_t setCCPipetteCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn);
    virtual ErrorCodes_t setLeakConductanceCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn);
    virtual ErrorCodes_t setBridgeBalanceCompensationOptions(std::vector<uint16_t> channelIndexes, std::vector<uint16_t> optionIndexes, bool applyFlagIn);

    virtual ErrorCodes_t setResistanceCorrectionLag(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn);
    virtual ErrorCodes_t setResistancePredictionPercentage(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn);
    virtual ErrorCodes_t setResistancePredictionBandwidthGain(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn);
    virtual ErrorCodes_t setResistancePredictionTau(std::vector<uint16_t> channelIndexes, std::vector<double> channelValues, bool applyFlagIn);
    
    ErrorCodes_t setVoltageProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t vRest);
    ErrorCodes_t setVoltageProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag);
    ErrorCodes_t setVoltageProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vFinal, Measurement_t vFinalStep, Measurement_t t0, Measurement_t t0Step, bool vHalfFlag);
    ErrorCodes_t setVoltageProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t v0, Measurement_t v0Step, Measurement_t vAmp, Measurement_t vAmpStep, Measurement_t f0, Measurement_t f0Step, bool vHalfFlag);

    ErrorCodes_t setCurrentProtocolStructure(uint16_t protId, uint16_t itemsNum, uint16_t sweepsNum, Measurement_t iRest);
    ErrorCodes_t setCurrentProtocolStep(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag);
    ErrorCodes_t setCurrentProtocolRamp(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iFinal, Measurement_t iFinalStep, Measurement_t t0, Measurement_t t0Step, bool cHalfFlag);
    ErrorCodes_t setCurrentProtocolSin(uint16_t itemIdx, uint16_t nextItemIdx, uint16_t loopReps, bool applyStepsFlag, Measurement_t i0, Measurement_t i0Step, Measurement_t iAmp, Measurement_t iAmpStep, Measurement_t f0, Measurement_t f0Step, bool cHalfFlag);

    ErrorCodes_t startProtocol();

    ErrorCodes_t setStateArrayStructure(int numberOfStates, int initialState);
    ErrorCodes_t setSateArrayState(int stateIdx, Measurement_t voltage, bool timeoutStateFlag, double timeout, int timeoutState, Measurement_t minTriggerValue, Measurement_t maxTriggerValue, int triggerState, bool triggerFlag, bool deltaFlag);
    ErrorCodes_t startStateArray();
    ErrorCodes_t setStateArrayEnabled(int chIdx, bool enabledFlag);
    bool isStateArrayAvailable();

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
    ErrorCodes_t getNextMessage(RxOutput_t &rxOutput, int16_t * data);
    ErrorCodes_t purgeData();
    ErrorCodes_t convertVoltageValue(int16_t intValue, double &fltValue);
    ErrorCodes_t convertCurrentValue(int16_t intValue, double &fltValue);
    ErrorCodes_t convertVoltageValues(int16_t * intValue, double * fltValue, int valuesNum);
    ErrorCodes_t convertCurrentValues(int16_t * intValue, double * fltValue, int valuesNum);

    ErrorCodes_t getLiquidJunctionVoltages(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> &voltages);

    ErrorCodes_t getVoltageHoldTunerFeatures(std::vector <RangedMeasurement_t> &voltageHoldTunerFeatures);
    ErrorCodes_t getVoltageHalfFeatures(std::vector <RangedMeasurement_t> &voltageHalfTunerFeatures);
    ErrorCodes_t getCurrentHoldTunerFeatures(std::vector <RangedMeasurement_t> &currentHoldTunerFeatures);
    ErrorCodes_t getCurrentHalfFeatures(std::vector <RangedMeasurement_t> &currentHalfTunerFeatures);
    ErrorCodes_t getLiquidJunctionRangesFeatures(std::vector <RangedMeasurement_t> &ranges);
    ErrorCodes_t getCalibVcCurrentGainFeatures(RangedMeasurement_t &calibVcCurrentGainFeatures);
    ErrorCodes_t getCalibVcCurrentOffsetFeatures(std::vector<RangedMeasurement_t> &calibVcCurrentOffsetFeatures);
    ErrorCodes_t getCalibCcVoltageGainFeatures(RangedMeasurement_t &calibCcVoltageGainFeatures);
    ErrorCodes_t getCalibCcVoltageOffsetFeatures(std::vector<RangedMeasurement_t> &calibCcVoltageOffsetFeatures);
    ErrorCodes_t hasGateVoltages();
    ErrorCodes_t hasSourceVoltages();
    ErrorCodes_t getGateVoltagesFeatures(RangedMeasurement_t &gateVoltagesFeatures);
    ErrorCodes_t getSourceVoltagesFeatures(RangedMeasurement_t &sourceVoltagesFeatures);
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

    ErrorCodes_t hasChannelSwitches();
    ErrorCodes_t hasStimulusSwitches();
    ErrorCodes_t hasOffsetCompensation();

    ErrorCodes_t getCalibParams(CalibrationParams_t &calibParams);
    ErrorCodes_t getCalibFileNames(std::vector<std::string> &calibFileNames);
    ErrorCodes_t getCalibFilesFlags(std::vector<std::vector <bool>> &calibFilesFlags);
    ErrorCodes_t getCalibMappingFileDir(std::string &dir);
    ErrorCodes_t getCalibMappingFilePath(std::string &path);

    ErrorCodes_t getVoltageProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range);
    ErrorCodes_t getCurrentProtocolRangeFeature(uint16_t rangeIdx, RangedMeasurement_t &range);
    ErrorCodes_t getTimeProtocolRangeFeature(RangedMeasurement_t &range);
    ErrorCodes_t getFrequencyProtocolRangeFeature(RangedMeasurement_t &range);

    ErrorCodes_t getMaxProtocolItemsFeature(uint32_t &num);
    ErrorCodes_t hasProtocols();
    ErrorCodes_t hasProtocolStepFeature();
    ErrorCodes_t hasProtocolRampFeature();
    ErrorCodes_t hasProtocolSinFeature();

    ErrorCodes_t getCalibData(CalibrationData_t &calibData);

    virtual ErrorCodes_t hasCompFeature(uint16_t feature);
    virtual ErrorCodes_t getCompFeatures(uint16_t paramToExtractFeatures, std::vector<RangedMeasurement_t> &compensationFeatures, double &defaultParamValue);
    virtual ErrorCodes_t getCompOptionsFeatures(CompensationTypes type ,std::vector <std::string> &compOptionsArray);
    virtual ErrorCodes_t getCompValueMatrix(std::vector<std::vector<double>> &compValueMatrix);
    virtual ErrorCodes_t getCompensationEnables(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> &onValues);

    virtual ErrorCodes_t getPipetteCompensationOptions(std::vector <std::string> options);
    virtual ErrorCodes_t getCCPipetteCompensationOptions(std::vector <std::string> options);
    virtual ErrorCodes_t getMembraneCompensationOptions(std::vector <std::string> options);
    virtual ErrorCodes_t getResistanceCompensationOptions(std::vector <std::string> options);
    virtual ErrorCodes_t getResistancePredictionOptions(std::vector <std::string> options);
    virtual ErrorCodes_t getLeakConductanceCompensationOptions(std::vector <std::string> options);
    virtual ErrorCodes_t getBridgeBalanceCompensationOptions(std::vector <std::string> options);

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

    /*! 20230531: MPAC here somehow useless*/
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

    ErrorCodes_t init();
    ErrorCodes_t deinit();

    static std::string getDeviceSerial(int index);
    static bool getDeviceCount(int &numDevs);
    virtual void handleCommunicationWithDevice() = 0;
    virtual uint32_t readDataFromDevice() = 0;
    virtual void parseDataFromDevice() = 0;
    void computeLiquidJunction();
    virtual void sendCommandsToDevice() = 0;
    virtual void initializeHW() = 0;

    void forceOutMessage();
    bool checkProtocolValidity(std::string &message);

    void storeFrameData(uint16_t rxMsgTypeId, RxMessageTypes_t rxMessageType);

    void stackOutgoingMessage(std::vector <uint16_t> &txDataMessage, TxTriggerType_t triggerType = TxTriggerParameteresUpdated);
    uint16_t popUint16FromRxRawBuffer();
    uint16_t readUint16FromRxRawBuffer(uint32_t n);

    void initializeRawDataFilterVariables();
    void computeRawDataFilterCoefficients();
    double applyRawDataFilter(uint16_t channelIdx, double x, double * iirNum, double * iirDen);

    void updateVoltageHoldTuner(bool applyFlag);
    void updateCurrentHoldTuner(bool applyFlag);

    virtual std::vector<double> user2AsicDomainTransform(int chIdx, std::vector<double> userDomainParams);
    virtual std::vector<double> asic2UserDomainTransform(int chIdx, std::vector<double> asicDomainParams, double oldUCpVc, double oldUCpCc);
    virtual ErrorCodes_t asic2UserDomainCompensable(int chIdx, std::vector<double> asicDomainParams, std::vector<double> userDomainParams);
    virtual double computeAsicCmCinj(double cm, bool chanCslowEnable, MultiCoder::MultiCoderConfig_t multiconfigCslow);

    void fillBoardList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard);
    void fillChannelList(uint16_t numOfBoards, uint16_t numOfChannelsOnBoard);

    void flushBoardList();

    /************\
     *  Fields  *
    \************/

    uint16_t rxSyncWord;

    unsigned int packetsPerFrame = 1;

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

    /*! Protocol's parameters */
    unsigned int protocolMaxItemsNum;
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

    std::vector <CommandCoder*> coders;

    BoolCoder * asicResetCoder = nullptr;
    BoolCoder * fpgaResetCoder = nullptr;

    uint32_t clampingModalitiesNum;
    uint32_t selectedClampingModalityIdx = 0;
    uint32_t selectedClampingModality = VOLTAGE_CLAMP;
    std::vector <ClampingModality_t> clampingModalitiesArray;
    uint16_t defaultClampingModalityIdx;
    BoolCoder * clampingModeCoder = nullptr;

    uint32_t vcCurrentRangesNum;
    uint32_t selectedVcCurrentRangeIdx = 0;
    std::vector <RangedMeasurement_t> vcCurrentRangesArray;
    uint16_t defaultVcCurrentRangeIdx;
    BoolCoder * vcCurrentRangeCoder = nullptr;

    uint32_t vcVoltageRangesNum;
    uint32_t selectedVcVoltageRangeIdx = 0;
    std::vector <RangedMeasurement_t> vcVoltageRangesArray;
    uint16_t defaultVcVoltageRangeIdx;
    BoolCoder * vcVoltageRangeCoder = nullptr;

    uint32_t liquidJunctionRangesNum;
    uint32_t selectedLiquidJunctionRangeIdx = 0;
    std::vector <RangedMeasurement_t> liquidJunctionRangesArray;
    uint16_t defaultLiquidJunctionRangeIdx;

    uint32_t ccCurrentRangesNum;
    uint32_t selectedCcCurrentRangeIdx = 0;
    std::vector <RangedMeasurement_t> ccCurrentRangesArray;
    uint16_t defaultCcCurrentRangeIdx;
    BoolCoder * ccCurrentRangeCoder = nullptr;

    uint32_t ccVoltageRangesNum;
    uint32_t selectedCcVoltageRangeIdx = 0;
    std::vector <RangedMeasurement_t> ccVoltageRangesArray;
    uint16_t defaultCcVoltageRangeIdx;
    BoolCoder * ccVoltageRangeCoder = nullptr;

    uint32_t vcCurrentFiltersNum;
    uint32_t selectedVcCurrentFilterIdx = 0;
    std::vector <Measurement_t> vcCurrentFiltersArray;
    uint16_t defaultVcCurrentFilterIdx;
    BoolCoder * vcCurrentFilterCoder = nullptr;

    uint32_t vcVoltageFiltersNum;
    uint32_t selectedVcVoltageFilterIdx = 0;
    std::vector <Measurement_t> vcVoltageFiltersArray;
    uint16_t defaultVcVoltageFilterIdx;
    BoolCoder * vcVoltageFilterCoder = nullptr;

    uint32_t ccCurrentFiltersNum;
    uint32_t selectedCcCurrentFilterIdx = 0;
    std::vector <Measurement_t> ccCurrentFiltersArray;
    uint16_t defaultCcCurrentFilterIdx;
    BoolCoder * ccCurrentFilterCoder = nullptr;

    uint32_t ccVoltageFiltersNum;
    uint32_t selectedCcVoltageFilterIdx = 0;
    std::vector <Measurement_t> ccVoltageFiltersArray;
    uint16_t defaultCcVoltageFilterIdx;
    BoolCoder * ccVoltageFilterCoder = nullptr;

    std::vector <BoolCoder *> enableStimulusCoders;
    std::vector <BoolCoder *> turnChannelsOnCoders;

    uint32_t samplingRatesNum;
    std::vector <Measurement_t> samplingRatesArray;
    std::vector <Measurement_t> realSamplingRatesArray;
    std::vector <Measurement_t> integrationStepArray;
    unsigned int defaultSamplingRateIdx = 0;
    BoolCoder * samplingRateCoder = nullptr;
    std::unordered_map<uint16_t, uint16_t> sr2LpfVcCurrentMap;
    std::unordered_map<uint16_t, uint16_t> sr2LpfCcVoltageMap;
    std::unordered_map<uint16_t, uint16_t> vcCurrRange2CalibResMap;

    std::vector<Measurement_t> selectedVoltageHoldVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<Measurement_t> selectedCurrentHoldVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<Measurement_t> selectedVoltageHalfVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<Measurement_t> selectedCurrentHalfVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector <std::vector <DoubleCoder *>> vHoldTunerCoders;
    std::vector <std::vector <DoubleCoder *>> vHalfTunerCoders;
    std::vector <std::vector <DoubleCoder *>> cHoldTunerCoders;
    std::vector <std::vector <DoubleCoder *>> cHalfTunerCoders;

    std::vector <BoolCoder *> digitalOffsetCompensationCoders;
    std::vector<Measurement_t> selectedLiquidJunctionVector; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector <std::vector <DoubleCoder *>> liquidJunctionVoltageCoders;

    // Calibration DAC coders and ranges
    RangedMeasurement_t calibCcCurrentGainRange;
    std::vector <DoubleCoder *> calibCcCurrentGainCoders;

    std::vector <RangedMeasurement_t> calibCcCurrentOffsetRanges;
    std::vector <std::vector <DoubleCoder *>> calibCcCurrentOffsetCoders;

    RangedMeasurement_t calibVcVoltageGainRange;
    std::vector <DoubleCoder *> calibVcVoltageGainCoders;

    std::vector <RangedMeasurement_t> calibVcVoltageOffsetRanges;
    std::vector <std::vector <DoubleCoder *>> calibVcVoltageOffsetCoders;

    // Calibration ADC coders and ranges
    RangedMeasurement_t calibVcCurrentGainRange;
    std::vector <DoubleCoder *> calibVcCurrentGainCoders;

    std::vector <RangedMeasurement_t> calibVcCurrentOffsetRanges;
    std::vector <std::vector <DoubleCoder *>> calibVcCurrentOffsetCoders;

    RangedMeasurement_t calibCcVoltageGainRange;
    std::vector <DoubleCoder *> calibCcVoltageGainCoders;

    std::vector <RangedMeasurement_t> calibCcVoltageOffsetRanges;
    std::vector <std::vector <DoubleCoder *>> calibCcVoltageOffsetCoders;

    RangedMeasurement_t gateVoltageRange;
    std::vector<Measurement_t> selectedGateVoltageVector;
    std::vector <DoubleCoder *> gateVoltageCoders;

    RangedMeasurement_t sourceVoltageRange;
    std::vector<Measurement_t> selectedSourceVoltageVector;
    std::vector <DoubleCoder *> sourceVoltageCoders;

    /*! \todo new coders and ranges, to be moved in proper places within this file*/
    BoolArrayCoder* voltageChanSourceCoder;
    BoolArrayCoder* currentChanSourceCoder;

    std::vector <BoolCoder *> calSwCoders;
    std::vector <BoolCoder *> vcSwCoders;
    std::vector <BoolCoder *> ccSwCoders;
    std::vector <BoolCoder *> vcCcSelCoders;
    std::vector <BoolCoder *> ccStimEnCoders;
    BoolArrayCoder* sourceForVoltageChannelCoder = nullptr;
    uint16_t selectedSourceForVoltageChannelIdx;
    BoolArrayCoder* sourceForCurrentChannelCoder = nullptr;
    uint16_t selectedSourceForCurrentChannelIdx;

    std::vector <RangedMeasurement_t> vcLeakCalibRange;
    std::vector <std::vector <DoubleCoder *>> vcLeakCalibCoders;

    BoolCoder * numberOfStatesCoder = nullptr;
    BoolCoder * initialStateCoder = nullptr;
    std::vector<BoolCoder*> enableStateArrayChannelsCoder;

    std::vector<std::vector<DoubleCoder*>> appliedVoltageCoders;

    std::vector<BoolArrayCoder*> stateTimeoutFlagCoders;
    std::vector<BoolArrayCoder*> stateTriggerFlagCoders;
    std::vector<BoolArrayCoder*> stateTriggerDeltaFlagCoders;
    std::vector<DoubleCoder*> stateTimeoutValueCoders;
    std::vector<BoolCoder*> stateTimeoutNextStateCoders;
    std::vector<std::vector<DoubleCoder*>> stateMinTriggerCurrentCoders;
    std::vector<std::vector<DoubleCoder*>> stateMaxTriggerCurrentCoders;
    std::vector<BoolCoder*> stateTriggerNextStateCoders;

    CalibrationData_t calibrationData;

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
    std::vector<bool> compRsCorrEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compRsPredEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    std::vector<bool> compCcCfastEnable; /*! \todo FCON sostituibile con le info reperibili dai channel model? */
    bool areVcCompsEnabled = false;
    bool areCcCompsEnabled = false;

    ErrorCodes_t calibrationLoadingError = ErrorCalibrationNotLoadedYet;
    CalibrationParams_t calibrationParams;
    std::vector<std::string> calibrationFileNames;
    std::vector<std::vector<bool>> calibrationFilesOkFlags;
    std::string calibrationMappingFileDir;
    std::string calibrationMappingFilePath;

    std::string deviceId;
    std::string deviceName;

    bool connected = false;
    bool threadsStarted = false;
    bool stopConnectionFlag = false;
    bool parsingFlag = false;

    std::vector <BoardModel *> boardModels;
    std::vector <ChannelModel *> channelModels;

    /*! Read data buffer management */
    uint16_t rxMaxWords;
    uint32_t maxInputDataLoadSize;

    uint16_t selectedSamplingRateIdx;

    /*! Read data buffer management */
    uint8_t * rxRawBuffer = nullptr; /*!< Raw incoming data from the device */
    uint16_t * rxRawBuffer16 = nullptr; /*!< Raw incoming data from the device */
    uint32_t rxRawBufferReadOffset = 0; /*!< Device Rx buffer offset position in which data are collected by the outputDataBuffer */
    uint32_t rxRawBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint32_t maxRxRawBytesRead = 0;
    uint32_t rxRawBytesAvailable = 0;
    uint32_t rxRawBufferWriteOffset = 0; /*!< Device Rx buffer offset position in which data are written by FTDI device */
    uint32_t rxRawBufferMask;
    MsgResume_t * rxMsgBuffer; /*!< Buffer of pre-digested messages that contains message's high level info */
    uint32_t rxMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t rxMsgBufferReadLength = 0; /*!< Lenght of the part of the buffer to be processed */
    uint32_t rxMsgBufferWriteOffset = 0;
    uint32_t rxDataBufferWriteOffset = 0;
    std::vector <uint16_t> voltageDataValues; /*! Store voltage data when current data and voltage data are not sent together in a single packet */

    uint32_t lastParsedMsgType = MsgTypeIdInvalid; /*!< Type of the last parsed message to check for repetitions  */

    uint16_t * rxDataBuffer; /*!< Buffer of pre-digested messages that contains message's data */

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
    unsigned int maxOutputPacketsNum;
    std::vector <uint16_t> txStatus; /*!< Status of the words written */
    uint16_t txModifiedStartingWord;
    uint16_t txModifiedEndingWord;

    double currentResolution = 1.0;
    double voltageResolution = 1.0;
    double liquidJunctionResolution = 1.0;
    bool liquidJunctionSameRangeAsVcDac = true;

    RangedMeasurement_t voltageRange;
    RangedMeasurement_t currentRange;
    RangedMeasurement_t liquidJunctionRange;
    std::vector <RangedMeasurement_t> vHoldRange;
    std::vector <RangedMeasurement_t> vHalfRange;
    std::vector <RangedMeasurement_t> cHoldRange;
    std::vector <RangedMeasurement_t> cHalfRange;

    Measurement_t samplingRate = {200.0, UnitPfxKilo, "Hz"};
    Measurement_t integrationStep = {5.0, UnitPfxMicro, "s"};

    std::vector <uint16_t> allChannelIndexes;

    /*! Protocols variables */
    uint16_t selectedProtocolItemsNum = 0;

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

    double ** iirX;
    double ** iirY;

    uint16_t iirOff = 0;

    /********************************************\
     *  Multi-thread synchronization variables  *
    \********************************************/

    std::thread deviceCommunicationThread;
    std::thread rxConsumerThread;
    std::thread liquidJunctionThread;

    mutable std::mutex rxRawMutex;
    std::condition_variable rxRawBufferNotEmpty;
    std::condition_variable rxRawBufferNotFull;

    mutable std::mutex rxMsgMutex;
    std::condition_variable rxMsgBufferNotEmpty;
    std::condition_variable rxMsgBufferNotFull;

    mutable std::mutex txMutex;
    std::condition_variable txMsgBufferNotEmpty;
    std::condition_variable txMsgBufferNotFull;

    mutable std::mutex ljMutex;

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
