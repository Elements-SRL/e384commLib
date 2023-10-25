#ifndef EMCRDEVICE_H
#define EMCRDEVICE_H

#include "messagedispatcher.h"

class EmcrDevice {
public:
    EmcrDevice(std::string deviceId);
    virtual ~EmcrDevice();

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = "");
    virtual ErrorCodes_t disconnectDevice();
    virtual ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag);

    virtual ErrorCodes_t connect(std::string fwPath);
    virtual ErrorCodes_t disconnect();

};

#endif // EMCRDEVICE_H
