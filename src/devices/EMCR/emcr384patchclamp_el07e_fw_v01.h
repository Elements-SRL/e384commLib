#ifndef EMCR384PATCHCLAMP_EL07E_FW_V01_H
#define EMCR384PATCHCLAMP_EL07E_FW_V01_H

#include "emcropalkellydevice.h"

class Emcr384PatchClamp_EL07e_fw_v01 : public EmcrOpalKellyDevice {
public:
    Emcr384PatchClamp_EL07e_fw_v01(std::string di);

    virtual ErrorCodes_t getCompOptionsFeatures(CompensationTypes_t type, std::vector <std::string> &compOptionsArray) override;
    virtual ErrorCodes_t getCompensationEnables(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <bool> &onValues) override;
    virtual ErrorCodes_t enableCompensation(std::vector <uint16_t> channelIndexes, CompensationTypes_t compTypeToEnable, std::vector <bool> onValues, bool applyFlag) override;
    virtual ErrorCodes_t enableVcCompensations(bool enable, bool applyFlag) override;
    virtual ErrorCodes_t enableCcCompensations(bool enable, bool applyFlag) override;
    virtual ErrorCodes_t setCompValues(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <double> newParamValues, bool applyFlag) override;
    virtual ErrorCodes_t setCompOptions(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <uint16_t> options, bool applyFlag) override;

    virtual ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlag) override;
    virtual ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlag) override;
    virtual ErrorCodes_t turnVoltageStimulusOn(bool onValue, bool applyFlag) override;
    virtual ErrorCodes_t turnCurrentStimulusOn(bool onValue, bool applyFlag) override;

    ErrorCodes_t setCoolingFansSpeed(Measurement_t speed, bool applyFlag) override;
    ErrorCodes_t getCoolingFansSpeedRange(RangedMeasurement_t &range) override;
    ErrorCodes_t setTemperatureControl(Measurement_t temperature, bool enabled) override;
    ErrorCodes_t setTemperatureControlPid(PidParams_t params) override;

protected:
    std::vector <double> membraneCapValueInjCapacitance;

    enum TemperatureChannels {
        TemperatureSensor0,
        TemperatureSensor1,
        TemperatureChannelsNum
    };

    enum ClampingModalities {
        VoltageClamp,
        ZeroCurrentClamp,
        CurrentClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0,
        ChannelSourceVoltageFromCurrentClamp = 1,
//        ChannelSourceVoltageFromDynamicClamp = 2,
//        ChannelSourceVoltageFromVoltagePlusDynamicClamp = 3,
        ChannelSourceCurrentFromVoltageClamp = 0,
        ChannelSourceCurrentFromCurrentClamp = 1/*,
        ChannelSourceCurrentFromDynamicClamp = 2,
        ChannelSourceCurrentFromCurrentPlusDynamicClamp = 3*/
    };

    enum VCCurrentRanges {
        VCCurrentRange10nA,
        VCCurrentRange40nALbw,
        VCCurrentRange40nAHbw,
        VCCurrentRange400nA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRange8nA,
        CCCurrentRange2nA,
        CCCurrentRangesNum
    };

    enum CCVoltageRanges {
        CCVoltageRange250mV,
        CCVoltageRangesNum
    };

    enum VCCurrentFilters {
        VCCurrentFilter5kHz,
        VCCurrentFilter6kHz,
        VCCurrentFilter10kHz,
        VCCurrentFilter12kHz,
        VCCurrentFilter40kHz,
        VCCurrentFilter48kHz,
        VCCurrentFilter100kHz,
        VCCurrentFilter120kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter1kHz,
        VCVoltageFilter10kHz,
        VCVoltageFilter20kHz,
        VCVoltageFilter50kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFilter1kHz,
        CCCurrentFilter10kHz,
        CCCurrentFilter20kHz,
        CCCurrentFilter50kHz,
        CCCurrentFiltersNum
    };

    enum CCVoltageFilters {
        CCVoltageFilter5kHz,
        CCVoltageFilter6kHz,
        CCVoltageFilter10kHz,
        CCVoltageFilter12kHz,
        CCVoltageFilter40kHz,
        CCVoltageFilter48kHz,
        CCVoltageFilter100kHz,
        CCVoltageFilter120kHz,
        CCVoltageFiltersNum
    };

    enum SamplingRates {
        SamplingRate5kHz,
        SamplingRate10kHz,
        SamplingRate20kHz,
        SamplingRate40kHz,
        SamplingRate80kHz,
        SamplingRatesNum
    };

    enum CompensationAsicParams {
        A_Cp,       //PipetteCapacitance
        A_Cm,       //MembraneCapacitance
        A_Taum,     //MembraneTau
        A_RsCr,     //SeriesCorrectionResistance
        A_RsPg,     //SeriesPredictionGain
        A_RsPtau,   //SeriesPredictionTau
        CompensationAsicParamsNum
    };

    enum CompensationRsCorrBws {
        CompensationRsCorrBw79_5kHz,
        CompensationRsCorrBw53_1kHz,
        CompensationRsCorrBw31_8kHz,
        CompensationRsCorrBw17_7kHz,
        CompensationRsCorrBw9_36kHz,
        CompensationRsCorrBw4_82kHz,
        CompensationRsCorrBw2_45kHz,
        CompensationRsCorrBw1_23kHz,
        CompensationRsCorrBwNum
    };

    enum FanTrimmerRanges {
        FanTrimmerOff,
        FanTrimmerSlow,
        FanTrimmerFast,
        FanTrimmerRangesNum
    };

    virtual ErrorCodes_t initializeHW() override;

    virtual std::vector <double> user2AsicDomainTransform(int chIdx, std::vector <double> userDomainParams) override;
    virtual std::vector <double> asic2UserDomainTransform(int chIdx, std::vector <double> asicDomainParams, double oldUCpVc, double oldUCpCc) override;
    virtual ErrorCodes_t asic2UserDomainCompensable(int chIdx, std::vector <double> asicDomainParams, std::vector <double> userDomainParams) override;

    virtual ErrorCodes_t getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) override;

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

#endif // EMCR384PATCHCLAMP_EL07E_FW_V01_H
