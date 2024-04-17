#include "emcr384patchclamp_prot_v04_fw_v04.h"
#include "utils.h"

Emcr384PatchClamp_prot_v04_fw_v04::Emcr384PatchClamp_prot_v04_fw_v04(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "384PatchClamp";

    fwName = "384PatchClamp_V04.bit";

    fwSize_B = 6313296;
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
    rxWordLengths[RxMessageDataHeader] = 4;

    rxWordOffsets[RxMessageDataTail] = rxWordOffsets[RxMessageDataHeader] + rxWordLengths[RxMessageDataHeader];
    rxWordLengths[RxMessageDataTail] = 1;

    rxWordOffsets[RxMessageStatus] = rxWordOffsets[RxMessageDataTail] + rxWordLengths[RxMessageDataTail];
    rxWordLengths[RxMessageStatus] = 1;

    rxMaxWords = totalChannelsNum; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE*packetsPerFrame;

    txDataWords = 4888; /** \todo recheck MPAC, aggiornato il 20231004*///4504; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txModifiedStartingWord = txDataWords;
    txModifiedEndingWord = 0;
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    /*! Clamping modalities */
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = ClampingModality_t::VOLTAGE_CLAMP;
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
    protocolWordOffset = 204;
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
    defaultVcCurrentRangeIdx = VCCurrentRange10nA;

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
    defaultCcCurrentRangeIdx = CCCurrentRange8nA;

    /*! Voltage ranges */
    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;
    ccVoltageRangesArray.resize(ccVoltageRangesNum);
    ccVoltageRangesArray[CCVoltageRange1000mV].max = 1000.0;
    ccVoltageRangesArray[CCVoltageRange1000mV].min = -1000.0;
    ccVoltageRangesArray[CCVoltageRange1000mV].step = ccVoltageRangesArray[CCVoltageRange1000mV].max/(SHORT_MAX+1.0);
    ccVoltageRangesArray[CCVoltageRange1000mV].prefix = UnitPfxMilli;
    ccVoltageRangesArray[CCVoltageRange1000mV].unit = "V";
    defaultCcVoltageRangeIdx = CCVoltageRange1000mV;

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;
    vcCurrentFiltersArray.resize(vcCurrentFiltersNum);
    vcCurrentFiltersArray[VCCurrentFilter3kHz].value = 3.0;
    vcCurrentFiltersArray[VCCurrentFilter3kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter3kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter3_6kHz].value = 3.6;
    vcCurrentFiltersArray[VCCurrentFilter3_6kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter3_6kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter10kHz].value = 10.0;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter10kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter12kHz].value = 12.0;
    vcCurrentFiltersArray[VCCurrentFilter12kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter12kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter25kHz].value = 25.0;
    vcCurrentFiltersArray[VCCurrentFilter25kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter25kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter30kHz].value = 30.0;
    vcCurrentFiltersArray[VCCurrentFilter30kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter30kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter100kHz].value = 100.0;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter100kHz].unit = "Hz";
    vcCurrentFiltersArray[VCCurrentFilter120kHz].value = 120.0;
    vcCurrentFiltersArray[VCCurrentFilter120kHz].prefix = UnitPfxKilo;
    vcCurrentFiltersArray[VCCurrentFilter120kHz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter3kHz;

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
    vcVoltageFiltersArray[VCVoltageFilter35kHz].value = 35.0;
    vcVoltageFiltersArray[VCVoltageFilter35kHz].prefix = UnitPfxKilo;
    vcVoltageFiltersArray[VCVoltageFilter35kHz].unit = "Hz";
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
    ccCurrentFiltersArray[CCCurrentFilter35kHz].value = 35.0;
    ccCurrentFiltersArray[CCCurrentFilter35kHz].prefix = UnitPfxKilo;
    ccCurrentFiltersArray[CCCurrentFilter35kHz].unit = "Hz";
    defaultCcCurrentFilterIdx = CCCurrentFilter1kHz;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;
    ccVoltageFiltersArray.resize(ccVoltageFiltersNum);
    ccVoltageFiltersArray[CCVoltageFilter3kHz].value = 3.0;
    ccVoltageFiltersArray[CCVoltageFilter3kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter3kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter3_6kHz].value = 3.6;
    ccVoltageFiltersArray[CCVoltageFilter3_6kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter3_6kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter10kHz].value = 10.0;
    ccVoltageFiltersArray[CCVoltageFilter10kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter10kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter12kHz].value = 12.0;
    ccVoltageFiltersArray[CCVoltageFilter12kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter12kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter25kHz].value = 25.0;
    ccVoltageFiltersArray[CCVoltageFilter25kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter25kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter30kHz].value = 30.0;
    ccVoltageFiltersArray[CCVoltageFilter30kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter30kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter100kHz].value = 100.0;
    ccVoltageFiltersArray[CCVoltageFilter100kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter100kHz].unit = "Hz";
    ccVoltageFiltersArray[CCVoltageFilter120kHz].value = 120.0;
    ccVoltageFiltersArray[CCVoltageFilter120kHz].prefix = UnitPfxKilo;
    ccVoltageFiltersArray[CCVoltageFilter120kHz].unit = "Hz";
    defaultCcVoltageFilterIdx = CCVoltageFilter3kHz;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
#ifdef E384PATCH_ADDITIONAL_SR_FLAG
    samplingRatesArray[SamplingRate100Hz].value = 100.0;
    samplingRatesArray[SamplingRate100Hz].prefix = UnitPfxNone;
    samplingRatesArray[SamplingRate100Hz].unit = "Hz";
    samplingRatesArray[SamplingRate200Hz].value = 200.0;
    samplingRatesArray[SamplingRate200Hz].prefix = UnitPfxNone;
    samplingRatesArray[SamplingRate200Hz].unit = "Hz";
    samplingRatesArray[SamplingRate400Hz].value = 400.0;
    samplingRatesArray[SamplingRate400Hz].prefix = UnitPfxNone;
    samplingRatesArray[SamplingRate400Hz].unit = "Hz";
    samplingRatesArray[SamplingRate800Hz].value = 800.0;
    samplingRatesArray[SamplingRate800Hz].prefix = UnitPfxNone;
    samplingRatesArray[SamplingRate800Hz].unit = "Hz";
    samplingRatesArray[SamplingRate1_6kHz].value = 1600.0;
    samplingRatesArray[SamplingRate1_6kHz].prefix = UnitPfxNone;
    samplingRatesArray[SamplingRate1_6kHz].unit = "Hz";
#endif
    samplingRatesArray[SamplingRate5kHz].value = 5.0;
    samplingRatesArray[SamplingRate5kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate5kHz].unit = "Hz";
    samplingRatesArray[SamplingRate10kHz].value = 10.0;
    samplingRatesArray[SamplingRate10kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate10kHz].unit = "Hz";
    samplingRatesArray[SamplingRate20kHz].value = 20.0;
    samplingRatesArray[SamplingRate20kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate20kHz].unit = "Hz";
    samplingRatesArray[SamplingRate40kHz].value = 40.0;
    samplingRatesArray[SamplingRate40kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate40kHz].unit = "Hz";
    samplingRatesArray[SamplingRate80kHz].value = 80.0;
    samplingRatesArray[SamplingRate80kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate80kHz].unit = "Hz";
#ifdef E384PATCH_ADDITIONAL_SR_FLAG
    defaultSamplingRateIdx = SamplingRate100Hz;
#else
    defaultSamplingRateIdx = SamplingRate5kHz;
#endif

    realSamplingRatesArray.resize(samplingRatesNum);
#ifdef E384PATCH_ADDITIONAL_SR_FLAG
    realSamplingRatesArray[SamplingRate100Hz].value = 100.0;
    realSamplingRatesArray[SamplingRate100Hz].prefix = UnitPfxNone;
    realSamplingRatesArray[SamplingRate100Hz].unit = "Hz";
    realSamplingRatesArray[SamplingRate200Hz].value = 200.0;
    realSamplingRatesArray[SamplingRate200Hz].prefix = UnitPfxNone;
    realSamplingRatesArray[SamplingRate200Hz].unit = "Hz";
    realSamplingRatesArray[SamplingRate400Hz].value = 400.0;
    realSamplingRatesArray[SamplingRate400Hz].prefix = UnitPfxNone;
    realSamplingRatesArray[SamplingRate400Hz].unit = "Hz";
    realSamplingRatesArray[SamplingRate800Hz].value = 800.0;
    realSamplingRatesArray[SamplingRate800Hz].prefix = UnitPfxNone;
    realSamplingRatesArray[SamplingRate800Hz].unit = "Hz";
    realSamplingRatesArray[SamplingRate1_6kHz].value = 1600.0;
    realSamplingRatesArray[SamplingRate1_6kHz].prefix = UnitPfxNone;
    realSamplingRatesArray[SamplingRate1_6kHz].unit = "Hz";
#endif
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


    integrationStepArray.resize(samplingRatesNum);
#ifdef E384PATCH_ADDITIONAL_SR_FLAG
    integrationStepArray[SamplingRate100Hz].value = 0.01;
    integrationStepArray[SamplingRate100Hz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate100Hz].unit = "s";
    integrationStepArray[SamplingRate200Hz].value = 0.005;
    integrationStepArray[SamplingRate200Hz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate200Hz].unit = "s";
    integrationStepArray[SamplingRate400Hz].value = 0.0025;
    integrationStepArray[SamplingRate400Hz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate400Hz].unit = "s";
    integrationStepArray[SamplingRate800Hz].value = 0.00125;
    integrationStepArray[SamplingRate800Hz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate800Hz].unit = "s";
    integrationStepArray[SamplingRate1_6kHz].value = 0.000625;
    integrationStepArray[SamplingRate1_6kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate1_6kHz].unit = "s";
#endif
    integrationStepArray[SamplingRate5kHz].value = 0.0002;
    integrationStepArray[SamplingRate5kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate5kHz].unit = "s";
    integrationStepArray[SamplingRate10kHz].value = 0.0001;
    integrationStepArray[SamplingRate10kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate10kHz].unit = "s";
    integrationStepArray[SamplingRate20kHz].value = 0.00005;
    integrationStepArray[SamplingRate20kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate20kHz].unit = "s";
    integrationStepArray[SamplingRate40kHz].value = 0.000025;
    integrationStepArray[SamplingRate40kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate40kHz].unit = "s";
    integrationStepArray[SamplingRate80kHz].value = 0.0000125;
    integrationStepArray[SamplingRate80kHz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate80kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
    #ifdef E384PATCH_ADDITIONAL_SR_FLAG
        {SamplingRate100Hz, VCCurrentFilter3kHz},
        {SamplingRate200Hz, VCCurrentFilter3kHz},
        {SamplingRate400Hz, VCCurrentFilter3kHz},
        {SamplingRate800Hz, VCCurrentFilter3kHz},
        {SamplingRate1_6kHz, VCCurrentFilter3kHz},
    #endif
        {SamplingRate5kHz, VCCurrentFilter3_6kHz},
        {SamplingRate10kHz, VCCurrentFilter10kHz},
        {SamplingRate20kHz, VCCurrentFilter25kHz},
        {SamplingRate40kHz, VCCurrentFilter30kHz},
        {SamplingRate80kHz, VCCurrentFilter100kHz}
    };

    // mapping ADC Current Clamp
    // undefined

    calibrationData.samplingRateIdx = SamplingRate5kHz;

    /*! VC calibration voltage steps*/
    calibrationData.vcCalibStepsArrays.resize(VCCurrentRangesNum);
    calibrationData.vcCalibStepsArrays[VCCurrentRange10nA].resize(5);
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nALbw].resize(5);
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nAHbw].resize(5);
    calibrationData.vcCalibStepsArrays[VCCurrentRange400nA].resize(5);

    calibrationData.vcCalibStepsArrays[VCCurrentRange10nA][0] = {-25.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange10nA][1] = {-12.5, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange10nA][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange10nA][3] = {12.5, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange10nA][4] = {25.0, UnitPfxMilli, "V"};

    calibrationData.vcCalibStepsArrays[VCCurrentRange40nALbw][0] = {-100.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nALbw][1] = {-50.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nALbw][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nALbw][3] = {50.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nALbw][4] = {100.0, UnitPfxMilli, "V"};

    calibrationData.vcCalibStepsArrays[VCCurrentRange40nAHbw][0] = {-100.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nAHbw][1] = {-50.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nAHbw][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nAHbw][3] = {50.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange40nAHbw][4] = {100.0, UnitPfxMilli, "V"};

    calibrationData.vcCalibStepsArrays[VCCurrentRange400nA][0] = {-400.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange400nA][1] = {-200.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange400nA][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange400nA][3] = {200.0, UnitPfxMilli, "V"};
    calibrationData.vcCalibStepsArrays[VCCurrentRange400nA][4] = {400.0, UnitPfxMilli, "V"};

    /*! VC calibration resistances*/
    calibrationData.vcCalibResArray.resize(VCCurrentRangesNum);
    calibrationData.vcCalibResArray[VCCurrentRange10nA] = {5.0, UnitPfxMega, "Ohm"}; // 10nA
    calibrationData.vcCalibResArray[VCCurrentRange40nALbw] = {5.0, UnitPfxMega, "Ohm"}; // 40nA
    calibrationData.vcCalibResArray[VCCurrentRange40nAHbw] = {5.0, UnitPfxMega, "Ohm"}; // 40nA
    calibrationData.vcCalibResArray[VCCurrentRange400nA] = {5.0, UnitPfxMega, "Ohm"}; // 400nA

    // mapping VC current range - calibration resistances
    calibrationData.vcCurrRange2CalibResMap = {
        {VCCurrentRange10nA, CalibRes5_0MOhm},
        {VCCurrentRange40nALbw, CalibRes5_0MOhm},
        {VCCurrentRange40nAHbw, CalibRes5_0MOhm},
        {VCCurrentRange400nA, CalibRes5_0MOhm}
    };

    calibrationData.areCalibResistOnBoard = true;
    calibrationData.canInputsBeOpened = true;
    calibrationData.adcCalibratedInOffsetBinary = true;

    calibrationData.ccCalibVoltStepsArrays.resize(CCVoltageRangesNum);
    calibrationData.ccCalibVoltStepsArrays[CCVoltageRange1000mV].resize(5);
    calibrationData.ccCalibVoltStepsArrays[CCVoltageRange1000mV][0] = {-400.0, UnitPfxMilli, "V"};
    calibrationData.ccCalibVoltStepsArrays[CCVoltageRange1000mV][1] = {-200.0, UnitPfxMilli, "V"};
    calibrationData.ccCalibVoltStepsArrays[CCVoltageRange1000mV][2] = {0.0, UnitPfxMilli, "V"};
    calibrationData.ccCalibVoltStepsArrays[CCVoltageRange1000mV][3] = {200.0, UnitPfxMilli, "V"};
    calibrationData.ccCalibVoltStepsArrays[CCVoltageRange1000mV][4] = {400.0, UnitPfxMilli, "V"};

    calibrationData.ccCalibCurrStepsArrays.resize(CCCurrentRangesNum);
    calibrationData.ccCalibCurrStepsArrays[CCCurrentRange8nA].resize(5);
    calibrationData.ccCalibCurrStepsArrays[CCCurrentRange8nA][0] = {-7.9, UnitPfxNano, "A"};
    calibrationData.ccCalibCurrStepsArrays[CCCurrentRange8nA][1] = {-4.0, UnitPfxNano, "A"};
    calibrationData.ccCalibCurrStepsArrays[CCCurrentRange8nA][2] = {0.0, UnitPfxNano, "A"};
    calibrationData.ccCalibCurrStepsArrays[CCCurrentRange8nA][3] = {4.0, UnitPfxNano, "A"};
    calibrationData.ccCalibCurrStepsArrays[CCCurrentRange8nA][4] = {7.9, UnitPfxNano, "A"};

    /*! CC calibration resistances on each board*/
    calibrationData.ccCalibResArray.resize(CCCurrentRangesNum);
    calibrationData.ccCalibResArray[CCCurrentRange8nA] = {5.0, UnitPfxMega, "Ohm"};

    /*! CC calibration resistances on Model cell*/
    calibrationData.ccCalibResForCcAdcOffsetArray.resize(CCVoltageRangesNum);
    calibrationData.ccCalibResForCcAdcOffsetArray[CCVoltageRange1000mV] = {120.0, UnitPfxKilo, "Ohm"};

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultCurrentHoldTuner = {0.0, ccCurrentRangesArray[CCCurrentRange8nA].prefix, ccCurrentRangesArray[CCCurrentRange8nA].unit};

    defaultVoltageHalfTuner = {0.0, vcVoltageRangesArray[VCVoltageRange500mV].prefix, vcVoltageRangesArray[VCVoltageRange500mV].unit};
    defaultCurrentHalfTuner = {0.0, ccCurrentRangesArray[CCCurrentRange8nA].prefix, ccCurrentRangesArray[CCCurrentRange8nA].unit};

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
    compValueMatrix.resize(currentChannelsNum, std::vector<double>(CompensationAsicParamsNum));
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
    const double pipetteVarResistance = 100.0e-3;
    const double pipetteFixedResistance = 80.0e-3;
    const int pipetteCapacitanceRanges = 4;
    const double pipetteCapacitanceValuesNum = 64.0;

    std::vector <double> pipetteInjCapacitance = {2.5, 5.0, 10.0, 20.0};
    /*! \todo FCON recheck, now trying to use ranged measurement fo Features  */
    pipetteCapacitanceRange.resize(pipetteCapacitanceRanges);
    for (int idx = 0; idx < pipetteCapacitanceRanges; idx++) {
        pipetteCapacitanceRange[idx].step = pipetteVarResistance/pipetteCapacitanceValuesNum/pipetteFixedResistance*pipetteInjCapacitance[idx];
        pipetteCapacitanceRange[idx].min = pipetteVarResistance/pipetteFixedResistance*pipetteInjCapacitance[idx]+pipetteCapacitanceRange[idx].step;
        pipetteCapacitanceRange[idx].max = pipetteCapacitanceRange[idx].min+(pipetteCapacitanceValuesNum-1.0)*pipetteCapacitanceRange[idx].step;
        pipetteCapacitanceRange[idx].prefix = UnitPfxPico;
        pipetteCapacitanceRange[idx].unit = "F";
    }

    /*! FEATURES ASIC DOMAIN Membrane capacitance*/
    const double membraneCapValueResistanceRatio = 2.0;
    const int membraneCapValueRanges = 4;
    const double membraneCapValueValuesNum = 64.0; // 6 bits

    membraneCapValueInjCapacitance = {5.0, 15.0, 45.0, 135.0};
    /*! \todo FCON recheck, now trying to use ranged measurement fo Features  */
    membraneCapValueRange.resize(membraneCapValueRanges);
    for (int idx = 0; idx < membraneCapValueRanges; idx++) {
        membraneCapValueRange[idx].step = membraneCapValueResistanceRatio/membraneCapValueValuesNum * membraneCapValueInjCapacitance[idx];
        membraneCapValueRange[idx].min = (1.0 + membraneCapValueResistanceRatio/membraneCapValueValuesNum) * membraneCapValueInjCapacitance[idx];
        membraneCapValueRange[idx].max = membraneCapValueRange[idx].min + (membraneCapValueValuesNum - 1.0) * membraneCapValueRange[idx].step;
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

    compensationControls[U_CpVc].resize(currentChannelsNum);
    this->getPipetteCapacitanceControl(control);
    std::fill(compensationControls[U_CpVc].begin(), compensationControls[U_CpVc].end(), control);

    compensationControls[U_Cm].resize(currentChannelsNum);
    this->getMembraneCapacitanceControl(control);
    std::fill(compensationControls[U_Cm].begin(), compensationControls[U_Cm].end(), control);

    compensationControls[U_Rs].resize(currentChannelsNum);
    this->getAccessResistanceControl(control);
    std::fill(compensationControls[U_Rs].begin(), compensationControls[U_Rs].end(), control);

    compensationControls[U_RsCp].resize(currentChannelsNum);
    this->getResistanceCorrectionPercentageControl(control);
    std::fill(compensationControls[U_RsCp].begin(), compensationControls[U_RsCp].end(), control);

    compensationControls[U_RsPg].resize(currentChannelsNum);
    this->getResistancePredictionGainControl(control);
    std::fill(compensationControls[U_RsPg].begin(), compensationControls[U_RsPg].end(), control);

    compensationControls[U_CpCc].resize(currentChannelsNum);
    this->getCCPipetteCapacitanceControl(control);
    std::fill(compensationControls[U_CpCc].begin(), compensationControls[U_CpCc].end(), control);

    /*! COMPENSATION OPTIONS STRINGS*/
    compensationOptionStrings.resize(CompensationTypesNum);
    compensationOptionStrings[CompRsCorr].resize(CompensationRsCorrBwNum);

    rsCorrBwArray.resize(CompensationRsCorrBwNum);
    rsCorrBwArray[CompensationRsCorrBw39_789kHz].value = 39.789 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw39_789kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw39_789kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw19_894kHz].value = 19.894 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw19_894kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw19_894kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw9_947kHz].value = 9.947 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw9_947kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw9_947kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw4_974kHz].value = 4.974 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw4_974kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw4_974kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw2_487kHz].value = 2.487 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw2_487kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw2_487kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw1_243kHz].value = 1.243 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw1_243kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw1_243kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw0_622kHz].value = 0.622 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw0_622kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw0_622kHz].unit = "Hz";
    rsCorrBwArray[CompensationRsCorrBw0_311kHz].value = 0.311 / this->clockRatio;
    rsCorrBwArray[CompensationRsCorrBw0_311kHz].prefix = UnitPfxKilo;
    rsCorrBwArray[CompensationRsCorrBw0_311kHz].unit = "Hz";
    defaultRsCorrBwIdx = CompensationRsCorrBw39_789kHz;

    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw39_789kHz] = rsCorrBwArray[CompensationRsCorrBw39_789kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw19_894kHz] = rsCorrBwArray[CompensationRsCorrBw19_894kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw9_947kHz] = rsCorrBwArray[CompensationRsCorrBw9_947kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw4_974kHz] = rsCorrBwArray[CompensationRsCorrBw4_974kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw2_487kHz] = rsCorrBwArray[CompensationRsCorrBw2_487kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw1_243kHz] = rsCorrBwArray[CompensationRsCorrBw1_243kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw0_622kHz] = rsCorrBwArray[CompensationRsCorrBw0_622kHz].niceLabel();
    compensationOptionStrings[CompRsCorr][CompensationRsCorrBw0_311kHz] = rsCorrBwArray[CompensationRsCorrBw0_311kHz].niceLabel();


    /*! Default values */
    currentRange = vcCurrentRangesArray[defaultVcCurrentRangeIdx];
    currentResolution = currentRange.step;
    voltageRange = vcVoltageRangesArray[defaultVcVoltageRangeIdx];
    voltageResolution =voltageRange.step;
    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    // Default USER DOMAIN compensation parameters
    defaultUserDomainParams.resize(CompensationUserParamsNum);
    defaultUserDomainParams[U_CpVc] = pipetteCapacitanceRange[0].min;
    defaultUserDomainParams[U_Cm] = membraneCapValueRange[0].min;
    defaultUserDomainParams[U_Rs] = membraneCapTauValueRange[0].min/membraneCapValueRange[0].min;
    defaultUserDomainParams[U_RsCp] = 1.0;
    defaultUserDomainParams[U_RsPg] = rsPredGainRange.min;
    defaultUserDomainParams[U_CpCc] = pipetteCapacitanceRange[0].min;

    // Selected default Idx
    selectedVcCurrentRangeIdx = defaultVcCurrentRangeIdx;
    selectedVcVoltageRangeIdx = defaultVcVoltageRangeIdx;
    selectedVcCurrentFilterIdx = defaultVcCurrentFilterIdx;
    selectedSamplingRateIdx = defaultSamplingRateIdx;

    // Initialization of the USER compensation domain with standard parameters
    for(int i = 0; i < currentChannelsNum; i++){
        compValueMatrix[i][U_CpVc] =  defaultUserDomainParams[U_CpVc];
        compValueMatrix[i][U_Cm] =  defaultUserDomainParams[U_Cm];
        compValueMatrix[i][U_Rs] =  defaultUserDomainParams[U_Rs];
        compValueMatrix[i][U_RsCp] =  defaultUserDomainParams[U_RsCp];
        compValueMatrix[i][U_RsPg] =  defaultUserDomainParams[U_RsPg];
        compValueMatrix[i][U_CpCc] =  defaultUserDomainParams[U_CpCc];
    }

    // Initialization of the RsCorr bandwidth option with default option
    for(int i = 0; i < currentChannelsNum; i++){
        selectedRsCorrBws[i] = defaultRsCorrBwIdx;
    }

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;
    MultiCoder::MultiCoderConfig_t multiCoderConfig;

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
#ifdef E384PATCH_ADDITIONAL_SR_FLAG
    samplingRateCoder = new BoolRandomArrayCoder(boolConfig);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(8);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(9);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(10);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(11);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(12);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(0);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(1);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(2);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(3);
    static_cast <BoolRandomArrayCoder *> (samplingRateCoder)->addMapItem(4);
#else
    samplingRateCoder = new BoolArrayCoder(boolConfig);
#endif
    coders.push_back(samplingRateCoder);

    /*! Clamping mode */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 7;
    boolConfig.bitsNum = 2;
    clampingModeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(clampingModeCoder);

    /*! Voltage channel / source */
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    voltageChanSourceCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(voltageChanSourceCoder);

    /*! Voltage channel / source */
    boolConfig.initialWord = 3;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    currentChanSourceCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(currentChanSourceCoder);

    /*! Current range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 4;
    vcCurrentRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentRangeCoder);

    /*! Voltage range VC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 4;
    boolConfig.bitsNum = 4;
    vcVoltageRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageRangeCoder);

    /*! Current range CC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 8;
    boolConfig.bitsNum = 4;
    ccCurrentRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccCurrentRangeCoder);

    /*! Voltage range CC */
    boolConfig.initialWord = 10;
    boolConfig.initialBit = 12;
    boolConfig.bitsNum = 4;
    ccVoltageRangeCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccVoltageRangeCoder);

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

    /*! Digital offset compensation */
    boolConfig.initialWord = 12;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    digitalOffsetCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        digitalOffsetCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(digitalOffsetCompensationCoders[idx]);
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
    boolConfig.initialWord = 84;
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

    /*! VC_SW */
    boolConfig.initialWord = 108;
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
    boolConfig.initialWord = 132;
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
    boolConfig.initialWord = 156;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    vcCcSelCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        vcCcSelCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(vcCcSelCoders[idx]);
        boolConfig.initialBit++;
        if (boolConfig.initialBit == CMC_BITS_PER_WORD) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! CC_Stim_En */
    boolConfig.initialWord = 180;
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
        doubleConfig.initialWord = 448;
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
        doubleConfig.initialWord = 448;
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
        doubleConfig.initialWord = 832;
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
        doubleConfig.initialWord = 832;
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
        doubleConfig.initialWord = 4504;
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
        doubleConfig.initialWord = 1216;
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
    doubleConfig.initialWord = 1600;
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
        doubleConfig.initialWord = 1984;
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
    doubleConfig.initialWord = 1600;
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
        doubleConfig.initialWord = 1984;
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
    doubleConfig.initialWord = 2368;
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
        doubleConfig.initialWord = 2752;
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
    doubleConfig.initialWord = 2368;
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
        doubleConfig.initialWord = 2752;
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
    boolConfig.initialWord = 3136;
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

    /*! Cfast / pipette capacitance compensation VALUE*/
    pipetteCapValCompensationMultiCoders.resize(currentChannelsNum);

    boolConfig.initialWord = 3160;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 3160;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;

    multiCoderConfig.doubleCoderVector.resize(pipetteCapacitanceRanges);
    multiCoderConfig.thresholdVector.resize(pipetteCapacitanceRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        /*! to encode the range, last 2 bits of the total 8 bits of Cfast compenstion for each channel*/
        multiCoderConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiCoderConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < pipetteCapacitanceRanges; rangeIdx++) {
            doubleConfig.minValue = pipetteCapacitanceRange[rangeIdx].min; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.maxValue = pipetteCapacitanceRange[rangeIdx].max; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.resolution = pipetteCapacitanceRange[rangeIdx].step; /*! \todo RECHECK THESE VALUES!*/

            multiCoderConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiCoderConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < pipetteCapacitanceRanges-1) {
                /*! \todo RECHECK: computed as the mean between the upper bound (Cmax) of this range and the lower bound (Cmin) of the next range */
                multiCoderConfig.thresholdVector[rangeIdx] = 0.5*(pipetteCapacitanceRange[rangeIdx].max + pipetteCapacitanceRange[rangeIdx+1].min);
            }
        }
        pipetteCapValCompensationMultiCoders[idx] = new MultiCoder(multiCoderConfig);
        coders.push_back(pipetteCapValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 8;
        if(boolConfig.initialBit > 14){
            boolConfig.initialBit = 6;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cfast value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation ENABLE */
    boolConfig.initialWord = 3352;
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

    boolConfig.initialWord = 3376;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 3376;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;

    multiCoderConfig.doubleCoderVector.resize(membraneCapValueRanges);
    multiCoderConfig.thresholdVector.resize(membraneCapValueRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        /*! to encode the range, last 2 bits of the total 8 bits of Cfast compenstion for each channel*/
        multiCoderConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiCoderConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < membraneCapValueRanges; rangeIdx++) {
            doubleConfig.minValue = membraneCapValueRange[rangeIdx].min; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.maxValue = membraneCapValueRange[rangeIdx].max; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.resolution = membraneCapValueRange[rangeIdx].step; /*! \todo RECHECK THESE VALUES!*/

            multiCoderConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiCoderConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < membraneCapValueRanges-1) {
                /*! \todo RECHECK: computed as the mean between the upper bound (Cmax) of this range and the lower bound (Cmin) of the next range */
                multiCoderConfig.thresholdVector[rangeIdx] = 0.5*(membraneCapValueRange[rangeIdx].max + membraneCapValueRange[rangeIdx+1].min);
            }
        }
        membraneCapValCompensationMultiCoders[idx] = new MultiCoder(multiCoderConfig);
        coders.push_back(membraneCapValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 8;
        if(boolConfig.initialBit > 14){
            boolConfig.initialBit = 6;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cslow value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Cslow / membrane capacitance compensation TAU and TAU RANGES */
    membraneCapTauValCompensationMultiCoders.resize(currentChannelsNum);

    doubleConfig.initialWord = 3568;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 8;

    boolConfig.initialWord = 3760;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;

    multiCoderConfig.doubleCoderVector.resize(membraneCapTauValueRanges);
    multiCoderConfig.thresholdVector.resize(membraneCapTauValueRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        /*! to encode the range, individual bits starting from word 2244*/
        multiCoderConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiCoderConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < membraneCapTauValueRanges; rangeIdx++) {
            doubleConfig.minValue =  membraneCapTauValueRange[rangeIdx].min;
            doubleConfig.maxValue = membraneCapTauValueRange[rangeIdx].max;
            doubleConfig.resolution = membraneCapTauValueRange[rangeIdx].step;

            multiCoderConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiCoderConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < membraneCapTauValueRanges-1) {
//                multiCoderConfig.thresholdVector[rangeIdx] = membraneCapTauValueMax_us[rangeIdx] + membraneCapTauValueStep_us[rangeIdx];
                multiCoderConfig.thresholdVector[rangeIdx] = membraneCapTauValueRange[rangeIdx].max + membraneCapTauValueRange[rangeIdx].step;
            }
        }
        membraneCapTauValCompensationMultiCoders[idx] = new MultiCoder(multiCoderConfig);
        coders.push_back(membraneCapTauValCompensationMultiCoders[idx]);


        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }

        boolConfig.initialBit++;
        if(boolConfig.initialBit == CMC_BITS_PER_WORD){
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs correction compensation ENABLE*/
    boolConfig.initialWord = 3784;
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
    doubleConfig.initialWord = 3808;
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
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Rs correction compensation BANDWIDTH */
    /*! \todo QUESTO VIENE IMPATTATO DALLA SWITCHED CAP FREQUENCY SOLO  A LIVELLO DI RAPPRESENTAZINE DI STRINGHE PER LA BANDA NELLA GUI. ATTIVAMENTE QUI NN FACCIAMO NULLA!!!!!*/
    boolConfig.initialWord = 4000;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 3;
    rsCorrBwCompensationCoders.resize(currentChannelsNum);
    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        rsCorrBwCompensationCoders[idx] = new BoolArrayCoder(boolConfig);
        coders.push_back(rsCorrBwCompensationCoders[idx]);
        boolConfig.initialBit += 4;
        if (boolConfig.initialBit >12) {
            boolConfig.initialBit = 0;
            boolConfig.initialWord++;
        }
    }

    /*! Rs PREDICTION compensation ENABLE*/
    boolConfig.initialWord = 4096;
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
    doubleConfig.initialWord = 4120;
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
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! Rs prediction compensation TAU*/
    doubleConfig.initialWord = 4312;
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
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    /*! CURRENT CLAMP Cfast / pipette capacitance compensation ENABLE */
    boolConfig.initialWord = 3136;
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

    boolConfig.initialWord = 3160;
    boolConfig.initialBit = 6;
    boolConfig.bitsNum = 2;

    doubleConfig.initialWord = 3160;
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 6;

    multiCoderConfig.doubleCoderVector.resize(pipetteCapacitanceRanges);
    multiCoderConfig.thresholdVector.resize(pipetteCapacitanceRanges-1);

    for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
        /*! to encode the range, last 2 bits of the total 8 bits of Cfast compenstion for each channel*/
        multiCoderConfig.boolCoder = new BoolArrayCoder(boolConfig);
        coders.push_back(multiCoderConfig.boolCoder);
        for (uint32_t rangeIdx = 0; rangeIdx < pipetteCapacitanceRanges; rangeIdx++) {
            doubleConfig.minValue = pipetteCapacitanceRange[rangeIdx].min; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.maxValue = pipetteCapacitanceRange[rangeIdx].max; /*! \todo RECHECK THESE VALUES!*/
            doubleConfig.resolution = pipetteCapacitanceRange[rangeIdx].step; /*! \todo RECHECK THESE VALUES!*/

            multiCoderConfig.doubleCoderVector[rangeIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(multiCoderConfig.doubleCoderVector[rangeIdx]);

            if (rangeIdx < pipetteCapacitanceRanges-1) {
                /*! \todo RECHECK: computed as the mean between the upper bound (Cmax) of this range and the lower bound (Cmin) of the next range */
                multiCoderConfig.thresholdVector[rangeIdx] = 0.5*(pipetteCapacitanceRange[rangeIdx].max + pipetteCapacitanceRange[rangeIdx+1].min);
            }
        }
        pipetteCapCcValCompensationMultiCoders[idx] = new MultiCoder(multiCoderConfig);
        coders.push_back(pipetteCapCcValCompensationMultiCoders[idx]);

        /*! Initial bits for the 2 bits for range : 6 and 6+8 = 14 */
        boolConfig.initialBit += 8;
        if(boolConfig.initialBit > 14){
            boolConfig.initialBit = 6;
            boolConfig.initialWord++;
        }

        /*! Initial bits for the 6 bits for Cfast value : 0 and 0+8 = 8 */
        doubleConfig.initialBit += 8;
        if (doubleConfig.initialBit > 8){
            doubleConfig.initialBit  = 0;
            doubleConfig.initialWord++;
        }
    }

    // Initialization Compensable in USER domain
    std::vector<double> defaultAsicDomainParams;
    for(int i = 0; i<currentChannelsNum; i++){
        defaultAsicDomainParams = user2AsicDomainTransform(i, defaultUserDomainParams);
        asic2UserDomainCompensable(i, defaultAsicDomainParams, defaultUserDomainParams);
    }

    /*! Default status */
    txStatus.resize(txDataWords);
    fill(txStatus.begin(), txStatus.end(), 0x0000);
    txStatus[2] = 0x0070; // fans on by default
}

Emcr384PatchClamp_prot_v04_fw_v04::~Emcr384PatchClamp_prot_v04_fw_v04() {

}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::initializeHW() {
    std::this_thread::sleep_for(std::chrono::seconds(motherboardBootTime_s));

    this->resetFpga(true, true);
    this->resetFpga(false, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    this->resetAsic(true, true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    this->resetAsic(false, true);

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getCompOptionsFeatures(CompensationTypes type, std::vector <std::string> &compOptionsArray){
    switch(type) {
    case CompRsCorr:
        if(rsCorrBwArray.size()==0){
            return ErrorFeatureNotImplemented;
        } else {
            compOptionsArray.resize(CompensationRsCorrBwNum);
            for(uint32_t i = 0; i < CompensationRsCorrBwNum; i++){
                compOptionsArray[i] = compensationOptionStrings[CompRsCorr][i];
            }
            return Success;
        }

        break;

    default:
        return ErrorFeatureNotImplemented;
    }
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getCompValueMatrix(std::vector<std::vector<double>> &compValueMatrix){
    compValueMatrix = this->compValueMatrix;
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getCompensationEnables(std::vector<uint16_t> channelIndexes, uint16_t compTypeToEnable, std::vector<bool> &onValues){
    switch(compTypeToEnable){
    case CompCfast:
        if(pipetteCapEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        }
        for(int i = 0; i<channelIndexes.size(); i++){
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[compTypeToEnable][channelIndexes[i]];
        }
        break;

    case CompCslow:
        if(membraneCapEnCompensationCoders.size() == 0 ){
            return ErrorFeatureNotImplemented;
        }
        for(int i = 0; i<channelIndexes.size(); i++){
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[compTypeToEnable][channelIndexes[i]];
        }
        break;

    case CompRsCorr:
        if(rsCorrEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        }
        for(int i = 0; i<channelIndexes.size(); i++){
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[compTypeToEnable][channelIndexes[i]];
        }
        break;

    case CompRsPred:
        if(rsPredEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        }
        for(int i = 0; i<channelIndexes.size(); i++){
            onValues[i] = vcCompensationsActivated && compensationsEnableFlags[compTypeToEnable][channelIndexes[i]];
        }
        break;

    case CompCcCfast:
        if(pipetteCapCcEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        }
        for(int i = 0; i<channelIndexes.size(); i++){
            onValues[i] =  ccCompensationsActivated && compensationsEnableFlags[compTypeToEnable][channelIndexes[i]];
        }
        break;

    default:
        return ErrorFeatureNotImplemented;
    }

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::enableCompensation(std::vector<uint16_t> channelIndexes, CompensationTypes compTypeToEnable, std::vector<bool> onValues, bool applyFlag){
#ifdef DEBUG_TX_DATA_PRINT
    std::string debugString = "";
#endif
    switch(compTypeToEnable){
    case CompCfast:
        if(pipetteCapEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
#ifdef DEBUG_TX_DATA_PRINT
        debugString += "enable cfast: ";
#endif
        for(int i = 0; i<channelIndexes.size(); i++){
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            pipetteCapEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            channelModels[channelIndexes[i]]->setCompensatingCfast(onValues[i]);
#ifdef DEBUG_TX_DATA_PRINT
            debugString += (onValues[i] ? std::to_string(channelIndexes[i]+1)+" ON, " : "");
#endif
        }
        break;

    case CompCslow:
        if(membraneCapEnCompensationCoders.size() == 0 ){
            return ErrorFeatureNotImplemented;
        } else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
#ifdef DEBUG_TX_DATA_PRINT
        debugString += "enable cslow: ";
#endif
        for(int i = 0; i<channelIndexes.size(); i++){
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            membraneCapEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            channelModels[channelIndexes[i]]->setCompensatingCslowRs(onValues[i]);
#ifdef DEBUG_TX_DATA_PRINT
            debugString += (onValues[i] ? std::to_string(channelIndexes[i]+1)+" ON, " : "");
#endif
        }
        break;

    case CompRsCorr:
        if(rsCorrEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
#ifdef DEBUG_TX_DATA_PRINT
        debugString += "enable rscorr: ";
#endif
        for(int i = 0; i<channelIndexes.size(); i++){
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            rsCorrEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            channelModels[channelIndexes[i]]->setCompensatingRsCp(onValues[i]);
            this->updateLiquidJunctionVoltage(channelIndexes[i], false);
#ifdef DEBUG_TX_DATA_PRINT
            debugString += (onValues[i] ? std::to_string(channelIndexes[i]+1)+" ON, " : "");
#endif
        }
        break;

    case CompRsPred:
        if(rsPredEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        }else if (!vcCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
#ifdef DEBUG_TX_DATA_PRINT
        debugString += "enable rspred: ";
#endif
        for(int i = 0; i<channelIndexes.size(); i++){
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            rsPredEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            channelModels[channelIndexes[i]]->setCompensatingRsPg(onValues[i]);
#ifdef DEBUG_TX_DATA_PRINT
            debugString += (onValues[i] ? std::to_string(channelIndexes[i]+1)+" ON, " : "");
#endif
        }
        break;

    case CompCcCfast:
        if(pipetteCapCcEnCompensationCoders.size() == 0){
            return ErrorFeatureNotImplemented;
        } else if (!ccCompensationsActivated) {
            return ErrorCompensationNotEnabled;
        }
#ifdef DEBUG_TX_DATA_PRINT
        debugString += "enable cccfast: ";
#endif
        for(int i = 0; i<channelIndexes.size(); i++){
            compensationsEnableFlags[compTypeToEnable][channelIndexes[i]] = onValues[i];
            pipetteCapCcEnCompensationCoders[channelIndexes[i]]->encode(onValues[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            channelModels[channelIndexes[i]]->setCompensatingCcCfast(onValues[i]);
#ifdef DEBUG_TX_DATA_PRINT
            debugString += (onValues[i] ? std::to_string(channelIndexes[i]+1)+" ON, " : "");
#endif

        }
        break;

    default:
        return ErrorFeatureNotImplemented;
    }

#ifdef DEBUG_TX_DATA_PRINT
    debugString += "\n";
    std::fprintf(txFid, debugString.c_str());
    std::fprintf(txFid, "\n");
    std::fflush(txFid);
#endif

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::enableVcCompensations(bool enable, bool applyFlag){
    vcCompensationsActivated = enable;

    for(int i = 0; i < currentChannelsNum; i++){
        pipetteCapEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompCfast][i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        membraneCapEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompCslow][i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        rsCorrEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompRsCorr][i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        rsPredEnCompensationCoders[i]->encode(vcCompensationsActivated && compensationsEnableFlags[CompRsPred][i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
        this->updateLiquidJunctionVoltage(i, false);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
#ifdef DEBUG_TX_DATA_PRINT
    std::fprintf(txFid, "Vc comps enabled %d\n", enable);
    std::fflush(txFid);
#endif

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::enableCcCompensations(bool enable, bool applyFlag){
    ccCompensationsActivated = enable;

    for(int i = 0; i < currentChannelsNum; i++){
        pipetteCapCcEnCompensationCoders[i]->encode(ccCompensationsActivated && compensationsEnableFlags[CompCcCfast][i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
    }

    if (applyFlag) {
        this->stackOutgoingMessage(txStatus);
    }
#ifdef DEBUG_TX_DATA_PRINT
    std::fprintf(txFid, "Cc comps enabled %d\n", enable);
    std::fflush(txFid);
#endif
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::setCompValues(std::vector<uint16_t> channelIndexes, CompensationUserParams paramToUpdate, std::vector<double> newParamValues, bool applyFlag){
    std::string debugString = "";
    // make local copy of the user domain param vectors
    std::vector<std::vector<double>> localCompValueSubMatrix;
    localCompValueSubMatrix.resize(channelIndexes.size());
    for (int chIdx = 0; chIdx < channelIndexes.size(); chIdx++) {
        localCompValueSubMatrix[chIdx] = this->compValueMatrix[channelIndexes[chIdx]];
    }

    // for each user param vector
    for (int chIdx = 0; chIdx < localCompValueSubMatrix.size(); chIdx++) {
        // update value in user domain
        localCompValueSubMatrix[chIdx][paramToUpdate] = newParamValues[chIdx];
        // convert user domain to asic domain
        std::vector<double> asicParams = this->user2AsicDomainTransform(channelIndexes[chIdx], localCompValueSubMatrix[chIdx]);
        double temp;

        // select asicParam to encode based on enum
        /*! \todo FCON recheck: IN CASE THERE'S INTERACTION AMONG ASICPARAMS, THEY COULD BE DESCRIBED IN THE SWITCH-CASE */
        switch(paramToUpdate) {
        case U_CpVc:
#ifdef DEBUG_TX_DATA_PRINT
            debugString += "[U_CpVc chan " + std::to_string(channelIndexes[chIdx]+1) + "]: userDom " + std::to_string(newParamValues[chIdx]) +", asicDom " + std::to_string(asicParams[A_Cp]) + "\n";
#endif
            //encode
            temp = pipetteCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Cp], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Cp] = temp;
            break;

        case U_Cm:
#ifdef DEBUG_TX_DATA_PRINT
            debugString += "[U_Cm chan " + std::to_string(channelIndexes[chIdx]+1) + "]: userDom " + std::to_string(newParamValues[chIdx]) +", asicDom " + std::to_string(asicParams[A_Cm]) + "\n";
#endif
            //encode
            temp = pipetteCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Cp], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Cp] = temp;

            //encode
            temp = membraneCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Cm], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Cm] = temp;

            //encode
            temp = membraneCapTauValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Taum], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Taum] = temp;
            break;

        case U_Rs:
#ifdef DEBUG_TX_DATA_PRINT
            debugString += "[U_Rs chan " + std::to_string(channelIndexes[chIdx]+1) + "]: userDom " + std::to_string(newParamValues[chIdx]) +", asicDom " + std::to_string(asicParams[A_Taum]) + "\n";
#endif
            //encode
            temp = membraneCapTauValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Taum], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_Taum] = temp;

            //encode
            temp = rsCorrValCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsCr], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_RsCr] = temp;
            break;

        case U_RsCp:
#ifdef DEBUG_TX_DATA_PRINT
            debugString += "[U_RsCp chan " + std::to_string(channelIndexes[chIdx]+1) + "]: userDom " + std::to_string(newParamValues[chIdx]) +", asicDom " + std::to_string(asicParams[A_RsCr]) + "\n";
#endif
            //encode
            temp = rsCorrValCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsCr], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_RsCr] = temp;
            break;

        case U_RsPg:
#ifdef DEBUG_TX_DATA_PRINT
            debugString += "[U_RsPg chan " + std::to_string(channelIndexes[chIdx]+1) + "]: userDom " + std::to_string(newParamValues[chIdx]) +", asicDom " + std::to_string(asicParams[A_RsPg]) + "\n";
#endif
            //encode
            temp = rsPredGainCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsPg], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_RsPg] = temp;

            //encode
            temp = rsPredTauCompensationCoders[channelIndexes[chIdx]]->encode(asicParams[A_RsPtau], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
            // update asic domain vector with coder return value
            asicParams[A_RsPtau] = temp;
            break;

        case U_CpCc:
#ifdef DEBUG_TX_DATA_PRINT
            debugString += "[U_CpCc chan " + std::to_string(channelIndexes[chIdx]+1) + "]: userDom " + std::to_string(newParamValues[chIdx]) +", asicDom " + std::to_string(asicParams[A_RsPtau]) + "\n";
#endif
            //encode
            temp = pipetteCapValCompensationMultiCoders[channelIndexes[chIdx]]->encode(asicParams[A_Cp], txStatus, txModifiedStartingWord, txModifiedEndingWord)   ;
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

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::setCompOptions(std::vector<uint16_t> channelIndexes, CompensationTypes type, std::vector<uint16_t> options, bool applyFlag){
#ifdef DEBUG_TX_DATA_PRINT
    std::string debugString = "";
#endif
    switch(type)
    {
    case CompRsCorr:
        if (rsCorrBwCompensationCoders.size() == 0) {
            return ErrorFeatureNotImplemented;
        } else {
            for(uint32_t i = 0; i < channelIndexes.size(); i++){
                selectedRsCorrBws[i] = options[i];
#ifdef DEBUG_TX_DATA_PRINT
            debugString += "[CompRsCorr chan " + std::to_string(channelIndexes[i]+1) + "]: selected opt " + std::to_string(selectedRsCorrBws[i]) +"\n";
#endif
                rsCorrBwCompensationCoders[channelIndexes[i]]->encode(options[i], txStatus, txModifiedStartingWord, txModifiedEndingWord);
            }

            if (applyFlag) {
                this->stackOutgoingMessage(txStatus);
            }
            return Success;
        }
    break;
    }
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::turnVoltageReaderOn(bool onValueIn, bool applyFlag){
    std::vector<bool> allTheTrueIneed;
    std::vector<bool> allTheFalseIneed;

    for(int i = 0; i< currentChannelsNum; i++){
        allTheTrueIneed.push_back(true);
        allTheFalseIneed.push_back(false);
    }

    if (onValueIn){
        this->turnCcSwOn(allChannelIndexes, allTheTrueIneed, false);
        this->turnVcCcSelOn(allChannelIndexes, allTheFalseIneed, false);
        this->updateCalibCcVoltageGain(allChannelIndexes, false);
        this->updateCalibCcVoltageOffset(allChannelIndexes, applyFlag);
        this->setAdcFilter();

    } else {
        this->turnCcSwOn(allChannelIndexes, allTheFalseIneed, applyFlag);
    }

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::turnCurrentReaderOn(bool onValueIn, bool applyFlag){
    std::vector<bool> allTheTrueIneed;
    std::vector<bool> allTheFalseIneed;

    for(int i = 0; i< currentChannelsNum; i++){
        allTheTrueIneed.push_back(true);
        allTheFalseIneed.push_back(false);
    }

    if(onValueIn){
        this->turnVcSwOn(allChannelIndexes, allTheTrueIneed, false);
        this->turnVcCcSelOn(allChannelIndexes, allTheTrueIneed, false);
        this->updateCalibVcCurrentGain(allChannelIndexes, false);
        this->updateCalibVcCurrentOffset(allChannelIndexes, applyFlag);
        this->setAdcFilter();

    }else{
        this->turnVcSwOn(allChannelIndexes, allTheFalseIneed, applyFlag);
    }

    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::turnVoltageStimulusOn(bool onValue, bool applyFlag){
    std::vector<bool> allTheTrueIneed;
    std::vector<bool> allTheFalseIneed;

    for(int i = 0; i< currentChannelsNum; i++){
        allTheTrueIneed.push_back(true);
        allTheFalseIneed.push_back(false);
    }

    if(onValue){
        this->updateCalibVcVoltageGain(allChannelIndexes, false);
        this->updateCalibVcVoltageOffset(allChannelIndexes, applyFlag);

    } else {

    }
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::turnCurrentStimulusOn(bool onValue, bool applyFlag){
    std::vector<bool> allTheTrueIneed;
    std::vector<bool> allTheFalseIneed;

    for(int i = 0; i< currentChannelsNum; i++){
        allTheTrueIneed.push_back(true);
        allTheFalseIneed.push_back(false);
    }

    if(onValue){
        this->enableCcStimulus(allChannelIndexes, allTheTrueIneed, false);
        this->updateCalibCcCurrentGain(allChannelIndexes, false);
        this->updateCalibCcCurrentOffset(allChannelIndexes, applyFlag);

    } else {
        this->enableCcStimulus(allChannelIndexes, allTheFalseIneed, applyFlag);
    }

    return Success;
}

std::vector<double> Emcr384PatchClamp_prot_v04_fw_v04::user2AsicDomainTransform(int chIdx, std::vector<double> userDomainParams){
    std::vector<double> asicDomainParameter;
    asicDomainParameter.resize(CompensationAsicParamsNum);
    double cp; // pipette capacitance
    double cm; // membrane capacitance
    double taum; // membrane capacitance tau
    double rsCr; // Rseries Correction
    double rsPg; //Rseries prediction gain
    double rsPtau; // Resies prediction tau

    double asicCmCinj;

    // membrane capacitance domain conversion
    cm = userDomainParams[U_Cm];

    // pipette capacitance VC to pipette capacitance domain conversion
    /*! \todo aggiungere check se il multicoder esiste sulla size del vettore di puntatori  a multiCoder*/

    MultiCoder::MultiCoderConfig_t aaa;
    membraneCapValCompensationMultiCoders[chIdx]->getMultiConfig(aaa);
    asicCmCinj = computeAsicCmCinj(userDomainParams[U_Cm], vcCompensationsActivated && compensationsEnableFlags[CompCslow][chIdx], aaa);

    if (selectedClampingModality == VOLTAGE_CLAMP){
        cp = userDomainParams[U_CpVc] + asicCmCinj;
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

std::vector<double> Emcr384PatchClamp_prot_v04_fw_v04::asic2UserDomainTransform(int chIdx, std::vector<double> asicDomainParams, double oldUCpVc, double oldUCpCc){
    std::vector<double> userDomainParameter;
    userDomainParameter.resize(CompensationUserParamsNum);

    double cpVc;
    double cm;
    double rs;
    double rsCp;
    double rsPg;
    double cpCC;

    double asicCmCinj;

    MultiCoder::MultiCoderConfig_t aaa;
    membraneCapValCompensationMultiCoders[chIdx]->getMultiConfig(aaa);
    asicCmCinj = computeAsicCmCinj(asicDomainParams[A_Cm], vcCompensationsActivated && compensationsEnableFlags[CompCslow][chIdx], aaa);

    //  pipette capacitance to pipette capacitance VC domain conversion
    if (selectedClampingModality == VOLTAGE_CLAMP){
        cpVc = asicDomainParams[A_Cp] - asicCmCinj;
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
    if (selectedClampingModality == VOLTAGE_CLAMP){
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

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::asic2UserDomainCompensable(int chIdx, std::vector<double> asicDomainParams, std::vector<double> userDomainParams){
    /*! \todo still to understand how to obtain them. COuld they be imputs of the function?*/
    std::vector<double> potentialMaxs;
    std::vector<double> potentialMins;

    double myInfinity = std::numeric_limits<double>::infinity();

    double asicCmCinj;
    MultiCoder::MultiCoderConfig_t aaa;
    membraneCapValCompensationMultiCoders[chIdx]->getMultiConfig(aaa);
    asicCmCinj = computeAsicCmCinj(asicDomainParams[A_Cm], vcCompensationsActivated && compensationsEnableFlags[CompCslow][chIdx], aaa);

    /*! Compensable for U_CpVc*/
    compensationControls[U_CpVc][chIdx].maxCompensable = pipetteCapacitanceRange.back().max - asicCmCinj;

    potentialMins.push_back(pipetteCapacitanceRange.front().min - asicCmCinj);
    potentialMins.push_back(0.0);
    compensationControls[U_CpVc][chIdx].minCompensable = *max_element(potentialMins.begin(), potentialMins.end());
    potentialMins.clear();

    compensationControls[U_CpVc][chIdx].step = pipetteCapacitanceRange.front().step;

    /*! Compensable for U_Cm*/
    // max
    /*! MPAC: added +1 after discussion with MBEN and FCON*/
    potentialMaxs.push_back(membraneCapValueRange.back().max);

    potentialMaxs.push_back(membraneCapTauValueRange.back().max/userDomainParams[U_Rs]);

    if(vcCompensationsActivated && compensationsEnableFlags[CompCfast][chIdx]){
        double zzz1;
        double zzz2;
        for (int i = 0; i < membraneCapValueInjCapacitance.size(); i++){
            zzz1 = membraneCapValueInjCapacitance[i] + userDomainParams[U_CpVc];
            if(zzz1 <= pipetteCapacitanceRange.back().max){
                zzz2 = zzz1;
            }
        }
        potentialMaxs.push_back(zzz2);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    if(vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]){
        potentialMaxs.push_back(rsPredTauRange.max*(userDomainParams[U_RsPg]+1)/userDomainParams[U_Rs]);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    compensationControls[U_Cm][chIdx].maxCompensable = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    potentialMins.push_back(membraneCapValueRange.front().min);

    potentialMins.push_back(membraneCapTauValueRange.front().min/userDomainParams[U_Rs]);

    if(vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]){
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
    if(vcCompensationsActivated && compensationsEnableFlags[CompCslow][chIdx]){
        potentialMaxs.push_back(membraneCapTauValueRange.back().max/userDomainParams[U_Cm]);
    } else {
        potentialMaxs.push_back(membraneCapTauValueRange.back().max/compensationControls[U_Cm][chIdx].minCompensable);
    }

    if(vcCompensationsActivated && compensationsEnableFlags[CompRsCorr][chIdx]){
        potentialMaxs.push_back(rsCorrValueRange.max / userDomainParams[U_RsCp] * 100.0);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    if(vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]){
        potentialMaxs.push_back(rsPredTauRange.max * (userDomainParams[U_RsPg]+1) / userDomainParams[U_Cm]);
    } else {
        potentialMaxs.push_back(myInfinity);
    }

    compensationControls[U_Rs][chIdx].maxCompensable = *min_element(potentialMaxs.begin(), potentialMaxs.end());
    potentialMaxs.clear();

    //min
    if(vcCompensationsActivated && compensationsEnableFlags[CompCslow][chIdx]){
        potentialMins.push_back(membraneCapTauValueRange.front().min / userDomainParams[U_Cm]);
    } else {
        potentialMins.push_back(membraneCapTauValueRange.front().min / compensationControls[U_Cm][chIdx].maxCompensable);
    }

    if(vcCompensationsActivated && compensationsEnableFlags[CompRsCorr][chIdx]){
        potentialMins.push_back(rsCorrValueRange.min / userDomainParams[U_RsCp] * 100.0);
    } else {
        potentialMins.push_back(0);
    }

    if(vcCompensationsActivated && compensationsEnableFlags[CompRsPred][chIdx]){
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
    compensationControls[U_CpCc][chIdx].maxCompensable = pipetteCapacitanceRange.back().max;
    compensationControls[U_CpCc][chIdx].minCompensable = pipetteCapacitanceRange.front().min;
    compensationControls[U_CpCc][chIdx].step = pipetteCapacitanceRange.front().step;

    return Success;
}

double Emcr384PatchClamp_prot_v04_fw_v04::computeAsicCmCinj(double cm, bool chanCslowEnable, MultiCoder::MultiCoderConfig_t multiconfigCslow){
    bool done = false;
    int i;
    double asicCmCinj;
    if((selectedClampingModality == CURRENT_CLAMP) || ((selectedClampingModality == VOLTAGE_CLAMP) && !chanCslowEnable)){
        asicCmCinj = 0;
    } else {
        for(i = 0; i<multiconfigCslow.thresholdVector.size(); i++){
            /*! \todo RECHECK: just <threshold as thresholds are as the mean between the upper bound (Cmax) of this range and the lower bound (Cmin) of the next range */
            if (cm < multiconfigCslow.thresholdVector[i] && !done){
                asicCmCinj = membraneCapValueInjCapacitance[i];
                done = true;
            }
        }
        if (!done){
            asicCmCinj = membraneCapValueInjCapacitance[i];
            done = true;
        }
    }
    return asicCmCinj;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getPipetteCapacitanceControl(CompensationControl_t &control) {
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
    control.name = "Pipette Capacitance";
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getCCPipetteCapacitanceControl(CompensationControl_t &control) {
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
    control.name = "Pipette Capacitance";
    return Success;
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getMembraneCapacitanceControl(CompensationControl_t &control) {
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
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getAccessResistanceControl(CompensationControl_t &control) {
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
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getResistanceCorrectionPercentageControl(CompensationControl_t &control) {
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
}

ErrorCodes_t Emcr384PatchClamp_prot_v04_fw_v04::getResistancePredictionGainControl(CompensationControl_t &control) {
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
}
