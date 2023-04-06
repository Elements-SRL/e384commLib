#include "messagedispatcher_4x10mhz.h"

MessageDispatcher_4x10MHz_V01::MessageDispatcher_4x10MHz_V01(string di) :
    MessageDispatcher_OpalKelly(di) {

    deviceName = "4x10MHz";

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

    rxWordOffsets[RxMessageDataHeader] = rxWordOffsets[RxMessageDataLoad] + rxWordLengths[RxMessageDataLoad];
    rxWordLengths[RxMessageDataHeader] = 4;

    rxWordOffsets[RxMessageDataTail] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageDataTail] = 1;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataTail] + rxWordLengths[RxMessageDataTail];
    rxWordLengths[RxMessageStatus] = 1;

    rxWordOffsets[RxMessageVoltageOffset] = rxWordOffsets[RxMessageStatus] + rxWordLengths[RxMessageStatus];
    rxWordLengths[RxMessageVoltageOffset] = currentChannelsNum;

    maxOutputPacketsNum = E384CL_DATA_ARRAY_SIZE/totalChannelsNum;

    rxMaxWords = currentChannelsNum; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 45; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
    txDataWords = (txDataWords/2+1)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txModifiedStartingWord = txDataWords;
    txModifiedEndingWord = 0;
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    /*! Clamping modalities */
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = E384CL_VOLTAGE_CLAMP_MODE;
    defaultClampingModalityIdx = VoltageClamp;

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
    vcVoltageFiltersNum = VCCurrentFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter20kHz].value = 20.0;
    vcVoltageFiltersArray[VCVoltageFilter20kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter20kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter1kHz].value = 1.0;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter3Hz].value = 3.0;
    vcVoltageFiltersArray[VCVoltageFilter3Hz].prefix = UnitPfxNone;
    vcVoltageFiltersArray[VCVoltageFilter3Hz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter20kHz;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate13_3MHz].value = 80.0/6.0/1.0;
    samplingRatesArray[SamplingRate13_3MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate13_3MHz].unit = "Hz";
    samplingRatesArray[SamplingRate6_67MHz].value = 80.0/6.0/2.0;
    samplingRatesArray[SamplingRate6_67MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate6_67MHz].unit = "Hz";
    samplingRatesArray[SamplingRate3_33MHz].value = 80.0/6.0/3.0;
    samplingRatesArray[SamplingRate3_33MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate3_33MHz].unit = "Hz";
    samplingRatesArray[SamplingRate1_67MHz].value = 80.0/6.0/4.0;
    samplingRatesArray[SamplingRate1_67MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate1_67MHz].unit = "Hz";
    samplingRatesArray[SamplingRate833kHz].value = 80.0/6.0/5.0;
    samplingRatesArray[SamplingRate833kHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate833kHz].unit = "Hz";
    samplingRatesArray[SamplingRate417kHz].value = 80.0/6.0/6.0;
    samplingRatesArray[SamplingRate417kHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate417kHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate13_3MHz;

    realSamplingRatesArray.resize(samplingRatesNum);
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
    realSamplingRatesArray[SamplingRate417kHz].value = 80.0/6.0/32.0;
    realSamplingRatesArray[SamplingRate417kHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate417kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
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
    integrationStepArray[SamplingRate417kHz].value = 32.0*6.0/80.0;
    integrationStepArray[SamplingRate417kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate417kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate13_3MHz, -1},
        {SamplingRate6_67MHz, -1},
        {SamplingRate3_33MHz, -1},
        {SamplingRate1_67MHz, -1},
        {SamplingRate833kHz, -1},
        {SamplingRate417kHz, -1},
    };

    // mapping ADC Current Clamp
    // undefined

    vHoldRange.resize(VCVoltageRangesNum);
    vHoldRange[VCVoltageRange1000mV].min = -1000.0; /*! \todo FCON qui bisogna mettere due range */
    vHoldRange[VCVoltageRange1000mV].max = 1000.0;
    vHoldRange[VCVoltageRange1000mV].step = 0.0625;
    vHoldRange[VCVoltageRange1000mV].prefix = UnitPfxMilli;
    vHoldRange[VCVoltageRange1000mV].unit = "V";
    vHoldRange[VCVoltageRange20000mV].min = -20000.0; /*! \todo FCON qui bisogna mettere due range */
    vHoldRange[VCVoltageRange20000mV].max = 20000.0;
    vHoldRange[VCVoltageRange20000mV].step = 1.28125;
    vHoldRange[VCVoltageRange20000mV].prefix = UnitPfxMilli;
    vHoldRange[VCVoltageRange20000mV].unit = "V";
    selectedVoltageHoldVector.resize(currentChannelsNum);
    Measurement_t defaultVoltageHoldTuner = {0.0, vHoldRange[VCVoltageRange1000mV].prefix, vHoldRange[VCVoltageRange1000mV].unit};

    /*! VC current gain */
    calibVcCurrentGainRange.step = 1.0/1024.0;
    calibVcCurrentGainRange.min = 0;//SHORT_MIN * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.max = SHORT_MAX * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.prefix = UnitPfxNone;
    calibVcCurrentGainRange.unit = "";
    selectedCalibVcCurrentGainVector.resize(currentChannelsNum);
    Measurement_t defaultCalibVcCurrentGain = {1.0, calibVcCurrentGainRange.prefix, calibVcCurrentGainRange.unit}; /*! \todo FCON qui c'è il valor medio per i 200nA */

    /*! VC current offset */
    calibVcCurrentOffsetRanges = vcCurrentRangesArray;
    selectedCalibVcCurrentOffsetVector.resize(currentChannelsNum);
    Measurement_t defaultCalibVcCurrentOffset = {0.0, calibVcCurrentOffsetRanges[defaultVcCurrentRangeIdx].prefix, calibVcCurrentOffsetRanges[defaultVcCurrentRangeIdx].unit};

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

    /*! Write ADC SPI */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    dcmResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(dcmResetCoder);

    /*! Write DAC SPI */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    dcmResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(dcmResetCoder);

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

    /*! Current range VC */
    // undefined

    /*! Voltage range VC */
    boolConfig.initialWord = 4+9; //updated
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
    boolConfig.initialWord = 2+9; //updated
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 8;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x00); // 20kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0x55); // 1kHz on all channels
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0xCC); // 3Hz on all channels
    coders.push_back(vcVoltageFilterCoder);

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
    boolConfig.initialWord = 4+9; //updated
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

    /*! V holding tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vHoldTunerCoders.resize(VCVoltageRangesNum);

    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 249+9; //updated
        doubleConfig.resolution = vHoldRange[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*40000.0; /*! The working point is 2.5V */
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! VC current gain tuner */
    doubleConfig.initialWord = 253+9; //updated
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
        doubleConfig.initialWord = 257+9; //updated
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
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    // settare solo i bit che di default sono ad uno e che non hanno un controllo diretto (bit di debug, etc)
}

MessageDispatcher_4x10MHz_V01::~MessageDispatcher_4x10MHz_V01() {

}

void MessageDispatcher_4x10MHz_V01::initializeHW() {
    dcmResetCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    this_thread::sleep_for(chrono::milliseconds(100));

    dcmResetCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    writeAdcSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    writeDacSpiCoder->encode(true, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);

    this_thread::sleep_for(chrono::milliseconds(100));

    writeAdcSpiCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    writeDacSpiCoder->encode(false, txStatus, txModifiedStartingWord, txModifiedEndingWord);
    this->stackOutgoingMessage(txStatus);
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
