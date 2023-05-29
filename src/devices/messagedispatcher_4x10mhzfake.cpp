#include "messagedispatcher_4x10mhzfake.h"

MessageDispatcher_4x10MHzFake::MessageDispatcher_4x10MHzFake(std::string id) :
    MessageDispatcher_4x10MHz_V01(id) {

    /*! Sampling rates */
    samplingRatesNum = SamplingRatesNum;
    samplingRatesArray.resize(samplingRatesNum);
    samplingRatesArray[SamplingRate100Hz].value =100.0;
    samplingRatesArray[SamplingRate100Hz].prefix = UnitPfxNone;
    samplingRatesArray[SamplingRate100Hz].unit = "Hz";
    defaultSamplingRateIdx = SamplingRate100Hz;

    realSamplingRatesArray.resize(samplingRatesNum);
    realSamplingRatesArray[SamplingRate100Hz].value = 100.0;
    realSamplingRatesArray[SamplingRate100Hz].prefix = UnitPfxNone;
    realSamplingRatesArray[SamplingRate100Hz].unit = "Hz";

    integrationStepArray.resize(samplingRatesNum);
    integrationStepArray[SamplingRate100Hz].value = 0.01;
    integrationStepArray[SamplingRate100Hz].prefix = UnitPfxNone;
    integrationStepArray[SamplingRate100Hz].unit = "s";

}

MessageDispatcher_4x10MHzFake::~MessageDispatcher_4x10MHzFake() {

}

ErrorCodes_t MessageDispatcher_4x10MHzFake::connect() {
    this->initializeBuffers();
    return MessageDispatcher::connect();
}

ErrorCodes_t MessageDispatcher_4x10MHzFake::disconnect() {
    MessageDispatcher::disconnect();
    return this->deinitializeBuffers();
}

void MessageDispatcher_4x10MHzFake::sendCommandsToDevice() {
    int writeTries = 0;
    okTRegisterEntries regs;
    regs.reserve(txMaxRegs);

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */

    /*! Variables used to access the tx data buffer */
    uint32_t txDataBufferReadIdx;

    bool notSentTxData;

    std::unique_lock <std::mutex> txMutexLock (txMutex);
    txMutexLock.unlock();

    while (!stopConnectionFlag) {

        /***********************\
         *  Data copying part  *
        \***********************/

        txMutexLock.lock();
        while (txMsgBufferReadLength <= 0) {
            txMsgBufferNotEmpty.wait_for(txMutexLock, std::chrono::milliseconds(100));
            if (stopConnectionFlag) {
                break;
            }
        }
        txMutexLock.unlock();
        if (stopConnectionFlag) {
            continue;
        }

        /*! Moving from 16 bits words to 32 bits registers += 2, /2, etc, are due to this conversion) */
        regs.resize(txMsgLength[txMsgBufferReadOffset]/2);
        for (txDataBufferReadIdx = 0; txDataBufferReadIdx < txMsgLength[txMsgBufferReadOffset]; txDataBufferReadIdx += 2) {
            regs[txDataBufferReadIdx/2].address = (txMsgOffsetWord[txMsgBufferReadOffset]+txDataBufferReadIdx)/2;
            regs[txDataBufferReadIdx/2].data =
                    (((uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx]) << 16) +
                    (uint32_t)txMsgBuffer[txMsgBufferReadOffset][txDataBufferReadIdx+1]; /*! Little endian */
        }

        txMsgBufferReadOffset = (txMsgBufferReadOffset+1) & TX_MSG_BUFFER_MASK;

        /******************\
         *  Sending part  *
        \******************/

        notSentTxData = true;
        while (notSentTxData && (writeTries++ < TX_MAX_WRITE_TRIES)) {
#ifdef DEBUG_TX_DATA_PRINT
            for (uint16_t regIdx = 0; regIdx < regs.size(); regIdx++) {
                fprintf(txFid, "%04d:0x%08X ", regs[regIdx].address, regs[regIdx].data);
                if (regIdx % 16 == 15) {
                    fprintf(txFid, "\n");
                }
            }
            fprintf(txFid, "\n");
            fflush(txFid);
#endif

            notSentTxData = false;
            writeTries = 0;
        }

        txMutexLock.lock();
        txMsgBufferReadLength--;
        txMsgBufferNotFull.notify_all();
        txMutexLock.unlock();
    }
}

uint32_t MessageDispatcher_4x10MHzFake::readDataFromDevice() {
    stopConnectionFlag = false;

    rxRawBufferReadOffset = 0;
    uint16_t syntheticData = 0;

    parsingFlag = true;

    /******************\
     *  Reading part  *
    \******************/

    while (!stopConnectionFlag) {
        /*! No data to receive, just sleep */
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
        syntheticData += 50;
        rxRawBufferReadLength += totalChannelsNum*RX_WORD_SIZE;

        this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageDataLoad);
    }

    if (rxMsgBufferReadLength <= 0) {
        std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
    }
    return rxRawBufferReadLength;
}
