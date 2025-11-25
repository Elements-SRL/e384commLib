#ifndef EMCR384PATCHCLAMPFAKE_H
#define EMCR384PATCHCLAMPFAKE_H

#include "emcr384patchclamp_el07cd_prot_v06_fw_v01.h"

#include <fstream>

class Emcr384FakePatchClamp: public Emcr384PatchClamp_EL07c_prot_v06_fw_v01 {
public:
    Emcr384FakePatchClamp(std::string id);

protected:

    enum SamplingRates {
        SamplingRate80kHz,
        SamplingRatesNum
    };

    /*************\
     *  Methods  *
    \*************/

    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual void initializeVariables() override;

    virtual ErrorCodes_t stopCommunication() override;

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;

private:
    ErrorCodes_t fillBuffer();
    void initializeLongBuffer();

    uint16_t syntheticData = 0;
    int64_t deltaBytes = 0;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
    double samplingRatesMHz = 0.08;
    double generatedByteRate = 768.0*2.0e6*samplingRatesMHz;

    std::string filenameLong = "e384CommLib_rxRaw.txt";
    bool fileOkLong;
    std::ifstream fileLong;
    std::streampos fileSizeLong;
    std::size_t numElementsLong;
    std::size_t maxElementsLong;
    std::size_t readElementsLong;
    std::vector <int16_t> dataLong;

    int32_t bytesPerFrame = 0;
};

#endif // EMCR384PATCHCLAMPFAKE_H
