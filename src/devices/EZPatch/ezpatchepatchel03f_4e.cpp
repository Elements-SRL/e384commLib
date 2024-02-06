#include "ezpatchepatchel03f_4e.h"

EZPatchePatchEL03F_4E_PCBV03_V04::EZPatchePatchEL03F_4E_PCBV03_V04(std::string di) :
    EZPatchePatchEL03F_4D_PCBV03_V04(di) {

    deviceName = "ePatchEl03F_4E";

    notificationTag = deviceName + "_PCBV03_V04";
}

EZPatchePatchEL03F_4E_PCBV03_V03::EZPatchePatchEL03F_4E_PCBV03_V03(std::string di) :
    EZPatchePatchEL03F_4D_PCBV03_V03(di) {

    deviceName = "ePatchEl03F_4E";

    notificationTag = deviceName + "_PCBV03_V03";
}

EZPatchePatchEL03F_4E_PCBV03_V02::EZPatchePatchEL03F_4E_PCBV03_V02(std::string di) :
    EZPatchePatchEL03F_4D_PCBV03_V02(di) {

    deviceName = "ePatchEl03F_4E";

    notificationTag = deviceName + "_PCBV03_V02";
}

EZPatchePatchEL03F_4E_PCBV02_V04::EZPatchePatchEL03F_4E_PCBV02_V04(std::string di) :
    EZPatchePatchEL03F_4D_PCBV02_V04(di) {

    deviceName = "ePatchEl03F_4E";

    notificationTag = deviceName + "_PCBV02_V04";
}

EZPatchePatchEL03F_4E_PCBV02_V03::EZPatchePatchEL03F_4E_PCBV02_V03(std::string di) :
    EZPatchePatchEL03F_4D_PCBV02_V03(di) {

    deviceName = "ePatchEl03F_4E";

    notificationTag = deviceName + "_PCBV02_V03";
}

EZPatchePatchEL03F_4E_PCBV02_V02::EZPatchePatchEL03F_4E_PCBV02_V02(std::string di) :
    EZPatchePatchEL03F_4D_PCBV02_V02(di) {

    deviceName = "ePatchEl03F_4E";

    notificationTag = deviceName + "_PCBV02_V02";
}

EZPatchePatchEL03F_4E_PCBV02_V01::EZPatchePatchEL03F_4E_PCBV02_V01(std::string di) :
    EZPatchePatchEL03F_4D_PCBV02_V01(di) {

    deviceName = "ePatchEl03F_4E";

    notificationTag = deviceName + "_PCBV02_V01";
}

EZPatchePatchEL03F_4E_PCBV02_V00::EZPatchePatchEL03F_4E_PCBV02_V00(std::string di) :
    EZPatchePatchEL03F_4D_PCBV02_V01(di) {

    deviceName = "ePatchEl03F_4E";

    upgradeNotes += "- Implemented programmable digital output\n";
    notificationTag = deviceName + "_PCBV02_V00";

    maxDigitalTriggerOutputEvents = 1;

    /*! Moreover this version uses the voltage measured from the CC front-end all the time. This has been changed in following versions */

    availableVoltageSourcesIdxs.VoltageFromVoltageClamp = ChannelSourceVoltageFromCurrentClamp; /*! We'll get voltage from cc front-end anyway */

    availableVoltageSourcesIdxsArray.resize(1);
    availableVoltageSourcesIdxsArray[0] = ChannelSourceVoltageFromCurrentClamp;
}

EZPatchePatchEL03F_4E_PCBV02_V00::~EZPatchePatchEL03F_4E_PCBV02_V00() {

}

void EZPatchePatchEL03F_4E_PCBV02_V00::selectChannelsResolutions() {
    if (selectedCurrentSourceIdx == ChannelSourceCurrentFromVoltageClamp) {
        rawDataFilterCurrentFlag = true;

    } else if (selectedCurrentSourceIdx == ChannelSourceCurrentFromCurrentClamp) {
        rawDataFilterCurrentFlag = false;
    }

    rawDataFilterVoltageFlag = true;

    this->selectVoltageOffsetResolution();
    /*! voltageResolution does not depend on selected source cause we want to get it from current clamp front end */

    this->computeRawDataFilterCoefficients();
}

void EZPatchePatchEL03F_4E_PCBV02_V00::selectVoltageOffsetResolution() {
    Measurement_t correctedValue;
    correctedValue.value = voltageOffsetCorrected;
    correctedValue.prefix = liquidJunctionPrefix;
    correctedValue.convertValue(voltageRange.prefix);
    voltageOffsetCorrection = correctedValue.value;
}
