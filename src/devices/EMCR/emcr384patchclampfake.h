#ifndef EMCR384PATCHCLAMPFAKE_H
#define EMCR384PATCHCLAMPFAKE_H

#include "emcr384patchclamp_V04.h"

class Emcr384FakePatchClamp: public Emcr384PatchClamp_V04 {
public:
    Emcr384FakePatchClamp(std::string id);
    virtual ~Emcr384FakePatchClamp();

    virtual ErrorCodes_t connect(std::string fwPath) override;
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
};

#endif // EMCR384PATCHCLAMPFAKE_H
