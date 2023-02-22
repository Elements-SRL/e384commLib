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

#define TX_WORD_SIZE (sizeof(uint16_t)) // 16 bit word

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
    virtual void readDataFromDevice() = 0;
    virtual void sendCommandsToDevice() = 0;
    static ErrorCodes_t getDeviceType(std::string deviceId, DeviceTypes_t &type);

protected:

    enum rxMessageTypes{rxMessageDataLoad, rxMessageDataHeader, rxMessageDataTail, rxMessageStatus, rxMessageVoltageOffset, rxMessageNum};

    /*************\
     *  Methods  *
    \*************/

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
    unsigned int defaultSamplingRateIdx = 0;
    std::vector <Measurement_t> realSamplingRatesArray;
    std::vector <Measurement_t> bandwidthsArray;
    std::vector <Measurement_t> integrationStepArray;
    BoolCoder * samplingRateCoder;

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

    int commonReadFrameLength;

    /*! Write data buffer management */
    uint8_t * txRawBuffer; /*!< Raw outgoing data to the device */
    std::vector <uint16_t> * txMsgBuffer; /*!< Buffer of arrays of bytes to communicate to the device */
    std::vector <uint16_t> txMsgOffsetWord; /*!< Buffer of offset word in txMsgBuffer */
    std::vector <uint16_t> txMsgLength; /*!< Buffer of txMsgBuffer length */
    uint32_t txMsgBufferWriteOffset = 0; /*!< Offset of the part of buffer to be written */
    uint32_t txMsgBufferReadLength = 0; /*!< Length of the part of the buffer to be processed */
    uint16_t txDataWords;
    uint16_t txMaxWords;
    uint16_t rxDataWordOffset; /*!< Offset of data words in rx frames */
    uint16_t rxDataWordLength; /*!< Number data words in rx frames */
    uint32_t rxBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed  */
    unsigned int maxOutputPacketsNum;
    std::vector <uint16_t> txStatus; /*!< Status of the bytes written */
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

    std::condition_variable rxMsgBufferNotEmpty;
    std::condition_variable rxMsgBufferNotFull;

    mutable std::mutex txMutex;
    std::condition_variable txMsgBufferNotEmpty;
    std::condition_variable txMsgBufferNotFull;
};

#endif // MESSAGEDISPATCHER_H
