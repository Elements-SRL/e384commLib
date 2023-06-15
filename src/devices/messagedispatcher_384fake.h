#ifndef MESSAGEDISPATCHER_384FAKE_H
#define MESSAGEDISPATCHER_384FAKE_H

#include "messagedispatcher_384nanopores.h"

class MessageDispatcher_384Fake : public MessageDispatcher_384NanoPores_V01 {
public:
    MessageDispatcher_384Fake(std::string id);
    virtual ~MessageDispatcher_384Fake();

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

    std::chrono::steady_clock::time_point startPrintfTime;
    std::chrono::steady_clock::time_point currentPrintfTime;
    uint32_t totalBytesWritten = 0;
};

#endif // MESSAGEDISPATCHER_384FAKE_H
