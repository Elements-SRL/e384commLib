#ifndef EMCR384PATCHCLAMP_EL07CD_PROT_V06_FW_V02_H
#define EMCR384PATCHCLAMP_EL07CD_PROT_V06_FW_V02_H

#include "emcr384patchclamp_el07cd_prot_v06_fw_v01.h"

class Emcr384PatchClamp_EL07c_prot_v06_fw_v02 : public Emcr384PatchClamp_EL07c_prot_v06_fw_v01 {
public:
    Emcr384PatchClamp_EL07c_prot_v06_fw_v02(std::string di);
};

class Emcr384PatchClamp_EL07d_prot_v06_fw_v02 : public Emcr384PatchClamp_EL07c_prot_v06_fw_v02 {
public:
    Emcr384PatchClamp_EL07d_prot_v06_fw_v02(std::string di);
};

#endif // EMCR384PATCHCLAMP_EL07CD_PROT_V06_FW_V02_H
