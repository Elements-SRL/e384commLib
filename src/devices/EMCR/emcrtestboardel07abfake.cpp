#include "emcrtestboardel07abfake.h"

EmcrTestBoardEl07abFake::EmcrTestBoardEl07abFake(std::string id) :
    EmcrTestBoardEl07ab(id) {

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    defaultSamplingRateIdx = SamplingRate5kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate5kHz].value = 5.0;
    realSamplingRatesArray[SamplingRate5kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate5kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate5kHz].value = 200.0;
    integrationStepArray[SamplingRate5kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate5kHz].unit = "s";

    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    selectedSamplingRateIdx = defaultSamplingRateIdx;
}

ErrorCodes_t EmcrTestBoardEl07abFake::startCommunication(std::string) {
    /*! Nothing to be done */
    return Success;
}

void EmcrTestBoardEl07abFake::initializeVariables() {
    EmcrDevice::initializeVariables();
    this->fillBuffer();
    this->initializeLongBuffer();
    startTime = std::chrono::steady_clock::now();
}

ErrorCodes_t EmcrTestBoardEl07abFake::initializeHW() {
    parsingStatus = ParsingParsing;
    return Success;
}

ErrorCodes_t EmcrTestBoardEl07abFake::stopCommunication() {
    /*! Nothing to be done */
    fileLong.close();
    return Success;
}

bool EmcrTestBoardEl07abFake::writeRegistersAndActivateTriggers(TxTriggerType_t) {
    return true;
}

uint32_t EmcrTestBoardEl07abFake::readDataFromDevice() {
#ifdef DEBUG_MAX_SPEED
    uint32_t bytesRead = UDB_RX_TRANSFER_SIZE; /*!< Bytes read during last transfer from UDB */
#else
#ifdef SHORT_FILE
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from UDB */

    currentTime = std::chrono::steady_clock::now();
    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    if (duration > (1000.0*(double)okTransferSize)/generatedByteRate) {
        startTime = currentTime;
        bytesRead = UDB_RX_TRANSFER_SIZE;
    }
#else

    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from UDB */

    currentTime = std::chrono::steady_clock::now();
    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    if (duration < (1000.0*(double)okTransferSize)/generatedByteRate) {
        return bytesRead;
    }
    startTime = currentTime;

    while (bytesRead+((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+6)*RX_WORD_SIZE < okTransferSize) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X10;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
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
                    rxRawBuffer[rxRawBufferWriteOffset] = (dataLong[syntheticData] & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = dataLong[syntheticData] & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (dataLong[syntheticData] & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = dataLong[syntheticData] & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }

        } else {
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }
        }
        bytesRead += ((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+6)*RX_WORD_SIZE;
    }

#endif
#endif
    return bytesRead;
}

void EmcrTestBoardEl07abFake::fillBuffer() {
    std::string filename = "iv_long.dat";
    std::ifstream file(filename, std::ios::binary);
    std::vector <int16_t> data;
    std::size_t numElements;

    bool fileOk = file.good();
    if (fileOk) {
        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        numElements = fileSize / sizeof(int16_t);

        // Read data into a vector
        data.resize(numElements);
        file.read(reinterpret_cast <char *> (data.data()), fileSize);

        file.close();

    } else {
        return;
    }

    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from UDB */

    while (bytesRead+((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+6)*RX_WORD_SIZE < OKY_RX_BUFFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X10;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        if (fileOk) {
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (data[syntheticData] & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = data[syntheticData] & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                    syntheticData++;
                    if (syntheticData == numElements) {
                        syntheticData = 0;
                    }
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (data[syntheticData] & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = data[syntheticData] & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                    syntheticData++;
                    if (syntheticData == numElements) {
                        syntheticData = 0;
                    }
                }
            }

        } else {
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                }
            }
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
                    rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
                    rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                    rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                    syntheticData++;
                }
            }
        }

        bytesRead += ((voltageChannelsNum+currentChannelsNum)*packetsPerFrame+6)*RX_WORD_SIZE;
    }
}

void EmcrTestBoardEl07abFake::initializeLongBuffer() {
    fileLong.open(filenameLong, std::ios::binary);

    fileOkLong = fileLong.good();
    if (fileOkLong) {
        fileLong.seekg(0, std::ios::end);
        fileSizeLong = fileLong.tellg();
        fileLong.seekg(0, std::ios::beg);

        numElementsLong = fileSizeLong / sizeof(int16_t);
        maxElementsLong = packetsPerFrame*totalChannelsNum;

        dataLong.resize(maxElementsLong);
        readElementsLong = 0;
    }
}
