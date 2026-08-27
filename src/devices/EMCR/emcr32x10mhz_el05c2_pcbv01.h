#ifndef EMCR32X1_MHZ_EL_5C2_PCBV_1_H
#define EMCR32X1_MHZ_EL_5C2_PCBV_1_H

#include "emcropalkellydevice.h"

class Emcr32x10MHz_EL05c2_PCBV01 : public EmcrOpalKellyDevice {
public:
    Emcr32x10MHz_EL05c2_PCBV01(std::string di);

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
        SamplingRate625kHz,
        SamplingRate1_25MHz,
        SamplingRate2_5MHz,
        SamplingRate5MHz,
        SamplingRate10MHz,
        SamplingRate20MHz,
        SamplingRatesNum
    };

    enum CustomFlags {
        ZapCh1,
        ZapCh2,
        ZapCh3,
        ZapCh4,
        ZapCh5,
        ZapCh6,
        ZapCh7,
        ZapCh8,
        ZapCh9,
        ZapCh10,
        ZapCh11,
        ZapCh12,
        ZapCh13,
        ZapCh14,
        ZapCh15,
        ZapCh16,
        ZapCh17,
        ZapCh18,
        ZapCh19,
        ZapCh20,
        ZapCh21,
        ZapCh22,
        ZapCh23,
        ZapCh24,
        ZapCh25,
        ZapCh26,
        ZapCh27,
        ZapCh28,
        ZapCh29,
        ZapCh30,
        ZapCh31,
        ZapCh32,
        CustomFlagsNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR32X1_MHZ_EL_5C2_PCBV_1_H
