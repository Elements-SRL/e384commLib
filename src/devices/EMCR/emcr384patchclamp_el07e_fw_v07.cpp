#include "emcr384patchclamp_el07e_fw_v07.h"

Emcr384PatchClamp_EL07e_fw_v07::Emcr384PatchClamp_EL07e_fw_v07(std::string di) :
    Emcr384PatchClamp_EL07e_fw_v06(di) {

    rxWordOffsets[RxMessageCalEeepromDataLoad] = rxWordOffsets[RxMessageDoubleSyncStatus] + rxWordLengths[RxMessageDoubleSyncStatus];
    rxWordLengths[RxMessageCalEeepromDataLoad] = 1024;

    txDataWords = 6184;
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    boolConfig.initialWord = 252;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    calibrationRamAddressCoder = new BoolArrayCoder(boolConfig);

    boolConfig.initialWord = 253;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 8;
    calibrationRamValueCoder = new BoolArrayCoder(boolConfig);

    boolConfig.initialWord = 254;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 24;
    calibrationRamSelectorCoder = new BoolOneHotCoder(boolConfig);

    /*! Rs correction offset calibration */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    calibRsCorrOffsetDacCoders.resize(VCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 5800;
        doubleConfig.resolution = calibVcVoltageOffsetRanges[0].step;
        doubleConfig.minValue = calibVcVoltageOffsetRanges[0].min;
        doubleConfig.maxValue = calibVcVoltageOffsetRanges[0].max;
        calibRsCorrOffsetDacCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            calibRsCorrOffsetDacCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibRsCorrOffsetDacCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! Default status */
    txStatus.resize(txDataWords);
    txStatus.encodingWords[2] |= 0x8000;
}

ErrorCodes_t Emcr384PatchClamp_EL07e_fw_v07::initializeHW() {
    Emcr384PatchClamp_EL07e_fw_v05::initializeHW();
    this->setDebugBit(4, 11, true, false);
    this->stackOutgoingMessage(txStatus, {TxTriggerReadCalEeprom, ResetIndifferent});
    return Success;
}