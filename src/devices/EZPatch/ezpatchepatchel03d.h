#ifndef EZPATCHEPATCHEL03D_H
#define EZPATCHEPATCHEL03D_H

#include "ezpatchftdidevice.h"

class EZPatchePatchEL03D_V04 : public EZPatchFtdiDevice {
public:
    EZPatchePatchEL03D_V04(std::string di);

    virtual ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) override;

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum VCCurrentRanges {
        VCCurrentRange200pA,
        VCCurrentRange2nA,
        VCCurrentRange20nA,
        VCCurrentRange200nA,
        VCCurrentRangesNum
    };

    /*! Front-end impendance in MOhm depends on current range selected */
    const double transImpedance[VCCurrentRangesNum] = {2250.0, 225.0, 22.5, 2.25};

    enum VCCurrentRangesSwitches {
        VCCurrentRangesSwitchRange1,
        VCCurrentRangesSwitchRange2,
        VCCurrentRangesSwitchRange3,
        VCCurrentRangesSwitchesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
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
        CCVoltageRangesNum = 0
    };

    enum CCVoltageRangesSwitches {
        CCVoltageRangesSwitchesNum = 0
    };

    enum VCStimulusSwitches {
        VCStimulusSwitchesNum = 0
    };

    enum CCStimulusSwitches {
        CCStimulusSwitchesNum = 0
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

    enum VCStimulusLpfs {
        VCStimulusLpfNoFilter, /*!< The other modality probably messes the compensations synchronization */
        VCStimulusLpfsNum
    };

    enum VCStimulusLpfSwitches {
        VCStimulusLpfSwitchEXT_cap,
        VCStimulusLpfSwitchesNum
    };

    enum CCStimulusLpfs {
        CCStimulusLpfsNum = 0
    };

    enum CCStimulusLpfSwitches {
        CCStimulusLpfSwitchesNum = 0
    };

    enum ConstantSwitches {
        ConstantSwitchesNum = 0
    };

    enum CompensationsSwitches {
        CompensationsSwitchCFastEn,
        CompensationsSwitchAuxCSlow,
        CompensationsSwitchVcInt,
        CompensationsSwitchAnOut,
        CompensationsSwitchVcPin,
        CompensationsSwitchExtCapCSlow,
        CompensationsSwitchExtCap2CSlow,
        CompensationsSwitchRsIn,
        CompensationsSwitchVcmForce2,
        CompensationsSwitchesNum
    };

    enum CompensationsRegisters {
        CompensationsRegisterAmpCFast = 0,
        CompensationsRegisterExtAmpCSlow = 1,
        CompensationsRegisterExtDacCSlow = 2,
        CompensationsRegisterOdacRs = 3,
        CompensationsRegisterRsLag = 4,
        CompensationsRegisterRsPrediction = 5,
        CompensationsRegistersNum
    };

    enum CompensationsCombos {
        CompensationsComboNone = 0,
        CompensationsComboPipette = 1,
        CompensationsComboMembrane = 2,
        CompensationsComboMembranePipette = 3,
        CompensationsComboResistance = 4,
        CompensationsComboResistancePipette = 5,
        CompensationsComboResistanceMembrane = 6,
        CompensationsComboResistanceMembranePipette = 7,
        CompensationsComboNum
    };

    void selectChannelsResolutions() override;
    void selectVoltageOffsetResolution() override;
    void initializeCompensations() override;
    bool checkCompensationsValues() override;
    bool fillCompensationsRegistersTxData(std::vector <uint16_t> &txDataMessage) override;
    void updateWrittenCompensationValues(std::vector <uint16_t> &txDataMessage) override;
    void compensationsFlags2Switches(std::vector <uint16_t> &txDataMessage) override;

    std::string vcCurrentRangesSwitchesLutStrings[VCCurrentRangesNum];
    std::string vcVoltageRangesSwitchesLutStrings[VCVoltageRangesNum];
    std::string vcStimulusLpfSwitchesLutStrings[VCStimulusLpfsNum];
    std::string compensationsComboSwitchesLutStrings[CompensationsComboNum];

    std::vector <bool> compensationsComboSwitchesLut[CompensationsComboNum];

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

    const double membraneCapacitanceSteps = 256.0;
    const double membraneCapacitanceRStep = 0.1/membraneCapacitanceSteps;
    const int membraneCapacitanceDecimals = 1;
    const double membraneCapacitanceRSeries = 60.0e-6;
    const double membraneCapacitanceAmpRes = 9.09e-3;
    const double membraneCapacitanceInjCap = 23;
    const double membraneCapacitanceStep = membraneCapacitanceInjCap*membraneCapacitanceRStep/membraneCapacitanceAmpRes;
    const double minMembraneCapacitance = membraneCapacitanceInjCap*membraneCapacitanceRSeries/membraneCapacitanceAmpRes;
    const double maxMembraneCapacitance = minMembraneCapacitance+(membraneCapacitanceSteps-1.0)*membraneCapacitanceStep;
    const UnitPfx_t membraneCapacitancePrefix = UnitPfxPico;
    const std::string membraneCapacitanceUnit = "F";
    const std::string membraneCapacitanceName = "Membrane\nCapacitance";

    const UnitPfx_t accessResistancePrefix = UnitPfxMega;
    const std::string accessResistanceUnit = "Ohm";
    const std::string accessResistanceName = "Access\nResistance";

    const double membraneTauSteps = 256.0;
    const double membraneTauRStep = 0.1/membraneTauSteps;
    const double membraneTauRSeries = 60.0e-6;
    const double membraneTauCap = 33000.0;
    const double membraneTauStep = membraneTauCap*membraneTauRStep;
    const double minMembraneTau = membraneTauCap*membraneTauRSeries;
    const double maxMembraneTau = minMembraneTau+(membraneTauSteps-1.0)*membraneTauStep;

    const double minResistanceCorrection = 0.0;
    inline double maxResistanceCorrection() const {
        return transImpedance[selectedVcCurrentRangeIdx];
    }

    const double maxResistanceCorrectionPercentage = 100.0;
    const double resistanceCorrectionPercentageSteps = 1024.0;
    const double resistanceCorrectionPercentageStep = maxResistanceCorrectionPercentage/(resistanceCorrectionPercentageSteps-1.0);
    const UnitPfx_t resistanceCorrectionPercentagePrefix = UnitPfxNone;
    const std::string resistanceCorrectionPercentageUnit = "%";
    const std::string resistanceCorrectionPercentageName = "Correction\nPercentage";

    const double resistanceCorrectionLagSteps = 256.0;
    const double resistanceCorrectionLagRStep = 0.1/resistanceCorrectionLagSteps;
    const int resistanceCorrectionLagDecimals = 1;
    const double resistanceCorrectionLagRSeries = 60.0e-6;
    const double resistanceCorrectionLagCap = 10000.0;
    const double resistanceCorrectionLagStep = resistanceCorrectionLagCap*resistanceCorrectionLagRStep;
    const double minResistanceCorrectionLag = resistanceCorrectionLagCap*resistanceCorrectionLagRSeries;
    const double maxResistanceCorrectionLag = minResistanceCorrectionLag+(resistanceCorrectionLagSteps-1.0)*resistanceCorrectionLagStep;
    const UnitPfx_t resistanceCorrectionLagPrefix = UnitPfxMicro;
    const std::string resistanceCorrectionLagUnit = "s";
    const std::string resistanceCorrectionLagName = "Correction\nLag";

    const double maxResistancePredictionPercentage = 100.0;
    const double resistancePredictionTauSteps = 256.0;
    const double resistancePredictionTauRStep = 0.1/resistancePredictionTauSteps;
    const double resistancePredictionTauRSeries = 60.0e-6;
    const double resistancePredictionTauCap = 10000.0;
    const double resistancePredictionTauStep = resistancePredictionTauCap*resistancePredictionTauRStep;
    const double minResistancePredictionTau = resistancePredictionTauCap*resistancePredictionTauRSeries;
    const double maxResistancePredictionTau = minResistancePredictionTau+(resistancePredictionTauSteps-1.0)*resistancePredictionTauStep;

    const UnitPfx_t resistancePredictionPercentagePrefix = UnitPfxNone;
    const std::string resistancePredictionPercentageUnit = "%";
    const std::string resistancePredictionPercentageName = "Prediction\nPercentage";

    /*! Compensations values succesfully written to the device (opposite from default values, this way the initialization write works, because the values must be updated) */
    std::vector <uint16_t> pipetteCapacitanceRegValue;
    std::vector <uint16_t> membraneCapacitanceRegValue;
    std::vector <uint16_t> membraneTauRegValue;
    std::vector <uint16_t> correctionGainRegValue;
    std::vector <uint16_t> correctionLagRegValue;
    std::vector <uint16_t> predictionTauRegValue;
};

class EZPatchePatchEL03D_V03 : public EZPatchePatchEL03D_V04 {
public:
    EZPatchePatchEL03D_V03(std::string di);
};

class EZPatchePatchEL03D_V02 : public EZPatchePatchEL03D_V03 {
public:
    EZPatchePatchEL03D_V02(std::string di);
};

class EZPatchePatchEL03D_V01 : public EZPatchePatchEL03D_V02 {
public:
    EZPatchePatchEL03D_V01(std::string di);
};

class EZPatchePatchEL03D_V00 : public EZPatchePatchEL03D_V01 {
public:
    EZPatchePatchEL03D_V00(std::string di);
};

class EZPatchePatchDlp : public EZPatchePatchEL03D_V03 {
public:
    EZPatchePatchDlp(std::string di);
};

#endif // EZPATCHEPATCHEL03D_H
