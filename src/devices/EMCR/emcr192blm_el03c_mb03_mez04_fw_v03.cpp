#include "emcr192blm_el03c_mb03_mez04_fw_v03.h"

Emcr192Blm_EL03c_Mb03_Mez04_fw_v03::Emcr192Blm_EL03c_Mb03_Mez04_fw_v03(std::string di) :
    Emcr192Blm_EL03c_Mb03_Mez04_fw_v02(di) {

    fwName = "192BLM_EL03c_V03.bit";

    motherboardBootTime_s = 30;

    defaultVInitRampTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultVFinalRampTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultTRampTuner = {0.0, UnitPfxNone, "s"};

    uint32_t vRampTunerCodersOffset = 316;
    uint32_t vRampTunerCodersSize = 8;

    txDataWords = 3016;
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    /*! V holding tuner */
    vHoldTunerCoders.clear();

    /*! V Ramp tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vInitRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vInitRampTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vInitRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vInitRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vFinalRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset+1;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vFinalRampTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vFinalRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vFinalRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    doubleConfig.initialWord = vRampTunerCodersOffset+2;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = positiveProtocolTimeRange.step;
    doubleConfig.minValue = positiveProtocolTimeRange.min;
    doubleConfig.maxValue = positiveProtocolTimeRange.max;
    tRampTunerCoders.resize(currentChannelsNum);

    for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        tRampTunerCoders[channelIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(tRampTunerCoders[channelIdx]);
        doubleConfig.initialWord += vRampTunerCodersSize;
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    quotRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset+4;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step/positiveProtocolTimeRange.step;
        doubleConfig.minValue = LINT32_MIN*doubleConfig.resolution;
        doubleConfig.maxValue = LINT32_MAX*doubleConfig.resolution;
        quotRampTunerCoders[rangeIdx].resize(currentChannelsNum);

        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            quotRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(quotRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    remRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset+6;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = LINT32_MIN*doubleConfig.resolution;
        doubleConfig.maxValue = LINT32_MAX*doubleConfig.resolution;
        remRampTunerCoders[rangeIdx].resize(currentChannelsNum);

        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            remRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(remRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    /*! Activate ramp tuners */
    boolConfig.initialWord = 2236;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    activateRampTunerCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        activateRampTunerCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(activateRampTunerCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! liquid junction voltage */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 10;
    liquidJunctionVoltageCoders.resize(liquidJunctionRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < liquidJunctionRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2044;
        doubleConfig.resolution = liquidJunctionRangesArray[rangeIdx].step;
        doubleConfig.minValue = liquidJunctionRangesArray[rangeIdx].min;
        doubleConfig.maxValue = liquidJunctionRangesArray[rangeIdx].max;
        liquidJunctionVoltageCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            liquidJunctionVoltageCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(liquidJunctionVoltageCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! DAC gain e offset */
    /*! VC Voltage gain */
    doubleConfig.initialWord = 2248;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibVcVoltageGainRange.step;
    doubleConfig.minValue = calibVcVoltageGainRange.min;
    doubleConfig.maxValue = calibVcVoltageGainRange.max;
    calibVcVoltageGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcVoltageGainCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(calibVcVoltageGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! VC Voltage offset */
    calibVcVoltageOffsetCoders.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2440;
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

    /*! ADC gain e offset */
    /*! VC current gain */
    doubleConfig.initialWord = 2632;
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
        doubleConfig.initialWord = 2824;
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
    txStatus.encodingWords[0] = 0x4000;
    txStatus.encodingWords[7] = 0x00FF; // fans max speed
    for (int c = 36; c < 48; c++) {
        txStatus.encodingWords[c] = 0xFFFF; // VC_int on
    }
    for (int c = 2044; c < 2236; c++) {
        txStatus.encodingWords[c] = 0x200; // ODAC zero
    }
}

ErrorCodes_t Emcr192Blm_EL03c_Mb03_Mez04_fw_v03::initializeHW() {
    std::this_thread::sleep_for(std::chrono::seconds(motherboardBootTime_s));

    this->sendCommands();

    return Success;
}
