#ifndef EMCR24X1_MHZ_EL_5C12_PCBV_1_H
#define EMCR24X1_MHZ_EL_5C12_PCBV_1_H

#include "emcropalkellydevice.h"

class Emcr24x10MHz_EL05c12_PCBV01 : public EmcrOpalKellyDevice {
public:
    Emcr24x10MHz_EL05c12_PCBV01(std::string di);

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
        SamplingRate781_25kHz,
        SamplingRate1_5625MHz,
        SamplingRate3_125MHz,
        SamplingRatesNum
    };

    enum CustomOptions {
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
        CustomOptionsNum
    };

    enum CustomDouble {
        CustomDacVcmAsic1,
        CustomDacZapAsic1,
        CustomDacRefAsic1,
        CustomDacVcmAsic2,
        CustomDacZapAsic2,
        CustomDacRefAsic2,
        CustomDacVcmAsic3,
        CustomDacZapAsic3,
        CustomDacRefAsic3,
        CustomDoublesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR24X1_MHZ_EL_5C12_PCBV_1_H
