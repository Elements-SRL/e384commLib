#ifndef EZPATCHE8PPATCHLINER_EL07CD_H
#define EZPATCHE8PPATCHLINER_EL07CD_H

#include "ezpatchftdidevice.h"

class EZPatche8PPatchliner_el07cd_artix7_PCBV01 : public EZPatchFtdiDevice {
public:
    EZPatche8PPatchliner_el07cd_artix7_PCBV01(std::string di);

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
        CCCurrentRange2nA,
        CCCurrentRangesNum
    };

    enum CCCurrentRangesSwitches {
        CCCurrentRangesSwtichRange1,
        CCCurrentRangesSwitchesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum VCVoltageRangesSwitches {
        VCVoltageRangesSwitchesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRange250mV,
        CCVoltageRangesNum
    };

    enum CCVoltageRangesSwitches {
        CCVoltageRangesSwitchesNum = 0
    };

    enum InputSwitches {
        InputSw,
        InputSwitchesNum
    };

    enum CalSwitches {
        CalSw,
        CalSwitchesNum
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
        VCReaderSwitch_CfastSw,
        VCReaderSwitch_ClampingMode0,
        VCReaderSwitchesNum
    };

    enum CCReaderSwitches {
        CCReaderSwitch_CcSW,
        CCReaderSwitch_GrEn,
        CCReaderSwitchesNum
    };

    enum SamplingRates {
        SamplingRate5kHz,
        SamplingRate10kHz,
        SamplingRate20kHz,
        SamplingRate40kHz,
        SamplingRate80kHz,
        SamplingRate160kHz,
        SamplingRatesNum
    };

    enum SamplingRatesSwitches {
        SamplingRateSwitchVcAdcFilter2,
        SamplingRateSwitchVcAdcFilter1,
        SamplingRateSwitchVcAdcFilter0,
        SamplingRateSwitchClockDiv1,
        SamplingRateSwitchClockDiv0,
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
        VCStimulusLpf20kHz,
        VCStimulusLpf50kHz,
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
        CCStimulusLpf20kHz,
        CCStimulusLpf50kHz,
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
        ConstantSwitchCh1RingEn,
        ConstantSwitchCh1PIn_En,
        ConstantSwitchCh2InSW,
        ConstantSwitchCh2CalibEn,
        ConstantSwitchCh2RingEn,
        ConstantSwitchCh2PIn_En,
        ConstantSwitchCh3InSW,
        ConstantSwitchCh3CalibEn,
        ConstantSwitchCh3RingEn,
        ConstantSwitchCh3PIn_En,
        ConstantSwitchCh4InSW,
        ConstantSwitchCh4CalibEn,
        ConstantSwitchCh4RingEn,
        ConstantSwitchCh4PIn_En,
        ConstantSwitchCh5InSW,
        ConstantSwitchCh5CalibEn,
        ConstantSwitchCh5RingEn,
        ConstantSwitchCh5PIn_En,
        ConstantSwitchCh6InSW,
        ConstantSwitchCh6CalibEn,
        ConstantSwitchCh6RingEn,
        ConstantSwitchCh6PIn_En,
        ConstantSwitchCh7InSW,
        ConstantSwitchCh7CalibEn,
        ConstantSwitchCh7RingEn,
        ConstantSwitchCh7PIn_En,
        ConstantSwitchCh8InSW,
        ConstantSwitchCh8CalibEn,
        ConstantSwitchCh8RingEn,
        ConstantSwitchCh8PIn_En,
        ConstantSwitchesNum
    };

    enum ConstantRegisters {
        ConstantRegisterOffsetCh1,
        ConstantRegisterOffsetCh2,
        ConstantRegisterOffsetCh3,
        ConstantRegisterOffsetCh4,
        ConstantRegisterOffsetCh5,
        ConstantRegisterOffsetCh6,
        ConstantRegisterOffsetCh7,
        ConstantRegisterOffsetCh8,
        ConstantRegistersNum
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
    std::vector <std::vector <bool> > samplingRatesSwitchesLut;

    std::string vcCurrentRangesSwitchesLutStrings[VCCurrentRangesNum];
    std::string ccCurrentRangesSwitchesLutStrings[CCCurrentRangesNum];
    std::string vcVoltageRangesSwitchesLutStrings[VCVoltageRangesNum];
    std::string ccVoltageRangesSwitchesLutStrings[CCVoltageRangesNum];
    std::string samplingRatesSwitchesLutStrings[SamplingRatesNum];
    std::string vcStimulusLpfSwitchesLutStrings[VCStimulusLpfsNum];
    std::string ccStimulusLpfSwitchesLutStrings[CCStimulusLpfsNum];
    std::string constantSwitchesLutStrings;
    std::string inputSwitchesLutStrings;
    std::string calSwitchesLutStrings;
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

    const double pipetteVarConductance = 320.0; // uS
    const double pipetteFixedResistance1 = 100.0e-3; // MOhm
    const double pipetteFixedResistance2 = 160.0e-3/27.0; // 5.92592 MOhm
    const double pipetteInjCapacitance1 = 3.0;
    const double pipetteInjCapacitance2 = 9.0;
    const double pipetteInjCapacitance3 = 27.0;
    const double pipetteCapacitanceSteps1 = 256.0;
    const double pipetteCapacitanceSteps2 = 256.0;
    const double pipetteCapacitanceSteps3 = 256.0;

    const double pipetteCapacitanceStep1 = pipetteVarConductance/pipetteCapacitanceSteps1*pipetteFixedResistance2*pipetteInjCapacitance1;
    const double minPipetteCapacitance1 = (pipetteVarConductance/pipetteCapacitanceSteps1+1.0/pipetteFixedResistance1)*pipetteFixedResistance2*pipetteInjCapacitance1;
    const double maxPipetteCapacitance1 = minPipetteCapacitance1+(pipetteCapacitanceSteps1-1.0)*pipetteCapacitanceStep1;
    const double pipetteCapacitanceStep2 = pipetteVarConductance/pipetteCapacitanceSteps2*pipetteFixedResistance2*pipetteInjCapacitance2;
    const double minPipetteCapacitance2 = (pipetteVarConductance/pipetteCapacitanceSteps2+1.0/pipetteFixedResistance1)*pipetteFixedResistance2*pipetteInjCapacitance2;
    const double maxPipetteCapacitance2 = minPipetteCapacitance2+(pipetteCapacitanceSteps2-1.0)*pipetteCapacitanceStep2;
    const double pipetteCapacitanceStep3 = pipetteVarConductance/pipetteCapacitanceSteps3*pipetteFixedResistance2*pipetteInjCapacitance3;
    const double minPipetteCapacitance3 = (pipetteVarConductance/pipetteCapacitanceSteps3+1.0/pipetteFixedResistance1)*pipetteFixedResistance2*pipetteInjCapacitance3;
    const double maxPipetteCapacitance3 = minPipetteCapacitance3+(pipetteCapacitanceSteps3-1.0)*pipetteCapacitanceStep3;

    /*! Using smaller resolution to cover both ranges */
    const double pipetteCapacitanceStep = pipetteCapacitanceStep1;
    const double minPipetteCapacitance = minPipetteCapacitance1;
    const double maxPipetteCapacitance = maxPipetteCapacitance3;
    const double pipetteCapacitanceSteps = (maxPipetteCapacitance-minPipetteCapacitance)/pipetteCapacitanceStep+1.0;
    const int pipetteCapacitanceDecimals = 2;
    const UnitPfx_t pipetteCapacitancePrefix = UnitPfxPico;
    const std::string pipetteCapacitanceUnit = "F";
    const std::string pipetteCapacitanceName = "Pipette\nCapacitance";

    const double membraneVarConductance = 320.0; // uS
    const double membraneFixedResistance8 = 61.5e-3; //MOhm
    const double membraneFixedResistance9 = 5.5e-3; //MOhm
    const int membraneCapValueRanges = 4;
    const double membraneCapacitanceValuesNum = 256.0;
    const double membraneInjCapacitance1 = 100.0/33.0;
    const double membraneInjCapacitance2 = 400.0/33.0;
    const double membraneInjCapacitance3 = 1600.0/33.0;
    const double membraneInjCapacitance4 = 1600.0/11.0;

    const double membraneCapacitanceStep1 = membraneVarConductance/membraneCapacitanceValuesNum*membraneFixedResistance9*membraneInjCapacitance1;
    const double minMembraneCapacitance1 = (membraneVarConductance/membraneCapacitanceValuesNum+1.0/membraneFixedResistance8)*membraneFixedResistance9*membraneInjCapacitance1;
    const double maxMembraneCapacitance1 = minMembraneCapacitance1+(membraneCapacitanceValuesNum-1.0)*minMembraneCapacitance1;
    const double membraneCapacitanceStep2 = membraneVarConductance/membraneCapacitanceValuesNum*membraneFixedResistance9*membraneInjCapacitance2;
    const double minMembraneCapacitance2 = (membraneVarConductance/membraneCapacitanceValuesNum+1.0/membraneFixedResistance8)*membraneFixedResistance9*membraneInjCapacitance2;
    const double maxMembraneCapacitance2 = minMembraneCapacitance2+(membraneCapacitanceValuesNum-1.0)*minMembraneCapacitance2;
    const double membraneCapacitanceStep3 = membraneVarConductance/membraneCapacitanceValuesNum*membraneFixedResistance9*membraneInjCapacitance3;
    const double minMembraneCapacitance3 = (membraneVarConductance/membraneCapacitanceValuesNum+1.0/membraneFixedResistance8)*membraneFixedResistance9*membraneInjCapacitance3;
    const double maxMembraneCapacitance3 = minMembraneCapacitance3+(membraneCapacitanceValuesNum-1.0)*minMembraneCapacitance3;
    const double membraneCapacitanceStep4 = membraneVarConductance/membraneCapacitanceValuesNum*membraneFixedResistance9*membraneInjCapacitance4;
    const double minMembraneCapacitance4 = (membraneVarConductance/membraneCapacitanceValuesNum+1.0/membraneFixedResistance8)*membraneFixedResistance9*membraneInjCapacitance4;
    const double maxMembraneCapacitance4 = minMembraneCapacitance4+(membraneCapacitanceValuesNum-1.0)*minMembraneCapacitance4;

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

#endif // EZPATCHE8PPATCHLINER_EL07CD_H
