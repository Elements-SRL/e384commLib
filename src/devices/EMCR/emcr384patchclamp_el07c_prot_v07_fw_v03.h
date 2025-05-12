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
    const Measurement_t fanTrimmerRf = {0.06, UnitPfxKilo, "Ohm"};
    const Measurement_t fanTrimmerVStep = {0.6, UnitPfxNone, "V"};
    const Measurement_t fanTrimmerVMax = {12.0, UnitPfxNone, "V"};
    const Measurement_t fanTrimmerWMax = {6300.0, UnitPfxNone, "rpm"};
    const Measurement_t fanTrimmerRTMin = {1.0, UnitPfxNone, "KpW"};
};

class Emcr384PatchClamp_EL07d_prot_v07_fw_v03 : public Emcr384PatchClamp_EL07c_prot_v07_fw_v03 {
public:
    Emcr384PatchClamp_EL07d_prot_v07_fw_v03(std::string di);
};

#endif // EMCR384PATCHCLAMP_EL07C_PROT_V07_FW_V03_H
