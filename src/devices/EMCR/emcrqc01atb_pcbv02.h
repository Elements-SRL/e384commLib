#ifndef EMCRQC01ATB_PCBV02_H
#define EMCRQC01ATB_PCBV02_H

#include "emcrftdidevice.h"

class EmcrQc01aTB_PCBV02 : public EmcrFtdiDevice {
public:
    EmcrQc01aTB_PCBV02(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        // VCCurrentRange1nA,
        VCCurrentRange10nA,
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
        VCCurrentFilter20kHz,
        VCCurrentFilter100kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter1kHz,
        VCVoltageFilter10kHz,
        VCVoltageFilter20kHz,
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
        SamplingRate312_5kHz,
        SamplingRate625kHz,
        SamplingRate1_25MHz,
        SamplingRate2_5MHz,
        SamplingRate5MHz,
        SamplingRate10MHz,
        SamplingRatesNum
    };

    enum CustomFlags {
        AcDsEn,
        DcDsEn,
        DcSepEn,
        CustomFlagsNum
    };

    enum CustomOptions {
        DacExtFilter,
        CustomOptionsNum
    };

    enum CustomDoubles {
        DacExt,
        SecondaryDacCore1,
        SecondaryDacCore2,
        CustomDoublesNum
    };

    virtual ErrorCodes_t initializeHW() override;
    virtual ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) override;
    virtual ErrorCodes_t setVoltageReference(Measurement_t voltage, bool applyFlag) override;
};

#endif // EMCRQC01ATB_PCBV02_H
