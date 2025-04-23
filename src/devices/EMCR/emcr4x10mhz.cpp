#include "emcr4x10mhz.h"

Emcr4x10MHz_PCBV01_V02::Emcr4x10MHz_PCBV01_V02(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "4x10MHz";

    fwName = "4x10MHz_V03.bit";

    waitingTimeBeforeReadingData = 2; //s

    rxSyncWord = 0x5aa5;

    packetsPerFrame = 256;

    voltageChannelsNum = 4;
    currentChannelsNum = 4;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 1;

    rxWordOffsets[RxMessageVoltageDataLoad] = 0;
    rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    rxWordOffsets[RxMessageCurrentDataLoad] = rxWordOffsets[RxMessageVoltageDataLoad] + rxWordLengths[RxMessageVoltageDataLoad];
    rxWordLengths[RxMessageCurrentDataLoad] = currentChannelsNum*packetsPerFrame;

    rxWordOffsets[RxMessageDataHeader] = rxWordOffsets[RxMessageCurrentDataLoad] + rxWordLengths[RxMessageCurrentDataLoad];
    rxWordLengths[RxMessageDataHeader] = 4;

    rxWordOffsets[RxMessageDataTail] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageDataTail] = 1;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataTail] + rxWordLengths[RxMessageDataTail];
    rxWordLengths[RxMessageStatus] = 1;

    rxMaxWords = currentChannelsNum; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE*packetsPerFrame;

    txDataWords = 440; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
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

    stateMaxNum = 20;
    stateWordOffset = 280;
    stateWordsNum = 8;

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
    vcVoltageRangesArray[VCVoltageRange20000mV].min = -20000.0;
    vcVoltageRangesArray[VCVoltageRange20000mV].max = 20000.0;
    vcVoltageRangesArray[VCVoltageRange20000mV].step = 1.28125;
    vcVoltageRangesArray[VCVoltageRange20000mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange20000mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange1000mV;

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
    vcVoltageFiltersArray[VCVoltageFilter3Hz].value = 3.0;
    vcVoltageFiltersArray[VCVoltageFilter3Hz].prefix = UnitPfxNone;
    vcVoltageFiltersArray[VCVoltageFilter3Hz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter10kHz;

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
    sr2srm.clear();
    sr2srm[SamplingRate26_7MHz] = 0;
    sr2srm[SamplingRate13_3MHz] = 0;
    sr2srm[SamplingRate6_67MHz] = 0;
    sr2srm[SamplingRate3_33MHz] = 0;
    sr2srm[SamplingRate1_67MHz] = 0;
    sr2srm[SamplingRate833kHz] = 0;

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

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange1000mV].prefix, vcVoltageRangesArray[VCVoltageRange1000mV].unit};

    /*! Calib VC current gain */
    calibVcCurrentGainRange.step = 1.0/1024.0;
    calibVcCurrentGainRange.min = 0;
    calibVcCurrentGainRange.max = SHORT_MAX * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.prefix = UnitPfxNone;
    calibVcCurrentGainRange.unit = "";

    /*! Calib VC current offset */
    calibVcCurrentOffsetRanges = vcCurrentRangesArray;

    /*! Calib VC voltage gain */
    calibVcVoltageGainRange.step = 1.0/1024.0;
    calibVcVoltageGainRange.min = 0;
    calibVcVoltageGainRange.max = SHORT_MAX * calibVcVoltageGainRange.step;
    calibVcVoltageGainRange.prefix = UnitPfxNone;
    calibVcVoltageGainRange.unit = "";

    /*! Calib VC voltage offset */
    calibVcVoltageOffsetRanges = vcVoltageRangesArray;

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
    selectedVcCurrentRangeIdx.resize(currentChannelsNum);
    std::fill(selectedVcCurrentRangeIdx.begin(), selectedVcCurrentRangeIdx.end(), defaultVcCurrentRangeIdx);
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

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 4;
    samplingRateCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(samplingRateCoder);

    /*! Current range VC */
    // undefined

    /*! Voltage range VC */
    boolConfig.initialWord = 13;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageRangeCoders.clear();
    vcVoltageRangeCoders.push_back(new BoolRandomArrayCoder(boolConfig));
    static_cast <BoolRandomArrayCoder *> (vcVoltageRangeCoders[0])->addMapItem(0xF); // x1 on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageRangeCoders[0])->addMapItem(0x0); // x20 on all channels
    coders.push_back(vcVoltageRangeCoders[0]);

    /*! Current range CC */
    // undefined

    /*! Voltage range CC */
    // undefined

    /*! Current filter VC */
    // undefined

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 8;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x00); // 10kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x55); // 1.5kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0xAA); // 3Hz on all channels
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    // undefined

    /*! Voltage filter CC */
    // undefined

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
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.maxValue = -doubleConfig.resolution*32768.0;
        doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;
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
        doubleConfig.maxValue = -doubleConfig.resolution*32768.0;
        doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;

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
        doubleConfig.resolution = -vcVoltageRangesArray[rangeIdx].step; /*! The voltage is applied on the reference pin, so voltages must be reversed */
        doubleConfig.maxValue = -doubleConfig.resolution*40000.0; /*! The working point is 2.5V */
        doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! VC current gain calibration */
    doubleConfig.initialWord = 262;
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
        doubleConfig.initialWord = 266;
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
    doubleConfig.initialWord = 270;
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
        doubleConfig.initialWord = 274;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcVoltageOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcVoltageOffsetRanges[rangeIdx].max;
        doubleConfig.maxValue = calibVcVoltageOffsetRanges[rangeIdx].min;
        calibVcVoltageOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcVoltageOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcVoltageOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }

    boolConfig.initialWord = 278;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    numberOfStatesCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(numberOfStatesCoder);

    boolConfig.initialWord = 279;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    initialStateCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(initialStateCoder);

    boolConfig.initialWord = 3;
    boolConfig.bitsNum = 1;

    enableStateArrayChannelsCoder.resize(currentChannelsNum);
    for (int chNum = 0; chNum < currentChannelsNum; chNum++) {
        boolConfig.initialBit = chNum;
        enableStateArrayChannelsCoder[chNum] = new BoolArrayCoder(boolConfig);
        coders.push_back(enableStateArrayChannelsCoder[chNum]);
    }

    stateAppliedVoltageCoders.resize(VCVoltageRangesNum);
    for (int rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        stateAppliedVoltageCoders[rangeIdx].resize(stateMaxNum);
    }

    stateTimeoutFlagCoders.resize(stateMaxNum);
    stateTriggerFlagCoders.resize(stateMaxNum);
    stateTriggerDeltaFlagCoders.resize(stateMaxNum);

    stateTimeoutValueCoders.resize(stateMaxNum);
    stateTimeoutNextStateCoders.resize(stateMaxNum);
    stateMinTriggerCurrentCoders.resize(VCCurrentRangesNum);
    stateMaxTriggerCurrentCoders.resize(VCCurrentRangesNum);
    for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        stateMinTriggerCurrentCoders[rangeIdx].resize(stateMaxNum);
        stateMaxTriggerCurrentCoders[rangeIdx].resize(stateMaxNum);
    }

    stateTriggerNextStateCoders.resize(stateMaxNum);

    for (int stateIdx = 0; stateIdx < stateMaxNum; stateIdx++) {
        doubleConfig.initialWord = stateWordOffset;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        for (int rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
            doubleConfig.resolution = -vcVoltageRangesArray[rangeIdx].step; /*! The voltage is applied on the reference pin, so voltages must be reversed */
            doubleConfig.maxValue = -doubleConfig.resolution*40000.0; /*! The working point is 2.5V */
            doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;
            stateAppliedVoltageCoders[rangeIdx][stateIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(stateAppliedVoltageCoders[rangeIdx][stateIdx]);
        }

        boolConfig.initialWord = stateWordOffset + 1;
        boolConfig.bitsNum = 1;
        boolConfig.initialBit = 0;
        stateTimeoutFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTimeoutFlagCoders[stateIdx]);

        boolConfig.initialBit = 1;
        stateTriggerFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerFlagCoders[stateIdx]);

        boolConfig.initialBit = 2;
        stateTriggerDeltaFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerDeltaFlagCoders[stateIdx]);

        doubleConfig.initialWord = stateWordOffset+2;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 32;
        doubleConfig.resolution = 1.0/protocolFpgaClockFrequencyHz;
        doubleConfig.minValue = 0;
        doubleConfig.maxValue = UINT32_MAX*doubleConfig.resolution;
        stateTimeoutValueCoders[stateIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(stateTimeoutValueCoders[stateIdx]);

        boolConfig.initialWord = stateWordOffset+4;
        boolConfig.initialBit = 0;
        boolConfig.bitsNum = 16;
        stateTimeoutNextStateCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTimeoutNextStateCoders[stateIdx]);

        doubleConfig.initialWord = stateWordOffset+5;
        doubleConfig.initialBit = 0;
        for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
            doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
            doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
            doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
            stateMinTriggerCurrentCoders[rangeIdx][stateIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(stateMinTriggerCurrentCoders[rangeIdx][stateIdx]);
        }

        doubleConfig.initialWord = stateWordOffset+6;
        doubleConfig.initialBit = 0;
        for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
            doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
            doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
            doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
            stateMaxTriggerCurrentCoders[rangeIdx][stateIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(stateMaxTriggerCurrentCoders[rangeIdx][stateIdx]);
        }

        boolConfig.initialWord = stateWordOffset+7;
        boolConfig.initialBit = 0;
        boolConfig.bitsNum = 16;
        stateTriggerNextStateCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerNextStateCoders[stateIdx]);

        stateWordOffset = stateWordOffset + stateWordsNum;
    }

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[0] = 0x0003; /*! FPGA and DCM in reset by default */
    txStatus[2] = 0x0001; /*! one voltage frame every current frame */
    txStatus[13] = 0x00F0; /*! disable the x20 amplification on startup */
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

ErrorCodes_t Emcr4x10MHz_PCBV01_V02::initializeHW() {
    /*! Reset DCM to start 10MHz clock */
    dcmResetCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    dcmResetCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    /*! After a short while the 10MHz clock starts */
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    this->resetFpga(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    this->resetFpga(false, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    writeAdcSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    writeDacSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    writeAdcSpiCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    return Success;
}

Emcr4x10MHz_PCBV01_V03::Emcr4x10MHz_PCBV01_V03(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "4x10MHz";

    fwName = "4x10MHz_V04.bit";

    waitingTimeBeforeReadingData = 2; //s

    rxSyncWord = 0x5aa5;

    packetsPerFrame = 256;

    voltageChannelsNum = 4;
    currentChannelsNum = 4;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 1;

    rxWordOffsets[RxMessageVoltageDataLoad] = 0;
    rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    rxWordOffsets[RxMessageCurrentDataLoad] = rxWordOffsets[RxMessageVoltageDataLoad] + rxWordLengths[RxMessageVoltageDataLoad];
    rxWordLengths[RxMessageCurrentDataLoad] = currentChannelsNum*packetsPerFrame;

    rxWordOffsets[RxMessageDataHeader] = rxWordOffsets[RxMessageCurrentDataLoad] + rxWordLengths[RxMessageCurrentDataLoad];
    rxWordLengths[RxMessageDataHeader] = 4;

    rxWordOffsets[RxMessageDataTail] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageDataTail] = 1;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataTail] + rxWordLengths[RxMessageDataTail];
    rxWordLengths[RxMessageStatus] = 1;

    rxMaxWords = currentChannelsNum; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE*packetsPerFrame;

    txDataWords = 440; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
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

    stateMaxNum = 20;
    stateWordOffset = 280;
    stateWordsNum = 8;

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
    vcVoltageRangesArray[VCVoltageRange20000mV].min = -20000.0;
    vcVoltageRangesArray[VCVoltageRange20000mV].max = 20000.0;
    vcVoltageRangesArray[VCVoltageRange20000mV].step = 1.28125;
    vcVoltageRangesArray[VCVoltageRange20000mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange20000mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange1000mV;

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
    vcVoltageFiltersArray[VCVoltageFilter3Hz].value = 3.0;
    vcVoltageFiltersArray[VCVoltageFilter3Hz].prefix = UnitPfxNone;
    vcVoltageFiltersArray[VCVoltageFilter3Hz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter10kHz;

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
    sr2srm.clear();
    sr2srm[SamplingRate26_7MHz] = 0;
    sr2srm[SamplingRate13_3MHz] = 0;
    sr2srm[SamplingRate6_67MHz] = 0;
    sr2srm[SamplingRate3_33MHz] = 0;
    sr2srm[SamplingRate1_67MHz] = 0;
    sr2srm[SamplingRate833kHz] = 0;

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

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange1000mV].prefix, vcVoltageRangesArray[VCVoltageRange1000mV].unit};

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
    selectedVcCurrentRangeIdx.resize(currentChannelsNum);
    std::fill(selectedVcCurrentRangeIdx.begin(), selectedVcCurrentRangeIdx.end(), defaultVcCurrentRangeIdx);
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

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 4;
    samplingRateCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(samplingRateCoder);

    /*! Current range VC */
    // undefined

    /*! Voltage range VC */
    boolConfig.initialWord = 13;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageRangeCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageRangeCoder)->addMapItem(0xF); // x1 on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageRangeCoder)->addMapItem(0x0); // x20 on all channels
    coders.push_back(vcVoltageRangeCoder);

    /*! Current range CC */
    // undefined

    /*! Voltage range CC */
    // undefined

    /*! Current filter VC */
    // undefined

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 8;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x00); // 10kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x55); // 1.5kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0xAA); // 3Hz on all channels
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    // undefined

    /*! Voltage filter CC */
    // undefined

    /*! Liquid junction compensation */
    boolConfig.initialWord = 13;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    liquidJunctionCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        liquidJunctionCompensationCoders[idx] = new BoolNegatedArrayCoder(boolConfig);
        coders.push_back(liquidJunctionCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

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
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.maxValue = -doubleConfig.resolution*32768.0;
        doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;
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
        doubleConfig.maxValue = -doubleConfig.resolution*32768.0;
        doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;

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
        doubleConfig.resolution = -vcVoltageRangesArray[rangeIdx].step; /*! The voltage is applied on the reference pin, so voltages must be reversed */
        doubleConfig.maxValue = -doubleConfig.resolution*40000.0; /*! The working point is 2.5V */
        doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! liquid junction voltage */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    liquidJunctionVoltageCoders.resize(liquidJunctionRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < liquidJunctionRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 6;
        doubleConfig.resolution = liquidJunctionRangesArray[rangeIdx].step;
        doubleConfig.minValue = liquidJunctionRangesArray[rangeIdx].max;
        doubleConfig.maxValue = liquidJunctionRangesArray[rangeIdx].min;
        liquidJunctionVoltageCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            liquidJunctionVoltageCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(liquidJunctionVoltageCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! VC current gain calibration */
    doubleConfig.initialWord = 262;
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
        doubleConfig.initialWord = 266;
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
    doubleConfig.initialWord = 270;
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
        doubleConfig.initialWord = 274;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcVoltageOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcVoltageOffsetRanges[rangeIdx].max;
        doubleConfig.maxValue = calibVcVoltageOffsetRanges[rangeIdx].min;
        calibVcVoltageOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcVoltageOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcVoltageOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }

    boolConfig.initialWord = 278;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    numberOfStatesCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(numberOfStatesCoder);

    boolConfig.initialWord = 279;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    initialStateCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(initialStateCoder);

    boolConfig.initialWord = 3;
    boolConfig.bitsNum = 1;

    enableStateArrayChannelsCoder.resize(currentChannelsNum);
    for (int chNum = 0; chNum < currentChannelsNum; chNum++) {
        boolConfig.initialBit = chNum;
        enableStateArrayChannelsCoder[chNum] = new BoolArrayCoder(boolConfig);
        coders.push_back(enableStateArrayChannelsCoder[chNum]);
    }

    stateAppliedVoltageCoders.resize(VCVoltageRangesNum);
    for (int rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        stateAppliedVoltageCoders[rangeIdx].resize(stateMaxNum);
    }

    stateTimeoutFlagCoders.resize(stateMaxNum);
    stateTriggerFlagCoders.resize(stateMaxNum);
    stateTriggerDeltaFlagCoders.resize(stateMaxNum);

    stateTimeoutValueCoders.resize(stateMaxNum);
    stateTimeoutNextStateCoders.resize(stateMaxNum);
    stateMinTriggerCurrentCoders.resize(VCCurrentRangesNum);
    stateMaxTriggerCurrentCoders.resize(VCCurrentRangesNum);
    for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        stateMinTriggerCurrentCoders[rangeIdx].resize(stateMaxNum);
        stateMaxTriggerCurrentCoders[rangeIdx].resize(stateMaxNum);
    }

    stateTriggerNextStateCoders.resize(stateMaxNum);

    for (unsigned int stateIdx = 0; stateIdx < stateMaxNum; stateIdx++) {
        doubleConfig.initialWord = stateWordOffset;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        for (int rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
            doubleConfig.resolution = -vcVoltageRangesArray[rangeIdx].step; /*! The voltage is applied on the reference pin, so voltages must be reversed */
            doubleConfig.maxValue = -doubleConfig.resolution*40000.0; /*! The working point is 2.5V */
            doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;
            stateAppliedVoltageCoders[rangeIdx][stateIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(stateAppliedVoltageCoders[rangeIdx][stateIdx]);
        }

        boolConfig.initialWord = stateWordOffset + 1;
        boolConfig.bitsNum = 1;
        boolConfig.initialBit = 0;
        stateTimeoutFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTimeoutFlagCoders[stateIdx]);

        boolConfig.initialBit = 1;
        stateTriggerFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerFlagCoders[stateIdx]);

        boolConfig.initialBit = 2;
        stateTriggerDeltaFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerDeltaFlagCoders[stateIdx]);

        doubleConfig.initialWord = stateWordOffset+2;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 32;
        doubleConfig.resolution = 1.0/protocolFpgaClockFrequencyHz;
        doubleConfig.minValue = 0;
        doubleConfig.maxValue = UINT32_MAX*doubleConfig.resolution;
        stateTimeoutValueCoders[stateIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(stateTimeoutValueCoders[stateIdx]);

        boolConfig.initialWord = stateWordOffset+4;
        boolConfig.initialBit = 0;
        boolConfig.bitsNum = 16;
        stateTimeoutNextStateCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTimeoutNextStateCoders[stateIdx]);

        doubleConfig.initialWord = stateWordOffset+5;
        doubleConfig.initialBit = 0;
        for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
            doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
            doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
            doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
            stateMinTriggerCurrentCoders[rangeIdx][stateIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(stateMinTriggerCurrentCoders[rangeIdx][stateIdx]);
        }

        doubleConfig.initialWord = stateWordOffset+6;
        doubleConfig.initialBit = 0;
        for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
            doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
            doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
            doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
            stateMaxTriggerCurrentCoders[rangeIdx][stateIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(stateMaxTriggerCurrentCoders[rangeIdx][stateIdx]);
        }

        boolConfig.initialWord = stateWordOffset+7;
        boolConfig.initialBit = 0;
        boolConfig.bitsNum = 16;
        stateTriggerNextStateCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerNextStateCoders[stateIdx]);

        stateWordOffset = stateWordOffset + stateWordsNum;
    }

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[0] = 0x0003; /*! FPGA and DCM in reset by default */
    txStatus[2] = 0x0001; /*! one voltage frame every current frame */
    txStatus[13] = 0x00F0; /*! disable the x20 amplification on startup */
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

ErrorCodes_t Emcr4x10MHz_PCBV01_V03::initializeHW() {
    /*! Reset DCM to start 10MHz clock */
    dcmResetCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    dcmResetCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    /*! After a short while the 10MHz clock starts */
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    this->resetFpga(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    this->resetFpga(false, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    writeAdcSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    writeDacSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    writeAdcSpiCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    return Success;
}

Emcr4x10MHz_PCBV03_V03::Emcr4x10MHz_PCBV03_V03(std::string di):
    Emcr4x10MHz_PCBV01_V03(di) {

    fwName = "4x10MHz_V05.bit";

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter16kHz].value = 16.0;
    vcVoltageFiltersArray[VCVoltageFilter16kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter16kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].value = 1.6;
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter16kHz;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 8;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x00); // 16kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x55); // 1.6kHz on all channels
    coders.push_back(vcVoltageFilterCoder);
}

Emcr4x10MHz_PCBV03_V04::Emcr4x10MHz_PCBV03_V04(std::string di):
    Emcr4x10MHz_PCBV03_V03(di) {

    fwName = "4x10MHz_V0_6_pcbV2.bit";

    txDataWords = 442; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txModifiedStartingWord = txDataWords;
    txModifiedEndingWord = 0;
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    stateWordOffset = 282;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    doubleConfig.initialWord = 280;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.minValue = 0.0;
    doubleConfig.maxValue = 267.0;
    doubleConfig.resolution = 1.0;
    stateArrayMovingAverageLengthCoder = new DoubleOffsetBinaryCoder(doubleConfig);

    stateAppliedVoltageCoders.resize(VCVoltageRangesNum);
    for (int rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        stateAppliedVoltageCoders[rangeIdx].resize(stateMaxNum);
    }

    stateTimeoutFlagCoders.resize(stateMaxNum);
    stateTriggerFlagCoders.resize(stateMaxNum);
    stateTriggerDeltaFlagCoders.resize(stateMaxNum);

    stateTimeoutValueCoders.resize(stateMaxNum);
    stateTimeoutNextStateCoders.resize(stateMaxNum);
    stateMinTriggerCurrentCoders.resize(VCCurrentRangesNum);
    stateMaxTriggerCurrentCoders.resize(VCCurrentRangesNum);
    for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        stateMinTriggerCurrentCoders[rangeIdx].resize(stateMaxNum);
        stateMaxTriggerCurrentCoders[rangeIdx].resize(stateMaxNum);
    }

    stateTriggerNextStateCoders.resize(stateMaxNum);

    for (unsigned int stateIdx = 0; stateIdx < stateMaxNum; stateIdx++) {
        doubleConfig.initialWord = stateWordOffset;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        for (int rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
            doubleConfig.resolution = -vcVoltageRangesArray[rangeIdx].step; /*! The voltage is applied on the reference pin, so voltages must be reversed */
            doubleConfig.maxValue = -doubleConfig.resolution*40000.0; /*! The working point is 2.5V */
            doubleConfig.minValue = doubleConfig.maxValue+doubleConfig.resolution*65535.0;
            stateAppliedVoltageCoders[rangeIdx][stateIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(stateAppliedVoltageCoders[rangeIdx][stateIdx]);
        }

        boolConfig.initialWord = stateWordOffset + 1;
        boolConfig.bitsNum = 1;
        boolConfig.initialBit = 0;
        stateTimeoutFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTimeoutFlagCoders[stateIdx]);

        boolConfig.initialBit = 1;
        stateTriggerFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerFlagCoders[stateIdx]);

        boolConfig.initialBit = 2;
        stateTriggerDeltaFlagCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerDeltaFlagCoders[stateIdx]);

        doubleConfig.initialWord = stateWordOffset+2;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 32;
        doubleConfig.resolution = 1.0/protocolFpgaClockFrequencyHz;
        doubleConfig.minValue = 0;
        doubleConfig.maxValue = UINT32_MAX*doubleConfig.resolution;
        stateTimeoutValueCoders[stateIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(stateTimeoutValueCoders[stateIdx]);

        boolConfig.initialWord = stateWordOffset+4;
        boolConfig.initialBit = 0;
        boolConfig.bitsNum = 16;
        stateTimeoutNextStateCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTimeoutNextStateCoders[stateIdx]);

        doubleConfig.initialWord = stateWordOffset+5;
        doubleConfig.initialBit = 0;
        for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
            doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
            doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
            doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
            stateMinTriggerCurrentCoders[rangeIdx][stateIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(stateMinTriggerCurrentCoders[rangeIdx][stateIdx]);
        }

        doubleConfig.initialWord = stateWordOffset+6;
        doubleConfig.initialBit = 0;
        for (int rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
            doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
            doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
            doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
            stateMaxTriggerCurrentCoders[rangeIdx][stateIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(stateMaxTriggerCurrentCoders[rangeIdx][stateIdx]);
        }

        boolConfig.initialWord = stateWordOffset+7;
        boolConfig.initialBit = 0;
        boolConfig.bitsNum = 16;
        stateTriggerNextStateCoders[stateIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(stateTriggerNextStateCoders[stateIdx]);

        stateWordOffset = stateWordOffset + stateWordsNum;
    }

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[0] = 0x0003; /*! FPGA and DCM in reset by default */
    txStatus[2] = 0x0001; /*! one voltage frame every current frame */
    txStatus[13] = 0x00F0; /*! disable the x20 amplification on startup */
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

Emcr4x10MHz_QuadAnalog_PCBV01_V05::Emcr4x10MHz_QuadAnalog_PCBV01_V05(std::string id) :
    Emcr4x10MHz_PCBV03_V04(id) {

    fwName = "4x10MHz_QA_V0_1_pcbV2.bit";

    /*! Disable state arrays */
    stateMaxNum = 0;
    numberOfStatesCoder = nullptr;
    initialStateCoder = nullptr;
    enableStateArrayChannelsCoder.clear();
    stateAppliedVoltageCoders.clear();
    stateTimeoutFlagCoders.clear();
    stateTriggerFlagCoders.clear();
    stateTriggerDeltaFlagCoders.clear();
    stateTimeoutValueCoders.clear();
    stateTimeoutNextStateCoders.clear();
    stateMinTriggerCurrentCoders.clear();
    stateMaxTriggerCurrentCoders.clear();
    stateTriggerNextStateCoders.clear();

    customOptionsNum = CustomOptionsNum;
    customOptionsNames.resize(customOptionsNum);
    customOptionsNames[CustomOptionInterposer] = "Interposer";
    customOptionsDescriptions.resize(customOptionsNum);
    customOptionsDescriptions[CustomOptionInterposer].resize(4);
    customOptionsDescriptions[CustomOptionInterposer][0] = "Ch 1, 5, 9, 13";
    customOptionsDescriptions[CustomOptionInterposer][1] = "Ch 2, 6, 10, 14";
    customOptionsDescriptions[CustomOptionInterposer][2] = "Ch 3, 7, 11, 15";
    customOptionsDescriptions[CustomOptionInterposer][3] = "Ch 4, 8, 12, 16";
    customOptionsDefault.resize(CustomOptionsNum);
    customOptionsDefault[CustomOptionInterposer] = 0;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    boolConfig.initialWord = 0;
    boolConfig.initialBit = 7;
    boolConfig.bitsNum = 2;
    customOptionsCoders.resize(customOptionsNum);
    customOptionsCoders[CustomOptionInterposer] = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (customOptionsCoders[CustomOptionInterposer])->addMapItem(0x3);
    static_cast <BoolRandomArrayCoder *> (customOptionsCoders[CustomOptionInterposer])->addMapItem(0x2);
    static_cast <BoolRandomArrayCoder *> (customOptionsCoders[CustomOptionInterposer])->addMapItem(0x1);
    static_cast <BoolRandomArrayCoder *> (customOptionsCoders[CustomOptionInterposer])->addMapItem(0x0);
    coders.push_back(customOptionsCoders[CustomOptionInterposer]);

    txStatus[0] = (txStatus[0] & 0xFE7F) | 0x0180; /*! Set the default interposer configuration */
}

Emcr4x10MHz_QuadAnalog_PCBV01_DIGV01_V05::Emcr4x10MHz_QuadAnalog_PCBV01_DIGV01_V05(std::string id) :
    Emcr4x10MHz_QuadAnalog_PCBV01_V05(id) {

    fwName = "4x10MHz_quad_analog_digv01_V08.bit";
}

Emcr4x10MHz_SB_PCBV01_V05::Emcr4x10MHz_SB_PCBV01_V05(std::string id) :
    Emcr4x10MHz_QuadAnalog_PCBV01_V05(id) {

    fwName = "4x10MHz_SB_EL05a_V3_1_1_0.bit";

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter16kHz].value = 16.0;
    vcVoltageFiltersArray[VCVoltageFilter16kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter16kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].value = 1.6;
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1_6kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter160Hz].value = 0.16;
    vcVoltageFiltersArray[VCVoltageFilter160Hz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter160Hz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter16kHz;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 8;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x00); // 16kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0xAA); // 1.6kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0xFF); // 160Hz on all channels
    coders.push_back(vcVoltageFilterCoder);
}

Emcr4x10MHz_SB_PCBV01_V06::Emcr4x10MHz_SB_PCBV01_V06(std::string id) :
    Emcr4x10MHz_SB_PCBV01_V05(id) {

    fwName = "4x10MHz_SB_EL05a_V3_2_1_0.bit";

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    /*! Protocol reset */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 1;
    protocolResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolResetCoder);
}
