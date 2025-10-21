#ifndef EMCR192BLM_EL_3C_MEZ_3_MB_4_FW_V_1_H
#define EMCR192BLM_EL_3C_MEZ_3_MB_4_FW_V_1_H

#include "emcropalkellydevice.h"

class Emcr192Blm_EL03c_Mez03_Mb04_fw_v01 : public EmcrOpalKellyDevice {
public:
    Emcr192Blm_EL03c_Mez03_Mb04_fw_v01(std::string di);

    ErrorCodes_t setCoolingFansSpeed(Measurement_t speed, bool applyFlag) override;
    ErrorCodes_t getCoolingFansSpeedRange(RangedMeasurement_t &range) override;
    ErrorCodes_t setTemperatureControl(Measurement_t temperature, bool enabled) override;
    ErrorCodes_t setTemperatureControlPid(PidParams_t params) override;

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange200pA,
        VCCurrentRange2nA,
        VCCurrentRange20nA,
        VCCurrentRange200nA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum LJVoltageRanges {
        LJVoltageRange50mV,
        LJVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum TemperatureChannels {
        TemperatureSensor0,
        TemperatureSensor1,
        TemperatureChannelsNum
    };

    enum VCCurrentFilters {
        VCCurrentFilter16kHz,
        VCCurrentFilter100kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter10Hz,
        VCVoltageFilter10kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate1_25kHz,
        SamplingRate5kHz,
        SamplingRate10kHz,
        SamplingRate20kHz,
        SamplingRate50kHz,
        SamplingRate100kHz,
        SamplingRate200kHz,
        SamplingRatesNum
    };

    enum FanTrimmerRanges {
        FanTrimmerOff,
        FanTrimmerSlow,
        FanTrimmerFast,
        FanTrimmerRangesNum
    };

    virtual ErrorCodes_t initializeHW() override;

    Measurement_t fanV2R(Measurement_t V);
    Measurement_t fanW2V(Measurement_t W);
    Measurement_t fanRT2W(Measurement_t RT);
    void processTemperatureData(std::vector <Measurement_t> temperaturesRead) override;

    MultiCoder * fanTrimmerCoder = nullptr;
    std::vector <RangedMeasurement_t> fanTrimmerRanges;

    /*! Temperature control */
    bool tControlEnabled = false;

    std::chrono::time_point <std::chrono::steady_clock> then;
    Measurement_t temperatureSet = {42.0, UnitPfxNone, "°C"};

    const Measurement_t fanTrimmerRf = {10.0, UnitPfxKilo, "Ohm"};
    const Measurement_t fanTrimmerVRef = {0.6, UnitPfxNone, "V"};
    const Measurement_t fanTrimmerVMax = {12.0, UnitPfxNone, "V"};
    const Measurement_t fanTrimmerWMax = {5900.0, UnitPfxNone, "rpm"};
    const Measurement_t fanTrimmerWMin = {3524.0, UnitPfxNone, "rpm"};
    const Measurement_t fanTrimmerRTMin = {0.264962843373366, UnitPfxNone, "K/W"};
    const Measurement_t fanTrimmerRTMax = {0.390196941234880, UnitPfxNone, "K/W"};
    const Measurement_t fanTrimmerRTOff = {1.582470252096335, UnitPfxNone, "K/W"};

    double ie = 0.0;
    double preve = 0.0;
    double pg = 0.04;
    double ig = 0.006;
    double dg = 0.0;
    double ieMax = 200.0;
    Measurement_t speedSet = fanTrimmerWMax;
};

#endif // EMCR192BLM_EL_3C_MEZ_3_MB_4_FW_V_1_H
