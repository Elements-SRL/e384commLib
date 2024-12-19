#include "emcr384patchclamp_el07cd_prot_v06_fw_v02.h"

Emcr384PatchClamp_EL07c_prot_v06_fw_v02::Emcr384PatchClamp_EL07c_prot_v06_fw_v02(std::string di) :
    Emcr384PatchClamp_EL07c_prot_v06_fw_v01(di) {

    fwName = "384PatchClamp_EL07c_V02.0.bit";

    fwSize_B = 6313140;
    motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;
}

Emcr384PatchClamp_EL07d_prot_v06_fw_v02::Emcr384PatchClamp_EL07d_prot_v06_fw_v02(std::string di) :
    Emcr384PatchClamp_EL07c_prot_v06_fw_v02(di) {

    fwName = "384PatchClamp_EL07d_V02.0.bit";

    fwSize_B = 5506748;
    motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;
}
