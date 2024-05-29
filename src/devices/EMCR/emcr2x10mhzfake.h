#ifndef EMCR2X10MHZFAKE_H
#define EMCR2X10MHZFAKE_H

#include "emcr2x10mhz.h"

class Emcr2x10MHzFake : public Emcr2x10MHz_PCBV01_V02 {
public:
    Emcr2x10MHzFake(std::string id);

protected:

    enum SamplingRates {
        SamplingRate2MHz,
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

#endif // EMCR2X10MHZFAKE_H
