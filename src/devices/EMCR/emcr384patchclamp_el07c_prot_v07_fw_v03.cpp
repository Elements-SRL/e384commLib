#include "emcr384patchclamp_el07c_prot_v07_fw_v03.h"

Emcr384PatchClamp_EL07c_prot_v07_fw_v03::Emcr384PatchClamp_EL07c_prot_v07_fw_v03(std::string di) :
    Emcr384PatchClamp_EL07c_prot_v06_fw_v02(di) {

    fwName = "384PatchClamp_EL07c_V03.1.bit";

    // fwSize_B = 6313140;
    // motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;

    temperatureChannelsRanges[TemperatureSensor0].step = 0.25;
    temperatureChannelsRanges[TemperatureSensor0].min = -8192.0;
    temperatureChannelsRanges[TemperatureSensor0].max = temperatureChannelsRanges[TemperatureSensor0].min+temperatureChannelsRanges[TemperatureSensor0].step*USHORT_MAX;
    temperatureChannelsRanges[TemperatureSensor0].prefix = UnitPfxNone;
    temperatureChannelsRanges[TemperatureSensor0].unit = "°C";
    temperatureChannelsRanges[TemperatureSensor1].step = 0.25;
    temperatureChannelsRanges[TemperatureSensor1].min = -8192.0;
    temperatureChannelsRanges[TemperatureSensor1].max = temperatureChannelsRanges[TemperatureSensor1].min+temperatureChannelsRanges[TemperatureSensor1].step*USHORT_MAX;
    temperatureChannelsRanges[TemperatureSensor1].prefix = UnitPfxNone;
    temperatureChannelsRanges[TemperatureSensor1].unit = "°C";

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    DoubleCoder::CoderConfig_t doubleConfig;

    customDoublesNum = CustomDoublesNum;
    customDoublesNames.resize(customDoublesNum);
    customDoublesNames[FanTrimmer] = "FAN trimmer";
    customDoublesRanges.resize(customDoublesNum);
    customDoublesRanges[FanTrimmer].step = 50.0/256.0;
    customDoublesRanges[FanTrimmer].min = 60.0e-3;
    customDoublesRanges[FanTrimmer].max = customDoublesRanges[FanTrimmer].min+customDoublesRanges[FanTrimmer].step*USHORT_MAX;
    customDoublesRanges[FanTrimmer].prefix = UnitPfxKilo;
    customDoublesRanges[FanTrimmer].unit = "Ohm";
    doubleConfig.initialWord = 260;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.minValue = customDoublesRanges[FanTrimmer].min;
    doubleConfig.maxValue = customDoublesRanges[FanTrimmer].max;
    doubleConfig.resolution = customDoublesRanges[FanTrimmer].step;
    customDoublesCoders.resize(customDoublesNum);
    customDoublesCoders[FanTrimmer] = new DoubleOffsetBinaryCoder(doubleConfig);
    customDoublesDefault.resize(customDoublesNum);
    customDoublesDefault[FanTrimmer] = customDoublesRanges[FanTrimmer].max;
    coders.push_back(customDoublesCoders[FanTrimmer]);

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[2] = 0x0AAA; // fans on
    for (int idx = 132; idx < 156; idx++) {
        txStatus[idx] = 0x1111; // GR_EN active
    }
    for (int idx = 1312; idx < 1504; idx++) {
        txStatus[idx] = 0x4040; // Set 0 of secondary DAC
    }
    for (int idx = 4384; idx < 4480; idx++) {
        txStatus[idx] = 0x1111; // rs bw avoid configuration with all zeros
    }
}

Emcr384PatchClamp_EL07d_prot_v07_fw_v03::Emcr384PatchClamp_EL07d_prot_v07_fw_v03(std::string di) :
    Emcr384PatchClamp_EL07c_prot_v07_fw_v03(di) {

    fwName = "384PatchClamp_EL07d_V03.1.bit";

    // fwSize_B = 5506748;
    // motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;
}
