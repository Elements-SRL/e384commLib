#ifndef EZPATCHE4PEL04F_H
#define EZPATCHE4PEL04F_H

#include "ezpatchftdidevice.h"

class EZPatche4PEL04F : public EZPatchFtdiDevice {
public:
    EZPatche4PEL04F(std::string di);

    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) override;

    ErrorCodes_t setResistancePredictionOptions(uint16_t optionIdx) override;
    ErrorCodes_t getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) override;

protected:
    enum ClampingModalities {
        VoltageClamp,
        ZeroCurrentClamp,
        CurrentClamp,
        ClampingModalitiesNum
    };

    enum VCCurrentRanges {
        VCCurrentRange300pA,
        VCCurrentRange3nA,
        VCCurrentRange30nA,
        VCCurrentRange300nA,
        VCCurrentRangesNum
    };

    enum VCCurrentRangesSwitches {
        VCCurrentRangesSwitchRange1,
        VCCurrentRangesSwitchRange2,
        VCCurrentRangesSwitchRange3,
        VCCurrentRangesSwitchRCorrRange1,
        VCCurrentRangesSwitchRCorrRange2,
        VCCurrentRangesSwitchResetPeriod0,
        VCCurrentRangesSwitchResetPeriod1,
        VCCurrentRangesSwitchResetDuration0,
        VCCurrentRangesSwitchResetDuration1,
        VCCurrentRangesSwitchesNum
    };

    enum CCCurrentRanges {
        CCCurrentRange2500pA,
        CCCurrentRange100nA,
        CCCurrentRangesNum
    };

    enum CCCurrentRangesSwitches {
        CCCurrentRangesSwitchRange1,
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
        CCVoltageRange700mV,
        CCVoltageRangesNum
    };

    enum CCVoltageRangesSwitches {
        CCVoltageRangesSwitchesNum = 0
    };

    enum VCStimulusSwitches {
        VCStimulusSwitch_VcEN2,
        VCStimulusSwitchesNum
    };

    enum CCStimulusSwitches {
        CCStimulusSwitch_StimEN,
        CCStimulusSwitchesNum
    };

    enum VCReaderSwitches {
        VCReaderSwitchesNum = 0
    };

    enum CCReaderSwitches {
        CCReaderSwitch_CcEN,
        CCReaderSwitchesNum
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

    enum SamplingRatesSwitches {
        SamplingRateSwitchThFilterBw3,
        SamplingRateSwitchThFilterBw2,
        SamplingRateSwitchThFilterBw1,
        SamplingRateSwitchThFilterBw0,
        SamplingRatesSwitchesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0,
        ChannelSourceVoltageFromCurrentClamp = 1,
        ChannelSourceCurrentFromVoltageClamp = 0,
        ChannelSourceCurrentFromCurrentClamp = 1
    };

    enum VCStimulusLpfs {
//        CCStimulusLpf10kHz,
        VCStimulusLpf1kHz,
        VCStimulusLpf100Hz,
        VCStimulusLpfsNum
    };

    enum VCStimulusLpfSwitches {
        VCStimulusLpfSwitchFilter1,
        VCStimulusLpfSwitchFilter2,
        VCStimulusLpfSwitchesNum
    };

    enum CCStimulusLpfs {
//        CCStimulusLpf10kHz,
        CCStimulusLpf1kHz,
        CCStimulusLpf100Hz,
        CCStimulusLpfsNum
    };

    enum CCStimulusLpfSwitches {
        CCStimulusLpfSwitchFilter1,
        CCStimulusLpfSwitchFilter2,
        CCStimulusLpfSwitchesNum
    };

    enum ConstantSwitches {
        ConstantSwitchExtCap,
        ConstantSwitchVcFilterOff,
        ConstantSwitchCalibEn,
        ConstantSwitchVcAdderEn,
        ConstantSwitchRingVc,
        ConstantSwitchStim1Out1,
        ConstantSwitchStim1Out2,
        ConstantSwitchStim2Out1,
        ConstantSwitchStim2Out2,
        ConstantSwitchResetForce0,
        ConstantSwitchResetForce1,
        ConstantSwitchCFastOpampEn,
        ConstantSwitchRPredOpampEn,
        ConstantSwitchCcStimOpampEN,
        ConstantSwitchSubtractorEn,
        ConstantSwitchPowerOn,
        ConstantSwitchEnNegVdda,
        ConstantSwitchEnSwitching,
        ConstantSwitchEnVddd,
        ConstantSwitchEnVdda,
        ConstantSwitchEnPosIsoPw,
        ConstantSwitchEn3V3Iso,
        ConstantSwitchEnNegIsoPw,
        ConstantSwitchLedBb,
        ConstantSwitchCh1VcEn1,
        ConstantSwitchCh1VcCorrBw, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchCh1PredRange, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchCh2VcEn1,
        ConstantSwitchCh2VcCorrBw, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchCh2PredRange, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchCh3VcEn1,
        ConstantSwitchCh3VcCorrBw, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchCh3PredRange, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchCh4VcEn1,
        ConstantSwitchCh4VcCorrBw, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchCh4PredRange, /*! \todo FCON non Ã¨ veramente costante */
        ConstantSwitchesNum
    };

    enum CompensationsSwitches {
        CompensationsSwitchCFastEn,
        CompensationsSwitchCSlowEn,
        CompensationsSwitchRCorrEn,
        CompensationsSwitchRPredEn,
        CompensationsSwitchRLeakEn,
        CompensationsSwitchCCFastEn,
        CompensationsSwitchBridgeDis,
        CompensationsSwitchesNum
    };

    enum CompensationsRegisters {
        CompensationsRegisterVCCFastGain = 4,
        CompensationsRegisterVCCSlowGain = 5,
        CompensationsRegisterVCCslowTau = 6,
        CompensationsRegisterVCRCorrGain = 7,
        CompensationsRegisterVCRPredTau = 8,
        CompensationsRegisterVCRLeakGain = 9,
        CompensationsRegisterCCCFastGain = 10,
        CompensationsRegisterCCBridgeGain = 11,
        CompensationsRegistersNum = 8
    };

    enum ChannelSources {
        ChannelSourceVoltage0 = 0,
        ChannelSourceCurrent0 = 0,
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
    std::string vcStimulusSwitchesLutStrings;
    std::string ccStimulusSwitchesLutStrings;
    std::string ccReaderSwitchesLutStrings;

    const double stimulusVoltageLimit = 0.5; /*! max voltage set for stimuli [V] */
    const double stimulusVoltageReference = 1.1; /*! voltage reference for stimuli [V] */

    /*! All of the following values are in pF (cap), MOhm (res), us (time) */

    const double liquidJunctionRange = 1100.0;
    const double liquidJunctionSteps = USHORT_MAX+1.0;
    const double liquidJunctionStep = 2.0*liquidJunctionRange/liquidJunctionSteps;
    const double minLiquidJunctionRange = -liquidJunctionRange;
    const double maxLiquidJunctionRange = minLiquidJunctionRange+(liquidJunctionSteps-1.0)*liquidJunctionStep;
    const int liquidJunctionDecimals = 1;
    const UnitPfx_t liquidJunctionPrefix = UnitPfxMilli;
    const std::string liquidJunctionUnit = "V";
    const std::string liquidJunctionName = "Offset";

    const double pipetteCapacitanceStep = 0.125;
    const double pipetteCapacitanceSteps = 256.0;
    const int pipetteCapacitanceDecimals = 2;
    const double minPipetteCapacitance = 0.0;
    const double maxPipetteCapacitance = minPipetteCapacitance+(pipetteCapacitanceSteps-1.0)*pipetteCapacitanceStep;
    const UnitPfx_t pipetteCapacitancePrefix = UnitPfxPico;
    const std::string pipetteCapacitanceUnit = "F";
    const std::string pipetteCapacitanceName = "Pipette\nCapacitance";

    const double membraneCapacitanceStep1 = 0.15625;
    const double membraneCapacitanceSteps1 = 256.0;
    const double minMembraneCapacitance1 = 5.0;
    const double maxMembraneCapacitance1 = minMembraneCapacitance1+(membraneCapacitanceSteps1-1.0)*membraneCapacitanceStep1;
    const double membraneCapacitanceStep2 = 1.25;
    const double membraneCapacitanceSteps2 = 256.0;
    const double minMembraneCapacitance2 = 40.0;
    const double maxMembraneCapacitance2 = minMembraneCapacitance2+(membraneCapacitanceSteps2-1.0)*membraneCapacitanceStep2;

    /*! Using smaller resolution to cover both ranges */
    const double membraneCapacitanceStep = membraneCapacitanceStep1;
    const double minMembraneCapacitance = minMembraneCapacitance1;
    const double maxMembraneCapacitance = maxMembraneCapacitance2;
    const double membraneCapacitanceSteps = (maxMembraneCapacitance-minMembraneCapacitance)/membraneCapacitanceStep+1.0;
    const int membraneCapacitanceDecimals = 1;
    const UnitPfx_t membraneCapacitancePrefix = UnitPfxPico;
    const std::string membraneCapacitanceUnit = "F";
    const std::string membraneCapacitanceName = "Membrane\nCapacitance";

    const UnitPfx_t accessResistancePrefix = UnitPfxMega;
    const std::string accessResistanceUnit = "Ohm";
    const std::string accessResistanceName = "Access\nResistance";

    const double membraneTauStep = 6.25;
    const double membraneTauSteps = 256.0;
    const double minMembraneTau = 0.0;
    const double maxMembraneTau = minMembraneTau+(membraneTauSteps-1.0)*membraneTauStep;

    const double resistanceCorrectionPercentageSteps = 256.0;
    const double minResistanceCorrection = 0.0;
    const double resistanceCorrectionStep = 0.1;
    const double maxResistanceCorrection = (resistanceCorrectionPercentageSteps-1.0)*resistanceCorrectionStep;
    const double minResistanceCorrectionPercentage = 0.0;
    const double maxResistanceCorrectionPercentage = 100.0;
    const UnitPfx_t resistanceCorrectionPercentagePrefix = UnitPfxNone;
    const std::string resistanceCorrectionPercentageUnit = "%";
    const std::string resistanceCorrectionPercentageName = "Correction\nPercentage";

    const double resistancePredictionGain0 = 2.0;
    const double resistancePredictionGain1 = 3.0;
    const double minResistancePredictionPercentage = 0.0;
    const double maxResistancePredictionPercentage = 100.0;
    const UnitPfx_t resistancePredictionPercentagePrefix = UnitPfxNone;
    const std::string resistancePredictionPercentageUnit = "%";
    const std::string resistancePredictionPercentageName = "Prediction\nPercentage";

    const double resistancePredictionTauStep = 10.0;
    const double resistancePredictionTauSteps = 256.0;
    const int resistancePredictionTauDecimals = 1;
    const double minResistancePredictionTau = 0.0;
    const double maxResistancePredictionTau = minResistancePredictionTau+(resistancePredictionTauSteps-1.0)*resistancePredictionTauStep;
    const UnitPfx_t resistancePredictionTauPrefix = UnitPfxMicro;
    const std::string resistancePredictionTauUnit = "s";
    const std::string resistancePredictionTauName = "Prediction\nTau";

    const double minLeakConductanceLow = 0.0;
    const double leakConductanceLowSteps = 256.0;
    const double leakConductanceLowStep = 0.625/8.0;
    const double maxLeakConductanceLow = minLeakConductanceLow+(leakConductanceLowSteps-1.0)*leakConductanceLowStep;
    const int leakConductanceLowDecimals = 2;
    const UnitPfx_t leakConductanceLowPrefix = UnitPfxNano;
    const std::string leakConductanceLowUnit = "S";
    const std::string leakConductanceLowName = "Leak\nConductance";

    const double minLeakConductanceHigh = 0.0;
    const double leakConductanceHighSteps = 256.0;
    const double leakConductanceHighStep = 6.25/8.0;
    const double maxLeakConductanceHigh = minLeakConductanceHigh+(leakConductanceHighSteps-1.0)*leakConductanceHighStep;
    const int leakConductanceHighDecimals = 1;
    const UnitPfx_t leakConductanceHighPrefix = UnitPfxNano;
    const std::string leakConductanceHighUnit = "S";
    const std::string leakConductanceHighName = "Leak\nConductance";

    const double ccPipetteCapacitanceStep = 0.125;
    const double ccPipetteCapacitanceSteps = 256.0;
    const int ccPipetteCapacitanceDecimals = 2;
    const double minCcPipetteCapacitance = 0.0;
    const double maxCcPipetteCapacitance = minCcPipetteCapacitance+(ccPipetteCapacitanceSteps-1.0)*ccPipetteCapacitanceStep;
    const UnitPfx_t ccPipetteCapacitancePrefix = UnitPfxPico;
    const std::string ccPipetteCapacitanceUnit = "F";
    const std::string ccPipetteCapacitanceName = "Pipette\nCapacitance";

    const double minBridgeBalanceResistance = 0.0;
    const double maxBridgeBalanceResistance = 40.0;
    const double bridgeBalanceResistanceSteps = 256.0;
    const double bridgeBalanceResistanceStep = (maxBridgeBalanceResistance-minBridgeBalanceResistance)/(bridgeBalanceResistanceSteps-1.0);
    const int bridgeBalanceResistanceDecimals = 2;
    const UnitPfx_t bridgeBalanceResistancePrefix = UnitPfxMega;
    const std::string bridgeBalanceResistanceUnit = "Ohm";
    const std::string bridgeBalanceResistanceName = "Bridge\nResistance";

    /*! Compensations values succesfully written to the device (opposite from default values, this way the initialization write works, because the values must be updated) */
    std::vector <uint16_t> pipetteCapacitanceRegValue;
    std::vector <uint16_t> membraneCapacitanceRegValue;
    std::vector <uint16_t> membraneTauRegValue;
    std::vector <uint16_t> correctionGainRegValue;
    std::vector <uint16_t> predictionTauRegValue;
    std::vector <uint16_t> leakConductanceRegValue;
    std::vector <uint16_t> ccPipetteCapacitanceRegValue;
    std::vector <uint16_t> bridgeBalanceResistanceRegValue;

    CompensationControl_t leakConductanceControlLow;
    CompensationControl_t leakConductanceControlHigh;
};

#endif // EZPATCHE4PEL04F_H
