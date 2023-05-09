#include "messagedispatcher_384patchclamp.h"
#include "utils.h"

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

    txDataWords = 2988; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
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
    selectedRsCorrBws.resize(currentChannelsNum);

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

    membraneCapValueInjCapacitance = {5.0, 15.0, 45.0, 135.0};
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
        uCmCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN Rs*/  
    uRsCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN RsCp*/
    uRsCpCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN RsPg*/
    uRsPgCompensable.resize(currentChannelsNum);

    /*! FEATURES/COMPENSABLES USER DOMAIN CpCc*/
    uCpCcCompensable.resize(currentChannelsNum);

    /*! COMPENSATION OPTIONS STRINGS*/
    compensationOptionStrings.resize(CompensationTypesNum);
    compensationOptionStrings[CompRsCorr].resize(CompensationRsCorrBwNum);

    rsCorrBwArray.resize(CompensationRsCorrBwNum);
    rsCorrBwArray[CompensationRsCorrBw39_789kHz].value = 39.789 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw39_789kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw39_789kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw19_894kHz].value = 19.894 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw19_894kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw19_894kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw9_947kHz].value = 9.947 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw9_947kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw9_947kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw4_974kHz].value = 4.974 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw4_974kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw4_974kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw2_487kHz].value = 2.487 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw2_487kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw2_487kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw1_243kHz].value = 1.243 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw1_243kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw1_243kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw0_622kHz].value = 0.622 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw0_622kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw0_622kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw0_311kHz].value = 0.311 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw0_311kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw0_311kHz].unit = "Hz";
    defaultRsCorrBwIdx = CompensationRsCorrBw39_789kHz;

    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw39_789kHz] = rsCorrBwArray[CompensationRsCorrBw39_789kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw19_894kHz] = rsCorrBwArray[CompensationRsCorrBw19_894kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw9_947kHz] = rsCorrBwArray[CompensationRsCorrBw9_947kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw4_974kHz] = rsCorrBwArray[CompensationRsCorrBw4_974kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw2_487kHz] = rsCorrBwArray[CompensationRsCorrBw2_487kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw1_243kHz] = rsCorrBwArray[CompensationRsCorrBw1_243kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw0_622kHz] = rsCorrBwArray[CompensationRsCorrBw0_622kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw0_311kHz] = rsCorrBwArray[CompensationRsCorrBw0_311kHz].niceLabel();


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
    defaultUserDomainParams.resize(CompensationUserParamsNum);
    defaultUserDomainParams[U_CpVc] = pipetteCapacitanceRange_pF[0].min;
    defaultUserDomainParams[U_Cm] = membraneCapValueRange_pF[0].min;
    defaultUserDomainParams[U_Rs] = membraneCapTauValueRange_us[0].min/membraneCapValueRange_pF[0].min;
    defaultUserDomainParams[U_RsCp] = 1.0;
    defaultUserDomainParams[U_RsPg] = rsPredGainRange.min;
    defaultUserDomainParams[U_CpCc] = pipetteCapacitanceRange_pF[0].min;

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
        compValueMatrix[i][U_CpVc] =  defaultUserDomainParams[U_CpVc];
        compValueMatrix[i][U_Cm] =  defaultUserDomainParams[U_Cm];
        compValueMatrix[i][U_Rs] =  defaultUserDomainParams[U_Rs];
        compValueMatrix[i][U_RsCp] =  defaultUserDomainParams[U_RsCp];
        compValueMatrix[i][U_RsPg] =  defaultUserDomainParams[U_RsPg];
        compValueMatrix[i][U_CpCc] =  defaultUserDomainParams[U_CpCc];
    }

    // Initialization of the RsCorr bandwidth option with default option
    for(int i = 0; i < currentChannelsNum; i++){
        selectedRsCorrBws[i] = defaultRsCorrBwIdx;
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
    boolConfig.initialWord = 10; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentRangeCoder);

    /*! Voltage range VC */
    boolConfig.initialWord = 10; //updated
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageRangeCoder);

    /*! Current range CC */
    boolConfig.initialWord = 10; //updated
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 4;
    ccCurrentRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccCurrentRangeCoder);

    /*! Voltage range CC */
    boolConfig.initialWord = 10; //updated
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 4;
    ccVoltageRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccVoltageRangeCoder);

    /*! Current filter VC */
    boolConfig.initialWord = 11; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentFilterCoder);

    /*! Voltage filter VC */
    boolConfig.initialWord = 11; //updated
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    boolConfig.initialWord = 11; //updated
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 4;
    ccCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccCurrentFilterCoder);

    /*! Voltage filter CC */
    boolConfig.initialWord = 11; //updated
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 4;
    ccVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccVoltageFilterCoder);

    /*! Digital offset compensation */
    boolConfig.initialWord = 12; //updated
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
    boolConfig.initialWord = 36; //updated
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
    boolConfig.initialWord = 60; //updated
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
        doubleConfig.initialWord = 420; //updated
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
        doubleConfig.initialWord = 420; //updated
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
    doubleConfig.initialWord = 852; //updated
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
        doubleConfig.initialWord = 1236; //updated
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
    boolConfig.initialWord = 1620; //updated
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

    boolConfig.initialWord = 1644;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 1644;
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
    boolConfig.initialWord = 1836; //updated
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

    boolConfig.initialWord = 1860;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 1860;
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

    doubleConfig.initialWord = 2052;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    boolConfig.initialWord = 2244;
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
    boolConfig.initialWord = 2268; //updated
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
    doubleConfig.initialWord = 2292; //updated
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
    boolConfig.initialWord = 2484; //updated
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
    boolConfig.initialWord = 2580; //updated
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
    doubleConfig.initialWord = 2604; //updated
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
    doubleConfig.initialWord = 2796; //updated
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

    // Initialization Compensable in USER domain
    vector<double> defaultAsicDomainParams;
    for(int i = 0; i<currentChannelsNum; i++){
        defaultAsicDomainParams = user2AsicDomainTransform(i, defaultUserDomainParams);
        asic2UserDomainCompensable(i, defaultAsicDomainParams, defaultUserDomainParams);
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

/*! \todo FCON recheck*/
ErrorCodes_t MessageDispatcher_384PatchClamp_V01::getCompFeatures(uint16_t paramToExtractFeatures, vector<RangedMeasurement_t> &compensationFeatures, double &defaultParamValue){
    switch(paramToExtractFeatures){
    case U_CpVc:
        if(pipetteCapEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else {
            for(int i = 0; i < currentChannelsNum; i++){
                compensationFeatures[i] = uCpVcCompensable[i];
                defaultParamValue = defaultUserDomainParams[U_CpVc];
            }
        }
    break;

    case U_Cm:
        if(membraneCapEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else {
            for(int i = 0; i < currentChannelsNum; i++){
                compensationFeatures[i] = uCmCompensable[i];
                defaultParamValue = defaultUserDomainParams[U_Cm];
            }
        }
    break;

    case U_Rs:
        if(membraneCapTauValCompensationMultiCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else {
            for(int i = 0; i < currentChannelsNum; i++){
                compensationFeatures[i] = uRsCompensable[i];
                defaultParamValue = defaultUserDomainParams[U_Rs];
            }
        }
    break;

    case U_RsCp:
        if(rsCorrValCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else {
            for(int i = 0; i < currentChannelsNum; i++){
                compensationFeatures[i] = uRsCpCompensable[i];
                defaultParamValue = defaultUserDomainParams[U_RsCp];
            }
        }
    break;
    case U_RsPg:
        if(rsPredEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else {
            for(int i = 0; i < currentChannelsNum; i++){
                compensationFeatures[i] = uRsPgCompensable[i];
                defaultParamValue = defaultUserDomainParams[U_RsPg];
            }
        }
    break;
    case U_CpCc:
        if(rsPredEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else {
            for(int i = 0; i < currentChannelsNum; i++){
                compensationFeatures[i] = uCpCcCompensable[i];
                defaultParamValue = defaultUserDomainParams[U_CpCc];
            }
        }
    break;
    }
    return Success;
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::getCompOptionsFeatures(CompensationTypes type ,std::vector <std::string> &compOptionsArray){
    switch(type)
    {
    case CompRsCorr:
        if(rsCorrBwArray.size()==0){
            return ErrorFeatureNotImplemented;
        } else {
            compOptionsArray.resize(CompensationRsCorrBwNum);
            for(uint32_t i = 0; i < CompensationRsCorrBwNum; i++){
                compOptionsArray[i] = compensationOptionStrings[CompRsCorr][i];
            }
            return Success;
        }

    break;
    }
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::getCompValueMatrix(std::vector<std::vector<double>> &compValueMatrix){
    compValueMatrix = this->compValueMatrix;
    return Success;
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::enableCompensation(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> onValues, bool applyFlagIn){
        switch(compTypeToEnable){
        case CompCfast:
            if(pipetteCapEnCompensationCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            for(int i = 0; i<channelIndexes.size(); i++){
                compCfastEnable[channelIndexes[i]] = onValues[i];
                pipetteCapEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            }
        break;

        case CompCslow:
            if(membraneCapEnCompensationCoders.size() == 0 ){
                return ErrorFeatureNotImplemented;
            }
            for(int i = 0; i<channelIndexes.size(); i++){
                compCslowEnable[channelIndexes[i]] = onValues[i];
                membraneCapEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            }
        break;

        case CompRsCorr:
            if(rsCorrEnCompensationCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            for(int i = 0; i<channelIndexes.size(); i++){
                compRsCorrEnable[channelIndexes[i]] = onValues[i];
                rsCorrEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            }
        break;

        case CompRsPred:
            if(rsPredEnCompensationCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            for(int i = 0; i<channelIndexes.size(); i++){
                compRsPredEnable[channelIndexes[i]] = onValues[i];
                rsPredEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            }
        break;
        }
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::enableVcCompensations(bool enable){
    areVcCompsEnabled = enable;

    for(int i = 0; i < currentChannelsNum; i++){
        pipetteCapEnCompensationCoders[i]->encode(areVcCompsEnabled, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        membraneCapEnCompensationCoders[i]->encode(areVcCompsEnabled, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        rsCorrEnCompensationCoders[i]->encode(areVcCompsEnabled, txStatus, txModifiedStartingWord, txModifiedEndingWord);
        rsPredEnCompensationCoders[i]->encode(areVcCompsEnabled, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    return Success;
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::enableCcCompensations(bool enable){
    areCcCompsEnabled = enable;

    for(int i = 0; i < currentChannelsNum; i++){
        pipetteCapEnCompensationCoders[i]->encode(areCcCompsEnabled, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    return Success;
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::setCompValues(std::vector<uint16_t> channelIndexes, CompensationUserParams paramToUpdate, std::vector<double> newParamValues, bool applyFlagIn){
    // make local copy of the user domain param vectors
    vector<vector<double>> localCompValueSubMatrix;
    localCompValueSubMatrix.resize(channelIndexes.size());
    for(int i = 0; i< channelIndexes.size(); i++){
        localCompValueSubMatrix[i] = this->compValueMatrix[channelIndexes[i]];
    }

    // for each user param vector
    for (int j = 0; j < localCompValueSubMatrix.size(); j++){
        // update value in user domain
        localCompValueSubMatrix[j][paramToUpdate] = newParamValues[j];

        // convert user domain to asic domain
        vector<double> asicParams = user2AsicDomainTransform(channelIndexes[j], localCompValueSubMatrix[j]);
        double temp;

        // select asicParam to encode based on enum
        /*! \todo FCON recheck: IN CASE THERE'S INTERACTION AMONG ASICPARAMS, THEY COULD BE DESCRIBED IN THE SWITCH-CASE */
        switch(paramToUpdate)
        {
        case U_CpVc:
            if(pipetteCapValCompensationMultiCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            //encode
            temp = pipetteCapValCompensationMultiCoders[channelIndexes[j]]->encode(asicParams[A_Cp], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Cp] = temp;
        break;
        case U_Cm:
            if(membraneCapValCompensationMultiCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            //encode
            temp = membraneCapValCompensationMultiCoders[channelIndexes[j]]->encode(asicParams[A_Cm], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Cm] = temp;
        break;
        case U_Rs:
            if(membraneCapTauValCompensationMultiCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            //encode
            temp = membraneCapTauValCompensationMultiCoders[channelIndexes[j]]->encode(asicParams[A_Taum], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Taum] = temp;
        break;
        case U_RsCp:
            if(rsCorrValCompensationCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            //encode
            temp = rsCorrValCompensationCoders[channelIndexes[j]]->encode(asicParams[A_RsCr], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_RsCr] = temp;
        break;
        case U_RsPg:
            if(rsPredGainCompensationCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            //encode
            temp = rsPredGainCompensationCoders[channelIndexes[j]]->encode(asicParams[A_RsPg], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_RsPg] = temp;
        break;
        case U_CpCc:
            if(rsPredTauCompensationCoders.size() == 0){
                return ErrorFeatureNotImplemented;
            }
            //encode
            temp = rsPredTauCompensationCoders[channelIndexes[j]]->encode(asicParams[A_RsPtau], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_RsPtau] = temp;
        break;
        }

        // convert to user domain
        double oldUCpVc = localCompValueSubMatrix[j][U_CpVc];
        double oldUCpCc = localCompValueSubMatrix[j][U_CpCc];
        localCompValueSubMatrix[j] = asic2UserDomainTransform(channelIndexes[j], asicParams, oldUCpVc, oldUCpCc);

        /*! \todo call here function to compute the compensable value ranges in the user domain*/
        asic2UserDomainCompensable(channelIndexes[j], asicParams, localCompValueSubMatrix[j]);

        //copy back to compValuematrix
        this->compValueMatrix[channelIndexes[j]] = localCompValueSubMatrix[j];

    //end for
    }
    // stack outgoing message
    if (applyFlagIn) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::setCompOptions(std::vector<uint16_t> channelIndexes, CompensationTypes type, std::vector<uint16_t> options, bool applyFlagIn){
    switch(type)
    {
    case CompRsCorr:
        if (rsCorrBwCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;
        } else {
            for(uint32_t i = 0; i < channelIndexes.size(); i++){
                selectedRsCorrBws[i] = options[i];
                rsCorrBwCompensationCoders[channelIndexes[i]]->encode(options[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            }

            if (applyFlagIn) {
                this->stackOutgoingMessage(txStatus);
            }
            return Success;
        }
    break;
    }
}

vector<double> MessageDispatcher_384PatchClamp_V01::user2AsicDomainTransform(int chIdx, vector<double> userDomainParams){
    vector<double> asicDomainParameter;
    asicDomainParameter.resize(CompensationAsicParamsNum);
    double cp; // pipette capacitance
    double cm; // membrane capacitance
    double taum; // membrane capacitance tau
    double rsCr; // Rseries Correction
    double rsPg; //Rseries prediction gain
    double rsPtau; // Resies prediction tau

    double asicCmCinj;

    // membrane capacitance domain conversion
    cm = userDomainParams[U_Cm];

    // pipette capacitance VC to pipette capacitance domain conversion
    /*! \todo aggiungere check se il multicoder esiste sulla size del vettore di puntatori  a multiCoder*/

    MultiCoder::MultiCoderConfig_t aaa;
    membraneCapValCompensationMultiCoders[chIdx]->getMultiConfig(aaa);
    asicCmCinj = computeAsicCmCinj(userDomainParams[U_Cm], compCslowEnable[chIdx], aaa);

    if(amIinVoltageClamp){
        cp = userDomainParams[U_CpVc] + asicCmCinj;
    } else {
        // A_Cp
        cp = userDomainParams[U_CpCc];
    }

    // Series resistance to Membrane tau domain conversion
    taum = userDomainParams[U_Cm] * userDomainParams[U_Rs];

    // Series correction percentage to Series correction resistance domain conversion
    rsCr = userDomainParams[U_Rs] * userDomainParams[U_RsCp];

    // Series prediction gain domain conversion
    rsPg = userDomainParams[U_RsPg];

    // pipette capacitance CC to Series prediction tau domain conversion
    /*! \todo MPAC recheck: added +1 after discussion with MBEN and FCON */
    rsPtau = taum / (userDomainParams[U_RsPg] + 1);

    asicDomainParameter[A_Cp] = cp;
    asicDomainParameter[A_Cm] = cm;
    asicDomainParameter[A_Taum] = taum;
    asicDomainParameter[A_RsCr] = rsCr;
    asicDomainParameter[A_RsPg] = rsPg;
    asicDomainParameter[A_RsPtau] = rsPtau;

    return asicDomainParameter;
}

std::vector<double> MessageDispatcher_384PatchClamp_V01::asic2UserDomainTransform(int chIdx, std::vector<double> asicDomainParams, double oldUCpVc, double oldUCpCc){
    vector<double> userDomainParameter;
    userDomainParameter.resize(CompensationUserParamsNum);

    double cpVc;
    double cm;
    double rs;
    double rsCp;
    double rsPg;
    double cpCC;

    double asicCmCinj;

    MultiCoder::MultiCoderConfig_t aaa;
    membraneCapValCompensationMultiCoders[chIdx]->getMultiConfig(aaa);
    asicCmCinj = computeAsicCmCinj(asicDomainParams[A_Cm], compCslowEnable[chIdx], aaa);

    //  pipette capacitance to pipette capacitance VC domain conversion
    if(amIinVoltageClamp){
        cpVc = asicDomainParams[A_Cp] - asicCmCinj;
    } else {
        cpVc = oldUCpVc; /*! \todo recheck */
    }

    // membrane capacitance domain conversion
    cm = asicDomainParams[A_Cm];

    // membrane tau to Series resistance domain conversion
    rs = asicDomainParams[A_Taum] / asicDomainParams[A_Cm];

    // Series correction resistance to Series correction percentage domain conversion
    /*! \todo FCON recheck: should use U_Rs's value after it's been updated according to clipping */
    rsCp = asicDomainParams[A_RsCr] / rs;

    // Series prediction gain domain conversion
    /*! \todo MPAC RECHECK: added -1 after discussion with MBEN and FCON */
    rsPg = -1 + asicDomainParams[A_Taum] / asicDomainParams[A_RsPtau];

    // Series prediction tau to Pipette capacitance CC domain conversion
    if(amIinVoltageClamp){
        cpCC = oldUCpCc; /*! \todo recheck */
    } else {
        cpCC = asicDomainParams[A_Cp];
    }

    userDomainParameter[U_CpVc] = cpVc;
    userDomainParameter[U_Cm] = cm;
    userDomainParameter[U_Rs] = rs;
    userDomainParameter[U_RsCp] = rsCp;
    userDomainParameter[U_RsPg] = rsPg;
    userDomainParameter[U_CpCc] = cpCC;
    return userDomainParameter;
}

ErrorCodes_t MessageDispatcher_384PatchClamp_V01::asic2UserDomainCompensable(int chIdx, std::vector<double> asicDomainParams, std::vector<double> userDomainParams){
    /*! \todo still to understand how to obtain them. COuld they be imputs of the function?*/
    vector<double> potentialMaxs;
    vector<double> potentialMins;

    double myInfinity = numeric_limits<double>::infinity();

    double asicCmCinj;
    MultiCoder::MultiCoderConfig_t aaa;
    membraneCapValCompensationMultiCoders[chIdx]->getMultiConfig(aaa);
    asicCmCinj = computeAsicCmCinj(asicDomainParams[A_Cm], compCslowEnable[chIdx], aaa);

    /*! Compensable for U_CpVc*/
    uCpVcCompensable[chIdx].max = pipetteCapacitanceRange_pF.back().max - asicCmCinj;

    potentialMins.push_back(pipetteCapacitanceRange_pF.back().min - asicCmCinj);
    potentialMins.push_back(0.0);
    uCpVcCompensable[chIdx].min = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    uCpVcCompensable[chIdx].step = pipetteCapacitanceRange_pF.front().step;

    /*! Compensable for U_Cm*/
    // max
    /*! MPAC: added +1 after discussion with MBEN and FCON*/
    potentialMaxs.push_back(membraneCapValueRange_pF.back().max);

    potentialMaxs.push_back(membraneCapTauValueRange_us.back().max/userDomainParams[U_Rs]);

    if(compCfastEnable[chIdx]){
        double zzz1;
        double zzz2;
        for (int i = 0; i < membraneCapValueInjCapacitance.size(); i++){
            zzz1 = membraneCapValueInjCapacitance[i] + userDomainParams[U_CpVc];
            if(zzz1 <= pipetteCapacitanceRange_pF.back().max){
                zzz2 = zzz1;
            }
        }
        potentialMaxs.push_back(zzz2);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    if(compRsPredEnable[chIdx]){
        potentialMaxs.push_back(rsPredTauRange.max*(userDomainParams[U_RsPg]+1)/userDomainParams[U_Rs]);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    uCmCompensable[chIdx].max = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    /*! MPAC: added +1 after discussion with MBEN and FCON*/
    potentialMins.push_back(membraneCapValueRange_pF.front().min);

    potentialMins.push_back(membraneCapTauValueRange_us.front().min/userDomainParams[U_Rs]);

    if(compRsPredEnable[chIdx]){
        potentialMins.push_back(rsPredTauRange.min*(userDomainParams[U_RsPg]+1)/userDomainParams[U_Rs]);
    } else {
        potentialMins.push_back(0.0);
    }

    uCmCompensable[chIdx].min = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    //step
    uCmCompensable[chIdx].step = membraneCapValueRange_pF.front().step;

    /*! Compensable for U_Rs*/
    //max
    /*! MPAC: added +1 after discussion with MBEN and FCON*/
    if(compCslowEnable[chIdx]){
        potentialMaxs.push_back(membraneCapTauValueRange_us.back().max/userDomainParams[U_Cm]);
    } else {
        potentialMaxs.push_back(membraneCapTauValueRange_us.back().max/uCmCompensable[chIdx].min);
    }

    if(compRsCorrEnable[chIdx]){
        potentialMaxs.push_back(rsCorrValueRange.max / userDomainParams[U_RsCp]);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    if(compRsPredEnable[chIdx]){
        potentialMaxs.push_back(rsPredTauRange.max * (userDomainParams[U_RsPg]+1) / userDomainParams[U_Cm]);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    uRsCompensable[chIdx].max = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    /*! MPAC: added +1 after discussion with MBEN and FCON*/
    if(compCslowEnable[chIdx]){
        potentialMins.push_back(membraneCapTauValueRange_us.front().min / userDomainParams[U_Cm]);
    } else {
        potentialMins.push_back(membraneCapTauValueRange_us.front().min / uCmCompensable[chIdx].max);
    }

    if(compRsCorrEnable[chIdx]){
        potentialMins.push_back(rsCorrValueRange.min / userDomainParams[U_RsCp]);
    } else {
        potentialMins.push_back(0);
    }

    if(compRsPredEnable[chIdx]){
        potentialMins.push_back(rsPredTauRange.min * (userDomainParams[U_RsPg]+1) / userDomainParams[U_Cm]);
    } else {
        potentialMins.push_back(0);
    }

    uRsCompensable[chIdx].min = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    //step
    uRsCompensable[chIdx].step = 0.1e6; //Ohm

    /*! Compensable for U_RsCp*/
    // max
    /*! \todo Pay attention to possible divisions by 0*/
    potentialMaxs.push_back(rsCorrValueRange.max / userDomainParams[U_Rs]);
    potentialMaxs.push_back(100.0); //%
    uRsCpCompensable[chIdx].max = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    /*! \todo Pay attention to possible divisions by 0*/
    potentialMins.push_back(0.0); //%
    potentialMins.push_back(rsCorrValueRange.min / userDomainParams[U_Rs]);
    uRsCpCompensable[chIdx].min = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    uRsCpCompensable[chIdx].step = 1.0; //%

    /*! Compensable for U_RsPg*/
    //max
    /*! MPAC: added -1 after discussion with MBEN and FCON*/
    potentialMaxs.push_back(rsPredGainRange.max);
    potentialMaxs.push_back(-1 + userDomainParams[U_Cm] * userDomainParams[U_Rs] / rsPredTauRange.min);
    uRsPgCompensable[chIdx].max = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    /*! MPAC: added -1 after discussion with MBEN and FCON*/
    potentialMins.push_back(rsPredGainRange.min);
    potentialMins.push_back(-1 + userDomainParams[U_Cm] * userDomainParams[U_Rs] / rsPredTauRange.max);
    uRsPgCompensable[chIdx].min = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    uRsPgCompensable[chIdx].step = rsPredGainRange.step;

    /*! Compensable for U_CpCc*/
    uCpCcCompensable[chIdx].max = pipetteCapacitanceRange_pF.back().max;
    uCpCcCompensable[chIdx].min = pipetteCapacitanceRange_pF.front().min;
    uCpCcCompensable[chIdx].step = pipetteCapacitanceRange_pF.front().step;

    return Success;
}

double MessageDispatcher_384PatchClamp_V01::computeAsicCmCinj(double cm, bool chanCslowEnable, MultiCoder::MultiCoderConfig_t multiconfigCslow){
    bool done = false;
    int i;
    double asicCmCinj;
    if(!amIinVoltageClamp || (amIinVoltageClamp && !chanCslowEnable)){
        asicCmCinj = 0;
    } else {
        for(i = 0; i<multiconfigCslow.thresholdVector.size(); i++){
            /*! \todo RECHECK: just <threshold as thresholds are as the mean between the upper bound (Cmax) of this range and the lower bound (Cmin) of the next range */
            if (cm < multiconfigCslow.thresholdVector[i] && !done){
                asicCmCinj = membraneCapValueInjCapacitance[i];
                done = true;
            }
        }
        if (!done){
            asicCmCinj = membraneCapValueInjCapacitance[i];
            done = true;
        }
    }
    return asicCmCinj;
}


