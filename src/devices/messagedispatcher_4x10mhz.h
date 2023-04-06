#ifndef MESSAGEDISPATCHER_4x10MHZ_H
#define MESSAGEDISPATCHER_4x10MHZ_H

#include "messagedispatcher_opalkelly.h"

using namespace std;

class MessageDispatcher_4x10MHz_V01 : public MessageDispatcher_OpalKelly {
public:
    MessageDispatcher_4x10MHz_V01(string di);
    virtual ~MessageDispatcher_4x10MHz_V01();

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
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
        VCVoltageFilter20kHz,
        VCVoltageFilter1kHz,
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
        SamplingRate13_3MHz,
        SamplingRate6_67MHz,
        SamplingRate3_33MHz,
        SamplingRate1_67MHz,
        SamplingRate833kHz,
        SamplingRate417kHz,
        SamplingRatesNum
    };

    enum Leds {
        LedBlue,
        LedsNum
    };

    virtual void initializeHW() override;

    BoolArrayCoder * dcmResetCoder = nullptr;
    BoolArrayCoder * writeAdcSpiCoder = nullptr;
    BoolArrayCoder * writeDacSpiCoder = nullptr;
};

#endif // MESSAGEDISPATCHER_4x10MHZ_H