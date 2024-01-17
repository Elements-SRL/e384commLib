#include "ezpatchepatchel04f.h"

EZPatchePatchEL04F::EZPatchePatchEL04F(std::string di) :
    EZPatchFtdiDevice(di) {

    deviceName = "ePatchEL04F";

    /*! Clamping modalities */
    clampingModalitiesArray.resize(3);
    clampingModalitiesArray[0] = VOLTAGE_CLAMP;
    clampingModalitiesArray[1] = ZERO_CURRENT_CLAMP;
    clampingModalitiesArray[2] = CURRENT_CLAMP;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange300pA].max = 300.0;
    vcCurrentRangesArray[VCCurrentRange300pA].min = -300.0;
    vcCurrentRangesArray[VCCurrentRange300pA].step = vcCurrentRangesArray[VCCurrentRange300pA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange300pA].prefix = UnitPfxPico;
    vcCurrentRangesArray[VCCurrentRange300pA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange3nA].max = 3.0;
    vcCurrentRangesArray[VCCurrentRange3nA].min = -3.0;
    vcCurrentRangesArray[VCCurrentRange3nA].step = vcCurrentRangesArray[VCCurrentRange3nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange3nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange3nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange30nA].max = 30.0;
    vcCurrentRangesArray[VCCurrentRange30nA].min = -30.0;
    vcCurrentRangesArray[VCCurrentRange30nA].step = vcCurrentRangesArray[VCCurrentRange30nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange30nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange30nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange300nA].max = 300.0;
    vcCurrentRangesArray[VCCurrentRange300nA].min = -300.0;
    vcCurrentRangesArray[VCCurrentRange300nA].step = vcCurrentRangesArray[VCCurrentRange300nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange300nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange300nA].unit = "A";

    vcCurrentRangesSwitchesLut.resize(vcCurrentRangesNum);

    vcCurrentRangesSwitchesNum = VCCurrentRangesSwitchesNum;
    vcCurrentRangesSwitchesWord.resize(vcCurrentRangesSwitchesNum);
    vcCurrentRangesSwitchesByte.resize(vcCurrentRangesSwitchesNum);

    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange1] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange2] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange3] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRCorrRange1] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRCorrRange2] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchResetPeriod0] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchResetPeriod1] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchResetDuration0] = 1;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchResetDuration1] = 1;

    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange1] = 0x0001;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange2] = 0x0002;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange3] = 0x0004;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRCorrRange1] = 0x0008;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRCorrRange2] = 0x0010;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchResetPeriod0] = 0x0020;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchResetPeriod1] = 0x0040;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchResetDuration0] = 0x0080;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchResetDuration1] = 0x0100;

    vcCurrentRangesSwitchesLutStrings[VCCurrentRange300pA] = "000000111";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange3nA] = "010001011";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange30nA] = "110101011";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange300nA] = "111010011";

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
    ccCurrentRangesArray[CCCurrentRange2500pA].max = 2500.0;
    ccCurrentRangesArray[CCCurrentRange2500pA].min = 2500.0;
    ccCurrentRangesArray[CCCurrentRange2500pA].step = ccCurrentRangesArray[CCCurrentRange2500pA].max/(SHORT_MAX+1.0)*stimulusVoltageReference/stimulusVoltageLimit; /*! partial range used to avoid delta-sigma clipping */
    ccCurrentRangesArray[CCCurrentRange2500pA].prefix = UnitPfxPico;
    ccCurrentRangesArray[CCCurrentRange2500pA].unit = "A";
    ccCurrentRangesArray[CCCurrentRange100nA].max = 100.0;
    ccCurrentRangesArray[CCCurrentRange100nA].min = -100.0;
    ccCurrentRangesArray[CCCurrentRange100nA].step = ccCurrentRangesArray[CCCurrentRange100nA].max/(SHORT_MAX+1.0)*stimulusVoltageReference/stimulusVoltageLimit; /*! partial range used to avoid delta-sigma clipping */
    ccCurrentRangesArray[CCCurrentRange100nA].prefix = UnitPfxNano;
    ccCurrentRangesArray[CCCurrentRange100nA].unit = "A";

    ccCurrentRangesSwitchesLut.resize(ccCurrentRangesNum);

    ccCurrentRangesSwitchesNum = CCCurrentRangesSwitchesNum;
    ccCurrentRangesSwitchesWord.resize(ccCurrentRangesSwitchesNum);
    ccCurrentRangesSwitchesByte.resize(ccCurrentRangesSwitchesNum);

    ccCurrentRangesSwitchesWord[CCCurrentRangesSwitchRange1] = 0;

    ccCurrentRangesSwitchesByte[CCCurrentRangesSwitchRange1] = 0x2000;

    ccCurrentRangesSwitchesLutStrings[CCCurrentRange2500pA] = "0";
    ccCurrentRangesSwitchesLutStrings[CCCurrentRange100nA] = "1";

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
    vcVoltageRangesArray[VCVoltageRange500mV].step = vcVoltageRangesArray[VCVoltageRange500mV].max/(SHORT_MAX+1.0)*stimulusVoltageReference/stimulusVoltageLimit; /*! partial range used to avoid delta-sigma clipping */
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
    ccVoltageRangesArray[CCVoltageRange700mV].max = 700.0;
    ccVoltageRangesArray[CCVoltageRange700mV].min = -700.0;
    ccVoltageRangesArray[CCVoltageRange700mV].step = ccVoltageRangesArray[CCVoltageRange700mV].max/(SHORT_MAX+1.0);
    ccVoltageRangesArray[CCVoltageRange700mV].prefix = UnitPfxMilli;
    ccVoltageRangesArray[CCVoltageRange700mV].unit = "V";

    ccVoltageRangesSwitchesLut.resize(ccVoltageRangesNum);

    ccVoltageRangesSwitchesNum = CCVoltageRangesSwitchesNum;
    ccVoltageRangesSwitchesWord.resize(ccVoltageRangesSwitchesNum);
    ccVoltageRangesSwitchesByte.resize(ccVoltageRangesSwitchesNum);

    /*! No words/bytes to set */
    ccVoltageRangesSwitchesLutStrings[CCVoltageRange700mV] = "";

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

    samplingRatesSwitchesLut.resize(samplingRatesNum);

    samplingRatesSwitchesNum = SamplingRatesSwitchesNum;
    samplingRatesSwitchesWord.resize(samplingRatesSwitchesNum);
    samplingRatesSwitchesByte.resize(samplingRatesSwitchesNum);

    samplingRatesSwitchesWord[SamplingRateSwitchThFilterBw3] = 2;
    samplingRatesSwitchesWord[SamplingRateSwitchThFilterBw2] = 2;
    samplingRatesSwitchesWord[SamplingRateSwitchThFilterBw1] = 2;
    samplingRatesSwitchesWord[SamplingRateSwitchThFilterBw0] = 2;

    samplingRatesSwitchesByte[SamplingRateSwitchThFilterBw3] = 0x0001;
    samplingRatesSwitchesByte[SamplingRateSwitchThFilterBw2] = 0x0002;
    samplingRatesSwitchesByte[SamplingRateSwitchThFilterBw1] = 0x0004;
    samplingRatesSwitchesByte[SamplingRateSwitchThFilterBw0] = 0x0008;

    samplingRatesSwitchesLutStrings[SamplingRate1_25kHz] = "0000";
    samplingRatesSwitchesLutStrings[SamplingRate5kHz] = "0000";
    samplingRatesSwitchesLutStrings[SamplingRate10kHz] = "0000";
    samplingRatesSwitchesLutStrings[SamplingRate20kHz] = "0000";
    samplingRatesSwitchesLutStrings[SamplingRate50kHz] = "0101";
    samplingRatesSwitchesLutStrings[SamplingRate100kHz] = "0011";
    samplingRatesSwitchesLutStrings[SamplingRate200kHz] = "0111";

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

    currentChannelsNum = 1;
    voltageChannelsNum = 1;
    totalChannelsNum = currentChannelsNum+voltageChannelsNum;

    availableVoltageSourcesIdxs.VoltageFromVoltageClamp = ChannelSourceVoltageFromVoltageClamp;
    availableVoltageSourcesIdxs.VoltageFromCurrentClamp = ChannelSourceVoltageFromCurrentClamp;

    availableCurrentSourcesIdxs.CurrentFromVoltageClamp = ChannelSourceCurrentFromVoltageClamp;
    availableCurrentSourcesIdxs.CurrentFromCurrentClamp = ChannelSourceCurrentFromCurrentClamp;

    availableVoltageSourcesIdxsArray.resize(2);
    availableVoltageSourcesIdxsArray[0] = ChannelSourceVoltageFromVoltageClamp;
    availableVoltageSourcesIdxsArray[1] = ChannelSourceVoltageFromCurrentClamp;

    availableCurrentSourcesIdxsArray.resize(2);
    availableCurrentSourcesIdxsArray[0] = ChannelSourceCurrentFromVoltageClamp;
    availableCurrentSourcesIdxsArray[1] = ChannelSourceCurrentFromCurrentClamp;

    /*! HW Switches */
    switchesStatusLength = 4;

    switchesStatus.resize(switchesStatusLength);

    switchesNames.resize(switchesStatusLength);
    switchesNames[0].resize(16);
    switchesNames[0][0] = "Ext_Cap";
    switchesNames[0][1] = "VC_Filter_OFF";
    switchesNames[0][2] = "Calib_EN";
    switchesNames[0][3] = "VC_AdderEN";
    switchesNames[0][4] = "Ring_VC";
    switchesNames[0][5] = "Vstim1Out1";
    switchesNames[0][6] = "Vstim1Out2";
    switchesNames[0][7] = "Vstim2Out1";
    switchesNames[0][8] = "Vstim2Out2";
    switchesNames[0][9] = "VC_Filter1";
    switchesNames[0][10] = "VC_Filter2";
    switchesNames[0][11] = "CC_Filter1";
    switchesNames[0][12] = "CC_Filter2";
    switchesNames[0][13] = "CC_range1";
    switchesNames[0][14] = "ResetChip";
    switchesNames[0][15] = "ResetDComp";

    switchesNames[1].resize(16);
    switchesNames[1][0] = "VC_Range1";
    switchesNames[1][1] = "VC_Range2";
    switchesNames[1][2] = "VC_Range3";
    switchesNames[1][3] = "VC_Corr_range1";
    switchesNames[1][4] = "VC_Corr_range2";
    switchesNames[1][5] = "reset_period0";
    switchesNames[1][6] = "reset_period1";
    switchesNames[1][7] = "reset_duration0";
    switchesNames[1][8] = "reset_duration1";
    switchesNames[1][9] = "reset_force0";
    switchesNames[1][10] = "reset_force1";
    switchesNames[1][11] = "VC_CfastOpEN";
    switchesNames[1][12] = "VC_PredOpEN";
    switchesNames[1][13] = "CC_IstimOpEN";
    switchesNames[1][14] = "Subtractor_EN";
    switchesNames[1][15] = "free";

    switchesNames[2].resize(16);
    switchesNames[2][0] = "TH_filter_BW3";
    switchesNames[2][1] = "TH_filter_BW2";
    switchesNames[2][2] = "TH_filter_BW1";
    switchesNames[2][3] = "TH_filter_BW0";
    switchesNames[2][4] = "free";
    switchesNames[2][5] = "free";
    switchesNames[2][6] = "free";
    switchesNames[2][7] = "free";
    switchesNames[2][8] = "free";
    switchesNames[2][9] = "free";
    switchesNames[2][10] = "free";
    switchesNames[2][11] = "free";
    switchesNames[2][12] = "free";
    switchesNames[2][13] = "free";
    switchesNames[2][14] = "free";
    switchesNames[2][15] = "free";

    switchesNames[3].resize(16);
    switchesNames[3][0] = "VC_EN1";
    switchesNames[3][1] = "VC_EN2";
    switchesNames[3][2] = "CC_EN";
    switchesNames[3][3] = "CC_StimEN";
    switchesNames[3][4] = "VC_CfastEN";
    switchesNames[3][5] = "VC_CslowEN";
    switchesNames[3][6] = "VC_Corr_EN";
    switchesNames[3][7] = "VC_PredEN";
    switchesNames[3][8] = "VC_RLeakEN";
    switchesNames[3][9] = "VC_CorrBW";
    switchesNames[3][10] = "VC_Pred_range";
    switchesNames[3][11] = "CC_CfastEN";
    switchesNames[3][12] = "CC_BridgeDIS";
    switchesNames[3][13] = "Protocol_EN";
    switchesNames[3][14] = "free";
    switchesNames[3][15] = "free";

    /*! LEDs */
    ledsNum = 1;
    ledsWord.resize(ledsNum);
    ledsByte.resize(ledsNum);
    ledsColorsArray.resize(ledsNum);

    ledsWord[0] = 1;
    ledsByte[0] = 0x8000;
    ledsColorsArray[0] = 0x00FF00;

    /*! Stimulus LPF */
    /*! VC */
    vcStimulusLpfOptionsNum = VCStimulusLpfsNum;
    vcVoltageFiltersArray.resize(vcStimulusLpfOptionsNum);
    vcStimulusLpfSwitchesLut.resize(vcStimulusLpfOptionsNum);

    vcVoltageFiltersArray[VCStimulusLpf1kHz].value = 1.0;
    vcVoltageFiltersArray[VCStimulusLpf1kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCStimulusLpf1kHz].unit = "Hz";
    vcVoltageFiltersArray[VCStimulusLpf100Hz].value = 100.0;
    vcVoltageFiltersArray[VCStimulusLpf100Hz].prefix = UnitPfxNone;
    vcVoltageFiltersArray[VCStimulusLpf100Hz].unit = "Hz";

    vcStimulusLpfSwitchesLutStrings[VCStimulusLpf1kHz] = "11";
    vcStimulusLpfSwitchesLutStrings[VCStimulusLpf100Hz] = "10";

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

//    ccStimulusLpfOptions[CCStimulusLpf10kHz] = "10 kHz";
    ccCurrentFiltersArray[CCStimulusLpf1kHz].value = 1.0;
    ccCurrentFiltersArray[CCStimulusLpf1kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCStimulusLpf1kHz].unit = "Hz";
    ccCurrentFiltersArray[CCStimulusLpf100Hz].value = 100.0;
    ccCurrentFiltersArray[CCStimulusLpf100Hz].prefix = UnitPfxNone;
    ccCurrentFiltersArray[CCStimulusLpf100Hz].unit = "Hz";

//    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf10kHz] = "01";
    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf1kHz] = "11";
    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf100Hz] = "10";

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

    /*! Constant switches */
    constantSwitchesNum = ConstantSwitchesNum;
    constantSwitchesWord.resize(constantSwitchesNum);
    constantSwitchesByte.resize(constantSwitchesNum);
    constantSwitchesLut.resize(constantSwitchesNum);

    constantSwitchesWord[ConstantSwitchExtCap] = 0;
    constantSwitchesWord[ConstantSwitchVcFilterOff] = 0;
    constantSwitchesWord[ConstantSwitchCalibEn] = 0;
    constantSwitchesWord[ConstantSwitchVcAdderEn] = 0;
    constantSwitchesWord[ConstantSwitchRingVc] = 0;
    constantSwitchesWord[ConstantSwitchStim1Out1] = 0;
    constantSwitchesWord[ConstantSwitchStim1Out2] = 0;
    constantSwitchesWord[ConstantSwitchStim2Out1] = 0;
    constantSwitchesWord[ConstantSwitchStim2Out2] = 0;
    constantSwitchesWord[ConstantSwitchResetForce0] = 1;
    constantSwitchesWord[ConstantSwitchResetForce1] = 1;
    constantSwitchesWord[ConstantSwitchCFastOpampEn] = 1;
    constantSwitchesWord[ConstantSwitchRPredOpampEn] = 1;
    constantSwitchesWord[ConstantSwitchCcStimOpampEN] = 1;
    constantSwitchesWord[ConstantSwitchSubtractorEn] = 1;
    constantSwitchesWord[ConstantSwitchVcEn1] = 3;
    constantSwitchesWord[ConstantSwitchVcCorrBw] = 3;
    constantSwitchesWord[ConstantSwitchPredRange] = 3;

    constantSwitchesByte[ConstantSwitchExtCap] = 0x0001;
    constantSwitchesByte[ConstantSwitchVcFilterOff] = 0x0002;
    constantSwitchesByte[ConstantSwitchCalibEn] = 0x0004;
    constantSwitchesByte[ConstantSwitchVcAdderEn] = 0x0008;
    constantSwitchesByte[ConstantSwitchRingVc] = 0x0010;
    constantSwitchesByte[ConstantSwitchStim1Out1] = 0x0020;
    constantSwitchesByte[ConstantSwitchStim1Out2] = 0x0040;
    constantSwitchesByte[ConstantSwitchStim2Out1] = 0x0080;
    constantSwitchesByte[ConstantSwitchStim2Out2] = 0x0100;
    constantSwitchesByte[ConstantSwitchResetForce0] = 0x0200;
    constantSwitchesByte[ConstantSwitchResetForce1] = 0x0400;
    constantSwitchesByte[ConstantSwitchCFastOpampEn] = 0x0800;
    constantSwitchesByte[ConstantSwitchRPredOpampEn] = 0x1000;
    constantSwitchesByte[ConstantSwitchCcStimOpampEN] = 0x2000;
    constantSwitchesByte[ConstantSwitchSubtractorEn] = 0x4000;
    constantSwitchesByte[ConstantSwitchVcEn1] = 0x0001;
    constantSwitchesByte[ConstantSwitchVcCorrBw] = 0x0200;
    constantSwitchesByte[ConstantSwitchPredRange] = 0x0400;

    constantSwitchesLutStrings = "110110110001111100";

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
    leakConductanceCompensationImplemented = true;
    ccPipetteCompensationImplemented = true;
    bridgeBalanceCompensationImplemented = true;

    compensationsSwitchesNum = CompensationsSwitchesNum;
    compensationsSwitchesWord.resize(compensationsSwitchesNum);
    compensationsSwitchesByte.resize(compensationsSwitchesNum);
    compensationsSwitchesLut.resize(compensationsSwitchesNum);
    compensationsSwitchesLutStrings.resize(compensationsSwitchesNum);
    compensationsSwitchesEnableSignArray.resize(compensationsSwitchesNum);

    compensationsSwitchesWord[CompensationsSwitchCFastEn] = 3;
    compensationsSwitchesWord[CompensationsSwitchCSlowEn] = 3;
    compensationsSwitchesWord[CompensationsSwitchRCorrEn] = 3;
    compensationsSwitchesWord[CompensationsSwitchRPredEn] = 3;
    compensationsSwitchesWord[CompensationsSwitchRLeakEn] = 3;
    compensationsSwitchesWord[CompensationsSwitchCCFastEn] = 3;
    compensationsSwitchesWord[CompensationsSwitchBridgeDis] = 3;

    compensationsSwitchesByte[CompensationsSwitchCFastEn] = 0x0010;
    compensationsSwitchesByte[CompensationsSwitchCSlowEn] = 0x0020;
    compensationsSwitchesByte[CompensationsSwitchRCorrEn] = 0x0040;
    compensationsSwitchesByte[CompensationsSwitchRPredEn] = 0x0080;
    compensationsSwitchesByte[CompensationsSwitchRLeakEn] = 0x0100;
    compensationsSwitchesByte[CompensationsSwitchCCFastEn] = 0x0800;
    compensationsSwitchesByte[CompensationsSwitchBridgeDis] = 0x1000;

    compensationsSwitchesLut[CompensationsSwitchCFastEn] = CompensationPipette;
    compensationsSwitchesLut[CompensationsSwitchCSlowEn] = CompensationMembrane;
    compensationsSwitchesLut[CompensationsSwitchRCorrEn] = CompensationResistanceCorrection;
    compensationsSwitchesLut[CompensationsSwitchRPredEn] = CompensationResistancePrediction;
    compensationsSwitchesLut[CompensationsSwitchRLeakEn] = CompensationLeakConductance;
    compensationsSwitchesLut[CompensationsSwitchCCFastEn] = CompensationCCPipette;
    compensationsSwitchesLut[CompensationsSwitchBridgeDis] = CompensationBridgeBalance;

    compensationsSwitchesLutStrings[CompensationsSwitchCFastEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchCSlowEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchRCorrEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchRPredEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchRLeakEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchCCFastEn] = '1';
    compensationsSwitchesLutStrings[CompensationsSwitchBridgeDis] = '0';

    for (unsigned int compensationSwitchIdx = 0; compensationSwitchIdx < CompensationsSwitchesNum; compensationSwitchIdx++) {
        if (compensationsSwitchesLutStrings[compensationSwitchIdx] == '1') {
            compensationsSwitchesEnableSignArray[compensationSwitchIdx] = true;

        } else {
            compensationsSwitchesEnableSignArray[compensationSwitchIdx] = false;
        }
    }

    /*! Compensations options */
    resistanceCorrectionOptions.resize(2);
    resistanceCorrectionOptions[0] = "BW 160Hz";
    resistanceCorrectionOptions[1] = "BW 1.6kHz";
    resistanceCorrectionOptionWord = 3;
    resistanceCorrectionOptionMask = 0x0200;
    resistanceCorrectionOptionOffset = 9;

    resistancePredictionOptions.resize(2);
    resistancePredictionOptions[0] = "BW gain x2";
    resistancePredictionOptions[1] = "BW gain x3";
    resistancePredictionOptionWord = 3;
    resistancePredictionOptionMask = 0x0400;
    resistancePredictionOptionMask = 10;

    /*! Compensations values */
    compensationsRegistersNum = CompensationsRegistersNum;
    coreSpecificRegistersNum = compensationsRegistersNum+4; /*! additional 4 are for VC and CC hold tuner */

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

    leakConductanceControlLow.implemented = true;
    leakConductanceControlLow.min = minLeakConductanceLow;
    leakConductanceControlLow.value = minLeakConductanceLow;
    leakConductanceControlLow.max = maxLeakConductanceLow;
    leakConductanceControlLow.compensable = maxLeakConductanceLow;
    leakConductanceControlLow.steps = leakConductanceLowSteps;
    leakConductanceControlLow.step = leakConductanceLowStep;
    leakConductanceControlLow.decimals = leakConductanceLowDecimals;
    leakConductanceControlLow.prefix = leakConductanceLowPrefix;
    leakConductanceControlLow.unit = leakConductanceLowUnit;
    leakConductanceControlLow.name = leakConductanceLowName;

    leakConductanceControlHigh.implemented = true;
    leakConductanceControlHigh.min = minLeakConductanceHigh;
    leakConductanceControlHigh.value = minLeakConductanceHigh;
    leakConductanceControlHigh.max = maxLeakConductanceHigh;
    leakConductanceControlHigh.compensable = maxLeakConductanceHigh;
    leakConductanceControlHigh.steps = leakConductanceHighSteps;
    leakConductanceControlHigh.step = leakConductanceHighStep;
    leakConductanceControlHigh.decimals = leakConductanceHighDecimals;
    leakConductanceControlHigh.prefix = leakConductanceHighPrefix;
    leakConductanceControlHigh.unit = leakConductanceHighUnit;
    leakConductanceControlHigh.name = leakConductanceHighName;

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

    bridgeBalanceResistanceControl.implemented = true;
    bridgeBalanceResistanceControl.min = minBridgeBalanceResistance;
    bridgeBalanceResistanceControl.value = minBridgeBalanceResistance;
    bridgeBalanceResistanceControl.max = maxBridgeBalanceResistance;
    bridgeBalanceResistanceControl.compensable = maxBridgeBalanceResistance;
    bridgeBalanceResistanceControl.steps = bridgeBalanceResistanceSteps;
    bridgeBalanceResistanceControl.step = bridgeBalanceResistanceStep;
    bridgeBalanceResistanceControl.decimals = bridgeBalanceResistanceDecimals;
    bridgeBalanceResistanceControl.prefix = bridgeBalanceResistancePrefix;
    bridgeBalanceResistanceControl.unit = bridgeBalanceResistanceUnit;
    bridgeBalanceResistanceControl.name = bridgeBalanceResistanceName;

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
    vcHoldTunerHwRegisterOffset = 12;

    /*! Current holding tuner command */
    currentHoldTunerImplemented = true;
    ccHoldTunerHwRegisterOffset = 14;

    /*! Reset commands */
    resetWord[ResetIndexChip] = 0;
    resetWord[ResetIndexDigitalOffsetCompensation] = 0;

    resetByte[ResetIndexChip] = 0x4000;
    resetByte[ResetIndexDigitalOffsetCompensation] = 0x8000;

    /*! Stimulus */
    /*! VC */
    vcStimulusSwitchesNum = VCStimulusSwitchesNum;
    vcStimulusSwitchesLut.resize(vcStimulusSwitchesNum);
    vcStimulusSwitchesWord.resize(vcStimulusSwitchesNum);
    vcStimulusSwitchesByte.resize(vcStimulusSwitchesNum);

    vcStimulusSwitchesWord[VCStimulusSwitch_VcEN2] = 3;

    vcStimulusSwitchesByte[VCStimulusSwitch_VcEN2] = 0x0002;

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

    ccStimulusSwitchesWord[CCStimulusSwitch_StimEN] = 3;

    ccStimulusSwitchesByte[CCStimulusSwitch_StimEN] = 0x0008;

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

    /*! CC */
    ccReaderSwitchesNum = CCReaderSwitchesNum;
    ccReaderSwitchesLut.resize(ccReaderSwitchesNum);
    ccReaderSwitchesWord.resize(ccReaderSwitchesNum);
    ccReaderSwitchesByte.resize(ccReaderSwitchesNum);

    ccReaderSwitchesWord[CCReaderSwitch_CcEN] = 3;

    ccReaderSwitchesByte[CCReaderSwitch_CcEN] = 0x0004;

    ccReaderSwitchesLutStrings = "1";

    for (unsigned int readerSwitchesIdx = 0; readerSwitchesIdx < ccReaderSwitchesNum; readerSwitchesIdx++) {
        if (ccReaderSwitchesLutStrings[readerSwitchesIdx] == '1') {
            ccReaderSwitchesLut[readerSwitchesIdx] = true;

        } else {
            ccReaderSwitchesLut[readerSwitchesIdx] = false;
        }
    }

    protocolMaxItemsNum = 15;

    maxDigitalTriggerOutputEvents = 21;
    digitalTriggersNum = 1;

    voltageProtocolStepImplemented = true;
    currentProtocolStepImplemented = true;
    voltageProtocolRampImplemented = true;
    currentProtocolRampImplemented = true;
    voltageProtocolSinImplemented = false;
    currentProtocolSinImplemented = false;

    multimeterStuckHazardFlag = true;

    /*! Payload */
    rxDataMessageMaxLen = 122;

    txDataMessageMaxLen = 26;

    notificationTag = deviceName;

    /*! Default values */
    currentRange = vcCurrentRangesArray[VCCurrentRange300pA];
    voltageRange = vcVoltageRangesArray[VCVoltageRange500mV];
    currentResolution = currentRange.step;
    voltageResolution = voltageRange.step;
    samplingRate = realSamplingRatesArray[SamplingRate1_25kHz];
}

EZPatchePatchEL04F::~EZPatchePatchEL04F() {

}

ErrorCodes_t EZPatchePatchEL04F::setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) {
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

ErrorCodes_t EZPatchePatchEL04F::setResistancePredictionOptions(uint16_t optionIdx) {
    if (optionIdx < resistancePredictionOptions.size()) {
        if (optionIdx == 0) {
            resistancePredictionGain[compensationsSettingChannel] = resistancePredictionGain0;

        } else {
            resistancePredictionGain[compensationsSettingChannel] = resistancePredictionGain1;
        }
        return EZPatchDevice::setResistancePredictionOptions(optionIdx);

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchePatchEL04F::setLeakConductance(Measurement_t conductance) {
    if (selectedVcCurrentRangeIdx < VCCurrentRange30nA) {
        return this->setCompensationValue(leakConductance[compensationsSettingChannel], leakConductanceControlLow, conductance);

    } else {
        return this->setCompensationValue(leakConductance[compensationsSettingChannel], leakConductanceControlHigh, conductance);
    }
}

ErrorCodes_t EZPatchePatchEL04F::getLeakConductanceControl(CompensationControl_t &control) {
    ErrorCodes_t ret = ErrorFeatureNotImplemented;
    if (selectedVcCurrentRangeIdx < VCCurrentRange30nA) {
        control = leakConductanceControlLow;

    } else {
        control = leakConductanceControlHigh;
    }
    ret = Success;
    return ret;
}

void EZPatchePatchEL04F::selectChannelsResolutions() {
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

void EZPatchePatchEL04F::selectVoltageOffsetResolution() {
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

void EZPatchePatchEL04F::initializeCompensations() {
    EZPatchDevice::initializeCompensations();

    pipetteCapacitanceRegValue.resize(currentChannelsNum);
    ccPipetteCapacitanceRegValue.resize(currentChannelsNum);
    membraneCapacitanceRegValue.resize(currentChannelsNum);
    membraneTauRegValue.resize(currentChannelsNum);
    correctionGainRegValue.resize(currentChannelsNum);
    predictionTauRegValue.resize(currentChannelsNum);
    leakConductanceRegValue.resize(currentChannelsNum);
    bridgeBalanceResistanceRegValue.resize(currentChannelsNum);

    for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        pipetteCapacitanceRegValue[channelIdx] = 0x00;
        ccPipetteCapacitanceRegValue[channelIdx] = 0x00;
        membraneCapacitanceRegValue[channelIdx] = 0x00;
        membraneTauRegValue[channelIdx] = 0x00;
        correctionGainRegValue[channelIdx] = 0x00;
        predictionTauRegValue[channelIdx] = 0x00;
        leakConductanceRegValue[channelIdx] = 0x00;
        bridgeBalanceResistanceRegValue[channelIdx] = 0x00;
    }
}

bool EZPatchePatchEL04F::checkCompensationsValues() {
    bool ret = true;
    if (voltageCompensationsFlag[compensationsSettingChannel]) {
        ret &= (pipetteCapacitance[compensationsSettingChannel] > (minPipetteCapacitance-0.5*pipetteCapacitanceStep) &&
                pipetteCapacitance[compensationsSettingChannel] < (maxPipetteCapacitance+0.5*pipetteCapacitanceStep));
        ret &= (membraneCapacitance[compensationsSettingChannel] > (minMembraneCapacitance-0.5*membraneCapacitanceStep) &&
                membraneCapacitance[compensationsSettingChannel] < (maxMembraneCapacitance+0.5*membraneCapacitanceStep));
        double membraneTau = membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel];
        ret &= (membraneTau > (minMembraneTau-0.5*membraneTauStep) &&
                membraneTau < (maxMembraneTau+0.5*membraneTauStep));
        double resistanceCorrected = accessResistance[compensationsSettingChannel]*resistanceCorrectionPercentage[compensationsSettingChannel]/maxResistanceCorrectionPercentage;
        ret &= (resistanceCorrected > (0.999*minResistanceCorrection-0.001*maxResistanceCorrection) &&
                resistanceCorrected < (0.001*minResistanceCorrection+1.001*maxResistanceCorrection));
        double resistancePredictedTau = membraneTau*resistancePredictionPercentage[compensationsSettingChannel]/maxResistancePredictionPercentage/resistancePredictionGain[compensationsSettingChannel];
        ret &= (resistancePredictedTau > (minResistancePredictionTau-0.5*resistancePredictionTauStep) &&
                resistancePredictedTau < (maxResistancePredictionTau+0.5*resistancePredictionTauStep));
        if (selectedVcCurrentRangeIdx < VCCurrentRange30nA) {
            ret &= (leakConductance[compensationsSettingChannel] > (minLeakConductanceLow-0.5*leakConductanceLowStep) &&
                    leakConductance[compensationsSettingChannel] < (maxLeakConductanceLow+0.5*leakConductanceLowStep));

        } else {
            ret &= (leakConductance[compensationsSettingChannel] > (minLeakConductanceHigh-0.5*leakConductanceHighStep) &&
                    leakConductance[compensationsSettingChannel] < (maxLeakConductanceHigh+0.5*leakConductanceHighStep));
        }
    }

    if (currentCompensationsFlag[compensationsSettingChannel]) {
        ret &= (ccPipetteCapacitance[compensationsSettingChannel] > (minCcPipetteCapacitance-0.5*ccPipetteCapacitanceStep) &&
                ccPipetteCapacitance[compensationsSettingChannel] < (maxCcPipetteCapacitance+0.5*ccPipetteCapacitanceStep));
        ret &= (bridgeBalanceResistance[compensationsSettingChannel] > (minBridgeBalanceResistance-0.5*bridgeBalanceResistanceStep) &&
                bridgeBalanceResistance[compensationsSettingChannel] < (maxBridgeBalanceResistance+0.5*bridgeBalanceResistanceStep));
    }

    if (ret) {
        if (voltageCompensationsFlag[compensationsSettingChannel]) {
            double rs = fmax(accessResistance[compensationsSettingChannel], accessResistanceControl.step);
            double rc = fmax(resistanceCorrectionPercentage[compensationsSettingChannel], resistanceCorrectionPercentageControl.step);
            double rp = fmax(resistancePredictionPercentage[compensationsSettingChannel], resistancePredictionPercentageControl.step);

            /*! pipetteCapacitanceControl.compensable doesn't depend on other controls */

            double d1 = maxMembraneCapacitance;
            double d2 = maxMembraneTau/rs;
            double d3 = maxResistancePredictionTau*maxResistancePredictionPercentage*resistancePredictionGain[compensationsSettingChannel]/(rs*rp);
            membraneCapacitanceControl.compensable = fmin(fmin(d1, d2), d3);

            d1 = accessResistanceControl.max;
            d2 = maxMembraneTau/membraneCapacitance[compensationsSettingChannel];
            d3 = maxResistanceCorrection*maxResistanceCorrectionPercentage/rc;
            double d4 = maxResistancePredictionTau*maxResistancePredictionPercentage*resistancePredictionGain[compensationsSettingChannel]/(membraneCapacitance[compensationsSettingChannel]*rp);
            accessResistanceControl.compensable = fmin(fmin(fmin(d1, d2), d3), d4);

            d1 = resistanceCorrectionPercentageControl.max;
            d2 = maxResistanceCorrection*maxResistanceCorrectionPercentage/rs;
            resistanceCorrectionPercentageControl.compensable = fmin(d1, d2);

            d1 = resistancePredictionPercentageControl.max;
            d2 = maxResistancePredictionTau*maxResistancePredictionPercentage*resistancePredictionGain[compensationsSettingChannel]/(membraneCapacitance[compensationsSettingChannel]*rs);
            resistancePredictionPercentageControl.compensable = fmin(d1, d2);
        }

        if (currentCompensationsFlag[compensationsSettingChannel]) {
            /*! ccPipetteCapacitanceControl.compensable doesn't depend on other controls */

            /*! bridgeBalanceResistanceControl.compensable doesn't depend on other controls */
        }
    }

    return ret;
}

bool EZPatchePatchEL04F::fillCompensationsRegistersTxData(std::vector <uint16_t> &txDataMessage) {
    bool anythingChanged = false;
    bool highMembraneCapacitanceFlag = (membraneCapacitance[compensationsSettingChannel] > maxMembraneCapacitance1 ? true : false);
    txDataMessage[0] = CompensationsRegisterVCCFastGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[1] = 0xFF-((voltageCompensationsFlag[compensationsSettingChannel] & pipetteCompensationFlag[compensationsSettingChannel]) ? (uint16_t)round(pipetteCapacitance[compensationsSettingChannel]/pipetteCapacitanceStep) : 0);
    txDataMessage[2] = CompensationsRegisterVCCSlowGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[3] = 0xFF-((voltageCompensationsFlag[compensationsSettingChannel] & membraneCompensationFlag[compensationsSettingChannel]) ? (uint16_t)round(highMembraneCapacitanceFlag ? (membraneCapacitance[compensationsSettingChannel]-minMembraneCapacitance2)/membraneCapacitanceStep2 : (membraneCapacitance[compensationsSettingChannel]-minMembraneCapacitance1)/membraneCapacitanceStep1) : 0)+(highMembraneCapacitanceFlag ? 0x0100 : 0);
    txDataMessage[4] = CompensationsRegisterVCCslowTau+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[5] = 0xFF-((voltageCompensationsFlag[compensationsSettingChannel] & membraneCompensationFlag[compensationsSettingChannel]) ? (uint16_t)round((membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel]-minMembraneTau)/membraneTauStep) : 0);
    txDataMessage[6] = CompensationsRegisterVCRCorrGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[7] = 0xFF-((voltageCompensationsFlag[compensationsSettingChannel] & resistanceCorrectionFlag[compensationsSettingChannel]) ? (uint16_t)round(resistanceCorrectionPercentage[compensationsSettingChannel]/maxResistanceCorrectionPercentage*accessResistance[compensationsSettingChannel]/resistanceCorrectionStep) : 0);
    txDataMessage[8] = CompensationsRegisterVCRPredTau+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[9] = 0xFF-((voltageCompensationsFlag[compensationsSettingChannel] & resistancePredictionFlag[compensationsSettingChannel]) ? (uint16_t)round((membraneCapacitance[compensationsSettingChannel]*accessResistance[compensationsSettingChannel]*resistancePredictionPercentage[compensationsSettingChannel]/maxResistancePredictionPercentage-minResistancePredictionTau)/resistancePredictionGain[compensationsSettingChannel]/resistancePredictionTauStep) : 0);
    txDataMessage[10] = CompensationsRegisterVCRLeakGain+compensationsSettingChannel*coreSpecificRegistersNum;
    if (selectedVcCurrentRangeIdx < VCCurrentRange30nA) {
        txDataMessage[11] = 0xFF-((voltageCompensationsFlag[compensationsSettingChannel] & leakConductanceCompensationFlag[compensationsSettingChannel]) ? (uint16_t)round((leakConductance[compensationsSettingChannel]-minLeakConductanceLow)/leakConductanceLowStep) : 0);

    } else {
        txDataMessage[11] = 0xFF-((voltageCompensationsFlag[compensationsSettingChannel] & leakConductanceCompensationFlag[compensationsSettingChannel]) ? (uint16_t)round((leakConductance[compensationsSettingChannel]-minLeakConductanceHigh)/leakConductanceHighStep) : 0);
    }
    txDataMessage[12] = CompensationsRegisterCCCFastGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[13] = 0xFF-((currentCompensationsFlag[compensationsSettingChannel] & ccPipetteCompensationFlag[compensationsSettingChannel]) ? (uint16_t)round(ccPipetteCapacitance[compensationsSettingChannel]/ccPipetteCapacitanceStep) : 0);
    txDataMessage[14] = CompensationsRegisterCCBridgeGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[15] = 0xFF-((currentCompensationsFlag[compensationsSettingChannel] & bridgeBalanceCompensationFlag[compensationsSettingChannel]) ? (uint16_t)round((bridgeBalanceResistance[compensationsSettingChannel]-minBridgeBalanceResistance)/bridgeBalanceResistanceStep) : 0);

    if (txDataMessage[1] != pipetteCapacitanceRegValue[compensationsSettingChannel] ||
            txDataMessage[3] != membraneCapacitanceRegValue[compensationsSettingChannel] ||
            txDataMessage[5] != membraneTauRegValue[compensationsSettingChannel] ||
            txDataMessage[7] != correctionGainRegValue[compensationsSettingChannel] ||
            txDataMessage[9] != predictionTauRegValue[compensationsSettingChannel] ||
            txDataMessage[11] != leakConductanceRegValue[compensationsSettingChannel] ||
            txDataMessage[13] != ccPipetteCapacitanceRegValue[compensationsSettingChannel] ||
            txDataMessage[15] != bridgeBalanceResistanceRegValue[compensationsSettingChannel]) {
        anythingChanged = true;
    }
    return anythingChanged;
}

void EZPatchePatchEL04F::updateWrittenCompesantionValues(std::vector <uint16_t> &txDataMessage) {
    pipetteCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[1];
    membraneCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[3];
    membraneTauRegValue[compensationsSettingChannel] = txDataMessage[5];
    correctionGainRegValue[compensationsSettingChannel] = txDataMessage[7];
    predictionTauRegValue[compensationsSettingChannel] = txDataMessage[9];
    leakConductanceRegValue[compensationsSettingChannel] = txDataMessage[11];
    ccPipetteCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[13];
    bridgeBalanceResistanceRegValue[compensationsSettingChannel] = txDataMessage[15];
}
