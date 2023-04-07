#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H

#define _USE_MATH_DEFINES

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <unordered_map>
#include <cmath>

#include "e384commlib_errorcodes.h"
#include "e384commlib_global.h"
#include "commandcoder.h"

#ifdef E384CL_LABVIEW_COMPATIBILITY
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
#define UINT28_MAX (static_cast <double> (0xFFFFFFF))
#define INT28_MAX (static_cast <double> (0x7FFFFFF))
#define INT28_MIN (-INT28_MAX-1.0)

#define IIR_ORD 2
#define IIR_2_SIN_PI_4 (2.0*sin(M_PI/4.0))
#define IIR_2_COS_PI_4 (2.0*cos(M_PI/4.0))
#define IIR_2_COS_PI_4_2 (IIR_2_COS_PI_4*IIR_2_COS_PI_4)

#define RX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define RX_DEFAULT_MIN_PACKETS_NUMBER 10
#define RX_DEFAULT_FEW_PACKETS_SLEEP_US 2000
#define RX_FEW_PACKETS_COEFF 0.01 /*!< = 10.0/1000.0: 10.0 because I want to get data once every 10ms, 1000 to convert sampling rate from Hz to kHz */
#define RX_MAX_BYTES_TO_WAIT_FOR 16384
#define RX_MSG_BUFFER_SIZE 0x10000 // ~65k
#define RX_MSG_BUFFER_MASK (RX_MSG_BUFFER_SIZE-1)
#define RX_DATA_BUFFER_SIZE 0x100000 // ~1M
#define RX_DATA_BUFFER_MASK (RX_DATA_BUFFER_SIZE-1)

#define TX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define TX_MSG_BUFFER_SIZE 0x100 /*!< Number of messages. Always use a power of 2 for efficient circular buffer management through index masking */
#define TX_MSG_BUFFER_MASK (TX_MSG_BUFFER_SIZE-1)
#define TX_MAX_WRITE_TRIES 10

#ifndef E384CL_LABVIEW_COMPATIBILITY
using namespace e384CommLib;
#endif

typedef struct MsgResume {
    uint16_t typeId;
    uint16_t dataLength;
    uint32_t startDataPtr;
} MsgResume_t;

class MessageDispatcher {
public:

    /*****************\
     *  Ctor / Dtor  *
    \*****************/

    MessageDispatcher(std::string deviceId);
    virtual ~MessageDispatcher();

    /************************\
     *  Connection methods  *
    \************************/

    ErrorCodes_t init();
    ErrorCodes_t deinit();
    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher);
    ErrorCodes_t disconnectDevice();
    ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag);

    virtual ErrorCodes_t connect();
    virtual ErrorCodes_t disconnect();

    /****************\
     *  Tx methods  *
    \****************/

    ErrorCodes_t initializeDevice();

    ErrorCodes_t resetAsic(bool resetFlag, bool applyFlagIn = true);
    ErrorCodes_t resetFpga(bool resetFlag, bool applyFlagIn = true);
    ErrorCodes_t setVoltageHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn);
    ErrorCodes_t setCurrentHoldTuner(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> voltages, bool applyFlagIn);
    ErrorCodes_t setCalibVcCurrentGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    ErrorCodes_t setCalibVcCurrentOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    ErrorCodes_t setCalibCcVoltageGain(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> gains, bool applyFlag);
    ErrorCodes_t setCalibCcVoltageOffset(std::vector<uint16_t> channelIndexes, std::vector<Measurement_t> offsets, bool applyFlag);
    ErrorCodes_t setGateVoltagesTuner(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> gateVoltages, bool applyFlag);
    ErrorCodes_t setSourceVoltagesTuner(std::vector<uint16_t> boardIndexes, std::vector<Measurement_t> sourceVoltages, bool applyFlag);

    ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn);
    ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn);
    ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn);
    ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn);

    ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlagIn);
    ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlagIn);

    ErrorCodes_t enableStimulus(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);
    ErrorCodes_t turnChannelsOn(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    ErrorCodes_t digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    ErrorCodes_t setAdcFilter();
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlagIn);

    ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status);
    ErrorCodes_t setDebugWord(uint16_t wordOffset, uint16_t wordValue);

    ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlagIn);
    ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlagIn);

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t allocateRxDataBuffer(int16_t * &data);
    ErrorCodes_t deallocateRxDataBuffer(int16_t * &data);
    ErrorCodes_t getNextMessage(RxOutput_t &rxOutput, int16_t * data);
    ErrorCodes_t convertVoltageValue(int16_t intValue, double &fltValue);
    ErrorCodes_t convertCurrentValue(int16_t intValue, double &fltValue);

    ErrorCodes_t getVoltageHoldTunerFeatures(RangedMeasurement_t &voltageHoldTunerFeatures);
    ErrorCodes_t getCalibVcCurrentGainFeatures(RangedMeasurement_t &calibVcCurrentGainFeatures);
    ErrorCodes_t getCalibVcCurrentOffsetFeatures(std::vector<RangedMeasurement_t> &calibVcCurrentOffsetFeatures);
    ErrorCodes_t getCalibCcVoltageGainFeatures(RangedMeasurement_t &calibCcVoltageGainFeatures);
    ErrorCodes_t getCalibCcVoltageOffsetFeatures(std::vector<RangedMeasurement_t> &calibCcVoltageOffsetFeatures);
    ErrorCodes_t getGateVoltagesTunerFeatures(RangedMeasurement_t &gateVoltagesTunerFeatures);
    ErrorCodes_t getSourceVoltagesTunerFeatures(RangedMeasurement_t &sourceVoltagesTunerFeatures);
    ErrorCodes_t getChannelNumberFeatures(uint16_t &voltageChannelNumberFeatures, uint16_t &currentChannelNumberFeatures);
    ErrorCodes_t getBoardsNumberFeatures(uint16_t &boardsNumberFeatures);
    ErrorCodes_t getClampingModalitiesFeatures(std::vector<uint16_t> &clampingModalitiesFeatures);

    ErrorCodes_t getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges);
    ErrorCodes_t getVCVoltageRanges(std::vector <RangedMeasurement_t> &currentRanges);
    ErrorCodes_t getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges);
    ErrorCodes_t getCCVoltageRanges(std::vector <RangedMeasurement_t> &currentRanges);

    ErrorCodes_t getSamplingRatesFeatures(std::vector <Measurement_t> &samplingRates);

    ErrorCodes_t getVoltageStimulusLpfs(std::vector <Measurement_t> &vcVoltageFilters);
    ErrorCodes_t getCurrentStimulusLpfs(std::vector <Measurement_t> &ccCurrentFilters);

    ErrorCodes_t getVcCalibVoltStepsFeatures(std::vector <Measurement_t> &vcCalibVoltSteps);
    ErrorCodes_t getVcCalibResFeatures(std::vector <Measurement_t> &vCCalibRes);

    ErrorCodes_t getCalibDefaultVcAdcGain(Measurement_t &defaultVcAdcGain);
    ErrorCodes_t getCalibDefaultVcAdcOffset(Measurement_t &defaultVcAdcOffset);
    ErrorCodes_t getCalibDefaultVcDacOffset(Measurement_t &defaultVcDacOffset);



protected:
    // Check Device->PC table in protocol
    typedef enum RxMessageTypes {
        RxMessageDataLoad,
        RxMessageDataHeader,
        RxMessageDataTail,
        RxMessageStatus,
        RxMessageVoltageOffset,
        RxMessageNum
    } RxMessageTypes_t;

    /*************\
     *  Methods  *
    \*************/

    static std::string getDeviceSerial(int index);
    static bool getDeviceCount(int &numDevs);
    virtual void readDataFromDevice() = 0;
    virtual void sendCommandsToDevice() = 0;

    bool checkProtocolValidity(std::string &message);

    void storeFrameData(uint16_t rxMsgTypeId, RxMessageTypes_t rxMessageType);

    void stackOutgoingMessage(std::vector <uint16_t> &txDataMessage);
    uint16_t popUint16FromRxRawBuffer();
    uint16_t readUint16FromRxRawBuffer(uint32_t n);

    void computeMinimumPacketNumber();
    void initializeRawDataFilterVariables();
    void computeRawDataFilterCoefficients();
    double applyRawDataFilter(uint16_t channelIdx, double x, double * iirNum, double * iirDen);

    /****************\
     *  Parameters  *
    \****************/

    uint16_t rxSyncWord;

    int packetsPerFrame = 1;

    uint16_t voltageChannelsNum = 1;
    uint16_t currentChannelsNum = 1;
    uint16_t totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    uint16_t totalBoardsNum = 1;

    std::vector<uint16_t> rxWordOffsets;
    std::vector<uint16_t> rxWordLengths;

    std::unordered_map <uint16_t, bool> rxEnabledTypesMap; /*! key is any message type ID, value tells if the message should be returned by the getNextMessage method */

    std::vector <CommandCoder*> coders;

    BoolCoder * asicResetCoder = nullptr;
    BoolCoder * fpgaResetCoder = nullptr;
    BoolCoder * docResetCoder = nullptr; //DOC = digital offset compensation

    uint32_t clampingModalitiesNum;
    uint32_t selectedClampingMdalityIdx = 0;
    std::vector <uint16_t> clampingModalitiesArray;
    uint16_t defaultClampingModalityIdx;
    BoolCoder * clampingModeCoder = nullptr;

    BoolCoder * docOverrideCoder = nullptr;


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

    std::vector <BoolCoder *> digitalOffsetCompensationCoders;

    std::vector<Measurement_t> selectedVoltageHoldVector;
    std::vector<Measurement_t> selectedCurrentHoldVector;
    std::vector <DoubleCoder *> vHoldTunerCoders;
    std::vector <DoubleCoder *> cHoldTunerCoders;

    RangedMeasurement_t calibVcCurrentGainRange;
    std::vector<Measurement_t> selectedCalibVcCurrentGainVector;
    std::vector <DoubleCoder *> calibVcCurrentGainCoders;
    Measurement_t defaultCalibVcCurrentGain;

    std::vector <RangedMeasurement_t> calibVcCurrentOffsetRanges;
    std::vector<Measurement_t> selectedCalibVcCurrentOffsetVector;
    std::vector <std::vector <DoubleCoder *>> calibVcCurrentOffsetCoders;
    Measurement_t defaultCalibVcCurrentOffset;

    Measurement_t defaultCalibVcDacOffset;

    RangedMeasurement_t calibCcVoltageGainRange;
    std::vector<Measurement_t> selectedCalibCcVoltageGainVector;
    std::vector <DoubleCoder *> calibCcVoltageGainCoders;

    std::vector <RangedMeasurement_t> calibCcVoltageOffsetRanges;
    std::vector<Measurement_t> selectedCalibCcVoltageOffsetVector;
    std::vector <std::vector <DoubleCoder *>> calibCcVoltageOffsetCoders;

    RangedMeasurement_t gateVoltageRange;
    std::vector<Measurement_t> selectedGateVoltageVector;
    std::vector <DoubleCoder *> gateVoltageCoders;

    RangedMeasurement_t sourceVoltageRange;
    std::vector<Measurement_t> selectedSourceVoltageVector;
    std::vector <DoubleCoder *> sourceVoltageCoders;

    std::vector<Measurement_t> vcCalibResArray;
    std::vector<Measurement_t> vcCalibVoltStepsArray;

    DoubleCoder * stimRestCoder = nullptr;

    BoolArrayCoder * bitDebugCoder = nullptr;
    BoolArrayCoder * wordDebugCoder = nullptr;

    /*!Compensations coders*/
    std::vector<BoolCoder*> pipetteCapEnCompensationCoders;
    std::vector<MultiCoder*> pipetteCapValCompensationMultiCoders;
    std::vector<BoolCoder*> membraneCapEnCompensationCoders;
    std::vector<MultiCoder*> membraneCapValCompensationMultiCoders;
    std::vector<MultiCoder*> membraneCapTauValCompensationMultiCoders; // includes Tau Values and Tau Ranges
    std::vector<BoolCoder*>  rsCorrEnCompensationCoders;
    std::vector<DoubleCoder*>  rsCorrValCompensationCoders;
    std::vector<BoolArrayCoder*>  rsCorrBwCompensationCoders;
    std::vector<BoolCoder*>  rsPredEnCompensationCoders;
    std::vector<DoubleCoder*> rsPredGainCompensationCoders;
    std::vector<DoubleCoder*> rsPredTauCompensationCoders;


    /***************\
     *  Variables  *
    \***************/

    std::string deviceId;
    std::string deviceName;

    bool connected = false;
    bool threadsStarted = false;
    bool stopConnectionFlag = false;
    bool parsingFlag = false;

    uint32_t minPacketsNumber = RX_DEFAULT_MIN_PACKETS_NUMBER;
    uint32_t fewPacketsSleepUs = RX_DEFAULT_FEW_PACKETS_SLEEP_US;

    /*! Read data buffer management */
    uint16_t rxMaxWords;
    uint32_t maxInputDataLoadSize;

    bool amIinVoltageClamp = true;
    uint16_t selectedSamplingRateIdx;

    /*! Read data buffer management */
    uint8_t * rxRawBuffer = nullptr; /*!< Raw incoming data from the device */
    uint32_t rxRawBufferReadOffset = 0; /*!< Device Rx buffer offset position in which data are collected by the outputDataBuffer */
    uint32_t rxRawBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint32_t rxRawBufferWriteOffset = 0; /*!< Device Rx buffer offset position in which data are written by FTDI device */
    uint32_t rxRawBufferMask;
    MsgResume_t * rxMsgBuffer; /*!< Buffer of pre-digested messages that contains message's high level info */
    uint32_t rxMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t rxMsgBufferReadLength = 0; /*!< Lenght of the part of the buffer to be processed */
    uint32_t rxMsgBufferWriteOffset = 0;
    uint32_t rxDataBufferWriteOffset = 0;

    uint32_t lastParsedMsgType = MsgTypeIdInvalid; /*!< Type of the last parsed message to check for repetitions  */

    uint16_t * rxDataBuffer; /*!< Buffer of pre-digested messages that contains message's data */

    /*! Write data buffer management */
    std::vector <uint16_t> * txMsgBuffer; /*!< Buffer of arrays of bytes to communicate to the device */
    std::vector <uint16_t> txMsgOffsetWord; /*!< Buffer of offset word in txMsgBuffer */
    std::vector <uint16_t> txMsgLength; /*!< Buffer of txMsgBuffer length */
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

    double voltageOffsetCorrected = 0.0; /*!< Value currently corrected in applied voltages by the device (expressed in the unit of the liquid junction control) */
    double voltageOffsetCorrection = 0.0; /*!< Value to be used to correct the measured voltage values (expressed in the unit of current voltage range) */

    RangedMeasurement_t voltageRange;
    RangedMeasurement_t currentRange;
    RangedMeasurement_t vHoldRange;
    RangedMeasurement_t cHoldRange;

    Measurement_t samplingRate = {200.0, UnitPfxKilo, "Hz"};
    Measurement_t integrationStep = {5.0, UnitPfxMicro, "s"};

    /***********************\
     *  Filters variables  *
    \***********************/

    Measurement_t rawDataFilterCutoffFrequency = {30.0, UnitPfxKilo, "Hz"};
    bool rawDataFilterLowPassFlag = true;
    bool rawDataFilterActiveFlag = false;
    bool rawDataFilterVoltageFlag = false;
    bool rawDataFilterCurrentFlag = false;

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

    std::thread rxThread;
    std::thread txThread;

    mutable std::mutex rxMutex;
    std::condition_variable rxMsgBufferNotEmpty;
    std::condition_variable rxMsgBufferNotFull;

    mutable std::mutex txMutex;
    std::condition_variable txMsgBufferNotEmpty;
    std::condition_variable txMsgBufferNotFull;

#ifdef DEBUG_PRINT
    FILE * fid;
#endif
};

#endif // MESSAGEDISPATCHER_H
