#include "messagedispatcher_384nanopores.h"

MessageDispatcher_384NanoPores_V01::MessageDispatcher_384NanoPores_V01(string di) :
    MessageDispatcher_OpalKelly(di) {

    deviceName = "384NanoPores";

    rxSyncWord = 0x5aa5;

    packetsPerFrame = 1;

    voltageChannelsNum = 384;
    currentChannelsNum = 384;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 24;

    rxWordOffsets.resize(RxMessageNum);
    rxWordLengths.resize(RxMessageNum);
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

    rxMaxWords = 0; // ANCORA NON DEFINITO, AGGIORNARE
    maxInputFrameSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 874+0; // ANCORA NON DEFINITO, AGGIORNARE
    txModifiedStartingWord = txDataWords;
    txModifiedEndingWord = 0;
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

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
    samplingRatesArray[SamplingRate100Hz].value = 100.0;
    samplingRatesArray[SamplingRate100Hz].prefix = UnitPfxNone;
    samplingRatesArray[SamplingRate100Hz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate100Hz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate100Hz].value = 100.0;
    realSamplingRatesArray[SamplingRate100Hz].prefix = UnitPfxNone;
    realSamplingRatesArray[SamplingRate100Hz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate100Hz].value = 0.01;
    integrationStepArray[SamplingRate100Hz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate100Hz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcMap = {
      {SamplingRate100Hz, VCCurrentFilter20kHz}
    };

    // mapping ADC Current Clamp
    // undefined

    vHoldRange.min = -500.0;
    vHoldRange.max = 500.0;
    vHoldRange.step = 0.125;
    vHoldRange.prefix = UnitPfxMilli;
    vHoldRange.unit = "V";

    /*! This will never change so it makes sense to initialize it here */
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

    selectedVoltageOffset.resize(currentChannelsNum);
    voltageOffsetRange = vcVoltageRangesArray[VCVoltageRange500mV];
    for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        selectedVoltageOffset[channelIdx].value = 0.0;
        selectedVoltageOffset[channelIdx].prefix = voltageOffsetRange.prefix;
        selectedVoltageOffset[channelIdx].unit = voltageOffsetRange.unit;
    }

    /*! VC current gain */
    vcCurrentGainRange.step = 1.0/1024.0;
    vcCurrentGainRange.min = SHORT_MIN * vcCurrentGainRange.step;
    vcCurrentGainRange.max = SHORT_MAX * vcCurrentGainRange.step;
    vcCurrentGainRange.prefix = UnitPfxNone;
    vcCurrentGainRange.unit = "";

    /*! VC current offset */
    vcCurrentOffsetRange = vcCurrentRangesArray[selectedVcCurrentRangeIdx];

    /*! Gate voltage range*/
    gateVoltageRange.step = 1;
    gateVoltageRange.min = SHORT_MIN * gateVoltageRange.step;
    gateVoltageRange.max = SHORT_MAX * gateVoltageRange.step;
    gateVoltageRange.prefix = UnitPfxMilli;
    gateVoltageRange.unit = "V";

    /*! Source voltage range*/
    sourceVoltageRange.step = 0.625;
    sourceVoltageRange.min = SHORT_MIN * sourceVoltageRange.step;
    sourceVoltageRange.max = SHORT_MAX * sourceVoltageRange.step;
    sourceVoltageRange.prefix = UnitPfxMilli;
    sourceVoltageRange.unit = "V";


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

    /*! FPGA reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    fpgaResetCoder = new BoolArrayCoder(boolConfig);

    /*! DOC reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 1;
    docResetCoder = new BoolArrayCoder(boolConfig);

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 4;
    samplingRateCoder = new BoolArrayCoder(boolConfig);

    /*! Clamping mode */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 7;
    boolConfig.bitsNum = 8;
    clampingModeCoder = new BoolArrayCoder(boolConfig);

    /*! DOC override */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 9;
    boolConfig.bitsNum = 1;
    docOverrideCoder = new BoolArrayCoder(boolConfig);

    /*! Current range VC */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentRangeCoder = new BoolArrayCoder(boolConfig);

    /*! Voltage range VC */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageRangeCoder = new BoolArrayCoder(boolConfig);

    /*! Current range CC */
    // undefined

    /*! Voltage range CC */
    // undefined

    /*! Current filter VC */
    boolConfig.initialWord = 2;
    boolConfig.initialBit = 0;//4;
    boolConfig.bitsNum = 4;
    vcCurrentFilterCoder = new BoolArrayCoder(boolConfig);

    /*! Voltage filter VC */
    // undefined

    /*! Current filter CC */
    // undefined

    /*! Voltage filter CC */
    // undefined

    /*! Digital offset compensation */
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    digitalOffsetCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        digitalOffsetCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }


    /*! Enable stimulus */
    boolConfig.initialWord = 27;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    enableStimulusCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        enableStimulusCoders[idx] = new BoolArrayCoder(boolConfig);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! V holding tuner */
    doubleConfig.initialWord = 411;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = vHoldRange.step;
    doubleConfig.minValue = SHORT_MIN*doubleConfig.resolution;
    doubleConfig.maxValue = SHORT_MAX*doubleConfig.resolution;
    vHoldTunerCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        vHoldTunerCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        doubleConfig.initialWord++;
    }
    /*! VC current gain tuner */
    /*! \todo FCON recheck minValue e maxValue*/
    doubleConfig.initialWord = 843;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = vcCurrentGainRange.step;
    doubleConfig.minValue = vcCurrentGainRange.min;
    doubleConfig.maxValue = vcCurrentGainRange.max;
    calibVcCurrentGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcCurrentGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        doubleConfig.initialWord++;
    }

    /*! VC current offset tuner */
    /*! \todo FCON recheck minValue e maxValue*/
    doubleConfig.initialWord = 1227;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = vcCurrentOffsetRange.step;
    doubleConfig.minValue = vcCurrentOffsetRange.min;
    doubleConfig.maxValue = vcCurrentOffsetRange.max;
    calibVcCurrentOffsetCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcCurrentOffsetCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        doubleConfig.initialWord++;
    }

    /*! gate voltage tuner */
    /*! \todo FCON recheck minValue e maxValue*/
    doubleConfig.initialWord = 795;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = gateVoltageRange.step;
    doubleConfig.minValue = gateVoltageRange.min;
    doubleConfig.maxValue = gateVoltageRange.max;
    gateVoltageCoders.resize(totalBoardsNum);
    for (uint32_t idx = 0; idx < totalBoardsNum; idx++) {
        gateVoltageCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        doubleConfig.initialWord++;
    }

    /*! source voltage tuner */
    /*! \todo FCON recheck minValue e maxValue*/
    doubleConfig.initialWord = 819;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = sourceVoltageRange.step;
    doubleConfig.minValue = sourceVoltageRange.min;
    doubleConfig.maxValue = sourceVoltageRange.max;
    sourceVoltageCoders.resize(totalBoardsNum);
    for (uint32_t idx = 0; idx < totalBoardsNum; idx++) {
        sourceVoltageCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        doubleConfig.initialWord++;
    }








    /*! VRest */
    doubleConfig.initialWord = 174;
    doubleConfig.initialBit = 3;
    doubleConfig.bitsNum = 13;
    doubleConfig.minValue = -512.0;
    doubleConfig.maxValue = 511.0;
    doubleConfig.resolution = 1.0;
    doubleConfig.offset = 0.0;
    stimRestCoder = new DoubleTwosCompCoder(doubleConfig);

    /*! Default status */
    txStatus.resize(txDataWords);

    int txIdx = 0;
    txStatus[txIdx++] = 0x0000; // CFG0 da completare
}

MessageDispatcher_384NanoPores_V01::~MessageDispatcher_384NanoPores_V01() {

}

void MessageDispatcher_384NanoPores_V01::initializeDevice() {
    /*! \todo FCON da riempire e magari spostare nel MessageDispatcher */
}

bool MessageDispatcher_384NanoPores_V01::checkProtocolValidity(string &message) {
    /*! \todo FCON da riempire e magari spostare nel MessageDispatcher */
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
