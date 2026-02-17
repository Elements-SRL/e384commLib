#ifndef EMCR8BLM_EL_3C_DIGBRD_FW_V_1_H
#define EMCR8BLM_EL_3C_DIGBRD_FW_V_1_H

#include "emcropalkellydevice.h"

class Emcr8Blm_EL03c_DigBrd_fw_v01 : public EmcrOpalKellyDevice {
public:
    Emcr8Blm_EL03c_DigBrd_fw_v01(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange200pA,
        VCCurrentRange2nA,
        VCCurrentRange20nA,
        VCCurrentRange200nA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum LJVoltageRanges {
        LJVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFilter16kHz,
        VCCurrentFilter100kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter10Hz,
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

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR8BLM_EL_3C_DIGBRD_FW_V_1_H
