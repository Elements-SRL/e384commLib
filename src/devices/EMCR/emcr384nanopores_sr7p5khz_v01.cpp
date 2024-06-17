#include "emcr384nanopores_sr7p5khz_v01.h"

Emcr384NanoPores_SR7p5kHz_V01::Emcr384NanoPores_SR7p5kHz_V01(std::string di):
    Emcr384NanoPores_V01(di) {

    rxWordLengths[RxMessageStatus] = 1;
    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate7_5kHz].value = 7.5;
    samplingRatesArray[SamplingRate7_5kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate7_5kHz].unit = "Hz";
    samplingRatesArray[SamplingRate15kHz].value = 15.0;
    samplingRatesArray[SamplingRate15kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate15kHz].unit = "Hz";
    samplingRatesArray[SamplingRate30kHz].value = 30.0;
    samplingRatesArray[SamplingRate30kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate30kHz].unit = "Hz";
    samplingRatesArray[SamplingRate60kHz].value = 60.0;
    samplingRatesArray[SamplingRate60kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate60kHz].unit = "Hz";
    samplingRatesArray[SamplingRate120kHz].value = 120.0;
    samplingRatesArray[SamplingRate120kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate120kHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate7_5kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate7_5kHz].value = 7.5;
    realSamplingRatesArray[SamplingRate7_5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate7_5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate15kHz].value = 15.0;
    realSamplingRatesArray[SamplingRate15kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate15kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate30kHz].value = 30.0;
    realSamplingRatesArray[SamplingRate30kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate30kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate60kHz].value = 60.0;
    realSamplingRatesArray[SamplingRate60kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate60kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate120kHz].value = 120.0;
    realSamplingRatesArray[SamplingRate120kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate120kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate7_5kHz].value = 1.0/7.5;
    integrationStepArray[SamplingRate7_5kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate7_5kHz].unit = "s";
    integrationStepArray[SamplingRate15kHz].value = 1.0/15.0;
    integrationStepArray[SamplingRate15kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate15kHz].unit = "s";
    integrationStepArray[SamplingRate30kHz].value = 1.0/30.0;
    integrationStepArray[SamplingRate30kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate30kHz].unit = "s";
    integrationStepArray[SamplingRate60kHz].value = 1.0/60.0;
    integrationStepArray[SamplingRate60kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate60kHz].unit = "s";
    integrationStepArray[SamplingRate120kHz].value = 1.0/120.0;
    integrationStepArray[SamplingRate120kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate120kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate7_5kHz, VCCurrentFilter20kHz},
        {SamplingRate15kHz, VCCurrentFilter20kHz},
        {SamplingRate30kHz, VCCurrentFilter20kHz},
        {SamplingRate60kHz, VCCurrentFilter100kHz},
        {SamplingRate120kHz, VCCurrentFilter100kHz}
    };

    // mapping ADC Current Clamp
    // undefined

    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];
    selectedSamplingRateIdx = defaultSamplingRateIdx;
}
