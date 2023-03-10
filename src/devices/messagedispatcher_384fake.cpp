#include "messagedispatcher_384fake.h"

MessageDispatcher_384Fake::MessageDispatcher_384Fake(string id) :
    MessageDispatcher_384NanoPores_V01(id) {

}

MessageDispatcher_384Fake::~MessageDispatcher_384Fake() {

}

ErrorCodes_t MessageDispatcher_384Fake::connect() {
    this->initializeBuffers();
    return MessageDispatcher::connect();
}

ErrorCodes_t MessageDispatcher_384Fake::disconnect() {
    MessageDispatcher::disconnect();
    return this->deinitializeBuffers();
}

void MessageDispatcher_384Fake::sendCommandsToDevice() {
    int writeTries = 0;
    okTRegisterEntries regs;
    regs.reserve(txMaxRegs);

    /*! Variables used to access the tx msg buffer */
    uint32_t txMsgBufferReadOffset = 0; /*!< Offset of the part of buffer to be processed */

    /*! Variables used to access the tx data buffer */
    uint32_t txDataBufferReadIdx;

    bool notSentTxData;

    unique_lock <mutex> txMutexLock (txMutex);
    txMutexLock.unlock();

    while (!stopConnectionFlag) {

        /***********************\
         *  Data copying part  *
        \***********************/

        txMutexLock.lock();
        while (txMsgBufferReadLength <= 0) {
            txMsgBufferNotEmpty.wait_for(txMutexLock, chrono::milliseconds(100));
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
            for (uint16_t regIdx = 0; regIdx < regs.size(); regIdx++) {
                fprintf(fid, "%04d:0x%08X ", regs[regIdx].address, regs[regIdx].data);
                if (regIdx % 16 == 15) {
                    fprintf(fid, "\n");
                }
            }
            fprintf(fid, "\n");
            fflush(fid);

            notSentTxData = false;
            writeTries = 0;
        }

        txMutexLock.lock();
        txMsgBufferReadLength--;
        txMsgBufferNotFull.notify_all();
        txMutexLock.unlock();
    }
}

void MessageDispatcher_384Fake::readDataFromDevice() {
    stopConnectionFlag = false;

    rxRawBufferReadOffset = 0;
    uint16_t syntheticData = 0;

    parsingFlag = true;

    /******************\
     *  Reading part  *
    \******************/

    while (!stopConnectionFlag) {
        /*! No data to receive, just sleep */
        this_thread::sleep_for(chrono::milliseconds(10));

        unsigned char d1 = (syntheticData & 0xFF00) >> 8;
        unsigned char d2 = syntheticData & 0x00FF;
        syntheticData += 50;

        for (uint32_t idx = 0; idx < totalChannelsNum; idx++) {
            rxRawBuffer[rxRawBufferWriteOffset] = d1;
            rxRawBuffer[rxRawBufferWriteOffset+1] = d2;
            rxRawBufferWriteOffset = (rxRawBufferWriteOffset+RX_WORD_SIZE) & OKY_RX_BUFFER_MASK;
        }
        rxRawBufferReadLength += totalChannelsNum*RX_WORD_SIZE;

        this->storeFrameData(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, RxMessageDataLoad);
    }

    if (rxMsgBufferReadLength <= 0) {
        unique_lock <mutex> rxMutexLock(rxMutex);
        parsingFlag = false;
        rxMsgBufferReadLength++;
        rxMsgBufferNotEmpty.notify_all();
    }
}
