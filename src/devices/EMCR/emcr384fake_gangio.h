#ifndef EMCR384FAKE_GANGIO_H
#define EMCR384FAKE_GANGIO_H

#include "emcr384patchclampfake.h"
#include <fstream>

class Emcr384fake_gangio: public Emcr384FakePatchClamp {

protected:
    void initializeVariables() override;
    ErrorCodes_t stopCommunication() override;
    bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    uint32_t readDataFromDevice() override;

public:
    Emcr384fake_gangio(std::string di);
private:
    void fillBuffer();
    uint32_t syntheticData = 0;
    double generatedByteRate = 4.0e6;

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
};

#endif // EMCR384FAKE_GANGIO_H
