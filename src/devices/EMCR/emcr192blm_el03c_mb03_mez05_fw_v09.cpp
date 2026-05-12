#include "emcr192blm_el03c_mb03_mez05_fw_v09.h"

Emcr192Blm_EL03c_Mb03_Mez05_fw_v09::Emcr192Blm_EL03c_Mb03_Mez05_fw_v09(std::string di) :
    Emcr192Blm_EL03c_Mb03_Mez05_fw_v07(di) {

    rxWordOffsets[RxMessageSyncStatus] = rxWordOffsets[RxMessageOnTime] + rxWordLengths[RxMessageOnTime];
    rxWordLengths[RxMessageSyncStatus] = 2;

    rxWordOffsets[RxMessageSpiDataLoad] = rxWordOffsets[RxMessageSyncStatus] + rxWordLengths[RxMessageSyncStatus];
    rxWordLengths[RxMessageSpiDataLoad] = 2;

    txDataWords = 3020;
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    overHeatingModeCoder = new BoolArrayCoder(boolConfig);

    boolConfig.initialWord = 3017;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 8;
    spiCommandCoder = new BoolArrayCoder(boolConfig);

    boolConfig.initialWord = 3018;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 32;
    spiDataLoadCoder = new BoolArrayCoder(boolConfig);

    txStatus.resize(txMaxWords);
}
