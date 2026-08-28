#include "emcr32x10mhz_el05c2_pcbv01.h"

Emcr32x10MHz_EL05c2_PCBV01::Emcr32x10MHz_EL05c2_PCBV01(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "32x10MHz_EL05c2";

    waitingTimeBeforeReadingData = 2; //s

    rxSyncWord = 0x5aa55aa5;

    packetsPerFrame = 8;

    voltageChannelsNum = 32;
    currentChannelsNum = 32;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;
    stimulusBlockSize = 4;

    totalBoardsNum = 4;

    rxWordOffsets[RxMessageVoltageDataLoad] = 0;
    rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    rxWordOffsets[RxMessageCurrentBlocksDataLoad] = rxWordOffsets[RxMessageVoltageDataLoad] + rxWordLengths[RxMessageVoltageDataLoad];
    rxWordLengths[RxMessageCurrentBlocksDataLoad] = currentChannelsNum*packetsPerFrame;

    rxCurrentBlockLength = 8;

    rxMaxWords = currentChannelsNum*packetsPerFrame; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 354; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
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
    voltageProtocolSinImplemented = false;

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
    defaultVcCurrentRangeIdxs.resize(1);
    defaultVcCurrentRangeIdxs[0] = VCCurrentRange100nA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].min = -500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].max = 500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 5000.0/65535.0;
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
    vcCurrentFiltersArray[VCCurrentFilter10MHz].value = 10.0;
    vcCurrentFiltersArray[VCCurrentFilter10MHz].prefix = UnitPfxMega;
    vcCurrentFiltersArray[VCCurrentFilter10MHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter1MHz].value = 1.0;
    vcCurrentFiltersArray[VCCurrentFilter1MHz].prefix = UnitPfxMega;
    vcCurrentFiltersArray[VCCurrentFilter1MHz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter1MHz;

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
    defaultSamplingRateIdx = SamplingRate625kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate625kHz].value = 625.0;
    realSamplingRatesArray[SamplingRate625kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate625kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate1_25MHz].value = 1.25;
    realSamplingRatesArray[SamplingRate1_25MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate1_25MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate2_5MHz].value = 2.5;
    realSamplingRatesArray[SamplingRate2_5MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate2_5MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate5MHz].value = 5.0;
    realSamplingRatesArray[SamplingRate5MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate5MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate10MHz].value = 10.0;
    realSamplingRatesArray[SamplingRate10MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate10MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate20MHz].value = 20.0;
    realSamplingRatesArray[SamplingRate20MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate20MHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate625kHz] = 0;
    sr2srm[SamplingRate1_25MHz] = 0;
    sr2srm[SamplingRate2_5MHz] = 0;
    sr2srm[SamplingRate5MHz] = 0;
    sr2srm[SamplingRate10MHz] = 0;
    sr2srm[SamplingRate20MHz] = 0;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate625kHz].value = 1000.0/625.0;
    integrationStepArray[SamplingRate625kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate625kHz].unit = "s";
    integrationStepArray[SamplingRate1_25MHz].value = 1000.0/1250.0;
    integrationStepArray[SamplingRate1_25MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_25MHz].unit = "s";
    integrationStepArray[SamplingRate2_5MHz].value = 1000.0/2500.0;
    integrationStepArray[SamplingRate2_5MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate2_5MHz].unit = "s";
    integrationStepArray[SamplingRate5MHz].value = 1000.0/5000.0;
    integrationStepArray[SamplingRate5MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5MHz].unit = "s";
    integrationStepArray[SamplingRate10MHz].value = 1000.0/10000.0;
    integrationStepArray[SamplingRate10MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate10MHz].unit = "s";
    integrationStepArray[SamplingRate20MHz].value = 1000.0/20000.0;
    integrationStepArray[SamplingRate20MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate20MHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap =
        {
          {SamplingRate625kHz, VCCurrentFilter1MHz},
          {SamplingRate1_25MHz, VCCurrentFilter1MHz},
          {SamplingRate2_5MHz, VCCurrentFilter1MHz},
          {SamplingRate5MHz, VCCurrentFilter10MHz},
          {SamplingRate10MHz, VCCurrentFilter10MHz},
          {SamplingRate20MHz, VCCurrentFilter10MHz},
          };

    // mapping ADC Current Clamp
    // undefined

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
    currentRanges.resize(currentChannelsNum);
    std::fill(currentRanges.begin(), currentRanges.end(), vcCurrentRangesArray[defaultVcCurrentRangeIdxs[0]]);
    currentResolutions.resize(currentChannelsNum);
    std::fill(currentResolutions.begin(), currentResolutions.end(), currentRanges[0].step);
    voltageRanges.resize(voltageChannelsNum);
    std::fill(voltageRanges.begin(), voltageRanges.end(), vcVoltageRangesArray[defaultVcVoltageRangeIdx]);
    voltageResolutions.resize(voltageChannelsNum);
    std::fill(voltageResolutions.begin(), voltageResolutions.end(), voltageRanges[0].step);
    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    /*! FPGA reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    fpgaResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(asicResetCoder);

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 4;
    samplingRateCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(5); /*! 625kHz  0b0101 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(4); /*! 1.25MHz  0b0100 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(3); /*! 2.5MHz  0b0011 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(2); /*! 5MHz  0b0010 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(1); /*! 10MHz  0b0001 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(0); /*! 20MHz  0b0000 */
    coders.push_back(samplingRateCoder);

    /*! Current range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    vcCurrentRangeCoders.clear();
    vcCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[0]);

    /*! Voltage range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 1;
    vcVoltageRangeCoders.clear();
    vcVoltageRangeCoders.push_back(new BoolNegatedArrayCoder(boolConfig));
    coders.push_back(vcVoltageRangeCoders[0]);

    /*! Current range CC */

    /*! Voltage range CC */

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
    vcVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */

    /*! Voltage filter CC */

    /*! Protocol reset */
    boolConfig.initialWord = 2;
    boolConfig.initialBit = 0;
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
    uint16_t stimulusBlockIdx = stimulusBlockSize;
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 258;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            if (--stimulusBlockIdx == 0) {
                doubleConfig.initialWord++;
                stimulusBlockIdx = stimulusBlockSize;
            }
        }
    }

    customFlagsNum = CustomFlagsNum;
    customFlagsNames.resize(customFlagsNum);
    customFlagsDefault.resize(customFlagsNum);
    customFlagsCoders.resize(customFlagsNum);
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    for (uint16_t i = 0; i < CustomFlagsNum; i++) {
        customFlagsNames[i] = "Zap " + std::to_string(i);
        customFlagsDefault[i] = false;
        customFlagsCoders[i] = new BoolArrayCoder(boolConfig);
        boolConfig.initialBit += 2;
        if (boolConfig.initialBit >= 16) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! DAC gain e offset */
    /*! VC Voltage gain */
    stimulusBlockIdx = stimulusBlockSize;
    doubleConfig.initialWord = 338;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 10;
    doubleConfig.resolution = calibVcVoltageGainRange.step;
    doubleConfig.minValue = calibVcVoltageGainRange.min;
    doubleConfig.maxValue = calibVcVoltageGainRange.max;
    calibVcVoltageGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcVoltageGainCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(calibVcVoltageGainCoders[idx]);
        if (--stimulusBlockIdx == 0) {
            doubleConfig.initialWord++;
            stimulusBlockIdx = stimulusBlockSize;
        }
    }

    /*! VC Voltage offset */
    calibVcVoltageOffsetCoders.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        stimulusBlockIdx = stimulusBlockSize;
        doubleConfig.initialWord = 346;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcVoltageOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcVoltageOffsetRanges[rangeIdx].min;
        doubleConfig.maxValue = calibVcVoltageOffsetRanges[rangeIdx].max;
        calibVcVoltageOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcVoltageOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcVoltageOffsetCoders[rangeIdx][idx]);
            if (--stimulusBlockIdx == 0) {
                doubleConfig.initialWord++;
                stimulusBlockIdx = stimulusBlockSize;
            }
        }
    }

    /*! ADC gain e offset */
    /*! VC current gain */
    doubleConfig.initialWord = 274;
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

    /*! VC current offset */
    calibVcCurrentOffsetCoders.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 306;
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

    /*! Default status */
    txStatus.init(txDataWords);
    txStatus.encodingWords[12] = 0xFFFF;
    txStatus.encodingWords[13] = 0xFFFF;
    for (uint16_t i = 274; i < 306; i++) {
        txStatus.encodingWords[i] = 0x0400;
    }
    for (uint16_t i = 338; i < 346; i++) {
        txStatus.encodingWords[i] = 0x0400;
    }
}

ErrorCodes_t Emcr32x10MHz_EL05c2_PCBV01::initializeHW() {
    fpgaResetCoder->encode(1, txStatus);
    this->stackOutgoingMessage(txStatus);
    fpgaResetCoder->encode(0, txStatus);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // ADC autocalibration
    this->setDebugBit(0, 15, true, true);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->setDebugWord(7, 0xFFFF);
    this->setDebugWord(8, 0xFFFF);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    this->setDebugWord(7, 0x0000);
    this->setDebugWord(8, 0x0000);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    this->setDebugBit(0, 15, false, true);

    return Success;
}
