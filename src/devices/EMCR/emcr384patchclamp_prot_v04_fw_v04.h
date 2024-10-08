#ifndef EMCR384PATCHCLAMP_PROT_V04_FW_V04_H
#define EMCR384PATCHCLAMP_PROT_V04_FW_V04_H

#include "emcropalkellydevice.h"

class Emcr384PatchClamp_prot_v04_fw_v04 : public EmcrOpalKellyDevice {
public:
    Emcr384PatchClamp_prot_v04_fw_v04(std::string di);

    virtual ErrorCodes_t getCompOptionsFeatures(CompensationTypes_t type, std::vector <std::string> &compOptionsArray) override;
    virtual ErrorCodes_t getCompensationEnables(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <bool> &onValues) override;
    virtual ErrorCodes_t enableCompensation(std::vector <uint16_t> channelIndexes, CompensationTypes_t compTypeToEnable, std::vector <bool> onValues, bool applyFlag) override;
    virtual ErrorCodes_t enableVcCompensations(bool enable, bool applyFlag) override;
    virtual ErrorCodes_t enableCcCompensations(bool enable, bool applyFlag) override;
    virtual ErrorCodes_t setCompValues(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <double> newParamValues, bool applyFlag) override;
    virtual ErrorCodes_t setCompOptions(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <uint16_t> options, bool applyFlag) override;

    virtual ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlag) override;
    virtual ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlag) override;
    virtual ErrorCodes_t turnVoltageStimulusOn(bool onValue, bool applyFlag) override;
    virtual ErrorCodes_t turnCurrentStimulusOn(bool onValue, bool applyFlag) override;

protected:
    const double nominalClock = 102.4; //MHz
    const double actualClock = 81.92; //MHz
    const double clockRatio = actualClock / nominalClock;
    std::vector <double> membraneCapValueInjCapacitance;

    enum ClampingModalities {
        VoltageClamp,
        CurrentClamp,
        ClampingModalitiesNum
    };

    enum ChannelSourcesIdxs {
        ChannelSourceVoltageFromVoltageClamp = 0,
        ChannelSourceVoltageFromCurrentClamp = 1,
//        ChannelSourceVoltageFromDynamicClamp = 2,
//        ChannelSourceVoltageFromVoltagePlusDynamicClamp = 3,
        ChannelSourceCurrentFromVoltageClamp = 0,
        ChannelSourceCurrentFromCurrentClamp = 1/*,
        ChannelSourceCurrentFromDynamicClamp = 2,
        ChannelSourceCurrentFromCurrentPlusDynamicClamp = 3*/
    };

    enum VCCurrentRanges {
        VCCurrentRange10nA,
        VCCurrentRange40nALbw,
        VCCurrentRange40nAHbw,
        VCCurrentRange400nA,
        VCCurrentRangesNum
    };

    enum VCVoltageRanges {
        VCVoltageRange500mV,
        VCVoltageRangesNum
    };

    enum CCCurrentRanges {
        CCCurrentRange8nA,
        CCCurrentRangesNum
    };

    enum CCVoltageRanges {
        CCVoltageRange1000mV,
        CCVoltageRangesNum
    };

    enum VCCurrentFilters {
        VCCurrentFilter3kHz,
        VCCurrentFilter3_6kHz,
        VCCurrentFilter10kHz,
        VCCurrentFilter12kHz,
        VCCurrentFilter25kHz,
        VCCurrentFilter30kHz,
        VCCurrentFilter100kHz,
        VCCurrentFilter120kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter1kHz,
        VCVoltageFilter10kHz,
        VCVoltageFilter35kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFilter1kHz,
        CCCurrentFilter10kHz,
        CCCurrentFilter35kHz,
        CCCurrentFiltersNum
    };

    enum CCVoltageFilters {
        CCVoltageFilter3kHz,
        CCVoltageFilter3_6kHz,
        CCVoltageFilter10kHz,
        CCVoltageFilter12kHz,
        CCVoltageFilter25kHz,
        CCVoltageFilter30kHz,
        CCVoltageFilter100kHz,
        CCVoltageFilter120kHz,
        CCVoltageFiltersNum
    };

    enum SamplingRates {
        SamplingRate5kHz,
        SamplingRate10kHz,
        SamplingRate20kHz,
        SamplingRate40kHz,
        SamplingRate80kHz,
        SamplingRatesNum
    };

    enum CompensationAsicParams {
        A_Cp,       //PipetteCapacitance
        A_Cm,       //MembraneCapacitance
        A_Taum,     //MembraneTau
        A_RsCr,     //SeriesCorrectionResistance
        A_RsPg,     //SeriesPredictionGain
        A_RsPtau,   //SeriesPredictionTau
        CompensationAsicParamsNum
    };

    enum CompensationRsCorrBws{
        CompensationRsCorrBw39_789kHz,
        CompensationRsCorrBw19_894kHz,
        CompensationRsCorrBw9_947kHz,
        CompensationRsCorrBw4_974kHz,
        CompensationRsCorrBw2_487kHz,
        CompensationRsCorrBw1_243kHz,
        CompensationRsCorrBw0_622kHz,
        CompensationRsCorrBw0_311kHz,
        CompensationRsCorrBwNum
    };

    virtual ErrorCodes_t initializeHW() override;

    virtual std::vector <double> user2AsicDomainTransform(int chIdx, std::vector <double> userDomainParams) override;
    virtual std::vector <double> asic2UserDomainTransform(int chIdx, std::vector <double> asicDomainParams, double oldUCpVc, double oldUCpCc) override;
    virtual ErrorCodes_t asic2UserDomainCompensable(int chIdx, std::vector <double> asicDomainParams, std::vector <double> userDomainParams) override;
    virtual double computeAsicCmCinj(double cm, bool chanCslowEnable, MultiCoder::MultiCoderConfig_t multiconfigCslow);

    virtual ErrorCodes_t getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) override;
};

#endif // EMCR384PATCHCLAMP_PROT_V04_FW_V04_H
