#ifndef EZPATCHEPATCHEL03F_4E_H
#define EZPATCHEPATCHEL03F_4E_H

#include "ezpatchepatchel03f_4d.h"

class EZPatchePatchEL03F_4E_PCBV03_V04 : public EZPatchePatchEL03F_4D_PCBV03_V04 {
public:
    EZPatchePatchEL03F_4E_PCBV03_V04(std::string di);
};

class EZPatchePatchEL03F_4E_PCBV03_V03 : public EZPatchePatchEL03F_4D_PCBV03_V03 {
public:
    EZPatchePatchEL03F_4E_PCBV03_V03(std::string di);
};

class EZPatchePatchEL03F_4E_PCBV03_V02 : public EZPatchePatchEL03F_4D_PCBV03_V02 {
public:
    EZPatchePatchEL03F_4E_PCBV03_V02(std::string di);
};

class EZPatchePatchEL03F_4E_PCBV02_V04 : public EZPatchePatchEL03F_4D_PCBV02_V04 {
public:
    EZPatchePatchEL03F_4E_PCBV02_V04(std::string di);
};

class EZPatchePatchEL03F_4E_PCBV02_V03 : public EZPatchePatchEL03F_4D_PCBV02_V03 {
public:
    EZPatchePatchEL03F_4E_PCBV02_V03(std::string di);
};

class EZPatchePatchEL03F_4E_PCBV02_V02 : public EZPatchePatchEL03F_4D_PCBV02_V02 {
public:
    EZPatchePatchEL03F_4E_PCBV02_V02(std::string di);
};

class EZPatchePatchEL03F_4E_PCBV02_V01 : public EZPatchePatchEL03F_4D_PCBV02_V01 {
public:
    EZPatchePatchEL03F_4E_PCBV02_V01(std::string di);
};

class EZPatchePatchEL03F_4E_PCBV02_V00 : public EZPatchePatchEL03F_4D_PCBV02_V01 {
public:
    EZPatchePatchEL03F_4E_PCBV02_V00(std::string di);

protected:
    void selectChannelsResolutions() override;
    void selectVoltageOffsetResolution() override;
};

#endif // EZPATCHEPATCHEL03F_4E_H
