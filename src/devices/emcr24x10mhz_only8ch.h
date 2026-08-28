#ifndef EMCR24x10MHZ_ONLY8CH_H
#define EMCR24x10MHZ_ONLY8CH_H

#include "emcropalkellydevice.h"

class Emcr24x10MHz_Only8Ch_EL05c4_PCBV01 : public EmcrOpalKellyDevice {
public:
    Emcr24x10MHz_Only8Ch_EL05c4_PCBV01(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange10nA_ch0_3,
        VCCurrentRange100nA_ch0_3,
        VCCurrentRange10nA_ch4_7,
        VCCurrentRange100nA_ch4_7,
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

class Emcr24x10MHz_Only8Ch_EL05c3_PCBV01 : public Emcr24x10MHz_Only8Ch_EL05c4_PCBV01 {
public:
    Emcr24x10MHz_Only8Ch_EL05c3_PCBV01(std::string di);

protected:
    enum VCCurrentRanges {
        VCCurrentRange100nA_ch0_3,
        VCCurrentRange100nA_ch4_7,
        VCCurrentRangesNum
    };
};

class Emcr24x10MHz_Only8Ch_EL05c2_PCBV01 : public Emcr24x10MHz_Only8Ch_EL05c3_PCBV01 {
public:
    Emcr24x10MHz_Only8Ch_EL05c2_PCBV01(std::string di);

protected:
    enum CustomDouble {
        CustomDacVcmAsic1,
        CustomDacRefAsic1,
        CustomDacVcmAsic2,
        CustomDacRefAsic2,
        CustomDacVcmAsic3,
        CustomDacRefAsic3,
        CustomDoublesNum
    };
};

class Emcr24x10MHz_Only8Ch_EL05c1_PCBV01 : public Emcr24x10MHz_Only8Ch_EL05c2_PCBV01 {
public:
    Emcr24x10MHz_Only8Ch_EL05c1_PCBV01(std::string di);

protected:
    enum VCCurrentFilters {
        VCCurrentFilter10MHz,
        VCCurrentFiltersNum
    };

    enum CustomOptions {
        CustomOptionsNum = 0
    };
};

#endif // EMCR24x10MHZ_ONLY8CH_H
