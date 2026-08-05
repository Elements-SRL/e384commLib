#include "emcr32x10mhz_only_dig_pcbv01.h"

Emcr32x10MHz_OnlyDig_PCBV01::Emcr32x10MHz_OnlyDig_PCBV01(std::string di) :
    EmcrOpalKellyDevice(di) {

    deviceName = "32x10MHz_only_dig";

    waitingTimeBeforeReadingData = 2; //s

    rxSyncWord = 0x5aa55aa5;

    packetsPerFrame = 8;

    voltageChannelsNum = 32;
    currentChannelsNum = 32;
    totalChannelsNum = voltageChannelsNum+currentChannelsNum;

    totalBoardsNum = 4;

    rxWordOffsets[RxMessageVoltageDataLoad] = 1;
    rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    rxWordOffsets[RxMessageCurrentBlocksDataLoad] = voltageChannelsNum;
    rxWordLengths[RxMessageCurrentBlocksDataLoad] = currentChannelsNum*packetsPerFrame;

    // rxWordOffsets[RxMessageVoltageDataLoad] = 0;
    // rxWordLengths[RxMessageVoltageDataLoad] = voltageChannelsNum;

    // rxWordOffsets[RxMessageCurrentBlocksDataLoad] = rxWordOffsets[RxMessageVoltageDataLoad] + rxWordLengths[RxMessageVoltageDataLoad];
    // rxWordLengths[RxMessageCurrentBlocksDataLoad] = currentChannelsNum*packetsPerFrame;

    rxCurrentBlockLength = 8;

    rxMaxWords = currentChannelsNum*packetsPerFrame; /*! \todo FCON da aggiornare se si aggiunge un pacchetto di ricezione più lungo del pacchetto dati */
    maxInputDataLoadSize = rxMaxWords*RX_WORD_SIZE;

    txDataWords = 200; /*! \todo FCON AGGIORNARE MAN MANO CHE SI AGGIUNGONO CAMPI */
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

    voltageProtocolStepImplemented = false;
    voltageProtocolRampImplemented = false;
    voltageProtocolSinImplemented = false;

    /*! Current ranges */
    /*! VC */
    vcCurrentRangesNum = VCCurrentRangesNum;
    vcCurrentRangesArray.resize(vcCurrentRangesNum);
    vcCurrentRangesArray[VCCurrentRange100nA].min = -100.0;
    vcCurrentRangesArray[VCCurrentRange100nA].max = 100.0;
    vcCurrentRangesArray[VCCurrentRange100nA].step = vcCurrentRangesArray[VCCurrentRange100nA].max/SHORT_MAX;
    vcCurrentRangesArray[VCCurrentRange100nA].prefix = UnitPfxNano;
    vcCurrentRangesArray[VCCurrentRange100nA].unit = "A";
    defaultVcCurrentRangeIdxs.resize(1);
    defaultVcCurrentRangeIdxs[0] = VCCurrentRange100nA;

    /*! Voltage ranges */
    /*! VC */
    vcVoltageRangesNum = VCVoltageRangesNum;
    vcVoltageRangesArray.resize(vcVoltageRangesNum);
    vcVoltageRangesArray[VCVoltageRange500mV].min = -500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].max = 500.0;
    vcVoltageRangesArray[VCVoltageRange500mV].step = 0.0625;
    vcVoltageRangesArray[VCVoltageRange500mV].prefix = UnitPfxMilli;
    vcVoltageRangesArray[VCVoltageRange500mV].unit = "V";
    defaultVcVoltageRangeIdx = VCVoltageRange500mV;

    /*! Current ranges */
    /*! CC */
    ccCurrentRangesNum = CCCurrentRangesNum;

    /*! Voltage ranges */
    /*! CC */
    ccVoltageRangesNum = CCVoltageRangesNum;

    /*! Current filters */
    /*! VC */
    vcCurrentFiltersNum = VCCurrentFiltersNum;

    /*! Voltage filters */
    /*! VC */
    vcVoltageFiltersNum = VCVoltageFiltersNum;

    liquidJunctionRangesNum = vcVoltageRangesNum;
    liquidJunctionRangesArray = vcVoltageRangesArray;
    defaultLiquidJunctionRangeIdx = defaultVcVoltageRangeIdx;

    /*! Current filters */
    /*! CC */
    ccCurrentFiltersNum = CCCurrentFiltersNum;

    /*! Voltage filters */
    /*! CC */
    ccVoltageFiltersNum = CCVoltageFiltersNum;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    defaultSamplingRateIdx = SamplingRate625kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate625kHz].value = 625.0;
    realSamplingRatesArray[SamplingRate625kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate625kHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate625kHz] = 0;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate625kHz].value = 1000.0/625.0;
    integrationStepArray[SamplingRate625kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate625kHz].unit = "s";

    // mapping ADC Voltage Clamp
    sr2LpfVcCurrentMap = {
        {SamplingRate625kHz, -1},
    };

    // mapping ADC Current Clamp
    // undefined

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

    /*! FPGA reset */
    boolConfig.initialWord = 0;
    boolConfig.initialBit = 0;
    boolConfig.bitsNum = 1;
    fpgaResetCoder = new BoolArrayCoder(boolConfig);
    coders.push_back(asicResetCoder);

    /*! Default status */
    txStatus.init(txDataWords);
    txStatus.encodingWords[0] = 0x8028;
}

ErrorCodes_t Emcr32x10MHz_OnlyDig_PCBV01::initializeHW() {
    fpgaResetCoder->encode(1, txStatus);
    this->stackOutgoingMessage(txStatus);
    fpgaResetCoder->encode(0, txStatus);
    this->stackOutgoingMessage(txStatus);
    return Success;
}