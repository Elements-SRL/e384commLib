#ifndef EMCR24x10MHZ_ONLY8CH_H
#define EMCR24x10MHZ_ONLY8CH_H

#include "emcropalkellydevice.h"

class Emcr24x10MHz_Only8Ch_PCBV01 : public EmcrOpalKellyDevice {
public:
    Emcr24x10MHz_Only8Ch_PCBV01(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange10nA,
        VCCurrentRange100nA,
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
        VCCurrentFilter10MHz,
        VCCurrentFilter1MHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter1_6kHz,
        VCVoltageFilter16kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate781_25kHz,
        SamplingRate1_5625MHz,
        SamplingRate3_125MHz,
        SamplingRate6_25MHz,
        SamplingRate12_5MHz,
        SamplingRate25MHz,
        SamplingRate10MHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR24x10MHZ_ONLY8CH_H
