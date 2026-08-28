#include "emcr192blm_el03c_mb03_mez04_fw_v08.h"

Emcr192Blm_EL03c_Mb03_Mez04_fw_v08::Emcr192Blm_EL03c_Mb03_Mez04_fw_v08(std::string di) :
    Emcr192Blm_EL03c_Mb03_Mez04_fw_v06(di) {

    rxWordOffsets[RxMessageSyncStatus] = rxWordOffsets[RxMessageTemperature] + rxWordLengths[RxMessageTemperature];
    rxWordLengths[RxMessageSyncStatus] = 2;
}
