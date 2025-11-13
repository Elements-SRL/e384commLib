#include "emcr192blm_el03c_mb02_mez03_fw_v04.h"

Emcr192Blm_EL03c_Mb02_Mez03_fw_v04::Emcr192Blm_EL03c_Mb02_Mez03_fw_v04(std::string di) :
    Emcr192Blm_EL03c_Mb02_Mez03_fw_v03(di) {

    fwName = "192BLM_EL03c_V04.bit";

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    boolConfig.initialWord = 4;
    boolConfig.initialBit = 15;
    boolConfig.bitsNum = 1;
    liquidJunctionResetCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        liquidJunctionResetCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(liquidJunctionResetCoders[idx]);
    }
}
