#ifndef MESSAGEDISPATCHER_384NANOPORES_H
#define MESSAGEDISPATCHER_384NANOPORES_H

#include "messagedispatcher_opalkelly.h"

class MessageDispatcher_384NanoPores_V01 : public MessageDispatcher_OpalKelly {
public:
    MessageDispatcher_384NanoPores_V01(std::string di);
    virtual ~MessageDispatcher_384NanoPores_V01();

protected:

    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
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
        SamplingRate7_5kHz,
#ifdef E384NPR_ADDITIONAL_SR_FLAG
        SamplingRate15kHz,
        SamplingRate30kHz,
        SamplingRate60kHz,
        SamplingRate120kHz,
#endif
        SamplingRatesNum
    };

    enum Leds {
        LedBlue,
        LedsNum
    };

    enum CalibResistances{
        CalibRes120kOhm,
        CalibRes2_49MOhm,
        CalibResNum
    };

    BoolArrayCoder * minus24VCoder = nullptr;
    BoolArrayCoder * plus24VCoder = nullptr;
    
    virtual void initializeHW() override;
};

#endif // MESSAGEDISPATCHER_384NANOPORES_H
