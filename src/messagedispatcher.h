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

    /*************\
     *  Methods  *
    \*************/

    /****************\
     *  Parameters  *
    \****************/

    uint16_t voltageChannelsNum = 1;
    uint16_t currentChannelsNum = 1;
    uint16_t totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    BoolCoder * asicResetCoder = nullptr;

    uint32_t vcCurrentRangesNum;
    uint32_t selectedVcCurrentRangeIdx = 0;
    std::vector <RangedMeasurement_t> vcCurrentRangesArray;
    uint16_t defaultVcCurrentRangeIdx;
    BoolCoder * vcCurrentRangeCoder = nullptr;

    std::vector <BoolCoder *> digitalOffsetCompensationCoders;

    DoubleCoder * VRestCoder = nullptr;


    /***************\
     *  Variables  *
    \***************/

    std::string deviceId;

    bool connected = false;
    bool threadsStarted = false;
    bool stopConnectionFlag = false;

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
