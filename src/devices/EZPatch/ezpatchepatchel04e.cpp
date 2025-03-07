#include "ezpatchepatchel04e.h"

EZPatchePatchEL04E::EZPatchePatchEL04E(std::string di) :
    EZPatchFtdiDevice(di) {

    deviceName = "ePatchEl04E";

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

    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange1] = 0;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange2] = 0;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRange3] = 0;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRCorrRange1] = 0;
    vcCurrentRangesSwitchesWord[VCCurrentRangesSwitchRCorrRange2] = 0;

    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange1] = 0x0800;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange2] = 0x1000;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRange3] = 0x2000;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRCorrRange1] = 0x0040;
    vcCurrentRangesSwitchesByte[VCCurrentRangesSwitchRCorrRange2] = 0x0080;

    vcCurrentRangesSwitchesLutStrings[VCCurrentRange300pA] = "00000";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange3nA] = "01011";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange30nA] = "11010";
    vcCurrentRangesSwitchesLutStrings[VCCurrentRange300nA] = "11101";

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
    ccCurrentRangesArray[CCCurrentRange2500pA].min = -2500.0;
    ccCurrentRangesArray[CCCurrentRange2500pA].step = ccCurrentRangesArray[CCCurrentRange2500pA].max/(SHORT_MAX+1.0)*1.5; /*! only 2/3 of total range used to avoid delta-sigma clipping */
    ccCurrentRangesArray[CCCurrentRange2500pA].prefix = UnitPfxPico;
    ccCurrentRangesArray[CCCurrentRange2500pA].unit = "A";
    ccCurrentRangesArray[CCCurrentRange100nA].max = 100.0;
    ccCurrentRangesArray[CCCurrentRange100nA].min = -100.0;
    ccCurrentRangesArray[CCCurrentRange100nA].step = ccCurrentRangesArray[CCCurrentRange100nA].max/(SHORT_MAX+1.0)*1.5; /*! only 2/3 of total range used to avoid delta-sigma clipping */
    ccCurrentRangesArray[CCCurrentRange100nA].prefix = UnitPfxNano;
    ccCurrentRangesArray[CCCurrentRange100nA].unit = "A";

    ccCurrentRangesSwitchesLut.resize(ccCurrentRangesNum);

    ccCurrentRangesSwitchesNum = CCCurrentRangesSwitchesNum;
    ccCurrentRangesSwitchesWord.resize(ccCurrentRangesSwitchesNum);
    ccCurrentRangesSwitchesByte.resize(ccCurrentRangesSwitchesNum);

    ccCurrentRangesSwitchesWord[CCCurrentRangesSwitchRange1] = 1;

    ccCurrentRangesSwitchesByte[CCCurrentRangesSwitchRange1] = 0x0020;

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
    vcVoltageRangesArray[VCVoltageRange500mV].step = vcVoltageRangesArray[VCVoltageRange500mV].max/(SHORT_MAX+1.0)*1.5; /*! only 2/3 of total range used to avoid delta-sigma clipping */
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

    vcVoltageCommandResolutionGain = 0.25;
    ccCurrentCommandResolutionGain = 0.25;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;

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
    sr2srm.clear();
    sr2srm[SamplingRate1_25kHz] = 0;
    sr2srm[SamplingRate5kHz] = 0;
    sr2srm[SamplingRate10kHz] = 0;
    sr2srm[SamplingRate20kHz] = 0;
    sr2srm[SamplingRate50kHz] = 1;
    sr2srm[SamplingRate100kHz] = 1;
    sr2srm[SamplingRate200kHz] = 1;

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
    switchesStatusLength = 3;

    switchesStatus.resize(switchesStatusLength);

    switchesNames.resize(switchesStatusLength);
    switchesNames[0].resize(16);
    switchesNames[0][0] = "VC_RLeakEN";
    switchesNames[0][1] = "VC_CfastEN";
    switchesNames[0][2] = "VC_CslowEN";
    switchesNames[0][3] = "VC_AdderEN";
    switchesNames[0][4] = "VC_CorrEN";
    switchesNames[0][5] = "VC_PredEN";
    switchesNames[0][6] = "VC_Corr_range1";
    switchesNames[0][7] = "VC_Corr_range2";
    switchesNames[0][8] = "free";
    switchesNames[0][9] = "VC_Corr_BW";
    switchesNames[0][10] = "VC_Pred_range";
    switchesNames[0][11] = "VC_Range1";
    switchesNames[0][12] = "VC_Range2";
    switchesNames[0][13] = "VC_Range3";
    switchesNames[0][14] = "VC_Filter_OFF";
    switchesNames[0][15] = "Ext_Cap";

    switchesNames[1].resize(16);
    switchesNames[1][0] = "free";
    switchesNames[1][1] = "in1_Vc";
    switchesNames[1][2] = "CC_StimEN";
    switchesNames[1][3] = "CC_CfastEN";
    switchesNames[1][4] = "CC_BridgeDIS";
    switchesNames[1][5] = "CC_range1";
    switchesNames[1][6] = "CC_Filter1";
    switchesNames[1][7] = "CC_Filter2";
    switchesNames[1][8] = "in1_Cc";
    switchesNames[1][9] = "RingCCVC";
    switchesNames[1][10] = "ResetChip";
    switchesNames[1][11] = "ResetDComp";
    switchesNames[1][12] = "LED_ON";
    switchesNames[1][13] = "in2_Cc";
    switchesNames[1][14] = "VC_Filter1";
    switchesNames[1][15] = "VC_Filter2";

    switchesNames[2].resize(16);
    switchesNames[2][0] = "Vstim1Out1";
    switchesNames[2][1] = "Vstim1Out2";
    switchesNames[2][2] = "Vstim2Out1";
    switchesNames[2][3] = "Vstim2Out2";
    switchesNames[2][4] = "VC_CfastOpEN";
    switchesNames[2][5] = "VC_PredOpEN";
    switchesNames[2][6] = "CC_IstimOpEN";
    switchesNames[2][7] = "free";
    switchesNames[2][8] = "free";
    switchesNames[2][9] = "free";
    switchesNames[2][10] = "free";
    switchesNames[2][11] = "free";
    switchesNames[2][12] = "free";
    switchesNames[2][13] = "free";
    switchesNames[2][14] = "free";
    switchesNames[2][15] = "free";

    /*! LEDs */
    ledsNum = 1;
    ledsWord.resize(ledsNum);
    ledsByte.resize(ledsNum);
    ledsColorsArray.resize(ledsNum);

    ledsWord[0] = 1;
    ledsByte[0] = 0x1000;
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
    vcStimulusLpfSwitchesWord[VCStimulusLpfSwitchFilter1] = 1;
    vcStimulusLpfSwitchesWord[VCStimulusLpfSwitchFilter2] = 1;
    vcStimulusLpfSwitchesByte[VCStimulusLpfSwitchFilter1] = 0x4000;
    vcStimulusLpfSwitchesByte[VCStimulusLpfSwitchFilter2] = 0x8000;

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

    ccCurrentFiltersArray[CCStimulusLpf10kHz].value = 10.0;
    ccCurrentFiltersArray[CCStimulusLpf10kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCStimulusLpf10kHz].unit = "Hz";
    //    ccStimulusLpfOptions[CCStimulusLpf1kHz] = "1 kHz";
//    ccStimulusLpfOptions[CCStimulusLpf100Hz] = "100 Hz";

    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf10kHz] = "01";
    //    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf1kHz] = "11";
//    ccStimulusLpfSwitchesLutStrings[CCStimulusLpf100Hz] = "10";

    ccStimulusLpfSwitchesNum = CCStimulusLpfSwitchesNum;
    ccStimulusLpfSwitchesWord.resize(ccStimulusLpfSwitchesNum);
    ccStimulusLpfSwitchesByte.resize(ccStimulusLpfSwitchesNum);
    ccStimulusLpfSwitchesWord[CCStimulusLpfSwitchFilter1] = 1;
    ccStimulusLpfSwitchesWord[CCStimulusLpfSwitchFilter2] = 1;
    ccStimulusLpfSwitchesByte[CCStimulusLpfSwitchFilter1] = 0x0040;
    ccStimulusLpfSwitchesByte[CCStimulusLpfSwitchFilter2] = 0x0080;

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

    constantSwitchesWord[ConstantSwitchVcAdderEn] = 0;
    constantSwitchesWord[ConstantSwitchVcFilterOff] = 0;
    constantSwitchesWord[ConstantSwitchExtCap] = 0;
    constantSwitchesWord[ConstantSwitchIn2Cc] = 1;
    constantSwitchesWord[ConstantSwitchStim1Out1] = 2;
    constantSwitchesWord[ConstantSwitchStim1Out2] = 2;
    constantSwitchesWord[ConstantSwitchStim2Out1] = 2;
    constantSwitchesWord[ConstantSwitchStim2Out2] = 2;
    constantSwitchesWord[ConstantSwitchCFastOpampEn] = 2;
    constantSwitchesWord[ConstantSwitchRPredOpampEn] = 2;
    constantSwitchesWord[ConstantSwitchCcStimOpampEN] = 2;
    constantSwitchesWord[ConstantSwitchVcCorrBw] = 0;
    constantSwitchesWord[ConstantSwitchPredRange] = 0;

    constantSwitchesByte[ConstantSwitchVcAdderEn] = 0x0008;
    constantSwitchesByte[ConstantSwitchVcFilterOff] = 0x4000;
    constantSwitchesByte[ConstantSwitchExtCap] = 0x8000;
    constantSwitchesByte[ConstantSwitchIn2Cc] = 0x2000;
    constantSwitchesByte[ConstantSwitchStim1Out1] = 0x0001;
    constantSwitchesByte[ConstantSwitchStim1Out2] = 0x0002;
    constantSwitchesByte[ConstantSwitchStim2Out1] = 0x0004;
    constantSwitchesByte[ConstantSwitchStim2Out2] = 0x0008;
    constantSwitchesByte[ConstantSwitchCFastOpampEn] = 0x0010;
    constantSwitchesByte[ConstantSwitchRPredOpampEn] = 0x0020;
    constantSwitchesByte[ConstantSwitchCcStimOpampEN] = 0x0040;
    constantSwitchesByte[ConstantSwitchVcCorrBw] = 0x0200;
    constantSwitchesByte[ConstantSwitchPredRange] = 0x0400;

    constantSwitchesLutStrings = "1100011011100";

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

    compensationsSwitchesWord[CompensationsSwitchCFastEn] = 0;
    compensationsSwitchesWord[CompensationsSwitchCSlowEn] = 0;
    compensationsSwitchesWord[CompensationsSwitchRCorrEn] = 0;
    compensationsSwitchesWord[CompensationsSwitchRPredEn] = 0;
    compensationsSwitchesWord[CompensationsSwitchRLeakEn] = 0;
    compensationsSwitchesWord[CompensationsSwitchCCFastEn] = 1;
    compensationsSwitchesWord[CompensationsSwitchBridgeDis] = 1;

    compensationsSwitchesByte[CompensationsSwitchCFastEn] = 0x0002;
    compensationsSwitchesByte[CompensationsSwitchCSlowEn] = 0x0004;
    compensationsSwitchesByte[CompensationsSwitchRCorrEn] = 0x0010;
    compensationsSwitchesByte[CompensationsSwitchRPredEn] = 0x0020;
    compensationsSwitchesByte[CompensationsSwitchRLeakEn] = 0x0001;
    compensationsSwitchesByte[CompensationsSwitchCCFastEn] = 0x0008;
    compensationsSwitchesByte[CompensationsSwitchBridgeDis] = 0x0010;

    compensationsSwitchesLut[CompensationsSwitchCFastEn] = CompCfast;
    compensationsSwitchesLut[CompensationsSwitchCSlowEn] = CompCslow;
    compensationsSwitchesLut[CompensationsSwitchRCorrEn] = CompRsCorr;
    compensationsSwitchesLut[CompensationsSwitchRPredEn] = CompRsPred;
    compensationsSwitchesLut[CompensationsSwitchRLeakEn] = CompGLeak;
    compensationsSwitchesLut[CompensationsSwitchCCFastEn] = CompCcCfast;
    compensationsSwitchesLut[CompensationsSwitchBridgeDis] = CompBridgeRes;

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
    resistanceCorrectionOptionWord = 0;
    resistanceCorrectionOptionMask = 0x0200;
    resistanceCorrectionOptionOffset = 9;

    resistancePredictionOptions.resize(2);
    resistancePredictionOptions[0] = "BW gain x2";
    resistancePredictionOptions[1] = "BW gain x3";
    resistancePredictionOptionWord = 0;
    resistancePredictionOptionMask = 0x0400;
    resistancePredictionOptionMask = 10;

    /*! Compensations values */
    compensationsRegistersNum = CompensationsRegistersNum;
    coreSpecificRegistersNum = compensationsRegistersNum+4; /*! additional 4 are for VC and CC hold tuner */

    liquidJunctionOffsetBinary = SHORT_OFFSET_BINARY;
    liquidJunctionResolution = liquidJunctionStep;
    liquidJunctionRounding = 16.0;

    liquidJunctionControl.implemented = true;
    liquidJunctionControl.min = minLiquidJunctionRange;
    liquidJunctionControl.value = 0.0;
    liquidJunctionControl.max = maxLiquidJunctionRange;
    liquidJunctionControl.maxCompensable = maxLiquidJunctionRange;
    liquidJunctionControl.steps = liquidJunctionSteps;
    liquidJunctionControl.step = liquidJunctionStep;
    liquidJunctionControl.decimals = liquidJunctionDecimals;
    liquidJunctionControl.prefix = liquidJunctionPrefix;
    liquidJunctionControl.unit = liquidJunctionUnit;
    liquidJunctionControl.name = liquidJunctionName;

    /*! Zap command */
    zapDurationHwRegisterOffset = 8;

    /*! Channel sources */
    uint16_t channelSourceHwRegisterOffset = 10;
    selectableTotalChannelsNum = ChannelSourcesNum;
    selectableCurrentChannelsNum = 1;
    selectableVoltageChannelsNum = 1;
    voltageChannelSourcesRegisters.resize(selectableVoltageChannelsNum);
    voltageChannelSourcesRegisters[ChannelSourceVoltage0] = channelSourceHwRegisterOffset;
    currentChannelSourcesRegisters.resize(selectableVoltageChannelsNum);
    currentChannelSourcesRegisters[ChannelSourceCurrent0] = channelSourceHwRegisterOffset+1;

    /*! Voltage holding tuner command */
    voltageHoldTunerImplemented = true;
    vcHoldTunerHwRegisterOffset = 12;

    /*! Current holding tuner command */
    currentHoldTunerImplemented = true;
    ccHoldTunerHwRegisterOffset = 14;

    /*! Reset commands */
    resetWord[ResetIndexChip] = 1;
    resetWord[ResetIndexLiquidJunctionCompensation] = 1;

    resetByte[ResetIndexChip] = 0x0400;
    resetByte[ResetIndexLiquidJunctionCompensation] = 0x0800;

    /*! Stimulus */
    /*! VC */
    vcStimulusSwitchesNum = VCStimulusSwitchesNum;
    vcStimulusSwitchesLut.resize(vcStimulusSwitchesNum);
    vcStimulusSwitchesWord.resize(vcStimulusSwitchesNum);
    vcStimulusSwitchesByte.resize(vcStimulusSwitchesNum);

    vcStimulusSwitchesWord[VCStimulusSwitch_in1Vc] = 1;

    vcStimulusSwitchesByte[VCStimulusSwitch_in1Vc] = 0x0002;

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

    ccStimulusSwitchesWord[CCStimulusSwitch_StimEN] = 1;

    ccStimulusSwitchesByte[CCStimulusSwitch_StimEN] = 0x0004;

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

    ccReaderSwitchesWord[CCReaderSwitch_in1Cc] = 1;

    ccReaderSwitchesByte[CCReaderSwitch_in1Cc] = 0x0100;

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

ErrorCodes_t EZPatchePatchEL04E::setResistancePredictionOptions(uint16_t optionIdx) {
    if (optionIdx < resistancePredictionOptions.size()) {
        if (optionIdx == 0) {
            compensationControls[U_RsPg][compensationsSettingChannel].value = resistancePredictionGain0;

        } else {
            compensationControls[U_RsPg][compensationsSettingChannel].value = resistancePredictionGain1;
        }
        return EZPatchDevice::setResistancePredictionOptions(optionIdx);

    } else if (optionIdx == 0) {
        return ErrorCommandNotImplemented;

    } else {
        return ErrorValueOutOfRange;
    }
}

ErrorCodes_t EZPatchePatchEL04E::getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) {
    if (param == U_LkG) {
        if (selectedVcCurrentRangeIdx < VCCurrentRange3nA) {
            compensationControls[U_LkG][compensationsSettingChannel].min = leakConductanceControlLow.min;
            compensationControls[U_LkG][compensationsSettingChannel].max = leakConductanceControlLow.max;
            compensationControls[U_LkG][compensationsSettingChannel].minCompensable = leakConductanceControlLow.minCompensable;
            compensationControls[U_LkG][compensationsSettingChannel].maxCompensable = leakConductanceControlLow.maxCompensable;
            compensationControls[U_LkG][compensationsSettingChannel].step = leakConductanceControlLow.step;
            compensationControls[U_LkG][compensationsSettingChannel].decimals = leakConductanceControlLow.decimals;

        } else {
            compensationControls[U_LkG][compensationsSettingChannel].min = leakConductanceControlHigh.min;
            compensationControls[U_LkG][compensationsSettingChannel].max = leakConductanceControlHigh.max;
            compensationControls[U_LkG][compensationsSettingChannel].minCompensable = leakConductanceControlHigh.minCompensable;
            compensationControls[U_LkG][compensationsSettingChannel].maxCompensable = leakConductanceControlHigh.maxCompensable;
            compensationControls[U_LkG][compensationsSettingChannel].step = leakConductanceControlHigh.step;
            compensationControls[U_LkG][compensationsSettingChannel].decimals = leakConductanceControlHigh.decimals;
        }
    }
    return EZPatchDevice::getCompensationControl(param, control);
}

void EZPatchePatchEL04E::selectChannelsResolutions() {
    for (unsigned int channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        if (selectedSourceForVoltageChannelIdx == ChannelSourceCurrentFromVoltageClamp) {
            currentTunerCorrection[channelIdx] = 0.0;
            rawDataFilterCurrentFlag = true;

        } else if (selectedSourceForVoltageChannelIdx == ChannelSourceCurrentFromCurrentClamp) {
            currentTunerCorrection[channelIdx] = selectedCurrentHoldVector[channelIdx].value;
            rawDataFilterCurrentFlag = false;
        }

        if (selectedSourceForVoltageChannelIdx == ChannelSourceVoltageFromVoltageClamp) {
            voltageTunerCorrection[channelIdx] = selectedVoltageHoldVector[channelIdx].value;
            rawDataFilterVoltageFlag = false;

        } else if (selectedSourceForVoltageChannelIdx == ChannelSourceVoltageFromCurrentClamp) {
            voltageTunerCorrection[channelIdx] = 0.0;
            rawDataFilterVoltageFlag = true;
        }
    }

    this->selectVoltageOffsetResolution();
    /*! voltageResolution does not depend on selected source cause we want to get it from current clamp front end */

    this->computeRawDataFilterCoefficients();
}

void EZPatchePatchEL04E::selectVoltageOffsetResolution() {
    if (selectedSourceForVoltageChannelIdx == ChannelSourceVoltageFromVoltageClamp) {
        voltageOffsetCorrection = 0.0;

    } else {
        Measurement_t correctedValue;
        correctedValue.value = voltageOffsetCorrected;
        correctedValue.prefix = liquidJunctionPrefix;
        correctedValue.convertValue(voltageRange.prefix);
        voltageOffsetCorrection = correctedValue.value;
    }
}

void EZPatchePatchEL04E::initializeCompensations() {
    EZPatchDevice::initializeCompensations();

    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    CompensationControl_t control;

    control.implemented = true;
    control.min = minPipetteCapacitance;
    control.value = minPipetteCapacitance;
    control.max = maxPipetteCapacitance;
    control.maxCompensable = maxPipetteCapacitance;
    control.steps = pipetteCapacitanceSteps;
    control.step = pipetteCapacitanceStep;
    control.decimals = pipetteCapacitanceDecimals;
    control.prefix = pipetteCapacitancePrefix;
    control.unit = pipetteCapacitanceUnit;
    control.name = pipetteCapacitanceName;
    std::fill(compensationControls[U_CpVc].begin(), compensationControls[U_CpVc].end(), control);

    control.implemented = true;
    control.min = minMembraneCapacitance;
    control.value = minMembraneCapacitance;
    control.max = maxMembraneCapacitance;
    control.maxCompensable = maxMembraneCapacitance;
    control.steps = membraneCapacitanceSteps;
    control.step = membraneCapacitanceStep;
    control.decimals = membraneCapacitanceDecimals;
    control.prefix = membraneCapacitancePrefix;
    control.unit = membraneCapacitanceUnit;
    control.name = membraneCapacitanceName;
    std::fill(compensationControls[U_Cm].begin(), compensationControls[U_Cm].end(), control);

    control.implemented = true;
    control.min = 0.0;
    control.value = 0.0;
    control.max = 100.0;
    control.maxCompensable = 100.0;
    control.steps = 1001.0;
    control.step = 0.1;
    control.decimals = 1;
    control.prefix = accessResistancePrefix;
    control.unit = accessResistanceUnit;
    control.name = accessResistanceName;
    std::fill(compensationControls[U_Rs].begin(), compensationControls[U_Rs].end(), control);

    control.implemented = true;
    control.min = 0.0;
    control.value = 0.0;
    control.max = 100.0;
    control.maxCompensable = 100.0;
    control.steps = 101.0;
    control.step = 1.0;
    control.decimals = 1;
    control.prefix = resistanceCorrectionPercentagePrefix;
    control.unit = resistanceCorrectionPercentageUnit;
    control.name = resistanceCorrectionPercentageName;
    std::fill(compensationControls[U_RsCp].begin(), compensationControls[U_RsCp].end(), control);

    control.implemented = true;
    control.min = 0.0;
    control.value = 100.0;
    control.max = 150.0;
    control.maxCompensable = 150.0;
    control.steps = 151.0;
    control.step = 1.0;
    control.decimals = 1;
    control.prefix = resistancePredictionPercentagePrefix;
    control.unit = resistancePredictionPercentageUnit;
    control.name = resistancePredictionPercentageName;
    std::fill(compensationControls[U_RsPp].begin(), compensationControls[U_RsPp].end(), control);

    control.implemented = false;
    control.min = minResistancePredictionTau;
    control.value = minResistancePredictionTau;
    control.max = maxResistancePredictionTau;
    control.maxCompensable = maxResistancePredictionTau;
    control.steps = resistancePredictionTauSteps;
    control.step = resistancePredictionTauStep;
    control.decimals = resistancePredictionTauDecimals;
    control.prefix = resistancePredictionTauPrefix;
    control.unit = resistancePredictionTauUnit;
    control.name = resistancePredictionTauName;
    std::fill(compensationControls[U_RsPt].begin(), compensationControls[U_RsPt].end(), control);

    leakConductanceControlLow.implemented = true;
    leakConductanceControlLow.min = minLeakConductanceHigh;
    leakConductanceControlLow.value = minLeakConductanceHigh;
    leakConductanceControlLow.max = maxLeakConductanceHigh;
    leakConductanceControlLow.maxCompensable = maxLeakConductanceHigh;
    leakConductanceControlLow.steps = leakConductanceHighSteps;
    leakConductanceControlLow.step = leakConductanceHighStep;
    leakConductanceControlLow.decimals = leakConductanceHighDecimals;
    leakConductanceControlLow.prefix = leakConductanceHighPrefix;
    leakConductanceControlLow.unit = leakConductanceHighUnit;
    leakConductanceControlLow.name = leakConductanceHighName;
    std::fill(compensationControls[U_LkG].begin(), compensationControls[U_LkG].end(), leakConductanceControlLow);

    leakConductanceControlHigh.implemented = true;
    leakConductanceControlHigh.min = minLeakConductanceHigh;
    leakConductanceControlHigh.value = minLeakConductanceHigh;
    leakConductanceControlHigh.max = maxLeakConductanceHigh;
    leakConductanceControlHigh.maxCompensable = maxLeakConductanceHigh;
    leakConductanceControlHigh.steps = leakConductanceHighSteps;
    leakConductanceControlHigh.step = leakConductanceHighStep;
    leakConductanceControlHigh.decimals = leakConductanceHighDecimals;
    leakConductanceControlHigh.prefix = leakConductanceHighPrefix;
    leakConductanceControlHigh.unit = leakConductanceHighUnit;
    leakConductanceControlHigh.name = leakConductanceHighName;

    control.implemented = true;
    control.min = minCcPipetteCapacitance;
    control.value = minCcPipetteCapacitance;
    control.max = maxCcPipetteCapacitance;
    control.maxCompensable = maxCcPipetteCapacitance;
    control.steps = ccPipetteCapacitanceSteps;
    control.step = ccPipetteCapacitanceStep;
    control.decimals = ccPipetteCapacitanceDecimals;
    control.prefix = ccPipetteCapacitancePrefix;
    control.unit = ccPipetteCapacitanceUnit;
    control.name = ccPipetteCapacitanceName;
    std::fill(compensationControls[U_CpCc].begin(), compensationControls[U_CpCc].end(), control);

    control.implemented = true;
    control.min = minBridgeBalanceResistance;
    control.value = minBridgeBalanceResistance;
    control.max = maxBridgeBalanceResistance;
    control.maxCompensable = maxBridgeBalanceResistance;
    control.steps = bridgeBalanceResistanceSteps;
    control.step = bridgeBalanceResistanceStep;
    control.decimals = bridgeBalanceResistanceDecimals;
    control.prefix = bridgeBalanceResistancePrefix;
    control.unit = bridgeBalanceResistanceUnit;
    control.name = bridgeBalanceResistanceName;
    std::fill(compensationControls[U_BrB].begin(), compensationControls[U_BrB].end(), control);

    compValueMatrix.resize(currentChannelsNum, std::vector <double> (CompensationUserParamsNum));
    int compIdx = 0;
    int channelIdx = 0;
    for (auto controls : compensationControls) {
        if (controls[0].implemented) {
            for (auto control : controls) {
                compValueMatrix[channelIdx++][compIdx] = control.value;
            }
            channelIdx = 0;
            compIdx++;
        }
    }

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

bool EZPatchePatchEL04E::checkCompensationsValues() {
    bool ret = true;
    if (vcCompensationsActivated) {
        ret &= (compensationControls[U_CpVc][compensationsSettingChannel].value > (minPipetteCapacitance-0.5*pipetteCapacitanceStep) &&
                compensationControls[U_CpVc][compensationsSettingChannel].value < (maxPipetteCapacitance+0.5*pipetteCapacitanceStep));
        ret &= (compensationControls[U_Cm][compensationsSettingChannel].value > (minMembraneCapacitance-0.5*membraneCapacitanceStep) &&
                compensationControls[U_Cm][compensationsSettingChannel].value < (maxMembraneCapacitance+0.5*membraneCapacitanceStep));
        double membraneTau = compensationControls[U_Cm][compensationsSettingChannel].value*compensationControls[U_Rs][compensationsSettingChannel].value;
        ret &= (membraneTau > (minMembraneTau-0.5*membraneTauStep) &&
                membraneTau < (maxMembraneTau+0.5*membraneTauStep));
        double resistanceCorrected = compensationControls[U_Rs][compensationsSettingChannel].value*compensationControls[U_RsCp][compensationsSettingChannel].value/maxResistanceCorrectionPercentage;
        ret &= (resistanceCorrected > (0.999*minResistanceCorrection-0.001*maxResistanceCorrection) &&
                resistanceCorrected < (0.001*minResistanceCorrection+1.001*maxResistanceCorrection));
        double resistancePredictedTau = membraneTau*compensationControls[U_RsPp][compensationsSettingChannel].value/maxResistancePredictionPercentage/compensationControls[U_RsPg][compensationsSettingChannel].value;
        ret &= (resistancePredictedTau > (minResistancePredictionTau-0.5*resistancePredictionTauStep) &&
                resistancePredictedTau < (maxResistancePredictionTau+0.5*resistancePredictionTauStep));
        if (selectedVcCurrentRangeIdx < VCCurrentRange3nA) {
            ret &= (compensationControls[U_LkG][compensationsSettingChannel].value > (minLeakConductanceLow-0.5*leakConductanceLowStep) &&
                    compensationControls[U_LkG][compensationsSettingChannel].value < (maxLeakConductanceLow+0.5*leakConductanceLowStep));

        } else {
            ret &= (compensationControls[U_LkG][compensationsSettingChannel].value > (minLeakConductanceHigh-0.5*leakConductanceHighStep) &&
                    compensationControls[U_LkG][compensationsSettingChannel].value < (maxLeakConductanceHigh+0.5*leakConductanceHighStep));
        }
    }

    if (ccCompensationsActivated) {
        ret &= (compensationControls[U_CpCc][compensationsSettingChannel].value > (minCcPipetteCapacitance-0.5*ccPipetteCapacitanceStep) &&
                compensationControls[U_CpCc][compensationsSettingChannel].value < (maxCcPipetteCapacitance+0.5*ccPipetteCapacitanceStep));
        ret &= (compensationControls[U_BrB][compensationsSettingChannel].value > (minBridgeBalanceResistance-0.5*bridgeBalanceResistanceStep) &&
                compensationControls[U_BrB][compensationsSettingChannel].value < (maxBridgeBalanceResistance+0.5*bridgeBalanceResistanceStep));
    }

    if (ret) {
        if (vcCompensationsActivated) {
            double rs = fmax(compensationControls[U_Rs][compensationsSettingChannel].value, compensationControls[U_Rs][compensationsSettingChannel].step);
            double rc = fmax(compensationControls[U_RsCp][compensationsSettingChannel].value, compensationControls[U_RsCp][compensationsSettingChannel].step);
            double rp = fmax(compensationControls[U_RsPp][compensationsSettingChannel].value, compensationControls[U_RsPp][compensationsSettingChannel].step);

            /*! pipetteCapacitanceControl.maxCompensable doesn't depend on other controls */

            double d1 = maxMembraneCapacitance;
            double d2 = maxMembraneTau/rs;
            double d3 = maxResistancePredictionTau*maxResistancePredictionPercentage*compensationControls[U_RsPg][compensationsSettingChannel].value/(rs*rp);
            compensationControls[U_Cm][compensationsSettingChannel].maxCompensable = fmin(fmin(d1, d2), d3);

            d1 = compensationControls[U_Rs][compensationsSettingChannel].max;
            d2 = maxMembraneTau/compensationControls[U_Cm][compensationsSettingChannel].value;
            d3 = maxResistanceCorrection*maxResistanceCorrectionPercentage/rc;
            double d4 = maxResistancePredictionTau*maxResistancePredictionPercentage*compensationControls[U_RsPg][compensationsSettingChannel].value/(compensationControls[U_Cm][compensationsSettingChannel].value*rp);
            compensationControls[U_Rs][compensationsSettingChannel].maxCompensable = fmin(fmin(fmin(d1, d2), d3), d4);

            d1 = compensationControls[U_RsCp][compensationsSettingChannel].max;
            d2 = maxResistanceCorrection*maxResistanceCorrectionPercentage/rs;
            compensationControls[U_RsCp][compensationsSettingChannel].maxCompensable = fmin(d1, d2);

            d1 = compensationControls[U_RsPp][compensationsSettingChannel].max;
            d2 = maxResistancePredictionTau*maxResistancePredictionPercentage*compensationControls[U_RsPg][compensationsSettingChannel].value/(compensationControls[U_Cm][compensationsSettingChannel].value*rs);
            compensationControls[U_RsPp][compensationsSettingChannel].maxCompensable = fmin(d1, d2);
        }

        if (ccCompensationsActivated) {
            /*! ccPipetteCapacitanceControl.maxCompensable doesn't depend on other controls */

            /*! bridgeBalanceResistanceControl.maxCompensable doesn't depend on other controls */
        }
    }

    return ret;
}

bool EZPatchePatchEL04E::fillCompensationsRegistersTxData(std::vector <uint16_t> &txDataMessage) {
    bool anythingChanged = false;
    bool highMembraneCapacitanceFlag = (compensationControls[U_Cm][compensationsSettingChannel].value > maxMembraneCapacitance1 ? true : false);
    txDataMessage[0] = CompensationsRegisterVCCFastGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[1] = 0xFF-((vcCompensationsActivated & compensationsEnableFlags[CompCfast][compensationsSettingChannel]) ? (uint16_t)round(compensationControls[U_CpVc][compensationsSettingChannel].value/pipetteCapacitanceStep) : 0);
    txDataMessage[2] = CompensationsRegisterVCCSlowGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[3] = 0xFF-((vcCompensationsActivated & compensationsEnableFlags[CompCslow][compensationsSettingChannel]) ? (uint16_t)round(highMembraneCapacitanceFlag ? (compensationControls[U_Cm][compensationsSettingChannel].value-minMembraneCapacitance2)/membraneCapacitanceStep2 : (compensationControls[U_Cm][compensationsSettingChannel].value-minMembraneCapacitance1)/membraneCapacitanceStep1) : 0)+(highMembraneCapacitanceFlag ? 0x0100 : 0);
    txDataMessage[4] = CompensationsRegisterVCCslowTau+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[5] = 0xFF-((vcCompensationsActivated & compensationsEnableFlags[CompCslow][compensationsSettingChannel]) ? (uint16_t)round((compensationControls[U_Cm][compensationsSettingChannel].value*compensationControls[U_Rs][compensationsSettingChannel].value-minMembraneTau)/membraneTauStep) : 0);
    txDataMessage[6] = CompensationsRegisterVCRCorrGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[7] = 0xFF-((vcCompensationsActivated & compensationsEnableFlags[CompRsCorr][compensationsSettingChannel]) ? (uint16_t)round(compensationControls[U_RsCp][compensationsSettingChannel].value/maxResistanceCorrectionPercentage*compensationControls[U_Rs][compensationsSettingChannel].value/resistanceCorrectionStep) : 0);
    txDataMessage[8] = CompensationsRegisterVCRPredTau+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[9] = 0xFF-((vcCompensationsActivated & compensationsEnableFlags[CompRsPred][compensationsSettingChannel]) ? (uint16_t)round((compensationControls[U_Cm][compensationsSettingChannel].value*compensationControls[U_Rs][compensationsSettingChannel].value*compensationControls[U_RsPp][compensationsSettingChannel].value/maxResistancePredictionPercentage-minResistancePredictionTau)/compensationControls[U_RsPg][compensationsSettingChannel].value/resistancePredictionTauStep) : 0);
    txDataMessage[10] = CompensationsRegisterVCRLeakGain+compensationsSettingChannel*coreSpecificRegistersNum;
    if (selectedVcCurrentRangeIdx < VCCurrentRange3nA) {
        txDataMessage[11] = 0xFF-((vcCompensationsActivated & compensationsEnableFlags[CompGLeak][compensationsSettingChannel]) ? (uint16_t)round((compensationControls[U_LkG][compensationsSettingChannel].value-minLeakConductanceLow)/leakConductanceLowStep) : 0);

    } else {
        txDataMessage[11] = 0xFF-((vcCompensationsActivated & compensationsEnableFlags[CompGLeak][compensationsSettingChannel]) ? (uint16_t)round((compensationControls[U_LkG][compensationsSettingChannel].value-minLeakConductanceHigh)/leakConductanceHighStep) : 0);
    }
    txDataMessage[12] = CompensationsRegisterCCCFastGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[13] = 0xFF-((ccCompensationsActivated & compensationsEnableFlags[CompCcCfast][compensationsSettingChannel]) ? (uint16_t)round(compensationControls[U_CpCc][compensationsSettingChannel].value/ccPipetteCapacitanceStep) : 0);
    txDataMessage[14] = CompensationsRegisterCCBridgeGain+compensationsSettingChannel*coreSpecificRegistersNum;
    txDataMessage[15] = 0xFF-((ccCompensationsActivated & compensationsEnableFlags[CompBridgeRes][compensationsSettingChannel]) ? (uint16_t)round((compensationControls[U_BrB][compensationsSettingChannel].value-minBridgeBalanceResistance)/bridgeBalanceResistanceStep) : 0);

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

void EZPatchePatchEL04E::updateWrittenCompensationValues(std::vector <uint16_t> &txDataMessage) {
    pipetteCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[1];
    membraneCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[3];
    membraneTauRegValue[compensationsSettingChannel] = txDataMessage[5];
    correctionGainRegValue[compensationsSettingChannel] = txDataMessage[7];
    predictionTauRegValue[compensationsSettingChannel] = txDataMessage[9];
    leakConductanceRegValue[compensationsSettingChannel] = txDataMessage[11];
    ccPipetteCapacitanceRegValue[compensationsSettingChannel] = txDataMessage[13];
    bridgeBalanceResistanceRegValue[compensationsSettingChannel] = txDataMessage[15];
}
