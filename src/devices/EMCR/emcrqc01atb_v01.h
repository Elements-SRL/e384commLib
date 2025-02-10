#ifndef EMCRQC01ATB_V01_H
#define EMCRQC01ATB_V01_H

#include "emcropalkellydevice.h"

class EmcrQc01aTB_V01 : public EmcrOpalKellyDevice {
public:
    EmcrQc01aTB_V01(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange1nA,
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
        VCCurrentFiltersNum = 0
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
        SamplingRate26_7MHz,
        SamplingRate13_3MHz,
        SamplingRate6_67MHz,
        SamplingRate3_33MHz,
        SamplingRate1_67MHz,
        SamplingRate833kHz,
        SamplingRate156kHz,
        SamplingRatesNum
    };

    enum CustomFlags {
        AcDsEn1,
        AcDsEn2,
        DcDsEn1,
        DcDsEn2,
        DcSepEn1,
        DcSepEn2,
        CustomFlagsNum
    };

    enum CustomOptions {
        DrvIn1,
        DrvIn2,
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

    BoolArrayCoder * dcmResetCoder = nullptr;
    BoolArrayCoder * writeAdcSpiCoder = nullptr;
    // BoolArrayCoder * writeDacSpiCoder = nullptr;
};

#endif // EMCRQC01ATB_V01_H
