#ifndef MESSAGEDISPATCHER_384FAKEPATCHCLAMP_H
#define MESSAGEDISPATCHER_384FAKEPATCHCLAMP_H

#include "messagedispatcher_384patchclamp_V04.h"

class MessageDispatcher_384FakePatchClamp: public MessageDispatcher_384PatchClamp_V04 {
public:
    MessageDispatcher_384FakePatchClamp(std::string id);
    virtual ~MessageDispatcher_384FakePatchClamp();

    virtual ErrorCodes_t connect() override;
    virtual ErrorCodes_t disconnect() override;

protected:

    enum SamplingRates {
        SamplingRate6kHz,
        SamplingRatesNum
    };

    /*************\
     *  Methods  *
    \*************/

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;

private:
    ErrorCodes_t fillBuffer();

    uint16_t syntheticData = 0;
};

#endif // MESSAGEDISPATCHER_384FAKEPATCHCLAMP_H
