#include "ezpatche8ppatchliner_el07ab.h"

EZPatche8PPatchliner_el07ab::EZPatche8PPatchliner_el07ab(std::string di) :
    EZPatchFtdiDevice(di) {

    deviceName = "e8PPatchliner";

    /*! Clamping modalities */
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = ClampingModality_t::VOLTAGE_CLAMP;
    clampingModalitiesArray[ZeroCurrentClamp] = ClampingModality_t::ZERO_CURRENT_CLAMP;
    clampingModalitiesArray[CurrentClamp] = ClampingModality_t::CURRENT_CLAMP;
    defaultClampingModalityIdx = VoltageClamp;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange10nA].max = 10.0;
    vcCurrentRangesArray[VCCurrentRange10nA].min = -10.0;
    vcCurrentRangesArray[VCCurrentRange10nA].step = vcCurrentRangesArray[VCCurrentRange10nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange10nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange10nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange40nALbw].max = 40.0;
    vcCurrentRangesArray[VCCurrentRange40nALbw].min = -40.0;
    vcCurrentRangesArray[VCCurrentRange40nALbw].step = vcCurrentRangesArray[VCCurrentRange40nALbw].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange40nALbw].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange40nALbw].unit = "A";
    vcCurrentRangesArray[VCCurrentRange40nAHbw].max = 40.0;
    vcCurrentRangesArray[VCCurrentRange40nAHbw].min = -40.0;
    vcCurrentRangesArray[VCCurrentRange40nAHbw].step = vcCurrentRangesArray[VCCurrentRange40nAHbw].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange40nAHbw].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange40nAHbw].unit = "A";
    vcCurrentRangesArray[VCCurrentRange400nA].max = 400.0;
    vcCurrentRangesArray[VCCurrentRange400nA].min = -400.0;
    vcCurrentRangesArray[VCCurrentRange400nA].step = vcCurrentRangesArray[VCCurrentRange400nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange400nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange400nA].unit = "A";

    vcCurrentRangesSwitchesLut.resize(vcCurrentRangesNum);

    vcCurrentRangesSwitchesNum = VCCurrentRangesSwitchesNum;
    vcCurrentRangesSwitchesWord.resize(vcCurrentRangesSwitchesNum);
    vcCurrentRangesSwitchesByte.resize(vcCurrentRangesSwitchesNum);

    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange1] = 0;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange2] = 0;

    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange1] = 0x0080;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange2] = 0x0100;

    vcCurrentRangesSwitchesLutStrings[VCCurrentRange10nA] = "00";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange40nALbw] = "10";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange40nAHbw] = "01";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange400nA] = "11";

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
    ccCurrentRangesArray.resize(ccCurrentRangesNum);
    ccCurrentRangesArray[CCCurrentRange8nA].max = 8000.0;
    ccCurrentRangesArray[CCCurrentRange8nA].min = 8000.0;
    ccCurrentRangesArray[CCCurrentRange8nA].step = ccCurrentRangesArray[CCCurrentRange8nA].max/(INT13_MAX+1.0);
    ccCurrentRangesArray[CCCurrentRange8nA].prefix = UnitPfxPico;
    ccCurrentRangesArray[CCCurrentRange8nA].unit = "A";

    ccCurrentRangesSwitchesLut.resize(ccCurrentRangesNum);

    ccCurrentRangesSwitchesNum = CCCurrentRangesSwitchesNum;
    ccCurrentRangesSwitchesWord.resize(ccCurrentRangesSwitchesNum);
    ccCurrentRangesSwitchesByte.resize(ccCurrentRangesSwitchesNum);

    /*! No words/bytes to set */
    ccCurrentRangesSwitchesLutStrings[CCCurrentRange8nA] = "";

    for (unsigned int currentRangesIdx = 0; currentRangesIdx < ccCurrentRangesNum; currentRangesIdx++) {
        ccCurrentRangesSwitchesLut[currentRangesIdx].resize(ccCurrentRangesSwitchesNum);
        for (unsigned int currentRangesSwitchesIdx = 0; currentRangesSwitchesIdx < ccCurrentRangesSwitchesNum; currentRangesSwitchesIdx++) {
            if (ccCurrentRangesSwitchesLutStrings[currentRangesIdx][currentRangesSwitchesIdx] == '1') {
                ccCurrentRangesSwitchesLut[currentRangesIdx][currentRangesSwitchesIdx] = true;

            } else {
                ccCurrentRangesSwitchesLut[currentRangesIdx][currentRangesSwitchesIdx] = false;
            }
        }
    }

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].max = 500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].min = -500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.125;
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
    ccVoltageRangesArray[CCVoltageRange1000mV].max = 1000.0;
    ccVoltageRangesArray[CCVoltageRange1000mV].min = -1000.0;
    ccVoltageRangesArray[CCVoltageRange1000mV].step = ccVoltageRangesArray[CCVoltageRange1000mV].max/(SHORT_MAX+1.0);
    ccVoltageRangesArray[CCVoltageRange1000mV].prefix = UnitPfxMilli;
    ccVoltageRangesArray[CCVoltageRange1000mV].unit = "V";

    ccVoltageRangesSwitchesLut.resize(ccVoltageRangesNum);

    ccVoltageRangesSwitchesNum = CCVoltageRangesSwitchesNum;
    ccVoltageRangesSwitchesWord.resize(ccVoltageRangesSwitchesNum);
    ccVoltageRangesSwitchesByte.resize(ccVoltageRangesSwitchesNum);

    /*! No words/bytes to set */
    ccVoltageRangesSwitchesLutStrings[CCVoltageRange1000mV] = "";

    for (unsigned int voltageRangesIdx = 0; voltageRangesIdx < ccVoltageRangesNum; voltageRangesIdx++) {
        ccVoltageRangesSwitchesLut[voltageRangesIdx].resize(ccVoltageRangesSwitchesNum);
        for (unsigned int voltageRangesSwitchesIdx = 0; voltageRangesSwitchesIdx < ccVoltageRangesSwitchesNum; voltageRangesSwitchesIdx++) {
            if (ccVoltageRangesSwitchesLutStrings[voltageRangesIdx][voltageRangesSwitchesIdx] == '1') {
                ccVoltageRangesSwitchesLut[voltageRangesIdx][voltageRangesSwitchesIdx] = true;

            } else {
                ccVoltageRangesSwitchesLut[voltageRangesIdx][voltageRangesSwitchesIdx] = false;
            }
        }
    }

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate6_25kHz].value = 6.25;
    samplingRatesArray[SamplingRate6_25kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate6_25kHz].unit = "Hz";
    samplingRatesArray[SamplingRate12_5kHz].value = 12.5;
    samplingRatesArray[SamplingRate12_5kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate12_5kHz].unit = "Hz";
    samplingRatesArray[SamplingRate25kHz].value = 25.0;
    samplingRatesArray[SamplingRate25kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate25kHz].unit = "Hz";
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
    realSamplingRatesArray[SamplingRate6_25kHz].value = 6.25;
    realSamplingRatesArray[SamplingRate6_25kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate6_25kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate12_5kHz].value = 12.5;
    realSamplingRatesArray[SamplingRate12_5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate12_5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate25kHz].value = 25.0;
    realSamplingRatesArray[SamplingRate25kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate25kHz].unit = "Hz";
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
    integrationStepArray[SamplingRate6_25kHz].value = 160.0;
    integrationStepArray[SamplingRate6_25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate6_25kHz].unit = "s";
    integrationStepArray[SamplingRate12_5kHz].value = 80.0;
    integrationStepArray[SamplingRate12_5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate12_5kHz].unit = "s";
    integrationStepArray[SamplingRate25kHz].value = 40.0;
    integrationStepArray[SamplingRate25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate25kHz].unit = "s";;
    integrationStepArray[SamplingRate50kHz].value = 20.0;
    integrationStepArray[SamplingRate50kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate50kHz].unit = "s";
    integrationStepArray[SamplingRate100kHz].value = 10.0;
    integrationStepArray[SamplingRate100kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate100kHz].unit = "s";
    integrationStepArray[SamplingRate200kHz].value = 5.0;
    integrationStepArray[SamplingRate200kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate200kHz].unit = "s";

    samplingRatesSwitchesLut.resize(samplingRatesNum);

    samplingRatesSwitchesNum = SamplingRatesSwitchesNum;
    samplingRatesSwitchesWord.resize(samplingRatesSwitchesNum);
    samplingRatesSwitchesByte.resize(samplingRatesSwitchesNum);

    samplingRatesSwitchesWord[SamplingRateSwitchVcAdcFilter2] = 0;
    samplingRatesSwitchesWord[SamplingRateSwitchVcAdcFilter1] = 0;
    samplingRatesSwitchesWord[SamplingRateSwitchVcAdcFilter0] = 0;

    samplingRatesSwitchesByte[SamplingRateSwitchVcAdcFilter2] = 0x0040;
    samplingRatesSwitchesByte[SamplingRateSwitchVcAdcFilter1] = 0x0020;
    samplingRatesSwitchesByte[SamplingRateSwitchVcAdcFilter0] = 0x0010;

    samplingRatesSwitchesLutStrings[SamplingRate6_25kHz] = "001";       // BW  3.6kHz
    samplingRatesSwitchesLutStrings[SamplingRate12_5kHz] = "010";       // BW  10kHz
    samplingRatesSwitchesLutStrings[SamplingRate25kHz] = "100";         // BW  25kHz
    samplingRatesSwitchesLutStrings[SamplingRate50kHz] = "101";         // BW  30kHz
    samplingRatesSwitchesLutStrings[SamplingRate100kHz] = "110";        // BW 100kHz
    samplingRatesSwitchesLutStrings[SamplingRate200kHz] = "111";        // BW 120kHz

    for (unsigned int samplingRatesIdx = 0; samplingRatesIdx < samplingRatesNum; samplingRatesIdx++) {
        samplingRatesSwitchesLut[samplingRatesIdx].resize(samplingRatesSwitchesNum);
        for (unsigned int samplingRatesSwitchesIdx = 0; samplingRatesSwitchesIdx < samplingRatesSwitchesNum; samplingRatesSwitchesIdx++) {
            if (samplingRatesSwitchesLutStrings[samplingRatesIdx][samplingRatesSwitchesIdx] == '1') {
                samplingRatesSwitchesLut[samplingRatesIdx][samplingRatesSwitchesIdx] = true;

            } else {
                samplingRatesSwitchesLut[samplingRatesIdx][samplingRatesSwitchesIdx] = false;
            }
        }
    }

    currentChannelsNum = 8;
    voltageChannelsNum = 8;
    totalChannelsNum = currentChannelsNum+voltageChannelsNum;

    availableVoltageSourcesIdxs.VoltageFromVoltageClamp = ChannelSourceVoltageFromVoltageClamp;
    availableVoltageSourcesIdxs.VoltageFromCurrentClamp = ChannelSourceVoltageFromCurrentClamp;
    availableVoltageSourcesIdxs.VoltageFromDynamicClamp = ChannelSourceVoltageFromDynamicClamp;
    availableVoltageSourcesIdxs.VoltageFromVoltagePlusDynamicClamp = ChannelSourceVoltageFromVoltagePlusDynamicClamp;

    availableCurrentSourcesIdxs.CurrentFromVoltageClamp = ChannelSourceCurrentFromVoltageClamp;
    availableCurrentSourcesIdxs.CurrentFromCurrentClamp = ChannelSourceCurrentFromCurrentClamp;
    availableCurrentSourcesIdxs.CurrentFromDynamicClamp = ChannelSourceCurrentFromDynamicClamp;
    availableCurrentSourcesIdxs.CurrentFromCurrentPlusDynamicClamp = ChannelSourceCurrentFromCurrentPlusDynamicClamp;

    availableVoltageSourcesIdxsArray.resize(4);
    availableVoltageSourcesIdxsArray[0] = ChannelSourceVoltageFromVoltageClamp;
    availableVoltageSourcesIdxsArray[1] = ChannelSourceVoltageFromCurrentClamp;
    availableVoltageSourcesIdxsArray[2] = ChannelSourceVoltageFromDynamicClamp;
    availableVoltageSourcesIdxsArray[3] = ChannelSourceVoltageFromVoltagePlusDynamicClamp;

    availableCurrentSourcesIdxsArray.resize(4);
    availableCurrentSourcesIdxsArray[0] = ChannelSourceCurrentFromVoltageClamp;
    availableCurrentSourcesIdxsArray[1] = ChannelSourceCurrentFromCurrentClamp;
    availableCurrentSourcesIdxsArray[2] = ChannelSourceCurrentFromDynamicClamp;
    availableCurrentSourcesIdxsArray[3] = ChannelSourceCurrentFromCurrentPlusDynamicClamp;

    /*! HW Switches */
    switchesStatusLength = 10;

    switchesStatus.resize(switchesStatusLength);

    switchesNames.resize(switchesStatusLength);
    switchesNames[0].resize(16);
    switchesNames[0][0] = "ResetChip";
    switchesNames[0][1] = "ResetDComp";
    switchesNames[0][2] = "VC_CC_SEL_CHIP";
    switchesNames[0][3] = "ProtStartMode";
    switchesNames[0][4] = "VC_ADC_Filter1";
    switchesNames[0][5] = "VC_ADC_Filter2";
    switchesNames[0][6] = "VC_ADC_Filter3";
    switchesNames[0][7] = "VC_ADC_Range1";
    switchesNames[0][8] = "VC_ADC_Range2";
    switchesNames[0][9] = "VC_DAC_Filter1";
    switchesNames[0][10] = "VC_DAC_Filter2";
    switchesNames[0][11] = "CC_DAC_Filter1";
    switchesNames[0][12] = "CC_DAC_Filter2";
    switchesNames[0][13] = "VC_dig_off_ovrd";
    switchesNames[0][14] = "LED_ON";
    switchesNames[0][15] = "free";

    switchesNames[1].resize(16);
    switchesNames[1][0] = "enableSPI_DAC";
    switchesNames[1][1] = "selectUnusedChannels";
    switchesNames[1][2] = "CalibrateDacData";
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

    switchesNames[2].resize(16);
    switchesNames[2][0] = "CH1_Input_SW";
    switchesNames[2][1] = "CH1_Calib_SW";
    switchesNames[2][2] = "CH1_VC_SW";
    switchesNames[2][3] = "CH1_CC_SW";
    switchesNames[2][4] = "CH1_CC_StimEN";
    switchesNames[2][5] = "CH1_CfastEN";
    switchesNames[2][6] = "CH1_VC_CslowEN";
    switchesNames[2][7] = "CH1_VC_CorrEN";
    switchesNames[2][8] = "CH1_VC_CorrBW1";
    switchesNames[2][9] = "CH1_VC_CorrBW2";
    switchesNames[2][10] = "CH1_VC_CorrBW3";
    switchesNames[2][11] = "CH1_VC_PredEN";
    switchesNames[2][12] = "CH1_ProtocolEN";
    switchesNames[2][13] = "free";
    switchesNames[2][14] = "free";
    switchesNames[2][15] = "free";

    switchesNames[3].resize(16);
    switchesNames[3][0] = "CH2_Input_SW";
    switchesNames[3][1] = "CH2_Calib_SW";
    switchesNames[3][2] = "CH2_VC_SW";
    switchesNames[3][3] = "CH2_CC_SW";
    switchesNames[3][4] = "CH2_CC_StimEN";
    switchesNames[3][5] = "CH2_CfastEN";
    switchesNames[3][6] = "CH2_VC_CslowEN";
    switchesNames[3][7] = "CH2_VC_CorrEN";
    switchesNames[3][8] = "CH2_VC_CorrBW1";
    switchesNames[3][9] = "CH2_VC_CorrBW2";
    switchesNames[3][10] = "CH2_VC_CorrBW3";
    switchesNames[3][11] = "CH2_VC_PredEN";
    switchesNames[3][12] = "CH2_ProtocolEN";
    switchesNames[3][13] = "free";
    switchesNames[3][14] = "free";
    switchesNames[3][15] = "free";

    switchesNames[4].resize(16);
    switchesNames[4][0] = "CH3_Input_SW";
    switchesNames[4][1] = "CH3_Calib_SW";
    switchesNames[4][2] = "CH3_VC_SW";
    switchesNames[4][3] = "CH3_CC_SW";
    switchesNames[4][4] = "CH3_CC_StimEN";
    switchesNames[4][5] = "CH3_CfastEN";
    switchesNames[4][6] = "CH3_VC_CslowEN";
    switchesNames[4][7] = "CH3_VC_CorrEN";
    switchesNames[4][8] = "CH3_VC_CorrBW1";
    switchesNames[4][9] = "CH3_VC_CorrBW2";
    switchesNames[4][10] = "CH3_VC_CorrBW3";
    switchesNames[4][11] = "CH3_VC_PredEN";
    switchesNames[4][12] = "CH3_ProtocolEN";
    switchesNames[4][13] = "free";
    switchesNames[4][14] = "free";
    switchesNames[4][15] = "free";

    switchesNames[5].resize(16);
    switchesNames[5][0] = "CH4_Input_SW";
    switchesNames[5][1] = "CH4_Calib_SW";
    switchesNames[5][2] = "CH4_VC_SW";
    switchesNames[5][3] = "CH4_CC_SW";
    switchesNames[5][4] = "CH4_CC_StimEN";
    switchesNames[5][5] = "CH4_CfastEN";
    switchesNames[5][6] = "CH4_VC_CslowEN";
    switchesNames[5][7] = "CH4_VC_CorrEN";
    switchesNames[5][8] = "CH4_VC_CorrBW1";
    switchesNames[5][9] = "CH4_VC_CorrBW2";
    switchesNames[5][10] = "CH4_VC_CorrBW3";
    switchesNames[5][11] = "CH4_VC_PredEN";
    switchesNames[5][12] = "CH4_ProtocolEN";
    switchesNames[5][13] = "free";
    switchesNames[5][14] = "free";
    switchesNames[5][15] = "free";

    switchesNames[6].resize(16);
    switchesNames[6][0] = "CH5_Input_SW";
    switchesNames[6][1] = "CH5_Calib_SW";
    switchesNames[6][2] = "CH5_VC_SW";
    switchesNames[6][3] = "CH5_CC_SW";
    switchesNames[6][4] = "CH5_CC_StimEN";
    switchesNames[6][5] = "CH5_CfastEN";
    switchesNames[6][6] = "CH5_VC_CslowEN";
    switchesNames[6][7] = "CH5_VC_CorrEN";
    switchesNames[6][8] = "CH5_VC_CorrBW1";
    switchesNames[6][9] = "CH5_VC_CorrBW2";
    switchesNames[6][10] = "CH5_VC_CorrBW3";
    switchesNames[6][11] = "CH5_VC_PredEN";
    switchesNames[6][12] = "CH5_ProtocolEN";
    switchesNames[6][13] = "free";
    switchesNames[6][14] = "free";
    switchesNames[6][15] = "free";

    switchesNames[7].resize(16);
    switchesNames[7][0] = "CH6_Input_SW";
    switchesNames[7][1] = "CH6_Calib_SW";
    switchesNames[7][2] = "CH6_VC_SW";
    switchesNames[7][3] = "CH6_CC_SW";
    switchesNames[7][4] = "CH6_CC_StimEN";
    switchesNames[7][5] = "CH6_CfastEN";
    switchesNames[7][6] = "CH6_VC_CslowEN";
    switchesNames[7][7] = "CH6_VC_CorrEN";
    switchesNames[7][8] = "CH6_VC_CorrBW1";
    switchesNames[7][9] = "CH6_VC_CorrBW2";
    switchesNames[7][10] = "CH6_VC_CorrBW3";
    switchesNames[7][11] = "CH6_VC_PredEN";
    switchesNames[7][12] = "CH6_ProtocolEN";
    switchesNames[7][13] = "free";
    switchesNames[7][14] = "free";
    switchesNames[7][15] = "free";

    switchesNames[8].resize(16);
    switchesNames[8][0] = "CH7_Input_SW";
    switchesNames[8][1] = "CH7_Calib_SW";
    switchesNames[8][2] = "CH7_VC_SW";
    switchesNames[8][3] = "CH7_CC_SW";
    switchesNames[8][4] = "CH7_CC_StimEN";
    switchesNames[8][5] = "CH7_CfastEN";
    switchesNames[8][6] = "CH7_VC_CslowEN";
    switchesNames[8][7] = "CH7_VC_CorrEN";
    switchesNames[8][8] = "CH7_VC_CorrBW1";
    switchesNames[8][9] = "CH7_VC_CorrBW2";
    switchesNames[8][10] = "CH7_VC_CorrBW3";
    switchesNames[8][11] = "CH7_VC_PredEN";
    switchesNames[8][12] = "CH7_ProtocolEN";
    switchesNames[8][13] = "free";
    switchesNames[8][14] = "free";
    switchesNames[8][15] = "free";

    switchesNames[9].resize(16);
    switchesNames[9][0] = "CH8_Input_SW";
    switchesNames[9][1] = "CH8_Calib_SW";
    switchesNames[9][2] = "CH8_VC_SW";
    switchesNames[9][3] = "CH8_CC_SW";
    switchesNames[9][4] = "CH8_CC_StimEN";
    switchesNames[9][5] = "CH8_CfastEN";
    switchesNames[9][6] = "CH8_VC_CslowEN";
    switchesNames[9][7] = "CH8_VC_CorrEN";
    switchesNames[9][8] = "CH8_VC_CorrBW1";
    switchesNames[9][9] = "CH8_VC_CorrBW2";
    switchesNames[9][10] = "CH8_VC_CorrBW3";
    switchesNames[9][11] = "CH8_VC_PredEN";
    switchesNames[9][12] = "CH8_ProtocolEN";
    switchesNames[9][13] = "free";
    switchesNames[9][14] = "free";
    switchesNames[9][15] = "free";

    /*! LEDs */
    ledsNum = 1;
    ledsWord.resize(ledsNum);
    ledsByte.resize(ledsNum);
    ledsColorsArray.resize(ledsNum);

    ledsWord[0] = 0;
    ledsByte[0] = 0x4000;
    ledsColorsArray[0] = 0x00FF00;

    /*! Stimulus LPF */
    /*! VC */
    vcStimulusLpfOptionsNum = VCStimulusLpfsNum;
    vcVoltageFiltersArray.resize(vcStimulusLpfOptionsNum);
    vcStimulusLpfSwitchesLut.resize(vcStimulusLpfOptionsNum);

    vcVoltageFiltersArray[VCStimulusLpf1kHz].value = 1.0;
    vcVoltageFiltersArray[VCStimulusLpf1kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCStimulusLpf1kHz].unit = "Hz";
    vcVoltageFiltersArray[VCStimulusLpf10kHz].value = 10.0;
    vcVoltageFiltersArray[VCStimulusLpf10kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCStimulusLpf10kHz].unit = "Hz";
    vcVoltageFiltersArray[VCStimulusLpf35kHz].value = 35.0;
    vcVoltageFiltersArray[VCStimulusLpf35kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCStimulusLpf35kHz].unit = "Hz";

    vcStimulusLpfSwitchesLutStrings[VCStimulusLpf1kHz] = "00";
    vcStimulusLpfSwitchesLutStrings[VCStimulusLpf10kHz] = "01";
    vcStimulusLpfSwitchesLutStrings[VCStimulusLpf35kHz] = "11";

    vcStimulusLpfSwitchesNum = VCStimulusLpfSwitchesNum;
    vcStimulusLpfSwitchesWord.resize(vcStimulusLpfSwitchesNum);
    vcStimulusLpfSwitchesByte.resize(vcStimulusLpfSwitchesNum);
    vcStimulusLpfSwitchesWord[VCStimulusLpfSwitchFilter1] = 0;
    vcStimulusLpfSwitchesWord[VCStimulusLpfSwitchFilter2] = 0;
    vcStimulusLpfSwitchesByte[VCStimulusLpfSwitchFilter1] = 0x0200;
    vcStimulusLpfSwitchesByte[VCStimulusLpfSwitchFilter2] = 0x0400;

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

    ccCurrentFiltersArray[CCStimulusLpf1kHz].value = 1.0;
    ccCurrentFiltersArray[CCStimulusLpf1kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCStimulusLpf1kHz].unit = "Hz";
    ccCurrentFiltersArray[CCStimulusLpf10kHz].value = 10.0;
    ccCurrentFiltersArray[CCStimulusLpf10kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCStimulusLpf10kHz].unit = "Hz";
    ccCurrentFiltersArray[CCStimulusLpf35kHz].value = 35.0;
    ccCurrentFiltersArray[CCStimulusLpf35kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCStimulusLpf35kHz].unit = "Hz";

    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf1kHz] = "00";
    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf10kHz] = "01";
    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf35kHz] = "11";

    ccStimulusLpfSwitchesNum = CCStimulusLpfSwitchesNum;
    ccStimulusLpfSwitchesWord.resize(ccStimulusLpfSwitchesNum);
    ccStimulusLpfSwitchesByte.resize(ccStimulusLpfSwitchesNum);
    ccStimulusLpfSwitchesWord[CCStimulusLpfSwitchFilter1] = 0;
    ccStimulusLpfSwitchesWord[CCStimulusLpfSwitchFilter2] = 0;
    ccStimulusLpfSwitchesByte[CCStimulusLpfSwitchFilter1] = 0x0800;
    ccStimulusLpfSwitchesByte[CCStimulusLpfSwitchFilter2] = 0x1000;

    for (unsigned int ccStimulusLpfOptionIdx = 0; ccStimulusLpfOptionIdx < ccStimulusLpfOptionsNum; ccStimulusLpfOptionIdx++) {
        ccStimulusLpfSwitchesLut[ccStimulusLpfOptionIdx].resize(ccStimulusLpfSwitchesNum);
        for (unsigned int ccStimulusLpfSwitchIdx = 0; ccStimulusLpfSwitchIdx < ccStimulusLpfSwitchesNum; ccStimulusLpfSwitchIdx++) {
            if (ccStimulusLpfSwitchesLutStrings[ccStimulusLpfOptionIdx][ccStimulusLpfSwitchIdx] == '1') {
                ccStimulusLpfSwitchesLut[ccStimulusLpfOptionIdx][ccStimulusLpfSwitchIdx] = true;

            } else {
                ccStimulusLpfSwitchesLut[ccStimulusLpfOptionIdx][ccStimulusLpfSwitchIdx] = false;
            }
        }
    }

    digitalOffsetCompensationOverrideImplemented = true;
    digitalOffsetCompensationOverrideSwitchWord = 0;
    digitalOffsetCompensationOverrideSwitchByte = 0x2000;
    digitalOffsetCompensationOverrideRegisterOffset = 14;

    vcCurrentOffsetDeltaImplemented = true;
    vcCurrentOffsetDeltaRegisterOffset = 15;
    ccCurrentOffsetDeltaImplemented = true;
    ccVoltageOffsetDeltaRegisterOffset = 16;

    startProtocolCommandImplemented = true;
    startProtocolSwitchWord = 0;
    startProtocolSwitchByte = 0x0008;

    /*! Enable stimulus */
    stimulusEnableImplemented = true;
    stimulusEnableSwitchWord = 2;
    stimulusEnableSwitchByte = 0x1000;

    /*! Constant switches */
    constantSwitchesNum = ConstantSwitchesNum;
    constantSwitchesWord.resize(constantSwitchesNum);
    constantSwitchesByte.resize(constantSwitchesNum);
    constantSwitchesLut.resize(constantSwitchesNum);

    constantSwitchesWord[ConstantSwitchEnableSpiDac] = 1;
    constantSwitchesWord[ConstantSwitchSelectUnusedChannels] = 1;
    constantSwitchesWord[ConstantSwitchCalibrateDacData] = 1;
    constantSwitchesWord[ConstantSwitchCh1InSW] = 2;
    constantSwitchesWord[ConstantSwitchCh1CalibEn] = 2;
    constantSwitchesWord[ConstantSwitchCh2InSW] = 3;
    constantSwitchesWord[ConstantSwitchCh2CalibEn] = 3;
    constantSwitchesWord[ConstantSwitchCh3InSW] = 4;
    constantSwitchesWord[ConstantSwitchCh3CalibEn] = 4;
    constantSwitchesWord[ConstantSwitchCh4InSW] = 5;
    constantSwitchesWord[ConstantSwitchCh4CalibEn] = 5;
    constantSwitchesWord[ConstantSwitchCh5InSW] = 6;
    constantSwitchesWord[ConstantSwitchCh5CalibEn] = 6;
    constantSwitchesWord[ConstantSwitchCh6InSW] = 7;
    constantSwitchesWord[ConstantSwitchCh6CalibEn] = 7;
    constantSwitchesWord[ConstantSwitchCh7InSW] = 8;
    constantSwitchesWord[ConstantSwitchCh7CalibEn] = 8;
    constantSwitchesWord[ConstantSwitchCh8InSW] = 9;
    constantSwitchesWord[ConstantSwitchCh8CalibEn] = 9;

    constantSwitchesByte[ConstantSwitchEnableSpiDac] = 0x0001;
    constantSwitchesByte[ConstantSwitchSelectUnusedChannels] = 0x0002;
    constantSwitchesByte[ConstantSwitchCalibrateDacData] = 0x0004;
    constantSwitchesByte[ConstantSwitchCh1InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh1CalibEn] = 0x0002;
    constantSwitchesByte[ConstantSwitchCh2InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh2CalibEn] = 0x0002;
    constantSwitchesByte[ConstantSwitchCh3InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh3CalibEn] = 0x0002;
    constantSwitchesByte[ConstantSwitchCh4InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh4CalibEn] = 0x0002;
    constantSwitchesByte[ConstantSwitchCh5InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh5CalibEn] = 0x0002;
    constantSwitchesByte[ConstantSwitchCh6InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh6CalibEn] = 0x0002;
    constantSwitchesByte[ConstantSwitchCh7InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh7CalibEn] = 0x0002;
    constantSwitchesByte[ConstantSwitchCh8InSW] = 0x0001;
    constantSwitchesByte[ConstantSwitchCh8CalibEn] = 0x0002;

    constantSwitchesLutStrings = "101"
                                 "10"
                                 "10"
                                 "10"
                                 "10"
                                 "10"
                                 "10"
                                 "10"
                                 "10";

    for (unsigned int constantSwitchIdx = 0; constantSwitchIdx < constantSwitchesNum; constantSwitchIdx++) {
        if (constantSwitchesLutStrings[constantSwitchIdx] == '1') {
            constantSwitchesLut[constantSwitchIdx] = true;

        } else {
            constantSwitchesLut[constantSwitchIdx] = false;
        }
    }

    /*! Compensations switches */
    pipetteCompensationImplemented = true;
    membraneCompensationImplemented = true;
    resistanceCorrectionImplemented = true;
    resistancePredictionImplemented = true;
    ccPipetteCompensationImplemented = true;

    compensationsSwitchesNum = CompensationsSwitchesNum;
    compensationsSwitchesWord.resize(compensationsSwitchesNum);
    compensationsSwitchesByte.resize(compensationsSwitchesNum);
    compensationsSwitchesLut.resize(compensationsSwitchesNum);
    compensationsSwitchesLutStrings.resize(compensationsSwitchesNum);
    compensationsSwitchesEnableSignArray.resize(compensationsSwitchesNum);
    coreSpecificSwitchesWordsNum = 1;

    compensationsSwitchesWord[CompensationsSwitchCFastEn] = 2;
    compensationsSwitchesWord[CompensationsSwitchCSlowEn] = 2;
    compensationsSwitchesWord[CompensationsSwitchRCorrEn] = 2;
    compensationsSwitchesWord[CompensationsSwitchRPredEn] = 2;
    compensationsSwitchesWord[CompensationsSwitchCCFastEn] = 2;

    compensationsSwitchesByte[CompensationsSwitchCFastEn] = 0x0020;
    compensationsSwitchesByte[CompensationsSwitchCSlowEn] = 0x0040;
    compensationsSwitchesByte[CompensationsSwitchRCorrEn] = 0x0080;
    compensationsSwitchesByte[CompensationsSwitchRPredEn] = 0x0800;
    compensationsSwitchesByte[CompensationsSwitchCCFastEn] = 0x0020;

    compensationsSwitchesLut[CompensationsSwitchCFastEn] = CompensationPipette;
    compensationsSwitchesLut[CompensationsSwitchCSlowEn] = CompensationMembrane;
    compensationsSwitchesLut[CompensationsSwitchRCorrEn] = CompensationResistanceCorrection;
    compensationsSwitchesLut[CompensationsSwitchRPredEn] = CompensationResistancePrediction;
    compensationsSwitchesLut[CompensationsSwitchCCFastEn] = CompensationCCPipette;

    compensationsSwitchesLutStrings[CompensationsSwitchCFastEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchCSlowEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchRCorrEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchRPredEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchCCFastEn] = '1';

    for (unsigned int compensationSwitchIdx = 0; compensationSwitchIdx < CompensationsSwitchesNum; compensationSwitchIdx++) {
        if (compensationsSwitchesLutStrings[compensationSwitchIdx] == '1') {
            compensationsSwitchesEnableSignArray[compensationSwitchIdx] = true;

        } else {
            compensationsSwitchesEnableSignArray[compensationSwitchIdx] = false;
        }
    }

    /*! Compensations options */
    resistanceCorrectionOptions.resize(8);
    resistanceCorrectionOptions[0] = "BW 40kHz";
    resistanceCorrectionOptions[1] = "BW 20kHz";
    resistanceCorrectionOptions[2] = "BW 10kHz";
    resistanceCorrectionOptions[3] = "BW 5kHz";
    resistanceCorrectionOptions[4] = "BW 2.5kHz";
    resistanceCorrectionOptions[5] = "BW 1.25kHz";
    resistanceCorrectionOptions[6] = "BW 625Hz";
    resistanceCorrectionOptions[7] = "BW 312Hz";
    resistanceCorrectionOptionWord = 2;
    resistanceCorrectionOptionMask = 0x0700;
    resistanceCorrectionOptionOffset = 8;

    /*! Compensations values */
    compensationsRegistersNum = CompensationsRegistersNum;
    coreSpecificRegistersNum = compensationsRegistersNum+7; /*! additional 7 words are for VC (2) and CC (2) hold tuner, VC dig off start (1), VC (1) and CC (1) offset delta */

    liquidJunctionOffsetBinary = SHORT_OFFSET_BINARY;
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

    resistancePredictionGainControl.implemented = true;
    resistancePredictionGainControl.min = minResistancePredictionGain;
    resistancePredictionGainControl.value = minResistancePredictionGain;
    resistancePredictionGainControl.max = maxResistancePredictionGain;
    resistancePredictionGainControl.compensable = maxResistancePredictionGain;
    resistancePredictionGainControl.steps = resistancePredictionGainSteps;
    resistancePredictionGainControl.step = resistancePredictionGainStep;
    resistancePredictionGainControl.decimals = 2;
    resistancePredictionGainControl.prefix = resistancePredictionGainPrefix;
    resistancePredictionGainControl.unit = resistancePredictionGainUnit;
    resistancePredictionGainControl.name = resistancePredictionGainName;

    resistancePredictionTauControl.implemented = false;
    resistancePredictionTauControl.min = minResistancePredictionTau;
    resistancePredictionTauControl.value = minResistancePredictionTau;
    resistancePredictionTauControl.max = maxResistancePredictionTau;
    resistancePredictionTauControl.compensable = maxResistancePredictionTau;
    resistancePredictionTauControl.steps = resistancePredictionTauSteps;
    resistancePredictionTauControl.step = resistancePredictionTauStep;
    resistancePredictionTauControl.decimals = resistancePredictionTauDecimals;
    resistancePredictionTauControl.prefix = resistancePredictionTauPrefix;
    resistancePredictionTauControl.unit = resistancePredictionTauUnit;
    resistancePredictionTauControl.name = resistancePredictionTauName;

    ccPipetteCapacitanceControl.implemented = true;
    ccPipetteCapacitanceControl.min = minCcPipetteCapacitance;
    ccPipetteCapacitanceControl.value = minCcPipetteCapacitance;
    ccPipetteCapacitanceControl.max = maxCcPipetteCapacitance;
    ccPipetteCapacitanceControl.compensable = maxCcPipetteCapacitance;
    ccPipetteCapacitanceControl.steps = ccPipetteCapacitanceSteps;
    ccPipetteCapacitanceControl.step = ccPipetteCapacitanceStep;
    ccPipetteCapacitanceControl.decimals = ccPipetteCapacitanceDecimals;
    ccPipetteCapacitanceControl.prefix = ccPipetteCapacitancePrefix;
    ccPipetteCapacitanceControl.unit = ccPipetteCapacitanceUnit;
    ccPipetteCapacitanceControl.name = ccPipetteCapacitanceName;

    /*! Zap command */
    zapDurationHwRegisterOffset = 0;

    /*! Channel sources */
    uint16_t channelSourceHwRegisterOffset = 2;
    selectableTotalChannelsNum = ChannelSourcesNum;
    selectableCurrentChannelsNum = 1;
    selectableVoltageChannelsNum = 1;
    channelSourcesRegisters.resize(selectableTotalChannelsNum);
    channelSourcesRegisters[ChannelSourceVoltage0] = channelSourceHwRegisterOffset;
    channelSourcesRegisters[ChannelSourceCurrent0] = channelSourceHwRegisterOffset+1;

    /*! Voltage holding tuner command */
    voltageHoldTunerImplemented = true;
    vcHoldTunerHwRegisterOffset = 10;

    /*! Current holding tuner command */
    currentHoldTunerImplemented = true;
    ccHoldTunerHwRegisterOffset = 12;

    /*! Reset commands */
    resetWord[ResetIndexChip] = 0;
    resetWord[ResetIndexDigitalOffsetCompensation] = 0;

    resetByte[ResetIndexChip] = 0x0001;
    resetByte[ResetIndexDigitalOffsetCompensation] = 0x0002;

    resetDuration = 20;

    /*! Stimulus */
    /*! VC */
    vcStimulusSwitchesNum = VCStimulusSwitchesNum;
    vcStimulusSwitchesLut.resize(vcStimulusSwitchesNum);
    vcStimulusSwitchesWord.resize(vcStimulusSwitchesNum);
    vcStimulusSwitchesByte.resize(vcStimulusSwitchesNum);

    vcStimulusSwitchesWord[VCStimulusSwitch_VcSW] = 2;

    vcStimulusSwitchesByte[VCStimulusSwitch_VcSW] = 0x0004;

    vcStimulusSwitchesLutStrings = "1";

    for (unsigned int stimulusSwitchesIdx = 0; stimulusSwitchesIdx < vcStimulusSwitchesNum; stimulusSwitchesIdx++) {
        if (vcStimulusSwitchesLutStrings[stimulusSwitchesIdx] == '1') {
            vcStimulusSwitchesLut[stimulusSwitchesIdx] = true;

        } else {
            vcStimulusSwitchesLut[stimulusSwitchesIdx] = false;
        }
    }

    /*! CC */
    ccStimulusSwitchesNum = CCStimulusSwitchesNum;
    ccStimulusSwitchesLut.resize(ccStimulusSwitchesNum);
    ccStimulusSwitchesWord.resize(ccStimulusSwitchesNum);
    ccStimulusSwitchesByte.resize(ccStimulusSwitchesNum);

    ccStimulusSwitchesWord[CCStimulusSwitch_StimEN] = 2;

    ccStimulusSwitchesByte[CCStimulusSwitch_StimEN] = 0x0010;

    ccStimulusSwitchesLutStrings = "1";

    for (unsigned int stimulusSwitchesIdx = 0; stimulusSwitchesIdx < ccStimulusSwitchesNum; stimulusSwitchesIdx++) {
        if (ccStimulusSwitchesLutStrings[stimulusSwitchesIdx] == '1') {
            ccStimulusSwitchesLut[stimulusSwitchesIdx] = true;

        } else {
            ccStimulusSwitchesLut[stimulusSwitchesIdx] = false;
        }
    }

    /*! Reader */
    /*! VC */
    vcReaderSwitchesNum = VCReaderSwitchesNum;
    vcReaderSwitchesLut.resize(vcReaderSwitchesNum);
    vcReaderSwitchesWord.resize(vcReaderSwitchesNum);
    vcReaderSwitchesByte.resize(vcReaderSwitchesNum);
    vcReaderSwitchChannelIndependent = false;

    vcReaderSwitchesWord[VCReaderSwitch_VcSel] = 0;

    vcReaderSwitchesByte[VCReaderSwitch_VcSel] = 0x0004;

    vcReaderSwitchesLutStrings = "1";

    for (unsigned int readerSwitchesIdx = 0; readerSwitchesIdx < vcReaderSwitchesNum; readerSwitchesIdx++) {
        if (vcReaderSwitchesLutStrings[readerSwitchesIdx] == '1') {
            vcReaderSwitchesLut[readerSwitchesIdx] = true;

        } else {
            vcReaderSwitchesLut[readerSwitchesIdx] = false;
        }
    }

    /*! CC */
    ccReaderSwitchesNum = CCReaderSwitchesNum;
    ccReaderSwitchesLut.resize(ccReaderSwitchesNum);
    ccReaderSwitchesWord.resize(ccReaderSwitchesNum);
    ccReaderSwitchesByte.resize(ccReaderSwitchesNum);

    ccReaderSwitchesWord[CCReaderSwitch_CcSW] = 2;

    ccReaderSwitchesByte[CCReaderSwitch_CcSW] = 0x0008;

    ccReaderSwitchesLutStrings = "1";

    for (unsigned int readerSwitchesIdx = 0; readerSwitchesIdx < ccReaderSwitchesNum; readerSwitchesIdx++) {
        if (ccReaderSwitchesLutStrings[readerSwitchesIdx] == '1') {
            ccReaderSwitchesLut[readerSwitchesIdx] = true;

        } else {
            ccReaderSwitchesLut[readerSwitchesIdx] = false;
        }
    }

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

    maxDigitalTriggerOutputEvents = 21;
    digitalTriggersNum = 1;

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
    currentRange = vcCurrentRangesArray[VCCurrentRange10nA];
    voltageRange = vcVoltageRangesArray[VCVoltageRange500mV];
    currentResolution = currentRange.step;
    voltageResolution = voltageRange.step;
    samplingRate = realSamplingRatesArray[SamplingRate6_25kHz];
}

EZPatche8PPatchliner_el07ab::~EZPatche8PPatchliner_el07ab() {

}

ErrorCodes_t EZPatche8PPatchliner_el07ab::getNextMessage(RxOutput_t &rxOutput, int16_t * data) {
    ErrorCodes_t ret = Success;
    double xFlt;

    std::unique_lock <std::mutex> rxMutexLock (rxMutex);
    if (rxMsgBufferReadLength <= 0) {
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point currentTime = startTime;
        while ((std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count()) < EZP_NO_DATA_WAIT_TIME_MS) {
            rxMsgBufferNotEmpty.wait_for(rxMutexLock, std::chrono::milliseconds(EZP_NO_DATA_WAIT_TIME_MS));
            currentTime = std::chrono::steady_clock::now();
        }
        if (rxMsgBufferReadLength <= 0) {
            return ErrorNoDataAvailable;
        }
    }
    uint32_t maxMsgRead = rxMsgBufferReadLength;
    rxMutexLock.unlock();

    if (!parsingFlag) {
        return ErrorDeviceNotConnected;
    }

    uint32_t msgReadCount = 0;
    bool headerSet = false;
    bool tailSet = false;

    rxOutput.dataLen = 0; /*! Initialize data length in case more messages are merged */
    bool exitLoop = false;

    while (msgReadCount < maxMsgRead) {
        if (rxOutput.msgTypeId == rxMsgBuffer[rxMsgBufferReadOffset].typeId) {
            switch (rxOutput.msgTypeId) {
            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData):
                if ((msgReadCount+1)*rxDataMessageMaxLen > E384CL_OUT_STRUCT_DATA_LEN) {
                    /*! Exit if the merge may exceed the data field length */
                    exitLoop = true;

                } else {
                    exitLoop = false;
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
                exitLoop = false;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdFpgaReset):
            case (MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp):
            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
            default:
                /*! Messages of these types can't be merged. */
                if (msgReadCount == 0) {
                    /*! If no messages have been read yet process it... */
                    exitLoop = false;

                } else {
                    /*! otherwise exit and force the beginning of another packet */
                    exitLoop = true;
                }
                break;
            }

        } else {
            /*! If new message type is different from previous one stay and read only if no other messages have been read yet */
            if (msgReadCount > 0) {
                /*! Otherwise exit or messages of different types will be merged */
                exitLoop = true;
            }
        }

        if (exitLoop) {
            break;

        } else {
            rxOutput.msgTypeId = rxMsgBuffer[rxMsgBufferReadOffset].typeId;
            uint32_t dataOffset = rxMsgBuffer[rxMsgBufferReadOffset].startDataPtr;
            uint16_t samplesNum;
            uint16_t sampleIdx = 0;
            uint16_t timeSamplesNum;
            uint16_t rawFloat;
            uint16_t dataWritten;

            switch (rxOutput.msgTypeId) {
            case (MsgDirectionDeviceToPc+MsgTypeIdFpgaReset):
                lastParsedMsgType = MsgTypeIdFpgaReset;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdDigitalOffsetComp):
                rxOutput.dataLen = 1;
                rxOutput.channelIdx = * (rxDataBuffer+dataOffset);
                rawFloat = * (rxDataBuffer+((dataOffset+1)&EZP_RX_DATA_BUFFER_MASK));
                voltageOffsetCorrected = (((double)rawFloat)-liquidJunctionOffsetBinary)*liquidJunctionResolution;
                this->selectVoltageOffsetResolution();
                data[0] = (int16_t)(rawFloat-liquidJunctionOffsetBinary);
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;

//                this->setDigitalOffsetCompensationOverrideValue(rxOutput.channelIdx, {voltageOffsetCorrected, liquidJunctionControl.prefix, "V"}); /*! \todo FCON capire perchè stato messo questo comando durante il porting su e384commlib */
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader):
                if (lastParsedMsgType != MsgTypeIdAcquisitionHeader) {
                    /*! Evaluate only if it's the first repetition */
                    rxOutput.dataLen = 0;
                    rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                    rxOutput.protocolItemIdx = * (rxDataBuffer+((dataOffset+1)&EZP_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolRepsIdx = * (rxDataBuffer+((dataOffset+2)&EZP_RX_DATA_BUFFER_MASK));
                    rxOutput.protocolSweepIdx = * (rxDataBuffer+((dataOffset+3)&EZP_RX_DATA_BUFFER_MASK));
                    lastParsedMsgType = MsgTypeIdAcquisitionHeader;
                    headerSet = true;

                } else {
                    if (!headerSet) {
                        /*! This message is a repetition and should not be passed to SW only if the first header message was not processed (headerSet) during this call */
                        ret = ErrorRepeatedHeader;
                    }
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData):
                if (rxMsgBuffer[rxMsgBufferReadOffset].dataLength < EZP_RX_MIN_DATA_PACKET_VALID_LEN) {
                    ret = ErrorIllFormedMessage;

                } else {
                    samplesNum = rxMsgBuffer[rxMsgBufferReadOffset].dataLength-2; /*!< The first 2 samples are used for firstSampleOffset */
                    dataWritten = rxOutput.dataLen;
                    rxOutput.dataLen += samplesNum;
                    timeSamplesNum = samplesNum/totalChannelsNum;
                    if (msgReadCount == 0) {
                        /*! Update firstSampleOffset only if it is not merging messages */
                        rxOutput.firstSampleOffset = * ((uint32_t *)(rxDataBuffer+dataOffset));
                    }
                    dataOffset = (dataOffset+2)&EZP_RX_DATA_BUFFER_MASK;

                    for (uint16_t idx = 0; idx < timeSamplesNum; idx++) {
                        /*! \todo FCON questo doppio ciclo va modificato per raccogliere i dati di impedenza in modalità lock-in */
                        for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyRawDataFilter(channelIdx, (((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx])+(voltageOffsetCorrection+voltageTunerCorrection[channelIdx])/voltageResolution, iirVNum, iirVDen);
                            xFlt = iirY[channelIdx][iirOff];
                            data[dataWritten+sampleIdx] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&EZP_LSB_NOISE_ARRAY_MASK;

                            rawFloat = * (rxDataBuffer+dataOffset);
                            this->applyRawDataFilter(channelIdx+voltageChannelsNum, (((double)rawFloat)-SHORT_OFFSET_BINARY+lsbNoiseArray[lsbNoiseIdx])+currentTunerCorrection[channelIdx]/currentResolution, iirINum, iirIDen);
                            xFlt = iirY[channelIdx+voltageChannelsNum][iirOff];
                            data[dataWritten+sampleIdx+voltageChannelsNum] = (int16_t)round(xFlt > SHORT_MAX ? SHORT_MAX : (xFlt < SHORT_MIN ? SHORT_MIN : xFlt));
                            dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;
                            lsbNoiseIdx = (lsbNoiseIdx+1)&EZP_LSB_NOISE_ARRAY_MASK;

                            if (anyLiquidJuctionActive) {
                                liquidJunctionCurrentSums[channelIdx] += (int64_t)data[dataWritten+sampleIdx+voltageChannelsNum];
                            }

                            sampleIdx++;
                        }

                        if (anyLiquidJuctionActive) {
                            liquidJunctionCurrentEstimatesNum++;
                        }
                        sampleIdx += currentChannelsNum;

                        if (iirOff < 1) {
                            iirOff = IIR_ORD;

                        } else {
                            iirOff--;
                        }
                    }
                }
                lastParsedMsgType = MsgTypeIdAcquisitionData;
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail):
                if (lastParsedMsgType != MsgTypeIdAcquisitionTail) {
                    /*! Evaluate only if it's the first repetition */
                    rxOutput.dataLen = 0;
                    rxOutput.protocolId = * (rxDataBuffer+dataOffset);
                    lastParsedMsgType = MsgTypeIdAcquisitionTail;
                    tailSet = true;

                } else {
                    if (!tailSet) {
                        /*! This message is a repetition and should not be passed to SW only if the first tail message was not processed (tailSet) during this call */
                        ret = ErrorRepeatedTail;
                    }
                }
                break;

            case (MsgDirectionDeviceToPc+MsgTypeIdAcquisitionSaturation):
                rxOutput.dataLen = rxMsgBuffer[rxMsgBufferReadOffset].dataLength;
                for (uint16_t dataIdx = 0; dataIdx < rxOutput.dataLen; dataIdx++) {
//                    rxOutput.uintData[dataIdx] = * (rxDataBuffer+dataOffset); /* \todo FCON al momento della saturazione non frega niente a nessuno */
                    dataOffset = (dataOffset+1)&EZP_RX_DATA_BUFFER_MASK;
                }
                lastParsedMsgType = rxOutput.msgTypeId-MsgDirectionDeviceToPc;
                break;

            default:
                lastParsedMsgType = MsgTypeIdInvalid;
                break;
            }

            msgReadCount++;
            rxMsgBufferReadOffset = (rxMsgBufferReadOffset+1)&EZP_RX_MSG_BUFFER_MASK;
        }
    }

    rxMutexLock.lock();
    rxMsgBufferReadLength -= msgReadCount;
    rxMsgBufferNotFull.notify_all();
    rxMutexLock.unlock();

    return ret;
}

ErrorCodes_t EZPatche8PPatchliner_el07ab::setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) {
    ErrorCodes_t ret;

    if (samplingRateIdx < samplingRatesNum) {
        uint16_t dataLength = switchesStatusLength;
        std::vector <uint16_t> txDataMessage(dataLength);
        this->switches2DataMessage(txDataMessage);

        for (unsigned int samplingRatesSwitchIdx = 0; samplingRatesSwitchIdx < samplingRatesSwitchesNum; samplingRatesSwitchIdx++) {
            if (samplingRatesSwitchesLut[samplingRateIdx][samplingRatesSwitchIdx]) {
                txDataMessage[samplingRatesSwitchesWord[samplingRatesSwitchIdx]] |=
                        samplingRatesSwitchesByte[samplingRatesSwitchIdx]; // 1

            } else {
                txDataMessage[samplingRatesSwitchesWord[samplingRatesSwitchIdx]] &=
                        ~samplingRatesSwitchesByte[samplingRatesSwitchIdx]; // 0
            }
        }

        ret = this->manageOutgoingMessageLife(MsgDirectionPcToDevice+MsgTypeIdSwitchCtrl, txDataMessage, dataLength);
        if (ret == Success) {
            this->dataMessage2Switches(txDataMessage);
            ret = EZPatchDevice::setSamplingRate(samplingRateIdx, applyFlag);
        }

    } else {
        ret = ErrorValueOutOfRange;
    }

    return ret;
}

ErrorCodes_t EZPatche8PPatchliner_el07ab::hasCompFeature(uint16_t feature) {
    switch (feature) {
    case U_CpVc:
    case U_Cm:
    case U_Rs:
    case U_RsCp:
    case U_RsPg:
    case U_CpCc:
        return Success;

    default:
        return ErrorFeatureNotImplemented;
    }
}

void EZPatche8PPatchliner_el07ab::selectChannelsResolutions() {
    for (unsigned int channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        if (selectedCurrentSourceIdx == ChannelSourceCurrentFromVoltageClamp) {
            currentTunerCorrection[channelIdx] = 0.0;
            rawDataFilterCurrentFlag = true;

        } else if (selectedCurrentSourceIdx == ChannelSourceCurrentFromCurrentClamp) {
            currentTunerCorrection[channelIdx] = selectedCurrentHoldVector[channelIdx].value;
            rawDataFilterCurrentFlag = false;
        }

        if (selectedVoltageSourceIdx == ChannelSourceVoltageFromVoltageClamp) {
            voltageTunerCorrection[channelIdx] = selectedVoltageHoldVector[channelIdx].value;
            rawDataFilterVoltageFlag = false;

        } else if (selectedVoltageSourceIdx == ChannelSourceVoltageFromCurrentClamp) {
            voltageTunerCorrection[channelIdx] = 0.0;
            rawDataFilterVoltageFlag = true;
        }
    }

    this->selectVoltageOffsetResolution();
    /*! voltageResolution does not depend on selected source cause we want to get it from current clamp front end */

    this->computeRawDataFilterCoefficients();
}

void EZPatche8PPatchliner_el07ab::selectVoltageOffsetResolution() {
    if (selectedVoltageSourceIdx == ChannelSourceVoltageFromVoltageClamp) {
        voltageOffsetCorrection = 0.0;

    } else {
        Measurement_t correctedValue;
        correctedValue.value = voltageOffsetCorrected;
        correctedValue.prefix = liquidJunctionPrefix;
        correctedValue.convertValue(voltageRange.prefix);
        voltageOffsetCorrection = correctedValue.value;
    }
}

void EZPatche8PPatchliner_el07ab::initializeCompensations() {
    EZPatchDevice::initializeCompensations();

    pipetteCapacitanceRegValue.resize(currentChannelsNum);
    ccPipetteCapacitanceRegValue.resize(currentChannelsNum);
    membraneCapacitanceRegValue.resize(currentChannelsNum);
    membraneTauRegValue.resize(currentChannelsNum);
    correctionGainRegValue.resize(currentChannelsNum);
    predictionGainRegValue.resize(currentChannelsNum);
    predictionTauRegValue.resize(currentChannelsNum);

    for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        pipetteCapacitanceRegValue[channelIdx] = 0xFF;
        ccPipetteCapacitanceRegValue[channelIdx] = 0xFF;
        membraneCapacitanceRegValue[channelIdx] = 0xFF;
        membraneTauRegValue[channelIdx] = 0xFF;
        correctionGainRegValue[channelIdx] = 0xFF;
        predictionGainRegValue[channelIdx] = 0xFF;
        predictionTauRegValue[channelIdx] = 0xFF;
    }
}

bool EZPatche8PPatchliner_el07ab::checkCompensationsValues() {
    bool ret = true;
    if (vcCompensationsActivated) {
        if (compCfastEnable[compensationsSettingChannel] && compCslowEnable[compensationsSettingChannel]) {
            if (membraneCapacitance[compensationsSettingChannel] > maxMembraneCapacitance3) {
                additionalPipetteCapacitanceFromMembrane = membraneInjCapacitance4;

            } else if (membraneCapacitance[compensationsSettingChannel] > maxMembraneCapacitance2) {
                additionalPipetteCapacitanceFromMembrane = membraneInjCapacitance3;

            } else if (membraneCapacitance[compensationsSettingChannel] > maxMembraneCapacitance1) {
                additionalPipetteCapacitanceFromMembrane = membraneInjCapacitance2;

            } else {
                additionalPipetteCapacitanceFromMembrane = membraneInjCapacitance1;
            }

        } else {
            additionalPipetteCapacitanceFromMembrane = 0.0;
        }

        if (compCfastEnable[compensationsSettingChannel]){
            ret &= (pipetteCapacitance[compensationsSettingChannel]+additionalPipetteCapacitanceFromMembrane > (minPipetteCapacitance-0.5*pipetteCapacitanceStep) &&
                    pipetteCapacitance[compensationsSettingChannel]+additionalPipetteCapacitanceFromMembrane < (maxPipetteCapacitance+0.5*pipetteCapacitanceStep));
        }

        double membraneTau;
        if (compCslowEnable[compensationsSettingChannel]) {
            ret &= (membraneCapacitance[compensationsSettingChannel] > (minMembraneCapacitance-0.5*membraneCapacitanceStep) &&
                    membraneCapacitance[compensationsSettingChannel] < (maxMembraneCapacitance+0.5*membraneCapacitanceStep));
            membraneTau = membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel];
            ret &= membraneTau < (maxMembraneTau+0.5*membraneTauStep);

        } else {
            membraneTau = 0.0;
        }

        if (compRsCorrEnable[compensationsSettingChannel]) {
            double resistanceCorrected = accessResistance[compensationsSettingChannel]*resistanceCorrectionPercentage[compensationsSettingChannel]/maxResistanceCorrectionPercentage; /*! Missing the minimum check because for this device this is handled when building the packet */
            ret &= (resistanceCorrected < (0.001*minResistanceCorrection+1.001*maxResistanceCorrection)); /*! Missing the minimum check because for this device this is handled when building the packet */
        }

        if (compRsPredEnable[compensationsSettingChannel]) {
            ret &= (resistancePredictionGain[compensationsSettingChannel] > (minResistancePredictionGain-0.5*resistancePredictionGainStep) &&
                    resistancePredictionGain[compensationsSettingChannel] < (maxResistancePredictionGain+0.5*resistancePredictionGainStep));
            double resistancePredictedTau = membraneTau/resistancePredictionGain[compensationsSettingChannel];
            ret &= resistancePredictedTau < (maxResistancePredictionTau+0.5*resistancePredictionTauStep); /*! Missing the minimum check because for this device this is handled when building the packet */
        }
    }

    if (ccCompensationsActivated) {
        ret &= (ccPipetteCapacitance[compensationsSettingChannel] > (minCcPipetteCapacitance-0.5*ccPipetteCapacitanceStep) &&
                ccPipetteCapacitance[compensationsSettingChannel] < (maxCcPipetteCapacitance+0.5*ccPipetteCapacitanceStep));
    }

    if (vcCompensationsActivated) {
        double rs = fmax(accessResistance[compensationsSettingChannel], accessResistanceControl.step);
        double rc = fmax(resistanceCorrectionPercentage[compensationsSettingChannel], resistanceCorrectionPercentageControl.step);
        double rp = fmax(resistancePredictionPercentage[compensationsSettingChannel], resistancePredictionPercentageControl.step);

        pipetteCapacitanceControl.compensable = fmax(0.0, maxPipetteCapacitance-additionalPipetteCapacitanceFromMembrane);

        double d1 = maxMembraneCapacitance;
        double d2 = maxMembraneTau/rs;
        double d3 = (compRsPredEnable[compensationsSettingChannel] ? maxResistancePredictionTau*resistancePredictionGain[compensationsSettingChannel]/(rs*rp) : std::numeric_limits <double>::max());
        membraneCapacitanceControl.compensable = fmin(fmin(d1, d2), d3);

        d1 = accessResistanceControl.max;
        d2 = maxMembraneTau/membraneCapacitance[compensationsSettingChannel];
        d3 = maxResistanceCorrection*maxResistanceCorrectionPercentage/rc;
        double d4 = (compRsPredEnable[compensationsSettingChannel] ? maxResistancePredictionTau*resistancePredictionGain[compensationsSettingChannel]/(membraneCapacitance[compensationsSettingChannel]*rp) : std::numeric_limits <double>::max());
        accessResistanceControl.compensable = fmin(fmin(fmin(d1, d2), d3), d4);

        d1 = resistanceCorrectionPercentageControl.max;
        d2 = maxResistanceCorrection*maxResistanceCorrectionPercentage/rs;
        resistanceCorrectionPercentageControl.compensable = fmin(d1, d2);
    }

    if (ccCompensationsActivated) {
        /*! ccPipetteCapacitanceControl.compensable doesn't depend on other controls */
    }

    return ret;
}

bool EZPatche8PPatchliner_el07ab::fillCompensationsRegistersTxData(std::vector <uint16_t> &txDataMessage) {
    bool anythingChanged = false;

    double pipetteValue;
    uint16_t pipetteRange;
    double totalPipetteCapacitance = pipetteCapacitance[compensationsSettingChannel]+additionalPipetteCapacitanceFromMembrane;
    if (totalPipetteCapacitance > maxPipetteCapacitance3) {
        if (totalPipetteCapacitance < minPipetteCapacitance4) {
            pipetteValue = 0x0000;

        } else {
            pipetteValue = (totalPipetteCapacitance-minPipetteCapacitance4)/pipetteCapacitanceStep4;
        }
        pipetteRange = 0x00C0;

    } else if (totalPipetteCapacitance > maxPipetteCapacitance2) {
        if (totalPipetteCapacitance < minPipetteCapacitance3) {
            pipetteValue = 0x0000;

        } else {
            pipetteValue = (totalPipetteCapacitance-minPipetteCapacitance3)/pipetteCapacitanceStep3;
        }
        pipetteRange = 0x0080;

    } else if (totalPipetteCapacitance > maxPipetteCapacitance1) {
        if (totalPipetteCapacitance < minPipetteCapacitance2) {
            pipetteValue = 0x0000;

        } else {
            pipetteValue = (totalPipetteCapacitance-minPipetteCapacitance2)/pipetteCapacitanceStep2;
        }
        pipetteRange = 0x0040;

    } else {
        if (totalPipetteCapacitance < minPipetteCapacitance1) {
            pipetteValue = 0x0000;

        } else {
            pipetteValue = (totalPipetteCapacitance-minPipetteCapacitance1)/pipetteCapacitanceStep1;
        }
        pipetteRange = 0x0000;
    }

    double membraneValue;
    uint16_t membraneRange;
    if (membraneCapacitance[compensationsSettingChannel] > maxMembraneCapacitance3) {
        if (membraneCapacitance[compensationsSettingChannel] < minMembraneCapacitance4) {
            membraneValue = 0x0000;

        } else {
            membraneValue = (membraneCapacitance[compensationsSettingChannel]-minMembraneCapacitance4)/membraneCapacitanceStep4;
        }
        membraneRange = 0x00C0;

    } else if (membraneCapacitance[compensationsSettingChannel] > maxMembraneCapacitance2) {
        if (membraneCapacitance[compensationsSettingChannel] < minMembraneCapacitance3) {
            membraneValue = 0x0000;

        } else {
            membraneValue = (membraneCapacitance[compensationsSettingChannel]-minMembraneCapacitance3)/membraneCapacitanceStep3;
        }
        membraneRange = 0x0080;

    } else if (membraneCapacitance[compensationsSettingChannel] > maxMembraneCapacitance1) {
        if (membraneCapacitance[compensationsSettingChannel] < minMembraneCapacitance2) {
            membraneValue = 0x0000;

        } else {
            membraneValue = (membraneCapacitance[compensationsSettingChannel]-minMembraneCapacitance2)/membraneCapacitanceStep2;
        }
        membraneRange = 0x0040;

    } else {
        if (membraneCapacitance[compensationsSettingChannel] < minMembraneCapacitance1) {
            membraneValue = 0x0000;

        } else {
            membraneValue = (membraneCapacitance[compensationsSettingChannel]-minMembraneCapacitance1)/membraneCapacitanceStep1;
        }
        membraneRange = 0x0000;
    }

    double membraneTauValue;
    uint16_t membraneTauRange;
    if (membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel] > maxMembraneTau1) {
        if (membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel] < minMembraneTau2) {
            membraneTauValue = 0x0000;

        } else {
            membraneTauValue = (membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel]-minMembraneTau2)/membraneTauStep2;
        }
        membraneTauRange = 0x0100;

    } else if (membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel] > minMembraneTau1) {
        if (membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel] < minMembraneTau1) {
            membraneTauValue = 0x0000;

        } else {
            membraneTauValue = (membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel]-minMembraneTau1)/membraneTauStep1;
        }
        membraneTauRange = 0x0000;

    } else {
        membraneTauValue = 0x0000;
        membraneTauRange = 0x0000;
    }

    double correctedResistance = resistanceCorrectionPercentage[compensationsSettingChannel]/maxResistanceCorrectionPercentage*accessResistance[compensationsSettingChannel];
    if (correctedResistance < minResistanceCorrection) {
        correctedResistance = minResistanceCorrection;
    }

    double ccPipetteValue;
    uint16_t ccPipetteRange;
    if (ccPipetteCapacitance[compensationsSettingChannel] > maxPipetteCapacitance3) {
        if (ccPipetteCapacitance[compensationsSettingChannel] < minPipetteCapacitance4) {
            ccPipetteValue = 0x0000;

        } else {
            ccPipetteValue = (ccPipetteCapacitance[compensationsSettingChannel]-minPipetteCapacitance4)/pipetteCapacitanceStep4;
        }
        ccPipetteRange = 0x00C0;

    } else if (ccPipetteCapacitance[compensationsSettingChannel] > maxPipetteCapacitance2) {
        if (ccPipetteCapacitance[compensationsSettingChannel] < minPipetteCapacitance3) {
            ccPipetteValue = 0x0000;

        } else {
            ccPipetteValue = (ccPipetteCapacitance[compensationsSettingChannel]-minPipetteCapacitance3)/pipetteCapacitanceStep3;
        }
        ccPipetteRange = 0x0080;

    } else if (ccPipetteCapacitance[compensationsSettingChannel] > maxPipetteCapacitance1) {
        if (ccPipetteCapacitance[compensationsSettingChannel] < minPipetteCapacitance2) {
            ccPipetteValue = 0x0000;

        } else {
            ccPipetteValue = (ccPipetteCapacitance[compensationsSettingChannel]-minPipetteCapacitance2)/pipetteCapacitanceStep2;
        }
        ccPipetteRange = 0x0040;

    } else {
        if (ccPipetteCapacitance[compensationsSettingChannel] < minPipetteCapacitance1) {
            ccPipetteValue = 0x0000;

        } else {
            ccPipetteValue = (ccPipetteCapacitance[compensationsSettingChannel]-minPipetteCapacitance1)/pipetteCapacitanceStep1;
        }
        ccPipetteRange = 0x0000;
    }

    if (vcCompensationsActivated) {
        txDataMessage[0] = CompensationsRegisterVCCFastGain+compensationsSettingChannel*coreSpecificRegistersNum;
        txDataMessage[1] = ((vcCompensationsActivated & compCfastEnable[compensationsSettingChannel]) ? pipetteRange | (uint16_t)round(pipetteValue) : 0);

    } else if (ccCompensationsActivated) {
        txDataMessage[0] = CompensationsRegisterCCCFastGain+compensationsSettingChannel*coreSpecificRegistersNum;
        txDataMessage[1] = ((ccCompensationsActivated & compCcCfastEnable[compensationsSettingChannel]) ? ccPipetteRange | (uint16_t)round(ccPipetteValue) : 0);
    }
    txDataMessage[2] = CompensationsRegisterVCCSlowGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[3] = ((vcCompensationsActivated & compCslowEnable[compensationsSettingChannel]) ? membraneRange | (uint16_t)round(membraneValue) : 0);
    txDataMessage[4] = CompensationsRegisterVCCslowTau+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[5] = ((vcCompensationsActivated & compCslowEnable[compensationsSettingChannel]) ? membraneTauRange | (uint16_t)round(membraneTauValue) : 0);
    txDataMessage[6] = CompensationsRegisterVCRCorrGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[7] = ((vcCompensationsActivated & compRsCorrEnable[compensationsSettingChannel]) ? (uint16_t)round((correctedResistance-minResistanceCorrection)/resistanceCorrectionStep) : 0);
    txDataMessage[8] = CompensationsRegisterVCRPredGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[9] = ((vcCompensationsActivated & compRsPredEnable[compensationsSettingChannel]) ? (uint16_t)round((resistancePredictionGain[compensationsSettingChannel]-minResistancePredictionGain)/resistancePredictionGainStep) : 0);
    txDataMessage[10] = CompensationsRegisterVCRPredTau+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[11] = ((vcCompensationsActivated & compRsPredEnable[compensationsSettingChannel]) ? (uint16_t)round((membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel]-minResistancePredictionTau)/resistancePredictionGain[compensationsSettingChannel]/resistancePredictionTauStep) : 0);

    if (vcCompensationsActivated) {
        if (txDataMessage[1] != pipetteCapacitanceRegValue[compensationsSettingChannel] ||
                txDataMessage[3] != membraneCapacitanceRegValue[compensationsSettingChannel] ||
                txDataMessage[5] != membraneTauRegValue[compensationsSettingChannel] ||
                txDataMessage[7] != correctionGainRegValue[compensationsSettingChannel] ||
                txDataMessage[9] != predictionGainRegValue[compensationsSettingChannel] ||
                txDataMessage[11] != predictionTauRegValue[compensationsSettingChannel]) {
            anythingChanged = true;
        }

    } else if (ccCompensationsActivated) {
        if (txDataMessage[1] != ccPipetteCapacitanceRegValue[compensationsSettingChannel] ||
                txDataMessage[3] != membraneCapacitanceRegValue[compensationsSettingChannel] ||
                txDataMessage[5] != membraneTauRegValue[compensationsSettingChannel] ||
                txDataMessage[7] != correctionGainRegValue[compensationsSettingChannel] ||
                txDataMessage[9] != predictionGainRegValue[compensationsSettingChannel] ||
                txDataMessage[11] != predictionTauRegValue[compensationsSettingChannel]) {
            anythingChanged = true;
        }
    }
    return anythingChanged;
}

void EZPatche8PPatchliner_el07ab::updateWrittenCompesantionValues(std::vector <uint16_t> &txDataMessage) {
    if (vcCompensationsActivated) {
        pipetteCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[1];

    } else if (ccCompensationsActivated) {
        ccPipetteCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[1];
    }
    membraneCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[3];
    membraneTauRegValue[compensationsSettingChannel] = txDataMessage[5];
    correctionGainRegValue[compensationsSettingChannel] = txDataMessage[7];
    predictionGainRegValue[compensationsSettingChannel] = txDataMessage[9];
    predictionTauRegValue[compensationsSettingChannel] = txDataMessage[11];
}

EZPatche8PPatchliner_el07ab_artix7_PCBV01::EZPatche8PPatchliner_el07ab_artix7_PCBV01(std::string di) :
    EZPatche8PPatchliner_el07ab(di) {

    spiChannel = 'B';
    rxChannel = 'A';
    txChannel = 'A';

    fpgaLoadType = FpgaFwLoadPatchlinerArtix7_V01;
}

EZPatche8PPatchliner_el07ab_artix7_PCBV01::~EZPatche8PPatchliner_el07ab_artix7_PCBV01() {

}

EZPatche8PPatchliner_el07ab_artix7_PCBV02::EZPatche8PPatchliner_el07ab_artix7_PCBV02(std::string di) :
    EZPatche8PPatchliner_el07ab_artix7_PCBV01(di) {

    fpgaLoadType = FpgaFwLoadAutomatic;
}

EZPatche8PPatchliner_el07ab_artix7_PCBV02::~EZPatche8PPatchliner_el07ab_artix7_PCBV02() {

}
