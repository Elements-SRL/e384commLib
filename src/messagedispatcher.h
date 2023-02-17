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

#ifdef E384CL_LABVIEW_COMPATIBILITY
#include "e384commlib_global_addendum.h"
#endif

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
