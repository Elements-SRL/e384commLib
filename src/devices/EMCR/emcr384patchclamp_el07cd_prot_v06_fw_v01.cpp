#include "emcr384patchclamp_el07cd_prot_v06_fw_v01.h"

Emcr384PatchClamp_EL07c_prot_v06_fw_v01::Emcr384PatchClamp_EL07c_prot_v06_fw_v01(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "384PatchClamp";

    fwSize_B = 6313140;
    motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;
    waitingTimeBeforeReadingData = 2; //s

    rxSyncWord = 0x5aa5;

    packetsPerFrame = 1;

    voltageChannelsNum = 384;
    currentChannelsNum = 384;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 24;

    rxWordOffsets[RxMessageDataLoad] = 0;
    rxWordLengths[RxMessageDataLoad] = (voltageChannelsNum+currentChannelsNum)*packetsPerFrame;

    rxWordOffsets[RxMessageDataHeader] = rxWordOffsets[RxMessageDataLoad] + rxWordLengths[RxMessageDataLoad];
    rxWordLengths[RxMessageDataHeader] = 6;

    rxWordOffsets[RxMessageDataTail] = 0xFFFF;
    rxWordLengths[RxMessageDataTail] = 0xFFFF;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageStatus] = 2;

    rxWordOffsets[RxMessageTemperature] = rxWordOffsets[RxMessageStatus] + rxWordLengths[RxMessageStatus];
    rxWordLengths[RxMessageTemperature] = 2;

    rxMaxWords = totalChannelsNum*packetsPerFrame; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 5272;
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    temperatureChannelsNum = TemperatureChannelsNum;

    temperatureChannelsNames.resize(temperatureChannelsNum);
    temperatureChannelsNames[TemperatureSensor0] = "Sensor0";
    temperatureChannelsNames[TemperatureSensor1] = "Sensor1";

    temperatureChannelsRanges.resize(temperatureChannelsNum);
    temperatureChannelsRanges[TemperatureSensor0].step = 0.0625;
    temperatureChannelsRanges[TemperatureSensor0].min = -2048.0;
    temperatureChannelsRanges[TemperatureSensor0].max = temperatureChannelsRanges[TemperatureSensor0].min+temperatureChannelsRanges[TemperatureSensor0].step*USHORT_MAX;
    temperatureChannelsRanges[TemperatureSensor0].prefix = UnitPfxNone;
    temperatureChannelsRanges[TemperatureSensor0].unit = "°C";
    temperatureChannelsRanges[TemperatureSensor1].step = 0.0625;
    temperatureChannelsRanges[TemperatureSensor1].min = -2048.0;
    temperatureChannelsRanges[TemperatureSensor1].max = temperatureChannelsRanges[TemperatureSensor1].min+temperatureChannelsRanges[TemperatureSensor1].step*USHORT_MAX;
    temperatureChannelsRanges[TemperatureSensor1].prefix = UnitPfxNone;
    temperatureChannelsRanges[TemperatureSensor1].unit = "°C";

    properHeaderPackets = true;
    canDoEpisodic = true;

    /*! Clamping modalities */
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = ClampingModality_t::VOLTAGE_CLAMP;
    clampingModalitiesArray[ZeroCurrentClamp] = ClampingModality_t::ZERO_CURRENT_CLAMP;
    clampingModalitiesArray[CurrentClamp] = ClampingModality_t::CURRENT_CLAMP;
    defaultClampingModalityIdx = VoltageClamp;

    /*! Channel sources */
    availableVoltageSourcesIdxs.VoltageFromVoltageClamp = ChannelSourceVoltageFromVoltageClamp;
    availableVoltageSourcesIdxs.VoltageFromCurrentClamp = ChannelSourceVoltageFromCurrentClamp;
//    availableVoltageSourcesIdxs.VoltageFromDynamicClamp = ChannelSourceVoltageFromDynamicClamp;
//    availableVoltageSourcesIdxs.VoltageFromVoltagePlusDynamicClamp = ChannelSourceVoltageFromVoltagePlusDynamicClamp;

    availableCurrentSourcesIdxs.CurrentFromVoltageClamp = ChannelSourceCurrentFromVoltageClamp;
    availableCurrentSourcesIdxs.CurrentFromCurrentClamp = ChannelSourceCurrentFromCurrentClamp;
//    availableCurrentSourcesIdxs.CurrentFromDynamicClamp = ChannelSourceCurrentFromDynamicClamp;
//    availableCurrentSourcesIdxs.CurrentFromCurrentPlusDynamicClamp = ChannelSourceCurrentFromCurrentPlusDynamicClamp;

    /*! Protocols parameters */
    protocolFpgaClockFrequencyHz = 10.0e3;

    protocolTimeRange.step = 1000.0/protocolFpgaClockFrequencyHz;
    protocolTimeRange.min = LINT32_MIN*protocolTimeRange.step;
    protocolTimeRange.max = LINT32_MAX*protocolTimeRange.step;
    protocolTimeRange.prefix = UnitPfxMilli;
    protocolTimeRange.unit = "s";

    positiveProtocolTimeRange = protocolTimeRange;
    positiveProtocolTimeRange.min = 0.0;

    voltageProtocolStepImplemented = true;
    voltageProtocolRampImplemented = true;
    voltageProtocolSinImplemented = false;

    currentProtocolStepImplemented = true;
    currentProtocolRampImplemented = true;
    currentProtocolSinImplemented = false;

    protocolMaxItemsNum = 15;
    protocolWordOffset = 300;
    protocolItemsWordsNum = 16;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange10nA].max = 10.0;
    vcCurrentRangesArray[VCCurrentRange10nA].min = -10.0;
    vcCurrentRangesArray[VCCurrentRange10nA].step = vcCurrentRangesArray[VCCurrentRange10nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange10nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange10nA].unit = "A";
    vcCurrentRangesArray[VCCurrentRange40nALbw].max = 40.0;
    vcCurrentRangesArray[VCCurrentRange40nALbw].min = -40.0;
    vcCurrentRangesArray[VCCurrentRange40nALbw].step = vcCurrentRangesArray[VCCurrentRange40nALbw].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange40nALbw].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange40nALbw].unit = "A";
    vcCurrentRangesArray[VCCurrentRange40nAHbw].max = 40.0;
    vcCurrentRangesArray[VCCurrentRange40nAHbw].min = -40.0;
    vcCurrentRangesArray[VCCurrentRange40nAHbw].step = vcCurrentRangesArray[VCCurrentRange40nAHbw].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange40nAHbw].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange40nAHbw].unit = "A";
    vcCurrentRangesArray[VCCurrentRange400nA].max = 400.0;
    vcCurrentRangesArray[VCCurrentRange400nA].min = -400.0;
    vcCurrentRangesArray[VCCurrentRange400nA].step = vcCurrentRangesArray[VCCurrentRange400nA].max/(SHORT_MAX+1.0);
    vcCurrentRangesArray[VCCurrentRange400nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange400nA].unit = "A";
    defaultVcCurrentRangeIdxs.resize(1);
    defaultVcCurrentRangeIdxs[0] = VCCurrentRange10nA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].max = 512.0;
    vcVoltageRangesArray[VCVoltageRange500mV].min = -512.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.125;
    vcVoltageRangesArray[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange500mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange500mV;

    liquidJunctionRangesNum = vcVoltageRangesNum;
    liquidJunctionRangesArray = vcVoltageRangesArray;
    defaultLiquidJunctionRangeIdx = defaultVcVoltageRangeIdx;

    /*! Current ranges */
    /*! CC */
    ccCurrentRangesNum = CCCurrentRangesNum;
    ccCurrentRangesArray.resize(ccCurrentRangesNum);
    ccCurrentRangesArray[CCCurrentRange8nA].max = 8000.0;
    ccCurrentRangesArray[CCCurrentRange8nA].min = -8000.0;
    ccCurrentRangesArray[CCCurrentRange8nA].step = ccCurrentRangesArray[CCCurrentRange8nA].max/(INT13_MAX+1.0);
    ccCurrentRangesArray[CCCurrentRange8nA].prefix = UnitPfxPico;
    ccCurrentRangesArray[CCCurrentRange8nA].unit = "A";
    ccCurrentRangesArray[CCCurrentRange2nA].max = 2000.0;
    ccCurrentRangesArray[CCCurrentRange2nA].min = -2000.0;
    ccCurrentRangesArray[CCCurrentRange2nA].step = ccCurrentRangesArray[CCCurrentRange2nA].max/(INT13_MAX+1.0);
    ccCurrentRangesArray[CCCurrentRange2nA].prefix = UnitPfxPico;
    ccCurrentRangesArray[CCCurrentRange2nA].unit = "A";
    defaultCcCurrentRangeIdx = CCCurrentRange8nA;

    /*! Voltage ranges */
    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;
    ccVoltageRangesArray.resize(ccVoltageRangesNum);
    ccVoltageRangesArray[CCVoltageRange250mV].max = 250.0;
    ccVoltageRangesArray[CCVoltageRange250mV].min = -250.0;
    ccVoltageRangesArray[CCVoltageRange250mV].step = ccVoltageRangesArray[CCVoltageRange250mV].max/(SHORT_MAX+1.0);
    ccVoltageRangesArray[CCVoltageRange250mV].prefix = UnitPfxMilli;
    ccVoltageRangesArray[CCVoltageRange250mV].unit = "V";
    defaultCcVoltageRangeIdx = CCVoltageRange250mV;

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;
    vcCurrentFiltersArray.resize(vcCurrentFiltersNum);
    vcCurrentFiltersArray[VCCurrentFilter5kHz].value = 5.0;
    vcCurrentFiltersArray[VCCurrentFilter5kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter5kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter6kHz].value = 6;
    vcCurrentFiltersArray[VCCurrentFilter6kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter6kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter10kHz].value = 10.0;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter12kHz].value = 12.0;
    vcCurrentFiltersArray[VCCurrentFilter12kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter12kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter40kHz].value = 40.0;
    vcCurrentFiltersArray[VCCurrentFilter40kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter40kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter48kHz].value = 48.0;
    vcCurrentFiltersArray[VCCurrentFilter48kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter48kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter100kHz].value = 100.0;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter120kHz].value = 120.0;
    vcCurrentFiltersArray[VCCurrentFilter120kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter120kHz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter5kHz;

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter1kHz].value = 1.0;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter1kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter10kHz].value = 10.0;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter10kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter20kHz].value = 20.0;
    vcVoltageFiltersArray[VCVoltageFilter20kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter20kHz].unit = "Hz";
    vcVoltageFiltersArray[VCVoltageFilter50kHz].value = 50.0;
    vcVoltageFiltersArray[VCVoltageFilter50kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter50kHz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter1kHz;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;
    ccCurrentFiltersArray.resize(ccCurrentFiltersNum);
    ccCurrentFiltersArray[CCCurrentFilter1kHz].value = 1.0;
    ccCurrentFiltersArray[CCCurrentFilter1kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter1kHz].unit = "Hz";
    ccCurrentFiltersArray[CCCurrentFilter10kHz].value = 10.0;
    ccCurrentFiltersArray[CCCurrentFilter10kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter10kHz].unit = "Hz";
    ccCurrentFiltersArray[CCCurrentFilter20kHz].value = 20.0;
    ccCurrentFiltersArray[CCCurrentFilter20kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter20kHz].unit = "Hz";
    ccCurrentFiltersArray[CCCurrentFilter50kHz].value = 50.0;
    ccCurrentFiltersArray[CCCurrentFilter50kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter50kHz].unit = "Hz";
    defaultCcCurrentFilterIdx = CCCurrentFilter1kHz;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;
    ccVoltageFiltersArray.resize(ccVoltageFiltersNum);
    ccVoltageFiltersArray[CCVoltageFilter5kHz].value = 5.0;
    ccVoltageFiltersArray[CCVoltageFilter5kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter5kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter6kHz].value = 6;
    ccVoltageFiltersArray[CCVoltageFilter6kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter6kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter10kHz].value = 10.0;
    ccVoltageFiltersArray[CCVoltageFilter10kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter10kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter12kHz].value = 12.0;
    ccVoltageFiltersArray[CCVoltageFilter12kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter12kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter40kHz].value = 40.0;
    ccVoltageFiltersArray[CCVoltageFilter40kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter40kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter48kHz].value = 48.0;
    ccVoltageFiltersArray[CCVoltageFilter48kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter48kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter100kHz].value = 100.0;
    ccVoltageFiltersArray[CCVoltageFilter100kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter100kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter120kHz].value = 120.0;
    ccVoltageFiltersArray[CCVoltageFilter120kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter120kHz].unit = "Hz";
    defaultCcVoltageFilterIdx = CCVoltageFilter5kHz;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    defaultSamplingRateIdx = SamplingRate5kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate5kHz].value = 5.0;
    realSamplingRatesArray[SamplingRate5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate5kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate10kHz].value = 10.0;
    realSamplingRatesArray[SamplingRate10kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate10kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate20kHz].value = 20.0;
    realSamplingRatesArray[SamplingRate20kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate20kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate40kHz].value = 40.0;
    realSamplingRatesArray[SamplingRate40kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate40kHz].unit = "Hz";
    realSamplingRatesArray[SamplingRate80kHz].value = 80.0;
    realSamplingRatesArray[SamplingRate80kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate80kHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate5kHz] = 0;
    sr2srm[SamplingRate10kHz] = 0;
    sr2srm[SamplingRate20kHz] = 0;
    sr2srm[SamplingRate40kHz] = 1;
    sr2srm[SamplingRate80kHz] = 1;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate5kHz].value = 200.0;
    integrationStepArray[SamplingRate5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5kHz].unit = "s";
    integrationStepArray[SamplingRate10kHz].value = 100.0;
    integrationStepArray[SamplingRate10kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate10kHz].unit = "s";
    integrationStepArray[SamplingRate20kHz].value = 50.0;
    integrationStepArray[SamplingRate20kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate20kHz].unit = "s";
    integrationStepArray[SamplingRate40kHz].value = 25.0;
    integrationStepArray[SamplingRate40kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate40kHz].unit = "s";
    integrationStepArray[SamplingRate80kHz].value = 12.5;
    integrationStepArray[SamplingRate80kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate80kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate5kHz, VCCurrentFilter5kHz},
        {SamplingRate10kHz, VCCurrentFilter6kHz},
        {SamplingRate20kHz, VCCurrentFilter12kHz},
        {SamplingRate40kHz, VCCurrentFilter40kHz},
        {SamplingRate80kHz, VCCurrentFilter48kHz}
    };

    // mapping ADC Current Clamp
    sr2LpfCcVoltageMap = {
        {SamplingRate5kHz,  CCVoltageFilter5kHz},
        {SamplingRate10kHz, CCVoltageFilter6kHz},
        {SamplingRate20kHz, CCVoltageFilter12kHz},
        {SamplingRate40kHz, CCVoltageFilter40kHz},
        {SamplingRate80kHz, CCVoltageFilter48kHz}
    };

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultCurrentHoldTuner = {0.0, ccCurrentRangesArray[CCCurrentRange8nA].prefix, ccCurrentRangesArray[CCCurrentRange8nA].unit};

    defaultVoltageHalfTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultCurrentHalfTuner = {0.0, ccCurrentRangesArray[CCCurrentRange8nA].prefix, ccCurrentRangesArray[CCCurrentRange8nA].unit};

    /*! Zap */
    zapDurationRange.step = 0.1;
    zapDurationRange.min = 0.0;
    zapDurationRange.max = zapDurationRange.min+zapDurationRange.step*(double)UINT16_MAX;
    zapDurationRange.prefix = UnitPfxMilli;
    zapDurationRange.unit = "s";

    /*! VC leak calibration (shunt resistance)*/
    rRShuntConductanceCalibRange.resize(VCCurrentRangesNum);
    rRShuntConductanceCalibRange[VCCurrentRange10nA].step = (vcCurrentRangesArray[VCCurrentRange10nA].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange10nA].min = -8.0*(vcCurrentRangesArray[VCCurrentRange10nA].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange10nA].max = 8.0*(vcCurrentRangesArray[VCCurrentRange10nA].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange10nA].step;
    rRShuntConductanceCalibRange[VCCurrentRange10nA].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange10nA].unit = "S";
    rRShuntConductanceCalibRange[VCCurrentRange40nALbw].step = (vcCurrentRangesArray[VCCurrentRange40nALbw].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange40nALbw].min = -8.0*(vcCurrentRangesArray[VCCurrentRange40nALbw].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange40nALbw].max = 8.0*(vcCurrentRangesArray[VCCurrentRange40nALbw].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange40nALbw].step;
    rRShuntConductanceCalibRange[VCCurrentRange40nALbw].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange40nALbw].unit = "S";
    rRShuntConductanceCalibRange[VCCurrentRange40nAHbw].step = (vcCurrentRangesArray[VCCurrentRange40nAHbw].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange40nAHbw].min = -8.0*(vcCurrentRangesArray[VCCurrentRange40nAHbw].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange40nAHbw].max = 8.0*(vcCurrentRangesArray[VCCurrentRange40nAHbw].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange40nAHbw].step;
    rRShuntConductanceCalibRange[VCCurrentRange40nAHbw].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange40nAHbw].unit = "S";
    rRShuntConductanceCalibRange[VCCurrentRange400nA].step = (vcCurrentRangesArray[VCCurrentRange400nA].step/vcVoltageRangesArray[0].step)/16384.0;
    rRShuntConductanceCalibRange[VCCurrentRange400nA].min = -8.0*(vcCurrentRangesArray[VCCurrentRange400nA].step/vcVoltageRangesArray[0].step);
    rRShuntConductanceCalibRange[VCCurrentRange400nA].max = 8.0*(vcCurrentRangesArray[VCCurrentRange400nA].step/vcVoltageRangesArray[0].step) - rRShuntConductanceCalibRange[VCCurrentRange400nA].step;
    rRShuntConductanceCalibRange[VCCurrentRange400nA].prefix = UnitPfxMicro;
    rRShuntConductanceCalibRange[VCCurrentRange400nA].unit = "S";

    /*! VC voltage calib gain (DAC) */
    calibVcVoltageGainRange.step = 1.0/1024.0;
    calibVcVoltageGainRange.min = 0;
    calibVcVoltageGainRange.max = SHORT_MAX * calibVcVoltageGainRange.step;
    calibVcVoltageGainRange.prefix = UnitPfxNone;
    calibVcVoltageGainRange.unit = "";

    /*! CC Current calib gain (DAC) */
    calibCcCurrentGainRange.step = 1.0/1024.0;
    calibCcCurrentGainRange.min = 0;
    calibCcCurrentGainRange.max = SHORT_MAX * calibCcCurrentGainRange.step;
    calibCcCurrentGainRange.prefix = UnitPfxNone;
    calibCcCurrentGainRange.unit = "";

    /*! VC current calib gain (ADC) */
    calibVcCurrentGainRange.step = 1.0/1024.0;
    calibVcCurrentGainRange.min = 0;
    calibVcCurrentGainRange.max = SHORT_MAX * calibVcCurrentGainRange.step;
    calibVcCurrentGainRange.prefix = UnitPfxNone;
    calibVcCurrentGainRange.unit = "";

    /*! CC voltage calib gain (ADC) */
    calibCcVoltageGainRange.step = 1.0/1024.0;
    calibCcVoltageGainRange.min = 0;
    calibCcVoltageGainRange.max = SHORT_MAX * calibCcVoltageGainRange.step;
    calibCcVoltageGainRange.prefix = UnitPfxNone;
    calibCcVoltageGainRange.unit = "";

    /*! VC Voltage calib offset (DAC)*/
    calibVcVoltageOffsetRanges = vcVoltageRangesArray;

    /*! CC current calib offset (DAC)*/
    calibCcCurrentOffsetRanges = ccCurrentRangesArray;

    /*! VC current calib offset (ADC)*/
    calibVcCurrentOffsetRanges = vcCurrentRangesArray;

    /*! CC voltage calib offset (ADC)*/
    calibCcVoltageOffsetRanges = ccVoltageRangesArray;


    /*! Compensations */
    /*! compValueMatrix contains one vector of compensation values for each of the channels (e.g. 384 channels) */
    compValueMatrix.resize(currentChannelsNum, std::vector <double>(CompensationUserParamsNum));
    selectedRsCorrBws.resize(currentChannelsNum);

    /*! Compensation type enables, one element per channel*/
    compensationsEnableFlags[CompCfast].resize(currentChannelsNum);
    compensationsEnableFlags[CompCslow].resize(currentChannelsNum);
    compensationsEnableFlags[CompRsCorr].resize(currentChannelsNum);
    compensationsEnableFlags[CompRsPred].resize(currentChannelsNum);
    compensationsEnableFlags[CompCcCfast].resize(currentChannelsNum);
    fill(compensationsEnableFlags[CompCfast].begin(), compensationsEnableFlags[CompCfast].end(), false);
    fill(compensationsEnableFlags[CompCslow].begin(), compensationsEnableFlags[CompCslow].end(), false);
    fill(compensationsEnableFlags[CompRsCorr].begin(), compensationsEnableFlags[CompRsCorr].end(), false);
    fill(compensationsEnableFlags[CompRsPred].begin(), compensationsEnableFlags[CompRsPred].end(), false);
    fill(compensationsEnableFlags[CompCcCfast].begin(), compensationsEnableFlags[CompCcCfast].end(), false);

    /*! FEATURES ASIC DOMAIN Pipette capacitance */
    const double pipetteVarConductance = 320.0; // uS
    const double pipetteFixedResistance1 = 100.0e-3; //MOhm
    const double pipetteFixedResistance2 = 5.92592592592592e-3; //MOhm
    const int pipetteCapacitanceRanges = 3;
    const double pipetteCapacitanceValuesNum = 256.0;

    std::vector <double> pipetteInjCapacitance = {3.0, 9.0, 27.0};
    pipetteCapacitanceRange.resize(pipetteCapacitanceRanges);
    for (int idx = 0; idx < pipetteCapacitanceRanges; idx++) {
        pipetteCapacitanceRange[idx].step = pipetteVarConductance/pipetteCapacitanceValuesNum*pipetteFixedResistance2*pipetteInjCapacitance[idx];
        pipetteCapacitanceRange[idx].min = (pipetteVarConductance/pipetteCapacitanceValuesNum+1.0/pipetteFixedResistance1)*pipetteFixedResistance2*pipetteInjCapacitance[idx];
        pipetteCapacitanceRange[idx].max = pipetteCapacitanceRange[idx].min+(pipetteCapacitanceValuesNum-1.0)*pipetteCapacitanceRange[idx].step;
        pipetteCapacitanceRange[idx].prefix = UnitPfxPico;
        pipetteCapacitanceRange[idx].unit = "F";
    }

    ccPipetteCapacitanceRange.resize(pipetteCapacitanceRanges);
    for (int idx = 0; idx < pipetteCapacitanceRanges; idx++) {
        ccPipetteCapacitanceRange[idx].step = (4.0*pipetteVarConductance/pipetteCapacitanceValuesNum*pipetteFixedResistance2)*pipetteInjCapacitance[idx];
        ccPipetteCapacitanceRange[idx].min = (4.0*(pipetteVarConductance/pipetteCapacitanceValuesNum+1.0/pipetteFixedResistance1)*pipetteFixedResistance2+3.0)*pipetteInjCapacitance[idx];
        ccPipetteCapacitanceRange[idx].max = ccPipetteCapacitanceRange[idx].min+(pipetteCapacitanceValuesNum-1.0)*ccPipetteCapacitanceRange[idx].step;
        ccPipetteCapacitanceRange[idx].prefix = UnitPfxPico;
        ccPipetteCapacitanceRange[idx].unit = "F";
    }

    /*! FEATURES ASIC DOMAIN Membrane capacitance*/
    const double membraneVarConductance = 320.0; // uS
    const double membraneFixedResistance8 = 61.5e-3; //MOhm
    const double membraneFixedResistance9 = 5.5e-3; //MOhm
    const int membraneCapValueRanges = 4;
    const double membraneCapacitanceValuesNum = 256.0;

    membraneCapValueInjCapacitance = {100.0/33.0, 400.0/33.0, 1600.0/33.0, 1600.0/11.0};
    membraneCapValueRange.resize(membraneCapValueRanges);
    for (int idx = 0; idx < membraneCapValueRanges; idx++) {
        membraneCapValueRange[idx].step = membraneVarConductance/membraneCapacitanceValuesNum*membraneFixedResistance9*membraneCapValueInjCapacitance[idx];
        membraneCapValueRange[idx].min = (membraneVarConductance/membraneCapacitanceValuesNum+1.0/membraneFixedResistance8)*membraneFixedResistance9*membraneCapValueInjCapacitance[idx];
        membraneCapValueRange[idx].max = membraneCapValueRange[idx].min+(membraneCapacitanceValuesNum-1.0)*membraneCapValueRange[idx].step;
        membraneCapValueRange[idx].prefix = UnitPfxPico;
        membraneCapValueRange[idx].unit = "F";
    }

    /*! FEATURES ASIC DOMAIN Membrane capacitance TAU*/
    const int membraneCapTauValueRanges = 2;
    const double membraneCapTauValueVarResistance_MOhm = 51.2 / this->clockRatio; /*! affected by switch cap clock!!!!!*/
    const double membraneCapTauValueValuesNum = 256.0; // 8 bits

    std::vector <double> membraneCapTauValueCapacitance = {2.5, 25.0};
    /*! \todo FCON recheck, now trying to use ranged measurement fo Features  */
    membraneCapTauValueRange.resize(membraneCapTauValueRanges);
    for (int idx = 0; idx < membraneCapTauValueRanges; idx++) {
        membraneCapTauValueRange[idx].step = membraneCapTauValueVarResistance_MOhm * membraneCapTauValueCapacitance[idx] / membraneCapTauValueValuesNum;
        membraneCapTauValueRange[idx].min = membraneCapTauValueRange[idx].step;
        membraneCapTauValueRange[idx].max = membraneCapTauValueRange[idx].min + (membraneCapTauValueValuesNum - 1.0) * membraneCapTauValueRange[idx].step;
        membraneCapTauValueRange[idx].prefix = UnitPfxMicro;
        membraneCapTauValueRange[idx].unit = "s";
    }

    /*! FEATURES ASIC DOMAIN Rs correction*/
    rsCorrValueRange.step = 0.4; // MOhm
    rsCorrValueRange.min = 0.4; // MOhm
    rsCorrValueRange.max = 25.6; // MOhm
    rsCorrValueRange.prefix = UnitPfxMega;
    rsCorrValueRange.unit = "Ohm";

    /*! FEATURES ASIC DOMAIN Rs prediction GAIN*/
    const double rsPredGainValuesNum = 64.0;
    rsPredGainRange.step = 1.0/16.0;
    rsPredGainRange.min = 1.0 + rsPredGainRange.step;
    rsPredGainRange.max = rsPredGainRange.min + rsPredGainRange.step * (rsPredGainValuesNum-1);
    rsPredGainRange.prefix = UnitPfxNone;
    rsPredGainRange.unit = "";

    /*! FEATURES ASIC DOMAIN Rs prediction TAU*/
    const double rsPredTauValuesNum = 256.0;
    rsPredTauRange.step = 2.0 / this->clockRatio; /*! affected by switch cap clock!!!!!*/
    rsPredTauRange.min = rsPredTauRange.step;
    rsPredTauRange.max = rsPredTauRange.min + rsPredTauRange.step * (rsPredTauValuesNum -1) ;
    rsPredTauRange.prefix = UnitPfxMicro;
    rsPredTauRange.unit = "s";

    /*! \todo FCON inizializzare con valori di default per prima attivazione GUI*/
    CompensationControl_t control;
    std::vector <CompensationUserParams_t> availableCompensationsControls = {U_CpVc, U_Cm, U_Rs, U_RsCp, U_RsPg, U_CpCc};
    for (auto param : availableCompensationsControls) {
        compensationControls[param].resize(currentChannelsNum);
        this->getCompensationControl(param, control);
        std::fill(compensationControls[param].begin(), compensationControls[param].end(), control);
    }

    /*! COMPENSATION OPTIONS STRINGS*/
    compensationOptionStrings.resize(CompensationTypesNum);
    compensationOptionStrings[CompRsCorr].resize(CompensationRsCorrBwNum);

    rsCorrBwArray.resize(CompensationRsCorrBwNum);
    rsCorrBwArray[CompensationRsCorrBw53_1kHz].value = 53.1 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw53_1kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw53_1kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw31_8kHz].value = 31.8 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw31_8kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw31_8kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw17_7kHz].value = 17.7 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw17_7kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw17_7kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw9_36kHz].value = 9.36 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw9_36kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw9_36kHz].unit = "Hz";
    defaultRsCorrBwIdx = CompensationRsCorrBw53_1kHz;

    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw53_1kHz] = rsCorrBwArray[CompensationRsCorrBw53_1kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw31_8kHz] = rsCorrBwArray[CompensationRsCorrBw31_8kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw17_7kHz] = rsCorrBwArray[CompensationRsCorrBw17_7kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw9_36kHz] = rsCorrBwArray[CompensationRsCorrBw9_36kHz].niceLabel();

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

    // Default USER DOMAIN compensation parameters
    defaultUserDomainParams.resize(CompensationUserParamsNum);
    defaultUserDomainParams[U_CpVc] = pipetteCapacitanceRange[0].min;
    defaultUserDomainParams[U_Cm] = membraneCapValueRange[0].min;
    defaultUserDomainParams[U_Rs] = membraneCapTauValueRange[0].min/membraneCapValueRange[0].min;
    defaultUserDomainParams[U_RsCp] = 1.0;
    defaultUserDomainParams[U_RsPg] = rsPredGainRange.min;
    defaultUserDomainParams[U_CpCc] = ccPipetteCapacitanceRange[0].min;

    // Selected default Idx
    selectedVcVoltageRangeIdx = defaultVcVoltageRangeIdx;
    selectedVcCurrentFilterIdx = defaultVcCurrentFilterIdx;
    selectedSamplingRateIdx = defaultSamplingRateIdx;

    // Initialization of the USER compensation domain with standard parameters
    for (int i = 0; i < currentChannelsNum; i++) {
        compValueMatrix[i][U_CpVc] =  defaultUserDomainParams[U_CpVc];
        compValueMatrix[i][U_Cm] =  defaultUserDomainParams[U_Cm];
        compValueMatrix[i][U_Rs] =  defaultUserDomainParams[U_Rs];
        compValueMatrix[i][U_RsCp] =  defaultUserDomainParams[U_RsCp];
        compValueMatrix[i][U_RsPg] =  defaultUserDomainParams[U_RsPg];
        compValueMatrix[i][U_CpCc] =  defaultUserDomainParams[U_CpCc];
    }

    // Initialization of the RsCorr bandwidth option with default option
    for (int i = 0; i < currentChannelsNum; i++) {
        selectedRsCorrBws[i] = defaultRsCorrBwIdx;
    }

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;
    MultiCoder::CoderConfig_t multiConfig;

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
    boolConfig.bitsNum = 6;
    samplingRateCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(32); /* 5kHz  0xb100000 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(33); /* 10kHz 0xb100001 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(34); /* 20kHz 0xb100010 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(19); /* 40kHz 0xb010011 */
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(20); /* 80kHz 0xb010100 */
    coders.push_back(samplingRateCoder);

    /*! Clamping mode */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 9;
    boolConfig.bitsNum = 2;
    clampingModeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(clampingModeCoder);

    /*! Protocol reset */
    boolConfig.initialWord = 4;
    boolConfig.initialBit = 14;
    boolConfig.bitsNum = 1;
    protocolResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(protocolResetCoder);

    /*! Voltage Source */
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    sourceForVoltageChannelCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(sourceForVoltageChannelCoder);

    /*! Current Source */
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    sourceForCurrentChannelCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(sourceForCurrentChannelCoder);

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
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 4;
    ccCurrentRangeCoders.clear();
    ccCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(ccCurrentRangeCoders[0]);

    /*! Voltage range CC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 4;
    ccVoltageRangeCoders.clear();
    ccVoltageRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(ccVoltageRangeCoders[0]);

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
    vcVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 4;
    ccCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccCurrentFilterCoder);

    /*! Voltage filter CC */
    boolConfig.initialWord = 11;
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 4;
    ccVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccVoltageFilterCoder);

    /*! Liquid junction compensation */
    boolConfig.initialWord = 12;
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

    /*! Enable stimulus */
    boolConfig.initialWord = 36;
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
    boolConfig.initialWord = 180;
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

    doubleConfig.initialWord = 5;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = zapDurationRange.step;
    doubleConfig.minValue = zapDurationRange.min;
    doubleConfig.maxValue = zapDurationRange.max;
    zapDurationCoder = new DoubleOffsetBinaryCoder(doubleConfig);
    coders.push_back(zapDurationCoder);

    /*! Turn channels on */
    boolConfig.initialWord = 60;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    turnChannelsOnCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        turnChannelsOnCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(turnChannelsOnCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! CAL_SW */
    boolConfig.initialWord = 108;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    calSwCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calSwCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(calSwCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! GR_En */
    boolConfig.initialWord = 132;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    grEnCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        grEnCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(grEnCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! VC_SW */
    boolConfig.initialWord = 204;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    vcSwCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        vcSwCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(vcSwCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! CC_SW */
    boolConfig.initialWord = 228;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    ccSwCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        ccSwCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(ccSwCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! VC_CC_SEL */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 11;
    boolConfig.bitsNum = 2;
    vcCcSelCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        vcCcSelCoders[idx] = new BoolRandomArrayCoder(boolConfig);
        static_cast <BoolRandomArrayCoder *> (vcCcSelCoders[idx])->addMapItem(0x2); // set only the Cfast_SW
        static_cast <BoolRandomArrayCoder *> (vcCcSelCoders[idx])->addMapItem(0x1); // set only the VC_CC_sel
        coders.push_back(vcCcSelCoders[idx]);
        // VC_CC_sel unified
    }

    /*! CC_Stim_En */
    boolConfig.initialWord = 276;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    ccStimEnCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        ccStimEnCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(ccStimEnCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

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

    doubleConfig.initialWord = protocolWordOffset+3;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    currentProtocolRestCoders.resize(CCCurrentRangesNum);

    for (unsigned int rangeIdx = 0; rangeIdx < ccCurrentRangesNum; rangeIdx++) {
        doubleConfig.resolution = ccCurrentRangesArray[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*32768.0;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;
        currentProtocolRestCoders[rangeIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(currentProtocolRestCoders[rangeIdx]);
    }

    /*! Protocol items */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
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

            doubleConfig.initialWord = protocolWordOffset+6+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim0StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim0StepCoders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+8+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim1Coders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim1Coders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+10+protocolItemsWordsNum*itemIdx;
            voltageProtocolStim1StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(voltageProtocolStim1StepCoders[rangeIdx][itemIdx]);
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    currentProtocolStim0Coders.resize(CCCurrentRangesNum);
    currentProtocolStim0StepCoders.resize(CCCurrentRangesNum);
    currentProtocolStim1Coders.resize(CCCurrentRangesNum);
    currentProtocolStim1StepCoders.resize(CCCurrentRangesNum);

    for (unsigned int rangeIdx = 0; rangeIdx < ccCurrentRangesNum; rangeIdx++) {
        currentProtocolStim0Coders[rangeIdx].resize(protocolMaxItemsNum);
        currentProtocolStim0StepCoders[rangeIdx].resize(protocolMaxItemsNum);
        currentProtocolStim1Coders[rangeIdx].resize(protocolMaxItemsNum);
        currentProtocolStim1StepCoders[rangeIdx].resize(protocolMaxItemsNum);

        doubleConfig.resolution = ccCurrentRangesArray[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*32768.0;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*65535.0;

        for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
            doubleConfig.initialWord = protocolWordOffset+4+protocolItemsWordsNum*itemIdx;
            currentProtocolStim0Coders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(currentProtocolStim0Coders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+6+protocolItemsWordsNum*itemIdx;
            currentProtocolStim0StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(currentProtocolStim0StepCoders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+8+protocolItemsWordsNum*itemIdx;
            currentProtocolStim1Coders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(currentProtocolStim1Coders[rangeIdx][itemIdx]);

            doubleConfig.initialWord = protocolWordOffset+10+protocolItemsWordsNum*itemIdx;
            currentProtocolStim1StepCoders[rangeIdx][itemIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(currentProtocolStim1StepCoders[rangeIdx][itemIdx]);
        }
    }

    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 32;
    doubleConfig.resolution = positiveProtocolTimeRange.step;
    doubleConfig.minValue = positiveProtocolTimeRange.min;
    doubleConfig.maxValue = positiveProtocolTimeRange.max;
    protocolTime0Coders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        doubleConfig.initialWord = protocolWordOffset+12+protocolItemsWordsNum*itemIdx;
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
        doubleConfig.initialWord = protocolWordOffset+14+protocolItemsWordsNum*itemIdx;
        protocolTime0StepCoders[itemIdx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(protocolTime0StepCoders[itemIdx]);
    }

    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 16;
    protocolItemIdxCoders.resize(protocolMaxItemsNum);
    protocolNextItemIdxCoders.resize(protocolMaxItemsNum);
    protocolLoopRepetitionsCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+16+protocolItemsWordsNum*itemIdx;
        protocolItemIdxCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolItemIdxCoders[itemIdx]);

        boolConfig.initialWord = protocolWordOffset+17+protocolItemsWordsNum*itemIdx;
        protocolNextItemIdxCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolNextItemIdxCoders[itemIdx]);

        boolConfig.initialWord = protocolWordOffset+18+protocolItemsWordsNum*itemIdx;
        protocolLoopRepetitionsCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolLoopRepetitionsCoders[itemIdx]);
    }

    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    protocolApplyStepsCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+19+protocolItemsWordsNum*itemIdx;
        protocolApplyStepsCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolApplyStepsCoders[itemIdx]);
    }

    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    protocolStimHalfCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+19+protocolItemsWordsNum*itemIdx;
        protocolStimHalfCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolStimHalfCoders[itemIdx]);
    }

    boolConfig.initialBit = 2;
    boolConfig.bitsNum = 4;
    protocolItemTypeCoders.resize(protocolMaxItemsNum);

    for (unsigned int itemIdx = 0; itemIdx < protocolMaxItemsNum; itemIdx++) {
        boolConfig.initialWord = protocolWordOffset+19+protocolItemsWordsNum*itemIdx;
        protocolItemTypeCoders[itemIdx] = new BoolArrayCoder(boolConfig);
        coders.push_back(protocolItemTypeCoders[itemIdx]);
    }

    /*! V holding tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vHoldTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 544;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! C holding tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    cHoldTunerCoders.resize(CCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < CCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 544;
        doubleConfig.resolution = ccCurrentRangesArray[rangeIdx].step;
        doubleConfig.minValue = ccCurrentRangesArray[rangeIdx].min;
        doubleConfig.maxValue = ccCurrentRangesArray[rangeIdx].max;
        cHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            cHoldTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(cHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! V half tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vHalfTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 928;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = vcVoltageRangesArray[rangeIdx].min;
        doubleConfig.maxValue = vcVoltageRangesArray[rangeIdx].max;
        vHalfTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHalfTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(vHalfTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! C half tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    cHalfTunerCoders.resize(CCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < CCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 928;
        doubleConfig.resolution = ccCurrentRangesArray[rangeIdx].step;
        doubleConfig.minValue = ccCurrentRangesArray[rangeIdx].min;
        doubleConfig.maxValue = ccCurrentRangesArray[rangeIdx].max;
        cHalfTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            cHalfTunerCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(cHalfTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! liquid junction voltage */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    liquidJunctionVoltageCoders.resize(liquidJunctionRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < liquidJunctionRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 4888;
        doubleConfig.resolution = liquidJunctionRangesArray[rangeIdx].step;
        doubleConfig.minValue = liquidJunctionRangesArray[rangeIdx].min;
        doubleConfig.maxValue = liquidJunctionRangesArray[rangeIdx].max;
        liquidJunctionVoltageCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            liquidJunctionVoltageCoders[rangeIdx][channelIdx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(liquidJunctionVoltageCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! VC leak calibration */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    calibRShuntConductanceCoders.resize(VCCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 1504;
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

    /*! DAC gain e offset*/
    /*! VC Voltage gain tuner */
    doubleConfig.initialWord = 1888;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibVcVoltageGainRange.step;
    doubleConfig.minValue = calibVcVoltageGainRange.min;
    doubleConfig.maxValue = calibVcVoltageGainRange.max;
    calibVcVoltageGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibVcVoltageGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(calibVcVoltageGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! VC Voltage offset tuner */
    calibVcVoltageOffsetCoders.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2272;
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

    /*! CC Current gain tuner */
    doubleConfig.initialWord = 1888;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibCcCurrentGainRange.step;
    doubleConfig.minValue = calibCcCurrentGainRange.min;
    doubleConfig.maxValue = calibCcCurrentGainRange.max;
    calibCcCurrentGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibCcCurrentGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(calibCcCurrentGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! CC Voltage offset tuner */
    calibCcCurrentOffsetCoders.resize(ccCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 2272;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibCcCurrentOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibCcCurrentOffsetRanges[rangeIdx].min;
        doubleConfig.maxValue = calibCcCurrentOffsetRanges[rangeIdx].max;
        calibCcCurrentOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibCcCurrentOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibCcCurrentOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }

    /*! ADC gain e offset*/
    /*! \note MPAC sebbene nel calibratore si cicli sui range per calcolare i gainADC, questi non dipendono dai range essendo numeri puri. Il ciclo sui
    range serve solo per selezionare gli step di corrente range-specifici*/
    /*! VC current gain tuner */
    doubleConfig.initialWord = 2656;
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

    /*! VC current offset tuner */
    calibVcCurrentOffsetCoders.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 3040;
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

    /*! CC Voltage gain tuner */
    doubleConfig.initialWord = 2656;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    doubleConfig.resolution = calibCcVoltageGainRange.step;
    doubleConfig.minValue = calibCcVoltageGainRange.min;
    doubleConfig.maxValue = calibCcVoltageGainRange.max;
    calibCcVoltageGainCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        calibCcVoltageGainCoders[idx] = new DoubleTwosCompCoder(doubleConfig);
        coders.push_back(calibCcVoltageGainCoders[idx]);
        doubleConfig.initialWord++;
    }

    /*! CC Voltage offset tuner */
    calibCcVoltageOffsetCoders.resize(ccVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 3040;
        doubleConfig.initialBit = 0;
        doubleConfig.bitsNum = 16;
        doubleConfig.resolution = calibCcVoltageOffsetRanges[rangeIdx].step;
        doubleConfig.minValue = calibCcVoltageOffsetRanges[rangeIdx].min;
        doubleConfig.maxValue = calibCcVoltageOffsetRanges[rangeIdx].max;
        calibCcVoltageOffsetCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            calibCcVoltageOffsetCoders[rangeIdx][idx] = new DoubleTwosCompCoder(doubleConfig);
            coders.push_back(calibCcVoltageOffsetCoders[rangeIdx][idx]);
            doubleConfig.initialWord++;
        }
    }

    /*! Compensation coders*/
    /*! Cfast / pipette capacitance compensation ENABLE */
    boolConfig.initialWord = 3424;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    pipetteCapEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        pipetteCapEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(pipetteCapEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Cfast / pipette capacitance compensation VALUE */
    pipetteCapValCompensationMultiCoders.resize(currentChannelsNum);

    boolConfig.initialWord = 3640;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 3448;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    multiConfig.doubleCoderVector.resize(pipetteCapacitanceRanges);
    multiConfig.thresholdVector.resize(pipetteCapacitanceRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        multiConfig.boolCoder = new BoolRandomArrayCoder(boolConfig);
        static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x0);
        static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x1);
        static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x3);
        coders.push_back(multiConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < pipetteCapacitanceRanges; rangeIdx++) {
            doubleConfig.minValue = pipetteCapacitanceRange[rangeIdx].min;
            doubleConfig.maxValue = pipetteCapacitanceRange[rangeIdx].max;
            doubleConfig.resolution = pipetteCapacitanceRange[rangeIdx].step;

            multiConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < pipetteCapacitanceRanges-1) {
                multiConfig.thresholdVector[rangeIdx] = pipetteCapacitanceRange[rangeIdx].max + pipetteCapacitanceRange[rangeIdx].step;
            }
        }
        pipetteCapValCompensationMultiCoders[idx] = new MultiCoder(multiConfig);
        coders.push_back(pipetteCapValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 2;
        if (boolConfig.initialBit > 14) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cfast value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8) {
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation ENABLE */
    boolConfig.initialWord = 3688;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    membraneCapEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        membraneCapEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(membraneCapEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation */
    membraneCapValCompensationMultiCoders.resize(currentChannelsNum);

    boolConfig.initialWord = 3904;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 3712;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    multiConfig.doubleCoderVector.resize(membraneCapValueRanges);
    multiConfig.thresholdVector.resize(membraneCapValueRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        multiConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < membraneCapValueRanges; rangeIdx++) {
            doubleConfig.minValue = membraneCapValueRange[rangeIdx].min;
            doubleConfig.maxValue = membraneCapValueRange[rangeIdx].max;
            doubleConfig.resolution = membraneCapValueRange[rangeIdx].step;

            multiConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < membraneCapValueRanges-1) {
                multiConfig.thresholdVector[rangeIdx] = membraneCapValueRange[rangeIdx].max + membraneCapValueRange[rangeIdx].step;
            }
        }
        membraneCapValCompensationMultiCoders[idx] = new MultiCoder(multiConfig);
        coders.push_back(membraneCapValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 2;
        if (boolConfig.initialBit > 14) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cslow value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8) {
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation TAU and TAU RANGES */
    membraneCapTauValCompensationMultiCoders.resize(currentChannelsNum);

    boolConfig.initialWord = 4144;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;

    doubleConfig.initialWord = 3952;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    multiConfig.doubleCoderVector.resize(membraneCapTauValueRanges);
    multiConfig.thresholdVector.resize(membraneCapTauValueRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        multiConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < membraneCapTauValueRanges; rangeIdx++) {
            doubleConfig.minValue =  membraneCapTauValueRange[rangeIdx].min;
            doubleConfig.maxValue = membraneCapTauValueRange[rangeIdx].max;
            doubleConfig.resolution = membraneCapTauValueRange[rangeIdx].step;

            multiConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < membraneCapTauValueRanges-1) {
                multiConfig.thresholdVector[rangeIdx] = membraneCapTauValueRange[rangeIdx].max + membraneCapTauValueRange[rangeIdx].step;
            }
        }
        membraneCapTauValCompensationMultiCoders[idx] = new MultiCoder(multiConfig);
        coders.push_back(membraneCapTauValCompensationMultiCoders[idx]);


        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8) {
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }

        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs correction compensation ENABLE*/
    boolConfig.initialWord = 4168;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    rsCorrEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsCorrEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(rsCorrEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs correction compensation VALUE*/
    doubleConfig.initialWord = 4192;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;
    doubleConfig.resolution = rsCorrValueRange.step;
    doubleConfig.minValue = rsCorrValueRange.min;
    doubleConfig.maxValue = rsCorrValueRange.max;
    rsCorrValCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsCorrValCompensationCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(rsCorrValCompensationCoders[idx]);
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8) {
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Rs correction compensation BANDWIDTH */
    /*! \todo QUESTO VIENE IMPATTATO DALLA SWITCHED CAP FREQUENCY SOLO  A LIVELLO DI RAPPRESENTAZINE DI STRINGHE PER LA BANDA NELLA GUI. ATTIVAMENTE QUI NN FACCIAMO NULLA!!!!!*/
    boolConfig.initialWord = 4384;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 3;
    rsCorrBwCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsCorrBwCompensationCoders[idx] = new BoolRandomArrayCoder(boolConfig);
        static_cast <BoolRandomArrayCoder *> (rsCorrBwCompensationCoders[idx])->addMapItem(1);
        static_cast <BoolRandomArrayCoder *> (rsCorrBwCompensationCoders[idx])->addMapItem(2);
        static_cast <BoolRandomArrayCoder *> (rsCorrBwCompensationCoders[idx])->addMapItem(3);
        static_cast <BoolRandomArrayCoder *> (rsCorrBwCompensationCoders[idx])->addMapItem(4);
        static_cast <BoolRandomArrayCoder *> (rsCorrBwCompensationCoders[idx])->addMapItem(5);
        static_cast <BoolRandomArrayCoder *> (rsCorrBwCompensationCoders[idx])->addMapItem(6);
        static_cast <BoolRandomArrayCoder *> (rsCorrBwCompensationCoders[idx])->addMapItem(7);
        coders.push_back(rsCorrBwCompensationCoders[idx]);
        boolConfig.initialBit += 4;
        if (boolConfig.initialBit >12) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs PREDICTION compensation ENABLE*/
    boolConfig.initialWord = 4480;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    rsPredEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsPredEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(rsPredEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs prediction compensation GAIN*/
    doubleConfig.initialWord = 4504;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;
    doubleConfig.resolution = rsPredGainRange.step;
    doubleConfig.minValue = rsPredGainRange.min;
    doubleConfig.maxValue = rsPredGainRange.max;
    rsPredGainCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsPredGainCompensationCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(rsPredGainCompensationCoders[idx]);
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8) {
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Rs prediction compensation TAU*/
    doubleConfig.initialWord = 4696;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;
    doubleConfig.resolution = rsPredTauRange.step;
    doubleConfig.minValue = rsPredTauRange.min;
    doubleConfig.maxValue = rsPredTauRange.max;
    rsPredTauCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsPredTauCompensationCoders[idx] = new DoubleOffsetBinaryCoder(doubleConfig);
        coders.push_back(rsPredTauCompensationCoders[idx]);
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8) {
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! CURRENT CLAMP Cfast / pipette capacitance compensation ENABLE */
    boolConfig.initialWord = 3424;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    pipetteCapCcEnCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        pipetteCapCcEnCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(pipetteCapCcEnCompensationCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! CURRENT CLAMP Cfast / pipette capacitance compensation VALUE*/
    pipetteCapCcValCompensationMultiCoders.resize(currentChannelsNum);

    boolConfig.initialWord = 3640;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 3448;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    multiConfig.doubleCoderVector.resize(pipetteCapacitanceRanges);
    multiConfig.thresholdVector.resize(pipetteCapacitanceRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        multiConfig.boolCoder = new BoolRandomArrayCoder(boolConfig);
        static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x0);
        static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x1);
        static_cast <BoolRandomArrayCoder *> (multiConfig.boolCoder)->addMapItem(0x3);
        coders.push_back(multiConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < pipetteCapacitanceRanges; rangeIdx++) {
            doubleConfig.minValue = ccPipetteCapacitanceRange[rangeIdx].min;
            doubleConfig.maxValue = ccPipetteCapacitanceRange[rangeIdx].max;
            doubleConfig.resolution = ccPipetteCapacitanceRange[rangeIdx].step;

            multiConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < pipetteCapacitanceRanges-1) {
                multiConfig.thresholdVector[rangeIdx] = ccPipetteCapacitanceRange[rangeIdx].max + ccPipetteCapacitanceRange[rangeIdx].step;
            }
        }
        pipetteCapCcValCompensationMultiCoders[idx] = new MultiCoder(multiConfig);
        coders.push_back(pipetteCapCcValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 2;
        if (boolConfig.initialBit > 14) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cfast value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8) {
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    // Initialization Compensable in USER domain
    std::vector <double> defaultAsicDomainParams;
    for (int i = 0; i < currentChannelsNum; i++) {
        defaultAsicDomainParams = user2AsicDomainTransform(i, defaultUserDomainParams);
        asic2UserDomainCompensable(i, defaultAsicDomainParams, defaultUserDomainParams);
    }

    /*! Default status */
    txStatus.init(txDataWords);
    txStatus.encodingWords[2] = 0x0070; // fans on
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

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::initializeHW() {
    std::this_thread::sleep_for(std::chrono::seconds(motherboardBootTime_s));

    this->resetFpga(true, true);
    this->resetFpga(false, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));

    this->resetAsic(true, true);
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    this->resetAsic(false, true); /*! Not synchronous across MB's FGPAs */
    std::this_thread::sleep_for (std::chrono::milliseconds(10));

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::getCompOptionsFeatures(CompensationTypes_t type, std::vector <std::string> &compOptionsArray) {
    switch (type) {
    case CompRsCorr:
        if (rsCorrBwArray.size()==0) {
            return ErrorFeatureNotImplemented;
        } else {
            compOptionsArray.resize(CompensationRsCorrBwNum);
            for (uint32_t i = 0; i < CompensationRsCorrBwNum; i++) {
                compOptionsArray[i] = compensationOptionStrings[CompRsCorr][i];
            }
            return Success;
        }

        break;

    default:
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::getCompensationEnables(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <bool> &onValues) {
    onValues.resize(channelIndexes.size());
    switch (type) {
    case CompCfast:
        if (pipetteCapEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[type][channelIndexes[i]];
        }
        break;

    case CompCslow:
        if (membraneCapEnCompensationCoders.size() == 0 ) {
            return ErrorFeatureNotImplemented;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[type][channelIndexes[i]];
        }
        break;

    case CompRsCorr:
        if (rsCorrEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[type][channelIndexes[i]];
        }
        break;

    case CompRsPred:
        if (rsPredEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[type][channelIndexes[i]];
        }
        break;

    case CompCcCfast:
        if (pipetteCapCcEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            onValues[i] = ccCompensationsActivated && compensationsEnableFlags[type][channelIndexes[i]];
        }
        break;

    default:
        return ErrorFeatureNotImplemented;
    }

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::enableCompensation(std::vector <uint16_t> channelIndexes, CompensationTypes_t compTypeToEnable, std::vector <bool> onValues, bool applyFlag) {
    switch (compTypeToEnable) {
    case CompCfast:
        if (pipetteCapEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;

        } else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            pipetteCapEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
            channelModels[channelIndexes[i]]->setCompensatingCfast(onValues[i]);
        }
        break;

    case CompCslow:
        if (membraneCapEnCompensationCoders.size() == 0 ) {
            return ErrorFeatureNotImplemented;

        } else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            membraneCapEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
            channelModels[channelIndexes[i]]->setCompensatingCslowRs(onValues[i]);
        }
        break;

    case CompRsCorr:
        if (rsCorrEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;

        } else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            rsCorrEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
            channelModels[channelIndexes[i]]->setCompensatingRsCp(onValues[i]);
            this->updateLiquidJunctionVoltage(channelIndexes[i], false);

        }
        break;

    case CompRsPred:
        if (rsPredEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;

        }else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }

        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            rsPredEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
            channelModels[channelIndexes[i]]->setCompensatingRsPg(onValues[i]);
        }
        break;

    case CompCcCfast:
        if (pipetteCapCcEnCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;
        } else if (!ccCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
        for (int i = 0; i < channelIndexes.size(); i++) {
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            pipetteCapCcEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus);
            channelModels[channelIndexes[i]]->setCompensatingCcCfast(onValues[i]);
        }
        break;

    default:
        return ErrorFeatureNotImplemented;
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::enableVcCompensations(bool enable, bool applyFlag) {
    vcCompensationsActivated = enable;

    for (int i = 0; i < currentChannelsNum; i++) {
        pipetteCapEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompCfast][i], txStatus);
        membraneCapEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompCslow][i], txStatus);
        rsCorrEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompRsCorr][i], txStatus);
        rsPredEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompRsPred][i], txStatus);
        this->updateLiquidJunctionVoltage(i, false);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::enableCcCompensations(bool enable, bool applyFlag) {
    ccCompensationsActivated = enable;

    for (int i = 0; i < currentChannelsNum; i++) {
        pipetteCapCcEnCompensationCoders[i]->encode(ccCompensationsActivated && compensationsEnableFlags[CompCcCfast][i], txStatus);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::setCompValues(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <double> newParamValues, bool applyFlag) {
    std::string debugString = "";
    // make local copy of the user domain param vectors
    std::vector <std::vector <double> > localCompValueSubMatrix;
    localCompValueSubMatrix.resize(channelIndexes.size());
    for (int chIdx = 0; chIdx < channelIndexes.size(); chIdx++) {
        localCompValueSubMatrix[chIdx] = this->compValueMatrix[channelIndexes[chIdx]];
    }

    // for each user param vector
    for (int chIdx = 0; chIdx < localCompValueSubMatrix.size(); chIdx++) {
        // update value in user domain
        localCompValueSubMatrix[chIdx][paramToUpdate] = newParamValues[chIdx];
        // convert user domain to asic domain
        std::vector <double> asicParams = this->user2AsicDomainTransform(channelIndexes[chIdx], localCompValueSubMatrix[chIdx]);
        double temp;

        // select asicParam to encode based on enum
        /*! \todo FCON recheck: IN CASE THERE'S INTERACTION AMONG ASICPARAMS, THEY COULD BE DESCRIBED IN THE SWITCH-CASE */
        switch (paramToUpdate) {
        case U_CpVc:
            //encode
            temp = pipetteCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Cp], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_Cp] = temp;
            break;

        case U_Cm:
            //encode
            temp = membraneCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Cm], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_Cm] = temp;

            //encode
            temp = membraneCapTauValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Taum], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_Taum] = temp;
            break;

        case U_Rs:
            //encode
            temp = membraneCapTauValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Taum], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_Taum] = temp;

            //encode
            temp = rsCorrValCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsCr], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_RsCr] = temp;
            break;

        case U_RsCp:
            //encode
            temp = rsCorrValCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsCr], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_RsCr] = temp;
            break;

        case U_RsPg:
            //encode
            temp = rsPredGainCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsPg], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_RsPg] = temp;

            //encode
            temp = rsPredTauCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsPtau], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_RsPtau] = temp;
            break;

        case U_CpCc:
            //encode
            temp = pipetteCapCcValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Cp], txStatus);
            // update asic domain vector with coder return value
            asicParams[A_RsPtau] = temp;
            break;
        }

        // convert to user domain
        double oldUCpVc = localCompValueSubMatrix[chIdx][U_CpVc];
        double oldUCpCc = localCompValueSubMatrix[chIdx][U_CpCc];
        localCompValueSubMatrix[chIdx] = asic2UserDomainTransform(channelIndexes[chIdx], asicParams, oldUCpVc, oldUCpCc);

        /*! \todo call here function to compute the compensable value ranges in the user domain*/
        asic2UserDomainCompensable(channelIndexes[chIdx], asicParams, localCompValueSubMatrix[chIdx]);

        //copy back to compValuematrix
        this->compValueMatrix[channelIndexes[chIdx]] = localCompValueSubMatrix[chIdx];

    //end for
    }
    // stack outgoing message
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::setCompOptions(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <uint16_t> options, bool applyFlag) {
    switch (type) {
    case CompRsCorr:
        if (rsCorrBwCompensationCoders.empty()) {
            return ErrorFeatureNotImplemented;
        }
        else {
            for (uint32_t i = 0; i < channelIndexes.size(); i++) {
                selectedRsCorrBws[channelIndexes[i]] = options[i];
                rsCorrBwCompensationCoders[channelIndexes[i]]->encode(options[i], txStatus);
            }

            if (applyFlag) {
                this->stackOutgoingMessage(txStatus);
            }
            return Success;
        }
        break;
    }
    return ErrorFeatureNotImplemented;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::turnVoltageReaderOn(bool onValueIn, bool applyFlag) {
    std::vector <bool> trues(currentChannelsNum);
    std::vector <bool> falses(currentChannelsNum);
    std::vector <ClampingModality_t> clamps(currentChannelsNum);

    std::fill(trues.begin(), trues.end(), true);
    std::fill(falses.begin(), falses.end(), false);
    std::fill(clamps.begin(), clamps.end(), ClampingModality_t::CURRENT_CLAMP);

    if (onValueIn) {
        this->turnCcSwOn(allChannelIndexes, trues, false);
        this->setAdcCore(allChannelIndexes, clamps, false);
        this->setGrEn(false, false);
        this->updateCalibCcVoltageGain(allChannelIndexes, false);
        this->updateCalibCcVoltageOffset(allChannelIndexes, applyFlag);
        this->setAdcFilter();

    } else {
        this->turnCcSwOn(allChannelIndexes, falses, applyFlag);
    }

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::turnCurrentReaderOn(bool onValueIn, bool applyFlag) {
    std::vector <bool> trues(currentChannelsNum);
    std::vector <bool> falses(currentChannelsNum);
    std::vector <ClampingModality_t> clamps(currentChannelsNum);

    std::fill(trues.begin(), trues.end(), true);
    std::fill(falses.begin(), falses.end(), false);
    std::fill(clamps.begin(), clamps.end(), ClampingModality_t::VOLTAGE_CLAMP);

    if (onValueIn) {
        this->turnVcSwOn(allChannelIndexes, trues, false);
        this->setAdcCore(allChannelIndexes, clamps, false);
        this->setGrEn(true, false);
        this->updateCalibVcCurrentGain(allChannelIndexes, false);
        this->updateCalibVcCurrentOffset(allChannelIndexes, applyFlag);
        this->setAdcFilter();

    } else {
        this->turnVcSwOn(allChannelIndexes, falses, applyFlag);
    }

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::turnVoltageStimulusOn(bool onValue, bool applyFlag) {
    if (onValue) {
        this->updateCalibVcVoltageGain(allChannelIndexes, false);
        this->updateCalibVcVoltageOffset(allChannelIndexes, applyFlag);

    } else {

    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::turnCurrentStimulusOn(bool onValue, bool applyFlag) {
    std::vector <bool> trues(currentChannelsNum);
    std::vector <bool> falses(currentChannelsNum);

    std::fill(trues.begin(), trues.end(), true);
    std::fill(falses.begin(), falses.end(), false);

    if (onValue) {
        this->enableCcStimulus(allChannelIndexes, trues, false);
        this->updateCalibCcCurrentGain(allChannelIndexes, false);
        this->updateCalibCcCurrentOffset(allChannelIndexes, applyFlag);

    } else {
        this->enableCcStimulus(allChannelIndexes, falses, applyFlag);
    }

    return Success;
}

std::vector <double> Emcr384PatchClamp_EL07c_prot_v06_fw_v01::user2AsicDomainTransform(int chIdx, std::vector <double> userDomainParams) {
    std::vector <double> asicDomainParameter;
    asicDomainParameter.resize(CompensationAsicParamsNum);
    double cp; // pipette capacitance
    double cm; // membrane capacitance
    double taum; // membrane capacitance tau
    double rsCr; // Rseries Correction
    double rsPg; //Rseries prediction gain
    double rsPtau; // Resies prediction tau

    // membrane capacitance domain conversion
    cm = userDomainParams[U_Cm];

    // pipette capacitance VC to pipette capacitance domain conversion
    /*! \todo aggiungere check se il multicoder esiste sulla size del vettore di puntatori  a multiCoder*/

    if (selectedClampingModality == VOLTAGE_CLAMP) {
        cp = userDomainParams[U_CpVc];
    } else {
        // A_Cp
        cp = userDomainParams[U_CpCc];
    }

    // Series resistance to Membrane tau domain conversion
    taum = userDomainParams[U_Cm] * userDomainParams[U_Rs];

    // Series correction percentage to Series correction resistance domain conversion
    rsCr = userDomainParams[U_Rs] * userDomainParams[U_RsCp] * 0.01; // RsCp is a percentage

    // Series prediction gain domain conversion
    rsPg = userDomainParams[U_RsPg];

    // pipette capacitance CC to Series prediction tau domain conversion
    rsPtau = taum / (userDomainParams[U_RsPg] + 1);

    asicDomainParameter[A_Cp] = cp;
    asicDomainParameter[A_Cm] = cm;
    asicDomainParameter[A_Taum] = taum;
    asicDomainParameter[A_RsCr] = rsCr;
    asicDomainParameter[A_RsPg] = rsPg;
    asicDomainParameter[A_RsPtau] = rsPtau;

    return asicDomainParameter;
}

std::vector <double> Emcr384PatchClamp_EL07c_prot_v06_fw_v01::asic2UserDomainTransform(int chIdx, std::vector <double> asicDomainParams, double oldUCpVc, double oldUCpCc) {
    std::vector <double> userDomainParameter;
    userDomainParameter.resize(CompensationUserParamsNum);

    double cpVc;
    double cm;
    double rs;
    double rsCp;
    double rsPg;
    double cpCC;

    //  pipette capacitance to pipette capacitance VC domain conversion
    if (selectedClampingModality == VOLTAGE_CLAMP) {
        cpVc = asicDomainParams[A_Cp];
    } else {
        cpVc = oldUCpVc;
    }

    // membrane capacitance domain conversion
    cm = asicDomainParams[A_Cm];

    // membrane tau to Series resistance domain conversion
    rs = asicDomainParams[A_Taum] / asicDomainParams[A_Cm];

    // Series correction resistance to Series correction percentage domain conversion
    rsCp = asicDomainParams[A_RsCr] / rs * 100.0;

    // Series prediction gain domain conversion
    rsPg = -1 + asicDomainParams[A_Taum] / asicDomainParams[A_RsPtau];

    // Series prediction tau to Pipette capacitance CC domain conversion
    if (selectedClampingModality == VOLTAGE_CLAMP) {
        cpCC = oldUCpCc; /*! \todo recheck */
    } else {
        cpCC = asicDomainParams[A_Cp];
    }

    userDomainParameter[U_CpVc] = cpVc;
    userDomainParameter[U_Cm] = cm;
    userDomainParameter[U_Rs] = rs;
    userDomainParameter[U_RsCp] = rsCp;
    userDomainParameter[U_RsPg] = rsPg;
    userDomainParameter[U_CpCc] = cpCC;
    return userDomainParameter;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::asic2UserDomainCompensable(int chIdx, std::vector <double> asicDomainParams, std::vector <double> userDomainParams) {
    /*! \todo still to understand how to obtain them. COuld they be imputs of the function?*/
    std::vector <double> potentialMaxs;
    std::vector <double> potentialMins;

    double myInfinity = std::numeric_limits<double>::infinity();

    /*! Compensable for U_CpVc*/
    compensationControls[U_CpVc][chIdx].maxCompensable = pipetteCapacitanceRange.back().max;

    potentialMins.push_back(pipetteCapacitanceRange.front().min);
    potentialMins.push_back(0.0);
    compensationControls[U_CpVc][chIdx].minCompensable = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    compensationControls[U_CpVc][chIdx].step = pipetteCapacitanceRange.front().step;

    /*! Compensable for U_Cm*/
    // max
    /*! MPAC: added +1 after discussion with MBEN and FCON*/
    potentialMaxs.push_back(membraneCapValueRange.back().max);

    potentialMaxs.push_back(membraneCapTauValueRange.back().max/userDomainParams[U_Rs]);

    if (vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]) {
        potentialMaxs.push_back(rsPredTauRange.max*(userDomainParams[U_RsPg]+1)/userDomainParams[U_Rs]);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    compensationControls[U_Cm][chIdx].maxCompensable = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    potentialMins.push_back(membraneCapValueRange.front().min);

    potentialMins.push_back(membraneCapTauValueRange.front().min/userDomainParams[U_Rs]);

    if (vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]) {
        potentialMins.push_back(rsPredTauRange.min*(userDomainParams[U_RsPg]+1)/userDomainParams[U_Rs]);
    } else {
        potentialMins.push_back(0.0);
    }

    compensationControls[U_Cm][chIdx].minCompensable = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    //step
    compensationControls[U_Cm][chIdx].step = membraneCapValueRange.front().step;

    /*! Compensable for U_Rs*/
    //max
    if (vcCompensationsActivated && compensationsEnableFlags[CompCslow][chIdx]) {
        potentialMaxs.push_back(membraneCapTauValueRange.back().max/userDomainParams[U_Cm]);
    } else {
        potentialMaxs.push_back(membraneCapTauValueRange.back().max/compensationControls[U_Cm][chIdx].minCompensable);
    }

    if (vcCompensationsActivated && compensationsEnableFlags[CompRsCorr][chIdx]) {
        potentialMaxs.push_back(rsCorrValueRange.max / userDomainParams[U_RsCp] * 100.0);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    if (vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]) {
        potentialMaxs.push_back(rsPredTauRange.max * (userDomainParams[U_RsPg]+1) / userDomainParams[U_Cm]);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    compensationControls[U_Rs][chIdx].maxCompensable = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    if (vcCompensationsActivated && compensationsEnableFlags[CompCslow][chIdx]) {
        potentialMins.push_back(membraneCapTauValueRange.front().min / userDomainParams[U_Cm]);
    } else {
        potentialMins.push_back(membraneCapTauValueRange.front().min / compensationControls[U_Cm][chIdx].maxCompensable);
    }

    if (vcCompensationsActivated && compensationsEnableFlags[CompRsCorr][chIdx]) {
        potentialMins.push_back(rsCorrValueRange.min / userDomainParams[U_RsCp] * 100.0);
    } else {
        potentialMins.push_back(0);
    }

    if (vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]) {
        potentialMins.push_back(rsPredTauRange.min * (userDomainParams[U_RsPg]+1) / userDomainParams[U_Cm]);
    } else {
        potentialMins.push_back(0);
    }

    compensationControls[U_Rs][chIdx].minCompensable = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    //step
    compensationControls[U_Rs][chIdx].step = 0.1; //MOhm

    /*! Compensable for U_RsCp*/
    // max
    /*! \todo Pay attention to possible divisions by 0*/
    potentialMaxs.push_back(rsCorrValueRange.max / userDomainParams[U_Rs] * 100.0);
    potentialMaxs.push_back(100.0); //%
    compensationControls[U_RsCp][chIdx].maxCompensable = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    /*! \todo Pay attention to possible divisions by 0*/
    potentialMins.push_back(0.0); //%
//    potentialMins.push_back(rsCorrValueRange.min / userDomainParams[U_Rs] * 100.0);
    compensationControls[U_RsCp][chIdx].minCompensable = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    compensationControls[U_RsCp][chIdx].step = 1.0; //%

    /*! Compensable for U_RsPg*/
    //max
    potentialMaxs.push_back(rsPredGainRange.max);
    potentialMaxs.push_back(-1 + userDomainParams[U_Cm] * userDomainParams[U_Rs] / rsPredTauRange.min);
    compensationControls[U_RsPg][chIdx].maxCompensable = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    potentialMins.push_back(rsPredGainRange.min);
    potentialMins.push_back(-1 + userDomainParams[U_Cm] * userDomainParams[U_Rs] / rsPredTauRange.max);
    compensationControls[U_RsPg][chIdx].minCompensable = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    compensationControls[U_RsPg][chIdx].step = rsPredGainRange.step;

    /*! Compensable for U_CpCc*/
    compensationControls[U_CpCc][chIdx].maxCompensable = ccPipetteCapacitanceRange.back().max;
    compensationControls[U_CpCc][chIdx].minCompensable = ccPipetteCapacitanceRange.front().min;
    compensationControls[U_CpCc][chIdx].step = ccPipetteCapacitanceRange.front().step;

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_EL07c_prot_v06_fw_v01::getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) {
    switch (param) {
    case U_CpVc:
        control.implemented = true;
        control.min = pipetteCapacitanceRange.front().min;
        control.max = pipetteCapacitanceRange.back().max;
        control.minCompensable = pipetteCapacitanceRange.front().min;
        control.maxCompensable = pipetteCapacitanceRange.back().max;
        control.step = pipetteCapacitanceRange.front().step;
        control.steps = round(1.0+(control.max-control.min)/control.step);
        control.decimals = pipetteCapacitanceRange.front().decimals();
        control.value = pipetteCapacitanceRange.front().min;
        control.prefix = pipetteCapacitanceRange.front().prefix;
        control.unit = pipetteCapacitanceRange.front().unit;
        control.name = "Pipette Capacitance (VC)";
        return Success;

    case U_CpCc:
        control.implemented = true;
        control.min = ccPipetteCapacitanceRange.front().min;
        control.max = ccPipetteCapacitanceRange.back().max;
        control.minCompensable = ccPipetteCapacitanceRange.front().min;
        control.maxCompensable = ccPipetteCapacitanceRange.back().max;
        control.step = ccPipetteCapacitanceRange.front().step;
        control.steps = round(1.0+(control.max-control.min)/control.step);
        control.decimals = ccPipetteCapacitanceRange.front().decimals();
        control.value = ccPipetteCapacitanceRange.front().min;
        control.prefix = ccPipetteCapacitanceRange.front().prefix;
        control.unit = ccPipetteCapacitanceRange.front().unit;
        control.name = "Pipette Capacitance (CC)";
        return Success;

    case U_Cm:
        control.implemented = true;
        control.min = membraneCapValueRange.front().min;
        control.max = membraneCapValueRange.back().max;
        control.minCompensable = membraneCapValueRange.front().min;
        control.maxCompensable = membraneCapValueRange.back().max;
        control.step = membraneCapValueRange.front().step;
        control.steps = round(1.0+(control.max-control.min)/control.step);
        control.decimals = membraneCapValueRange.front().decimals();
        control.value = membraneCapValueRange.front().min;
        control.prefix = membraneCapValueRange.front().prefix;
        control.unit = membraneCapValueRange.front().unit;
        control.name = "Membrane Capacitance";
        return Success;

    case U_Rs:
        control.implemented = true;
        control.min = rsCorrValueRange.min;
        control.max = rsCorrValueRange.max;
        control.minCompensable = rsCorrValueRange.min;
        control.maxCompensable = rsCorrValueRange.max;
        control.step = rsCorrValueRange.step;
        control.steps = rsCorrValueRange.steps();
        control.decimals = rsCorrValueRange.decimals();
        control.value = rsCorrValueRange.min;
        control.prefix = rsCorrValueRange.prefix;
        control.unit = rsCorrValueRange.unit;
        control.name = "Access Resistance";
        return Success;

    case U_RsCp:
        control.implemented = true;
        control.min = 1.0;
        control.max = 100.0;
        control.minCompensable = 1.0;
        control.maxCompensable = 100.0;
        control.step = 1.0;
        control.steps = round(1.0+(control.max-control.min)/control.step);
        control.decimals = 1;
        control.value = 1.0;
        control.prefix = UnitPfxNone;
        control.unit = "%";
        control.name = "Correction Percentage";
        return Success;

    case U_RsPg:
        control.implemented = true;
        control.min = rsPredGainRange.min;
        control.max = rsPredGainRange.max;
        control.minCompensable = rsPredGainRange.min;
        control.maxCompensable = rsPredGainRange.max;
        control.step = rsPredGainRange.step;
        control.steps = rsPredGainRange.steps();
        control.decimals = rsPredGainRange.decimals();
        control.value = rsPredGainRange.min;
        control.prefix = rsPredGainRange.prefix;
        control.unit = rsPredGainRange.unit;
        control.name = "Prediction Gain";
        return Success;

    default:
        return ErrorFeatureNotImplemented;
    }
}

void Emcr384PatchClamp_EL07c_prot_v06_fw_v01::setGrEn(bool flag, bool applyFlag) {
    for (auto coder : grEnCoders) {
        coder->encode(flag, txStatus);
    }
    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
}

Emcr384PatchClamp_EL07d_prot_v06_fw_v01::Emcr384PatchClamp_EL07d_prot_v06_fw_v01(std::string di) :
    Emcr384PatchClamp_EL07c_prot_v06_fw_v01(di) {

    fwSize_B = 5506748;
    motherboardBootTime_s = fwSize_B/OKY_MOTHERBOARD_FPGA_BYTES_PER_S+5;
}
