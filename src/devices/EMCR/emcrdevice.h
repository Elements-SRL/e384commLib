#ifndef EMCRDEVICE_H
#define EMCRDEVICE_H

#include "messagedispatcher.h"

class EmcrDevice : public MessageDispatcher {
public:
    EmcrDevice(std::string deviceId);
    virtual ~EmcrDevice();

    static ErrorCodes_t detectDevices(std::vector <std::string> &deviceIds);
    static ErrorCodes_t isDeviceSerialDetected(std::string deviceId);
    static ErrorCodes_t connectDevice(std::string deviceId, MessageDispatcher * &messageDispatcher, std::string fwPath = "");
    ErrorCodes_t disconnectDevice();
    ErrorCodes_t enableRxMessageType(MsgTypeId_t messageType, bool flag);

    virtual ErrorCodes_t connect(std::string fwPath);
    virtual ErrorCodes_t disconnect();

protected:

    /*************\
     *  Methods  *
    \*************/

    /************\
     *  Fields  *
    \************/

    std::vector<uint16_t> rxWordOffsets;
    std::vector<uint16_t> rxWordLengths;

    std::vector <bool> rxEnabledTypesMap; /*! key is any message type ID, value tells if the message should be returned by the getNextMessage method */

};

#endif // EMCRDEVICE_H
