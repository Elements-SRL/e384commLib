#include "emcr384patchclampfake.h"

#define TEST_DATA

Emcr384FakePatchClamp::Emcr384FakePatchClamp(std::string id) :
    Emcr384PatchClamp_EL07c_prot_v06_fw_v01(id) {

    waitingTimeBeforeReadingData = 0;
    motherboardBootTime_s = 0;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    defaultSamplingRateIdx = SamplingRate80kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate80kHz].value = samplingRatesMHz*1000.0;
    realSamplingRatesArray[SamplingRate80kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate80kHz].unit = "Hz";
    sr2srm.clear();
    sr2srm[SamplingRate80kHz] = 0;

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate80kHz].value = 1000.0/realSamplingRatesArray[SamplingRate80kHz].value;
    integrationStepArray[SamplingRate80kHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate80kHz].unit = "s";

    samplingRate = realSamplingRatesArray[defaultSamplingRateIdx];
    integrationStep = integrationStepArray[defaultSamplingRateIdx];

    selectedSamplingRateIdx = defaultSamplingRateIdx;
    bytesPerFrame = (totalChannelsNum*packetsPerFrame+6)*RX_WORD_SIZE;
}

ErrorCodes_t Emcr384FakePatchClamp::startCommunication(std::string fwPath) {
    /*! Nothing to be done */
    return Success;
}

void Emcr384FakePatchClamp::initializeVariables() {
    EmcrDevice::initializeVariables();
#ifdef TEST_DATA
    this->initializeLongBuffer();
#else
    this->fillBuffer();
#endif
    startTime = std::chrono::steady_clock::now();
}

ErrorCodes_t Emcr384FakePatchClamp::stopCommunication() {
    /*! Nothing to be done */
    return Success;
}

bool Emcr384FakePatchClamp::writeRegistersAndActivateTriggers(TxTriggerType_t) {
    return true;
}

uint32_t Emcr384FakePatchClamp::readDataFromDevice() {
    if (debugLevelEnabled(DebugLevelMaxSpeed)) {
        return OKY_RX_TRANSFER_SIZE; /*!< Bytes read during last transfer from Opal Kelly */
    }
#ifdef TEST_DATA

    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from OKY */

    currentTime = std::chrono::steady_clock::now();
    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    if (duration < (1000.0*(double)OKY_RX_TRANSFER_SIZE)/generatedByteRate) {
        return bytesRead;
    }
    startTime = currentTime;

    while (bytesRead+maxElementsLong < OKY_RX_TRANSFER_SIZE) {
        if (fileOkLong) {
            if (readElementsLong+maxElementsLong > numElementsLong) {
                fileLong.seekg(0, std::ios::beg);
                readElementsLong = 0;
            }
            fileLong.read(reinterpret_cast <char *> (dataLong.data()), maxElementsLong*RX_WORD_SIZE);
            readElementsLong += maxElementsLong;
            syntheticData = 0;

            for (uint32_t idx = 0; idx < maxElementsLong/2; idx++) {
                rxRawBuffer[rxRawBufferWriteOffset] = (dataLong[syntheticData] & 0xFF00) >> 8;
                rxRawBuffer[rxRawBufferWriteOffset+1] = dataLong[syntheticData] & 0x00FF;
                rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
                syntheticData++;
            }

        } else {
            rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
            rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
            rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
            rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
            rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
            rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
            rxRawBuffer[rxRawBufferWriteOffset] = 0X08;
            rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
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
        bytesRead += maxElementsLong;
    }
    return bytesRead;
#else
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */

    currentTime = std::chrono::steady_clock::now();
    int64_t duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    int64_t newDeltaBytes = duration * (int64_t)samplingRate.getNoPrefixValue() / 1000 * totalChannelsNum * RX_WORD_SIZE - (OKY_RX_TRANSFER_SIZE-deltaBytes);
    if (newDeltaBytes < 0) {
        return bytesRead;
    }
    deltaBytes = newDeltaBytes;
    startTime = currentTime;
    const double R = 10.0; // MOhm

    while (bytesRead+(totalChannelsNum+3)*RX_WORD_SIZE < OKY_RX_TRANSFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X03;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            uint16_t value = selectedVoltageHoldVector[idx].value/voltageResolutions[idx];
            rxRawBuffer[rxRawBufferWriteOffset] = (value >> 8) & 0x00FF;
            rxRawBuffer[rxRawBufferWriteOffset+1] = value & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            uint16_t value = selectedVoltageHoldVector[idx].value/R/currentResolutions[idx];
            rxRawBuffer[rxRawBufferWriteOffset] = (value >> 8) & 0x00FF;
            rxRawBuffer[rxRawBufferWriteOffset+1] = value & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        bytesRead += (totalChannelsNum+3)*RX_WORD_SIZE;
    }
    return bytesRead;
#endif
}

ErrorCodes_t Emcr384FakePatchClamp::fillBuffer() {
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */

    while (bytesRead+(totalChannelsNum+3)*RX_WORD_SIZE < OKY_RX_BUFFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X03;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        syntheticData += 1;
        bytesRead += (totalChannelsNum+3)*RX_WORD_SIZE;
    }

    return Success;
}

void Emcr384FakePatchClamp::initializeLongBuffer() {
    fileLong.open(filenameLong, std::ios::binary);

    fileOkLong = fileLong.good();
    if (fileOkLong) {
        fileLong.seekg(0, std::ios::end);
        fileSizeLong = fileLong.tellg();
        fileLong.seekg(0, std::ios::beg);

        numElementsLong = fileSizeLong / sizeof(int16_t);
        maxElementsLong = bytesPerFrame;

        dataLong.resize(maxElementsLong);
        readElementsLong = 0;
    }
}
