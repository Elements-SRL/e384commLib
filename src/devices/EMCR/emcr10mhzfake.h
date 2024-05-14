#ifndef EMCR10MHZFAKE_H
#define EMCR10MHZFAKE_H

#include "emcr10mhz.h"

class Emcr10MHzFake : public Emcr10MHz_V01 {
public:
    Emcr10MHzFake(std::string id);

protected:

    enum SamplingRates {
        SamplingRate40MHz,
        SamplingRatesNum
    };

    /*************\
     *  Methods  *
    \*************/

    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual void initializeVariables() override;

    virtual ErrorCodes_t initializeHW() override;
    virtual ErrorCodes_t stopCommunication() override;

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;

private:
    ErrorCodes_t fillBuffer();

    uint16_t syntheticData = 0;
    double generatedByteRate = 160.0e6;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
    uint32_t totalBytesWritten = 0;
};

#endif // EMCR10MHZFAKE_H
