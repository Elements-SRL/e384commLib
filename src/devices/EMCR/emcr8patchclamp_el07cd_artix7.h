#ifndef EMCR8PATCHCLAMP_EL07CD_ARTIX7_H
#define EMCR8PATCHCLAMP_EL07CD_ARTIX7_H

#include "emcrftdidevice.h"

class Emcr8PatchClamp_EL07c_artix7_PCBV01_fw_v01 : public EmcrFtdiDevice {
public:
    Emcr8PatchClamp_EL07c_artix7_PCBV01_fw_v01(std::string di);

    virtual ErrorCodes_t getCompOptionsFeatures(CompensationTypes_t type, std::vector <std::string> &compOptionsArray) override;
    virtual ErrorCodes_t getCompensationEnables(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <bool> &onValues) override;
    virtual ErrorCodes_t enableCompensation(std::vector <uint16_t> channelIndexes, CompensationTypes_t compTypeToEnable, std::vector <bool> onValues, bool applyFlag) override;
    virtual ErrorCodes_t enableVcCompensations(bool enable, bool applyFlag) override;
    virtual ErrorCodes_t enableCcCompensations(bool enable, bool applyFlag) override;
    virtual ErrorCodes_t setCompValues(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <double> newParamValues, bool applyFlag) override;
    virtual ErrorCodes_t setCompRanges(std::vector <uint16_t> channelIndexes, CompensationUserParams_t paramToUpdate, std::vector <uint16_t> newRanges, bool applyFlag) override;
    virtual ErrorCodes_t setCompOptions(std::vector <uint16_t> channelIndexes, CompensationTypes_t type, std::vector <uint16_t> options, bool applyFlag) override;

    virtual ErrorCodes_t turnVoltageReaderOn(bool onValueIn, bool applyFlag) override;
    virtual ErrorCodes_t turnCurrentReaderOn(bool onValueIn, bool applyFlag) override;
    virtual ErrorCodes_t turnVoltageStimulusOn(bool onValue, bool applyFlag) override;
    virtual ErrorCodes_t turnCurrentStimulusOn(bool onValue, bool applyFlag) override;

protected:
    const double nominalClock = 81.92; //MHz
    const double actualClock = 81.92; //MHz
    const double clockRatio = actualClock / nominalClock;
    std::vector <double> membraneCapValueInjCapacitance;

    std::vector <BoolArrayCoder *> grEnCoders;

    enum ClampingModalities {
        VoltageClamp,
        ZeroCurrentClamp,
        CurrentClamp,
#ifdef CALIBRATION
        VoltageClampVoltageRead,
        CurrentClampCurrentRead,
#endif
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
        CCCurrentRange2nA,
        CCCurrentRangesNum
    };

    enum CCVoltageRanges {
        CCVoltageRange250mV,
        CCVoltageRangesNum
    };

    enum VCCurrentFilters {
        VCCurrentFilter5kHz,
        VCCurrentFilter6kHz,
        VCCurrentFilter10kHz,
        VCCurrentFilter12kHz,
        VCCurrentFilter40kHz,
        VCCurrentFilter48kHz,
        VCCurrentFilter100kHz,
        VCCurrentFilter120kHz,
        VCCurrentFiltersNum
    };

    enum VCVoltageFilters {
        VCVoltageFilter1kHz,
        VCVoltageFilter10kHz,
        VCVoltageFilter20kHz,
        VCVoltageFilter50kHz,
        VCVoltageFiltersNum
    };

    enum CCCurrentFilters {
        CCCurrentFilter1kHz,
        CCCurrentFilter10kHz,
        CCCurrentFilter20kHz,
        CCCurrentFilter50kHz,
        CCCurrentFiltersNum
    };

    enum CCVoltageFilters {
        CCVoltageFilter5kHz,
        CCVoltageFilter6kHz,
        CCVoltageFilter10kHz,
        CCVoltageFilter12kHz,
        CCVoltageFilter40kHz,
        CCVoltageFilter48kHz,
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
        SamplingRate160kHz,
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

    enum CompensationRsCorrBws {
        CompensationRsCorrBw53_1kHz,
        CompensationRsCorrBw31_8kHz,
        CompensationRsCorrBw17_7kHz,
        CompensationRsCorrBw9_36kHz,
        CompensationRsCorrBwNum
    };

    enum CustomDouble {
        CustomOffset1,
        CustomOffset2,
        CustomOffset3,
        CustomOffset4,
        CustomOffset5,
        CustomOffset6,
        CustomOffset7,
        CustomOffset8,
        CustomDoublesNum
    };

    virtual ErrorCodes_t initializeHW() override;

    virtual std::vector <double> user2AsicDomainTransform(int chIdx, std::vector <double> userDomainParams) override;
    virtual std::vector <double> asic2UserDomainTransform(int chIdx, std::vector <double> asicDomainParams, double oldUCpVc, double oldUCpCc) override;
    virtual ErrorCodes_t asic2UserDomainCompensable(int chIdx, std::vector <double> asicDomainParams, std::vector <double> userDomainParams) override;

    virtual ErrorCodes_t getCompensationControl(CompensationUserParams_t param, CompensationControl_t &control) override;

    void setGrEn(bool flag, bool applyFlag);
};

class Emcr8PatchClamp_EL07c_artix7_PCBV02_fw_v01 : public Emcr8PatchClamp_EL07c_artix7_PCBV01_fw_v01 {
public:
    Emcr8PatchClamp_EL07c_artix7_PCBV02_fw_v01(std::string di);
};

class Emcr4PatchClamp_EL07c_artix7_PCBV01_fw_v01 : public Emcr8PatchClamp_EL07c_artix7_PCBV01_fw_v01 {
public:
    Emcr4PatchClamp_EL07c_artix7_PCBV01_fw_v01(std::string di);
};
#endif // EMCR8PATCHCLAMP_EL07CD_ARTIX7_H
