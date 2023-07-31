#include "messagedispatcher_2x10mhzfake.h"

MessageDispatcher_2x10MHzFake::MessageDispatcher_2x10MHzFake(std::string id) :
    MessageDispatcher_2x10MHz_V01(id) {

    waitingTimeBeforeReadingData = 0;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate2MHz].value = 2.0;
    samplingRatesArray[SamplingRate2MHz].prefix = UnitPfxMega;
    samplingRatesArray[SamplingRate2MHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate2MHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate2MHz].value = 2.0;
    realSamplingRatesArray[SamplingRate2MHz].prefix = UnitPfxMega;
    realSamplingRatesArray[SamplingRate2MHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate2MHz].value = 0.5;
    integrationStepArray[SamplingRate2MHz].prefix = UnitPfxMicro;
    integrationStepArray[SamplingRate2MHz].unit = "s";
}

MessageDispatcher_2x10MHzFake::~MessageDispatcher_2x10MHzFake() {

}

ErrorCodes_t MessageDispatcher_2x10MHzFake::connect() {
    this->initializeBuffers();
    this->fillBuffer();

    return MessageDispatcher::connect();
}

ErrorCodes_t MessageDispatcher_2x10MHzFake::disconnect() {
    MessageDispatcher::disconnect();
    return this->deinitializeBuffers();
}

bool MessageDispatcher_2x10MHzFake::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    return true;
}

uint32_t MessageDispatcher_2x10MHzFake::readDataFromDevice() {
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

ErrorCodes_t MessageDispatcher_2x10MHzFake::fillBuffer() {
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