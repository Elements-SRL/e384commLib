#include "emcr192blm_el08b_mb02_mez03_fw_v05.h"

Emcr192Blm_EL08b_Mb02_Mez03_fw_v05::Emcr192Blm_EL08b_Mb02_Mez03_fw_v05(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "192Blm";

    motherboardBootTime_s = 30;
    waitingTimeBeforeReadingData = 2; //s
    okTransferSize = 0x8000;

    rxSyncWord = 0x5aa55aa5;

    packetsPerFrame = 1;

    voltageChannelsNum = 192;
    currentChannelsNum = 192;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 24;

    rxWordOffsets[RxMessageDataLoad] = 0;
    rxWordLengths[RxMessageDataLoad] = (voltageChannelsNum+currentChannelsNum)*packetsPerFrame;

    rxMaxWords = totalChannelsNum*packetsPerFrame; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 3024;
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    /*! Clamping modalities */
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = ClampingModality_t::VOLTAGE_CLAMP;
    defaultClampingModalityIdx = VoltageClamp;

    /*! Channel sources */
    availableVoltageSourcesIdxs.VoltageFromVoltageClamp = ChannelSourceVoltageFromVoltageClamp;

    /*! Protocols parameters */
    protocolFpgaClockFrequencyHz = 10.0e6;

    protocolTimeRange.step = 1000.0/protocolFpgaClockFrequencyHz;
    protocolTimeRange.min = LINT32_MIN*protocolTimeRange.step;
    protocolTimeRange.max = LINT32_MAX*protocolTimeRange.step;
    protocolTimeRange.prefix = UnitPfxMilli;
    protocolTimeRange.unit = "s";

    positiveProtocolTimeRange = protocolTimeRange;
    positiveProtocolTimeRange.min = 0.0;

    protocolFrequencyRange.step = protocolFpgaClockFrequencyHz/(256.0*(UINT24_MAX+1.0)); /*! 10.0MHz / 256 / 2^24 */
    protocolFrequencyRange.min = INT24_MIN*protocolFrequencyRange.step;
    protocolFrequencyRange.max = INT24_MAX*protocolFrequencyRange.step;
    protocolFrequencyRange.prefix = UnitPfxNone;
    protocolFrequencyRange.unit = "Hz";

    positiveProtocolFrequencyRange = protocolFrequencyRange;
    positiveProtocolFrequencyRange.min = 0.0;

    voltageProtocolStepImplemented = true;
    voltageProtocolRampImplemented = true;
    voltageProtocolSinImplemented = true;

    protocolMaxItemsNum = 20;
    protocolWordOffset = 72;
    protocolItemsWordsNum = 12;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange250pA].max = 250.0;
    vcCurrentRangesArray[VCCurrentRange250pA].min = -250.0;
    vcCurrentRangesArray[VCCurrentRange250pA].step = vcCurrentRangesArray[VCCurrentRange250pA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange250pA].prefix = UnitPfxPico;
    vcCurrentRangesArray[VCCurrentRange250pA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange2_5nA].max = 2.5;
    vcCurrentRangesArray[VCCurrentRange2_5nA].min = -2.5;
    vcCurrentRangesArray[VCCurrentRange2_5nA].step = vcCurrentRangesArray[VCCurrentRange2_5nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange2_5nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange2_5nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange25nA].max = 25.0;
    vcCurrentRangesArray[VCCurrentRange25nA].min = -25.0;
    vcCurrentRangesArray[VCCurrentRange25nA].step = vcCurrentRangesArray[VCCurrentRange25nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange25nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange25nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange250nA].max = 250.0;
    vcCurrentRangesArray[VCCurrentRange250nA].min = -250.0;
    vcCurrentRangesArray[VCCurrentRange250nA].step = vcCurrentRangesArray[VCCurrentRange250nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange250nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange250nA].unit = "A";
    defaultVcCurrentRangeIdxs.resize(1);
    defaultVcCurrentRangeIdxs[0] = VCCurrentRange250pA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].max = 512.0;
    vcVoltageRangesArray[VCVoltageRange500mV].min = -512.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.0625;
    vcVoltageRangesArray[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange500mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange500mV;

    liquidJunctionSameRangeAsVcDac = true;
    liquidJunctionRangesNum = LJVoltageRangesNum;
    liquidJunctionRangesArray = vcVoltageRangesArray;
    defaultLiquidJunctionRangeIdx = defaultVcVoltageRangeIdx;

    /*! Current ranges */
    /*! CC */

    /*! Voltage ranges */
    /*! CC */

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;
    vcCurrentFiltersArray.resize(vcCurrentFiltersNum);
    vcCurrentFiltersArray[VCCurrentFilter5kHz].value = 5.0;
    vcCurrentFiltersArray[VCCurrentFilter5kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter5kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter10kHz].value = 10.0;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter20kHz].value = 20.0;
    vcCurrentFiltersArray[VCCurrentFilter20kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter20kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter100kHz].value = 100.0;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter5kHz;

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter26Hz].value = 26.0;
    vcVoltageFiltersArray[VCVoltageFilter26Hz].prefix = UnitPfxNone;
    vcVoltageFiltersArray[VCVoltageFilter26Hz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter1kHz].value = 1.0;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter5kHz].value = 5.0;
    vcVoltageFiltersArray[VCVoltageFilter5kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter5kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter10kHz].value = 10.0;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter26Hz;

    /*! Current filters */
    /*! CC */

    /*! Voltage filters */
    /*! CC */

    /*! Clock dividers */
    clockDividersNum = ClockDividersNum;

    clockDividersArray.resize(clockDividersNum);
    clockDividersArray[ClockDivider8] = 8;
    clockDividersArray[ClockDivider4] = 4;
    clockDividersArray[ClockDivider2] = 2;
    clockDividersArray[ClockDivider1] = 1;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    defaultSamplingRateIdx = SamplingRate1_25kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate1_25kHz].value = 1250.0/1024.0;
    realSamplingRatesArray[SamplingRate1_25kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate1_25kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate2_5kHz].value = 1250.0/512.0;
    realSamplingRatesArray[SamplingRate2_5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate2_5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate5kHz].value = 1250.0/256.0;
    realSamplingRatesArray[SamplingRate5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate10kHz].value = 1250.0/128.0;
    realSamplingRatesArray[SamplingRate10kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate10kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate20kHz].value = 1250.0/64.0;
    realSamplingRatesArray[SamplingRate20kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate20kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate50kHz].value = 50.0;
    realSamplingRatesArray[SamplingRate50kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate50kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate100kHz].value = 100.0;
    realSamplingRatesArray[SamplingRate100kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate100kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate200kHz].value = 200.0;
    realSamplingRatesArray[SamplingRate200kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate200kHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate1_25kHz] = ClockDivider8;
    sr2srm[SamplingRate2_5kHz] = ClockDivider8;
    sr2srm[SamplingRate5kHz] = ClockDivider4;
    sr2srm[SamplingRate10kHz] = ClockDivider4;
    sr2srm[SamplingRate20kHz] = ClockDivider2;
    sr2srm[SamplingRate50kHz] = ClockDivider2;
    sr2srm[SamplingRate100kHz] = ClockDivider1;
    sr2srm[SamplingRate200kHz] = ClockDivider1;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate1_25kHz].value = 1024.0/1.250;
    integrationStepArray[SamplingRate1_25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1_25kHz].unit = "s";
    integrationStepArray[SamplingRate2_5kHz].value = 512.0/1.250;
    integrationStepArray[SamplingRate2_5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate2_5kHz].unit = "s";
    integrationStepArray[SamplingRate5kHz].value = 256.0/1.250;
    integrationStepArray[SamplingRate5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5kHz].unit = "s";
    integrationStepArray[SamplingRate10kHz].value = 128.0/1.250;
    integrationStepArray[SamplingRate10kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate10kHz].unit = "s";
    integrationStepArray[SamplingRate20kHz].value = 64.0/1.250;
    integrationStepArray[SamplingRate20kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate20kHz].unit = "s";
    integrationStepArray[SamplingRate50kHz].value = 20.0;
    integrationStepArray[SamplingRate50kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate50kHz].unit = "s";
    integrationStepArray[SamplingRate100kHz].value = 10.0;
    integrationStepArray[SamplingRate100kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate100kHz].unit = "s";
    integrationStepArray[SamplingRate200kHz].value = 5.0;
    integrationStepArray[SamplingRate200kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate200kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate1_25kHz, VCCurrentFilter5kHz},
        {SamplingRate2_5kHz, VCCurrentFilter5kHz},
        {SamplingRate5kHz, VCCurrentFilter5kHz},
        {SamplingRate10kHz, VCCurrentFilter5kHz},
        {SamplingRate20kHz, VCCurrentFilter10kHz},
        {SamplingRate50kHz, VCCurrentFilter10kHz},
        {SamplingRate100kHz, VCCurrentFilter20kHz},
        {SamplingRate200kHz, VCCurrentFilter100kHz}
    };

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};

    defaultVInitRampTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultVFinalRampTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultTRampTuner = {0.0, UnitPfxNone, "s"};

    uint32_t vRampTunerCodersOffset = 268;
    uint32_t vRampTunerCodersSize = 8;

    /*! Zap */
    zapDurationRange.step = 0.1;
    zapDurationRange.min = 0.0;
    zapDurationRange.max = zapDurationRange.min+zapDurationRange.step*(double)UINT16_MAX;
    zapDurationRange.prefix = UnitPfxMilli;
    zapDurationRange.unit = "s";

    /*! VC leak calibration (shunt resistance)*/
    rRShuntConductanceCalibRange.resize(VCCurrentRangesNum);
    rRShuntConductanceCalibRange[VCCurrentRange250pA].step = (vcCurrentRangesArray[VCCurrentRange250pA].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange250pA].min = -2.0*(vcCurrentRangesArray[VCCurrentRange250pA].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange250pA].max = 2.0*(vcCurrentRangesArray[VCCurrentRange250pA].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange250pA].step;
    rRShuntConductanceCalibRange[VCCurrentRange250pA].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange250pA].unit = "S";
    rRShuntConductanceCalibRange[VCCurrentRange2_5nA].step = (vcCurrentRangesArray[VCCurrentRange2_5nA].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange2_5nA].min = -2.0*(vcCurrentRangesArray[VCCurrentRange2_5nA].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange2_5nA].max = 2.0*(vcCurrentRangesArray[VCCurrentRange2_5nA].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange2_5nA].step;
    rRShuntConductanceCalibRange[VCCurrentRange2_5nA].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange2_5nA].unit = "S";
    rRShuntConductanceCalibRange[VCCurrentRange25nA].step = (vcCurrentRangesArray[VCCurrentRange25nA].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange25nA].min = -2.0*(vcCurrentRangesArray[VCCurrentRange25nA].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange25nA].max = 2.0*(vcCurrentRangesArray[VCCurrentRange25nA].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange25nA].step;
    rRShuntConductanceCalibRange[VCCurrentRange25nA].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange25nA].unit = "S";
    rRShuntConductanceCalibRange[VCCurrentRange250nA].step = (vcCurrentRangesArray[VCCurrentRange250nA].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange250nA].min = -2.0*(vcCurrentRangesArray[VCCurrentRange250nA].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange250nA].max = 2.0*(vcCurrentRangesArray[VCCurrentRange250nA].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange250nA].step;
    rRShuntConductanceCalibRange[VCCurrentRange250nA].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange250nA].unit = "S";

    /*! VC voltage calib gain (DAC) */
    calibVcVoltageGainRange.step = 1.0/16384.0;
    calibVcVoltageGainRange.min = 0;
    calibVcVoltageGainRange.max = SHORT_MAX * calibVcVoltageGainRange.step;
    calibVcVoltageGainRange.prefix = UnitPfxNone;
    calibVcVoltageGainRange.unit = "";

    /*! VC current calib gain (ADC) */
    calibVcCurrentGainRange.step = 1.0/16384.0;
    calibVcCurrentGainRange.min = 0;
    calibVcCurrentGainRange.max = SHORT_MAX * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.prefix = UnitPfxNone;
    calibVcCurrentGainRange.unit = "";

    /*! VC Voltage calib offset (DAC)*/
    calibVcVoltageOffsetRanges = vcVoltageRangesArray;

    /*! VC current calib offset (ADC)*/
    calibVcCurrentOffsetRanges = vcCurrentRangesArray;

    /*! Default values */
    currentRanges.resize(currentChannelsNum);
    std::fill(currentRanges.begin(), currentRanges.end(), vcCurrentRangesArray[defaultVcCurrentRangeIdxs[0]]);
    currentResolutions.resize(currentChannelsNum);
    std::fill(currentResolutions.begin(), currentResolutions.end(), currentRanges[0].step);
    voltageRanges.resize(voltageChannelsNum);
    std::fill(voltageRanges.begin(), voltageRanges.end(), vcVoltageRangesArray[defaultVcVoltageRangeIdx]);
    voltageResolutions.resize(voltageChannelsNum);
    std::fill(voltageResolutions.begin(), voltageResolutions.end(), voltageRanges[0].step);
    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    /*! Asic reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    asicResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(asicResetCoder);

    /*! FPGA reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    fpgaResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(fpgaResetCoder);

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 3;
    boolConfig.bitsNum = 4;
    samplingRateCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(samplingRateCoder);

    /*! Clock divider */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 7;
    boolConfig.bitsNum = 2;
    clockDividerCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (clockDividerCoder)->addMapItem(3);
    static_cast <BoolRandomArrayCoder *> (clockDividerCoder)->addMapItem(2);
    static_cast <BoolRandomArrayCoder *> (clockDividerCoder)->addMapItem(1);
    static_cast <BoolRandomArrayCoder *> (clockDividerCoder)->addMapItem(0);
    coders.push_back(clockDividerCoder);

    boolConfig.initialWord = 0;
    boolConfig.initialBit = 10;
    boolConfig.bitsNum = 1;
    overHeatingModeCoder = new BoolArrayCoder(boolConfig);

    /*! Current range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentRangeCoders.clear();
    vcCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[0]);

    /*! Voltage range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageRangeCoders.clear();
    vcVoltageRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcVoltageRangeCoders[0]);

    /*! Current range CC */

    /*! Voltage range CC */

    /*! Current filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentFilterCoder);

    /*! Voltage filter VC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageFilterCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(3); // 26 Hz
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(0); // 1 kHz
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(1); // 5 kHz
    static_cast <BoolRandomArrayCoder *> (vcVoltageFilterCoder)->addMapItem(2); // 10kHz
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */

    /*! Voltage filter CC */

    /*! Liquid junction compensation */
    boolConfig.initialWord = 1996;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    liquidJunctionCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        liquidJunctionCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(liquidJunctionCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    boolConfig.initialWord = 2020;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    liquidJunctionResetCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        liquidJunctionResetCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(liquidJunctionResetCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    boolConfig.initialWord = 2008;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    liquidJunctionAutoStopCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        liquidJunctionAutoStopCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(liquidJunctionAutoStopCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    currentTrackingCoders.resize(VCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 1804;
        doubleConfig.resolution = vcCurrentRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcCurrentRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcCurrentRangesArray[rangeIdx].max;
        currentTrackingCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            currentTrackingCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(currentTrackingCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! Enable stimulus */
    boolConfig.initialWord = 2032;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    enableStimulusCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        enableStimulusCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(enableStimulusCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Zap */
    boolConfig.initialWord = 2044;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    zapCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        zapCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(zapCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    doubleConfig.initialWord = 2056;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = zapDurationRange.step;
    doubleConfig.minValue = zapDurationRange.min;
    doubleConfig.maxValue = zapDurationRange.max;
    zapDurationCoder = new DoubleOffsetBinaryCoder(doubleConfig);
    coders.push_back(zapDurationCoder);

    /*! Protocol reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 9;
    boolConfig.bitsNum = 1;
    protocolResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolResetCoder);

    /*! Protocol structure */
    boolConfig.initialWord = protocolWordOffset;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolIdCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolIdCoder);

    boolConfig.initialWord = protocolWordOffset+1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolItemsNumberCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolItemsNumberCoder);

    boolConfig.initialWord = protocolWordOffset+2;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolSweepsNumberCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolSweepsNumberCoder);

    doubleConfig.initialWord = protocolWordOffset+3;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    voltageProtocolRestCoders.resize(VCVoltageRangesNum);

    for (unsigned int rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*32768.0;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;
        voltageProtocolRestCoders[rangeIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(voltageProtocolRestCoders[rangeIdx]);
    }

    /*! Protocol items */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    voltageProtocolStim0Coders.resize(VCVoltageRangesNum);
    voltageProtocolStim0StepCoders.resize(VCVoltageRangesNum);
    voltageProtocolStim1Coders.resize(VCVoltageRangesNum);
    voltageProtocolStim1StepCoders.resize(VCVoltageRangesNum);

    for (unsigned int rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        voltageProtocolStim0Coders[rangeIdx].resize(protocolMaxItemsNum);
        voltageProtocolStim0StepCoders[rangeIdx].resize(protocolMaxItemsNum);
        voltageProtocolStim1Coders[rangeIdx].resize(protocolMaxItemsNum);
        voltageProtocolStim1StepCoders[rangeIdx].resize(protocolMaxItemsNum);

        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*32768.0;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;

        for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
            doubleConfig.initialWord = protocolWordOffset+4+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim0Coders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim0Coders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+5+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim0StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim0StepCoders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+6+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim1Coders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim1Coders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+7+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim1StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim1StepCoders[rangeIdx][itemIdx]);
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = positiveProtocolTimeRange.step;
    doubleConfig.minValue = positiveProtocolTimeRange.min;
    doubleConfig.maxValue = positiveProtocolTimeRange.max;
    protocolTime0Coders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+8+protocolItemsWordsNum*itemIdx;
        protocolTime0Coders[itemIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(protocolTime0Coders[itemIdx]);
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = protocolTimeRange.step;
    doubleConfig.minValue = protocolTimeRange.min;
    doubleConfig.maxValue = protocolTimeRange.max;
    protocolTime0StepCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+10+protocolItemsWordsNum*itemIdx;
        protocolTime0StepCoders[itemIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(protocolTime0StepCoders[itemIdx]);
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = positiveProtocolFrequencyRange.step;
    doubleConfig.minValue = positiveProtocolFrequencyRange.min;
    doubleConfig.maxValue = positiveProtocolFrequencyRange.max;
    protocolFrequency0Coders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+8+protocolItemsWordsNum*itemIdx;
        protocolFrequency0Coders[itemIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(protocolFrequency0Coders[itemIdx]);
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = protocolFrequencyRange.step;
    doubleConfig.minValue = protocolFrequencyRange.min;
    doubleConfig.maxValue = protocolFrequencyRange.max;
    protocolFrequency0StepCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+10+protocolItemsWordsNum*itemIdx;
        protocolFrequency0StepCoders[itemIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(protocolFrequency0StepCoders[itemIdx]);
    }

    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolItemIdxCoders.resize(protocolMaxItemsNum);
    protocolNextItemIdxCoders.resize(protocolMaxItemsNum);
    protocolLoopRepetitionsCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+12+protocolItemsWordsNum*itemIdx;
        protocolItemIdxCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolItemIdxCoders[itemIdx]);

        boolConfig.initialWord = protocolWordOffset+13+protocolItemsWordsNum*itemIdx;
        protocolNextItemIdxCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolNextItemIdxCoders[itemIdx]);

        boolConfig.initialWord = protocolWordOffset+14+protocolItemsWordsNum*itemIdx;
        protocolLoopRepetitionsCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolLoopRepetitionsCoders[itemIdx]);
    }

    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    protocolApplyStepsCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+15+protocolItemsWordsNum*itemIdx;
        protocolApplyStepsCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolApplyStepsCoders[itemIdx]);
    }

    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 4;
    protocolItemTypeCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+15+protocolItemsWordsNum*itemIdx;
        protocolItemTypeCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolItemTypeCoders[itemIdx]);
    }

    /*! V Ramp tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vInitRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vInitRampTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vInitRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vInitRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vFinalRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset+1;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vFinalRampTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vFinalRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vFinalRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    doubleConfig.initialWord = vRampTunerCodersOffset+2;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = positiveProtocolTimeRange.step;
    doubleConfig.minValue = positiveProtocolTimeRange.min;
    doubleConfig.maxValue = positiveProtocolTimeRange.max;
    tRampTunerCoders.resize(currentChannelsNum);

    for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        tRampTunerCoders[channelIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(tRampTunerCoders[channelIdx]);
        doubleConfig.initialWord += vRampTunerCodersSize;
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    quotRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset+4;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step/positiveProtocolTimeRange.step;
        doubleConfig.minValue = LINT32_MIN*doubleConfig.resolution;
        doubleConfig.maxValue = LINT32_MAX*doubleConfig.resolution;
        quotRampTunerCoders[rangeIdx].resize(currentChannelsNum);

        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            quotRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(quotRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    remRampTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = vRampTunerCodersOffset+6;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = LINT32_MIN*doubleConfig.resolution;
        doubleConfig.maxValue = LINT32_MAX*doubleConfig.resolution;
        remRampTunerCoders[rangeIdx].resize(currentChannelsNum);

        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            remRampTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(remRampTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord += vRampTunerCodersSize;
        }
    }

    /*! Activate ramp tuners */
    boolConfig.initialWord = 256;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    activateRampTunerCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        activateRampTunerCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(activateRampTunerCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! DAC gain e offset */
    /*! VC Voltage gain */
    doubleConfig.initialWord = 2057;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibVcVoltageGainRange.step;
    doubleConfig.minValue = calibVcVoltageGainRange.min;
    doubleConfig.maxValue = calibVcVoltageGainRange.max;
    calibVcVoltageGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcVoltageGainCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(calibVcVoltageGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! VC Voltage offset */
    calibVcVoltageOffsetCoders.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2249;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcVoltageOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcVoltageOffsetRanges[rangeIdx].min;
        doubleConfig.maxValue = calibVcVoltageOffsetRanges[rangeIdx].max;
        calibVcVoltageOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcVoltageOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcVoltageOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }

    /*! ADC gain e offset */
    /*! VC current gain */
    doubleConfig.initialWord = 2441;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibVcCurrentGainRange.step;
    doubleConfig.minValue = calibVcCurrentGainRange.min;
    doubleConfig.maxValue = calibVcCurrentGainRange.max;
    calibVcCurrentGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcCurrentGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(calibVcCurrentGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! VC current offset */
    calibVcCurrentOffsetCoders.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2633;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibVcCurrentOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibVcCurrentOffsetRanges[rangeIdx].min;
        doubleConfig.maxValue = calibVcCurrentOffsetRanges[rangeIdx].max;
        calibVcCurrentOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibVcCurrentOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibVcCurrentOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }

    /*! VC leak calibration */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    calibRShuntConductanceCoders.resize(VCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2825;
        doubleConfig.resolution = rRShuntConductanceCalibRange[rangeIdx].step;
        doubleConfig.minValue = rRShuntConductanceCalibRange[rangeIdx].min;
        doubleConfig.maxValue = rRShuntConductanceCalibRange[rangeIdx].max;
        calibRShuntConductanceCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            calibRShuntConductanceCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibRShuntConductanceCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! Default status */
    txStatus.init(txDataWords);
    txStatus.encodingWords[0] = 0x4000; // data real
}

ErrorCodes_t Emcr192Blm_EL08b_Mb02_Mez03_fw_v05::initializeHW() {
    std::this_thread::sleep_for (std::chrono::seconds(motherboardBootTime_s));

    this->sendCommands();

    return Success;
}
