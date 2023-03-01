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

    rxWordOffsets.resize(rxMessageNum);
    rxWordLengths.resize(rxMessageNum);
    rxWordOffsets[rxMessageDataLoad] = 0;
    rxWordLengths[rxMessageDataLoad] = (voltageChannelsNum+currentChannelsNum)*packetsPerFrame;

    rxWordOffsets[rxMessageDataHeader] = rxWordOffsets[rxMessageDataLoad] + rxWordLengths[rxMessageDataLoad];
    rxWordLengths[rxMessageDataHeader] = 4;

    rxWordOffsets[rxMessageDataTail] = rxWordOffsets[rxMessageDataHeader] + rxWordLengths[rxMessageDataHeader];
    rxWordLengths[rxMessageDataTail] = 1;

    rxWordOffsets[rxMessageStatus] = rxWordOffsets[rxMessageDataTail] + rxWordLengths[rxMessageDataTail];
    rxWordLengths[rxMessageStatus] = 1;

    rxWordOffsets[rxMessageVoltageOffset] = rxWordOffsets[rxMessageStatus] + rxWordLengths[rxMessageStatus];
    rxWordLengths[rxMessageVoltageOffset] = currentChannelsNum;

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
    vcCurrentGainRange.min = 0;
    vcCurrentGainRange.max = 64; /*! \todo FCON recheck*/
    vcCurrentGainRange.step = 0.001; /*! \todo FCON recheck*/
    vcCurrentGainRange.prefix = UnitPfxNone; /*! \todo FCON recheck*/
    vcCurrentGainRange.unit = "";/*! \todo FCON recheck*/

    /*! VC current offset */
    vcCurrentOffsetRange.min = vcCurrentRangesArray[selectedVcCurrentRangeIdx].min;
    vcCurrentOffsetRange.max = vcCurrentRangesArray[selectedVcCurrentRangeIdx].max; /*! \todo FCON recheck*/
    vcCurrentOffsetRange.step = vcCurrentRangesArray[selectedVcCurrentRangeIdx].step; /*! \todo FCON recheck*/
    vcCurrentOffsetRange.prefix = vcCurrentRangesArray[selectedVcCurrentRangeIdx].prefix; /*! \todo FCON recheck*/
    vcCurrentOffsetRange.unit = vcCurrentRangesArray[selectedVcCurrentRangeIdx].unit;/*! \todo FCON recheck*/

    /*! Gate voltage range*/
    gateVoltageRange.min = -32768;
    gateVoltageRange.max = 32768; /*! \todo FCON recheck*/
    gateVoltageRange.step = 1; /*! \todo FCON recheck*/
    gateVoltageRange.prefix = UnitPfxMilli; /*! \todo FCON recheck*/
    gateVoltageRange.unit = "V";/*! \todo FCON recheck*/

    /*! Source voltage range*/
    sourceVoltageRange.min = -2048;
    sourceVoltageRange.max = 2048; /*! \todo FCON recheck*/
    sourceVoltageRange.step = 0.625; /*! \todo FCON recheck*/
    sourceVoltageRange.prefix = UnitPfxMilli; /*! \todo FCON recheck*/
    sourceVoltageRange.unit = "V";/*! \todo FCON recheck*/


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
    vcCurrentGainTunerCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        vcCurrentGainTunerCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
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
    vcCurrentOffsetTunerCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        vcCurrentOffsetTunerCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
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

MessageDispatcher_Remi8_V01::~MessageDispatcher_Remi8_V01() {

}

ErrorCodes_t MessageDispatcher_Remi8_V01::setVoltageRangeOffset(uint16_t voltageRangeIdx, Measurement_t offset) {
    if (voltageRangeIdx == VoltageRange15V) {
        /*! The offset is changhable only for the internal DAC */
        return ErrorValueOutOfRange;
    }

    ErrorCodes_t ret = MessageDispatcher::setVoltageRangeOffset(voltageRangeIdx, offset);
    if (ret != Success) {
        return ret;
    }

    /*! Update range and coder for Vhold, VInit and VFinal */
    protocolVoltageRanges[ProtocolVHold].min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVHold].max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVInit].min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVInit].max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVFinal].min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVFinal].max = voltageRangesWithOffsetArray[VoltageRange500mV].max;

    DoubleCoder::CoderConfig_t doubleConfig;
    doubleConfig.initialBit = 4;
    doubleConfig.bitsNum = 12;
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVHold].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVHold].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVHold].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    doubleConfig.initialWord = 22;
    protocolVoltageCoders[ProtocolVHold] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVInit].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVInit].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVInit].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    doubleConfig.initialWord = 26;
    protocolVoltageCoders[ProtocolVInit] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVFinal].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVFinal].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVFinal].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    doubleConfig.initialWord = 27;
    protocolVoltageCoders[ProtocolVFinal] = new DoubleTwosCompCoder(doubleConfig);

    /*! Update range and coder for FSM voltage parameters */
    fsmVoltageRange.min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    fsmVoltageRange.max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    fsmGUIVoltageRange.min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    fsmGUIVoltageRange.max = voltageRangesWithOffsetArray[VoltageRange500mV].max;

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = fsmVoltageRange.step;
    doubleConfig.minValue = fsmVoltageRange.min;
    doubleConfig.maxValue = fsmVoltageRange.max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    fsmVoltageCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        fsmVoltageCoders[popIdx].resize(FsmVoltagesNum);
        doubleConfig.initialWord = 56+48*popIdx;
        for (unsigned int idx = 0; idx < FsmVoltagesNum; idx++) {
            fsmVoltageCoders[popIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            doubleConfig.initialWord++;
        }
    }

    return ret;
}

ErrorCodes_t MessageDispatcher_Remi8_V01::setRemi2Mode(bool flag) {
    uint32_t channelOnState;
    if (flag) {
        channelOnState = 0x0003; /*!< In REMI2 mode keep only channels 0 and 1 active */

    } else {
        channelOnState = 0;
         /*! After exiting REMI2 mode restore the channels that were active before  */
        for (unsigned int idx = 0; idx < currentChannelsNum; idx++) {
            channelOnState |= (channelOnStates[idx] ? (uint32_t)1 : 0) << idx;
        }
    }
    channelOnCoder->encode(channelOnState, txStatus, txModifiedStartingWord, txModifiedEndingWord);

    /*! The message is sent by MessageDispatcher::setRemi2Mode */
    return MessageDispatcher::setRemi2Mode(flag);
}

void MessageDispatcher_Remi8_V01::initializeDevice() {
    for (int idx = 0; idx < currentChannelsNum; idx++) {
        infoStruct.vComp[idx] = 0;
    }

    this->selectStimulusChannel(currentChannelsNum, true, false);
    this->setSamplingRate(defaultSamplingRateIdx, false);

    int16_t vcOffsetInt = (int16_t)ftdiEeprom->getVcOffset();
    dacExtDefault.value = dacExtRange.step*(double)vcOffsetInt;
    this->applyDacExt(dacExtDefault, false);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        this->digitalOffsetCompensation(idx, false, false);
    }

    this->selectVoltageProtocol(0);

    for (int voltageIdx = 0; voltageIdx < ProtocolVoltagesNum; voltageIdx++) {
        this->setProtocolVoltage(voltageIdx, protocolVoltageDefault[voltageIdx], false);
    }

    for (int timeIdx = 0; timeIdx < ProtocolTimesNum; timeIdx++) {
        this->setProtocolTime(timeIdx, protocolTimeDefault[timeIdx], false);
    }

    for (uint32_t flagIdx = 0; flagIdx < FsmFlagsNum; flagIdx++) {
        this->setFsmFlag(fsmStateChannelsNum, flagIdx, fsmFlagDefault[flagIdx], false);
    }

    for (uint32_t voltageIdx = 0; voltageIdx < FsmVoltagesNum; voltageIdx++) {
        this->setFsmVoltage(fsmStateChannelsNum, voltageIdx, fsmVoltageDefault[voltageIdx], false);
    }

    for (uint32_t currentIdx = 0; currentIdx < FsmCurrentsNum; currentIdx++) {
        this->setFsmThresholdCurrent(fsmStateChannelsNum, currentIdx, fsmThresholdCurrentDefault[currentIdx], false);
    }

    for (uint32_t timeIdx = 0; timeIdx < FsmTimesNum; timeIdx++) {
        this->setFsmTime(fsmStateChannelsNum, timeIdx, fsmTimeDefault[timeIdx], false);
    }

    for (uint32_t integerIdx = 0; integerIdx < FsmIntegersNum; integerIdx++) {
        this->setFsmInteger(fsmStateChannelsNum, integerIdx, (unsigned int)fsmIntegerDefault[integerIdx].value, false);
    }

    this->set1stOrderFilterTau(firstOrderFilterTauDefault);
    this->set4thOrderFilterCutoffFrequency(fourthOrderFilterCutoffFrequencyDefault);

    for (int voltageIdx = 0; voltageIdx < ConditioningVoltagesNum; voltageIdx++) {
        this->setConditioningProtocolVoltage(voltageIdx, conditioningProtocolVoltageDefault[voltageIdx], false);
    }

    for (int timeIdx = 0; timeIdx < ConditioningTimesNum; timeIdx++) {
        this->setConditioningProtocolTime(timeIdx, conditioningProtocolTimeDefault[timeIdx], false);
    }

    this->setPoreForming(false, false);

    this->enableConditioning(false, false);

    this->activateDacExtFilter(true, false);
}

bool MessageDispatcher_Remi8_V01::checkProtocolValidity(string &message) {
    bool validFlag = true;
    message = "Valid protocol";
    switch (selectedProtocol) {
    case ProtocolConstant:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolTriangular:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPk]))) {
            validFlag = false;
            message = "Vhold+Vamp\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]-selectedProtocolVoltage[ProtocolVPk]))) {
            validFlag = false;
            message = "Vhold-Vamp\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange2_10ms].includes(selectedProtocolTime[ProtocolTPe]))) {
            validFlag = false;
            message = "TPeriod\nmust be within [1,1000]ms";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolSquareWave:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]-selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold-Vpulse\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolConductance:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]+
                                                                                    selectedProtocolVoltage[ProtocolVStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Vhold+Vpulse+Vstep(N-1)\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]-selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold-Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]-
                                                                                    selectedProtocolVoltage[ProtocolVStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Vhold+Vpulse-Vstep(N-1)\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust be at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolVariableAmplitude:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]+
                                                                                    selectedProtocolVoltage[ProtocolVStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Vhold+Vpulse+Vstep(N-1)\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust be at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolVariableDuration:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRangeSigned2_15].includes(selectedProtocolTime[ProtocolTStep]))) {
            validFlag = false;
            message = "Tstep\nmust be within [-32e3, 32e3]ms";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1orMore].includes(selectedProtocolTime[ProtocolTPulse]+
                                                                               selectedProtocolTime[ProtocolTStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Tpulse+Tstep(N-1)\nmust be at least 1ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolRamp:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVFinal]))) {
            validFlag = false;
            message = "Vfinal\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVInit]))) {
            validFlag = false;
            message = "Vinit\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTRamp]))) {
            validFlag = false;
            message = "Tramp\nmust be within [1, 65e3]ms";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolCyclicVoltammetry:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVFinal]))) {
            validFlag = false;
            message = "Vfinal\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVInit]))) {
            validFlag = false;
            message = "Vinit\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTRamp]))) {
            validFlag = false;
            message = "Tramp\nmust be within [1, 65e3]ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;
    }
    return validFlag;
}

void MessageDispatcher_Remi8_V01::updateDeviceStatus(vector <bool> &fsmRunFlag, bool &poreForming, bool &communicationError) {
    for (int idx = 0; idx < fsmStateChannelsNum; idx++) {
        fsmRunFlag[idx] = (infoStruct.status & (0x0001 << idx)) == 0 ? false : true;
    }
    poreForming = (infoStruct.status & (0x0001 << 8)) == 0 ? false : true;
    communicationError = (infoStruct.status & (0x0001 << 9)) == 0 ? false : true;
}

void MessageDispatcher_Remi8_V01::updateVoltageOffsetCompensations(vector <Measurement_t> &offsets) {
    for (int idx = 0; idx < currentChannelsNum; idx++) {
        offsets[idx] = voltageOffsetCompensationGain*(double)infoStruct.vComp[idx];
    }
}
