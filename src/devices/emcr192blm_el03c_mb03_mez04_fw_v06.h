#ifndef EMCR192BLM_EL_3C_MB_3_MEZ_4_FW_V_6_H
#define EMCR192BLM_EL_3C_MB_3_MEZ_4_FW_V_6_H

#include "emcr192blm_el03c_mb03_mez04_fw_v05.h"

class Emcr192Blm_EL03c_Mb03_Mez04_fw_v06 : public Emcr192Blm_EL03c_Mb03_Mez04_fw_v05 {
public:
    Emcr192Blm_EL03c_Mb03_Mez04_fw_v06(std::string di);

protected:
    enum TemperatureChannels {
        TemperatureSensor0,
        TemperatureSensor1,
        TemperatureChannelsNum
    };
};

#endif // EMCR192BLM_EL_3C_MB_3_MEZ_4_FW_V_6_H
