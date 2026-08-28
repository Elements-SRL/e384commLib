#include "emcr192blm_el03c_mb03_mez05_fw_v07.h"

Emcr192Blm_EL03c_Mb03_Mez05_fw_v07::Emcr192Blm_EL03c_Mb03_Mez05_fw_v07(std::string di) :
    Emcr192Blm_EL03c_Mb03_Mez04_fw_v06(di) {

    rxWordOffsets[RxMessageOnTime] = rxWordOffsets[RxMessageTemperature] + rxWordLengths[RxMessageTemperature];
    rxWordLengths[RxMessageOnTime] = 2;

    onTimeRange.step = 0.25;
    onTimeRange.max = onTimeRange.step*LUINT32_MAX;
    onTimeRange.prefix = UnitPfxNone;
    onTimeRange.unit = "s";

    txStatus.encodingWords[9] = 0x0A;
}
