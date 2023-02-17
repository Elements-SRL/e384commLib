#ifndef MESSAGEDISPATCHER_OPALKELLY_H
#define MESSAGEDISPATCHER_OPALKELLY_H

#include "okFrontPanelDLL.h"

#include "messagedispatcher.h"

class MessageDispatcher_OpalKelly : public MessageDispatcher {
public:
    MessageDispatcher_OpalKelly(std::string deviceId);
    virtual ~MessageDispatcher_OpalKelly();

    virtual ErrorCodes_t connect() override;
    virtual ErrorCodes_t disconnect() override;
    virtual void readDataFromDevice() override;
    virtual void sendCommandsToDevice() override;

protected:
    okCFrontPanel * dev;

    std::string fwName = "top.bit";
};

#endif // MESSAGEDISPATCHER_OPALKELLY_H
