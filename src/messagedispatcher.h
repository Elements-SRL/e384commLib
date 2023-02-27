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

#define SHORT_OFFSET_BINARY (static_cast <double> (0x8000))
#define SHORT_MAX (static_cast <double> (0x7FFF))
#define SHORT_MIN (-SHORT_MAX-1.0)
#define USHORT_MAX (static_cast <double> (0xFFFF))
#define UINT10_MAX (static_cast <double> (0x3FF))
#define INT14_MAX (static_cast <double> (0x1FFF))
#define UINT14_MAX (static_cast <double> (0x3FFF))
#define INT18_MAX (static_cast <double> (0x1FFFF))
#define UINT28_MAX (static_cast <double> (0xFFFFFFF))
#define INT28_MAX (static_cast <double> (0x7FFFFFF))
#define INT28_MIN (-INT28_MAX-1.0)

#define RX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define RX_DEFAULT_MIN_PACKETS_NUMBER 10
#define RX_DEFAULT_FEW_PACKETS_SLEEP_US 2000
#define RX_FEW_PACKETS_COEFF 0.01 /*!< = 10.0/1000.0: 10.0 because I want to get data once every 10ms, 1000 to convert sampling rate from Hz to kHz */
#define RX_MAX_BYTES_TO_WAIT_FOR 16384

#define TX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word
#define TX_MSG_BUFFER_SIZE 0x100 /*!< Number of messages. Always use a power of 2 for efficient circular buffer management through index masking */
#define TX_MSG_BUFFER_MASK (TX_MSG_BUFFER_SIZE-1)

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

    virtual ErrorCodes_t connect();
    virtual ErrorCodes_t disconnect();
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);

    /****************\
     *  Tx methods  *
    \****************/

    ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn);
    ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn);
    ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlagIn);
    ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlagIn);

    ErrorCodes_t setVoltageStimulusLpf(uint16_t filterIdx, bool applyFlagIn);
    ErrorCodes_t setCurrentStimulusLpf(uint16_t filterIdx, bool applyFlagIn);

    ErrorCodes_t digitalOffsetCompensation(std::vector<uint16_t> channelIndexes, std::vector<bool> onValues, bool applyFlag);

    ErrorCodes_t setAdcFilter();
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlagIn);

    ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlagIn);
    ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlagIn);

    /****************\
     *  Rx methods  *
    \****************/

    ErrorCodes_t getVCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges);
    ErrorCodes_t getVCVoltageRanges(std::vector <RangedMeasurement_t> &currentRanges);
    ErrorCodes_t getCCCurrentRanges(std::vector <RangedMeasurement_t> &currentRanges);
    ErrorCodes_t getCCVoltageRanges(std::vector <RangedMeasurement_t> &currentRanges);

    ErrorCodes_t getVoltageStimulusLpfs(std::vector <std::string> &filterOptions);
    ErrorCodes_t getCurrentStimulusLpfs(std::vector <std::string> &filterOptions);

protected:

    enum rxMessageTypes{rxMessageDataLoad, rxMessageDataHeader, rxMessageDataTail, rxMessageStatus, rxMessageVoltageOffset, rxMessageNum};

    /*************\
     *  Methods  *
    \*************/

    virtual void readDataFromDevice() = 0;
    virtual void sendCommandsToDevice() = 0;
    void stackOutgoingMessage(std::vector <uint16_t> &txDataMessage);

    /****************\
     *  Parameters  *
    \****************/

    uint16_t rxSyncWord;
    uint16_t txSyncWord;

    uint16_t txCrcInitialValue = 0xFFFF;

    int packetsPerFrame = 16;

    uint16_t voltageChannelsNum = 1;
    uint16_t currentChannelsNum = 1;
    uint16_t totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    std::vector<uint16_t> rxWordOffsets;
    std::vector<uint16_t> rxWordLengths;


    BoolCoder * asicResetCoder = nullptr;
    BoolCoder * fpgaResetCoder = nullptr;
    BoolCoder * docResetCoder = nullptr; //DOC = digital offset compensation

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

    uint32_t samplingRatesNum;
    std::vector <Measurement_t> samplingRatesArray;
    std::vector <Measurement_t> realSamplingRatesArray;
    std::vector <Measurement_t> integrationStepArray;
    unsigned int defaultSamplingRateIdx = 0;
    BoolCoder * samplingRateCoder = nullptr;
    std::unordered_map<uint16_t, uint16_t> sr2LpfVcMap;
    std::unordered_map<uint16_t, uint16_t> sr2LpfCcMap;



    bool voltageOffsetControlImplemented = false;
    RangedMeasurement_t voltageOffsetRange;

    bool currentOffsetControlImplemented = false;
    RangedMeasurement_t currentOffsetRange;

    std::vector <BoolCoder *> digitalOffsetCompensationCoders;

    DoubleCoder * stimRestCoder = nullptr;

    /***************\
     *  Variables  *
    \***************/

    std::string deviceId;
    std::string deviceName;

    bool connected = false;
    bool threadsStarted = false;
    bool stopConnectionFlag = false;

    uint32_t minPacketsNumber = RX_DEFAULT_MIN_PACKETS_NUMBER;
    uint32_t fewPacketsSleepUs = RX_DEFAULT_FEW_PACKETS_SLEEP_US;

    /*! Read data buffer management */
    uint16_t rxMaxWords;
    uint32_t maxInputFrameSize;

    bool amIinVoltageClamp = false;
    uint16_t selectedSamplingRateIdx;

    /*! Write data buffer management */
    std::vector <uint16_t> * txMsgBuffer; /*!< Buffer of arrays of bytes to communicate to the device */
    std::vector <uint16_t> txMsgOffsetWord; /*!< Buffer of offset word in txMsgBuffer */
    std::vector <uint16_t> txMsgLength; /*!< Buffer of txMsgBuffer length */
    uint32_t txMsgBufferWriteOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t txMsgBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint16_t txDataWords;
    uint16_t txMaxWords;
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

    Measurement_t samplingRate = {200.0, UnitPfxKilo, "Hz"};
    Measurement_t integrationStep = {5.0, UnitPfxMicro, "s"};

    std::vector <Measurement_t> selectedVoltageOffset;
    std::vector <Measurement_t> selectedCurrentOffset;

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
};

#endif // MESSAGEDISPATCHER_H
