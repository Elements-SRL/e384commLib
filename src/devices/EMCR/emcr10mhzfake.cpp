#include "emcr10mhzfake.h"

Emcr10MHzFake::Emcr10MHzFake(std::string id) :
    Emcr10MHz_V01(id) {

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate1MHz].value = 1.0;
    samplingRatesArray[SamplingRate1MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate1MHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate1MHz;
    defaultSamplingRateIdx = SamplingRate1MHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate1MHz].value = 1.0;
    realSamplingRatesArray[SamplingRate1MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate1MHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate1MHz].value = 1000.0;
    integrationStepArray[SamplingRate1MHz].prefix = UnitPfxNano;
    integrationStepArray[SamplingRate1MHz].unit = "s";

    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    selectedSamplingRateIdx = defaultSamplingRateIdx;
}

ErrorCodes_t Emcr10MHzFake::startCommunication(std::string) {
    /*! Nothing to be done */
    return Success;
}

void Emcr10MHzFake::initializeVariables() {
    EmcrDevice::initializeVariables();
    this->fillBuffer();
    this->initializeLongBuffer();
    startTime = std::chrono::steady_clock::now();
}

ErrorCodes_t Emcr10MHzFake::initializeHW() {
    fwLoadedFlag = true;
    parsingStatus = ParsingParsing;
    return Success;
}

ErrorCodes_t Emcr10MHzFake::stopCommunication() {
    /*! Nothing to be done */
    fileLong.close();
    return Success;
}

bool Emcr10MHzFake::writeRegistersAndActivateTriggers(TxTriggerType_t) {
    return true;
}

uint32_t Emcr10MHzFake::readDataFromDevice() {
#ifdef DEBUG_MAX_SPEED
    uint32_t bytesRead = UDB_RX_TRANSFER_SIZE; /*!< Bytes read during last transfer from UDB */
#else
#ifdef SHORT_FILE
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from UDB */

    currentTime = std::chrono::steady_clock::now();
    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    if (duration > (1000.0*(double)UDB_RX_TRANSFER_SIZE)/generatedByteRate) {
        startTime = currentTime;
        bytesRead = UDB_RX_TRANSFER_SIZE;
    }
#else

    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from UDB */

    currentTime = std::chrono::steady_clock::now();
    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    if (duration < (1000.0*(double)UDB_RX_TRANSFER_SIZE)/generatedByteRate) {
        return bytesRead;
    }
    startTime = currentTime;

    while (bytesRead+((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+8)*RX_WORD_SIZE < UDB_RX_TRANSFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X08;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        if (fileOkLong) {
            if (readElementsLong+maxElementsLong > numElementsLong) {
                fileLong.seekg(0, std::ios::beg);
                readElementsLong = 0;
            }
            fileLong.read(reinterpret_cast <char *> (dataLong.data()), maxElementsLong*RX_WORD_SIZE);
            readElementsLong += maxElementsLong;
            syntheticData = 0;

            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = 0;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = 0;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (dataLong[syntheticData] & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = dataLong[syntheticData] & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }

        } else {
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }
        }
        bytesRead += ((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+8)*RX_WORD_SIZE;
    }

#endif
#endif
    return bytesRead;
}

void Emcr10MHzFake::fillBuffer() {
    std::string filename = "data.dat";
    std::ifstream file(filename, std::ios::binary);
    std::vector <int16_t> data;

    bool fileOk = file.good();
    if (fileOk) {
        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::size_t numElements = fileSize / sizeof(int16_t);

        // Read data into a vector
        data.resize(numElements);
        file.read(reinterpret_cast <char *> (data.data()), fileSize);

        file.close();
    }

    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from UDB */

    while (bytesRead+((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+8)*RX_WORD_SIZE < UDB_RX_BUFFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X08;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
        if (fileOk) {
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = 0;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = 0;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (data[syntheticData] & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = data[syntheticData] & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }

        } else {
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & UDB_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }
        }

        bytesRead += ((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+8)*RX_WORD_SIZE;
    }
}

void Emcr10MHzFake::initializeLongBuffer() {
    fileLong.open(filenameLong, std::ios::binary);

    fileOkLong = fileLong.good();
    if (fileOkLong) {
        fileLong.seekg(0, std::ios::end);
        fileSizeLong = fileLong.tellg();
        fileLong.seekg(0, std::ios::beg);

        numElementsLong = fileSizeLong / sizeof(int16_t);
//        maxElementsLong = UDB_RX_BUFFER_SIZE/RX_WORD_SIZE;
        maxElementsLong = packetsPerFrame*currentChannelsNum;

        dataLong.resize(maxElementsLong);
        readElementsLong = 0;
    }
}
