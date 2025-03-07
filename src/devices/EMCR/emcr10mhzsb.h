#ifndef EMCR10MHZ_H
#define EMCR10MHZ_H

#include "emcropalkellydevice.h"

class Emcr10MHzSB_V01 : public EmcrOpalKellyDevice {
public:
    Emcr10MHzSB_V01(std::string di);

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
        VCVoltageRange1600mV,
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
        VCVoltageFilter160Hz,
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
        SamplingRate40MHz,
        SamplingRate20MHz,
        SamplingRate10MHz,
        SamplingRate5MHz,
        SamplingRate2_5MHz,
        SamplingRate1_25MHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCR10MHZ_H
