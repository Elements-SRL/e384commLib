#include "emcr384patchclamp_prot_v04_fw_v05.h"

Emcr384PatchClamp_prot_v04_fw_v05::Emcr384PatchClamp_prot_v04_fw_v05(std::string di) :
    Emcr384PatchClamp_prot_v04_fw_v04(di) {

    fwSize_B = 6281776;
    motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;

    boolConfig.initialWord = 4;
    boolConfig.initialBit = 14;
    boolConfig.bitsNum = 1;
    protocolResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolResetCoder);
}
