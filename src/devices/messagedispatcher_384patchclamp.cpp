#include "messagedispatcher_384patchclamp.h"

/*! \todo FCON tutto da rifare, è una copia del nanopore */

MessageDispatcher_384PatchClamp_V01::MessageDispatcher_384PatchClamp_V01(string di) :
    MessageDispatcher_OpalKelly(di) {

    deviceName = "384PatchClamp";

    fwName = "384PatchClamp_V01_top.bit";

    waitingTimeBeforeReadingData = 10; //s

    rxSyncWord = 0x5aa5;

    packetsPerFrame = 1;

    voltageChannelsNum = 384;
    currentChannelsNum = 384;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 24;

    rxWordOffsets[RxMessageDataLoad] = 0;
    rxWordLengths[RxMessageDataLoad] = (voltageChannelsNum+currentChannelsNum)*packetsPerFrame;

    rxWordOffsets[RxMessageDataHeader] = rxWordOffsets[RxMessageDataLoad] + rxWordLengths[RxMessageDataLoad];
    rxWordLengths[RxMessageDataHeader] = 4;

    rxWordOffsets[RxMessageDataTail] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageDataTail] = 1;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataTail] + rxWordLengths[RxMessageDataTail];
    rxWordLengths[RxMessageStatus] = 1;

    rxWordOffsets[RxMessageVoltageOffset] = rxWordOffsets[RxMessageStatus] + rxWordLengths[RxMessageStatus];
    rxWordLengths[RxMessageVoltageOffset] = currentChannelsNum;

    maxOutputPacketsNum = E384CL_DATA_ARRAY_SIZE/totalChannelsNum;

    rxMaxWords = totalChannelsNum; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE*packetsPerFrame;

    txDataWords = 1611+9; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
    txDataWords = (txDataWords/2+1)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txModifiedStartingWord = txDataWords;
    txModifiedEndingWord = 0;
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    /*! Clamping modalities */
    /*! \todo FCON rechecks*/
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = E384CL_VOLTAGE_CLAMP_MODE;
    clampingModalitiesArray[CurrentClamp] = E384CL_CURRENT_CLAMP_MODE;
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
    defaultVcCurrentRangeIdx = VCCurrentRange10nA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].max = 512.0;
    vcVoltageRangesArray[VCVoltageRange500mV].min = -512.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.125;
    vcVoltageRangesArray[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange500mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange500mV;

    /*! Current ranges */
    /*! CC */
    ccCurrentRangesNum = CCCurrentRangesNum;
    ccCurrentRangesArray.resize(ccCurrentRangesNum);
    ccCurrentRangesArray[CCCurrentRange8nA].max = 8000.0;
    ccCurrentRangesArray[CCCurrentRange8nA].min = 8000.0;
    ccCurrentRangesArray[CCCurrentRange8nA].step = ccCurrentRangesArray[CCCurrentRange8nA].max/(INT13_MAX+1.0);
    ccCurrentRangesArray[CCCurrentRange8nA].prefix = UnitPfxPico;
    ccCurrentRangesArray[CCCurrentRange8nA].unit = "A";

    /*! Voltage ranges */
    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;
    ccVoltageRangesArray.resize(ccVoltageRangesNum);
    ccVoltageRangesArray[CCVoltageRange1000mV].max = 1000.0;
    ccVoltageRangesArray[CCVoltageRange1000mV].min = -1000.0;
    ccVoltageRangesArray[CCVoltageRange1000mV].step = ccVoltageRangesArray[CCVoltageRange1000mV].max/(SHORT_MAX+1.0);
    ccVoltageRangesArray[CCVoltageRange1000mV].prefix = UnitPfxMilli;
    ccVoltageRangesArray[CCVoltageRange1000mV].unit = "V";

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;
    vcCurrentFiltersArray.resize(vcCurrentFiltersNum);
    vcCurrentFiltersArray[VCCurrentFilter3kHz].value = 3.0;
    vcCurrentFiltersArray[VCCurrentFilter3kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter3kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter3_6kHz].value = 3.6;
    vcCurrentFiltersArray[VCCurrentFilter3_6kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter3_6kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter10kHz].value = 10.0;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter12kHz].value = 12.0;
    vcCurrentFiltersArray[VCCurrentFilter12kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter12kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter25kHz].value = 25.0;
    vcCurrentFiltersArray[VCCurrentFilter25kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter25kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter30kHz].value = 30.0;
    vcCurrentFiltersArray[VCCurrentFilter30kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter30kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter100kHz].value = 100.0;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter120kHz].value = 120.0;
    vcCurrentFiltersArray[VCCurrentFilter120kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter120kHz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter3kHz;

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter1kHz].value = 1.0;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter10kHz].value = 10.0;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter35kHz].value = 35.0;
    vcVoltageFiltersArray[VCVoltageFilter35kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter35kHz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter1kHz;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;
    ccCurrentFiltersArray.resize(ccCurrentFiltersNum);
    ccCurrentFiltersArray[CCCurrentFilter1kHz].value = 1.0;
    ccCurrentFiltersArray[CCCurrentFilter1kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter1kHz].unit = "Hz";
    ccCurrentFiltersArray[CCCurrentFilter10kHz].value = 10.0;
    ccCurrentFiltersArray[CCCurrentFilter10kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter10kHz].unit = "Hz";
    ccCurrentFiltersArray[CCCurrentFilter35kHz].value = 35.0;
    ccCurrentFiltersArray[CCCurrentFilter35kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter35kHz].unit = "Hz";
    defaultCcCurrentFilterIdx = CCCurrentFilter1kHz;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;
    ccVoltageFiltersArray.resize(ccVoltageFiltersNum);
    ccVoltageFiltersArray[CCVoltageFilter3kHz].value = 3.0;
    ccVoltageFiltersArray[CCVoltageFilter3kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter3kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter3_6kHz].value = 3.6;
    ccVoltageFiltersArray[CCVoltageFilter3_6kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter3_6kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter10kHz].value = 10.0;
    ccVoltageFiltersArray[CCVoltageFilter10kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter10kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter12kHz].value = 12.0;
    ccVoltageFiltersArray[CCVoltageFilter12kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter12kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter25kHz].value = 25.0;
    ccVoltageFiltersArray[CCVoltageFilter25kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter25kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter30kHz].value = 30.0;
    ccVoltageFiltersArray[CCVoltageFilter30kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter30kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter100kHz].value = 100.0;
    ccVoltageFiltersArray[CCVoltageFilter100kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter100kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter120kHz].value = 120.0;
    ccVoltageFiltersArray[CCVoltageFilter120kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter120kHz].unit = "Hz";
    defaultCcVoltageFilterIdx = CCVoltageFilter3kHz;

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
    defaultSamplingRateIdx = SamplingRate6_25kHz;


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


    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate6_25kHz].value = 0.00016;
    integrationStepArray[SamplingRate6_25kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate6_25kHz].unit = "s";
    integrationStepArray[SamplingRate12_5kHz].value = 0.00008;
    integrationStepArray[SamplingRate12_5kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate12_5kHz].unit = "s";
    integrationStepArray[SamplingRate25kHz].value = 0.00004;
    integrationStepArray[SamplingRate25kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate25kHz].unit = "s";
    integrationStepArray[SamplingRate50kHz].value = 0.00002;
    integrationStepArray[SamplingRate50kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate50kHz].unit = "s";
    integrationStepArray[SamplingRate100kHz].value = 0.00001;
    integrationStepArray[SamplingRate100kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate100kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
      {SamplingRate6_25kHz, VCCurrentFilter3_6kHz},
      {SamplingRate12_5kHz, VCCurrentFilter10kHz},
      {SamplingRate25kHz, VCCurrentFilter25kHz},
      {SamplingRate50kHz, VCCurrentFilter30kHz},
      {SamplingRate100kHz, VCCurrentFilter100kHz}
    };

    // mapping ADC Current Clamp
    // undefined

    vHoldRange.resize(VCVoltageRangesNum);
    vHoldRange[VCVoltageRange500mV].min = -500.0;
    vHoldRange[VCVoltageRange500mV].max = 500.0;
    vHoldRange[VCVoltageRange500mV].step = 0.125;
    vHoldRange[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vHoldRange[VCVoltageRange500mV].unit = "V";
    selectedVoltageHoldVector.resize(currentChannelsNum);
    Measurement_t defaultVoltageHoldTuner = {0.0, vHoldRange[VCVoltageRange500mV].prefix, vHoldRange[VCVoltageRange500mV].unit};

    cHoldRange.resize(CCCurrentRangesNum);
    cHoldRange[CCCurrentRange8nA].min = -8.0;
    cHoldRange[CCCurrentRange8nA].max = 8.0 - 16.0/8192.0;
    cHoldRange[CCCurrentRange8nA].step = 16.0/8192.0;
    cHoldRange[CCCurrentRange8nA].prefix = UnitPfxNano;
    cHoldRange[CCCurrentRange8nA].unit = "A";
    selectedCurrentHoldVector.resize(currentChannelsNum);
    Measurement_t defaultCurrentHoldTuner = {0.0, cHoldRange[CCCurrentRange8nA].prefix, cHoldRange[CCCurrentRange8nA].unit};

    /*! VC current calib gain */
    calibVcCurrentGainRange.step = 1.0/1024.0;
    calibVcCurrentGainRange.min = 0;//SHORT_MIN * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.max = SHORT_MAX * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.prefix = UnitPfxNone;
    calibVcCurrentGainRange.unit = "";
    selectedCalibVcCurrentGainVector.resize(currentChannelsNum);
    defaultCalibVcCurrentGain = {1, calibVcCurrentGainRange.prefix, calibVcCurrentGainRange.unit};

    /*! VC current calib offset */
    calibVcCurrentOffsetRanges = vcCurrentRangesArray;
    selectedCalibVcCurrentOffsetVector.resize(currentChannelsNum);
    defaultCalibVcCurrentOffset = {0.0, calibVcCurrentOffsetRanges[defaultVcCurrentRangeIdx].prefix, calibVcCurrentOffsetRanges[defaultVcCurrentRangeIdx].unit};

    /*! VC calib DAC offset */
    defaultCalibVcDacOffset = {0.0, vcVoltageRangesArray[defaultVcVoltageRangeIdx].prefix, vcVoltageRangesArray[defaultVcVoltageRangeIdx].unit};


    /*! Compensations */
    /*! compValueMatrix contains one vector of compensation values for each of the channels (e.g. 384 channels) */
    compValueMatrix.resize(currentChannelsNum, std::vector<double>(CompensationAsicParamsNum));

    /*! Compensation type enables, one element per channel*/
    compCfastEnable.resize(currentChannelsNum);
    compCslowEnable.resize(currentChannelsNum);
    compRsCorrEnable.resize(currentChannelsNum);
    compRsPredEnable.resize(currentChannelsNum);
    fill(compCfastEnable.begin(), compCfastEnable.end(), false);
    fill(compCslowEnable.begin(), compCslowEnable.end(), false);
    fill(compRsCorrEnable.begin(), compRsCorrEnable.end(), false);
    fill(compRsPredEnable.begin(), compRsPredEnable.end(), false);

    /*! FEATURES ASIC DOMAIN Pipette capacitance */
    const double pipetteVarResistance = 100.0e-3;
    const double pipetteFixedResistance = 80.0e-3;
    const int pipetteCapacitanceRanges = 4;
    const double pipetteCapacitanceValuesNum = 64.0;

    vector <double> pipetteInjCapacitance = {2.5, 5.0, 10.0, 20.0};
    /*! \todo FCON recheck, now trying to use ranged measurement fo Features  */
    pipetteCapacitanceRange_pF.resize(pipetteCapacitanceRanges);
    for (int idx = 0; idx < pipetteCapacitanceRanges; idx++) {
        pipetteCapacitanceRange_pF[idx].step = pipetteVarResistance/pipetteCapacitanceValuesNum/pipetteFixedResistance*pipetteInjCapacitance[idx];
        pipetteCapacitanceRange_pF[idx].min = pipetteVarResistance/pipetteFixedResistance*pipetteInjCapacitance[idx]+pipetteCapacitanceRange_pF[idx].step;
        pipetteCapacitanceRange_pF[idx].max = pipetteCapacitanceRange_pF[idx].min+(pipetteCapacitanceValuesNum-1.0)*pipetteCapacitanceRange_pF[idx].step;
        pipetteCapacitanceRange_pF[idx].prefix = UnitPfxPico;
        pipetteCapacitanceRange_pF[idx].unit = "F";
    }

    /*! FEATURES ASIC DOMAIN Membrane capacitance*/
    const double membraneCapValueResistanceRatio = 2.0;
    const int membraneCapValueRanges = 4;
    const double membraneCapValueValuesNum = 64.0; // 6 bits

    vector <double> membraneCapValueInjCapacitance = {5.0, 15.0, 45.0, 135.0};
    /*! \todo FCON recheck, now trying to use ranged measurement fo Features  */
    membraneCapValueRange_pF.resize(membraneCapValueRanges);
    for (int idx = 0; idx < membraneCapValueRanges; idx++) {
        membraneCapValueRange_pF[idx].step = membraneCapValueResistanceRatio/membraneCapValueValuesNum * membraneCapValueInjCapacitance[idx];
        membraneCapValueRange_pF[idx].min = (1.0 + membraneCapValueResistanceRatio/membraneCapValueValuesNum) * membraneCapValueInjCapacitance[idx];
        membraneCapValueRange_pF[idx].max = membraneCapValueRange_pF[idx].min + (membraneCapValueValuesNum - 1.0) * membraneCapValueRange_pF[idx].step;
        membraneCapValueRange_pF[idx].prefix = UnitPfxPico;
        membraneCapValueRange_pF[idx].unit = "F";
    }

    /*! FEATURES ASIC DOMAIN Membrane capacitance TAU*/
    const int membraneCapTauValueRanges = 2;
    const double membraneCapTauValueVarResistance_MOhm = 51.2 / this->clockRatio; /*! affected by switch cap clock!!!!!*/
    const double membraneCapTauValueValuesNum = 256.0; // 8 bits

    vector <double> membraneCapTauValueCapacitance = {2.5, 25.0};
    /*! \todo FCON recheck, now trying to use ranged measurement fo Features  */
    membraneCapTauValueRange_us.resize(membraneCapTauValueRanges);
    for (int idx = 0; idx < membraneCapTauValueRanges; idx++) {
        membraneCapTauValueRange_us[idx].step = membraneCapTauValueVarResistance_MOhm * membraneCapTauValueCapacitance[idx] / membraneCapTauValueValuesNum;
        membraneCapTauValueRange_us[idx].min = membraneCapTauValueRange_us[idx].step;
        membraneCapTauValueRange_us[idx].max = membraneCapTauValueRange_us[idx].min + (membraneCapTauValueValuesNum - 1.0) * membraneCapTauValueRange_us[idx].step;
        membraneCapTauValueRange_us[idx].prefix = UnitPfxMicro;
        membraneCapTauValueRange_us[idx].unit = "s";
    }

    /*! FEATURES ASIC DOMAIN Rs correction*/
    rsCorrValueRange.step = 0.4; // MOhm
    rsCorrValueRange.min = 0.4; // MOhm
    rsCorrValueRange.max = 25.6; // MOhm
    rsCorrValueRange.prefix = UnitPfxMega;
    rsCorrValueRange.unit = "Ohm";

    /*! FEATURES ASIC DOMAIN Rs prediction GAIN*/
    const double rsPredGainValuesNum = 64.0;
    rsPredGainRange.step = 1/16.0; // MOhm
    rsPredGainRange.min = 1 + rsPredGainRange.step; // MOhm
    rsPredGainRange.max = rsPredGainRange.min + rsPredGainRange.step * (rsPredGainValuesNum -1) ; // MOhm
    rsPredGainRange.prefix = UnitPfxMega;
    rsPredGainRange.unit = "Ohm";

    /*! FEATURES ASIC DOMAIN Rs prediction TAU*/
    const double rsPredTauValuesNum = 256.0;
    rsPredTauRange.step = 2.0 / this->clockRatio; /*! affected by switch cap clock!!!!!*/
    rsPredTauRange.min = rsPredTauRange.step;
    rsPredTauRange.max = rsPredTauRange.min + rsPredTauRange.step * (rsPredTauValuesNum -1) ;
    rsPredTauRange.prefix = UnitPfxMicro;
    rsPredTauRange.unit = "s";

    /*! FEATURES/COMPENSABLES USER DOMAIN CpVc*/
    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    uCpVcCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN Cm*/
    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    uCmCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN Rs*/
    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    uRsCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN RsCp*/
    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    uRsCpCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN RsPg*/
    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    uRsPgCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN CpCc*/
    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    uCpCcCompensable.resize(currentChannelsNum);

    /*! Default values */
    currentRange = vcCurrentRangesArray[defaultVcCurrentRangeIdx];
    currentResolution = currentRange.step;
    voltageRange = vcVoltageRangesArray[defaultVcVoltageRangeIdx];
    voltageResolution =voltageRange.step;
    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    // Default USER DOMAIN compensation parameters
    double defaultPipetteInjCapacitance = pipetteInjCapacitance[0];
    double defaultMembraneCapValueInjCapacitance = membraneCapValueInjCapacitance[0];
    double default_U_CpVc = defaultPipetteInjCapacitance;
    double default_U_Cm = defaultMembraneCapValueInjCapacitance;
    double default_U_Rs = 0.0;
    double default_U_RsCp = 0.0;
    double default_U_RsPg = rsPredGainRange.min;
    double default_U_CpCc = defaultPipetteInjCapacitance;

    // Selected default Idx
    selectedVcCurrentRangeIdx = defaultVcCurrentRangeIdx;
    selectedVcVoltageRangeIdx = defaultVcVoltageRangeIdx;
    selectedVcCurrentFilterIdx = defaultVcCurrentFilterIdx;
    selectedSamplingRateIdx = defaultSamplingRateIdx;

    fill(selectedVoltageHoldVector.begin(), selectedVoltageHoldVector.end(), defaultVoltageHoldTuner);
    fill(selectedCurrentHoldVector.begin(), selectedCurrentHoldVector.end(), defaultCurrentHoldTuner);
    fill(selectedCalibVcCurrentGainVector.begin(), selectedCalibVcCurrentGainVector.end(), defaultCalibVcCurrentGain);
    fill(selectedCalibVcCurrentOffsetVector.begin(), selectedCalibVcCurrentOffsetVector.end(), defaultCalibVcCurrentOffset);

    // Initialization of the USER compensation domain with standard parameters
    for(int i = 0; i < currentChannelsNum; i++){
        compValueMatrix[i][1] = default_U_CpVc;
        compValueMatrix[i][2] = default_U_Cm;
        compValueMatrix[i][3] = default_U_Rs;
        compValueMatrix[i][4] = default_U_RsCp;
        compValueMatrix[i][5] = default_U_RsPg;
        compValueMatrix[i][6] = default_U_CpCc;
    }

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;
    MultiCoder::MultiCoderConfig_t multiCoderConfig;

    /*! Asic reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    asicResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(asicResetCoder);

    /*! FPGA reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    fpgaResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(fpgaResetCoder);

    /*! DOC reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 1;
    docResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(docResetCoder);

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 4;
    samplingRateCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(samplingRateCoder);

    /*! Clamping mode */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 7;
    boolConfig.bitsNum = 8;
    clampingModeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(clampingModeCoder);

    /*! DOC override */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 9;
    boolConfig.bitsNum = 1;
    docOverrideCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(docOverrideCoder);

    /*! Current range VC */
    boolConfig.initialWord = 1+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentRangeCoder);

    /*! Voltage range VC */
    boolConfig.initialWord = 1+9; //updated
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageRangeCoder);

    /*! Current range CC */
    boolConfig.initialWord = 1+9; //updated
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 4;
    ccCurrentRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccCurrentRangeCoder);

    /*! Voltage range CC */
    boolConfig.initialWord = 1+9; //updated
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 4;
    ccVoltageRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccVoltageRangeCoder);

    /*! Current filter VC */
    boolConfig.initialWord = 2+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentFilterCoder);

    /*! Voltage filter VC */
    boolConfig.initialWord = 2+9; //updated
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    boolConfig.initialWord = 2+9; //updated
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 4;
    ccCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccCurrentFilterCoder);

    /*! Voltage filter CC */
    boolConfig.initialWord = 2+9; //updated
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 4;
    ccVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccVoltageFilterCoder);

    /*! Digital offset compensation */
    boolConfig.initialWord = 3+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    digitalOffsetCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        digitalOffsetCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(digitalOffsetCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }


    /*! Enable stimulus */
    boolConfig.initialWord = 27+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    enableStimulusCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        enableStimulusCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(enableStimulusCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Turn channels on */
    boolConfig.initialWord = 51+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    turnChannelsOnCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        turnChannelsOnCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(turnChannelsOnCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! V holding tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vHoldTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 411+9; //updated
        doubleConfig.resolution = vHoldRange[rangeIdx].step;
        doubleConfig.minValue = vHoldRange[rangeIdx].min;
        doubleConfig.maxValue = vHoldRange[rangeIdx].max;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! C holding tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    cHoldTunerCoders.resize(CCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < CCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 411+9; //updated
        doubleConfig.resolution = cHoldRange[rangeIdx].step;
        doubleConfig.minValue = cHoldRange[rangeIdx].min;
        doubleConfig.maxValue = cHoldRange[rangeIdx].max;
        cHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            cHoldTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(cHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! VC current gain tuner */
    doubleConfig.initialWord = 843+9; //updated
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibVcCurrentGainRange.step;
    doubleConfig.minValue = calibVcCurrentGainRange.min;
    doubleConfig.maxValue = calibVcCurrentGainRange.max;
    calibVcCurrentGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcCurrentGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(calibVcCurrentGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! VC current offset tuner */
    calibVcCurrentOffsetCoders.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 1227+9; //updated
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcCurrentOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcCurrentOffsetRanges[rangeIdx].min;
        doubleConfig.maxValue = calibVcCurrentOffsetRanges[rangeIdx].max;
        calibVcCurrentOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcCurrentOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcCurrentOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }


    /*! Cfast / pipette capacitance compensation ENABLE */
    boolConfig.initialWord = 1611+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    pipetteCapEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        pipetteCapEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(pipetteCapEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Cfast / pipette capacitance compensation VALUE*/
    pipetteCapValCompensationMultiCoders.resize(currentChannelsNum);

    boolConfig.initialWord = 1635+9;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 1635+9;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;

    multiCoderConfig.doubleCoderVector.resize(pipetteCapacitanceRanges);
    multiCoderConfig.thresholdVector.resize(pipetteCapacitanceRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        /*! to encode the range, last 2 bits of the total 8 bits of Cfast compenstion for each channel*/
        multiCoderConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiCoderConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < pipetteCapacitanceRanges; rangeIdx++) {
            doubleConfig.minValue = pipetteCapacitanceRange_pF[rangeIdx].min; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.maxValue = pipetteCapacitanceRange_pF[rangeIdx].max; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.resolution = pipetteCapacitanceRange_pF[rangeIdx].step; /*! \todo RECHECK THESE VALUES!*/

            multiCoderConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiCoderConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < pipetteCapacitanceRanges-1) {
                /*! \todo RECHECK: computed as the mean between the upper bound (Cmax) of this range and the lower bound (Cmin) of the next range */
                multiCoderConfig.thresholdVector[rangeIdx] = 0.5*(pipetteCapacitanceRange_pF[rangeIdx].max + pipetteCapacitanceRange_pF[rangeIdx+1].min);
            }
        }
        pipetteCapValCompensationMultiCoders[idx] = new MultiCoder(multiCoderConfig);
        coders.push_back(pipetteCapValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 8;
        if(boolConfig.initialBit > 14){
            boolConfig.initialBit = 6;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cfast value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation ENABLE */
    boolConfig.initialWord = 1827+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    membraneCapEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        membraneCapEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(membraneCapEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation */
    membraneCapValCompensationMultiCoders.resize(currentChannelsNum);

    boolConfig.initialWord = 1851+9;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 1851+9;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;

    multiCoderConfig.doubleCoderVector.resize(membraneCapValueRanges);
    multiCoderConfig.thresholdVector.resize(membraneCapValueRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        /*! to encode the range, last 2 bits of the total 8 bits of Cfast compenstion for each channel*/
        multiCoderConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiCoderConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < membraneCapValueRanges; rangeIdx++) {
            doubleConfig.minValue = membraneCapValueRange_pF[rangeIdx].min; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.maxValue = membraneCapValueRange_pF[rangeIdx].max; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.resolution = membraneCapValueRange_pF[rangeIdx].step; /*! \todo RECHECK THESE VALUES!*/

            multiCoderConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiCoderConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < membraneCapValueRanges-1) {
                /*! \todo RECHECK: computed as the mean between the upper bound (Cmax) of this range and the lower bound (Cmin) of the next range */
                multiCoderConfig.thresholdVector[rangeIdx] = 0.5*(membraneCapValueRange_pF[rangeIdx].max + membraneCapValueRange_pF[rangeIdx+1].min);
            }
        }
        membraneCapValCompensationMultiCoders[idx] = new MultiCoder(multiCoderConfig);
        coders.push_back(membraneCapValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 8;
        if(boolConfig.initialBit > 14){
            boolConfig.initialBit = 6;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cslow value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation TAU and TAU RANGES */
    membraneCapTauValCompensationMultiCoders.resize(currentChannelsNum);

    doubleConfig.initialWord = 2043+9;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    boolConfig.initialWord = 2235+9;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;

    multiCoderConfig.doubleCoderVector.resize(membraneCapTauValueRanges);
    multiCoderConfig.thresholdVector.resize(membraneCapTauValueRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        /*! to encode the range, individual bits starting from word 2244*/
        multiCoderConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiCoderConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < membraneCapTauValueRanges; rangeIdx++) {
            doubleConfig.minValue =  membraneCapTauValueRange_us[rangeIdx].min;
            doubleConfig.maxValue = membraneCapTauValueRange_us[rangeIdx].max;
            doubleConfig.resolution = membraneCapTauValueRange_us[rangeIdx].step;

            multiCoderConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiCoderConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < membraneCapTauValueRanges-1) {
//                multiCoderConfig.thresholdVector[rangeIdx] = membraneCapTauValueMax_us[rangeIdx] + membraneCapTauValueStep_us[rangeIdx];
                multiCoderConfig.thresholdVector[rangeIdx] = membraneCapTauValueRange_us[rangeIdx].max + membraneCapTauValueRange_us[rangeIdx].step;
            }
        }
        membraneCapTauValCompensationMultiCoders[idx] = new MultiCoder(multiCoderConfig);
        coders.push_back(membraneCapTauValCompensationMultiCoders[idx]);


        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }

        boolConfig.initialBit++;
        if(boolConfig.initialBit == CMC_BITS_PER_WORD){
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs correction compensation ENABLE*/
    boolConfig.initialWord = 2259+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    rsCorrEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsCorrEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(rsCorrEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs correction compensation VALUE*/
    doubleConfig.initialWord = 2283+9; //updated
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;
    doubleConfig.resolution = rsCorrValueRange.step;
    doubleConfig.minValue = rsCorrValueRange.min;
    doubleConfig.maxValue = rsCorrValueRange.max;
    rsCorrValCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsCorrValCompensationCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(rsCorrValCompensationCoders[idx]);
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Rs correction compensation BANDWIDTH*/
    /*! \todo QUESTO VIENE IMPATTATO DALLA SWITCHED CAP FREQUENCY SOILO  A LIVELLO DI RAPPRESENTAZINE DI STRINGHE PER LA BANDA NELLA GUI. ATTIVAMENTE QUI NN FACCIAMO NULLA!!!!!*/
    boolConfig.initialWord = 2475+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 3;
    rsCorrBwCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsCorrBwCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(rsCorrBwCompensationCoders[idx]);
        boolConfig.initialBit += 4;
        if (boolConfig.initialBit >12) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs PREDICTION compensation ENABLE*/
    boolConfig.initialWord = 2571+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    rsPredEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsPredEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(rsPredEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs prediction compensation GAIN*/
    doubleConfig.initialWord = 2595+9; //updated
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;
    doubleConfig.resolution = rsPredGainRange.step;
    doubleConfig.minValue = rsPredGainRange.min;
    doubleConfig.maxValue = rsPredGainRange.max;
    rsPredGainCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsPredGainCompensationCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(rsPredGainCompensationCoders[idx]);
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Rs prediction compensation TAU*/
    doubleConfig.initialWord = 2787+9; //updated
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;
    doubleConfig.resolution = rsPredTauRange.step;
    doubleConfig.minValue = rsPredTauRange.min;
    doubleConfig.maxValue = rsPredTauRange.max;
    rsPredTauCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsPredTauCompensationCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(rsPredTauCompensationCoders[idx]);
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[2] = 0x0070; // fans on by default
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

MessageDispatcher_384PatchClamp_V01::~MessageDispatcher_384PatchClamp_V01() {

}

void MessageDispatcher_384PatchClamp_V01::initializeHW() {
    this->resetFpga(true, true);
    this->resetFpga(false, false);

//    this->resetAsic(true, true);
//    this_thread::sleep_for(chrono::milliseconds(100));
//    this->resetAsic(false, true);
}

//void MessageDispatcher_384PatchClamp_V01::updateDeviceStatus(vector <bool> &fsmRunFlag, bool &poreForming, bool &communicationError) {
//    for (int idx = 0; idx < fsmStateChannelsNum; idx++) {
//        fsmRunFlag[idx] = (infoStruct.status & (0x0001 << idx)) == 0 ? false : true;
//    }
//    poreForming = (infoStruct.status & (0x0001 << 8)) == 0 ? false : true;
//    communicationError = (infoStruct.status & (0x0001 << 9)) == 0 ? false : true;
//}

//void MessageDispatcher_384PatchClamp_V01::updateVoltageOffsetCompensations(vector <Measurement_t> &offsets) {
//    for (int idx = 0; idx < currentChannelsNum; idx++) {
//        offsets[idx] = voltageOffsetCompensationGain*(double)infoStruct.vComp[idx];
//    }
//}
