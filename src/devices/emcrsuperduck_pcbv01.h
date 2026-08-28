#ifndef EMCRSUPERDUCK_PCBV_1_H
#define EMCRSUPERDUCK_PCBV_1_H

#include "emcrftdidevice.h"

class EmcrSuperDuck_PCBV01 : public EmcrFtdiDevice {
public:
    EmcrSuperDuck_PCBV01(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange3330mV,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange2500mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFilter10Hz,
        VCCurrentFiltersNum,
    };

    enum VCVoltageFilters {
        VCVoltageFilter10Hz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate25kHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCRSUPERDUCK_PCBV_1_H
