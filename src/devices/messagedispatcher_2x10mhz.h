#ifndef MESSAGEDISPATCHER_2X10MHZ_H
#define MESSAGEDISPATCHER_2X10MHZ_H

#include "messagedispatcher_opalkelly.h"

//#define REALLY_4_CHANNELS

class MessageDispatcher_2x10MHz_V01 : public MessageDispatcher_OpalKelly {
public:
    MessageDispatcher_2x10MHz_V01(std::string di);
    virtual ~MessageDispatcher_2x10MHz_V01();

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

    enum CalibResistances {
        CalibRes10_0MOhm,
        CalibResNum
    };

    virtual void initializeHW() override;

    BoolArrayCoder * dcmResetCoder = nullptr;
    BoolArrayCoder * writeAdcSpiCoder = nullptr;
    BoolArrayCoder * writeDacSpiCoder = nullptr;
};

#endif // MESSAGEDISPATCHER_2X10MHZ_H
