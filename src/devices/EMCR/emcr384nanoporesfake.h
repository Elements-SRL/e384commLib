#ifndef EMCR384NANOPORESFAKE_H
#define EMCR384NANOPORESFAKE_H

#include "emcr384nanopores.h"

class Emcr384FakeNanopores : public Emcr384NanoPores_V01 {
public:
    Emcr384FakeNanopores(std::string id);
    virtual ~Emcr384FakeNanopores();

protected:
    enum SamplingRates {
        SamplingRate6kHz,
        SamplingRatesNum
    };

    /*************\
     *  Methods  *
    \*************/

    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual void initializeVariables() override;

    virtual ErrorCodes_t stopCommunication() override;

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;

private:
    ErrorCodes_t fillBuffer();

    uint16_t syntheticData = 0;
    double generatedByteRate = 1000.0e6;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
    uint32_t totalBytesWritten = 0;
};

#endif // EMCR384NANOPORESFAKE_H
