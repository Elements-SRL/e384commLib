#include "emcr384patchclamp_el07e_fw_v07.h"

Emcr384PatchClamp_EL07e_fw_v07::Emcr384PatchClamp_EL07e_fw_v07(std::string di) :
    Emcr384PatchClamp_EL07e_fw_v06(di) {

    rxWordOffsets[RxMessageCalEeepromDataLoad] = rxWordOffsets[RxMessageDoubleSyncStatus] + rxWordLengths[RxMessageDoubleSyncStatus];
    rxWordLengths[RxMessageCalEeepromDataLoad] = 1024;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

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

    /*! Default status */
    txStatus.encodingWords[2] |= 0x8000;
}
