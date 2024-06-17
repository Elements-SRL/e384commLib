#ifndef EZPATCHE8PPATCHLINER_EL07AB_H
#define EZPATCHE8PPATCHLINER_EL07AB_H

#include "ezpatchftdidevice.h"

class EZPatche8PPatchliner_el07ab : public EZPatchFtdiDevice {
public:
    EZPatche8PPatchliner_el07ab(std::string di);
    virtual ~EZPatche8PPatchliner_el07ab();

    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) override;

protected:
    enum ClampingModalities {
        VoltageClamp,
        ZeroCurrentClamp,
        CurrentClamp,
        ClampingModalitiesNum
    };

    enum VCCurrentRanges {
        VCCurrentRange10nA,
        VCCurrentRange40nALbw,
        VCCurrentRange40nAHbw,
        VCCurrentRange400nA,
        VCCurrentRangesNum
    };

    enum VCCurrentRangesSwitches {
        VCCurrentRangesSwitchRange1,
        VCCurrentRangesSwitchRange2,
        VCCurrentRangesSwitchesNum
    };

    enum CCCurrentRanges {
        CCCurrentRange8nA,
        CCCurrentRangesNum
    };

    enum CCCurrentRangesSwitches {
        CCCurrentRangesSwitchesNum = 0
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum VCVoltageRangesSwitches {
        VCVoltageRangesSwitchesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRange1000mV,
        CCVoltageRangesNum
    };

    enum CCVoltageRangesSwitches {
        CCVoltageRangesSwitchesNum = 0
    };

    enum VCStimulusSwitches {
        VCStimulusSwitch_VcSW,
        VCStimulusSwitchesNum
    };

    enum CCStimulusSwitches {
        CCStimulusSwitch_StimEN,
        CCStimulusSwitchesNum
    };

    enum VCReaderSwitches {
        VCReaderSwitch_VcSel,
        VCReaderSwitchesNum
    };

    enum CCReaderSwitches {
        CCReaderSwitch_CcSW,
        CCReaderSwitchesNum
    };

    enum SamplingRates {
        SamplingRate6_25kHz,
        SamplingRate12_5kHz,
        SamplingRate25kHz,
        SamplingRate50kHz,
        SamplingRate100kHz,
        SamplingRate200kHz,
        SamplingRatesNum
    };

    enum SamplingRatesSwitches {
        SamplingRateSwitchVcAdcFilter2,
        SamplingRateSwitchVcAdcFilter1,
        SamplingRateSwitchVcAdcFilter0,
        SamplingRatesSwitchesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0,
        ChannelSourceVoltageFromCurrentClamp = 1,
        ChannelSourceVoltageFromDynamicClamp = 2,
        ChannelSourceVoltageFromVoltagePlusDynamicClamp = 3,
        ChannelSourceCurrentFromVoltageClamp = 0,
        ChannelSourceCurrentFromCurrentClamp = 1,
        ChannelSourceCurrentFromDynamicClamp = 2,
        ChannelSourceCurrentFromCurrentPlusDynamicClamp = 3
    };

    enum VCStimulusLpfs {
        VCStimulusLpf1kHz,
        VCStimulusLpf10kHz,
        VCStimulusLpf35kHz,
        VCStimulusLpfsNum
    };

    enum VCStimulusLpfSwitches {
        VCStimulusLpfSwitchFilter1,
        VCStimulusLpfSwitchFilter2,
        VCStimulusLpfSwitchesNum
    };

    enum CCStimulusLpfs {
        CCStimulusLpf1kHz,
        CCStimulusLpf10kHz,
        CCStimulusLpf35kHz,
        CCStimulusLpfsNum
    };

    enum CCStimulusLpfSwitches {
        CCStimulusLpfSwitchFilter1,
        CCStimulusLpfSwitchFilter2,
        CCStimulusLpfSwitchesNum
    };

    enum ConstantSwitches {
        ConstantSwitchEnableSpiDac,
        ConstantSwitchSelectUnusedChannels,
        ConstantSwitchCalibrateDacData,
        ConstantSwitchCh1InSW,
        ConstantSwitchCh1CalibEn,
        ConstantSwitchCh2InSW,
        ConstantSwitchCh2CalibEn,
        ConstantSwitchCh3InSW,
        ConstantSwitchCh3CalibEn,
        ConstantSwitchCh4InSW,
        ConstantSwitchCh4CalibEn,
        ConstantSwitchCh5InSW,
        ConstantSwitchCh5CalibEn,
        ConstantSwitchCh6InSW,
        ConstantSwitchCh6CalibEn,
        ConstantSwitchCh7InSW,
        ConstantSwitchCh7CalibEn,
        ConstantSwitchCh8InSW,
        ConstantSwitchCh8CalibEn,
        ConstantSwitchesNum
    };

    enum CompensationsSwitches {
        CompensationsSwitchCFastEn,
        CompensationsSwitchCSlowEn,
        CompensationsSwitchRCorrEn,
        CompensationsSwitchRPredEn,
        CompensationsSwitchCCFastEn,
        CompensationsSwitchesNum
    };

    enum CompensationsRegisters {
        CompensationsRegisterVCCFastGain = 4,
        CompensationsRegisterVCCSlowGain = 5,
        CompensationsRegisterVCCslowTau = 6,
        CompensationsRegisterVCRCorrGain = 7,
        CompensationsRegisterVCRPredGain = 8,
        CompensationsRegisterVCRPredTau = 9,
        CompensationsRegisterCCCFastGain = 4,
        CompensationsRegistersNum = 6
    };

    enum ChannelSources {
        ChannelSourceVoltage0 = 0,
        ChannelSourceCurrent0 = 1,
        ChannelSourcesNum
    };

    void selectChannelsResolutions() override;
    void selectVoltageOffsetResolution() override;
    void initializeCompensations() override;
    bool checkCompensationsValues() override;
    bool fillCompensationsRegistersTxData(std::vector <uint16_t> &txDataMessage) override;
    void updateWrittenCompensationValues(std::vector <uint16_t> &txDataMessage) override;

    uint16_t samplingRatesSwitchesNum = 0;
    std::vector <uint16_t> samplingRatesSwitchesWord;
    std::vector <uint16_t> samplingRatesSwitchesByte;
    std::vector <std::vector <bool>> samplingRatesSwitchesLut;

    std::string vcCurrentRangesSwitchesLutStrings[VCCurrentRangesNum];
    std::string ccCurrentRangesSwitchesLutStrings[CCCurrentRangesNum];
    std::string vcVoltageRangesSwitchesLutStrings[VCVoltageRangesNum];
    std::string ccVoltageRangesSwitchesLutStrings[CCVoltageRangesNum];
    std::string samplingRatesSwitchesLutStrings[SamplingRatesNum];
    std::string vcStimulusLpfSwitchesLutStrings[VCStimulusLpfsNum];
    std::string ccStimulusLpfSwitchesLutStrings[CCStimulusLpfsNum];
    std::string constantSwitchesLutStrings;
    std::string vcStimulusSwitchesLutStrings;
    std::string vcReaderSwitchesLutStrings;
    std::string ccStimulusSwitchesLutStrings;
    std::string ccReaderSwitchesLutStrings;

    /*! All of the following values are in pF (cap), MOhm (res), us (time) */

    const double liquidJunctionRange = 512.0;
    const double liquidJunctionSteps = UINT13_MAX+1.0;
    const double liquidJunctionStep = 2.0*liquidJunctionRange/liquidJunctionSteps;
    const double minLiquidJunctionRange = -liquidJunctionRange;
    const double maxLiquidJunctionRange = minLiquidJunctionRange+(liquidJunctionSteps-1.0)*liquidJunctionStep;
    const int liquidJunctionDecimals = 1;
    const UnitPfx_t liquidJunctionPrefix = UnitPfxMilli;
    const std::string liquidJunctionUnit = "V";
    const std::string liquidJunctionName = "Offset";

    const double pipetteVarResistance = 100.0e-3;
    const double pipetteFixedResistance = 80.0e-3;
    const double pipetteInjCapacitance1 = 2.5;
    const double pipetteInjCapacitance2 = 5.0;
    const double pipetteInjCapacitance3 = 10.0;
    const double pipetteInjCapacitance4 = 20.0;
    const double pipetteCapacitanceSteps1 = 64.0;
    const double pipetteCapacitanceSteps2 = 64.0;
    const double pipetteCapacitanceSteps3 = 64.0;
    const double pipetteCapacitanceSteps4 = 64.0;

    const double pipetteCapacitanceStep1 = pipetteVarResistance/pipetteFixedResistance/pipetteCapacitanceSteps1*pipetteInjCapacitance1;
    const double minPipetteCapacitance1 = pipetteCapacitanceStep1*(pipetteCapacitanceSteps1+1.0);
    const double maxPipetteCapacitance1 = minPipetteCapacitance1+(pipetteCapacitanceSteps1-1.0)*pipetteCapacitanceStep1;
    const double pipetteCapacitanceStep2 = pipetteVarResistance/pipetteFixedResistance/pipetteCapacitanceSteps2*pipetteInjCapacitance2;
    const double minPipetteCapacitance2 = pipetteCapacitanceStep2*(pipetteCapacitanceSteps2+1.0);
    const double maxPipetteCapacitance2 = minPipetteCapacitance2+(pipetteCapacitanceSteps2-1.0)*pipetteCapacitanceStep2;
    const double pipetteCapacitanceStep3 = pipetteVarResistance/pipetteFixedResistance/pipetteCapacitanceSteps3*pipetteInjCapacitance3;
    const double minPipetteCapacitance3 = pipetteCapacitanceStep3*(pipetteCapacitanceSteps3+1.0);
    const double maxPipetteCapacitance3 = minPipetteCapacitance3+(pipetteCapacitanceSteps3-1.0)*pipetteCapacitanceStep3;
    const double pipetteCapacitanceStep4 = pipetteVarResistance/pipetteFixedResistance/pipetteCapacitanceSteps4*pipetteInjCapacitance4;
    const double minPipetteCapacitance4 = pipetteCapacitanceStep4*(pipetteCapacitanceSteps4+1.0);
    const double maxPipetteCapacitance4 = minPipetteCapacitance4+(pipetteCapacitanceSteps4-1.0)*pipetteCapacitanceStep4;
    double additionalPipetteCapacitanceFromMembrane = 0.0;

    /*! Using smaller resolution to cover both ranges */
    const double pipetteCapacitanceStep = pipetteCapacitanceStep1;
    const double minPipetteCapacitance = minPipetteCapacitance1;
    const double maxPipetteCapacitance = maxPipetteCapacitance4;
    const double pipetteCapacitanceSteps = (maxPipetteCapacitance-minPipetteCapacitance)/pipetteCapacitanceStep+1.0;
    const int pipetteCapacitanceDecimals = 2;
    const UnitPfx_t pipetteCapacitancePrefix = UnitPfxPico;
    const std::string pipetteCapacitanceUnit = "F";
    const std::string pipetteCapacitanceName = "Pipette\nCapacitance";

    const double membraneGainSteps = 64.0;
    const double membraneGainStep = 2.0/membraneGainSteps;
    const double minMembraneGain = 1.0+membraneGainStep;
    const double maxMembraneGain = minMembraneGain+(membraneGainSteps-1.0)*membraneGainStep;
    const double membraneInjCapacitance1 = 5.0;
    const double membraneInjCapacitance2 = 15.0;
    const double membraneInjCapacitance3 = 45.0;
    const double membraneInjCapacitance4 = 135.0;
    const double membraneCapacitanceSteps1 = 64.0;
    const double membraneCapacitanceSteps2 = 64.0;
    const double membraneCapacitanceSteps3 = 64.0;
    const double membraneCapacitanceSteps4 = 64.0;

    const double membraneCapacitanceStep1 = membraneGainStep*membraneInjCapacitance1;
    const double minMembraneCapacitance1 = minMembraneGain*membraneInjCapacitance1;
    const double maxMembraneCapacitance1 = maxMembraneGain*membraneInjCapacitance1;
    const double membraneCapacitanceStep2 = membraneGainStep*membraneInjCapacitance2;
    const double minMembraneCapacitance2 = minMembraneGain*membraneInjCapacitance2;
    const double maxMembraneCapacitance2 = maxMembraneGain*membraneInjCapacitance2;
    const double membraneCapacitanceStep3 = membraneGainStep*membraneInjCapacitance3;
    const double minMembraneCapacitance3 = minMembraneGain*membraneInjCapacitance3;
    const double maxMembraneCapacitance3 = maxMembraneGain*membraneInjCapacitance3;
    const double membraneCapacitanceStep4 = membraneGainStep*membraneInjCapacitance4;
    const double minMembraneCapacitance4 = minMembraneGain*membraneInjCapacitance4;
    const double maxMembraneCapacitance4 = maxMembraneGain*membraneInjCapacitance4;

    /*! Using smaller resolution to cover both ranges */
    const double membraneCapacitanceStep = membraneCapacitanceStep1;
    const double minMembraneCapacitance = minMembraneCapacitance1;
    const double maxMembraneCapacitance = maxMembraneCapacitance4;
    const double membraneCapacitanceSteps = (maxMembraneCapacitance-minMembraneCapacitance)/membraneCapacitanceStep+1.0;
    const int membraneCapacitanceDecimals = 1;
    const UnitPfx_t membraneCapacitancePrefix = UnitPfxPico;
    const std::string membraneCapacitanceUnit = "F";
    const std::string membraneCapacitanceName = "Membrane\nCapacitance";

    const UnitPfx_t accessResistancePrefix = UnitPfxMega;
    const std::string accessResistanceUnit = "Ohm";
    const std::string accessResistanceName = "Access\nResistance";

    const double membraneTauResistanceStep = 0.2;
    const double minMembraneTauResistance = 0.2;
    const double membraneTauResistanceSteps = 256.0;
    const double maxMembraneTauResistance = minMembraneTauResistance+(membraneTauResistanceSteps-1.0)*membraneTauResistanceStep;
    const double minMembraneTauCapacitance1 = 2.5;
    const double minMembraneTauCapacitance2 = 25.0;

    const double membraneTauSteps = membraneTauResistanceSteps;
    const double membraneTauStep1 = membraneTauResistanceStep*minMembraneTauCapacitance1;
    const double minMembraneTau1 = membraneTauStep1;
    const double maxMembraneTau1 = minMembraneTau1+(membraneTauSteps-1.0)*membraneTauStep1;
    const double membraneTauStep2 = membraneTauResistanceStep*minMembraneTauCapacitance2;
    const double minMembraneTau2 = membraneTauStep2;
    const double maxMembraneTau2 = minMembraneTau2+(membraneTauSteps-1.0)*membraneTauStep2;

    const double membraneTauStep = membraneTauStep1;
    const double minMembraneTau = minMembraneTau1;
    const double maxMembraneTau = maxMembraneTau2;

    const double resistanceCorrectionPercentageSteps = 64.0;
    const double minResistanceCorrection = 0.4;
    const double resistanceCorrectionStep = 0.4;
    const double maxResistanceCorrection = (resistanceCorrectionPercentageSteps-1.0)*resistanceCorrectionStep;
    const double minResistanceCorrectionPercentage = 0.0;
    const double maxResistanceCorrectionPercentage = 100.0;
    const UnitPfx_t resistanceCorrectionPercentagePrefix = UnitPfxNone;
    const std::string resistanceCorrectionPercentageUnit = "%";
    const std::string resistanceCorrectionPercentageName = "Correction\nPercentage";

    const double resistancePredictionGainSteps = 64.0;
    const double resistancePredictionGainRange = 4.0;
    const double resistancePredictionGainStep = resistancePredictionGainRange/resistancePredictionGainSteps;
    const double minResistancePredictionGain = 1.0+resistancePredictionGainStep;
    const double maxResistancePredictionGain = minResistancePredictionGain+(resistancePredictionGainSteps-1.0)*resistancePredictionGainStep;
    const UnitPfx_t resistancePredictionGainPrefix = UnitPfxNone;
    const std::string resistancePredictionGainUnit = " ";
    const std::string resistancePredictionGainName = "Prediction\nGain";

    const double resistancePredictionTauStep = 2.0;
    const double resistancePredictionTauSteps = 256.0;
    const int resistancePredictionTauDecimals = 0;
    const double minResistancePredictionTau = 2.0;
    const double maxResistancePredictionTau = minResistancePredictionTau+(resistancePredictionTauSteps-1.0)*resistancePredictionTauStep;
    const UnitPfx_t resistancePredictionTauPrefix = UnitPfxMicro;
    const std::string resistancePredictionTauUnit = "s";
    const std::string resistancePredictionTauName = "Prediction\nTau";

    const double ccPipetteCapacitanceStep = pipetteCapacitanceStep;
    const double ccPipetteCapacitanceSteps = pipetteCapacitanceSteps;
    const int ccPipetteCapacitanceDecimals = pipetteCapacitanceDecimals;
    const double minCcPipetteCapacitance = minPipetteCapacitance;
    const double maxCcPipetteCapacitance = maxPipetteCapacitance;
    const UnitPfx_t ccPipetteCapacitancePrefix = pipetteCapacitancePrefix;
    const std::string ccPipetteCapacitanceUnit = pipetteCapacitanceUnit;
    const std::string ccPipetteCapacitanceName = pipetteCapacitanceName;

    /*! Compensations values succesfully written to the device (opposite from default values, this way the initialization write works, because the values must be updated) */
    std::vector <uint16_t> pipetteCapacitanceRegValue;
    std::vector <uint16_t> membraneCapacitanceRegValue;
    std::vector <uint16_t> membraneTauRegValue;
    std::vector <uint16_t> correctionGainRegValue;
    std::vector <uint16_t> predictionGainRegValue;
    std::vector <uint16_t> predictionTauRegValue;
    std::vector <uint16_t> ccPipetteCapacitanceRegValue;
};

class EZPatche8PPatchliner_el07ab_artix7_PCBV01 : public EZPatche8PPatchliner_el07ab {
public:
    EZPatche8PPatchliner_el07ab_artix7_PCBV01(std::string di);
    virtual ~EZPatche8PPatchliner_el07ab_artix7_PCBV01();
};

class EZPatche8PPatchliner_el07ab_artix7_PCBV02_V01 : public EZPatche8PPatchliner_el07ab_artix7_PCBV01 {
public:
    EZPatche8PPatchliner_el07ab_artix7_PCBV02_V01(std::string di);
    virtual ~EZPatche8PPatchliner_el07ab_artix7_PCBV02_V01();
};

class EZPatche8PPatchliner_el07ab_artix7_PCBV02_V02 : public EZPatche8PPatchliner_el07ab_artix7_PCBV02_V01 {
public:
    EZPatche8PPatchliner_el07ab_artix7_PCBV02_V02(std::string di);
    virtual ~EZPatche8PPatchliner_el07ab_artix7_PCBV02_V02();
};

#endif // EZPATCHE8PPATCHLINER_EL07AB_H
