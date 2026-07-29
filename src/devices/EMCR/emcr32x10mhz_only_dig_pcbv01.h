#ifndef EMCR32X1_MHZ_ONLY_DIG_PCBV_1_H
#define EMCR32X1_MHZ_ONLY_DIG_PCBV_1_H

#include "emcropalkellydevice.h"

class Emcr32x10MHz_OnlyDig_PCBV01 : public EmcrOpalKellyDevice {
public:
    Emcr32x10MHz_OnlyDig_PCBV01(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
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
        VCCurrentFiltersNum = 0
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
        SamplingRate625kHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR32X1_MHZ_ONLY_DIG_PCBV_1_H
