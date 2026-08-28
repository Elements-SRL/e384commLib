#include "emcr384patchclamp_prot_v04_fw_v03.h"

Emcr384PatchClamp_prot_v04_fw_v03::Emcr384PatchClamp_prot_v04_fw_v03(std::string di):Emcr384PatchClamp_prot_v04_fw_v04(di) {
    fwSize_B = 5333800;
    motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;
}
