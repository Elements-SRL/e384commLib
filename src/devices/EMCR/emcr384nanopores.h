#ifndef EMCR384NANOPORES_H
#define EMCR384NANOPORES_H

#include "emcropalkellydevice.h"

class Emcr384NanoPores_V01 : public EmcrOpalKellyDevice {
public:
    Emcr384NanoPores_V01(std::string di);

protected:

    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange4uA,
        VCCurrentRange200nA,
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
        VCCurrentFilter100kHz,
        VCCurrentFilter20kHz,
        VCCurrentFiltersNum
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
        SamplingRate6_25kHz,
        SamplingRate12_5kHz,
        SamplingRate25kHz,
        SamplingRate50kHz,
        SamplingRate100kHz,
        SamplingRate200kHz,
        SamplingRatesNum
    };

    enum Leds {
        LedBlue,
        LedsNum
    };

    BoolArrayCoder * minus24VCoder = nullptr;
    BoolArrayCoder * plus24VCoder = nullptr;
    
    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR384NANOPORES_H
