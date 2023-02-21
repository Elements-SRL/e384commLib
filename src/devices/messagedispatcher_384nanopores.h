#ifndef MESSAGEDISPATCHER_384NANOPORES_H
#define MESSAGEDISPATCHER_384NANOPORES_H

#include "messagedispatcher_opalkelly.h"

#include <iostream>

using namespace std;

class MessageDispatcher_384NanoPores_V01 : public MessageDispatcher_OpalKelly {
public:
    MessageDispatcher_384NanoPores_V01(string di);
    virtual ~MessageDispatcher_384NanoPores_V01();

protected:
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

    enum SamplingRates {
        SamplingRate100Hz,
        SamplingRatesNum
    };

    enum Leds {
        LedBlue,
        LedsNum
    };
};

#endif // MESSAGEDISPATCHER_384NANOPORES_H
