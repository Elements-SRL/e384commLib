#ifndef MESSAGEDISPATCHER_384FAKENANOPORES_H
#define MESSAGEDISPATCHER_384FAKENANOPORES_H

#include "messagedispatcher_384nanopores.h"

class MessageDispatcher_384FakeNanopores : public MessageDispatcher_384NanoPores_V01 {
public:
    MessageDispatcher_384FakeNanopores(std::string id);
    virtual ~MessageDispatcher_384FakeNanopores();

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
    double generatedSamplingRate = 100.0e6;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
    uint32_t totalBytesWritten = 0;
};

#endif // MESSAGEDISPATCHER_384FAKENANOPORES_H
