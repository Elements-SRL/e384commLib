#include "messagedispatcher_384nanopores.h"

MessageDispatcher_384NanoPores_V01::MessageDispatcher_384NanoPores_V01(string di) :
    MessageDispatcher_OpalKelly(di) {








    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    asicResetCoder = new BoolArrayCoder(boolConfig);

    /*! Current range */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentRangeCoder = new BoolArrayCoder(boolConfig);

    /*! Digital offset compensation */
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        digitalOffsetCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! VRest */
    doubleConfig.initialWord = 174;
    doubleConfig.initialBit = 3;
    doubleConfig.bitsNum = 13;
    doubleConfig.minValue = -512.0;
    doubleConfig.maxValue = 511.0;
    doubleConfig.resolution = 1.0;
    doubleConfig.offset = 0.0;
    stimRestCoder = new DoubleTwosCompCoder(doubleConfig);











    deviceName = "384NanoPores";

    rxSyncWord = 0x5aa5;
    txSyncWord = 0x5aa5;

    packetsPerFrame = 1;

    voltageChannelsNum = 384;
    currentChannelsNum = 384;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    rxWordOffsets.resize(rxMessageNum);
    rxWordLengths.resize(rxMessageNum);
    rxWordOffsets[rxMessageDataLoad] = 0;
    rxWordLengths[rxMessageDataLoad] = (voltageChannelsNum+currentChannelsNum)*packetsPerFrame;

    rxWordOffsets[rxMessageDataHeader] = rxWordOffsets[rxMessageDataLoad] + rxWordLengths[rxMessageDataLoad];
    rxWordLengths[rxMessageDataHeader] = 4;

    rxWordOffsets[rxMessageDataTail] = rxWordOffsets[rxMessageDataHeader] + rxWordLengths[rxMessageDataHeader];
    rxWordLengths[rxMessageDataTail] = 1;

    rxWordOffsets[rxMessageStatus] = rxWordOffsets[rxMessageDataTail] + rxWordLengths[rxMessageDataTail];
    rxWordLengths[rxMessageStatus] = 1;

    rxWordOffsets[rxMessageVoltageOffset] = rxWordOffsets[rxMessageStatus] + rxWordLengths[rxMessageStatus];
    rxWordLengths[rxMessageVoltageOffset] = currentChannelsNum;


    commonReadFrameLength = sizeof(rxSyncWord)/RX_WORD_SIZE + 2 + rxWordLengths[rxMessageDataLoad];

    maxOutputPacketsNum = E384CL_DATA_ARRAY_SIZE/totalChannelsNum;

    txDataWords = 874+0; // ANCORA NON DEFINITO, AGGIORNARE
    txModifiedStartingWord = txDataWords;
    txModifiedEndingWord = 0;
    txMaxWords = sizeof(txSyncWord)/TX_WORD_SIZE + 2 + txDataWords + sizeof(txCrcInitialValue)/TX_WORD_SIZE; /*! Additional words are header, offset, length and CRC */

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange4uA].min = -4000000.0;
    vcCurrentRangesArray[VCCurrentRange4uA].max = 4000000.0;
    vcCurrentRangesArray[VCCurrentRange4uA].step = vcCurrentRangesArray[VCCurrentRange4uA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange4uA].prefix = UnitPfxPico;
    vcCurrentRangesArray[VCCurrentRange4uA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange200nA].min = -200000.0;
    vcCurrentRangesArray[VCCurrentRange200nA].max = 200000.0;
    vcCurrentRangesArray[VCCurrentRange200nA].step = vcCurrentRangesArray[VCCurrentRange200nA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange200nA].prefix = UnitPfxPico;
    vcCurrentRangesArray[VCCurrentRange200nA].unit = "A";
    defaultVcCurrentRangeIdx = VCCurrentRange200nA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].min = -500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].max = 500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.0125;
    vcVoltageRangesArray[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange500mV].unit = "V";

    defaultVcVoltageRangeIdx = VCVoltageRange500mV;

    voltageRangesWithOffsetArray.resize(voltageRangesNum);
    for (unsigned int voltageRangeIdx = 0; voltageRangeIdx < voltageRangesNum; voltageRangeIdx++) {
        voltageRangesWithOffsetArray[voltageRangeIdx] = voltageRangesArray[voltageRangeIdx];
    }

    voltageOffsetArray.resize(voltageRangesNum);
    intVoltageOffsetArray.resize(voltageRangesNum);
    for (unsigned int voltageRangeIdx = 0; voltageRangeIdx < voltageRangesNum; voltageRangeIdx++) {
        voltageOffsetArray[voltageRangeIdx] = 0.5*(voltageRangesWithOffsetArray[voltageRangeIdx].max+voltageRangesWithOffsetArray[voltageRangeIdx].min);
        intVoltageOffsetArray[voltageRangeIdx] = (uint16_t)(round(voltageOffsetArray[voltageRangeIdx]/voltageRangesWithOffsetArray[voltageRangeIdx].step));
    }
    voltageOffsetArray[VoltageRange15V] = -1650.0; /*! - Vcm */
    intVoltageOffsetArray[VoltageRange15V] = (uint16_t)round(65536.0+voltageOffsetArray[VoltageRange15V]/0.0625); /*! half range - Vcm / dac_ext resolution */

    sequencingVoltageRangeIdx = VoltageRange500mV;
    conditioningVoltageRangeIdx = VoltageRange15V;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate50x4kHz].value = 100.0e3/512.0;
    samplingRatesArray[SamplingRate50x4kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate50x4kHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate50x4kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate50x4kHz].value = 100.0e3/512.0;
    realSamplingRatesArray[SamplingRate50x4kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate50x4kHz].unit = "Hz";

    bandwidthsArray.resize(samplingRatesNum);
    bandwidthsArray[SamplingRate50x4kHz].value = 20.0;
    bandwidthsArray[SamplingRate50x4kHz].prefix = UnitPfxKilo;
    bandwidthsArray[SamplingRate50x4kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate50x4kHz].value = 5.12;
    integrationStepArray[SamplingRate50x4kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate50x4kHz].unit = "s";

    voltageOffsetCompensationGain.value = 0.0625;
    voltageOffsetCompensationGain.prefix = UnitPfxMilli;
    voltageOffsetCompensationGain.unit = "V";

    /*! This will never change so it makes sense to initialize it here */
    /*! Default values */
    currentRange = currentRangesArray[CurrentRange200nA];
    currentResolution = currentRangesArray[CurrentRange200nA].step;
    voltageRange = voltageRangesWithOffsetArray[VoltageRange500mV];
    voltageResolution = voltageRangesWithOffsetArray[VoltageRange500mV].step;
    voltageOffset = voltageOffsetArray[VoltageRange500mV];
    intVoltageOffset = intVoltageOffsetArray[VoltageRange500mV];
    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    /*! Front end latency */
    frontEndLatency.value = 67.4;
    frontEndLatency.prefix = UnitPfxMicro;
    frontEndLatency.unit = "s";

    /*! External DAC */
    dacExtRange.step = 0.0625;
    dacExtRange.min = 0.0;
    dacExtRange.max = 65535.0*dacExtRange.step;
    dacExtRange.prefix = UnitPfxMilli;
    dacExtRange.unit = "V";

    dacExtDefault.value = 1650.0;
    dacExtDefault.prefix = UnitPfxMilli;
    dacExtDefault.unit = "V";

    /*************\
     * Protocols *
    \*************/

    /*! Voltage ranges */
    protocolVoltageRangesArray.resize(ProtocolVoltageRangesNum);
    protocolVoltageRangesArray[ProtocolVoltageRange500mV].min = -500.0;
    protocolVoltageRangesArray[ProtocolVoltageRange500mV].max = 500.0;
    protocolVoltageRangesArray[ProtocolVoltageRange500mV].step = 0.0625;
    protocolVoltageRangesArray[ProtocolVoltageRange500mV].prefix = UnitPfxMilli;
    protocolVoltageRangesArray[ProtocolVoltageRange500mV].unit = "V";

    /*! Time ranges */
    protocolTimeRangesArray.resize(ProtocolTimeRangesNum);
    protocolTimeRangesArray[ProtocolTimeRange2_10ms].min = 1.0;
    protocolTimeRangesArray[ProtocolTimeRange2_10ms].max = 1000.0;
    protocolTimeRangesArray[ProtocolTimeRange2_10ms].step = 1.0;
    protocolTimeRangesArray[ProtocolTimeRange2_10ms].prefix = UnitPfxMilli;
    protocolTimeRangesArray[ProtocolTimeRange2_10ms].unit = "s";
    protocolTimeRangesArray[ProtocolTimeRange0to2_16].min = 0.0;
    protocolTimeRangesArray[ProtocolTimeRange0to2_16].max = 65.0e3;
    protocolTimeRangesArray[ProtocolTimeRange0to2_16].step = 1.0;
    protocolTimeRangesArray[ProtocolTimeRange0to2_16].prefix = UnitPfxMilli;
    protocolTimeRangesArray[ProtocolTimeRange0to2_16].unit = "s";
    protocolTimeRangesArray[ProtocolTimeRange1to2_16].min = 1.0;
    protocolTimeRangesArray[ProtocolTimeRange1to2_16].max = 65.0e3;
    protocolTimeRangesArray[ProtocolTimeRange1to2_16].step = 1.0;
    protocolTimeRangesArray[ProtocolTimeRange1to2_16].prefix = UnitPfxMilli;
    protocolTimeRangesArray[ProtocolTimeRange1to2_16].unit = "s";
    protocolTimeRangesArray[ProtocolTimeRange1orMore].min = 1.0;
    protocolTimeRangesArray[ProtocolTimeRange1orMore].max = (numeric_limits <double> ::max)();
    protocolTimeRangesArray[ProtocolTimeRange1orMore].step = 1.0;
    protocolTimeRangesArray[ProtocolTimeRange1orMore].prefix = UnitPfxMilli;
    protocolTimeRangesArray[ProtocolTimeRange1orMore].unit = "s";
    protocolTimeRangesArray[ProtocolTimeRangeSigned2_15].min = -32.5e3;
    protocolTimeRangesArray[ProtocolTimeRangeSigned2_15].max = 32.5e3;
    protocolTimeRangesArray[ProtocolTimeRangeSigned2_15].step = 1.0;
    protocolTimeRangesArray[ProtocolTimeRangeSigned2_15].prefix = UnitPfxMilli;
    protocolTimeRangesArray[ProtocolTimeRangeSigned2_15].unit = "s";

    /*! Protocol selection */
    protocolsNames.resize(ProtocolsNum);
    protocolsNames[ProtocolConstant] = "Constant";
    protocolsNames[ProtocolTriangular] = "Triangular";
    protocolsNames[ProtocolSquareWave] = "Square wave";
    protocolsNames[ProtocolConductance] = "Conductance";
    protocolsNames[ProtocolVariableAmplitude] = "Variable Amplitude";
    protocolsNames[ProtocolVariableDuration] = "Variable Duration";
    protocolsNames[ProtocolRamp] = "Ramp";
    protocolsNames[ProtocolCyclicVoltammetry] = "Cyclic Voltammetry";
    defaultProtocol = ProtocolConstant;
    selectedProtocol = defaultProtocol;
    triangularProtocolIdx = ProtocolTriangular;
    sealTestProtocolIdx = ProtocolSquareWave;

    protocolsImages.resize(ProtocolsNum);
    protocolsImages[ProtocolConstant] = "holdingVoltage001";
    protocolsImages[ProtocolTriangular] = "triangularParametric001";
    protocolsImages[ProtocolSquareWave] = "sealTest001";
    protocolsImages[ProtocolConductance] = "conductance001";
    protocolsImages[ProtocolVariableAmplitude] = "stepVariableAmplitude001";
    protocolsImages[ProtocolVariableDuration] = "stepVariableDuration001";
    protocolsImages[ProtocolRamp] = "ramp002";
    protocolsImages[ProtocolCyclicVoltammetry] = "cyclicVoltammetry002";

    protocolsAvailableVoltages.resize(ProtocolsNum);
    protocolsAvailableTimes.resize(ProtocolsNum);
    protocolsAvailableAdimensionals.resize(ProtocolsNum);

    protocolsAvailableVoltages[ProtocolConstant].push_back(ProtocolVHold);

    protocolsAvailableVoltages[ProtocolTriangular].push_back(ProtocolVHold);
    protocolsAvailableVoltages[ProtocolTriangular].push_back(ProtocolVPk);
    protocolsAvailableTimes[ProtocolTriangular].push_back(ProtocolTPe);

    protocolsAvailableVoltages[ProtocolSquareWave].push_back(ProtocolVHold);
    protocolsAvailableVoltages[ProtocolSquareWave].push_back(ProtocolVPulse);
    protocolsAvailableTimes[ProtocolSquareWave].push_back(ProtocolTHold);
    protocolsAvailableTimes[ProtocolSquareWave].push_back(ProtocolTPulse);

    protocolsAvailableVoltages[ProtocolConductance].push_back(ProtocolVHold);
    protocolsAvailableVoltages[ProtocolConductance].push_back(ProtocolVPulse);
    protocolsAvailableVoltages[ProtocolConductance].push_back(ProtocolVStep);
    protocolsAvailableTimes[ProtocolConductance].push_back(ProtocolTHold);
    protocolsAvailableTimes[ProtocolConductance].push_back(ProtocolTPulse);
    protocolsAvailableAdimensionals[ProtocolConductance].push_back(ProtocolN);
    protocolsAvailableAdimensionals[ProtocolConductance].push_back(ProtocolNR);

    protocolsAvailableVoltages[ProtocolVariableAmplitude].push_back(ProtocolVHold);
    protocolsAvailableVoltages[ProtocolVariableAmplitude].push_back(ProtocolVPulse);
    protocolsAvailableVoltages[ProtocolVariableAmplitude].push_back(ProtocolVStep);
    protocolsAvailableTimes[ProtocolVariableAmplitude].push_back(ProtocolTHold);
    protocolsAvailableTimes[ProtocolVariableAmplitude].push_back(ProtocolTPulse);
    protocolsAvailableAdimensionals[ProtocolVariableAmplitude].push_back(ProtocolN);
    protocolsAvailableAdimensionals[ProtocolVariableAmplitude].push_back(ProtocolNR);

    protocolsAvailableVoltages[ProtocolVariableDuration].push_back(ProtocolVHold);
    protocolsAvailableVoltages[ProtocolVariableDuration].push_back(ProtocolVPulse);
    protocolsAvailableTimes[ProtocolVariableDuration].push_back(ProtocolTHold);
    protocolsAvailableTimes[ProtocolVariableDuration].push_back(ProtocolTPulse);
    protocolsAvailableTimes[ProtocolVariableDuration].push_back(ProtocolTStep);
    protocolsAvailableAdimensionals[ProtocolVariableDuration].push_back(ProtocolN);
    protocolsAvailableAdimensionals[ProtocolVariableDuration].push_back(ProtocolNR);

    protocolsAvailableVoltages[ProtocolRamp].push_back(ProtocolVHold);
    protocolsAvailableVoltages[ProtocolRamp].push_back(ProtocolVFinal);
    protocolsAvailableVoltages[ProtocolRamp].push_back(ProtocolVInit);
    protocolsAvailableTimes[ProtocolRamp].push_back(ProtocolTHold);
    protocolsAvailableTimes[ProtocolRamp].push_back(ProtocolTPulse);
    protocolsAvailableTimes[ProtocolRamp].push_back(ProtocolTRamp);
    protocolsAvailableAdimensionals[ProtocolRamp].push_back(ProtocolNR);

    protocolsAvailableVoltages[ProtocolCyclicVoltammetry].push_back(ProtocolVHold);
    protocolsAvailableVoltages[ProtocolCyclicVoltammetry].push_back(ProtocolVFinal);
    protocolsAvailableVoltages[ProtocolCyclicVoltammetry].push_back(ProtocolVInit);
    protocolsAvailableTimes[ProtocolCyclicVoltammetry].push_back(ProtocolTHold);
    protocolsAvailableTimes[ProtocolCyclicVoltammetry].push_back(ProtocolTRamp);
    protocolsAvailableAdimensionals[ProtocolCyclicVoltammetry].push_back(ProtocolN);
    protocolsAvailableAdimensionals[ProtocolCyclicVoltammetry].push_back(ProtocolNR);

    /*! Protocol voltages */
    protocolVoltagesNum = ProtocolVoltagesNum;
    protocolVoltageNames.resize(ProtocolVoltagesNum);
    protocolVoltageNames[ProtocolVHold] = "Vhold";
    protocolVoltageNames[ProtocolVPulse] = "Vpulse";
    protocolVoltageNames[ProtocolVStep] = "Vstep";
    protocolVoltageNames[ProtocolVPk] = "Vamp";
    protocolVoltageNames[ProtocolVFinal] = "Vfinal";
    protocolVoltageNames[ProtocolVInit] = "Vinit";

    protocolVoltageRanges.resize(ProtocolVoltagesNum);
    protocolVoltageRanges[ProtocolVHold].step = 1.0;
    protocolVoltageRanges[ProtocolVHold].min = voltageRangesArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVHold].max = voltageRangesArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVHold].prefix = UnitPfxMilli;
    protocolVoltageRanges[ProtocolVHold].unit = "V";
    protocolVoltageRanges[ProtocolVPulse].step = 1.0;
    protocolVoltageRanges[ProtocolVPulse].min = voltageRangesArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVPulse].max = voltageRangesArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVPulse].prefix = UnitPfxMilli;
    protocolVoltageRanges[ProtocolVPulse].unit = "V";
    protocolVoltageRanges[ProtocolVStep].step = 1.0;
    protocolVoltageRanges[ProtocolVStep].min = voltageRangesArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVStep].max = voltageRangesArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVStep].prefix = UnitPfxMilli;
    protocolVoltageRanges[ProtocolVStep].unit = "V";
    protocolVoltageRanges[ProtocolVPk].step = 25.0;
    protocolVoltageRanges[ProtocolVPk].min = 25.0;
    protocolVoltageRanges[ProtocolVPk].max = 4.0*protocolVoltageRanges[ProtocolVPk].step;
    protocolVoltageRanges[ProtocolVPk].prefix = UnitPfxMilli;
    protocolVoltageRanges[ProtocolVPk].unit = "V";
    protocolVoltageRanges[ProtocolVFinal].step = 1.0;
    protocolVoltageRanges[ProtocolVFinal].min = voltageRangesArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVFinal].max = voltageRangesArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVFinal].prefix = UnitPfxMilli;
    protocolVoltageRanges[ProtocolVFinal].unit = "V";
    protocolVoltageRanges[ProtocolVInit].step = 1.0;
    protocolVoltageRanges[ProtocolVInit].min = voltageRangesArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVInit].max = voltageRangesArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVInit].prefix = UnitPfxMilli;
    protocolVoltageRanges[ProtocolVInit].unit = "V";

    protocolVoltageDefault.resize(ProtocolVoltagesNum);
    protocolVoltageDefault[ProtocolVHold].value = 0.0;
    protocolVoltageDefault[ProtocolVHold].prefix = UnitPfxMilli;
    protocolVoltageDefault[ProtocolVHold].unit = "V";
    protocolVoltageDefault[ProtocolVPulse].value = 100.0;
    protocolVoltageDefault[ProtocolVPulse].prefix = UnitPfxMilli;
    protocolVoltageDefault[ProtocolVPulse].unit = "V";
    protocolVoltageDefault[ProtocolVStep].value = 20.0;
    protocolVoltageDefault[ProtocolVStep].prefix = UnitPfxMilli;
    protocolVoltageDefault[ProtocolVStep].unit = "V";
    protocolVoltageDefault[ProtocolVPk].value = 100.0;
    protocolVoltageDefault[ProtocolVPk].prefix = UnitPfxMilli;
    protocolVoltageDefault[ProtocolVPk].unit = "V";
    protocolVoltageDefault[ProtocolVFinal].value = 100.0;
    protocolVoltageDefault[ProtocolVFinal].prefix = UnitPfxMilli;
    protocolVoltageDefault[ProtocolVFinal].unit = "V";
    protocolVoltageDefault[ProtocolVInit].value = -100.0;
    protocolVoltageDefault[ProtocolVInit].prefix = UnitPfxMilli;
    protocolVoltageDefault[ProtocolVInit].unit = "V";
    selectedProtocolVoltage.resize(ProtocolVoltagesNum);
    for (unsigned int idx = 0; idx < ProtocolVoltagesNum; idx++) {
        selectedProtocolVoltage[idx] = protocolVoltageDefault[idx];
    }

    /*! Protocol times */
    protocolTimesNum = ProtocolTimesNum;
    protocolTimeNames.resize(ProtocolTimesNum);
    protocolTimeNames[ProtocolTHold] = "Thold";
    protocolTimeNames[ProtocolTPulse] = "Tpulse";
    protocolTimeNames[ProtocolTStep] = "Tstep";
    protocolTimeNames[ProtocolTRamp] = "Tramp";
    protocolTimeNames[ProtocolTPe] = "TPeriod";

    protocolTimeRanges.resize(ProtocolTimesNum);
    protocolTimeRanges[ProtocolTHold].step = 1.0;
    protocolTimeRanges[ProtocolTHold].min = 0.0;
    protocolTimeRanges[ProtocolTHold].max = USHORT_MAX*protocolTimeRanges[ProtocolTHold].step;
    protocolTimeRanges[ProtocolTHold].prefix = UnitPfxMilli;
    protocolTimeRanges[ProtocolTHold].unit = "s";
    protocolTimeRanges[ProtocolTPulse].step = 1.0;
    protocolTimeRanges[ProtocolTPulse].min = 0.0;
    protocolTimeRanges[ProtocolTPulse].max = USHORT_MAX*protocolTimeRanges[ProtocolTPulse].step;
    protocolTimeRanges[ProtocolTPulse].prefix = UnitPfxMilli;
    protocolTimeRanges[ProtocolTPulse].unit = "s";
    protocolTimeRanges[ProtocolTStep].step = 1.0;
    protocolTimeRanges[ProtocolTStep].min = SHORT_MIN*protocolTimeRanges[ProtocolTStep].step;
    protocolTimeRanges[ProtocolTStep].max = SHORT_MAX*protocolTimeRanges[ProtocolTStep].step;
    protocolTimeRanges[ProtocolTStep].prefix = UnitPfxMilli;
    protocolTimeRanges[ProtocolTStep].unit = "s";
    protocolTimeRanges[ProtocolTRamp].step = 1.0;
    protocolTimeRanges[ProtocolTRamp].min = 0.0;
    protocolTimeRanges[ProtocolTRamp].max = USHORT_MAX*protocolTimeRanges[ProtocolTRamp].step;
    protocolTimeRanges[ProtocolTRamp].prefix = UnitPfxMilli;
    protocolTimeRanges[ProtocolTRamp].unit = "s";
    protocolTimeRanges[ProtocolTPe].step = 1.0;
    protocolTimeRanges[ProtocolTPe].min = 0.0;
    protocolTimeRanges[ProtocolTPe].max = UINT10_MAX*protocolTimeRanges[ProtocolTPe].step;
    protocolTimeRanges[ProtocolTPe].prefix = UnitPfxMilli;
    protocolTimeRanges[ProtocolTPe].unit = "s";

    protocolTimeDefault.resize(ProtocolTimesNum);
    protocolTimeDefault[ProtocolTHold].value = 100.0;
    protocolTimeDefault[ProtocolTHold].prefix = UnitPfxMilli;
    protocolTimeDefault[ProtocolTHold].unit = "s";
    protocolTimeDefault[ProtocolTPulse].value = 100.0;
    protocolTimeDefault[ProtocolTPulse].prefix = UnitPfxMilli;
    protocolTimeDefault[ProtocolTPulse].unit = "s";
    protocolTimeDefault[ProtocolTStep].value = 20.0;
    protocolTimeDefault[ProtocolTStep].prefix = UnitPfxMilli;
    protocolTimeDefault[ProtocolTStep].unit = "s";
    protocolTimeDefault[ProtocolTRamp].value = 1000.0;
    protocolTimeDefault[ProtocolTRamp].prefix = UnitPfxMilli;
    protocolTimeDefault[ProtocolTRamp].unit = "s";
    protocolTimeDefault[ProtocolTPe].value = 100.0;
    protocolTimeDefault[ProtocolTPe].prefix = UnitPfxMilli;
    protocolTimeDefault[ProtocolTPe].unit = "s";
    selectedProtocolTime.resize(ProtocolTimesNum);
    for (unsigned int idx = 0; idx < ProtocolTimesNum; idx++) {
        selectedProtocolTime[idx] = protocolTimeDefault[idx];
    }

    /*! Protocol adimensionals */
    protocolAdimensionalsNum = ProtocolAdimensionalsNum;
    protocolAdimensionalNames.resize(ProtocolAdimensionalsNum);
    protocolAdimensionalNames[ProtocolN] = "N";
    protocolAdimensionalNames[ProtocolNR] = "NR";

    protocolAdimensionalRanges.resize(ProtocolAdimensionalsNum);
    protocolAdimensionalRanges[ProtocolN].step = 1.0;
    protocolAdimensionalRanges[ProtocolN].min = 0.0;
    protocolAdimensionalRanges[ProtocolN].max = UINT10_MAX*protocolAdimensionalRanges[ProtocolN].step;
    protocolAdimensionalRanges[ProtocolN].prefix = UnitPfxNone;
    protocolAdimensionalRanges[ProtocolN].unit = "";
    protocolAdimensionalRanges[ProtocolNR].step = 1.0;
    protocolAdimensionalRanges[ProtocolNR].min = 0.0;
    protocolAdimensionalRanges[ProtocolNR].max = UINT10_MAX*protocolAdimensionalRanges[ProtocolNR].step;
    protocolAdimensionalRanges[ProtocolNR].prefix = UnitPfxNone;
    protocolAdimensionalRanges[ProtocolNR].unit = "";

    protocolAdimensionalDefault.resize(ProtocolAdimensionalsNum);
    protocolAdimensionalDefault[ProtocolN].value = 5.0;
    protocolAdimensionalDefault[ProtocolN].prefix = UnitPfxNone;
    protocolAdimensionalDefault[ProtocolN].unit = "";
    protocolAdimensionalDefault[ProtocolNR].value = 0.0;
    protocolAdimensionalDefault[ProtocolNR].prefix = UnitPfxNone;
    protocolAdimensionalDefault[ProtocolNR].unit = "";
    selectedProtocolAdimensional.resize(ProtocolAdimensionalsNum);
    for (unsigned int idx = 0; idx < ProtocolAdimensionalsNum; idx++) {
        selectedProtocolAdimensional[idx] = protocolAdimensionalDefault[idx];
    }

    selectedVoltageOffset.resize(currentChannelsNum);
    voltageOffsetRange.step = 1.0;
    voltageOffsetRange.min = -500.0;
    voltageOffsetRange.max = 500.0;
    voltageOffsetRange.prefix = UnitPfxMilli;
    voltageOffsetRange.unit = "V";
    for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        selectedVoltageOffset[channelIdx].value = 0.0;
        selectedVoltageOffset[channelIdx].prefix = voltageOffsetRange.prefix;
        selectedVoltageOffset[channelIdx].unit = voltageOffsetRange.unit;
    }

    /*! FSM flags */
    fsmFlagsNum = FsmFlagsNum;
    fsmFlagNames.resize(FsmFlagsNum);
    fsmFlagNames[FsmRun] = "Run";
    fsmFlagNames[FsmEnableReScan] = "Enable Re-scan";
    fsmFlagNames[FsmEnableP2Exit] = "Enable P2 Exit";
    fsmFlagNames[FsmInvertChannels] = "Invert Channels";

    fsmFlagDefault.resize(FsmFlagsNum);
    fsmFlagDefault[FsmRun] = false;
    fsmFlagDefault[FsmEnableReScan] = false;
    fsmFlagDefault[FsmEnableP2Exit] = false;
    fsmFlagDefault[FsmInvertChannels] = false;

    /*! FSM voltage */
    fsmVoltagesNum = FsmVoltagesNum;
    fsmVoltageNames.resize(FsmVoltagesNum);
    fsmVoltageNames[FsmV1PreCapture] = "Pre-Capture V1";
    fsmVoltageNames[FsmV1L2R] = "L-to-R cal V1";
    fsmVoltageNames[FsmV1R2L] = "R-to-L cal V1";
    fsmVoltageNames[FsmV1PrePause] = "Pre-Pause V1";
    fsmVoltageNames[FsmV1Capture] = "Capture V1";
    fsmVoltageNames[FsmV2Reverse] = "Reverse V2";
    fsmVoltageNames[FsmV2Capture] = "Capture V2";
    fsmVoltageNames[FsmV12Push] = "Push V12";
    fsmVoltageNames[FsmV12Pull] = "Pull V12";
    fsmVoltageNames[FsmV1ClogEject] = "Clog Eject V1";
    fsmVoltageNames[FsmV1Pause] = "Pause V1";

    fsmVoltageRange.min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    fsmVoltageRange.max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    fsmVoltageRange.step = voltageRangesWithOffsetArray[VoltageRange500mV].step;
    fsmVoltageRange.prefix = UnitPfxMilli;
    fsmVoltageRange.unit = "V";

    fsmGUIVoltageRange.min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    fsmGUIVoltageRange.max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    fsmGUIVoltageRange.step = 1.0;
    fsmGUIVoltageRange.prefix = UnitPfxMilli;
    fsmGUIVoltageRange.unit = "V";

    fsmVoltageDefault.resize(FsmVoltagesNum);
    fsmVoltageDefault[FsmV1PreCapture].value = 200.0;
    fsmVoltageDefault[FsmV1PreCapture].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV1PreCapture].unit = "V";
    fsmVoltageDefault[FsmV1L2R].value = 250.0;
    fsmVoltageDefault[FsmV1L2R].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV1L2R].unit = "V";
    fsmVoltageDefault[FsmV1R2L].value = 650.0;
    fsmVoltageDefault[FsmV1R2L].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV1R2L].unit = "V";
    fsmVoltageDefault[FsmV1PrePause].value = 0.0;
    fsmVoltageDefault[FsmV1PrePause].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV1PrePause].unit = "V";
    fsmVoltageDefault[FsmV1Capture].value = -200.0;
    fsmVoltageDefault[FsmV1Capture].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV1Capture].unit = "V";
    fsmVoltageDefault[FsmV2Reverse].value = 300.0;
    fsmVoltageDefault[FsmV2Reverse].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV2Reverse].unit = "V";
    fsmVoltageDefault[FsmV2Capture].value = 400.0;
    fsmVoltageDefault[FsmV2Capture].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV2Capture].unit = "V";
    fsmVoltageDefault[FsmV12Push].value = 200.0;
    fsmVoltageDefault[FsmV12Push].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV12Push].unit = "V";
    fsmVoltageDefault[FsmV12Pull].value = -200.0;
    fsmVoltageDefault[FsmV12Pull].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV12Pull].unit = "V";
    fsmVoltageDefault[FsmV1ClogEject].value = 650.0;
    fsmVoltageDefault[FsmV1ClogEject].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV1ClogEject].unit = "V";
    fsmVoltageDefault[FsmV1Pause].value = 0.0;
    fsmVoltageDefault[FsmV1Pause].prefix = UnitPfxMilli;
    fsmVoltageDefault[FsmV1Pause].unit = "V";

    /*! FSM threshold currents */
    fsmCurrentsNum = FsmCurrentsNum;
    fsmCurrentNames.resize(FsmCurrentsNum);
    fsmCurrentNames[FsmP1PreCaptureThreshold] = "P1 Pre-Capture Threshold";
    fsmCurrentNames[FsmP1CaptureThreshold] = "P1 Capture Threshold";
    fsmCurrentNames[FsmP2CaptureThreshold] = "P2 Capture Threshold";
    fsmCurrentNames[FsmP2TagThreshold] = "P2 Tag Threshold";
    fsmCurrentNames[FsmP2ExitThreshold] = "P2 Exit Threshold";
    fsmCurrentNames[FsmP2FoldThreshold] = "P2 Fold Threshold";
    fsmCurrentNames[FsmPullCaptureThreshold] = "Pull Capture Threshold";

    fsmThresholdCurrentRange.step = currentRangesArray[CurrentRange200nA].step*0.5;
    fsmThresholdCurrentRange.min = 0.0;
    fsmThresholdCurrentRange.max = currentRangesArray[CurrentRange200nA].max;
    fsmThresholdCurrentRange.prefix = UnitPfxPico;
    fsmThresholdCurrentRange.unit = "A";

    fsmThresholdCurrentDefault.resize(FsmCurrentsNum);
    fsmThresholdCurrentDefault[FsmP1PreCaptureThreshold].value = 100.0;
    fsmThresholdCurrentDefault[FsmP1PreCaptureThreshold].prefix = UnitPfxPico;
    fsmThresholdCurrentDefault[FsmP1PreCaptureThreshold].unit = "A";
    fsmThresholdCurrentDefault[FsmP1CaptureThreshold].value = 90.0;
    fsmThresholdCurrentDefault[FsmP1CaptureThreshold].prefix = UnitPfxPico;
    fsmThresholdCurrentDefault[FsmP1CaptureThreshold].unit = "A";
    fsmThresholdCurrentDefault[FsmP2CaptureThreshold].value = 200.0;
    fsmThresholdCurrentDefault[FsmP2CaptureThreshold].prefix = UnitPfxPico;
    fsmThresholdCurrentDefault[FsmP2CaptureThreshold].unit = "A";
    fsmThresholdCurrentDefault[FsmP2TagThreshold].value = 70.0;
    fsmThresholdCurrentDefault[FsmP2TagThreshold].prefix = UnitPfxPico;
    fsmThresholdCurrentDefault[FsmP2TagThreshold].unit = "A";
    fsmThresholdCurrentDefault[FsmP2ExitThreshold].value = 170.0;
    fsmThresholdCurrentDefault[FsmP2ExitThreshold].prefix = UnitPfxPico;
    fsmThresholdCurrentDefault[FsmP2ExitThreshold].unit = "A";
    fsmThresholdCurrentDefault[FsmP2FoldThreshold].value = 100.0;
    fsmThresholdCurrentDefault[FsmP2FoldThreshold].prefix = UnitPfxPico;
    fsmThresholdCurrentDefault[FsmP2FoldThreshold].unit = "A";
    fsmThresholdCurrentDefault[FsmPullCaptureThreshold].value = 90.0;
    fsmThresholdCurrentDefault[FsmPullCaptureThreshold].prefix = UnitPfxPico;
    fsmThresholdCurrentDefault[FsmPullCaptureThreshold].unit = "A";

    /*! FSM time */
    fsmTimesNum = FsmTimesNum;
    fsmTimeNames.resize(FsmTimesNum);
    fsmTimeNames[FsmTransientSettleTime] = "Transient Settle Time";
    fsmTimeNames[FsmTofHoldTime] = "TOF Hold";
    fsmTimeNames[FsmPauseFilterResetTime] = "Pause Filter Reset";
    fsmTimeNames[FsmPreCaptureLowLimitTime] = "Pre-Capture low limit";
    fsmTimeNames[FsmPreCaptureHighLimitTime] = "Pre-Capture high limit";
    fsmTimeNames[FsmL2RCalibrationTime] = "L-to-R Cali Wait";
    fsmTimeNames[FsmR2LCalibrationTime] = "R-to-L Cali Wait";
    fsmTimeNames[FsmPrePauseWaitTime] = "Pre-Pause Wait";
    fsmTimeNames[FsmP1CaptureTimeout] = "P1 Capture Timeout";
    fsmTimeNames[FsmP1CaptureWaitTime] = "P1 Capture Wait";
    fsmTimeNames[FsmP2CaptureTimeout] = "P2 Capture Timeout";
    fsmTimeNames[FsmP2CaptureWaitTime] = "P2 Capture Wait";
    fsmTimeNames[FsmP2ReverseTimeout] = "P2 Reverse Timeout";
    fsmTimeNames[FsmP2ExitWaitTime] = "P2 Exit Wait";
    fsmTimeNames[FsmTagDurationMaxTime] = "Tag duration max";
    fsmTimeNames[FsmTagDurationMinTime] = "Tag duration min";
    fsmTimeNames[FsmTagWaitTime] = "Tag wait";
    fsmTimeNames[FsmTagHoldTime] = "Tag hold";
    fsmTimeNames[FsmBaselineSettleTime] = "Baseline settle";
    fsmTimeNames[FsmPushTime] = "Push time";
    fsmTimeNames[FsmPullTimeout] = "Pull Timeout";
    fsmTimeNames[FsmPullInDelayTime] = "Pull in delay";
    fsmTimeNames[FsmRecaptureTimeout] = "Recapture timeout";
    fsmTimeNames[FsmClogEjectDuration] = "Clog Eject Duration";

    fsmTimeRanges.resize(FsmTimesNum);
    fsmTimeRanges[FsmTransientSettleTime].step = 0.1;
    fsmTimeRanges[FsmTransientSettleTime].min = 0.0;
    fsmTimeRanges[FsmTransientSettleTime].max = USHORT_MAX*fsmTimeRanges[FsmTransientSettleTime].step;
    fsmTimeRanges[FsmTransientSettleTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmTransientSettleTime].unit = "s";
    fsmTimeRanges[FsmTofHoldTime].step = 0.1;
    fsmTimeRanges[FsmTofHoldTime].min = 0.0;
    fsmTimeRanges[FsmTofHoldTime].max = USHORT_MAX*fsmTimeRanges[FsmTofHoldTime].step;
    fsmTimeRanges[FsmTofHoldTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmTofHoldTime].unit = "s";
    fsmTimeRanges[FsmPauseFilterResetTime].step = 0.1;
    fsmTimeRanges[FsmPauseFilterResetTime].min = 0.0;
    fsmTimeRanges[FsmPauseFilterResetTime].max = USHORT_MAX*fsmTimeRanges[FsmPauseFilterResetTime].step;
    fsmTimeRanges[FsmPauseFilterResetTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmPauseFilterResetTime].unit = "s";
    fsmTimeRanges[FsmPreCaptureLowLimitTime].step = 0.1;
    fsmTimeRanges[FsmPreCaptureLowLimitTime].min = 0.0;
    fsmTimeRanges[FsmPreCaptureLowLimitTime].max = USHORT_MAX*fsmTimeRanges[FsmPreCaptureLowLimitTime].step;
    fsmTimeRanges[FsmPreCaptureLowLimitTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmPreCaptureLowLimitTime].unit = "s";
    fsmTimeRanges[FsmPreCaptureHighLimitTime].step = 6.4;
    fsmTimeRanges[FsmPreCaptureHighLimitTime].min = 0.0;
    fsmTimeRanges[FsmPreCaptureHighLimitTime].max = USHORT_MAX*fsmTimeRanges[FsmPreCaptureHighLimitTime].step;
    fsmTimeRanges[FsmPreCaptureHighLimitTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmPreCaptureHighLimitTime].unit = "s";
    fsmTimeRanges[FsmL2RCalibrationTime].step = 6.4;
    fsmTimeRanges[FsmL2RCalibrationTime].min = 0.0;
    fsmTimeRanges[FsmL2RCalibrationTime].max = USHORT_MAX*fsmTimeRanges[FsmL2RCalibrationTime].step;
    fsmTimeRanges[FsmL2RCalibrationTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmL2RCalibrationTime].unit = "s";
    fsmTimeRanges[FsmR2LCalibrationTime].step = 6.4;
    fsmTimeRanges[FsmR2LCalibrationTime].min = 0.0;
    fsmTimeRanges[FsmR2LCalibrationTime].max = USHORT_MAX*fsmTimeRanges[FsmR2LCalibrationTime].step;
    fsmTimeRanges[FsmR2LCalibrationTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmR2LCalibrationTime].unit = "s";
    fsmTimeRanges[FsmPrePauseWaitTime].step = 6.4;
    fsmTimeRanges[FsmPrePauseWaitTime].min = 0.0;
    fsmTimeRanges[FsmPrePauseWaitTime].max = USHORT_MAX*fsmTimeRanges[FsmPrePauseWaitTime].step;
    fsmTimeRanges[FsmPrePauseWaitTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmPrePauseWaitTime].unit = "s";
    fsmTimeRanges[FsmP1CaptureTimeout].step = 409.6;
    fsmTimeRanges[FsmP1CaptureTimeout].min = 0.0;
    fsmTimeRanges[FsmP1CaptureTimeout].max = USHORT_MAX*fsmTimeRanges[FsmP1CaptureTimeout].step;
    fsmTimeRanges[FsmP1CaptureTimeout].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmP1CaptureTimeout].unit = "s";
    fsmTimeRanges[FsmP1CaptureWaitTime].step = 6.4;
    fsmTimeRanges[FsmP1CaptureWaitTime].min = 0.0;
    fsmTimeRanges[FsmP1CaptureWaitTime].max = USHORT_MAX*fsmTimeRanges[FsmP1CaptureWaitTime].step;
    fsmTimeRanges[FsmP1CaptureWaitTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmP1CaptureWaitTime].unit = "s";
    fsmTimeRanges[FsmP2CaptureTimeout].step = 409.6;
    fsmTimeRanges[FsmP2CaptureTimeout].min = 0.0;
    fsmTimeRanges[FsmP2CaptureTimeout].max = USHORT_MAX*fsmTimeRanges[FsmP2CaptureTimeout].step;
    fsmTimeRanges[FsmP2CaptureTimeout].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmP2CaptureTimeout].unit = "s";
    fsmTimeRanges[FsmP2CaptureWaitTime].step = 0.1;
    fsmTimeRanges[FsmP2CaptureWaitTime].min = 0.0;
    fsmTimeRanges[FsmP2CaptureWaitTime].max = USHORT_MAX*fsmTimeRanges[FsmP2CaptureWaitTime].step;
    fsmTimeRanges[FsmP2CaptureWaitTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmP2CaptureWaitTime].unit = "s";
    fsmTimeRanges[FsmP2ReverseTimeout].step = 409.6;
    fsmTimeRanges[FsmP2ReverseTimeout].min = 0.0;
    fsmTimeRanges[FsmP2ReverseTimeout].max = USHORT_MAX*fsmTimeRanges[FsmP2ReverseTimeout].step;
    fsmTimeRanges[FsmP2ReverseTimeout].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmP2ReverseTimeout].unit = "s";
    fsmTimeRanges[FsmP2ExitWaitTime].step = 0.1;
    fsmTimeRanges[FsmP2ExitWaitTime].min = 0.0;
    fsmTimeRanges[FsmP2ExitWaitTime].max = USHORT_MAX*fsmTimeRanges[FsmP2ExitWaitTime].step;
    fsmTimeRanges[FsmP2ExitWaitTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmP2ExitWaitTime].unit = "s";
    fsmTimeRanges[FsmTagDurationMaxTime].step = 6.4;
    fsmTimeRanges[FsmTagDurationMaxTime].min = 0.0;
    fsmTimeRanges[FsmTagDurationMaxTime].max = USHORT_MAX*fsmTimeRanges[FsmTagDurationMaxTime].step;
    fsmTimeRanges[FsmTagDurationMaxTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmTagDurationMaxTime].unit = "s";
    fsmTimeRanges[FsmTagDurationMinTime].step = 0.1;
    fsmTimeRanges[FsmTagDurationMinTime].min = 0.0;
    fsmTimeRanges[FsmTagDurationMinTime].max = USHORT_MAX*fsmTimeRanges[FsmTagDurationMinTime].step;
    fsmTimeRanges[FsmTagDurationMinTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmTagDurationMinTime].unit = "s";
    fsmTimeRanges[FsmTagWaitTime].step = 0.1;
    fsmTimeRanges[FsmTagWaitTime].min = 0.0;
    fsmTimeRanges[FsmTagWaitTime].max = USHORT_MAX*fsmTimeRanges[FsmTagWaitTime].step;
    fsmTimeRanges[FsmTagWaitTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmTagWaitTime].unit = "s";
    fsmTimeRanges[FsmTagHoldTime].step = 0.1;
    fsmTimeRanges[FsmTagHoldTime].min = 0.0;
    fsmTimeRanges[FsmTagHoldTime].max = USHORT_MAX*fsmTimeRanges[FsmTagHoldTime].step;
    fsmTimeRanges[FsmTagHoldTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmTagHoldTime].unit = "s";
    fsmTimeRanges[FsmBaselineSettleTime].step = 0.1;
    fsmTimeRanges[FsmBaselineSettleTime].min = 0.0;
    fsmTimeRanges[FsmBaselineSettleTime].max = USHORT_MAX*fsmTimeRanges[FsmBaselineSettleTime].step;
    fsmTimeRanges[FsmBaselineSettleTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmBaselineSettleTime].unit = "s";
    fsmTimeRanges[FsmPushTime].step = 6.4;
    fsmTimeRanges[FsmPushTime].min = 0.0;
    fsmTimeRanges[FsmPushTime].max = USHORT_MAX*fsmTimeRanges[FsmPushTime].step;
    fsmTimeRanges[FsmPushTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmPushTime].unit = "s";
    fsmTimeRanges[FsmPullTimeout].step = 409.6;
    fsmTimeRanges[FsmPullTimeout].min = 0.0;
    fsmTimeRanges[FsmPullTimeout].max = USHORT_MAX*fsmTimeRanges[FsmPullTimeout].step;
    fsmTimeRanges[FsmPullTimeout].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmPullTimeout].unit = "s";
    fsmTimeRanges[FsmPullInDelayTime].step = 6.4;
    fsmTimeRanges[FsmPullInDelayTime].min = 0.0;
    fsmTimeRanges[FsmPullInDelayTime].max = USHORT_MAX*fsmTimeRanges[FsmPullInDelayTime].step;
    fsmTimeRanges[FsmPullInDelayTime].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmPullInDelayTime].unit = "s";
    fsmTimeRanges[FsmRecaptureTimeout].step = 409.6;
    fsmTimeRanges[FsmRecaptureTimeout].min = 0.0;
    fsmTimeRanges[FsmRecaptureTimeout].max = USHORT_MAX*fsmTimeRanges[FsmRecaptureTimeout].step;
    fsmTimeRanges[FsmRecaptureTimeout].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmRecaptureTimeout].unit = "s";
    fsmTimeRanges[FsmClogEjectDuration].step = 409.6;
    fsmTimeRanges[FsmClogEjectDuration].min = 0.0;
    fsmTimeRanges[FsmClogEjectDuration].max = USHORT_MAX*fsmTimeRanges[FsmClogEjectDuration].step;
    fsmTimeRanges[FsmClogEjectDuration].prefix = UnitPfxMicro;
    fsmTimeRanges[FsmClogEjectDuration].unit = "s";

    fsmTimeDefault.resize(FsmTimesNum);
    fsmTimeDefault[FsmTransientSettleTime].value = 1000.0;
    fsmTimeDefault[FsmTransientSettleTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmTransientSettleTime].unit = "s";
    fsmTimeDefault[FsmTofHoldTime].value = 500.0;
    fsmTimeDefault[FsmTofHoldTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmTofHoldTime].unit = "s";
    fsmTimeDefault[FsmPauseFilterResetTime].value = 100.0;
    fsmTimeDefault[FsmPauseFilterResetTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmPauseFilterResetTime].unit = "s";
    fsmTimeDefault[FsmPreCaptureLowLimitTime].value = 0.5;
    fsmTimeDefault[FsmPreCaptureLowLimitTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmPreCaptureLowLimitTime].unit = "s";
    fsmTimeDefault[FsmPreCaptureHighLimitTime].value = 20.0;
    fsmTimeDefault[FsmPreCaptureHighLimitTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmPreCaptureHighLimitTime].unit = "s";
    fsmTimeDefault[FsmL2RCalibrationTime].value = 11.0;
    fsmTimeDefault[FsmL2RCalibrationTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmL2RCalibrationTime].unit = "s";
    fsmTimeDefault[FsmR2LCalibrationTime].value = 11.0;
    fsmTimeDefault[FsmR2LCalibrationTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmR2LCalibrationTime].unit = "s";
    fsmTimeDefault[FsmPrePauseWaitTime].value = 10.0;
    fsmTimeDefault[FsmPrePauseWaitTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmPrePauseWaitTime].unit = "s";
    fsmTimeDefault[FsmP1CaptureTimeout].value = 300.0;
    fsmTimeDefault[FsmP1CaptureTimeout].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmP1CaptureTimeout].unit = "s";
    fsmTimeDefault[FsmP1CaptureWaitTime].value = 0.5;
    fsmTimeDefault[FsmP1CaptureWaitTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmP1CaptureWaitTime].unit = "s";
    fsmTimeDefault[FsmP2CaptureTimeout].value = 2.0;
    fsmTimeDefault[FsmP2CaptureTimeout].prefix = UnitPfxNone;
    fsmTimeDefault[FsmP2CaptureTimeout].unit = "s";
    fsmTimeDefault[FsmP2CaptureWaitTime].value = 0.013;
    fsmTimeDefault[FsmP2CaptureWaitTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmP2CaptureWaitTime].unit = "s";
    fsmTimeDefault[FsmP2ReverseTimeout].value = 1.0;
    fsmTimeDefault[FsmP2ReverseTimeout].prefix = UnitPfxNone;
    fsmTimeDefault[FsmP2ReverseTimeout].unit = "s";
    fsmTimeDefault[FsmP2ExitWaitTime].value = 5000.0;
    fsmTimeDefault[FsmP2ExitWaitTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmP2ExitWaitTime].unit = "s";
    fsmTimeDefault[FsmTagDurationMaxTime].value = 2000.0;
    fsmTimeDefault[FsmTagDurationMaxTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmTagDurationMaxTime].unit = "s";
    fsmTimeDefault[FsmTagDurationMinTime].value = 7.0;
    fsmTimeDefault[FsmTagDurationMinTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmTagDurationMinTime].unit = "s";
    fsmTimeDefault[FsmTagWaitTime].value = 600.0;
    fsmTimeDefault[FsmTagWaitTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmTagWaitTime].unit = "s";
    fsmTimeDefault[FsmTagHoldTime].value = 300.0;
    fsmTimeDefault[FsmTagHoldTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmTagHoldTime].unit = "s";
    fsmTimeDefault[FsmBaselineSettleTime].value = 30.0;
    fsmTimeDefault[FsmBaselineSettleTime].prefix = UnitPfxMicro;
    fsmTimeDefault[FsmBaselineSettleTime].unit = "s";
    fsmTimeDefault[FsmPushTime].value = 30.0;
    fsmTimeDefault[FsmPushTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmPushTime].unit = "s";
    fsmTimeDefault[FsmPullTimeout].value = 1000.0;
    fsmTimeDefault[FsmPullTimeout].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmPullTimeout].unit = "s";
    fsmTimeDefault[FsmPullInDelayTime].value = 20.0;
    fsmTimeDefault[FsmPullInDelayTime].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmPullInDelayTime].unit = "s";
    fsmTimeDefault[FsmRecaptureTimeout].value = 1000.0;
    fsmTimeDefault[FsmRecaptureTimeout].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmRecaptureTimeout].unit = "s";
    fsmTimeDefault[FsmClogEjectDuration].value = 20.0;
    fsmTimeDefault[FsmClogEjectDuration].prefix = UnitPfxMilli;
    fsmTimeDefault[FsmClogEjectDuration].unit = "s";

    /*! FSM integers */
    fsmIntegersNum = FsmIntegersNum;
    fsmIntegerNames.resize(FsmIntegersNum);
    fsmIntegerNames[FsmInitialTagMax] = "Initial tag max";
    fsmIntegerNames[FsmTagCountStop] = "Tag count stop";
    fsmIntegerNames[FsmScanCountMax] = "Scan count max";
    fsmIntegerNames[FsmRecaptureTagCount] = "Recapture tag count";

    fsmIntegerRange.step = 1.0;
    fsmIntegerRange.min = 0.0;
    fsmIntegerRange.max = 127.0;
    fsmIntegerRange.prefix = UnitPfxNone;
    fsmIntegerRange.unit = " ";

    fsmIntegerDefault.resize(FsmIntegersNum);
    fsmIntegerDefault[FsmInitialTagMax].value = 2.0;
    fsmIntegerDefault[FsmInitialTagMax].prefix = UnitPfxNone;
    fsmIntegerDefault[FsmInitialTagMax].unit = " ";
    fsmIntegerDefault[FsmTagCountStop].value = 6.0;
    fsmIntegerDefault[FsmTagCountStop].prefix = UnitPfxNone;
    fsmIntegerDefault[FsmTagCountStop].unit = " ";
    fsmIntegerDefault[FsmScanCountMax].value = 4.0;
    fsmIntegerDefault[FsmScanCountMax].prefix = UnitPfxNone;
    fsmIntegerDefault[FsmScanCountMax].unit = " ";
    fsmIntegerDefault[FsmRecaptureTagCount].value = 3.0;
    fsmIntegerDefault[FsmRecaptureTagCount].prefix = UnitPfxNone;
    fsmIntegerDefault[FsmRecaptureTagCount].unit = " ";

    /*! 1st order filter */
    firstOrderFilterTauRange.step = 0.5;
    firstOrderFilterTauRange.min = 1.0;
    firstOrderFilterTauRange.max = 50.0;
    firstOrderFilterTauRange.prefix = UnitPfxMilli;
    firstOrderFilterTauRange.unit = "s";

    firstOrderFilterFastTauRange.step = 10.0;
    firstOrderFilterFastTauRange.min = 40.0;
    firstOrderFilterFastTauRange.max = 1000.0;
    firstOrderFilterFastTauRange.prefix = UnitPfxMicro;
    firstOrderFilterFastTauRange.unit = "s";

    firstOrderFilterFastDurationRange.step = 10.0;
    firstOrderFilterFastDurationRange.min = 10.0;
    firstOrderFilterFastDurationRange.max = 600.0;
    firstOrderFilterFastDurationRange.prefix = UnitPfxMicro;
    firstOrderFilterFastDurationRange.unit = "s";

    firstOrderFilterTauDefault.value = 5.0;
    firstOrderFilterTauDefault.prefix = UnitPfxMilli;
    firstOrderFilterTauDefault.unit = "s";
    firstOrderFilterTau = firstOrderFilterTauDefault;

    firstOrderFilterFastTauDefault.value = 50.0;
    firstOrderFilterFastTauDefault.prefix = UnitPfxMicro;
    firstOrderFilterFastTauDefault.unit = "s";
    firstOrderFilterFastTau = firstOrderFilterFastTauDefault;

    firstOrderFilterFastDurationDefault.value = 100.0;
    firstOrderFilterFastDurationDefault.prefix = UnitPfxMicro;
    firstOrderFilterFastDurationDefault.unit = "s";
    firstOrderFilterFastDuration = firstOrderFilterFastDurationDefault;

    /*! 4th order filter */
    fourthOrderFilterCutoffFrequencyRange.step = 0.1;
    fourthOrderFilterCutoffFrequencyRange.min = 1.0;
    fourthOrderFilterCutoffFrequencyRange.max = 35.0;
    fourthOrderFilterCutoffFrequencyRange.prefix = UnitPfxKilo;
    fourthOrderFilterCutoffFrequencyRange.unit = "Hz";

    fourthOrderFilterCutoffFrequencyDefault.value = 10.0;
    fourthOrderFilterCutoffFrequencyDefault.prefix = UnitPfxKilo;
    fourthOrderFilterCutoffFrequencyDefault.unit = "Hz";
    fourthOrderFilterCutoffFrequency = fourthOrderFilterCutoffFrequencyDefault;

    /*! Conditioning protocol voltage */
    conditioningVoltagesNum = ConditioningVoltagesNum;
    conditioningProtocolVoltageNames.resize(ConditioningVoltagesNum);
    conditioningProtocolVoltageNames[ConditioningVChargePos] = "V Charge +";
    conditioningProtocolVoltageNames[ConditioningVChargeNeg] = "V Charge -";

    conditioningProtocolVoltageRanges.resize(ConditioningVoltagesNum);
    conditioningProtocolVoltageRanges[ConditioningVChargePos] = voltageRangesArray[VoltageRange15V];
    conditioningProtocolVoltageRanges[ConditioningVChargeNeg] = voltageRangesArray[VoltageRange15V];

    appliedConditioningVoltages.resize(ConditioningVoltagesNum);
    conditioningVoltageOffset.resize(currentChannelsNum);
    conditioningVoltageOffsetInt.resize(currentChannelsNum);

    conditioningProtocolVoltageDefault.resize(ConditioningVoltagesNum);
    conditioningProtocolVoltageDefault[ConditioningVChargePos].value = 5000.0;
    conditioningProtocolVoltageDefault[ConditioningVChargePos].prefix = UnitPfxMilli;
    conditioningProtocolVoltageDefault[ConditioningVChargePos].unit = "V";
    conditioningProtocolVoltageDefault[ConditioningVChargeNeg].value = -5000.0;
    conditioningProtocolVoltageDefault[ConditioningVChargeNeg].prefix = UnitPfxMilli;
    conditioningProtocolVoltageDefault[ConditioningVChargeNeg].unit = "V";

    /*! Conditioning protocol time */
    conditioningTimesNum = ConditioningTimesNum;
    conditioningProtocolTimeNames.resize(ConditioningTimesNum);
    conditioningProtocolTimeNames[ConditioningTCharge] = "T Charge";

    conditioningProtocolTimeRanges.resize(ConditioningTimesNum);
    conditioningProtocolTimeRanges[ConditioningTCharge].step = 1.0;
    conditioningProtocolTimeRanges[ConditioningTCharge].min = 1.0;
    conditioningProtocolTimeRanges[ConditioningTCharge].max = USHORT_MAX*conditioningProtocolTimeRanges[ConditioningTCharge].step;
    conditioningProtocolTimeRanges[ConditioningTCharge].prefix = UnitPfxMilli;
    conditioningProtocolTimeRanges[ConditioningTCharge].unit = "s";

    conditioningProtocolTimeDefault.resize(ConditioningTimesNum);
    conditioningProtocolTimeDefault[ConditioningTCharge].value = 250.0;
    conditioningProtocolTimeDefault[ConditioningTCharge].prefix = UnitPfxMilli;
    conditioningProtocolTimeDefault[ConditioningTCharge].unit = "s";

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;

    /*! Device reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    deviceResetCoder = new BoolArrayCoder(boolConfig);

    /*! Calibration reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 15;
    boolConfig.bitsNum = 1;
    calibrationResetCoder = new BoolArrayCoder(boolConfig);

    /*! Current range */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    currentRangeCoder = new BoolRandomArrayCoder(boolConfig);
    currentRangeCoder->addMapItem(0x0); /*!< 4uA   -> 0b0 */
    currentRangeCoder->addMapItem(0x1); /*!< 200nA -> 0b1 */

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 2;
    samplingRateCoder = new BoolRandomArrayCoder(boolConfig);
    samplingRateCoder->addMapItem(0x2); /*!< 4x50kHz -> 0b10 */
    samplingRateCoder->addMapItem(0x1); /*!< 200kHz  -> 0b01 */

    /*! Voltage amplifier (as conditioning, but doesn't change the current range */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 2;
    voltageAmplifierCoder = new BoolRandomArrayCoder(boolConfig);
    voltageAmplifierCoder->addMapItem(0x2); /*!< standard   -> 0b10 */
    voltageAmplifierCoder->addMapItem(0x1); /*!< amplifying -> 0b01 */

    /*! Pore forming */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 1;
    poreFormingCoder = new BoolArrayCoder(boolConfig);

    /*! ASIC internal DAC filter (dummy control) */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 1;
    dacIntFilterCoder = new BoolArrayCoder(boolConfig);

    /*! External DAC filter */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 7;
    boolConfig.bitsNum = 1;
    dacExtFilterCoder = new BoolNegatedArrayCoder(boolConfig);

    /*! Protocol selection */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 9;
    boolConfig.bitsNum = 4;
    protocolsSelectCoder = new BoolArrayCoder(boolConfig);

    /*! Protocol start */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 13;
    boolConfig.bitsNum = 1;
    protocolStartCoder = new BoolArrayCoder(boolConfig);

    /*! Digital offset compensation */
    boolConfig.initialWord = 1;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    digitalOffsetCompensationCoder = new BoolArrayCoder(boolConfig);

    digitalOffsetCompensationStates.resize(currentChannelsNum);
    for (unsigned int currentIdx = 0; currentIdx < currentChannelsNum; currentIdx++) {
        digitalOffsetCompensationStates[currentIdx] = false;
    }

    boolConfig.initialWord = 0;
    boolConfig.initialBit = 14;
    boolConfig.bitsNum = 1;
    digitalOffsetCompensationResetCoder = new BoolArrayCoder(boolConfig);

    /*! Select stimulus channel */
    boolConfig.initialWord = 2;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    selectStimulusChannelCoder = new BoolArrayCoder(boolConfig);

    selectStimulusChannelStates.resize(currentChannelsNum);
    for (unsigned int currentIdx = 0; currentIdx < currentChannelsNum; currentIdx++) {
        selectStimulusChannelStates[currentIdx] = false;
    }

    /*! Channel on */
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    channelOnCoder = new BoolArrayCoder(boolConfig);

    channelOnStates.resize(currentChannelsNum);
    for (unsigned int currentIdx = 0; currentIdx < currentChannelsNum; currentIdx++) {
        channelOnStates[currentIdx] = true;
    }

    /*! REMI2 mode */
    boolConfig.initialWord = 4;
    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 1;
    remi2ModeCoder = new BoolArrayCoder(boolConfig);

    /*! LEDs */
    boolConfig.initialWord = 4;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    ledsNum = LedsNum;
    ledsCoders.resize(ledsNum);
    ledsCoders[LedBlue] = new BoolArrayCoder(boolConfig);

    /*! External DAC */
    doubleConfig.initialWord = 5;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = dacExtRange.step;
    doubleConfig.minValue = dacExtRange.min;
    doubleConfig.maxValue = dacExtRange.max;
    doubleConfig.offset = 0.0;
    dacExtCoder = new DoubleOffsetBinaryCoder(doubleConfig);

    /*! Voltage offsets */
    voltageOffsetCoders.resize(currentChannelsNum);
    doubleConfig.initialBit = 4;
    doubleConfig.bitsNum = 12;
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVHold].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVHold].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVHold].max;
    for (uint16_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
        doubleConfig.initialWord = 6+channelIdx;
        voltageOffsetCoders[channelIdx] = new DoubleTwosCompCoder(doubleConfig);
    }

    /*! Voltage parameters */
    doubleConfig.initialBit = 4;
    doubleConfig.bitsNum = 12;
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVHold].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVHold].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVHold].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    protocolVoltageCoders.resize(ProtocolVoltagesNum);
    doubleConfig.initialWord = 22;
    protocolVoltageCoders[ProtocolVHold] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVPulse].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVPulse].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVPulse].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    protocolVoltageCoders.resize(ProtocolVoltagesNum);
    doubleConfig.initialWord = 24;
    protocolVoltageCoders[ProtocolVPulse] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVStep].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVStep].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVStep].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    protocolVoltageCoders.resize(ProtocolVoltagesNum);
    doubleConfig.initialWord = 25;
    protocolVoltageCoders[ProtocolVStep] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVInit].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVInit].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVInit].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    protocolVoltageCoders.resize(ProtocolVoltagesNum);
    doubleConfig.initialWord = 26;
    protocolVoltageCoders[ProtocolVInit] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVFinal].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVFinal].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVFinal].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    protocolVoltageCoders.resize(ProtocolVoltagesNum);
    doubleConfig.initialWord = 27;
    protocolVoltageCoders[ProtocolVFinal] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVPk].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVPk].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVPk].max;
    doubleConfig.offset = 0.0;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 2;
    doubleConfig.initialWord = 23;
    protocolVoltageCoders[ProtocolVPk] = new DoubleTwosCompCoder(doubleConfig);

    /*! Time parameters */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.offset = 0.0;
    doubleConfig.resolution = protocolTimeRanges[ProtocolTHold].step;
    doubleConfig.minValue = protocolTimeRanges[ProtocolTHold].min;
    doubleConfig.maxValue = protocolTimeRanges[ProtocolTHold].max;
    doubleConfig.initialWord = 28;
    protocolTimeCoders.resize(ProtocolTimesNum);
    protocolTimeCoders[ProtocolTHold] = new DoubleOffsetBinaryCoder(doubleConfig);
    doubleConfig.resolution = protocolTimeRanges[ProtocolTPulse].step;
    doubleConfig.minValue = protocolTimeRanges[ProtocolTPulse].min;
    doubleConfig.maxValue = protocolTimeRanges[ProtocolTPulse].max;
    doubleConfig.initialWord = 29;
    protocolTimeCoders.resize(ProtocolTimesNum);
    protocolTimeCoders[ProtocolTPulse] = new DoubleOffsetBinaryCoder(doubleConfig);
    doubleConfig.resolution = protocolTimeRanges[ProtocolTStep].step;
    doubleConfig.minValue = protocolTimeRanges[ProtocolTStep].min;
    doubleConfig.maxValue = protocolTimeRanges[ProtocolTStep].max;
    doubleConfig.initialWord = 30;
    protocolTimeCoders.resize(ProtocolTimesNum);
    protocolTimeCoders[ProtocolTStep] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolTimeRanges[ProtocolTRamp].step;
    doubleConfig.minValue = protocolTimeRanges[ProtocolTRamp].min;
    doubleConfig.maxValue = protocolTimeRanges[ProtocolTRamp].max;
    doubleConfig.initialWord = 31;
    protocolTimeCoders.resize(ProtocolTimesNum);
    protocolTimeCoders[ProtocolTRamp] = new DoubleOffsetBinaryCoder(doubleConfig);
    doubleConfig.resolution = protocolTimeRanges[ProtocolTPe].step;
    doubleConfig.minValue = protocolTimeRanges[ProtocolTPe].min;
    doubleConfig.maxValue = protocolTimeRanges[ProtocolTPe].max;
    doubleConfig.initialWord = 23;
    doubleConfig.initialBit = 2;
    doubleConfig.bitsNum = 10;
    protocolTimeCoders.resize(ProtocolTimesNum);
    protocolTimeCoders[ProtocolTPe] = new DoubleOffsetBinaryCoder(doubleConfig);

    /*! Protocol Adimensionals */
    protocolAdimensionalCoders.resize(ProtocolAdimensionalsNum);
    doubleConfig.initialWord = 32;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 10;
    doubleConfig.resolution = protocolAdimensionalRanges[ProtocolN].step;
    doubleConfig.minValue = protocolAdimensionalRanges[ProtocolN].min;
    doubleConfig.maxValue = protocolAdimensionalRanges[ProtocolN].max;
    protocolAdimensionalCoders[ProtocolN] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.initialWord = 33;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 10;
    doubleConfig.resolution = protocolAdimensionalRanges[ProtocolNR].step;
    doubleConfig.minValue = protocolAdimensionalRanges[ProtocolNR].min;
    doubleConfig.maxValue = protocolAdimensionalRanges[ProtocolNR].max;
    protocolAdimensionalCoders[ProtocolNR] = new DoubleTwosCompCoder(doubleConfig);

    /*! Conditioning protocol voltage */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = voltageRangesArray[VoltageRange15V].step;
    doubleConfig.minValue = (65535.0+(double)((int16_t)intVoltageOffsetArray[VoltageRange15V]))*voltageRangesArray[VoltageRange15V].step;
    doubleConfig.maxValue = ((double)((int16_t)intVoltageOffsetArray[VoltageRange15V]))*voltageRangesArray[VoltageRange15V].step;
    doubleConfig.offset = 0.0;
    conditioningProtocolVoltageCoders.resize(ConditioningVoltagesNum);
    for (unsigned int idx = 0; idx < ConditioningVoltagesNum; idx++) {
        doubleConfig.initialWord = 34+idx;
        conditioningProtocolVoltageCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
    }

    /*! Conditioning protocol time */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = conditioningProtocolTimeRanges[ConditioningTCharge].step;
    doubleConfig.minValue = 0.0;
    doubleConfig.maxValue = conditioningProtocolTimeRanges[ConditioningTCharge].max;
    doubleConfig.offset = 0.0;
    conditioningProtocolTimeCoders.resize(ConditioningTimesNum);
    for (unsigned int idx = 0; idx < ConditioningTimesNum; idx++) {
        doubleConfig.initialWord = 36+idx;
        conditioningProtocolTimeCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
    }

    /*! 1st order filter */
    doubleConfig.initialWord = 37;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = pow(2.0, -34.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    doubleConfig.offset = 0.0;
    firstOrderFilterKxCoder = new DoubleTwosCompCoder(doubleConfig);

    doubleConfig.initialWord = 39;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = pow(2.0, -30.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    doubleConfig.offset = 0.0;
    firstOrderFilterFastKxCoder = new DoubleTwosCompCoder(doubleConfig);

    doubleConfig.initialWord = 41;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = 5.12;
    doubleConfig.minValue = 0.0;
    doubleConfig.maxValue = 5.12*65535.0;
    doubleConfig.offset = 0.0;
    firstOrderFilterFastDurationCoder = new DoubleOffsetBinaryCoder(doubleConfig);

    /*! 4th order filter */
    doubleConfig.initialWord = 42;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = pow(2.0, -26.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    doubleConfig.offset = 0.0;
    fourthOrderFilterA01Coder = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.initialWord = 44;
    doubleConfig.resolution = pow(2.0, -27.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    fourthOrderFilterA02Coder = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.initialWord = 46;
    doubleConfig.resolution = pow(2.0, -28.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    fourthOrderFilterK0Coder = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.initialWord = 48;
    doubleConfig.resolution = pow(2.0, -26.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    fourthOrderFilterA11Coder = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.initialWord = 50;
    doubleConfig.resolution = pow(2.0, -27.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    fourthOrderFilterA12Coder = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.initialWord = 52;
    doubleConfig.resolution = pow(2.0, -27.0);
    doubleConfig.minValue = -doubleConfig.resolution*pow(2.0, 31.0);
    doubleConfig.maxValue = -doubleConfig.minValue-doubleConfig.resolution;
    fourthOrderFilterK1Coder = new DoubleTwosCompCoder(doubleConfig);

    /*! Fsm flags */
    boolConfig.bitsNum = 1;
    fsmFlagCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        fsmFlagCoders[popIdx].resize(FsmFlagsNum);
        boolConfig.initialWord = 54+48*popIdx;
        for (unsigned int idx = 0; idx < FsmFlagsNum; idx++) {
            boolConfig.initialBit = idx;
            fsmFlagCoders[popIdx][idx] = new BoolArrayCoder(boolConfig);
        }
    }

    /*! Voltage range offset */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 18;
    doubleConfig.resolution = fsmVoltageRange.step;
    doubleConfig.minValue = fsmVoltageRange.min;
    doubleConfig.maxValue = fsmVoltageRange.max;
    doubleConfig.offset = 0.0;
    voltageRangeOffsetCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        doubleConfig.initialWord = 55+48*popIdx;
        voltageRangeOffsetCoders[popIdx] = new DoubleTwosCompCoder(doubleConfig);
    }

    /*! Fsm voltage */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = fsmVoltageRange.step;
    doubleConfig.minValue = fsmVoltageRange.min;
    doubleConfig.maxValue = fsmVoltageRange.max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    fsmVoltageCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        fsmVoltageCoders[popIdx].resize(FsmVoltagesNum);
        doubleConfig.initialWord = 56+48*popIdx;
        for (unsigned int idx = 0; idx < FsmVoltagesNum; idx++) {
            fsmVoltageCoders[popIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            doubleConfig.initialWord++;
        }
    }

    /*! Fsm currents */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = fsmThresholdCurrentRange.step;
    doubleConfig.minValue = fsmThresholdCurrentRange.min;
    doubleConfig.maxValue = fsmThresholdCurrentRange.max;
    doubleConfig.offset = 0.0;
    fsmThresholdCurrentCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        fsmThresholdCurrentCoders[popIdx].resize(FsmCurrentsNum);
        doubleConfig.initialWord = 67+48*popIdx;
        for (unsigned int idx = 0; idx < FsmCurrentsNum; idx++) {
            fsmThresholdCurrentCoders[popIdx][idx] = new DoubleOffsetBinaryCoder(doubleConfig);
            doubleConfig.initialWord++;
        }
    }

    /*! Fsm times */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.offset = 0.0;
    fsmTimeCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        fsmTimeCoders[popIdx].resize(FsmTimesNum);
        doubleConfig.initialWord = 74+48*popIdx;
        for (unsigned int idx = 0; idx < FsmTimesNum; idx++) {
            doubleConfig.resolution = fsmTimeRanges[idx].step;
            doubleConfig.minValue = fsmTimeRanges[idx].min;
            doubleConfig.maxValue = fsmTimeRanges[idx].max;
            fsmTimeCoders[popIdx][idx] = new DoubleOffsetBinaryCoder(doubleConfig);
            doubleConfig.initialWord++;
        }
    }

    /*! Fsm integers */
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    fsmIntegerCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        fsmIntegerCoders[popIdx].resize(FsmIntegersNum);
        boolConfig.initialWord = 98+48*popIdx;
        for (unsigned int idx = 0; idx < FsmIntegersNum; idx++) {
            fsmIntegerCoders[popIdx][idx] = new BoolArrayCoder(boolConfig);
            boolConfig.initialWord++;
        }
    }

    /*! Default status */
    txStatus.resize(txDataWords);

    int txIdx = 0;
    txStatus[txIdx++] = 0x00AA; // CFG0
    txStatus[txIdx++] = 0x0000; // CFG1
    txStatus[txIdx++] = 0xFFFF; // CFG2
    txStatus[txIdx++] = 0xFFFF; // CFG3
    txStatus[txIdx++] = 0x0002; // DBG0
    txStatus[txIdx++] = 0x6720; // Dacext
    for (uint16_t idx = 0; idx < currentChannelsNum; idx++) {
        txStatus[txIdx++] = 0x0000; // Voffsetx
    }
    txStatus[txIdx++] = 0x0000; // VHold
    txStatus[txIdx++] = 0x0000; // Triangular
    txStatus[txIdx++] = 0x0000; // VPulse
    txStatus[txIdx++] = 0x0000; // VStep
    txStatus[txIdx++] = 0x0000; // VInit
    txStatus[txIdx++] = 0x0000; // VFinal
    txStatus[txIdx++] = 0x0000; // THold
    txStatus[txIdx++] = 0x0000; // TPulse
    txStatus[txIdx++] = 0x0000; // TStep
    txStatus[txIdx++] = 0x0000; // TRamp
    txStatus[txIdx++] = 0x0000; // N
    txStatus[txIdx++] = 0x0000; // NR
    txStatus[txIdx++] = 0x0000; // VCharge+
    txStatus[txIdx++] = 0x0000; // VCharge-
    txStatus[txIdx++] = 0x0000; // TCharge
    txStatus[txIdx++] = 0x0000; // FOF_KX
    txStatus[txIdx++] = 0x0000;
    txStatus[txIdx++] = 0x0000; // FOF_fast_KX
    txStatus[txIdx++] = 0x0000;
    txStatus[txIdx++] = 0x0000; // FOF_fast_duration
    txStatus[txIdx++] = 0x0000; // IIR_A01
    txStatus[txIdx++] = 0x0000;
    txStatus[txIdx++] = 0x0000; // IIR_A02
    txStatus[txIdx++] = 0x0000;
    txStatus[txIdx++] = 0x0000; // IIR_K0
    txStatus[txIdx++] = 0x0000;
    txStatus[txIdx++] = 0x0000; // IIR_A11
    txStatus[txIdx++] = 0x0000;
    txStatus[txIdx++] = 0x0000; // IIR_A12
    txStatus[txIdx++] = 0x0000;
    txStatus[txIdx++] = 0x0000; // IIR_K1
    txStatus[txIdx++] = 0x0000;
    for (uint16_t stateIdx = 0; stateIdx < fsmStateChannelsNum; stateIdx++) {
        txStatus[txIdx++] = 0x0000; // fsmFlags
        txStatus[txIdx++] = 0x0000; // VrangeOffset

        for (uint16_t idx = 0; idx < FsmVoltagesNum; idx ++) {
            txStatus[txIdx++] = 0x0000; // fsmVoltage
        }

        for (uint16_t idx = 0; idx < FsmCurrentsNum; idx ++) {
            txStatus[txIdx++] = 0x0000; // fsmCurrent
        }

        for (uint16_t idx = 0; idx < FsmTimesNum; idx ++) {
            txStatus[txIdx++] = 0x0000; // fsmTime
        }

        for (uint16_t idx = 0; idx < FsmIntegersNum; idx ++) {
            txStatus[txIdx++] = 0x0000; // fsmInteger
        }
    }
}

MessageDispatcher_Remi8_V01::~MessageDispatcher_Remi8_V01() {

}

ErrorCodes_t MessageDispatcher_Remi8_V01::setVoltageRangeOffset(uint16_t voltageRangeIdx, Measurement_t offset) {
    if (voltageRangeIdx == VoltageRange15V) {
        /*! The offset is changhable only for the internal DAC */
        return ErrorValueOutOfRange;
    }

    ErrorCodes_t ret = MessageDispatcher::setVoltageRangeOffset(voltageRangeIdx, offset);
    if (ret != Success) {
        return ret;
    }

    /*! Update range and coder for Vhold, VInit and VFinal */
    protocolVoltageRanges[ProtocolVHold].min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVHold].max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVInit].min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVInit].max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    protocolVoltageRanges[ProtocolVFinal].min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    protocolVoltageRanges[ProtocolVFinal].max = voltageRangesWithOffsetArray[VoltageRange500mV].max;

    DoubleCoder::CoderConfig_t doubleConfig;
    doubleConfig.initialBit = 4;
    doubleConfig.bitsNum = 12;
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVHold].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVHold].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVHold].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    doubleConfig.initialWord = 22;
    protocolVoltageCoders[ProtocolVHold] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVInit].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVInit].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVInit].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    doubleConfig.initialWord = 26;
    protocolVoltageCoders[ProtocolVInit] = new DoubleTwosCompCoder(doubleConfig);
    doubleConfig.resolution = protocolVoltageRanges[ProtocolVFinal].step;
    doubleConfig.minValue = protocolVoltageRanges[ProtocolVFinal].min;
    doubleConfig.maxValue = protocolVoltageRanges[ProtocolVFinal].max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    doubleConfig.initialWord = 27;
    protocolVoltageCoders[ProtocolVFinal] = new DoubleTwosCompCoder(doubleConfig);

    /*! Update range and coder for FSM voltage parameters */
    fsmVoltageRange.min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    fsmVoltageRange.max = voltageRangesWithOffsetArray[VoltageRange500mV].max;
    fsmGUIVoltageRange.min = voltageRangesWithOffsetArray[VoltageRange500mV].min;
    fsmGUIVoltageRange.max = voltageRangesWithOffsetArray[VoltageRange500mV].max;

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = fsmVoltageRange.step;
    doubleConfig.minValue = fsmVoltageRange.min;
    doubleConfig.maxValue = fsmVoltageRange.max;
    doubleConfig.offset = voltageOffsetArray[VoltageRange500mV];
    fsmVoltageCoders.resize(fsmStateChannelsNum);
    for (unsigned int popIdx = 0; popIdx < fsmStateChannelsNum; popIdx++) {
        fsmVoltageCoders[popIdx].resize(FsmVoltagesNum);
        doubleConfig.initialWord = 56+48*popIdx;
        for (unsigned int idx = 0; idx < FsmVoltagesNum; idx++) {
            fsmVoltageCoders[popIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            doubleConfig.initialWord++;
        }
    }

    return ret;
}

ErrorCodes_t MessageDispatcher_Remi8_V01::setRemi2Mode(bool flag) {
    uint32_t channelOnState;
    if (flag) {
        channelOnState = 0x0003; /*!< In REMI2 mode keep only channels 0 and 1 active */

    } else {
        channelOnState = 0;
         /*! After exiting REMI2 mode restore the channels that were active before  */
        for (unsigned int idx = 0; idx < currentChannelsNum; idx++) {
            channelOnState |= (channelOnStates[idx] ? (uint32_t)1 : 0) << idx;
        }
    }
    channelOnCoder->encode(channelOnState, txStatus, txModifiedStartingWord, txModifiedEndingWord);

    /*! The message is sent by MessageDispatcher::setRemi2Mode */
    return MessageDispatcher::setRemi2Mode(flag);
}

void MessageDispatcher_Remi8_V01::initializeDevice() {
    for (int idx = 0; idx < currentChannelsNum; idx++) {
        infoStruct.vComp[idx] = 0;
    }

    this->selectStimulusChannel(currentChannelsNum, true, false);
    this->setSamplingRate(defaultSamplingRateIdx, false);

    int16_t vcOffsetInt = (int16_t)ftdiEeprom->getVcOffset();
    dacExtDefault.value = dacExtRange.step*(double)vcOffsetInt;
    this->applyDacExt(dacExtDefault, false);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        this->digitalOffsetCompensation(idx, false, false);
    }

    this->selectVoltageProtocol(0);

    for (int voltageIdx = 0; voltageIdx < ProtocolVoltagesNum; voltageIdx++) {
        this->setProtocolVoltage(voltageIdx, protocolVoltageDefault[voltageIdx], false);
    }

    for (int timeIdx = 0; timeIdx < ProtocolTimesNum; timeIdx++) {
        this->setProtocolTime(timeIdx, protocolTimeDefault[timeIdx], false);
    }

    for (uint32_t flagIdx = 0; flagIdx < FsmFlagsNum; flagIdx++) {
        this->setFsmFlag(fsmStateChannelsNum, flagIdx, fsmFlagDefault[flagIdx], false);
    }

    for (uint32_t voltageIdx = 0; voltageIdx < FsmVoltagesNum; voltageIdx++) {
        this->setFsmVoltage(fsmStateChannelsNum, voltageIdx, fsmVoltageDefault[voltageIdx], false);
    }

    for (uint32_t currentIdx = 0; currentIdx < FsmCurrentsNum; currentIdx++) {
        this->setFsmThresholdCurrent(fsmStateChannelsNum, currentIdx, fsmThresholdCurrentDefault[currentIdx], false);
    }

    for (uint32_t timeIdx = 0; timeIdx < FsmTimesNum; timeIdx++) {
        this->setFsmTime(fsmStateChannelsNum, timeIdx, fsmTimeDefault[timeIdx], false);
    }

    for (uint32_t integerIdx = 0; integerIdx < FsmIntegersNum; integerIdx++) {
        this->setFsmInteger(fsmStateChannelsNum, integerIdx, (unsigned int)fsmIntegerDefault[integerIdx].value, false);
    }

    this->set1stOrderFilterTau(firstOrderFilterTauDefault);
    this->set4thOrderFilterCutoffFrequency(fourthOrderFilterCutoffFrequencyDefault);

    for (int voltageIdx = 0; voltageIdx < ConditioningVoltagesNum; voltageIdx++) {
        this->setConditioningProtocolVoltage(voltageIdx, conditioningProtocolVoltageDefault[voltageIdx], false);
    }

    for (int timeIdx = 0; timeIdx < ConditioningTimesNum; timeIdx++) {
        this->setConditioningProtocolTime(timeIdx, conditioningProtocolTimeDefault[timeIdx], false);
    }

    this->setPoreForming(false, false);

    this->enableConditioning(false, false);

    this->activateDacExtFilter(true, false);
}

bool MessageDispatcher_Remi8_V01::checkProtocolValidity(string &message) {
    bool validFlag = true;
    message = "Valid protocol";
    switch (selectedProtocol) {
    case ProtocolConstant:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolTriangular:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPk]))) {
            validFlag = false;
            message = "Vhold+Vamp\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]-selectedProtocolVoltage[ProtocolVPk]))) {
            validFlag = false;
            message = "Vhold-Vamp\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange2_10ms].includes(selectedProtocolTime[ProtocolTPe]))) {
            validFlag = false;
            message = "TPeriod\nmust be within [1,1000]ms";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolSquareWave:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]-selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold-Vpulse\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolConductance:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]+
                                                                                    selectedProtocolVoltage[ProtocolVStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Vhold+Vpulse+Vstep(N-1)\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]-selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold-Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]-
                                                                                    selectedProtocolVoltage[ProtocolVStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Vhold+Vpulse-Vstep(N-1)\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust be at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolVariableAmplitude:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]+
                                                                                    selectedProtocolVoltage[ProtocolVStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Vhold+Vpulse+Vstep(N-1)\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust be at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolVariableDuration:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]+selectedProtocolVoltage[ProtocolVPulse]))) {
            validFlag = false;
            message = "Vhold+Vpulse\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTPulse]))) {
            validFlag = false;
            message = "Tpulse\nmust be within [1, 65e3]ms";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRangeSigned2_15].includes(selectedProtocolTime[ProtocolTStep]))) {
            validFlag = false;
            message = "Tstep\nmust be within [-32e3, 32e3]ms";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1orMore].includes(selectedProtocolTime[ProtocolTPulse]+
                                                                               selectedProtocolTime[ProtocolTStep]*(selectedProtocolAdimensional[ProtocolN].value-1.0)))) {
            validFlag = false;
            message = "Tpulse+Tstep(N-1)\nmust be at least 1ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolRamp:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVFinal]))) {
            validFlag = false;
            message = "Vfinal\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVInit]))) {
            validFlag = false;
            message = "Vinit\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTRamp]))) {
            validFlag = false;
            message = "Tramp\nmust be within [1, 65e3]ms";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;

    case ProtocolCyclicVoltammetry:
        if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVHold]))) {
            validFlag = false;
            message = "Vhold\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVFinal]))) {
            validFlag = false;
            message = "Vfinal\nmust be within [-500,500]mV";

        } else if (!(voltageRangesWithOffsetArray[ProtocolVoltageRange500mV].includes(selectedProtocolVoltage[ProtocolVInit]))) {
            validFlag = false;
            message = "Vinit\nmust be within [-500,500]mV";

        } else if (!(protocolTimeRangesArray[ProtocolTimeRange1to2_16].includes(selectedProtocolTime[ProtocolTRamp]))) {
            validFlag = false;
            message = "Tramp\nmust be within [1, 65e3]ms";

        } else if (!(selectedProtocolAdimensional[ProtocolN].value > 0)) {
            validFlag = false;
            message = "N\nmust at least 1";

        } else {
            validFlag = true;
            message = "Valid protocol";
        }
        break;
    }
    return validFlag;
}

void MessageDispatcher_Remi8_V01::updateDeviceStatus(vector <bool> &fsmRunFlag, bool &poreForming, bool &communicationError) {
    for (int idx = 0; idx < fsmStateChannelsNum; idx++) {
        fsmRunFlag[idx] = (infoStruct.status & (0x0001 << idx)) == 0 ? false : true;
    }
    poreForming = (infoStruct.status & (0x0001 << 8)) == 0 ? false : true;
    communicationError = (infoStruct.status & (0x0001 << 9)) == 0 ? false : true;
}

void MessageDispatcher_Remi8_V01::updateVoltageOffsetCompensations(vector <Measurement_t> &offsets) {
    for (int idx = 0; idx < currentChannelsNum; idx++) {
        offsets[idx] = voltageOffsetCompensationGain*(double)infoStruct.vComp[idx];
    }
}
