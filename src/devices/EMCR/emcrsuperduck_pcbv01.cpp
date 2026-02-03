#include "emcrsuperduck_pcbv01.h"

EmcrSuperDuck_PCBV01::EmcrSuperDuck_PCBV01(std::string di) :
    EmcrFtdiDevice(di) {

    spiChannel = 'A';
    rxChannel = 'B';
    txChannel = 'B';

    fpgaLoadType = FtdiFpgaFwLoadAutomatic;

    deviceName = "SuperDuck";

    fwSize_B = 0;
    motherboardBootTime_s = 0; // no motherboard to be started

    rxSyncWord = 0x5aa55aa5;

    packetsPerFrame = 1;

    voltageChannelsNum = 1;
    currentChannelsNum = 1;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 1;

    rxWordOffsets[RxMessageDataLoad] = 0;
    rxWordLengths[RxMessageDataLoad] = totalChannelsNum*packetsPerFrame;

    rxMaxWords = totalChannelsNum*packetsPerFrame; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 2;
    txDataWords = ((txDataWords+1)/2)*2; /*! Since registers are written in blocks of 2 16 bits words, create an even number */
    txMaxWords = txDataWords;
    txMaxRegs = (txMaxWords+1)/2; /*! Ceil of the division by 2 (each register is a 32 bits word) */

    properHeaderPackets = true;
    canDoEpisodic = true;

    /*! Clamping modalities */
    clampingModalitiesNum = ClampingModalitiesNum;
    clampingModalitiesArray.resize(clampingModalitiesNum);
    clampingModalitiesArray[VoltageClamp] = ClampingModality_t::VOLTAGE_CLAMP;
    defaultClampingModalityIdx = VoltageClamp;

    /*! Channel sources */
    availableVoltageSourcesIdxs.VoltageFromVoltageClamp = ChannelSourceVoltageFromVoltageClamp;

    voltageProtocolStepImplemented = false;
    voltageProtocolRampImplemented = false;
    voltageProtocolSinImplemented = false;

    currentProtocolStepImplemented = false;
    currentProtocolRampImplemented = false;
    currentProtocolSinImplemented = false;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange3330mV].max = 3330.0;
    vcCurrentRangesArray[VCCurrentRange3330mV].min = -3330.0;
    vcCurrentRangesArray[VCCurrentRange3330mV].step = vcCurrentRangesArray[VCCurrentRange3330mV].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange3330mV].prefix = UnitPfxMilli;
    vcCurrentRangesArray[VCCurrentRange3330mV].unit = "A";
    defaultVcCurrentRangeIdxs.resize(1);
    defaultVcCurrentRangeIdxs[0] = VCCurrentRange3330mV;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange2500mV].step = 62.5e-3;
    vcVoltageRangesArray[VCVoltageRange2500mV].min = -1024.0;
    vcVoltageRangesArray[VCVoltageRange2500mV].max = vcVoltageRangesArray[VCVoltageRange2500mV].min+vcVoltageRangesArray[VCVoltageRange2500mV].step*USHORT_MAX;
    vcVoltageRangesArray[VCVoltageRange2500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange2500mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange2500mV;

    liquidJunctionRangesNum = vcVoltageRangesNum;
    liquidJunctionRangesArray = vcVoltageRangesArray;
    defaultLiquidJunctionRangeIdx = defaultVcVoltageRangeIdx;

    /*! Current ranges */
    /*! CC */
    ccCurrentRangesNum = CCCurrentRangesNum;
    ccCurrentRangesArray.resize(ccCurrentRangesNum);

    /*! Voltage ranges */
    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;
    ccVoltageRangesArray.resize(ccVoltageRangesNum);

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;
    vcCurrentFiltersArray.resize(vcCurrentFiltersNum);
    vcCurrentFiltersArray[VCCurrentFilter10Hz].value = 10.0;
    vcCurrentFiltersArray[VCCurrentFilter10Hz].prefix = UnitPfxNone;
    vcCurrentFiltersArray[VCCurrentFilter10Hz].unit = "Hz";
    defaultVcCurrentFilterIdx = VCCurrentFilter10Hz;

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;
    vcVoltageFiltersArray.resize(vcVoltageFiltersNum);
    vcVoltageFiltersArray[VCVoltageFilter10Hz].value = 10.0;
    vcVoltageFiltersArray[VCVoltageFilter10Hz].prefix = UnitPfxNone;
    vcVoltageFiltersArray[VCVoltageFilter10Hz].unit = "Hz";
    defaultVcVoltageFilterIdx = VCVoltageFilter10Hz;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;
    ccCurrentFiltersArray.resize(ccCurrentFiltersNum);

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;
    ccVoltageFiltersArray.resize(ccVoltageFiltersNum);

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    defaultSamplingRateIdx = SamplingRate25kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate25kHz].value = 25.0;
    realSamplingRatesArray[SamplingRate25kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate25kHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate25kHz] = 0;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate25kHz].value = 40.0;
    integrationStepArray[SamplingRate25kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate25kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate25kHz, VCCurrentFilter10Hz}
    };

    defaultVoltageHoldTuner = {0.0, vcVoltageRangesArray[VCVoltageRange2500mV].prefix, vcVoltageRangesArray[VCVoltageRange2500mV].unit};

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

    // Selected default Idx
    selectedVcVoltageRangeIdx = defaultVcVoltageRangeIdx;
    selectedVcCurrentFilterIdx = defaultVcCurrentFilterIdx;
    selectedSamplingRateIdx = defaultSamplingRateIdx;

    /**********\
     * Coders *
    \**********/

    /*! Input controls */
    BoolCoder::CoderConfig_t boolConfig;
    DoubleCoder::CoderConfig_t doubleConfig;
    MultiCoder::CoderConfig_t multiConfig;

    /*! Sampling rate */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    samplingRateCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(samplingRateCoder);

    /*! Current range VC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    vcCurrentRangeCoders.clear();
    vcCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcCurrentRangeCoders[0]);

    /*! Voltage range VC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    vcVoltageRangeCoders.clear();
    vcVoltageRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(vcVoltageRangeCoders[0]);

    /*! Current range CC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    ccCurrentRangeCoders.clear();
    ccCurrentRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(ccCurrentRangeCoders[0]);

    /*! Voltage range CC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    ccVoltageRangeCoders.clear();
    ccVoltageRangeCoders.push_back(new BoolArrayCoder(boolConfig));
    coders.push_back(ccVoltageRangeCoders[0]);

    /*! Current filter VC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    vcCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcCurrentFilterCoder);

    /*! Voltage filter VC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    vcVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(vcVoltageFilterCoder);

    /*! Current filter CC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    ccCurrentFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccCurrentFilterCoder);

    /*! Voltage filter CC */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 1;
    boolConfig.bitsNum = 1;
    ccVoltageFilterCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(ccVoltageFilterCoder);

    /*! Enable stimulus */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 4;
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

    /*! V holding tuner */
    doubleConfig.initialBit = 0;
    doubleConfig.bitsNum = 16;
    vHoldTunerCoders.resize(VCVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < VCVoltageRangesNum; rangeIdx++) {
        doubleConfig.initialWord = 1;
        doubleConfig.resolution = vcVoltageRangesArray[rangeIdx].step;
        doubleConfig.minValue = -doubleConfig.resolution*(double)0x4000;
        doubleConfig.maxValue = doubleConfig.minValue+doubleConfig.resolution*USHORT_MAX;
        vHoldTunerCoders[rangeIdx].resize(currentChannelsNum);
        for (uint32_t channelIdx = 0; channelIdx < currentChannelsNum; channelIdx++) {
            vHoldTunerCoders[rangeIdx][channelIdx] = new DoubleOffsetBinaryCoder(doubleConfig);
            coders.push_back(vHoldTunerCoders[rangeIdx][channelIdx]);
            doubleConfig.initialWord++;
        }
    }

    /*! Default status */
    txStatus.init(txDataWords);
    txStatus.encodingWords[0] = 0x000C;
    txStatus.encodingWords[1] = 0x4000;
}

ErrorCodes_t EmcrSuperDuck_PCBV01::initializeHW() {
    std::this_thread::sleep_for (std::chrono::seconds(motherboardBootTime_s));

    return Success;
}
