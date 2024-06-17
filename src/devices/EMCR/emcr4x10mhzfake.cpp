#include "emcr4x10mhzfake.h"

Emcr4x10MHzFake::Emcr4x10MHzFake(std::string id) :
    Emcr4x10MHz_PCBV01_V03(id) {

    waitingTimeBeforeReadingData = 0;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate1MHz].value = 1.0;
    samplingRatesArray[SamplingRate1MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate1MHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate1MHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate1MHz].value = 1.0;
    realSamplingRatesArray[SamplingRate1MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate1MHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate1MHz].value = 1.0;
    integrationStepArray[SamplingRate1MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate1MHz].unit = "s";
}

ErrorCodes_t Emcr4x10MHzFake::startCommunication(std::string fwPath) {
    /*! Nothing to be done */
    return Success;
}

void Emcr4x10MHzFake::initializeVariables() {
    EmcrDevice::initializeVariables();
    this->fillBuffer();
}

ErrorCodes_t Emcr4x10MHzFake::stopCommunication() {
    /*! Nothing to be done */
    return Success;
}

bool Emcr4x10MHzFake::writeRegistersAndActivateTriggers(TxTriggerType_t) {
    return true;
}

uint32_t Emcr4x10MHzFake::readDataFromDevice() {
#ifdef DEBUG_MAX_SPEED
    uint32_t bytesRead = OKY_RX_TRANSFER_SIZE; /*!< Bytes read during last transfer from Opal Kelly */
#else
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */

    /*! No data to receive, just sleep */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    while (bytesRead+(voltageChannelsNum+currentChannelsNum*packetsPerFrame+6)*RX_WORD_SIZE < OKY_RX_TRANSFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X04;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x04;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X04;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
                rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
            }
            syntheticData += 1;
        }

        bytesRead += (voltageChannelsNum+currentChannelsNum*packetsPerFrame+6)*RX_WORD_SIZE;
    }
#endif

    return bytesRead;
}

ErrorCodes_t Emcr4x10MHzFake::fillBuffer() {
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */

    while (bytesRead+(voltageChannelsNum+currentChannelsNum*packetsPerFrame+6)*RX_WORD_SIZE < OKY_RX_BUFFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X04;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
            rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        rxRawBuffer[rxRawBufferWriteOffset] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x04;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X04;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t pkIdx = 0; pkIdx < packetsPerFrame; pkIdx++) {
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                rxRawBuffer[rxRawBufferWriteOffset] = ((syntheticData+idx*20) & 0xFF00) >> 8;
                rxRawBuffer[rxRawBufferWriteOffset+1] = (syntheticData+idx*20) & 0x00FF;
                rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
            }
            syntheticData += 1;
        }

        bytesRead += (voltageChannelsNum+currentChannelsNum*packetsPerFrame+6)*RX_WORD_SIZE;
    }

    return Success;
}
