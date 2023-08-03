#include "messagedispatcher_2x10mhz.h"

MessageDispatcher_2x10MHz_V01::MessageDispatcher_2x10MHz_V01(std::string di) :
    MessageDispatcher_OpalKelly(di) {

    deviceName = "2x10MHz";

    fwName = "2x10MHz_V03.bit";

    waitingTimeBeforeReadingData = 2; //s

    rxSyncWord = 0x5aa5;

#ifdef REALLY_4_CHANNELS
    packetsPerFrame = 256;

    voltageChannelsNum = 4;
    currentChannelsNum = 4;
#else
    packetsPerFrame = 512;

    voltageChannelsNum = 2;
    currentChannelsNum = 2;
#endif
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 1;

    rxWordOffsets[RxMessageVoltageDataLoad] = 0;
    rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    rxWordOffsets[RxMessageCurrentDataLoad] = 4;
    rxWordLengths[RxMessageCurrentDataLoad] = currentChannelsNum*packetsPerFrame;

    rxWordOffsets[RxMessageDataHeader] = rxWordOffsets[RxMessageDataLoad] + rxWordLengths[RxMessageDataLoad];
    rxWordLengths[RxMessageDataHeader] = 4;

    rxWordOffsets[RxMessageDataTail] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageDataTail] = 1;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataTail] + rxWordLengths[RxMessageDataTail];
    rxWordLengths[RxMessageStatus] = 1;

    maxOutputPacketsNum = E384CL_DATA_ARRAY_SIZE/totalChannelsNum;

    rxMaxWords = currentChannelsNum; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE*packetsPerFrame;

    txDataWords = 360; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
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
    protocolFpgaClockFrequencyHz = 10.08e6;

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
    voltageProtocolSinImplemented = true;

    protocolMaxItemsNum = 20;
    protocolWordOffset = 14;
    protocolItemsWordsNum = 12;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange100nA].min = -100.0;
    vcCurrentRangesArray[VCCurrentRange100nA].max = 100.0;
    vcCurrentRangesArray[VCCurrentRange100nA].step = vcCurrentRangesArray[VCCurrentRange100nA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange100nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange100nA].unit = "A";
    defaultVcCurrentRangeIdx = VCCurrentRange100nA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange1000mV].min = -1000.0;
    vcVoltageRangesArray[VCVoltageRange1000mV].max = 1000.0;
    vcVoltageRangesArray[VCVoltageRange1000mV].step = 0.0625;
    vcVoltageRangesArray[VCVoltageRange1000mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange1000mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange1000mV;

    liquidJunctionRangesNum = vcVoltageRangesNum;
    liquidJunctionRangesArray = vcVoltageRangesArray;
    defaultLiquidJunctionRangeIdx = defaultVcVoltageRangeIdx;

    /*! Current ranges */
    /*! CC */
    ccCurrentRangesNum = CCCurrentRangesNum;

    /*! Voltage ranges */
    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter10kHz].value = 10.0;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter1_5kHz].value = 1.5;
    vcVoltageFiltersArray[VCVoltageFilter1_5kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1_5kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter30Hz].value = 30.0;
    vcVoltageFiltersArray[VCVoltageFilter30Hz].prefix = UnitPfxNone;
    vcVoltageFiltersArray[VCVoltageFilter30Hz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter10kHz;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate26_7MHz].value = 80.0/6.0/0.5;
    samplingRatesArray[SamplingRate26_7MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate26_7MHz].unit = "Hz";
    samplingRatesArray[SamplingRate13_3MHz].value = 80.0/6.0/1.0;
    samplingRatesArray[SamplingRate13_3MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate13_3MHz].unit = "Hz";
    samplingRatesArray[SamplingRate6_67MHz].value = 80.0/6.0/2.0;
    samplingRatesArray[SamplingRate6_67MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate6_67MHz].unit = "Hz";
    samplingRatesArray[SamplingRate3_33MHz].value = 80.0/6.0/4.0;
    samplingRatesArray[SamplingRate3_33MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate3_33MHz].unit = "Hz";
    samplingRatesArray[SamplingRate1_67MHz].value = 80.0/6.0/8.0;
    samplingRatesArray[SamplingRate1_67MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate1_67MHz].unit = "Hz";
    samplingRatesArray[SamplingRate833kHz].value = 80.0/6.0/16.0;
    samplingRatesArray[SamplingRate833kHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate833kHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate26_7MHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate26_7MHz].value = 80.0/6.0/0.5;
    realSamplingRatesArray[SamplingRate26_7MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate26_7MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate13_3MHz].value = 80.0/6.0/1.0;
    realSamplingRatesArray[SamplingRate13_3MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate13_3MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate6_67MHz].value = 80.0/6.0/2.0;
    realSamplingRatesArray[SamplingRate6_67MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate6_67MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate3_33MHz].value = 80.0/6.0/4.0;
    realSamplingRatesArray[SamplingRate3_33MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate3_33MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate1_67MHz].value = 80.0/6.0/8.0;
    realSamplingRatesArray[SamplingRate1_67MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate1_67MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate833kHz].value = 80.0/6.0/16.0;
    realSamplingRatesArray[SamplingRate833kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate833kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate26_7MHz].value = 0.5*6.0/80.0;
    integrationStepArray[SamplingRate26_7MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate26_7MHz].unit = "s";
    integrationStepArray[SamplingRate13_3MHz].value = 1.0*6.0/80.0;
    integrationStepArray[SamplingRate13_3MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate13_3MHz].unit = "s";
    integrationStepArray[SamplingRate6_67MHz].value = 2.0*6.0/80.0;
    integrationStepArray[SamplingRate6_67MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate6_67MHz].unit = "s";
    integrationStepArray[SamplingRate3_33MHz].value = 4.0*6.0/80.0;
    integrationStepArray[SamplingRate3_33MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate3_33MHz].unit = "s";
    integrationStepArray[SamplingRate1_67MHz].value = 8.0*6.0/80.0;
    integrationStepArray[SamplingRate1_67MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_67MHz].unit = "s";
    integrationStepArray[SamplingRate833kHz].value = 16.0*6.0/80.0;
    integrationStepArray[SamplingRate833kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate833kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate26_7MHz, -1},
        {SamplingRate13_3MHz, -1},
        {SamplingRate6_67MHz, -1},
        {SamplingRate3_33MHz, -1},
        {SamplingRate1_67MHz, -1},
        {SamplingRate833kHz, -1}
    };

    // mapping ADC Current Clamp
    // undefined

    calibrationData.samplingRateIdx = SamplingRate833kHz;

    /*! VC calibration voltage steps*/
    calibrationData.vcCalibStepsArrays.resize(VCCurrentRangesNum);
    calibrationData.vcCalibStepsArrays[VCCurrentRange100nA].resize(5);

    calibrationData.vcCalibStepsArrays[VCCurrentRange100nA][0] = {-500.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange100nA][1] = {-250.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange100nA][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange100nA][3] = {250.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange100nA][4] = {500.0, UnitPfxMilli, "V"};

    /*! VC calibration resistances*/
    calibrationData.vcCalibResArray.resize(VCCurrentRangesNum);
    calibrationData.vcCalibResArray[VCCurrentRange100nA] = {10.0, UnitPfxMega, "Ohm"};

    // mapping VC current range - calibration resistances
    calibrationData.vcCurrRange2CalibResMap = {
        {VCCurrentRange100nA, CalibRes10_0MOhm}
    };

    calibrationData.areCalibResistOnBoard = false;
    calibrationData.canInputsBeOpened = false;
    calibrationData.adcCalibratedInOffsetBinary = false;

    vHoldRange.resize(VCVoltageRangesNum);
    vHoldRange[VCVoltageRange1000mV].min = -1000.0;
    vHoldRange[VCVoltageRange1000mV].max = 1000.0;
    vHoldRange[VCVoltageRange1000mV].step = 0.0625;
    vHoldRange[VCVoltageRange1000mV].prefix = UnitPfxMilli;
    vHoldRange[VCVoltageRange1000mV].unit = "V";
    selectedVoltageHoldVector.resize(currentChannelsNum);
    Measurement_t defaultVoltageHoldTuner = {0.0, vHoldRange[VCVoltageRange1000mV].prefix, vHoldRange[VCVoltageRange1000mV].unit};

    /*! Calib VC current gain */
    calibVcCurrentGainRange.step = 1.0/1024.0;
    calibVcCurrentGainRange.min = 0;//SHORT_MIN * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.max = SHORT_MAX * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.prefix = UnitPfxNone;
    calibVcCurrentGainRange.unit = "";

    /*! Calib VC current offset */
    calibVcCurrentOffsetRanges = vcCurrentRangesArray;

    /*! Calib VC voltage gain */
    calibVcVoltageGainRange.step = 1.0/1024.0;
    calibVcVoltageGainRange.min = 0;//SHORT_MIN * calibVcVoltageGainRange.step;
    calibVcVoltageGainRange.max = SHORT_MAX * calibVcVoltageGainRange.step;
    calibVcVoltageGainRange.prefix = UnitPfxNone;
    calibVcVoltageGainRange.unit = "";

    /*! Calib VC voltage offset */
    calibVcVoltageOffsetRanges = vcVoltageRangesArray;

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

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    /*! Clock reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    dcmResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(dcmResetCoder);

    /*! FPGA reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    fpgaResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(fpgaResetCoder);

    /*! Write ADC SPI */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    writeAdcSpiCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(writeAdcSpiCoder);

    /*! Write DAC SPI */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    writeDacSpiCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(writeDacSpiCoder);

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

    /*! Current range VC */
    // undefined

    /*! Voltage range VC */
    // undefined

    /*! Current range CC */
    // undefined

    /*! Voltage range CC */
    // undefined

    /*! Current filter VC */
    // undefined

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x0); // 10kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x5); // 1.5kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0xA); // 30Hz on all channels
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    // undefined

    /*! Voltage filter CC */
    // undefined

    /*! Digital offset compensation */
//    boolConfig.initialWord = 12;
//    boolConfig.initialBit = 0;
//    boolConfig.bitsNum = 1;
//    digitalOffsetCompensationCoders.resize(currentChannelsNum);
//    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
//        digitalOffsetCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
//        coders.push_back(digitalOffsetCompensationCoders[idx]);
//        boolConfig.initialBit++;
//        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
//            boolConfig.initialBit = 0;
//            boolConfig.initialWord++;
//        }
//    }

    /*! Enable stimulus */
    boolConfig.initialWord = 13;
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
    doubleConfig.bitsNum = 32;
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
    doubleConfig.bitsNum = 32;
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
    doubleConfig.bitsNum = 32;
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
    doubleConfig.bitsNum = 32;
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
        doubleConfig.initialWord = 258;
        doubleConfig.resolution = vHoldRange[rangeIdx].step; /*! The voltage is applied on the reference pin, so voltages must be reversed */
        doubleConfig.minValue = -doubleConfig.resolution*40000.0; /*! The working point is 2.5V */
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! VC current gain calibration */
    doubleConfig.initialWord = 344;
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

    /*! VC current offset calibration */
    calibVcCurrentOffsetCoders.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 348;
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

    /*! VC voltage gain calibration */
    doubleConfig.initialWord = 352;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibVcVoltageGainRange.step;
    doubleConfig.minValue = calibVcVoltageGainRange.min;
    doubleConfig.maxValue = calibVcVoltageGainRange.max;
    calibVcVoltageGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcVoltageGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(calibVcVoltageGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! VC voltage offset calibration */
    calibVcVoltageOffsetCoders.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 356;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcVoltageOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcVoltageOffsetRanges[rangeIdx].min;
        doubleConfig.maxValue = calibVcVoltageOffsetRanges[rangeIdx].max;
        calibVcVoltageOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcVoltageOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcVoltageOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[0] = 0x0003; /*! FPGA and DCM in reset by default */
    txStatus[2] = 0x0001; /*! one voltage frame every current frame */
    txStatus[13] = 0x00F0; /*! the 4 channels FW wants the voltage range x1 set */
    txStatus[344] = 0x0400; /*! current gain 1 */
    txStatus[345] = 0x0400; /*! current gain 1 */
    txStatus[352] = 0x0400; /*! voltage gain 1 */
    txStatus[353] = 0x0400; /*! voltage gain 1 */
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

MessageDispatcher_2x10MHz_V01::~MessageDispatcher_2x10MHz_V01() {

}

void MessageDispatcher_2x10MHz_V01::initializeHW() {
    /*! Reset DCM to start 10MHz clock */
    dcmResetCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    dcmResetCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    /*! After a short while the 10MHz clock starts */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    /*! Reset FPGA to initialize state machines */
    fpgaResetCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    fpgaResetCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    /*! Wait for the initialization */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    writeAdcSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    writeDacSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    writeAdcSpiCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);
}
