#ifndef EMCR4X10MHZFAKE_H
#define EMCR4X10MHZFAKE_H

#include "emcr4x10mhz.h"

class Emcr4x10MHzFake : public Emcr4x10MHz_PCBV01_V02 {
public:
    Emcr4x10MHzFake(std::string id);
    virtual ~Emcr4x10MHzFake();

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

#endif // EMCR4X10MHZFAKE_H
