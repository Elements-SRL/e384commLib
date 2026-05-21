#include "emcr384patchclamp_el07e_fw_v06.h"

Emcr384PatchClamp_EL07e_fw_v06::Emcr384PatchClamp_EL07e_fw_v06(std::string di) :
    Emcr384PatchClamp_EL07e_fw_v05(di) {

    rxWordOffsets[RxMessageDoubleSyncStatus] = rxWordOffsets[RxMessageTemperature] + rxWordLengths[RxMessageTemperature];
    rxWordLengths[RxMessageDoubleSyncStatus] = 4;
}
