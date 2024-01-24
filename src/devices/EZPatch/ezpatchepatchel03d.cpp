#include "ezpatchepatchel03d.h"

#include <math.h>
#include <algorithm>

EZPatchePatchEL03D_V04::EZPatchePatchEL03D_V04(std::string di) :
    EZPatchFtdiDevice(di) {

    deviceName = "ePatchEl03D";

    /*! Clamping modalities */
    clampingModalitiesArray.resize(1);
    clampingModalitiesArray[0] = VOLTAGE_CLAMP;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange200pA].max = 200.0;
    vcCurrentRangesArray[VCCurrentRange200pA].min = -200.0;
    vcCurrentRangesArray[VCCurrentRange200pA].step = vcCurrentRangesArray[VCCurrentRange200pA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange200pA].prefix = UnitPfxPico;
    vcCurrentRangesArray[VCCurrentRange200pA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange2nA].max = 2.0;
    vcCurrentRangesArray[VCCurrentRange2nA].min = -2.0;
    vcCurrentRangesArray[VCCurrentRange2nA].step = vcCurrentRangesArray[VCCurrentRange2nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange2nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange2nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange20nA].max = 20.0;
    vcCurrentRangesArray[VCCurrentRange20nA].min = -20.0;
    vcCurrentRangesArray[VCCurrentRange20nA].step = vcCurrentRangesArray[VCCurrentRange20nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange20nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange20nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange200nA].max = 200.0;
    vcCurrentRangesArray[VCCurrentRange200nA].min = -200.0;
    vcCurrentRangesArray[VCCurrentRange200nA].step = vcCurrentRangesArray[VCCurrentRange200nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange200nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange200nA].unit = "A";

    vcCurrentRangesSwitchesLut.resize(vcCurrentRangesNum);

    vcCurrentRangesSwitchesNum = VCCurrentRangesSwitchesNum;
    vcCurrentRangesSwitchesWord.resize(vcCurrentRangesSwitchesNum);
    vcCurrentRangesSwitchesByte.resize(vcCurrentRangesSwitchesNum);

    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange1] = 0;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange2] = 0;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange3] = 0;

    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange1] = 0x0800;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange2] = 0x1000;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange3] = 0x2000;

    vcCurrentRangesSwitchesLutStrings[VCCurrentRange200pA] = "000";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange2nA] = "010";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange20nA] = "110";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange200nA] = "111";

    for (unsigned int currentRangesIdx = 0; currentRangesIdx < vcCurrentRangesNum; currentRangesIdx++) {
        vcCurrentRangesSwitchesLut[currentRangesIdx].resize(vcCurrentRangesSwitchesNum);
        for (unsigned int currentRangesSwitchesIdx = 0; currentRangesSwitchesIdx < vcCurrentRangesSwitchesNum; currentRangesSwitchesIdx++) {
            if (vcCurrentRangesSwitchesLutStrings[currentRangesIdx][currentRangesSwitchesIdx] == '1') {
                vcCurrentRangesSwitchesLut[currentRangesIdx][currentRangesSwitchesIdx] = true;

            } else {
                vcCurrentRangesSwitchesLut[currentRangesIdx][currentRangesSwitchesIdx] = false;
            }
        }
    }

    /*! CC */
    ccCurrentRangesNum = CCCurrentRangesNum;
    ccCurrentRangesArray.resize(1); /*! CC current ranges must have at least 1 item */

    ccCurrentRangesArray[0].max = 1.0;
    ccCurrentRangesArray[0].min = 1.0;
    ccCurrentRangesArray[0].step = 1.0;
    ccCurrentRangesArray[0].prefix = UnitPfxNone;
    ccCurrentRangesArray[0].unit = "A";

    ccCurrentRangesSwitchesLut.resize(ccCurrentRangesNum);

    ccCurrentRangesSwitchesNum = CCCurrentRangesSwitchesNum;
    ccCurrentRangesSwitchesWord.resize(ccCurrentRangesSwitchesNum);
    ccCurrentRangesSwitchesByte.resize(ccCurrentRangesSwitchesNum);

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].max = 500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].min = -500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 1.0;
    vcVoltageRangesArray[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange500mV].unit = "V";

    vcVoltageRangesSwitchesLut.resize(vcVoltageRangesNum);

    vcVoltageRangesSwitchesNum = VCVoltageRangesSwitchesNum;
    vcVoltageRangesSwitchesWord.resize(vcVoltageRangesSwitchesNum);
    vcVoltageRangesSwitchesByte.resize(vcVoltageRangesSwitchesNum);

    /*! No words/bytes to set */
    vcVoltageRangesSwitchesLutStrings[VCVoltageRange500mV] = "";

    for (unsigned int voltageRangesIdx = 0; voltageRangesIdx < vcVoltageRangesNum; voltageRangesIdx++) {
        vcVoltageRangesSwitchesLut[voltageRangesIdx].resize(vcVoltageRangesSwitchesNum);
        for (unsigned int voltageRangesSwitchesIdx = 0; voltageRangesSwitchesIdx < vcVoltageRangesSwitchesNum; voltageRangesSwitchesIdx++) {
            if (vcVoltageRangesSwitchesLutStrings[voltageRangesIdx][voltageRangesSwitchesIdx] == '1') {
                vcVoltageRangesSwitchesLut[voltageRangesIdx][voltageRangesSwitchesIdx] = true;

            } else {
                vcVoltageRangesSwitchesLut[voltageRangesIdx][voltageRangesSwitchesIdx] = false;
            }
        }
    }

    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;
    ccVoltageRangesArray.resize(ccVoltageRangesNum);

    ccVoltageRangesSwitchesLut.resize(ccVoltageRangesNum);

    ccVoltageRangesSwitchesNum = CCVoltageRangesSwitchesNum;
    ccVoltageRangesSwitchesWord.resize(ccVoltageRangesSwitchesNum);
    ccVoltageRangesSwitchesByte.resize(ccVoltageRangesSwitchesNum);

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate1_25kHz].value = 1.25;
    samplingRatesArray[SamplingRate1_25kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate1_25kHz].unit = "Hz";
    samplingRatesArray[SamplingRate5kHz].value = 5.0;
    samplingRatesArray[SamplingRate5kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate5kHz].unit = "Hz";
    samplingRatesArray[SamplingRate10kHz].value = 10.0;
    samplingRatesArray[SamplingRate10kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate10kHz].unit = "Hz";
    samplingRatesArray[SamplingRate20kHz].value = 20.0;
    samplingRatesArray[SamplingRate20kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate20kHz].unit = "Hz";
    samplingRatesArray[SamplingRate50kHz].value = 50.0;
    samplingRatesArray[SamplingRate50kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate50kHz].unit = "Hz";
    samplingRatesArray[SamplingRate100kHz].value = 100.0;
    samplingRatesArray[SamplingRate100kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate100kHz].unit = "Hz";
    samplingRatesArray[SamplingRate200kHz].value = 200.0;
    samplingRatesArray[SamplingRate200kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate200kHz].unit = "Hz";

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate1_25kHz].value = 1.25e3/1024.0;
    realSamplingRatesArray[SamplingRate1_25kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate1_25kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate5kHz].value = 1.25e3/256.0;
    realSamplingRatesArray[SamplingRate5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate10kHz].value = 1.25e3/128.0;
    realSamplingRatesArray[SamplingRate10kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate10kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate20kHz].value = 1.25e3/64.0;
    realSamplingRatesArray[SamplingRate20kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate20kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate50kHz].value = 50.0;
    realSamplingRatesArray[SamplingRate50kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate50kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate100kHz].value = 100.0;
    realSamplingRatesArray[SamplingRate100kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate100kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate200kHz].value = 200.0;
    realSamplingRatesArray[SamplingRate200kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate200kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate1_25kHz].value = 1024.0/1.25;
    integrationStepArray[SamplingRate1_25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_25kHz].unit = "s";
    integrationStepArray[SamplingRate5kHz].value = 256.0/1.25;
    integrationStepArray[SamplingRate5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5kHz].unit = "s";
    integrationStepArray[SamplingRate10kHz].value = 128.0/1.25;
    integrationStepArray[SamplingRate10kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate10kHz].unit = "s";
    integrationStepArray[SamplingRate20kHz].value = 64.0/1.25;
    integrationStepArray[SamplingRate20kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate20kHz].unit = "s";
    integrationStepArray[SamplingRate50kHz].value = 20.0;
    integrationStepArray[SamplingRate50kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate50kHz].unit = "s";
    integrationStepArray[SamplingRate100kHz].value = 10.0;
    integrationStepArray[SamplingRate100kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate100kHz].unit = "s";
    integrationStepArray[SamplingRate200kHz].value = 5.0;
    integrationStepArray[SamplingRate200kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate200kHz].unit = "s";

    currentChannelsNum = 1;
    voltageChannelsNum = 1;
    totalChannelsNum = currentChannelsNum+voltageChannelsNum;

    /*! HW Switches */
    switchesStatusLength = 2;

    switchesStatus.resize(switchesStatusLength);

    switchesNames.resize(switchesStatusLength);
    switchesNames[0].resize(16);
    switchesNames[0][0] = "CfastEN";
    switchesNames[0][1] = "AUX_Cslow";
    switchesNames[0][2] = "ExtCAP_Cslow";
    switchesNames[0][3] = "ExtCAP2_Cslow";
    switchesNames[0][4] = "VcInt";
    switchesNames[0][5] = "VcPIN";
    switchesNames[0][6] = "RsIN";
    switchesNames[0][7] = "AN_OUT";
    switchesNames[0][8] = "VcmForce1";
    switchesNames[0][9] = "VcmForce2";
    switchesNames[0][10] = "Ext_Cap";
    switchesNames[0][11] = "Range1";
    switchesNames[0][12] = "Range2";
    switchesNames[0][13] = "Range3";
    switchesNames[0][14] = "resetChip";
    switchesNames[0][15] = "resetDComp";

    switchesNames[1].resize(16);
    switchesNames[1][0] = "free";
    switchesNames[1][1] = "FPGA_OK";
    switchesNames[1][2] = "free";
    switchesNames[1][3] = "free";
    switchesNames[1][4] = "free";
    switchesNames[1][5] = "free";
    switchesNames[1][6] = "free";
    switchesNames[1][7] = "free";
    switchesNames[1][8] = "free";
    switchesNames[1][9] = "free";
    switchesNames[1][10] = "free";
    switchesNames[1][11] = "free";
    switchesNames[1][12] = "free";
    switchesNames[1][13] = "free";
    switchesNames[1][14] = "free";
    switchesNames[1][15] = "free";

    /*! LEDs */
    ledsNum = 1;
    ledsWord.resize(ledsNum);
    ledsByte.resize(ledsNum);
    ledsColorsArray.resize(ledsNum);

    ledsWord[0] = 1;
    ledsByte[0] = 0x0002;
    ledsColorsArray[0] = 0x00FF00;

    /*! Stimulus LPF */
    /*! VC */
    vcStimulusLpfOptionsNum = VCStimulusLpfsNum;
    vcVoltageFiltersArray.resize(vcStimulusLpfOptionsNum);
    vcStimulusLpfSwitchesLut.resize(vcStimulusLpfOptionsNum);

    vcVoltageFiltersArray[VCStimulusLpfNoFilter].value = 10.0;
    vcVoltageFiltersArray[VCStimulusLpfNoFilter].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCStimulusLpfNoFilter].unit = "Hz";

    vcStimulusLpfSwitchesLutStrings[VCStimulusLpfNoFilter] = "0";

    vcStimulusLpfSwitchesNum = VCStimulusLpfSwitchesNum;
    vcStimulusLpfSwitchesWord.resize(vcStimulusLpfSwitchesNum);
    vcStimulusLpfSwitchesByte.resize(vcStimulusLpfSwitchesNum);
    vcStimulusLpfSwitchesWord[VCStimulusLpfSwitchEXT_cap] = 0;
    vcStimulusLpfSwitchesByte[VCStimulusLpfSwitchEXT_cap] = 0x0400;

    for (unsigned int vcStimulusLpfOptionIdx = 0; vcStimulusLpfOptionIdx < vcStimulusLpfOptionsNum; vcStimulusLpfOptionIdx++) {
        vcStimulusLpfSwitchesLut[vcStimulusLpfOptionIdx].resize(vcStimulusLpfSwitchesNum);
        for (unsigned int vcStimulusLpfSwitchIdx = 0; vcStimulusLpfSwitchIdx < vcStimulusLpfSwitchesNum; vcStimulusLpfSwitchIdx++) {
            if (vcStimulusLpfSwitchesLutStrings[vcStimulusLpfOptionIdx][vcStimulusLpfSwitchIdx] == '1') {
                vcStimulusLpfSwitchesLut[vcStimulusLpfOptionIdx][vcStimulusLpfSwitchIdx] = true;

            } else {
                vcStimulusLpfSwitchesLut[vcStimulusLpfOptionIdx][vcStimulusLpfSwitchIdx] = false;
            }
        }
    }

    /*! CC */
    ccStimulusLpfOptionsNum = CCStimulusLpfsNum;
    ccCurrentFiltersArray.resize(ccStimulusLpfOptionsNum);
    ccStimulusLpfSwitchesLut.resize(ccStimulusLpfOptionsNum);

    ccStimulusLpfSwitchesNum = CCStimulusLpfSwitchesNum;
    ccStimulusLpfSwitchesWord.resize(ccStimulusLpfSwitchesNum);
    ccStimulusLpfSwitchesByte.resize(ccStimulusLpfSwitchesNum);

    /*! Constant switches */
    constantSwitchesNum = ConstantSwitchesNum;
    constantSwitchesWord.resize(constantSwitchesNum);
    constantSwitchesByte.resize(constantSwitchesNum);
    constantSwitchesLut.resize(constantSwitchesNum);

    /*! Compensations switches */
    pipetteCompensationImplemented = true;
    membraneCompensationImplemented = true;
    resistanceCompensationImplemented = true;

    compensationsSwitchesNum = CompensationsSwitchesNum;
    compensationsSwitchesWord.resize(compensationsSwitchesNum);
    compensationsSwitchesByte.resize(compensationsSwitchesNum);

    compensationsSwitchesWord[CompensationsSwitchCFastEn] = 0;
    compensationsSwitchesWord[CompensationsSwitchAuxCSlow] = 0;
    compensationsSwitchesWord[CompensationsSwitchVcInt] = 0;
    compensationsSwitchesWord[CompensationsSwitchAnOut] = 0;
    compensationsSwitchesWord[CompensationsSwitchVcPin] = 0;
    compensationsSwitchesWord[CompensationsSwitchExtCapCSlow] = 0;
    compensationsSwitchesWord[CompensationsSwitchExtCap2CSlow] = 0;
    compensationsSwitchesWord[CompensationsSwitchRsIn] = 0;
    compensationsSwitchesWord[CompensationsSwitchVcmForce2] = 0;

    compensationsSwitchesByte[CompensationsSwitchCFastEn] = 0x0001;
    compensationsSwitchesByte[CompensationsSwitchAuxCSlow] = 0x0002;
    compensationsSwitchesByte[CompensationsSwitchVcInt] = 0x0010;
    compensationsSwitchesByte[CompensationsSwitchAnOut] = 0x0080;
    compensationsSwitchesByte[CompensationsSwitchVcPin] = 0x0020;
    compensationsSwitchesByte[CompensationsSwitchExtCapCSlow] = 0x0004;
    compensationsSwitchesByte[CompensationsSwitchExtCap2CSlow] = 0x0008;
    compensationsSwitchesByte[CompensationsSwitchRsIn] = 0x0040;
    compensationsSwitchesByte[CompensationsSwitchVcmForce2] = 0x0200;

    compensationsComboSwitchesLutStrings[CompensationsComboNone] = "001000001";
    compensationsComboSwitchesLutStrings[CompensationsComboPipette] = "101000001";
    compensationsComboSwitchesLutStrings[CompensationsComboMembrane] = "010011010";
    compensationsComboSwitchesLutStrings[CompensationsComboMembranePipette] = "110011010";
    compensationsComboSwitchesLutStrings[CompensationsComboResistance] = "000110011";
    compensationsComboSwitchesLutStrings[CompensationsComboResistancePipette] = "100110011";
    compensationsComboSwitchesLutStrings[CompensationsComboResistanceMembrane] = "010111010";
    compensationsComboSwitchesLutStrings[CompensationsComboResistanceMembranePipette] = "110111010";

    for (unsigned int compensationsComboIdx = 0; compensationsComboIdx < CompensationsComboNum; compensationsComboIdx++) {
        compensationsComboSwitchesLut[compensationsComboIdx].resize(compensationsSwitchesNum);
        for (unsigned int compensationsSwitchesIdx = 0; compensationsSwitchesIdx < compensationsSwitchesNum; compensationsSwitchesIdx++) {
            if (compensationsComboSwitchesLutStrings[compensationsComboIdx][compensationsSwitchesIdx] == '1') {
                compensationsComboSwitchesLut[compensationsComboIdx][compensationsSwitchesIdx] = true;

            } else {
                compensationsComboSwitchesLut[compensationsComboIdx][compensationsSwitchesIdx] = false;
            }
        }
    }

    /*! Compensations values */
    compensationsRegistersNum = CompensationsRegistersNum;
    coreSpecificRegistersNum = compensationsRegistersNum+2; /*! additional 2 are for VC hold tuner */

    liquidJunctionOffsetBinary = liquidJunctionSteps/2.0;
    liquidJunctionResolution = liquidJunctionStep;

    liquidJunctionControl.implemented = true;
    liquidJunctionControl.min = minLiquidJunctionRange;
    liquidJunctionControl.value = 0.0;
    liquidJunctionControl.max = maxLiquidJunctionRange;
    liquidJunctionControl.compensable = maxLiquidJunctionRange;
    liquidJunctionControl.steps = liquidJunctionSteps;
    liquidJunctionControl.step = liquidJunctionStep;
    liquidJunctionControl.decimals = liquidJunctionDecimals;
    liquidJunctionControl.prefix = liquidJunctionPrefix;
    liquidJunctionControl.unit = liquidJunctionUnit;
    liquidJunctionControl.name = liquidJunctionName;

    pipetteCapacitanceControl.implemented = true;
    pipetteCapacitanceControl.min = minPipetteCapacitance;
    pipetteCapacitanceControl.value = minPipetteCapacitance;
    pipetteCapacitanceControl.max = maxPipetteCapacitance;
    pipetteCapacitanceControl.compensable = maxPipetteCapacitance;
    pipetteCapacitanceControl.steps = pipetteCapacitanceSteps;
    pipetteCapacitanceControl.step = pipetteCapacitanceStep;
    pipetteCapacitanceControl.decimals = pipetteCapacitanceDecimals;
    pipetteCapacitanceControl.prefix = pipetteCapacitancePrefix;
    pipetteCapacitanceControl.unit = pipetteCapacitanceUnit;
    pipetteCapacitanceControl.name = pipetteCapacitanceName;

    membraneCapacitanceControl.implemented = true;
    membraneCapacitanceControl.min = minMembraneCapacitance;
    membraneCapacitanceControl.value = minMembraneCapacitance;
    membraneCapacitanceControl.max = maxMembraneCapacitance;
    membraneCapacitanceControl.compensable = maxMembraneCapacitance;
    membraneCapacitanceControl.steps = membraneCapacitanceSteps;
    membraneCapacitanceControl.step = membraneCapacitanceStep;
    membraneCapacitanceControl.decimals = membraneCapacitanceDecimals;
    membraneCapacitanceControl.prefix = membraneCapacitancePrefix;
    membraneCapacitanceControl.unit = membraneCapacitanceUnit;
    membraneCapacitanceControl.name = membraneCapacitanceName;

    accessResistanceControl.implemented = true;
    accessResistanceControl.min = 0.0;
    accessResistanceControl.value = 0.0;
    accessResistanceControl.max = 100.0;
    accessResistanceControl.compensable = 100.0;
    accessResistanceControl.steps = 1001.0;
    accessResistanceControl.step = 0.1;
    accessResistanceControl.decimals = 1;
    accessResistanceControl.prefix = accessResistancePrefix;
    accessResistanceControl.unit = accessResistanceUnit;
    accessResistanceControl.name = accessResistanceName;

    resistanceCorrectionPercentageControl.implemented = true;
    resistanceCorrectionPercentageControl.min = 0.0;
    resistanceCorrectionPercentageControl.value = 0.0;
    resistanceCorrectionPercentageControl.max = 100.0;
    resistanceCorrectionPercentageControl.compensable = 100.0;
    resistanceCorrectionPercentageControl.steps = 101.0;
    resistanceCorrectionPercentageControl.step = 1.0;
    resistanceCorrectionPercentageControl.decimals = 1;
    resistanceCorrectionPercentageControl.prefix = resistanceCorrectionPercentagePrefix;
    resistanceCorrectionPercentageControl.unit = resistanceCorrectionPercentageUnit;
    resistanceCorrectionPercentageControl.name = resistanceCorrectionPercentageName;

    resistanceCorrectionLagControl.implemented = true;
    resistanceCorrectionLagControl.min = minResistanceCorrectionLag;
    resistanceCorrectionLagControl.value = minResistanceCorrectionLag;
    resistanceCorrectionLagControl.max = maxResistanceCorrectionLag;
    resistanceCorrectionLagControl.compensable = maxResistanceCorrectionLag;
    resistanceCorrectionLagControl.steps = resistanceCorrectionLagSteps;
    resistanceCorrectionLagControl.step = resistanceCorrectionLagStep;
    resistanceCorrectionLagControl.decimals = resistanceCorrectionLagDecimals;
    resistanceCorrectionLagControl.prefix = resistanceCorrectionLagPrefix;
    resistanceCorrectionLagControl.unit = resistanceCorrectionLagUnit;
    resistanceCorrectionLagControl.name = resistanceCorrectionLagName;

    resistancePredictionPercentageControl.implemented = true;
    resistancePredictionPercentageControl.min = 0.0;
    resistancePredictionPercentageControl.value = 100.0;
    resistancePredictionPercentageControl.max = 150.0;
    resistancePredictionPercentageControl.compensable = 150.0;
    resistancePredictionPercentageControl.steps = 151.0;
    resistancePredictionPercentageControl.step = 1.0;
    resistancePredictionPercentageControl.decimals = 1;
    resistancePredictionPercentageControl.prefix = resistancePredictionPercentagePrefix;
    resistancePredictionPercentageControl.unit = resistancePredictionPercentageUnit;
    resistancePredictionPercentageControl.name = resistancePredictionPercentageName;

    /*! Zap command */
    zapDurationHwRegisterOffset = 6;

    /*! Channel sources */
    selectableTotalChannelsNum = 0;
    selectableCurrentChannelsNum = 0;
    selectableVoltageChannelsNum = 0;
    channelSourcesRegisters.resize(selectableTotalChannelsNum);

    /*! Voltage holding tuner command */
    voltageHoldTunerImplemented = true;
    vcHoldTunerHwRegisterOffset = 8;

    currentHoldTunerImplemented = false;

    /*! Reset commands */
    resetWord[ResetIndexChip] = 0;
    resetWord[ResetIndexDigitalOffsetCompensation] = 0;

    resetByte[ResetIndexChip] = 0x4000;
    resetByte[ResetIndexDigitalOffsetCompensation] = 0x8000;

    /*! Stimulus */
    /*! VC */
    vcStimulusSwitchesNum = VCStimulusSwitchesNum;
    vcStimulusSwitchesLut.resize(vcStimulusSwitchesNum);

    /*! CC */
    ccStimulusSwitchesNum = CCStimulusSwitchesNum;
    ccStimulusSwitchesLut.resize(ccStimulusSwitchesNum);

    /*! Reader */
    /*! VC */
    vcReaderSwitchesNum = VCReaderSwitchesNum;
    vcReaderSwitchesLut.resize(vcReaderSwitchesNum);

    /*! CC */
    ccReaderSwitchesNum = CCReaderSwitchesNum;
    ccReaderSwitchesLut.resize(ccReaderSwitchesNum);

    /*! Protocols parameters */
    protocolFpgaClockFrequencyHz = 10.0e3;

    protocolTimeRange.step = 1000.0/protocolFpgaClockFrequencyHz;
    protocolTimeRange.min = LINT32_MIN*protocolTimeRange.step;
    protocolTimeRange.max = LINT32_MAX*protocolTimeRange.step;
    protocolTimeRange.prefix = UnitPfxMilli;
    protocolTimeRange.unit = "s";

    positiveProtocolTimeRange = protocolTimeRange;
    positiveProtocolTimeRange.min = 0.0;

    protocolMaxItemsNum = 15;

    maxDigitalTriggerOutputEvents = 83;
    digitalTriggersNum = 1;
    repetitiveTriggerAvailableFlag = true;

    voltageProtocolStepImplemented = true;
    currentProtocolStepImplemented = true;
    voltageProtocolRampImplemented = true;
    currentProtocolRampImplemented = true;
    voltageProtocolSinImplemented = false;
    currentProtocolSinImplemented = false;

    multimeterStuckHazardFlag = false;

    /*! Payload */
    rxDataMessageMaxLen = 122;

    txDataMessageMaxLen = 26;

    notificationTag = deviceName;

    /*! Default values */
    currentRange = vcCurrentRangesArray[VCCurrentRange200pA];
    voltageRange = vcVoltageRangesArray[VCVoltageRange500mV];
    currentResolution = currentRange.step;
    voltageResolution = voltageRange.step;
    samplingRate = realSamplingRatesArray[SamplingRate1_25kHz];
}

EZPatchePatchEL03D_V04::~EZPatchePatchEL03D_V04() {

}

ErrorCodes_t EZPatchePatchEL03D_V04::setChannelsSources(int16_t, int16_t) {
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t EZPatchePatchEL03D_V04::setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) {
    ErrorCodes_t ret;
    ret = EZPatchDevice::setVCCurrentRange(currentRangeIdx, applyFlag);
    if (ret == Success) {
        /*! \todo FCON qui bisogna gestire in qualche modo il fatto che può andare a buon fine il cambio di range, ma non
                       l'aggiornamento delle compensazioni, magari con errori diversi*/
        if (this->checkCompensationsValues()) {
            this->updateCompensations();
        }
    }
    return ret;
}

void EZPatchePatchEL03D_V04::selectChannelsResolutions() {
    for (unsigned int channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        currentTunerCorrection[channelIdx] = 0.0;
        voltageTunerCorrection[channelIdx] = selectedVoltageHoldVector[channelIdx].value;
    }
    rawDataFilterVoltageFlag = false;
    rawDataFilterCurrentFlag = true;
    this->selectVoltageOffsetResolution();
    /*! only channel sources available are from current clamp front end */

    this->computeRawDataFilterCoefficients();
}

void EZPatchePatchEL03D_V04::selectVoltageOffsetResolution() {
    voltageOffsetCorrection = 0.0;
    /*! The voltage is not measured so we do not need to correct it with the voltage offset */
}

void EZPatchePatchEL03D_V04::initializeCompensations() {
    EZPatchDevice::initializeCompensations();

    pipetteCapacitanceRegValue.resize(currentChannelsNum);
    membraneCapacitanceRegValue.resize(currentChannelsNum);
    membraneTauRegValue.resize(currentChannelsNum);
    correctionGainRegValue.resize(currentChannelsNum);
    correctionLagRegValue.resize(currentChannelsNum);
    predictionTauRegValue.resize(currentChannelsNum);

    for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        pipetteCapacitanceRegValue[channelIdx] = 0x0F;
        membraneCapacitanceRegValue[channelIdx] = 0xFF;
        membraneTauRegValue[channelIdx] = 0xFF;
        correctionGainRegValue[channelIdx] = 0xFF;
        correctionLagRegValue[channelIdx] = 0xFF;
        predictionTauRegValue[channelIdx] = 0xFF;
    }
}

bool EZPatchePatchEL03D_V04::checkCompensationsValues() {
    bool ret = true;
    ret &= (pipetteCapacitance[compensationsSettingChannel] > (minPipetteCapacitance-0.5*pipetteCapacitanceStep) &&
            pipetteCapacitance[compensationsSettingChannel] < (maxPipetteCapacitance+0.5*pipetteCapacitanceStep));
    ret &= (membraneCapacitance[compensationsSettingChannel] > (minMembraneCapacitance-0.5*membraneCapacitanceStep) &&
            membraneCapacitance[compensationsSettingChannel] < (maxMembraneCapacitance+0.5*membraneCapacitanceStep));
    double membraneTau = membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel];
    ret &= (membraneTau > (minMembraneTau-0.5*membraneTauStep) &&
            membraneTau < (maxMembraneTau+0.5*membraneTauStep));
    double resistanceCorrected = accessResistance[compensationsSettingChannel]*resistanceCorrectionPercentage[compensationsSettingChannel]/maxResistanceCorrectionPercentage;
    ret &= (resistanceCorrected > (0.999*minResistanceCorrection-0.001*maxResistanceCorrection()) &&
            resistanceCorrected < (0.001*minResistanceCorrection+1.001*maxResistanceCorrection()));
    ret &= (resistanceCorrectionLag[compensationsSettingChannel] > (minResistanceCorrectionLag-0.5*resistanceCorrectionLagStep) &&
            resistanceCorrectionLag[compensationsSettingChannel] < (maxResistanceCorrectionLag+0.5*resistanceCorrectionLagStep));
    double resistancePredictionTau = membraneTau*resistancePredictionPercentage[compensationsSettingChannel]/maxResistancePredictionPercentage;
    ret &= (resistancePredictionTau > (minResistancePredictionTau-0.5*resistancePredictionTauStep) &&
            resistancePredictionTau < (maxResistancePredictionTau+0.5*resistancePredictionTauStep));

    if (ret) {
        double rs = fmax(accessResistance[compensationsSettingChannel], accessResistanceControl.step);
        double rc = fmax(resistanceCorrectionPercentage[compensationsSettingChannel], resistanceCorrectionPercentageControl.step);
        double rp = fmax(resistancePredictionPercentage[compensationsSettingChannel], resistancePredictionPercentageControl.step);

        /*! pipetteCapacitanceControl.compensable doesn't depend on other controls */

        double d1 = maxMembraneCapacitance;
        double d2 = maxMembraneTau/rs;
        double d3 = maxResistancePredictionTau*maxResistancePredictionPercentage/(rs*rp);
        membraneCapacitanceControl.compensable = fmin(fmin(d1, d2), d3);

        d1 = accessResistanceControl.max;
        d2 = maxMembraneTau/membraneCapacitance[compensationsSettingChannel];
        d3 = maxResistanceCorrection()*maxResistanceCorrectionPercentage/rc;
        double d4 = maxResistancePredictionTau*maxResistancePredictionPercentage/(membraneCapacitance[compensationsSettingChannel]*rp);
        accessResistanceControl.compensable = fmin(fmin(fmin(d1, d2), d3), d4);

        d1 = resistanceCorrectionPercentageControl.max;
        d2 = maxResistanceCorrection()*maxResistanceCorrectionPercentage/rs;
        resistanceCorrectionPercentageControl.compensable = fmin(d1, d2);

        /*! resistanceCorrectionLagControl.compensable doesn't depend on other controls */

        d1 = resistancePredictionPercentageControl.max;
        d2 = maxResistancePredictionTau*maxResistancePredictionPercentage/(membraneCapacitance[compensationsSettingChannel]*rs);
        resistancePredictionPercentageControl.compensable = fmin(d1, d2);
    }

    return ret;
}

bool EZPatchePatchEL03D_V04::fillCompensationsRegistersTxData(std::vector <uint16_t> &txDataMessage) {
    bool anythingChanged = false;
    txDataMessage[0] = CompensationsRegisterAmpCFast+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[1] = ((vcCompensationsActivated & compCfastEnable[compensationsSettingChannel]) ? (uint16_t)round(pipetteCapacitance[compensationsSettingChannel]/pipetteCapacitanceStep) : 0);
    txDataMessage[2] = CompensationsRegisterExtAmpCSlow+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[3] = ((vcCompensationsActivated & compCslowEnable[compensationsSettingChannel]) ? (uint16_t)round((membraneCapacitance[compensationsSettingChannel]-minMembraneCapacitance)/membraneCapacitanceStep) : 0);
    txDataMessage[4] = CompensationsRegisterExtDacCSlow+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[5] = ((vcCompensationsActivated & compCslowEnable[compensationsSettingChannel]) ? (uint16_t)round((membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel]-minMembraneTau)/membraneTauStep) : 0);
    txDataMessage[6] = CompensationsRegisterOdacRs+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[7] = ((vcCompensationsActivated & compRsCompEnable[compensationsSettingChannel]) ? (uint16_t)round(resistanceCorrectionPercentage[compensationsSettingChannel]/resistanceCorrectionPercentageStep*accessResistance[compensationsSettingChannel]/transImpedance[selectedVcCurrentRangeIdx]) : 0);
    txDataMessage[8] = CompensationsRegisterRsLag+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[9] = ((vcCompensationsActivated & compRsCompEnable[compensationsSettingChannel]) ? (uint16_t)round((resistanceCorrectionLag[compensationsSettingChannel]-minResistanceCorrectionLag)/resistanceCorrectionLagStep) : 0);
    txDataMessage[10] = CompensationsRegisterRsPrediction+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[11] = ((vcCompensationsActivated & compRsCompEnable[compensationsSettingChannel]) ? (uint16_t)round((membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel]*resistancePredictionPercentage[compensationsSettingChannel]/maxResistancePredictionPercentage-minResistancePredictionTau)/resistancePredictionTauStep) : 0);

    if (txDataMessage[1] != pipetteCapacitanceRegValue[compensationsSettingChannel] ||
            txDataMessage[3] != membraneCapacitanceRegValue[compensationsSettingChannel] ||
            txDataMessage[5] != membraneTauRegValue[compensationsSettingChannel] ||
            txDataMessage[7] != correctionGainRegValue[compensationsSettingChannel] ||
            txDataMessage[9] != correctionLagRegValue[compensationsSettingChannel] ||
            txDataMessage[11] != predictionTauRegValue[compensationsSettingChannel]) {
        anythingChanged = true;
    }
    return anythingChanged;
}

void EZPatchePatchEL03D_V04::updateWrittenCompesantionValues(std::vector <uint16_t> &txDataMessage) {
    pipetteCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[1];
    membraneCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[3];
    membraneTauRegValue[compensationsSettingChannel] = txDataMessage[5];
    correctionGainRegValue[compensationsSettingChannel] = txDataMessage[7];
    correctionLagRegValue[compensationsSettingChannel] = txDataMessage[9];
    predictionTauRegValue[compensationsSettingChannel] = txDataMessage[11];
}

void EZPatchePatchEL03D_V04::compensationsFlags2Switches(std::vector <uint16_t> &txDataMessage) {
    int compensationCombo = 0;
    if (vcCompensationsActivated) {
        compensationCombo = (compCfastEnable[compensationsSettingChannel] ? 1 : 0)+
                (compCslowEnable[compensationsSettingChannel] ? 2 : 0)+
                (compRsCompEnable[compensationsSettingChannel] ? 4 : 0);
    }

    for (unsigned int compensationsSwitchIdx = 0; compensationsSwitchIdx < compensationsSwitchesNum; compensationsSwitchIdx++) {
        if (compensationsComboSwitchesLut[compensationCombo][compensationsSwitchIdx]) {
            txDataMessage[compensationsSwitchesWord[compensationsSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] |=
                    compensationsSwitchesByte[compensationsSwitchIdx]; // 1

        } else {
            txDataMessage[compensationsSwitchesWord[compensationsSwitchIdx]+compensationsSettingChannel*coreSpecificSwitchesWordsNum] &=
                    ~compensationsSwitchesByte[compensationsSwitchIdx]; // 0
        }
    }
}

EZPatchePatchEL03D_V03::EZPatchePatchEL03D_V03(std::string di) :
    EZPatchePatchEL03D_V04(di) {
    upgradeNotes += "- Repetitive output trigger events\n";
    notificationTag = deviceName + "_V03";

    repetitiveTriggerAvailableFlag = false;
}

EZPatchePatchEL03D_V03::~EZPatchePatchEL03D_V03() {

}

EZPatchePatchEL03D_V02::EZPatchePatchEL03D_V02(std::string di) :
    EZPatchePatchEL03D_V03(di) {
    upgradeNotes += "- Increased maximum amout of output trigger events\n";
    notificationTag = deviceName + "_V02";

    maxDigitalTriggerOutputEvents = 21;
}

EZPatchePatchEL03D_V02::~EZPatchePatchEL03D_V02() {

}

EZPatchePatchEL03D_V01::EZPatchePatchEL03D_V01(std::string di) :
    EZPatchePatchEL03D_V02(di) {
    upgradeNotes += "- Implemented holding voltage tuner\n";
    notificationTag = deviceName + "_V01";

    voltageHoldTunerImplemented = false;
}

EZPatchePatchEL03D_V01::~EZPatchePatchEL03D_V01() {

}

EZPatchePatchEL03D_V00::EZPatchePatchEL03D_V00(std::string di) :
    EZPatchePatchEL03D_V01(di) {
    upgradeNotes += "- Implemented programmable digital output\n";
    notificationTag = deviceName + "_V00";

    maxDigitalTriggerOutputEvents = 1;
}

EZPatchePatchEL03D_V00::~EZPatchePatchEL03D_V00() {

}

EZPatchePatchDlp::EZPatchePatchDlp(std::string di) :
    EZPatchePatchEL03D_V03(di) {
    spiChannel = 'B';
    rxChannel = 'A';
    txChannel = 'A';
}

EZPatchePatchDlp::~EZPatchePatchDlp() {

}
