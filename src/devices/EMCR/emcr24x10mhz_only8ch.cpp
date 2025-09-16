#include "emcr24x10mhz_only8ch.h"

Emcr24x10MHz_Only8Ch_PCBV01::Emcr24x10MHz_Only8Ch_PCBV01(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "24x10MHz (only 8ch)";

    waitingTimeBeforeReadingData = 2; //s

    rxSyncWord = 0x5aa55aa5;

    packetsPerFrame = 1024;

    voltageChannelsNum = 4;
    currentChannelsNum = 4;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 1;

    rxWordOffsets[RxMessageVoltageDataLoad] = 0;
    rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    rxWordOffsets[RxMessageCurrentBlocksDataLoad] = rxWordOffsets[RxMessageVoltageDataLoad] + rxWordLengths[RxMessageVoltageDataLoad];
    rxWordLengths[RxMessageCurrentBlocksDataLoad] = currentChannelsNum*packetsPerFrame;

    rxCurrentBlockLength = 4;

    rxMaxWords = currentChannelsNum*packetsPerFrame; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 281; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
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

    protocolFrequencyRange.step = protocolFpgaClockFrequencyHz/(256.0*(UINT24_MAX+1.0)); /*! 10.0MHz / 256 / 2^24 */
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
    vcCurrentRangesArray[VCCurrentRange10nA_ch0_3].min = -10.0;
    vcCurrentRangesArray[VCCurrentRange10nA_ch0_3].max = 10.0;
    vcCurrentRangesArray[VCCurrentRange10nA_ch0_3].step = vcCurrentRangesArray[VCCurrentRange10nA_ch0_3].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange10nA_ch0_3].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange10nA_ch0_3].unit = "A";
    vcCurrentRangesArray[VCCurrentRange100nA_ch0_3].min = -100.0;
    vcCurrentRangesArray[VCCurrentRange100nA_ch0_3].max = 100.0;
    vcCurrentRangesArray[VCCurrentRange100nA_ch0_3].step = vcCurrentRangesArray[VCCurrentRange100nA_ch0_3].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange100nA_ch0_3].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange100nA_ch0_3].unit = "A";
    vcCurrentRangesArray[VCCurrentRange10nA_ch4_7].min = -10.0;
    vcCurrentRangesArray[VCCurrentRange10nA_ch4_7].max = 10.0;
    vcCurrentRangesArray[VCCurrentRange10nA_ch4_7].step = vcCurrentRangesArray[VCCurrentRange10nA_ch4_7].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange10nA_ch4_7].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange10nA_ch4_7].unit = "A";
    vcCurrentRangesArray[VCCurrentRange100nA_ch4_7].min = -100.0;
    vcCurrentRangesArray[VCCurrentRange100nA_ch4_7].max = 100.0;
    vcCurrentRangesArray[VCCurrentRange100nA_ch4_7].step = vcCurrentRangesArray[VCCurrentRange100nA_ch4_7].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange100nA_ch4_7].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange100nA_ch4_7].unit = "A";
    defaultVcCurrentRangeIdx = VCCurrentRange100nA_ch0_3;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].min = -500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].max = 500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.0625;
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

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].value = 1.6;
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter16kHz].value = 16.0;
    vcVoltageFiltersArray[VCVoltageFilter16kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter16kHz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter1_6kHz;

    liquidJunctionRangesNum = vcVoltageRangesNum;
    liquidJunctionRangesArray = vcVoltageRangesArray;
    defaultLiquidJunctionRangeIdx = defaultVcVoltageRangeIdx;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    defaultSamplingRateIdx = SamplingRate25MHz; /*! \todo rimettere la sampling rate bassa a regime */

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate781_25kHz].value = 25.0/32.0;
    realSamplingRatesArray[SamplingRate781_25kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate781_25kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate1_5625MHz].value = 25.0/16.0;
    realSamplingRatesArray[SamplingRate1_5625MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate1_5625MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate3_125MHz].value = 25.0/8.0;
    realSamplingRatesArray[SamplingRate3_125MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate3_125MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate6_25MHz].value = 25.0/4.0;
    realSamplingRatesArray[SamplingRate6_25MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate6_25MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate12_5MHz].value = 25.0/2.0;
    realSamplingRatesArray[SamplingRate12_5MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate12_5MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate25MHz].value = 25.0;
    realSamplingRatesArray[SamplingRate25MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate25MHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate781_25kHz] = 0;
    sr2srm[SamplingRate1_5625MHz] = 0;
    sr2srm[SamplingRate3_125MHz] = 0;
    sr2srm[SamplingRate6_25MHz] = 0;
    sr2srm[SamplingRate12_5MHz] = 0;
    sr2srm[SamplingRate25MHz] = 0;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate781_25kHz].value = 32.0/25.0;
    integrationStepArray[SamplingRate781_25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate781_25kHz].unit = "s";
    integrationStepArray[SamplingRate1_5625MHz].value = 16.0/25.0;
    integrationStepArray[SamplingRate1_5625MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_5625MHz].unit = "s";
    integrationStepArray[SamplingRate3_125MHz].value = 8.0/25.0;
    integrationStepArray[SamplingRate3_125MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate3_125MHz].unit = "s";
    integrationStepArray[SamplingRate6_25MHz].value = 4.0/25.0;
    integrationStepArray[SamplingRate6_25MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate6_25MHz].unit = "s";
    integrationStepArray[SamplingRate12_5MHz].value = 2.0/25.0;
    integrationStepArray[SamplingRate12_5MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate12_5MHz].unit = "s";
    integrationStepArray[SamplingRate25MHz].value = 1.0/25.0;
    integrationStepArray[SamplingRate25MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate25MHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate781_25kHz, VCCurrentFilter1MHz},
        {SamplingRate1_5625MHz, VCCurrentFilter1MHz},
        {SamplingRate3_125MHz, VCCurrentFilter10MHz},
        {SamplingRate6_25MHz, VCCurrentFilter10MHz},
        {SamplingRate12_5MHz, VCCurrentFilter10MHz},
        {SamplingRate25MHz, VCCurrentFilter10MHz}
    };

    // mapping ADC Current Clamp
    // undefined

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};

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

    customOptionsNum = CustomOptionsNum;
    customOptionsNames.resize(customOptionsNum);
    customOptionsDescriptions.resize(customOptionsNum);
    customOptionsDefault.resize(customOptionsNum);
    for (int optIdx = 0; optIdx < customOptionsNum; optIdx++) {
        customOptionsNames[optIdx] = "Dac Ch " + std::to_string(optIdx+1);
        customOptionsDescriptions[optIdx].resize(2);
        customOptionsDescriptions[optIdx][0] = "Dac Vcm";
        customOptionsDescriptions[optIdx][1] = "Dac Zap";
        customOptionsDefault[optIdx] = 0;

    }

    customDoublesNum = CustomDoublesNum;
    customDoublesNames.resize(customDoublesNum);
    customDoublesNames[CustomDacVcmAsic1] = "Vcm Asic 1";
    customDoublesNames[CustomDacZapAsic1] = "Zap Asic 1";
    customDoublesNames[CustomDacRefAsic1] = "Ref Asic 1";
    customDoublesNames[CustomDacVcmAsic2] = "Vcm Asic 2";
    customDoublesNames[CustomDacZapAsic2] = "Zap Asic 2";
    customDoublesNames[CustomDacRefAsic2] = "Ref Asic 2";
    customDoublesNames[CustomDacVcmAsic3] = "Vcm Asic 3";
    customDoublesNames[CustomDacZapAsic3] = "Zap Asic 3";
    customDoublesNames[CustomDacRefAsic3] = "Ref Asic 3";
    customDoublesRanges.resize(customDoublesNum);
    RangedMeasurement_t customRange = {-1650.0, -1650.0+65535.0*0.0625, 0.0625, UnitPfxMilli, "V"};
    std::fill(customDoublesRanges.begin(), customDoublesRanges.end(), customRange);
    customRange = {-500.0, 500.0, 0.0625, UnitPfxMilli, "V"};
    customDoublesRanges[0] = customRange;
    customDoublesRanges[3] = customRange;
    customDoublesRanges[6] = customRange;
    customDoublesDefault.resize(customDoublesNum);
    std::fill(customDoublesDefault.begin(), customDoublesDefault.end(), 0.0);

    /*! Default values */
    currentRanges.resize(currentChannelsNum);
    std::fill(currentRanges.begin(), currentRanges.end(), vcCurrentRangesArray[defaultVcCurrentRangeIdx]);
    currentResolutions.resize(currentChannelsNum);
    std::fill(currentResolutions.begin(), currentResolutions.end(), currentRanges[0].step);
    voltageRanges.resize(voltageChannelsNum);
    std::fill(voltageRanges.begin(), voltageRanges.end(), vcVoltageRangesArray[defaultVcVoltageRangeIdx]);
    voltageResolutions.resize(voltageChannelsNum);
    std::fill(voltageResolutions.begin(), voltageResolutions.end(), voltageRanges[0].step);
    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    // Selected default Idx
    selectedVcVoltageRangeIdx = defaultVcVoltageRangeIdx;
    selectedVcCurrentFilterIdx = defaultVcCurrentFilterIdx;
    selectedSamplingRateIdx = defaultSamplingRateIdx;

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
    fpgaResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(fpgaResetCoder);

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 4;
    samplingRateCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(5); /*! 780kHz  0b0101 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(4); /*! 1.5MHz  0b0100 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(3); /*! 3.1MHz  0b0011 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(2); /*! 6.2MHz  0b0010 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(1); /*! 12.5MHz  0b0001 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(0); /*! 25MHz  0b0000 */
    coders.push_back(samplingRateCoder);

    /*! Current range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 2;
    vcCurrentRangeCoders.clear();
    vcCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[0]);

    /*! Voltage range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 1;
    vcVoltageRangeCoders.clear();
    vcVoltageRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcVoltageRangeCoders[0]);

    /*! Current range CC */
    // undefined

    /*! Voltage range CC */
    // undefined

    /*! Current filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 1;
    vcCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentFilterCoder);

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    vcVoltageFilterCoder = new BoolNegatedArrayCoder(boolConfig);
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    // undefined

    /*! Voltage filter CC */
    // undefined

    /*! Liquid junction compensation */
    // not available

    /*! Enable stimulus */
    // not available

    /*! Protocol reset */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 1;
    protocolResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolResetCoder);

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
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
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

        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
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
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step; /*! The voltage is applied on the reference pin, so voltages must be reversed */
        doubleConfig.minValue = -1650.0;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            // doubleConfig.initialWord++; /*! \todo FCON tutti sulla stessa word perchè il DAC è uno */
        }
    }

    /*! liquid junction voltage */
    // not available

    /*! VC current gain calibration */
    doubleConfig.initialWord = 267;
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
        doubleConfig.initialWord = 271;
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
    doubleConfig.initialWord = 275;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibVcVoltageGainRange.step;
    doubleConfig.minValue = calibVcVoltageGainRange.min;
    doubleConfig.maxValue = calibVcVoltageGainRange.max;
    calibVcVoltageGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcVoltageGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(calibVcVoltageGainCoders[idx]);
            // doubleConfig.initialWord++; /*! \todo FCON tutti sulla stessa word perchè il DAC è uno */
    }

    /*! VC voltage offset calibration */
    calibVcVoltageOffsetCoders.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 278;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcVoltageOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcVoltageOffsetRanges[rangeIdx].max;
        doubleConfig.maxValue = calibVcVoltageOffsetRanges[rangeIdx].min;
        calibVcVoltageOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcVoltageOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcVoltageOffsetCoders[rangeIdx][idx]);
            // doubleConfig.initialWord++; /*! \todo FCON tutti sulla stessa word perchè il DAC è uno */
        }
    }

    boolConfig.initialWord = 3;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    customOptionsCoders.resize(customOptionsNum);
    for (int optIdx = 0; optIdx < customOptionsNum; optIdx++) {
        customOptionsCoders[optIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(customOptionsCoders[optIdx]);
        boolConfig.initialBit++;
    }

    doubleConfig.initialWord = 258;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    customDoublesCoders.resize(customDoublesNum);
    for (int idx = 0; idx < customDoublesNum; idx++) {
        doubleConfig.minValue = customDoublesRanges[CustomDacZapAsic1].min;
        doubleConfig.maxValue = customDoublesRanges[CustomDacZapAsic1].max;
        doubleConfig.resolution = customDoublesRanges[CustomDacZapAsic1].step;
        customDoublesCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(customDoublesCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! Default status */
    txStatus.init(txDataWords);
    txStatus.encodingWords[0] = 0x0121; /*! 800kHz default sampling rate */
    txStatus.encodingWords[2] = 0x0001; /*! 1 voltage frame for every current frame */
    txStatus.encodingWords[4] = 0x00FF; /*! Enable all channels by default */
    for (int idx = 258; idx < 267; idx++) {
        txStatus.encodingWords[idx] = 0x6720; /*! Set all DACs at Vcm by default */
    }
    for (int idx = 276; idx < 278; idx++) {
        txStatus.encodingWords[idx] = 0x0400; /*! Set gain 1 for Dac Zap and Dac Ref */
    }
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

ErrorCodes_t Emcr24x10MHz_Only8Ch_PCBV01::initializeHW() {
    this->resetFpga(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    this->resetFpga(false, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    return Success;
}
