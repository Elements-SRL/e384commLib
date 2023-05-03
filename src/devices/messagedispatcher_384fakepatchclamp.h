#ifndef MESSAGEDISPATCHER_384FAKEPATCHCLAMP_H
#define MESSAGEDISPATCHER_384FAKEPATCHCLAMP_H

#include "messagedispatcher_384patchclamp.h"

class MessageDispatcher_384FakePatchClamp: public MessageDispatcher_384PatchClamp_V01
{
public:
    MessageDispatcher_384FakePatchClamp(string id);
    virtual ~MessageDispatcher_384FakePatchClamp();

    virtual ErrorCodes_t connect() override;
    virtual ErrorCodes_t disconnect() override;

protected:

    enum SamplingRates {
        SamplingRate100Hz,
        SamplingRatesNum
    };

    /*************\
     *  Methods  *
    \*************/

    virtual void sendCommandsToDevice() override;
    virtual void readDataFromDevice() override;
};

#endif // MESSAGEDISPATCHER_384FAKEPATCHCLAMP_H
