#ifndef EMCRTESTBOARDEL_5C5_V_1_H
#define EMCRTESTBOARDEL_5C5_V_1_H

#include "emcropalkellydevice.h"

class EmcrTestBoardEl05c5TransistorFeSingleEndedOut : public EmcrOpalKellyDevice {
public:
    EmcrTestBoardEl05c5TransistorFeSingleEndedOut(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange100nA,
        VCCurrentRange1uA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange1600mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFiltersNum = 0
    };

    enum VCVoltageFilters {
        VCVoltageFilter160Hz,
        VCVoltageFilter1_6kHz,
        VCVoltageFilter16kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate40MHz,
        SamplingRate20MHz,
        SamplingRate10MHz,
        SamplingRate5MHz,
        SamplingRate2_5MHz,
        SamplingRate1_25MHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

class EmcrTestBoardEl05c5ResistiveFeSingleEndedOut : public EmcrOpalKellyDevice {
public:
    EmcrTestBoardEl05c5ResistiveFeSingleEndedOut(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange100nA,
        VCCurrentRange10nA,
        VCCurrentRange1uA,
        VCCurrentRange10uA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange1600mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFiltersNum = 0
    };

    enum VCVoltageFilters {
        VCVoltageFilter160Hz,
        VCVoltageFilter1_6kHz,
        VCVoltageFilter16kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate40MHz,
        SamplingRate20MHz,
        SamplingRate10MHz,
        SamplingRate5MHz,
        SamplingRate2_5MHz,
        SamplingRate1_25MHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

class EmcrTestBoardEl05c5TransistorFeDifferentialOut : public EmcrOpalKellyDevice {
public:
    EmcrTestBoardEl05c5TransistorFeDifferentialOut(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange100nA,
        VCCurrentRange1uA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange1600mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFilter1MHz,
        VCCurrentFilter10MHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter160Hz,
        VCVoltageFilter1_6kHz,
        VCVoltageFilter16kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate40MHz,
        SamplingRate20MHz,
        SamplingRate10MHz,
        SamplingRate5MHz,
        SamplingRate2_5MHz,
        SamplingRate1_25MHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

class EmcrTestBoardEl05c5ResistiveFeDifferentialOut : public EmcrOpalKellyDevice {
public:
    EmcrTestBoardEl05c5ResistiveFeDifferentialOut(std::string di);

protected:
    enum ClampingModalities {
        VoltageClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0
    };

    enum VCCurrentRanges {
        VCCurrentRange10nA,
        VCCurrentRange100nA,
        VCCurrentRange1uA,
        VCCurrentRange10uA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange1600mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRangesNum = 0
    };

    enum CCVoltageRanges {
        CCVoltageRangesNum = 0
    };

    enum VCCurrentFilters {
        VCCurrentFilter1MHz,
        VCCurrentFilter10MHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter160Hz,
        VCVoltageFilter1_6kHz,
        VCVoltageFilter16kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFiltersNum = 0
    };

    enum CCVoltageFilters {
        CCVoltageFiltersNum = 0
    };

    enum SamplingRates {
        SamplingRate40MHz,
        SamplingRate20MHz,
        SamplingRate10MHz,
        SamplingRate5MHz,
        SamplingRate2_5MHz,
        SamplingRate1_25MHz,
        SamplingRatesNum
    };

    virtual ErrorCodes_t initializeHW() override;
};

#endif // EMCRTESTBOARDEL_5C5_V_1_H
