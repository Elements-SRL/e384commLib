#ifndef EMCR2X1_MHZ_SB_PCBV_2_FESTIM_H
#define EMCR2X1_MHZ_SB_PCBV_2_FESTIM_H

#include "emcropalkellydevice.h"

class Emcr2x10MHz_SB_PCBV02_FEStim_V01 : public EmcrOpalKellyDevice {
public:
    Emcr2x10MHz_SB_PCBV02_FEStim_V01(std::string di);

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
        VCVoltageFilter16kHz,
        VCVoltageFilter1_6kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate26_7MHz,
        SamplingRate13_3MHz,
        SamplingRate6_67MHz,
        SamplingRate3_33MHz,
        SamplingRate1_67MHz,
        SamplingRate833kHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;

    BoolArrayCoder * dcmResetCoder = nullptr;
    BoolArrayCoder * writeAdcSpiCoder = nullptr;
    BoolArrayCoder * writeDacSpiCoder = nullptr;
};

#endif // EMCR2X1_MHZ_SB_PCBV_2_FESTIM_H
