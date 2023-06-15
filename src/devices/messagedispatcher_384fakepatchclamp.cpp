#include "messagedispatcher_384fakepatchclamp.h"

MessageDispatcher_384FakePatchClamp::MessageDispatcher_384FakePatchClamp(std::string id) :
    MessageDispatcher_384PatchClamp_V01(id) {

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

MessageDispatcher_384FakePatchClamp::~MessageDispatcher_384FakePatchClamp() {

}

ErrorCodes_t MessageDispatcher_384FakePatchClamp::connect() {
    this->initializeBuffers();
    return MessageDispatcher::connect();
}

ErrorCodes_t MessageDispatcher_384FakePatchClamp::disconnect() {
    MessageDispatcher::disconnect();
    return this->deinitializeBuffers();
}

bool MessageDispatcher_384FakePatchClamp::writeRegistersAndActivateTriggers(TxTriggerType_t type) {
    return true;
}

uint32_t MessageDispatcher_384FakePatchClamp::readDataFromDevice() {
    /*! Declare variables to manage buffers indexing */
    uint32_t bytesRead = 0; /*!< Bytes read during last transfer from Opal Kelly */
    parsingFlag = true;

    /*! No data to receive, just sleep */
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    while (bytesRead+totalChannelsNum*RX_WORD_SIZE < OKY_RX_TRANSFER_SIZE) {
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
        bytesRead += totalChannelsNum*RX_WORD_SIZE;

        this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageDataLoad);
    }

    return bytesRead;
}
