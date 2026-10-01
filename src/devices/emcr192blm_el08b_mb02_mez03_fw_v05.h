#ifndef EMCR192BLM_EL_8B_MB_2_MEZ_3_FW_V_5_H
#define EMCR192BLM_EL_8B_MB_2_MEZ_3_FW_V_5_H

#include "emcropalkellydevice.h"

class Emcr192Blm_EL08b_Mb02_Mez03_fw_v05 : public EmcrOpalKellyDevice {
public:
    Emcr192Blm_EL08b_Mb02_Mez03_fw_v05(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange250pA,
        VCCurrentRange2_5nA,
        VCCurrentRange25nA,
        VCCurrentRange250nA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum LJVoltageRanges {
        LJVoltageRange500mV,
        LJVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFilter5kHz,
        VCCurrentFilter10kHz,
        VCCurrentFilter20kHz,
        VCCurrentFilter100kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter26Hz,
        VCVoltageFilter1kHz,
        VCVoltageFilter5kHz,
        VCVoltageFilter10kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate1_25kHz,
        SamplingRate2_5kHz,
        SamplingRate5kHz,
        SamplingRate10kHz,
        SamplingRate20kHz,
        SamplingRate50kHz,
        SamplingRate100kHz,
        SamplingRate200kHz,
        SamplingRatesNum
    };

    enum ClockDividers {
        ClockDivider8,
        ClockDivider4,
        ClockDivider2,
        ClockDivider1,
        ClockDividersNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR192BLM_EL_8B_MB_2_MEZ_3_FW_V_5_H
