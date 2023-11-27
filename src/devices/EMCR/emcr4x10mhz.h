#ifndef EMCR4x10MHZ_H
#define EMCR4x10MHZ_H

#include "emcropalkellydevice.h"

class Emcr4x10MHz_PCBV01_V02 : public EmcrOpalKellyDevice {
public:
    Emcr4x10MHz_PCBV01_V02(std::string di);
    virtual ~Emcr4x10MHz_PCBV01_V02();

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

    enum CalibResistances {
        CalibRes10_0MOhm,
        CalibResNum
    };

    virtual void initializeHW() override;

    BoolArrayCoder * dcmResetCoder = nullptr;
    BoolArrayCoder * writeAdcSpiCoder = nullptr;
    BoolArrayCoder * writeDacSpiCoder = nullptr;
};

class Emcr4x10MHz_PCBV01_V03 : public EmcrOpalKellyDevice {
public:
    Emcr4x10MHz_PCBV01_V03(std::string di);
    virtual ~Emcr4x10MHz_PCBV01_V03();

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

    enum CalibResistances {
        CalibRes10_0MOhm,
        CalibResNum
    };

    virtual void initializeHW() override;

    BoolArrayCoder * dcmResetCoder = nullptr;
    BoolArrayCoder * writeAdcSpiCoder = nullptr;
    BoolArrayCoder * writeDacSpiCoder = nullptr;
};

class Emcr4x10MHz_PCBV03_V05: public Emcr4x10MHz_PCBV01_V03{
public:
    Emcr4x10MHz_PCBV03_V05(std::string di);
    virtual ~Emcr4x10MHz_PCBV03_V05();
};

#endif // EMCR4x10MHZ_H
