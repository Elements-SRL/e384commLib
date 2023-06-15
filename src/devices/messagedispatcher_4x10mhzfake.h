#ifndef MESSAGEDISPATCHER_4X10MHZFAKE_H
#define MESSAGEDISPATCHER_4X10MHZFAKE_H

#include "messagedispatcher_4x10mhz.h"

class MessageDispatcher_4x10MHzFake : public MessageDispatcher_4x10MHz_V01 {
public:
    MessageDispatcher_4x10MHzFake(std::string id);
    virtual ~MessageDispatcher_4x10MHzFake();

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
    uint16_t syntheticData = 0;
};

#endif // MESSAGEDISPATCHER_4X10MHZFAKE_H
