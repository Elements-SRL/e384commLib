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
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;
    MultiCoder::CoderConfig_t multiConfig;

    /*! T control */
    int fanTrimmerRangesNum = FanTrimmerRangesNum;

    double fanTrimmerLevels = 256.0;
    double Rb = 140.0;
    double Rc = 0.06;
    double Rp = 499.0;
    double Rm = 50.0;

    fanTrimmerRanges.resize(fanTrimmerRangesNum);
    fanTrimmerRanges[FanTrimmerOff].step = 1.0;
    fanTrimmerRanges[FanTrimmerOff].min = 0.0;
    fanTrimmerRanges[FanTrimmerOff].max = 0.0;
    fanTrimmerRanges[FanTrimmerOff].prefix = UnitPfxKilo;
    fanTrimmerRanges[FanTrimmerOff].unit = "Ohm";
    fanTrimmerRanges[FanTrimmerFast].step = Rm/fanTrimmerLevels;
    fanTrimmerRanges[FanTrimmerFast].min = Rb+Rc;
    fanTrimmerRanges[FanTrimmerFast].max = fanTrimmerRanges[FanTrimmerFast].min+(fanTrimmerLevels-1.0)*fanTrimmerRanges[FanTrimmerFast].step;
    fanTrimmerRanges[FanTrimmerFast].prefix = UnitPfxKilo;
    fanTrimmerRanges[FanTrimmerFast].unit = "Ohm";
    fanTrimmerRanges[FanTrimmerSlow].min = fanTrimmerRanges[FanTrimmerFast].min*Rp/(fanTrimmerRanges[FanTrimmerFast].min+Rp);
    fanTrimmerRanges[FanTrimmerSlow].max = fanTrimmerRanges[FanTrimmerFast].max*Rp/(fanTrimmerRanges[FanTrimmerFast].max+Rp);
    fanTrimmerRanges[FanTrimmerSlow].step = (fanTrimmerRanges[FanTrimmerSlow].max-fanTrimmerRanges[FanTrimmerSlow].min)/fanTrimmerLevels;
    fanTrimmerRanges[FanTrimmerSlow].prefix = UnitPfxKilo;
    fanTrimmerRanges[FanTrimmerSlow].unit = "Ohm";

    boolConfig.initialWord = 2;
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 7;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    multiConfig.doubleCoderVector.resize(fanTrimmerRangesNum);
    multiConfig.thresholdVector.resize(fanTrimmerRangesNum-1);

    multiConfig.boolCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x3);
    static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x2);
    static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x0);
    coders.push_back(multiConfig.boolCoder);

    for (uint32_t rangeIdx = 0; rangeIdx < fanTrimmerRangesNum; rangeIdx++) {
        doubleConfig.minValue = fanTrimmerRanges[rangeIdx].min;
        doubleConfig.maxValue = fanTrimmerRanges[rangeIdx].max;
        doubleConfig.resolution = fanTrimmerRanges[rangeIdx].step;
        multiConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(multiConfig.doubleCoderVector[rangeIdx]);
        if (rangeIdx > 0) {
            multiConfig.thresholdVector[rangeIdx-1] = (fanTrimmerRanges[rangeIdx].min + fanTrimmerRanges[rangeIdx-1].max)*0.5;
        }
    }
    fanTrimmerCoder = new MultiCoder(multiConfig);
    coders.push_back(fanTrimmerCoder);

    /*! Default status */
    txStatus.init(txDataWords);
    for (int idx = 132; idx < 156; idx++) {
        txStatus.encodingWords[idx] = 0x1111; // GR_EN active
    }
    for (int idx = 1312; idx < 1504; idx++) {
        txStatus.encodingWords[idx] = 0x4040; // Set 0 of secondary DAC
    }
    for (int idx = 4384; idx < 4480; idx++) {
        txStatus.encodingWords[idx] = 0x1111; // rs bw avoid configuration with all zeros
    }
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v07_fw_v03::setCoolingFansSpeed(Measurement_t speed, bool applyFlag) {
    fanTrimmerCoder->encode(fanV2R(fanW2V(speed)).value, txStatus);
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v07_fw_v03::getCoolingFansSpeedRange(RangedMeasurement_t &range) {
    range = {0.0, fanTrimmerWMax.value, 10.0, fanTrimmerWMax.prefix, fanTrimmerWMax.unit};
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v07_fw_v03::setTemperatureControl(Measurement_t temperature, bool enabled) {
    if (enabled == tControlEnabled) {
        return Success;
    }
    temperatureSet = temperature;
    temperatureSet.convertValue(UnitPfxNone);

    tControlEnabled = enabled;
    ie = 0.0;
    then = std::chrono::steady_clock::now();
    return Success;
}

void Emcr384PatchClamp_EL07c_prot_v07_fw_v03::processTemperatureData(std::vector <Measurement_t> temperaturesRead) {
    /*! \todo FCON tutte le conversioni in real time potrebbero essere evitate assicurandosi nel costruttore che temperatureSet abbia la stessa unità dei range di corrente, ma credo sia meno compreonsibile perchè va */
    temperaturesRead[0].convertValue(UnitPfxNone);
    double temperatureIntMeas = temperaturesRead[0].value;
    // temperaturesRead[1].convertValue(UnitPfxNone);
    // double temperatureExtMeas = temperaturesRead[1].value;
    if (tControlEnabled) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - then).count();
        if (elapsed < 1800.0) {
            return;
        }
        then = now;
        double e = temperatureIntMeas-temperatureSet.value;
        ie += e;
        ie = (std::max)(-ieMax, (std::min)(ieMax, ie));
        double RT = (std::max)(0.1, e*pg+ie*ig);
        Measurement_t speed = {fanTrimmerWMax.value*minRT/RT, UnitPfxNone, "rpm"};
        this->setCoolingFansSpeed(speed, true);
    }
}

Measurement_t Emcr384PatchClamp_EL07c_prot_v07_fw_v03::fanV2R(Measurement_t V) {
    V.convertValue(fanTrimmerVStep.prefix);
    return fanTrimmerRf*(V.value/fanTrimmerVStep.value-1.0);
}

Measurement_t Emcr384PatchClamp_EL07c_prot_v07_fw_v03::fanW2V(Measurement_t W) {
    W.convertValue(fanTrimmerWMax.prefix);
    return fanTrimmerVMax*(W.value/fanTrimmerWMax.value);
}

Measurement_t Emcr384PatchClamp_EL07c_prot_v07_fw_v03::fanRT2W(Measurement_t RT) {
    RT.convertValue(fanTrimmerRTMin.prefix);
    return fanTrimmerWMax/(RT.value/fanTrimmerRTMin.value);
}

Emcr384PatchClamp_EL07d_prot_v07_fw_v03::Emcr384PatchClamp_EL07d_prot_v07_fw_v03(std::string di) :
    Emcr384PatchClamp_EL07c_prot_v07_fw_v03(di) {

    fwName = "384PatchClamp_EL07d_V03.1.bit";

    // fwSize_B = 5506748;
    // motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;
}
