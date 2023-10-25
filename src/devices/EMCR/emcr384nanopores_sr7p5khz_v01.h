#ifndef EMCR384NANOPORES_SR7P5KHZ_V01_H
#define EMCR384NANOPORES_SR7P5KHZ_V01_H
#include "emcr384nanopores.h"

class Emcr384NanoPores_SR7p5kHz_V01 : public Emcr384NanoPores_V01 {
public:
    Emcr384NanoPores_SR7p5kHz_V01(std::string di);
    virtual ~Emcr384NanoPores_SR7p5kHz_V01();

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

#endif // EMCR384NANOPORES_SR7P5KHZ_V01_H
