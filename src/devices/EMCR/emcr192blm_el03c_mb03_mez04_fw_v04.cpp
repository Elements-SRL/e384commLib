#include "emcr192blm_el03c_mb03_mez04_fw_v04.h"

Emcr192Blm_EL03c_Mb03_Mez04_fw_v04::Emcr192Blm_EL03c_Mb03_Mez04_fw_v04(std::string di) :
    Emcr192Blm_EL03c_Mb03_Mez04_fw_v03(di) {

    fwName = "192BLM_EL03c_V04.bit";

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    boolConfig.initialWord = 4;
    boolConfig.initialBit = 15;
    boolConfig.bitsNum = 1;
    liquidJunctionResetCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        liquidJunctionResetCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(liquidJunctionResetCoders[idx]);
    }

    // boolConfig.initialWord = 1852;
    // boolConfig.initialBit = 0;
    // boolConfig.bitsNum = 1;
    // liquidJunctionResetCoders.resize(currentChannelsNum);
    // for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
    //     liquidJunctionResetCoders[idx] = new BoolArrayCoder(boolConfig);
    //     coders.push_back(liquidJunctionResetCoders[idx]);
    //     boolConfig.initialBit++;
    //     if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
    //         boolConfig.initialBit = 0;
    //         boolConfig.initialWord++;
    //     }
    // }

    boolConfig.initialWord = 1866;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    liquidJunctionAutoStopCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        liquidJunctionAutoStopCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(liquidJunctionAutoStopCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    currentTrackingCoders.resize(VCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2044;
        doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
        currentTrackingCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            currentTrackingCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(currentTrackingCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }
}
