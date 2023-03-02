#ifndef MESSAGEDISPATCHER_384PATCHCLAMP_H
#define MESSAGEDISPATCHER_384PATCHCLAMP_H

/*! \todo FCON tutto da rifare, è una copia del nanopore */

#include "messagedispatcher_opalkelly.h"

using namespace std;

class MessageDispatcher_384PatchClamp_V01 : public MessageDispatcher_OpalKelly {
public:
    MessageDispatcher_384PatchClamp_V01(string di);
    virtual ~MessageDispatcher_384PatchClamp_V01();

protected:
    enum VCCurrentRanges {
        VCCurrentRange4uA,
        VCCurrentRange200nA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFilter100kHz,
        VCCurrentFilter20kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFiltersNum = 0
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate100Hz,
        SamplingRatesNum
    };

    enum Leds {
        LedBlue,
        LedsNum
    };
};

#endif // MESSAGEDISPATCHER_384PATCHCLAMP_H
