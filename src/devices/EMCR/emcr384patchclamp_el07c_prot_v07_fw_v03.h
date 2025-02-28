#ifndef EMCR384PATCHCLAMP_EL07C_PROT_V07_FW_V03_H
#define EMCR384PATCHCLAMP_EL07C_PROT_V07_FW_V03_H

#include "emcr384patchclamp_el07cd_prot_v06_fw_v02.h"

class Emcr384PatchClamp_EL07c_prot_v07_fw_v03 : public Emcr384PatchClamp_EL07c_prot_v06_fw_v02 {
public:
    Emcr384PatchClamp_EL07c_prot_v07_fw_v03(std::string di);

protected:
    enum CustomDoubles {
        FanTrimmer,
        CustomDoublesNum
    };
};

class Emcr384PatchClamp_EL07d_prot_v07_fw_v03 : public Emcr384PatchClamp_EL07c_prot_v07_fw_v03 {
public:
    Emcr384PatchClamp_EL07d_prot_v07_fw_v03(std::string di);
};

#endif // EMCR384PATCHCLAMP_EL07C_PROT_V07_FW_V03_H
