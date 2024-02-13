#include "ezpatchepatchel03f_4f.h"

EZPatchePatchEL03F_4F_PCBV01_AnalogOut_V01::EZPatchePatchEL03F_4F_PCBV01_AnalogOut_V01(std::string di) :
    EZPatchePatchEL03F_4D_PCBV03_V03(di) {

    deviceName = "ePatchEl03F_4F";

    notificationTag = deviceName + "_AnalogOut_V01";

    analogOutImplementedFlag = true;
    analogOutWord = 1;
    analogOutByte = 0x8000;

    switchesNames[1][1] = "Calib_EN";
    switchesNames[1][8] = "CC_EN";
    switchesNames[1][15] = "AnalogOut";
}

EZPatchePatchEL03F_4F_PCBV03_V04::EZPatchePatchEL03F_4F_PCBV03_V04(std::string di) :
    EZPatchePatchEL03F_4D_PCBV03_V04(di) {

    deviceName = "ePatchEl03F_4F";

    notificationTag = deviceName + "_V04";

    switchesNames[1][1] = "Calib_EN";
    switchesNames[1][8] = "CC_EN";
}

EZPatchePatchEL03F_4F_PCBV02_V04::EZPatchePatchEL03F_4F_PCBV02_V04(std::string di) :
    EZPatchePatchEL03F_4D_PCBV02_V04(di) {

    deviceName = "ePatchEl03F_4F";

    notificationTag = deviceName + "_V04";

    switchesNames[1][1] = "Calib_EN";
    switchesNames[1][8] = "CC_EN";
}

EZPatchePatchEL03F_4F_PCBV03_V03::EZPatchePatchEL03F_4F_PCBV03_V03(std::string di) :
    EZPatchePatchEL03F_4D_PCBV03_V03(di) {

    deviceName = "ePatchEl03F_4F";

    notificationTag = deviceName + "_V03";

    switchesNames[1][1] = "Calib_EN";
    switchesNames[1][8] = "CC_EN";
}
