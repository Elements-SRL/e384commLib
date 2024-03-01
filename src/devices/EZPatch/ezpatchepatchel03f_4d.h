#ifndef EZPATCHEPATCHEL03F_4D_H
#define EZPATCHEPATCHEL03F_4D_H

#include "ezpatchftdidevice.h"

class EZPatchePatchEL03F_4D_PCBV03_V04 : public EZPatchFtdiDevice {
public:
    EZPatchePatchEL03F_4D_PCBV03_V04(std::string di);
    virtual ~EZPatchePatchEL03F_4D_PCBV03_V04();

    ErrorCodes_t setResistancePredictionOptions(uint16_t optionIdx) override;

    ErrorCodes_t hasCompFeature(CompensationUserParams feature) override;

protected:
    enum ClampingModalities {
        VoltageClamp,
        ZeroCurrentClamp,
        CurrentClamp,
        ClampingModalitiesNum
    };

    enum VCCurrentRanges {
        VCCurrentRange200pA,
        VCCurrentRange2nA,
        VCCurrentRange20nA,
        VCCurrentRange200nA,
        VCCurrentRangesNum
    };

    enum VCCurrentRangesSwitches {
        VCCurrentRangesSwitchRange1,
        VCCurrentRangesSwitchRange2,
        VCCurrentRangesSwitchRange3,
        VCCurrentRangesSwitchRCorrRange1,
        VCCurrentRangesSwitchRCorrRange2,
        VCCurrentRangesSwitchRCorrRange3,
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
        VCStimulusSwitch_Input,
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
        CCReaderSwitchesNum = 0
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

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0,
        ChannelSourceVoltageFromCurrentClamp = 1,
        ChannelSourceCurrentFromVoltageClamp = 0,
        ChannelSourceCurrentFromCurrentClamp = 1
    };

    enum VCStimulusLpfs {
        VCStimulusLpfNoFilter, /*!< The other modality probably creates a partitor that cripples the applied Vc */
        VCStimulusLpfsNum
    };

    enum VCStimulusLpfSwitches {
        VCStimulusLpfSwitchFilterOff,
        VCStimulusLpfSwitchExtCap,
        VCStimulusLpfSwitchesNum
    };

    enum CCStimulusLpfs {
        CCStimulusLpf10kHz,
//        CCStimulusLpf1kHz,
//        CCStimulusLpf100Hz,
        CCStimulusLpfsNum
    };

    enum CCStimulusLpfSwitches {
        CCStimulusLpfSwitchFilter1,
        CCStimulusLpfSwitchFilter2,
        CCStimulusLpfSwitchesNum
    };

    enum ConstantSwitches {
        ConstantSwitchVcAdderEn,
        ConstantSwitchVcInt,
        ConstantSwitchCcInput,
        ConstantSwitchCcDacDbg,
        ConstantSwitchVcmForce,
        ConstantSwitchVcCorrBw, /*! \todo FCON non è veramente costante */
        ConstantSwitchPredRange, /*! \todo FCON non è veramente costante */
        ConstantSwitchesNum
    };

    enum CompensationsSwitches {
        CompensationsSwitchCFastEn,
        CompensationsSwitchCSlowEn,
        CompensationsSwitchRCorrEn,
        CompensationsSwitchRPredEn,
        CompensationsSwitchCCFastEn,
        CompensationsSwitchBridgeDis,
        CompensationsSwitchesNum
    };

    enum CompensationsRegisters {
        CompensationsRegisterVCCFastGain = 0,
        CompensationsRegisterVCCSlowGain = 1,
        CompensationsRegisterVCCslowTau = 2,
        CompensationsRegisterVCRCorrGain = 3,
        CompensationsRegisterVCRPredTau = 4,
        CompensationsRegisterCCCFastGain = 5,
        CompensationsRegisterCCBridgeGain = 6,
        CompensationsRegistersNum
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

    std::string vcCurrentRangesSwitchesLutStrings[VCCurrentRangesNum];
    std::string ccCurrentRangesSwitchesLutStrings[CCCurrentRangesNum];
    std::string vcVoltageRangesSwitchesLutStrings[VCVoltageRangesNum];
    std::string ccVoltageRangesSwitchesLutStrings[CCVoltageRangesNum];
    std::string vcStimulusLpfSwitchesLutStrings[VCStimulusLpfsNum];
    std::string ccStimulusLpfSwitchesLutStrings[CCStimulusLpfsNum];
    std::string constantSwitchesLutStrings;
    std::string vcStimulusSwitchesLutStrings;
    std::string ccStimulusSwitchesLutStrings;

    /*! All of the following values are in pF (cap), MOhm (res), us (time) */

    const double liquidJunctionRange = 50.0;
    const double liquidJunctionSteps = 1024.0;
    const double liquidJunctionStep = 2.0*liquidJunctionRange/liquidJunctionSteps;
    const double minLiquidJunctionRange = -liquidJunctionRange;
    const double maxLiquidJunctionRange = minLiquidJunctionRange+(liquidJunctionSteps-1.0)*liquidJunctionStep;
    const int liquidJunctionDecimals = 2;
    const UnitPfx_t liquidJunctionPrefix = UnitPfxMilli;
    const std::string liquidJunctionUnit = "V";
    const std::string liquidJunctionName = "Offset";

    const double pipetteCapacitanceStep = 0.75;
    const double pipetteCapacitanceSteps = 16.0;
    const int pipetteCapacitanceDecimals = 2;
    const double minPipetteCapacitance = 0.0;
    const double maxPipetteCapacitance = minPipetteCapacitance+(pipetteCapacitanceSteps-1.0)*pipetteCapacitanceStep;
    const UnitPfx_t pipetteCapacitancePrefix = UnitPfxPico;
    const std::string pipetteCapacitanceUnit = "F";
    const std::string pipetteCapacitanceName = "Pipette\nCapacitance";

    const double membraneCapacitanceStep = 1.0;
    const double membraneCapacitanceSteps = 256.0;
    const int membraneCapacitanceDecimals = 1;
    const double minMembraneCapacitance = 0.0;
    const double maxMembraneCapacitance = minMembraneCapacitance+(membraneCapacitanceSteps-1.0)*membraneCapacitanceStep;
    const UnitPfx_t membraneCapacitancePrefix = UnitPfxPico;
    const std::string membraneCapacitanceUnit = "F";
    const std::string membraneCapacitanceName = "Membrane\nCapacitance";

    const UnitPfx_t accessResistancePrefix = UnitPfxMega;
    const std::string accessResistanceUnit = "Ohm";
    const std::string accessResistanceName = "Access\nResistance";

    const double membraneTauCap = 3300.0;
    const double membraneTauRStep = 3.0e-3;
    const double membraneTauStep = membraneTauCap*membraneTauRStep;
    const double membraneTauSteps = 256.0;
    const double minMembraneTau = 0.0;
    const double maxMembraneTau = minMembraneTau+(membraneTauSteps-1.0)*membraneTauStep;

    const double resistanceCorrectionPercentageSteps = 256.0;
    const double transImpendance = 225.0;
    const double transImpendanceNorm = 6.8;
    const double resistanceCorrectionRStep = 3.0e-3;
    const double minResistanceCorrection = 0.0;
    const double resistanceCorrectionStep = transImpendance/transImpendanceNorm*resistanceCorrectionRStep;
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

    const double resistancePredictionTauRStep = 3.0e-3;
    const double resistancePredictionTauCap = 1.0e3;
    const double resistancePredictionTauStep = resistancePredictionTauCap*resistancePredictionTauRStep;
    const double resistancePredictionTauSteps = 256.0;
    const int resistancePredictionTauDecimals = 1;
    const double minResistancePredictionTau = 0.0;
    const double maxResistancePredictionTau = minResistancePredictionTau+(resistancePredictionTauSteps-1.0)*resistancePredictionTauStep;
    const UnitPfx_t resistancePredictionTauPrefix = UnitPfxMicro;
    const std::string resistancePredictionTauUnit = "s";
    const std::string resistancePredictionTauName = "Prediction\nTau";

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
    std::vector <uint16_t> ccPipetteCapacitanceRegValue;
    std::vector <uint16_t> bridgeBalanceResistanceRegValue;
};

class EZPatchePatchEL03F_4D_PCBV03_V03 : public EZPatchePatchEL03F_4D_PCBV03_V04 {
public:
    EZPatchePatchEL03F_4D_PCBV03_V03(std::string di);

    virtual ~EZPatchePatchEL03F_4D_PCBV03_V03();
};

class EZPatchePatchEL03F_4D_PCBV03_V02 : public EZPatchePatchEL03F_4D_PCBV03_V03 {
public:
    EZPatchePatchEL03F_4D_PCBV03_V02(std::string di);

    virtual ~EZPatchePatchEL03F_4D_PCBV03_V02();
};

class EZPatchePatchEL03F_4D_PCBV02_V04 : public EZPatchePatchEL03F_4D_PCBV03_V04 {
public:
    EZPatchePatchEL03F_4D_PCBV02_V04(std::string di);

    virtual ~EZPatchePatchEL03F_4D_PCBV02_V04();
};

class EZPatchePatchEL03F_4D_PCBV02_V03 : public EZPatchePatchEL03F_4D_PCBV03_V03 {
public:
    EZPatchePatchEL03F_4D_PCBV02_V03(std::string di);

    virtual ~EZPatchePatchEL03F_4D_PCBV02_V03();
};

class EZPatchePatchEL03F_4D_PCBV02_V02 : public EZPatchePatchEL03F_4D_PCBV03_V02 {
public:
    EZPatchePatchEL03F_4D_PCBV02_V02(std::string di);

    virtual ~EZPatchePatchEL03F_4D_PCBV02_V02();
};

class EZPatchePatchEL03F_4D_PCBV02_V01 : public EZPatchePatchEL03F_4D_PCBV02_V02 {
public:
    EZPatchePatchEL03F_4D_PCBV02_V01(std::string di);

    virtual ~EZPatchePatchEL03F_4D_PCBV02_V01();
};

#endif // EZPATCHEPATCHEL03F_4D_H
