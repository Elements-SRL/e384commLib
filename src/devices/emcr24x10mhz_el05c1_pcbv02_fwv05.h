#ifndef EMCR24X1_MHZ_EL_5C1_PCBV_2_FWV_5_H
#define EMCR24X1_MHZ_EL_5C1_PCBV_2_FWV_5_H

#include "emcropalkellydevice.h"

class Emcr24x10MHz_EL05c1_PCBV02_FWV05 : public EmcrOpalKellyDevice {
public:
    Emcr24x10MHz_EL05c1_PCBV02_FWV05(std::string di);

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
        SamplingRatesNum
    };

    enum CustomDouble {
        CustomDacVcmAsic1,
        CustomDacRefAsic1,
        CustomDacVcmAsic2,
        CustomDacRefAsic2,
        CustomDacVcmAsic3,
        CustomDacRefAsic3,
        CustomDoublesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR24X1_MHZ_EL_5C1_PCBV_2_FWV_5_H
