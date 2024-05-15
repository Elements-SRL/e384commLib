#ifndef EMCR4x10MHZ_H
#define EMCR4x10MHZ_H

#include "emcropalkellydevice.h"

class Emcr4x10MHz_PCBV01_V02 : public EmcrOpalKellyDevice {
public:
    Emcr4x10MHz_PCBV01_V02(std::string di);

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
        VCVoltageRange1000mV,
        VCVoltageRange20000mV,
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
        VCVoltageFilter10kHz,
        VCVoltageFilter1_5kHz,
        VCVoltageFilter3Hz,
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

class Emcr4x10MHz_PCBV01_V03 : public EmcrOpalKellyDevice {
public:
    Emcr4x10MHz_PCBV01_V03(std::string di);

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
        VCVoltageRange1000mV,
        VCVoltageRange20000mV,
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
        VCVoltageFilter10kHz,
        VCVoltageFilter1_5kHz,
        VCVoltageFilter3Hz,
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

class Emcr4x10MHz_PCBV03_V03: public Emcr4x10MHz_PCBV01_V03 {
public:
    Emcr4x10MHz_PCBV03_V03(std::string di);

protected:
    enum VCVoltageFilters {
        VCVoltageFilter16kHz,
        VCVoltageFilter1_6kHz,
        VCVoltageFiltersNum
    };
};

class Emcr4x10MHz_PCBV03_V04: public Emcr4x10MHz_PCBV03_V03 {
public:
    Emcr4x10MHz_PCBV03_V04(std::string di);
};

class Emcr4x10MHz_SB_PCBV01_V05: public Emcr4x10MHz_PCBV03_V04 {
public:
    Emcr4x10MHz_SB_PCBV01_V05(std::string di);

protected:
    enum CustomOptions {
        CustomOptionInterposer,
        CustomOptionsNum
    };
};

#endif // EMCR4x10MHZ_H
