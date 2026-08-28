#ifndef EMCR2X10MHZ_H
#define EMCR2X10MHZ_H

#include "emcropalkellydevice.h"

class Emcr2x10MHz_PCBV01_V01 : public EmcrOpalKellyDevice {
public:
    Emcr2x10MHz_PCBV01_V01(std::string di);

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
        VCVoltageFilter30Hz,
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

class Emcr2x10MHz_PCBV02_V01 : public Emcr2x10MHz_PCBV01_V01 {
public:
    Emcr2x10MHz_PCBV02_V01(std::string di);
};

class Emcr2x10MHz_PCBV01_V06 : public EmcrOpalKellyDevice {
public:
    Emcr2x10MHz_PCBV01_V06(std::string di);

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
        VCVoltageFilter30Hz,
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

class Emcr2x10MHz_PCBV02_V06 : public Emcr2x10MHz_PCBV01_V06 {
public:
    Emcr2x10MHz_PCBV02_V06(std::string di);

protected:
    enum VCVoltageFilters {
        VCVoltageFiltersNum = 0
    };
};

#endif // EMCR2X10MHZ_H
