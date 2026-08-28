#ifndef EMCR10MHZFAKE_H
#define EMCR10MHZFAKE_H

#include "emcr10mhz.h"

#include <fstream>

class Emcr10MHzFake : public Emcr10MHz_V01 {
public:
    Emcr10MHzFake(std::string id);

protected:

    enum SamplingRates {
        SamplingRate1MHz,
        SamplingRatesNum
    };

    /*************\
     *  Methods  *
    \*************/

    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual void initializeVariables() override;

    virtual ErrorCodes_t initializeHW() override;
    virtual ErrorCodes_t stopCommunication() override;

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;

private:
    void fillBuffer();
    void initializeLongBuffer();

    uint32_t syntheticData = 0;
    double samplingRatesMHz = 1.25;
    double generatedByteRate = 4.0e6*samplingRatesMHz;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
    uint32_t totalBytesWritten = 0;

    std::string filenameLong = "data_long.dat";
    bool fileOkLong;
    std::ifstream fileLong;
    std::streampos fileSizeLong;
    std::size_t numElementsLong;
    std::size_t maxElementsLong;
    std::size_t readElementsLong;
    std::vector <int16_t> dataLong;

    int32_t bytesPerFrame = 0;
};

#endif // EMCR10MHZFAKE_H
