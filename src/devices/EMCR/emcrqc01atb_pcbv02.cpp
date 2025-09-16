#include "emcrqc01atb_pcbv02.h"

EmcrQc01aTB_PCBV02::EmcrQc01aTB_PCBV02(std::string di) :
    EmcrFtdiDevice(di) {

    spiChannel = 'B';
    rxChannel = 'A';
    txChannel = 'A';

    fpgaLoadType = FtdiFpgaFwLoadAutomatic;

    deviceName = "QC01aTB";

    fwSize_B = 0;
    motherboardBootTime_s = 0; // no motherboard to be started

    rxSyncWord = 0x5aa55aa5;

    packetsPerFrame = 256;

    voltageChannelsNum = 4;
    currentChannelsNum = 4;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 1;

    rxWordOffsets[RxMessageVoltageDataLoad] = 0;
    rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    rxWordOffsets[RxMessageCurrentDataLoad] = rxWordOffsets[RxMessageVoltageDataLoad] + rxWordLengths[RxMessageVoltageDataLoad];
    rxWordLengths[RxMessageCurrentDataLoad] = currentChannelsNum*packetsPerFrame;

    rxMaxWords = currentChannelsNum*packetsPerFrame; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 262; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
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
    // voltageProtocolSinImplemented = true;

    protocolMaxItemsNum = 20;
    protocolWordOffset = 14;
    protocolItemsWordsNum = 12;

    /*! Current ranges */
    /*! VC */
    independentVcCurrentRanges = true;
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    // vcCurrentRangesArray[VCCurrentRange1nA].min = -1.0;
    // vcCurrentRangesArray[VCCurrentRange1nA].max = 1.0;
    // vcCurrentRangesArray[VCCurrentRange1nA].step = vcCurrentRangesArray[VCCurrentRange1nA].max/SHORT_MAX;
    // vcCurrentRangesArray[VCCurrentRange1nA].prefix = UnitPfxNano;
    // vcCurrentRangesArray[VCCurrentRange1nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange10nA].min = -10.0;
    vcCurrentRangesArray[VCCurrentRange10nA].max = 10.0;
    vcCurrentRangesArray[VCCurrentRange10nA].step = vcCurrentRangesArray[VCCurrentRange10nA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange10nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange10nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange100nA].min = -100.0;
    vcCurrentRangesArray[VCCurrentRange100nA].max = 100.0;
    vcCurrentRangesArray[VCCurrentRange100nA].step = vcCurrentRangesArray[VCCurrentRange100nA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange100nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange100nA].unit = "A";
    defaultVcCurrentRangeIdx = VCCurrentRange10nA;

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
    vcCurrentFiltersArray[VCCurrentFilter20kHz].value = 20.0;
    vcCurrentFiltersArray[VCCurrentFilter20kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter20kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter100kHz].value = 100.0;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter20kHz;

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
    vcVoltageFiltersArray[VCVoltageFilter20kHz].value = 20.0;
    vcVoltageFiltersArray[VCVoltageFilter20kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter20kHz].unit = "Hz";
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
    defaultSamplingRateIdx = SamplingRate1_25kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate1_25kHz].value = 80.0/8.0/8192.0;
    realSamplingRatesArray[SamplingRate1_25kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate1_25kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate2_5kHz].value = 80.0/8.0/4096.0;
    realSamplingRatesArray[SamplingRate2_5kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate2_5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate5kHz].value = 80.0/8.0/2048.0;
    realSamplingRatesArray[SamplingRate5kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate10kHz].value = 80.0/8.0/1024.0;
    realSamplingRatesArray[SamplingRate10kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate10kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate20kHz].value = 80.0/8.0/512.0;
    realSamplingRatesArray[SamplingRate20kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate20kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate50kHz].value = 80.0/8.0/200.0;
    realSamplingRatesArray[SamplingRate50kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate50kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate100kHz].value = 80.0/8.0/100.0;
    realSamplingRatesArray[SamplingRate100kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate100kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate200kHz].value = 80.0/8.0/50.0;
    realSamplingRatesArray[SamplingRate200kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate200kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate312_5kHz].value = 90.0/9.0/32.0;
    realSamplingRatesArray[SamplingRate312_5kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate312_5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate625kHz].value = 90.0/9.0/16.0;
    realSamplingRatesArray[SamplingRate625kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate625kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate1_25MHz].value = 90.0/9.0/8.0;
    realSamplingRatesArray[SamplingRate1_25MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate1_25MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate2_5MHz].value = 90.0/9.0/4.0;
    realSamplingRatesArray[SamplingRate2_5MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate2_5MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate5MHz].value = 90.0/9.0/2.0;
    realSamplingRatesArray[SamplingRate5MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate5MHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate10MHz].value = 90.0/9.0;
    realSamplingRatesArray[SamplingRate10MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate10MHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate1_25kHz] = 2;
    sr2srm[SamplingRate5kHz] = 2;
    sr2srm[SamplingRate10kHz] = 2;
    sr2srm[SamplingRate20kHz] = 2;
    sr2srm[SamplingRate50kHz] = 1;
    sr2srm[SamplingRate100kHz] = 1;
    sr2srm[SamplingRate200kHz] = 1;
    sr2srm[SamplingRate312_5kHz] = 0;
    sr2srm[SamplingRate625kHz] = 0;
    sr2srm[SamplingRate1_25MHz] = 0;
    sr2srm[SamplingRate2_5MHz] = 0;
    sr2srm[SamplingRate5MHz] = 0;
    sr2srm[SamplingRate10MHz] = 0;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate1_25kHz].value = 8192.0*8.0/80.0;
    integrationStepArray[SamplingRate1_25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_25kHz].unit = "s";
    integrationStepArray[SamplingRate2_5kHz].value = 4096.0*8.0/80.0;
    integrationStepArray[SamplingRate2_5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate2_5kHz].unit = "s";
    integrationStepArray[SamplingRate5kHz].value = 2048.0*8.0/80.0;
    integrationStepArray[SamplingRate5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5kHz].unit = "s";
    integrationStepArray[SamplingRate10kHz].value = 1024.0*8.0/80.0;
    integrationStepArray[SamplingRate10kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate10kHz].unit = "s";
    integrationStepArray[SamplingRate20kHz].value = 512.0*8.0/80.0;
    integrationStepArray[SamplingRate20kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate20kHz].unit = "s";
    integrationStepArray[SamplingRate50kHz].value = 200.0*8.0/80.0;
    integrationStepArray[SamplingRate50kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate50kHz].unit = "s";
    integrationStepArray[SamplingRate100kHz].value = 100.0*8.0/80.0;
    integrationStepArray[SamplingRate100kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate100kHz].unit = "s";
    integrationStepArray[SamplingRate200kHz].value = 50.0*8.0/80.0;
    integrationStepArray[SamplingRate200kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate200kHz].unit = "s";
    integrationStepArray[SamplingRate312_5kHz].value = 32.0*9.0/90.0;
    integrationStepArray[SamplingRate312_5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate312_5kHz].unit = "s";
    integrationStepArray[SamplingRate625kHz].value = 16.0*9.0/90.0;
    integrationStepArray[SamplingRate625kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate625kHz].unit = "s";
    integrationStepArray[SamplingRate1_25MHz].value = 8.0*9.0/90.0;
    integrationStepArray[SamplingRate1_25MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_25MHz].unit = "s";
    integrationStepArray[SamplingRate2_5MHz].value = 4.0*9.0/90.0;
    integrationStepArray[SamplingRate2_5MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate2_5MHz].unit = "s";
    integrationStepArray[SamplingRate5MHz].value = 2.0*9.0/90.0;
    integrationStepArray[SamplingRate5MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5MHz].unit = "s";
    integrationStepArray[SamplingRate10MHz].value = 1.0*9.0/90.0;
    integrationStepArray[SamplingRate10MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate10MHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate1_25kHz, VCCurrentFilter20kHz},
        {SamplingRate2_5kHz, VCCurrentFilter20kHz},
        {SamplingRate5kHz, VCCurrentFilter20kHz},
        {SamplingRate10kHz, VCCurrentFilter20kHz},
        {SamplingRate20kHz, VCCurrentFilter20kHz},
        {SamplingRate50kHz, VCCurrentFilter100kHz},
        {SamplingRate100kHz, VCCurrentFilter100kHz},
        {SamplingRate200kHz, VCCurrentFilter100kHz},
        {SamplingRate312_5kHz, VCCurrentFilter100kHz},
        {SamplingRate625kHz, VCCurrentFilter100kHz},
        {SamplingRate1_25MHz, VCCurrentFilter100kHz},
        {SamplingRate2_5MHz, VCCurrentFilter100kHz},
        {SamplingRate5MHz, VCCurrentFilter100kHz},
        {SamplingRate10MHz, VCCurrentFilter100kHz}
    };

    // mapping ADC Current Clamp
    // undefined

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};

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

    /*! ASIC reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    asicResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(asicResetCoder);

    /*! FPGA reset */
    boolConfig.initialWord = 6;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 1;
    fpgaResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(fpgaResetCoder);

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 7;
    samplingRateCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(127);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(94);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(93);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(92);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(91);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(58);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(57);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(24);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(5);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(4);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(3);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(2);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(1);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(0);
    coders.push_back(samplingRateCoder);

    /*! Protocol reset */
    boolConfig.initialWord = 6;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 1;
    protocolResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolResetCoder);

    /*! Current range VC */
    vcCurrentRangeCoders.clear();

    boolConfig.initialWord = 10;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    vcCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[0]);

    boolConfig.initialWord = 10;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    vcCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[1]);

    boolConfig.initialWord = 10;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 2;
    vcCurrentRangeCoders.push_back(new BoolOneHotCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[2]);

    boolConfig.initialWord = 10;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;
    vcCurrentRangeCoders.push_back(new BoolOneHotCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[3]);

    /*! Voltage range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 8;
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
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 2;
    vcCurrentFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcCurrentFilterCoder)->addMapItem(3); // 20kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcCurrentFilterCoder)->addMapItem(0); // 100kHz on all channels
    coders.push_back(vcCurrentFilterCoder);

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x00); // 1kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x55); // 10kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0xAA); // 20kHz on all channels
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
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    customFlagsNum = CustomFlagsNum;
    customFlagsNames.resize(customFlagsNum);
    customFlagsNames[AcDsEn1] = "Enable AC DS for core 1";
    customFlagsNames[AcDsEn2] = "Enable AC DS for core 1";
    customFlagsNames[DcDsEn1] = "Enable DC DS for core 2";
    customFlagsNames[DcDsEn2] = "Enable DC DS for core 2";
    customFlagsNames[DcSepEn1] = "Separate DC for core 1";
    customFlagsNames[DcSepEn2] = "Separate DC for core 2";
    customFlagsDefault.resize(customFlagsNum);
    customFlagsDefault[AcDsEn1] = true;
    customFlagsDefault[AcDsEn2] = true;
    customFlagsDefault[DcDsEn1] = false;
    customFlagsDefault[DcDsEn2] = false;
    customFlagsDefault[DcSepEn1] = false;
    customFlagsDefault[DcSepEn2] = false;
    customFlagsCoders.resize(customFlagsNum);
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    customFlagsCoders[AcDsEn1] = new BoolArrayCoder(boolConfig);
    coders.push_back(customFlagsCoders[AcDsEn1]);
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    customFlagsCoders[AcDsEn2] = new BoolArrayCoder(boolConfig);
    coders.push_back(customFlagsCoders[AcDsEn2]);
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 1;
    customFlagsCoders[DcDsEn1] = new BoolArrayCoder(boolConfig);
    coders.push_back(customFlagsCoders[DcDsEn1]);
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 1;
    customFlagsCoders[DcDsEn2] = new BoolArrayCoder(boolConfig);
    coders.push_back(customFlagsCoders[DcDsEn2]);
    boolConfig.initialWord = 2;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    customFlagsCoders[DcSepEn1] = new BoolArrayCoder(boolConfig);
    coders.push_back(customFlagsCoders[DcSepEn1]);
    boolConfig.initialWord = 2;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    customFlagsCoders[DcSepEn2] = new BoolArrayCoder(boolConfig);
    coders.push_back(customFlagsCoders[DcSepEn2]);

    customOptionsNum = CustomOptionsNum;
    customOptionsNames.resize(customOptionsNum);
    customOptionsNames[DacExtFilter] = "External DAC bandwidth";
    customOptionsDescriptions.resize(customOptionsNum);
    customOptionsDescriptions[DacExtFilter].resize(2);
    customOptionsDescriptions[DacExtFilter][0] = "16Hz";
    customOptionsDescriptions[DacExtFilter][1] = "180kHz";
    customOptionsDefault.resize(customOptionsNum);
    customOptionsDefault[DacExtFilter] = 0;
    customOptionsCoders.resize(customOptionsNum);
    boolConfig.initialWord = 4;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    customOptionsCoders[DacExtFilter] = new BoolNegatedArrayCoder(boolConfig);
    coders.push_back(customOptionsCoders[DacExtFilter]);

    customDoublesNum = CustomDoublesNum;
    customDoublesNames.resize(customDoublesNum);
    customDoublesNames[DacExt] = "External DAC";
    customDoublesNames[SecondaryDacCore1] = "Secondary DAC core 1";
    customDoublesNames[SecondaryDacCore2] = "Secondary DAC core 2";
    customDoublesRanges.resize(customDoublesNum);
    customDoublesRanges[DacExt].step = 0.0625;
    customDoublesRanges[DacExt].min = -1650.0;
    customDoublesRanges[DacExt].max = customDoublesRanges[DacExt].min+customDoublesRanges[DacExt].step*USHORT_MAX;
    customDoublesRanges[DacExt].prefix = UnitPfxMilli;
    customDoublesRanges[DacExt].unit = "V";
    customDoublesRanges[SecondaryDacCore1].step = 1.0;
    customDoublesRanges[SecondaryDacCore1].min = -64.0;
    customDoublesRanges[SecondaryDacCore1].max = 63.0;
    customDoublesRanges[SecondaryDacCore1].prefix = UnitPfxMilli;
    customDoublesRanges[SecondaryDacCore1].unit = "V";
    customDoublesRanges[SecondaryDacCore2].step = 1.0;
    customDoublesRanges[SecondaryDacCore2].min = -64.0;
    customDoublesRanges[SecondaryDacCore2].max = 63.0;
    customDoublesRanges[SecondaryDacCore2].prefix = UnitPfxMilli;
    customDoublesRanges[SecondaryDacCore2].unit = "V";
    customDoublesDefault.resize(customDoublesNum);
    customDoublesDefault[DacExt] = 0.0;
    customDoublesDefault[SecondaryDacCore1] = 0.0;
    customDoublesDefault[SecondaryDacCore2] = 0.0;
    customDoublesCoders.resize(customDoublesNum);
    doubleConfig.initialWord = 260;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.minValue = customDoublesRanges[DacExt].min;
    doubleConfig.maxValue = customDoublesRanges[DacExt].max;
    doubleConfig.resolution = customDoublesRanges[DacExt].step;
    customDoublesCoders[DacExt] = new DoubleOffsetBinaryCoder(doubleConfig);
    coders.push_back(customDoublesCoders[DacExt]);
    doubleConfig.initialWord = 8;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 7;
    doubleConfig.minValue = customDoublesRanges[SecondaryDacCore1].min;
    doubleConfig.maxValue = customDoublesRanges[SecondaryDacCore1].max;
    doubleConfig.resolution = customDoublesRanges[SecondaryDacCore1].step;
    customDoublesCoders[SecondaryDacCore1] = new DoubleOffsetBinaryCoder(doubleConfig);
    coders.push_back(customDoublesCoders[SecondaryDacCore1]);
    doubleConfig.initialWord = 9;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 7;
    doubleConfig.minValue = customDoublesRanges[SecondaryDacCore2].min;
    doubleConfig.maxValue = customDoublesRanges[SecondaryDacCore2].max;
    doubleConfig.resolution = customDoublesRanges[SecondaryDacCore2].step;
    customDoublesCoders[SecondaryDacCore2] = new DoubleOffsetBinaryCoder(doubleConfig);
    coders.push_back(customDoublesCoders[SecondaryDacCore2]);

    /*! Default status */
    txStatus.init(txDataWords);
    txStatus.encodingWords[1] = 0x000F; /*! DS of AC and DC cores enabled */
    txStatus.encodingWords[3] = 0x0003; /*! Capacitors on VcCaps, Vcm generated externally */
    txStatus.encodingWords[6] = 0x0004; /*! FPGA in reset by default */
    txStatus.encodingWords[7] = 0x0001; /*! one voltage frame every current frame */
    txStatus.encodingWords[8] = 0x0040; /*! null offset on secondary DAC for core 1 */
    txStatus.encodingWords[9] = 0x0040; /*! null offset on secondary DAC for core 2 */
}

ErrorCodes_t EmcrQc01aTB_PCBV02::initializeHW() {
    /*! After a short while the 10MHz clock starts */
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    this->resetFpga(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));
    this->resetFpga(false, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(100));

    return Success;
}

ErrorCodes_t EmcrQc01aTB_PCBV02::setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) {
    // bool adcToBeRenabled = (selectedSamplingRateIdx >= SamplingRate200kHz && samplingRateIdx < SamplingRate200kHz);
    // if (adcToBeRenabled) {
    //     /*! Prepare the bit but don't send it yet, send it together with the sampling rate command */
    //     writeAdcSpiCoder->encode(true, txStatus);
    // }

    EmcrDevice::setSamplingRate(samplingRateIdx, applyFlag);

    // if (adcToBeRenabled) {
    //     std::this_thread::sleep_for (std::chrono::milliseconds(100));
    //     writeAdcSpiCoder->encode(false, txStatus);
    //     this->stackOutgoingMessage(txStatus);
    // }
    return Success;
}
