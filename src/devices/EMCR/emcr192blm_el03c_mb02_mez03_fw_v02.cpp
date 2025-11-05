#include "emcr192blm_el03c_mb02_mez03_fw_v02.h"

Emcr192Blm_EL03c_Mb02_Mez03_fw_v02::Emcr192Blm_EL03c_Mb02_Mez03_fw_v02(std::string di) :
    Emcr192Blm_EL03c_Mb02_Mez03_fw_v01(di) {

    fwName = "192BLM_EL03c_V02.bit";

    // fwSize_B = 6313140;
    // motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;

    protocolMaxItemsNum = 20;
    protocolWordOffset = 72;
    protocolItemsWordsNum = 12;

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

    voltageProtocolSinImplemented = true;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

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

    /*! Default status */
    txStatus.encodingWords[0] = 0x4000;
}
