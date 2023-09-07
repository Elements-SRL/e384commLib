#ifndef MESSAGEDISPATCHER_2X10MHZFAKE_H
#define MESSAGEDISPATCHER_2X10MHZFAKE_H

#include "messagedispatcher_2x10mhz.h"

class MessageDispatcher_2x10MHzFake : public MessageDispatcher_2x10MHz_PCBV01_V02 {
public:
    MessageDispatcher_2x10MHzFake(std::string id);
    virtual ~MessageDispatcher_2x10MHzFake();

    virtual ErrorCodes_t connect() override;
    virtual ErrorCodes_t disconnect() override;

protected:

    enum SamplingRates {
        SamplingRate2MHz,
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

#endif // MESSAGEDISPATCHER_2X10MHZFAKE_H
