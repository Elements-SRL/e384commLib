#include "messagedispatcher_384nanopores.h"

MessageDispatcher_384NanoPores_V01::MessageDispatcher_384NanoPores_V01(std::string di) :
    MessageDispatcher_OpalKelly(di) {

    deviceName = "384NanoPores";

    fwName = "384NanoPores_V01_top.bit";

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
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = ClampingModality_t::VOLTAGE_CLAMP;
    defaultClampingModalityIdx = VoltageClamp;

    /*! Channel sources */
    availableVoltageSourcesIdxs.VoltageFromVoltageClamp = ChannelSourceVoltageFromVoltageClamp;

    /*! Protocols parameters */
    protocolFpgaClockFrequencyHz = 10.0e6;

    protocolTimeRange.step = 1000.0/protocolFpgaClockFrequencyHz;
    protocolTimeRange.min = LINT32_MIN*protocolTimeRange.step;
    protocolTimeRange.max = LINT32_MAX*protocolTimeRange.step;
    protocolTimeRange.prefix = UnitPfxMilli;
    protocolTimeRange.unit = "s";

    positiveProtocolTimeRange = protocolTimeRange;
    positiveProtocolTimeRange.min = 0.0;

    protocolFrequencyRange.step = protocolFpgaClockFrequencyHz/(256.0*(UINT24_MAX+1.0)); /*! 10.08MHz / 256 / 2^24 */
    protocolFrequencyRange.min = INT24_MIN*protocolFrequencyRange.step;
    protocolFrequencyRange.max = INT24_MAX*protocolFrequencyRange.step;
    protocolFrequencyRange.prefix = UnitPfxNone;
    protocolFrequencyRange.unit = "Hz";

    positiveProtocolFrequencyRange = protocolFrequencyRange;
    positiveProtocolFrequencyRange.min = 0.0;

    voltageProtocolStepImplemented = true;
    voltageProtocolRampImplemented = true;
    voltageProtocolSinImplemented = false;

    protocolWordOffset = 84;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange4uA].min = -4000000.0;
    vcCurrentRangesArray[VCCurrentRange4uA].max = 4000000.0;
    vcCurrentRangesArray[VCCurrentRange4uA].step = vcCurrentRangesArray[VCCurrentRange4uA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange4uA].prefix = UnitPfxPico;
    vcCurrentRangesArray[VCCurrentRange4uA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange200nA].min = -200000.0;
    vcCurrentRangesArray[VCCurrentRange200nA].max = 200000.0;
    vcCurrentRangesArray[VCCurrentRange200nA].step = vcCurrentRangesArray[VCCurrentRange200nA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange200nA].prefix = UnitPfxPico;
    vcCurrentRangesArray[VCCurrentRange200nA].unit = "A";
    defaultVcCurrentRangeIdx = VCCurrentRange200nA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].min = -500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].max = 500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.125;
    vcVoltageRangesArray[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange500mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange500mV;

    /*! Current ranges */
    /*! CC */
    ccCurrentRangesNum = CCCurrentRangesNum;

    /*! Voltage ranges */
    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;
    vcCurrentFiltersArray.resize(vcCurrentFiltersNum);
    vcCurrentFiltersArray[VCCurrentFilter100kHz].value = 100.0;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter20kHz].value = 20.0;
    vcCurrentFiltersArray[VCCurrentFilter20kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter20kHz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter20kHz;

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate7_5kHz].value = 7.5;
    samplingRatesArray[SamplingRate7_5kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate7_5kHz].unit = "Hz";
#ifdef E384NPR_ADDITIONAL_SR_FLAG
    samplingRatesArray[SamplingRate15kHz].value = 15.0;
    samplingRatesArray[SamplingRate15kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate15kHz].unit = "Hz";
    samplingRatesArray[SamplingRate30kHz].value = 30.0;
    samplingRatesArray[SamplingRate30kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate30kHz].unit = "Hz";
    samplingRatesArray[SamplingRate60kHz].value = 60.0;
    samplingRatesArray[SamplingRate60kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate60kHz].unit = "Hz";
    samplingRatesArray[SamplingRate120kHz].value = 120.0;
    samplingRatesArray[SamplingRate120kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate120kHz].unit = "Hz";
#endif
    defaultSamplingRateIdx = SamplingRate7_5kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate7_5kHz].value = 7.5;
    realSamplingRatesArray[SamplingRate7_5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate7_5kHz].unit = "Hz";
#ifdef E384NPR_ADDITIONAL_SR_FLAG
    realSamplingRatesArray[SamplingRate15kHz].value = 15.0;
    realSamplingRatesArray[SamplingRate15kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate15kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate30kHz].value = 30.0;
    realSamplingRatesArray[SamplingRate30kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate30kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate60kHz].value = 60.0;
    realSamplingRatesArray[SamplingRate60kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate60kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate120kHz].value = 120.0;
    realSamplingRatesArray[SamplingRate120kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate120kHz].unit = "Hz";
#endif

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate7_5kHz].value = 1.0/7.5;
    integrationStepArray[SamplingRate7_5kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate7_5kHz].unit = "s";
#ifdef E384NPR_ADDITIONAL_SR_FLAG
    integrationStepArray[SamplingRate15kHz].value = 1.0/15.0;
    integrationStepArray[SamplingRate15kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate15kHz].unit = "s";
    integrationStepArray[SamplingRate30kHz].value = 1.0/30.0;
    integrationStepArray[SamplingRate30kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate30kHz].unit = "s";
    integrationStepArray[SamplingRate60kHz].value = 1.0/60.0;
    integrationStepArray[SamplingRate60kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate60kHz].unit = "s";
    integrationStepArray[SamplingRate120kHz].value = 1.0/120.0;
    integrationStepArray[SamplingRate120kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate120kHz].unit = "s";
#endif

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate7_5kHz, VCCurrentFilter20kHz}
    #ifdef E384NPR_ADDITIONAL_SR_FLAG
        ,{SamplingRate15kHz, VCCurrentFilter20kHz},
        {SamplingRate30kHz, VCCurrentFilter20kHz},
        {SamplingRate60kHz, VCCurrentFilter100kHz},
        {SamplingRate120kHz, VCCurrentFilter100kHz}
    #endif
    };

    // mapping ADC Current Clamp
    // undefined

    /*! Calibraition param vectors*/
    allGainAdcMeas.resize(VCCurrentRangesNum);
    allGainAdcMeas[VCCurrentRange200nA].resize(currentChannelsNum);
    allGainAdcMeas[VCCurrentRange4uA].resize(currentChannelsNum);

    allOffsetAdcMeas.resize(VCCurrentRangesNum);
    allOffsetAdcMeas[VCCurrentRange200nA].resize(currentChannelsNum);
    allOffsetAdcMeas[VCCurrentRange4uA].resize(currentChannelsNum);

    //gainDacMeas;

    allOffsetDacMeas.resize(VCVoltageRangesNum);
    allOffsetDacMeas[VCVoltageRange500mV].resize(currentChannelsNum);

    /*! VC calibration voltage steps*/
    calibrationData.vcCalibStepsArrays.resize(VCCurrentRangesNum);
    calibrationData.vcCalibStepsArrays[VCCurrentRange4uA].resize(5);
    calibrationData.vcCalibStepsArrays[VCCurrentRange200nA].resize(5);
    calibrationData.vcCalibStepsArrays[VCCurrentRange4uA][0] = {-400.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange4uA][1] = {-200.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange4uA][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange4uA][3] = {200.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange4uA][4] = {400.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange200nA][0] = {-400.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange200nA][1] = {-200.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange200nA][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange200nA][3] = {200.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange200nA][4] = {400.0, UnitPfxMilli, "V"};

    /*! VC calibration resistances*/
    calibrationData.vcCalibResArray.resize(CalibResNum);
    calibrationData.vcCalibResArray[CalibRes120kOhm] = {120.0, UnitPfxKilo, "Ohm"}; // 4uA
    calibrationData.vcCalibResArray[CalibRes2_49MOhm] = {2.49, UnitPfxMega, "Ohm"}; // 200nA

    // mapping VC current range - calibration resistances
    calibrationData.vcCurrRange2CalibResMap = {
      {VCCurrentRange4uA, CalibRes120kOhm},
      {VCCurrentRange200nA, CalibRes2_49MOhm}
    };

    calibrationData.areCalibResistOnBoard = false;

    vHoldRange.resize(VCVoltageRangesNum);
    vHoldRange[VCVoltageRange500mV].min = -500.0;
    vHoldRange[VCVoltageRange500mV].max = 500.0;
    vHoldRange[VCVoltageRange500mV].step = 0.125;
    vHoldRange[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vHoldRange[VCVoltageRange500mV].unit = "V";
    selectedVoltageHoldVector.resize(currentChannelsNum);
    Measurement_t defaultVoltageHoldTuner = {0.0, vHoldRange[VCVoltageRange500mV].prefix, vHoldRange[VCVoltageRange500mV].unit};

    /*! VC current calib gain */
    calibVcCurrentGainRange.step = 1.0/1024.0;
    calibVcCurrentGainRange.min = 0;//SHORT_MIN * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.max = SHORT_MAX * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.prefix = UnitPfxNone;
    calibVcCurrentGainRange.unit = "";
    selectedCalibVcCurrentGainVector.resize(currentChannelsNum);
    defaultCalibVcCurrentGain = {1.0, calibVcCurrentGainRange.prefix, calibVcCurrentGainRange.unit};

    /*! VC current calib offset */
    calibVcCurrentOffsetRanges = vcCurrentRangesArray;
    selectedCalibVcCurrentOffsetVector.resize(currentChannelsNum);
    defaultCalibVcCurrentOffset = {0.0, calibVcCurrentOffsetRanges[defaultVcCurrentRangeIdx].prefix, calibVcCurrentOffsetRanges[defaultVcCurrentRangeIdx].unit};

    /*! VC Voltage calib gain (DAC)*/
    calibVcVoltageGainRange.step = 1.0/1024.0;
    calibVcVoltageGainRange.min = 0;
    calibVcVoltageGainRange.max = SHORT_MAX * calibVcVoltageGainRange.step;
    calibVcVoltageGainRange.prefix = UnitPfxNone;
    calibVcVoltageGainRange.unit = "";
    selectedCalibVcVoltageGainVector.resize(currentChannelsNum);
    defaultCalibVcVoltageGain = {1.0, calibVcVoltageGainRange.prefix, calibVcVoltageGainRange.unit};

    /*! VC Voltage calib offset (DAC)*/
    calibVcVoltageOffsetRanges = vcVoltageRangesArray;
    selectedCalibVcVoltageOffsetVector.resize(currentChannelsNum);
    defaultCalibVcVoltageOffset = {0.0, calibVcVoltageOffsetRanges[defaultVcVoltageRangeIdx].prefix, calibVcVoltageOffsetRanges[defaultVcVoltageRangeIdx].unit};

    /*! Gate voltage range*/
    gateVoltageRange.step = 1;
    gateVoltageRange.min = -24000;//SHORT_MIN * gateVoltageRange.step;
    gateVoltageRange.max = 24000;//SHORT_MAX * gateVoltageRange.step;
    gateVoltageRange.prefix = UnitPfxMilli;
    gateVoltageRange.unit = "V";
    selectedGateVoltageVector.resize(totalBoardsNum);
    Measurement_t defaultGateVoltage = {0.0, gateVoltageRange.prefix, gateVoltageRange.unit};

    /*! Source voltage range*/
    sourceVoltageRange.step = 0.0625;
    sourceVoltageRange.min = SHORT_MIN * sourceVoltageRange.step;
    sourceVoltageRange.max = SHORT_MAX * sourceVoltageRange.step;
    sourceVoltageRange.prefix = UnitPfxMilli;
    sourceVoltageRange.unit = "V";
    selectedSourceVoltageVector.resize(totalBoardsNum);
    Measurement_t defaultSourceVoltage = {0.0, sourceVoltageRange.prefix, sourceVoltageRange.unit};

    /*! Default values */
    currentRange = vcCurrentRangesArray[defaultVcCurrentRangeIdx];
    currentResolution = currentRange.step;
    voltageRange = vcVoltageRangesArray[defaultVcVoltageRangeIdx];
    voltageResolution =voltageRange.step;
    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    // Selected default Idx
    selectedVcCurrentRangeIdx = defaultVcCurrentRangeIdx;
    selectedVcVoltageRangeIdx = defaultVcVoltageRangeIdx;
    selectedVcCurrentFilterIdx = defaultVcCurrentFilterIdx;
    selectedSamplingRateIdx = defaultSamplingRateIdx;

    fill(selectedVoltageHoldVector.begin(), selectedVoltageHoldVector.end(), defaultVoltageHoldTuner);
    fill(selectedCalibVcCurrentGainVector.begin(), selectedCalibVcCurrentGainVector.end(), defaultCalibVcCurrentGain);
    fill(selectedCalibVcCurrentOffsetVector.begin(), selectedCalibVcCurrentOffsetVector.end(), defaultCalibVcCurrentOffset);
    fill(selectedCalibVcVoltageGainVector.begin(), selectedCalibVcVoltageGainVector.end(), defaultCalibVcVoltageGain);
    fill(selectedCalibVcVoltageOffsetVector.begin(), selectedCalibVcVoltageOffsetVector.end(), defaultCalibVcVoltageOffset);
    fill(selectedGateVoltageVector.begin(), selectedGateVoltageVector.end(), defaultGateVoltage);
    fill(selectedSourceVoltageVector.begin(), selectedSourceVoltageVector.end(), defaultSourceVoltage);

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

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
    // undefined

    /*! Voltage range CC */
    // undefined

    /*! Current filter VC */
    boolConfig.initialWord = 2+9; //updated
    boolConfig.initialBit = 0;//4;
    boolConfig.bitsNum = 4;
    vcCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentFilterCoder);

    /*! Voltage filter VC */
    // undefined

    /*! Current filter CC */
    // undefined

    /*! Voltage filter CC */
    // undefined

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

    /*! Protocol structure */
    boolConfig.initialWord = protocolWordOffset;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolIdCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolIdCoder);

    boolConfig.initialWord = protocolWordOffset+1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolItemsNumberCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolItemsNumberCoder);

    boolConfig.initialWord = protocolWordOffset+2;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolSweepsNumberCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolSweepsNumberCoder);

    doubleConfig.initialWord = protocolWordOffset+3;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    voltageProtocolRestCoders.resize(VCVoltageRangesNum);

    for (unsigned int rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        doubleConfig.resolution = vHoldRange[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*32768.0;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;
        voltageProtocolRestCoders[rangeIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(voltageProtocolRestCoders[rangeIdx]);
    }

    /*! Protocol items */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    voltageProtocolStim0Coders.resize(VCVoltageRangesNum);
    voltageProtocolStim0StepCoders.resize(VCVoltageRangesNum);
    voltageProtocolStim1Coders.resize(VCVoltageRangesNum);
    voltageProtocolStim1StepCoders.resize(VCVoltageRangesNum);

    for (unsigned int rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        voltageProtocolStim0Coders[rangeIdx].resize(protocolMaxItemsNum);
        voltageProtocolStim0StepCoders[rangeIdx].resize(protocolMaxItemsNum);
        voltageProtocolStim1Coders[rangeIdx].resize(protocolMaxItemsNum);
        voltageProtocolStim1StepCoders[rangeIdx].resize(protocolMaxItemsNum);

        doubleConfig.resolution = vHoldRange[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*32768.0;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;

        for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
            doubleConfig.initialWord = protocolWordOffset+4+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim0Coders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim0Coders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+5+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim0StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim0StepCoders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+6+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim1Coders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim1Coders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+7+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim1StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim1StepCoders[rangeIdx][itemIdx]);
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = positiveProtocolTimeRange.step;
    doubleConfig.minValue = positiveProtocolTimeRange.min;
    doubleConfig.maxValue = positiveProtocolTimeRange.max;
    protocolTime0Coders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+8+protocolItemsWordsNum*itemIdx;
        protocolTime0Coders[itemIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(protocolTime0Coders[itemIdx]);
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = protocolTimeRange.step;
    doubleConfig.minValue = protocolTimeRange.min;
    doubleConfig.maxValue = protocolTimeRange.max;
    protocolTime0StepCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+10+protocolItemsWordsNum*itemIdx;
        protocolTime0StepCoders[itemIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(protocolTime0StepCoders[itemIdx]);
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = positiveProtocolFrequencyRange.step;
    doubleConfig.minValue = positiveProtocolFrequencyRange.min;
    doubleConfig.maxValue = positiveProtocolFrequencyRange.max;
    protocolFrequency0Coders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+8+protocolItemsWordsNum*itemIdx;
        protocolFrequency0Coders[itemIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(protocolFrequency0Coders[itemIdx]);
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = protocolFrequencyRange.step;
    doubleConfig.minValue = protocolFrequencyRange.min;
    doubleConfig.maxValue = protocolFrequencyRange.max;
    protocolFrequency0StepCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+10+protocolItemsWordsNum*itemIdx;
        protocolFrequency0StepCoders[itemIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(protocolFrequency0StepCoders[itemIdx]);
    }

    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolItemIdxCoders.resize(protocolMaxItemsNum);
    protocolNextItemIdxCoders.resize(protocolMaxItemsNum);
    protocolLoopRepetitionsCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+12+protocolItemsWordsNum*itemIdx;
        protocolItemIdxCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolItemIdxCoders[itemIdx]);

        boolConfig.initialWord = protocolWordOffset+13+protocolItemsWordsNum*itemIdx;
        protocolNextItemIdxCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolNextItemIdxCoders[itemIdx]);

        boolConfig.initialWord = protocolWordOffset+14+protocolItemsWordsNum*itemIdx;
        protocolLoopRepetitionsCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolLoopRepetitionsCoders[itemIdx]);
    }

    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    protocolApplyStepsCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+15+protocolItemsWordsNum*itemIdx;
        protocolApplyStepsCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolApplyStepsCoders[itemIdx]);
    }

    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 4;
    protocolItemTypeCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+15+protocolItemsWordsNum*itemIdx;
        protocolItemTypeCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolItemTypeCoders[itemIdx]);
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

    /*! gate voltage tuner */
    doubleConfig.initialWord = 795+9; //updated
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = gateVoltageRange.step;
    doubleConfig.minValue = -1024.0*31.0;
    doubleConfig.maxValue = gateVoltageRange.min+gateVoltageRange.step*USHORT_MAX;
    gateVoltageCoders.resize(totalBoardsNum);
    for (uint32_t idx = 0; idx < totalBoardsNum; idx++) {
        gateVoltageCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(gateVoltageCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! source voltage tuner */
    doubleConfig.initialWord = 819+9; //updated
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = sourceVoltageRange.step;
    doubleConfig.minValue = -2048.0;
    doubleConfig.maxValue = sourceVoltageRange.min+sourceVoltageRange.step*USHORT_MAX;
    sourceVoltageCoders.resize(totalBoardsNum);
    for (uint32_t idx = 0; idx < totalBoardsNum; idx++) {
        sourceVoltageCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(sourceVoltageCoders[idx]);
        doubleConfig.initialWord++;
    }

    boolConfig.initialWord = 2;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 2;
    plus24VCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(plus24VCoder);

    boolConfig.initialWord = 2;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 2;
    minus24VCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(minus24VCoder);

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[2] = 0x0070; // fans on by default
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

MessageDispatcher_384NanoPores_V01::~MessageDispatcher_384NanoPores_V01() {

}

void MessageDispatcher_384NanoPores_V01::initializeHW() {
    this->resetFpga(true, true);
    this->resetFpga(false, false);

//    this->resetAsic(true, true);
//    this_thread::sleep_for(chrono::milliseconds(100));
//    this->resetAsic(false, true);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    minus24VCoder->encode(3, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    plus24VCoder->encode(3, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    stackOutgoingMessage(txStatus);
}

//void MessageDispatcher_384NanoPores_V01::updateDeviceStatus(vector <bool> &fsmRunFlag, bool &poreForming, bool &communicationError) {
//    for (int idx = 0; idx < fsmStateChannelsNum; idx++) {
//        fsmRunFlag[idx] = (infoStruct.status & (0x0001 << idx)) == 0 ? false : true;
//    }
//    poreForming = (infoStruct.status & (0x0001 << 8)) == 0 ? false : true;
//    communicationError = (infoStruct.status & (0x0001 << 9)) == 0 ? false : true;
//}

//void MessageDispatcher_384NanoPores_V01::updateVoltageOffsetCompensations(vector <Measurement_t> &offsets) {
//    for (int idx = 0; idx < currentChannelsNum; idx++) {
//        offsets[idx] = voltageOffsetCompensationGain*(double)infoStruct.vComp[idx];
//    }
//}
