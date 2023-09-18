#include "messagedispatcher_384fakenanopores.h"

MessageDispatcher_384FakeNanopores::MessageDispatcher_384FakeNanopores(std::string id) :
    MessageDispatcher_384NanoPores_V01(id) {

    waitingTimeBeforeReadingData = 0;

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate6kHz].value = 6.0;
    samplingRatesArray[SamplingRate6kHz].prefix = UnitPfxKilo;
    samplingRatesArray[SamplingRate6kHz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate6kHz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate6kHz].value = 6.0;
    realSamplingRatesArray[SamplingRate6kHz].prefix = UnitPfxKilo;
    realSamplingRatesArray[SamplingRate6kHz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate6kHz].value = 1.0/6.0;
    integrationStepArray[SamplingRate6kHz].prefix = UnitPfxMilli;
    integrationStepArray[SamplingRate6kHz].unit = "s";
}

MessageDispatcher_384FakeNanopores::~MessageDispatcher_384FakeNanopores() {

}

ErrorCodes_t MessageDispatcher_384FakeNanopores::connect() {
    this->initializeBuffers();
    this->fillBuffer();
    startTime = std::chrono::steady_clock::now();

    return MessageDispatcher::connect();
}

ErrorCodes_t MessageDispatcher_384FakeNanopores::disconnect() {
    MessageDispatcher::disconnect();
    return this->deinitializeBuffers();
}

bool MessageDispatcher_384FakeNanopores::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    return true;
}

uint32_t MessageDispatcher_384FakeNanopores::readDataFromDevice() {
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */
#ifdef DEBUG_MAX_SPEED
    currentTime = std::chrono::steady_clock::now();
    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (currentTime-startTime).count());
    if (duration > (1000.0*(double)OKY_RX_TRANSFER_SIZE)/generatedSamplingRate) {
        startTime = currentTime;
        bytesRead = OKY_RX_TRANSFER_SIZE;
    }
#else
    /*! No data to receive, just sleep */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

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

//    totalBytesWritten += bytesRead;
//    long long duration = (std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::steady_clock::now()-startPrintfTime).count());
//    if (duration > 1000) {
//        printf("%lld kB/s\n", totalBytesWritten/duration);
//        fflush(stdout);
//        totalBytesWritten = 0;
//        startPrintfTime = std::chrono::steady_clock::now();
//    }
#endif

    return bytesRead;
}

ErrorCodes_t MessageDispatcher_384FakeNanopores::fillBuffer() {
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */

    while (bytesRead+(totalChannelsNum+3)*RX_WORD_SIZE < OKY_RX_BUFFER_SIZE) {
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X5A;
        rxRawBuffer[rxRawBufferWriteOffset] = 0XA5;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0x00;
        rxRawBuffer[rxRawBufferWriteOffset] = 0x00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        rxRawBuffer[rxRawBufferWriteOffset+1] = 0X03;
        rxRawBuffer[rxRawBufferWriteOffset] = 0X00;
        rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset+1] = (((syntheticData+idx*20) & 0x1F00) >> 8) - 0x10;
            rxRawBuffer[rxRawBufferWriteOffset] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset+1] = ((syntheticData+idx*20) & 0xFF00) >> 8;
            rxRawBuffer[rxRawBufferWriteOffset] = (syntheticData+idx*20) & 0x00FF;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }

        syntheticData += 1;
        bytesRead += (totalChannelsNum+3)*RX_WORD_SIZE;
    }

    return Success;
}
