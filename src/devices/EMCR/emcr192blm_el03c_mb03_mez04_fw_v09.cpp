#include "emcr192blm_el03c_mb03_mez04_fw_v09.h"

Emcr192Blm_EL03c_Mb03_Mez04_fw_v09::Emcr192Blm_EL03c_Mb03_Mez04_fw_v09(std::string di) :
    Emcr192Blm_EL03c_Mb03_Mez04_fw_v08(di) {

    rxWordOffsets[RxMessageSyncStatus] = rxWordOffsets[RxMessageTemperature] + rxWordLengths[RxMessageTemperature] + 2;
    rxWordLengths[RxMessageSyncStatus] = 2;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    overHeatingModeCoder = new BoolArrayCoder(boolConfig);
}
