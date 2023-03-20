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
    enum ClampingModalities {
        VoltageClamp,
        CurrentClamp,
        ClampingModalitiesNum
    };

    enum VCCurrentRanges {
        VCCurrentRange10nA,
        VCCurrentRange40nALbw,
        VCCurrentRange40nAHbw,
        VCCurrentRange400nA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange512mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRange8nA,
        CCCurrentRangesNum
    };

    enum CCVoltageRanges {
        CCVoltageRange1000mV,
        CCVoltageRangesNum
    };

    enum VCCurrentFilters {
        VCCurrentFilter3kHz,
        VCCurrentFilter3_6kHz,
        VCCurrentFilter10kHz,
        VCCurrentFilter12kHz,
        VCCurrentFilter25kHz,
        VCCurrentFilter30kHz,
        VCCurrentFilter100kHz,
        VCCurrentFilter120kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter1kHz,
        VCVoltageFilter10kHz,
        VCVoltageFilter35kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFilter1kHz,
        CCCurrentFilter10kHz,
        CCCurrentFilter35kHz,
        CCCurrentFiltersNum
    };

    enum CCVoltageFilters {
        CCVoltageFilter3kHz,
        CCVoltageFilter3_6kHz,
        CCVoltageFilter10kHz,
        CCVoltageFilter12kHz,
        CCVoltageFilter25kHz,
        CCVoltageFilter30kHz,
        CCVoltageFilter100kHz,
        CCVoltageFilter120kHz,
        CCVoltageFiltersNum
    };

    enum SamplingRates {
        SamplingRate6_25kHz,
        SamplingRate12_5kHz,
        SamplingRate25kHz,
        SamplingRate50kHz,
        SamplingRate100kHz,
        SamplingRatesNum
    };

    enum Leds {
        LedBlue,
        LedsNum
    };
};

#endif // MESSAGEDISPATCHER_384PATCHCLAMP_H
