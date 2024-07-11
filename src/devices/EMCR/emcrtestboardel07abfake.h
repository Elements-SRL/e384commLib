#ifndef EMCRTESTBOARDEL07ABFAKE_H
#define EMCRTESTBOARDEL07ABFAKE_H

//#define SHORT_FILE

#include "emcrtestboardel07ab.h"

#include <fstream>

class EmcrTestBoardEl07abFake : public EmcrTestBoardEl07ab {
public:
    EmcrTestBoardEl07abFake(std::string id);

protected:

    enum SamplingRates {
        SamplingRate5kHz,
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
    double generatedByteRate = 160.0e3;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
    uint32_t totalBytesWritten = 0;

    std::string filenameLong = "iv_long.dat";
    bool fileOkLong;
    std::ifstream fileLong;
    std::streampos fileSizeLong;
    std::size_t numElementsLong;
    std::size_t maxElementsLong;
    std::size_t readElementsLong;
    std::vector <int16_t> dataLong;
};

#endif // EMCRTESTBOARDEL07ABFAKE_H
