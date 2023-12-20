#ifndef EZPATCHFAKEP8_H
#define EZPATCHFAKEP8_H

#include "ezpatche8ppatchliner_el07ab.h"

class EZPatchFakeP8 : public EZPatche8PPatchliner_el07ab {
public:
    /*****************\
     *  Ctor / Dtor  *
    \*****************/

    EZPatchFakeP8(std::string di);
    ~EZPatchFakeP8();

    virtual void createCommunicationThreads() override;
    virtual void joinCommunicationThreads() override;

    /*******************************\
     *  Rx methods from generator  *
    \*******************************/

    void ackFromGenerator(uint16_t hb);
    void saturationFromGenerator();

    /******************************\
     *  Tx methods for generator  *
    \******************************/

    ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) override;
    ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag) override;
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag) override;
    ErrorCodes_t digitalOffsetCompensation(uint16_t channelIdx) override;
    ErrorCodes_t resetFpga() override;

protected:
    enum SamplingRates {
        SamplingRate1_25kHz,
        SamplingRate5kHz,
        SamplingRate10kHz,
        SamplingRate20kHz,
        SamplingRatesNum
    };

    typedef enum {
        ProtocolItemVStepTStep,
        ProtocolItemVRamp,
        ProtocolItemVSin,
        ProtocolItemIStepTStep,
        ProtocolItemIRamp,
        ProtocolItemISin
    } ProtocolItemType_t;

    typedef struct {
        ProtocolItemType_t type;
        double a0;
        double da;
        double b0;
        double db;
        double c0;
        double dc;
        unsigned short currentItem;
        unsigned short nextItem;
        unsigned short repsNum;
        bool applySteps;
    } ProtocolItem_t;

    typedef struct {
        unsigned short protocolId;
        unsigned short itemsNum;
        unsigned short sweepsNum;
        std::vector <ProtocolItem_t> items;
    } ProtocolStruct_t;

    /****************\
     *  Parameters  *
    \****************/

    /***************\
     *  Variables  *
    \***************/

    unsigned int ftdiRxMsgBufferReadLength = 0;

    unsigned int genRxMsgBufferLen = 0;

    /*! Variable used to access the rx msg buffer */
    uint32_t rxMsgBufferWriteOffset = 0;

    /*! Rx heartbeat variable */
    uint16_t rxHeartbeat = 0x0000;

    /*! Variables used to access the rx data buffer */
    uint32_t rxDataBufferWriteOffset = 0;

    bool ackFromGenAvailable = false;
    uint16_t lastAckHbFromGen;

    bool protocolAvailable = false;
    bool protocolOpened = false;

    ProtocolStruct_t builtProtocol;
    ProtocolStruct_t pushedProtocol;
    ProtocolStruct_t poppedProtocol;

    /***********************\
     *  Signals variables  *
    \***********************/

    double genResistance = 50.0e6;

    RangedMeasurement_t genVcVoltageRange;
    RangedMeasurement_t genCcVoltageRange;
    double genVHoldTuner = 0.0;
    double genVoltage = 0.0;
    double genVoltageStep = 0.0;
    double genVoltageAmp = 0.0;
    double genVoltageNorm;
    uint16_t genVoltageInt;

    RangedMeasurement_t genVcCurrentRange;
    RangedMeasurement_t genCcCurrentRange;
    double genIHoldTuner = 0.0;
    double genCurrent = 0.0;
    double genCurrentStep = 0.0;
    double genCurrentAmp = 0.0;
    double genCurrentNorm;
    uint16_t genCurrentInt;

    double wFHN = 0.0;
    double dwFHN = 0.0;
    double vFHN = 0.0;
    double dvFHN = 0.0;
    const double aFHN = 0.0007;
    const double bFHN = 0.8;
    const double tFHN = 5.0;

    Measurement_t genSamplingRate;
    double samplingTime = 0.0001;
    double integrationStep = 0.01;
    int integrationItemStepsNum;

    bool saturationFlag = false;

    /********************************************\
     *  Multi-thread synchronization variables  *
    \********************************************/

    mutable std::mutex genRxMutex;
    std::condition_variable genRxMsgBufferNotEmpty;
    std::condition_variable genRxMsgBufferNotFull;

    mutable std::mutex genTxMutex;
    std::condition_variable genTxProtocolAvailable;

    mutable std::mutex genParamMutex;

    std::thread gnThread;
    std::thread satThread;

    /*************\
     *  Methods  *
    \*************/

    void selectChannelsResolutions() override;

    void readAndParseMessagesForGenerator();

    void unwrapAndSendMessagesForGenerator();

    void generateData();

    inline double checkSaturation(double value);
    inline void integrateFHNModel(double value);
};

#endif // EZPATCHFAKEP8_H
