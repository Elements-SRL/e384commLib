#ifndef MESSAGEDISPATCHER_384NANOPORES_SR7P5KHZ_V01_H
#define MESSAGEDISPATCHER_384NANOPORES_SR7P5KHZ_V01_H
#include "messagedispatcher_384nanopores.h"

class MessageDispatcher_384NanoPores_SR7p5kHz_V01 :public MessageDispatcher_384NanoPores_V01 {
public:
    MessageDispatcher_384NanoPores_SR7p5kHz_V01(std::string di);
    virtual ~MessageDispatcher_384NanoPores_SR7p5kHz_V01();

protected:
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
};

#endif // MESSAGEDISPATCHER_384NANOPORES_SR7P5KHZ_V01_H
