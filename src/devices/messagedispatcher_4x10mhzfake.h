#ifndef MESSAGEDISPATCHER_4X10MHZFAKE_H
#define MESSAGEDISPATCHER_4X10MHZFAKE_H

#include "messagedispatcher_4x10mhz.h"

class MessageDispatcher_4x10MHzFake : public MessageDispatcher_4x10MHz_PCBV01_V02 {
public:
    MessageDispatcher_4x10MHzFake(std::string id);
    virtual ~MessageDispatcher_4x10MHzFake();

    virtual ErrorCodes_t connect(std::string fwPath) override;
    virtual ErrorCodes_t disconnect() override;

protected:

    enum SamplingRates {
        SamplingRate1MHz,
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

#endif // MESSAGEDISPATCHER_4X10MHZFAKE_H
