#include "emcr8npatchclamp_el07e_artix7_pcbv02_fw_v02.h"

Emcr8nPatchClamp_EL07e_artix7_PCBV02_fw_v02::Emcr8nPatchClamp_EL07e_artix7_PCBV02_fw_v02(std::string di) :
    Emcr8nPatchClamp_EL07e_artix7_PCBV02_fw_v01(di) {

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
    double Rb = 90.0;
    double Rc = 0.06;
    double Rm = 100.0;

    fanTrimmerRanges.resize(fanTrimmerRangesNum);
    fanTrimmerRanges[FanTrimmerOff].step = 1.0;
    fanTrimmerRanges[FanTrimmerOff].min = 0.0;
    fanTrimmerRanges[FanTrimmerOff].max = 0.0;
    fanTrimmerRanges[FanTrimmerOff].prefix = UnitPfxKilo;
    fanTrimmerRanges[FanTrimmerOff].unit = "Ohm";
    fanTrimmerRanges[FanTrimmerOn].step = Rm/fanTrimmerLevels;
    fanTrimmerRanges[FanTrimmerOn].min = Rb+Rc;
    fanTrimmerRanges[FanTrimmerOn].max = fanTrimmerRanges[FanTrimmerOn].min+(fanTrimmerLevels-1.0)*fanTrimmerRanges[FanTrimmerOn].step;
    fanTrimmerRanges[FanTrimmerOn].prefix = UnitPfxKilo;
    fanTrimmerRanges[FanTrimmerOn].unit = "Ohm";

    boolConfig.initialWord = 4;
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 4;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    multiConfig.doubleCoderVector.resize(fanTrimmerRangesNum);
    multiConfig.thresholdVector.resize(fanTrimmerRangesNum-1);

    multiConfig.boolCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x3);
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
}
