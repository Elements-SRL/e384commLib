#ifndef EMCR384PATCHCLAMP_EL07C_PROT_V07_FW_V03_H
#define EMCR384PATCHCLAMP_EL07C_PROT_V07_FW_V03_H

#include "emcr384patchclamp_el07cd_prot_v06_fw_v02.h"

class Emcr384PatchClamp_EL07c_prot_v07_fw_v03 : public Emcr384PatchClamp_EL07c_prot_v06_fw_v02 {
public:
    Emcr384PatchClamp_EL07c_prot_v07_fw_v03(std::string di);

    ErrorCodes_t setCoolingFansSpeed(Measurement_t speed, bool applyFlag) override;
    ErrorCodes_t getCoolingFansSpeedRange(RangedMeasurement_t &range) override;

protected:
    enum FanTrimmerRanges {
        FanTrimmerOff,
        FanTrimmerSlow,
        FanTrimmerFast,
        FanTrimmerRangesNum
    };

    Measurement_t fanV2R(Measurement_t V);
    Measurement_t fanW2V(Measurement_t W);
    Measurement_t fanRT2W(Measurement_t RT);

    MultiCoder * fanTrimmerCoder = nullptr;
    std::vector <RangedMeasurement_t> fanTrimmerRanges;
    const Measurement_t fanTrimmerRf = {10.0, UnitPfxKilo, "Ohm"};
    const Measurement_t fanTrimmerVStep = {0.6, UnitPfxNone, "V"};
    const Measurement_t fanTrimmerVMax = {12.0, UnitPfxNone, "V"};
    const Measurement_t fanTrimmerWMax = {5900.0, UnitPfxNone, "rpm"};
    const Measurement_t fanTrimmerRTMin = {0.3501, UnitPfxNone, "K/W"};
};

class Emcr384PatchClamp_EL07d_prot_v07_fw_v03 : public Emcr384PatchClamp_EL07c_prot_v07_fw_v03 {
public:
    Emcr384PatchClamp_EL07d_prot_v07_fw_v03(std::string di);
};

#endif // EMCR384PATCHCLAMP_EL07C_PROT_V07_FW_V03_H
