#ifndef EMCR4X10MHZFAKE_H
#define EMCR4X10MHZFAKE_H

#include "emcr4x10mhz.h"

class Emcr4x10MHzFake : public Emcr4x10MHz_PCBV01_V03 {
public:
    Emcr4x10MHzFake(std::string id);

protected:

    enum SamplingRates {
        SamplingRate1MHz,
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
};

#endif // EMCR4X10MHZFAKE_H
