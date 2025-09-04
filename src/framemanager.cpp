#include "framemanager.h"

#include "emcrdevice.h"
#include "speed_test.h"

#define ACQ_DATA_TYPE type2Pc(MsgTypeIdAcquisitionData)
#define MAX_U16_DATA_SIZE ((size_t)0x40000000)

FrameManager::FrameManager(MessageDispatcher * md) :
    md(md) {

    emd = static_cast <EmcrDevice *> (md);
    md->getChannelNumberFeatures(voltageChannelsNum, currentChannelsNum, gpChannelsNum);
    totalChannelsNum = voltageChannelsNum+currentChannelsNum+gpChannelsNum;
    ivChannelsNum = voltageChannelsNum+currentChannelsNum;

    rxEnabledTypesMap.resize(MsgDirectionDeviceToPc*2);
    rxEnabledTypesMap[type2Pc(MsgTypeIdAck)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdNack)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdPing)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdFpgaReset)] = true;
    rxEnabledTypesMap[type2Pc(MsgTypeIdLiquidJunctionComp)] = true;
    rxEnabledTypesMap[type2Pc(MsgTypeIdAcquisitionHeader)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdAcquisitionData)] = true;
    rxEnabledTypesMap[type2Pc(MsgTypeIdAcquisitionTail)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdAcquisitionSaturation)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdAcquisitionDataLoss)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdAcquisitionDataOverflow)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdInvalid)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdDeviceStatus)] = false;
    rxEnabledTypesMap[type2Pc(MsgTypeIdAcquisitionTemperature)] = true;

    /*! Allocate memory for voltage values for devices that send only data current in standard data frames */
    voltageDataValues.resize(voltageChannelsNum);
    std::fill(voltageDataValues.begin(), voltageDataValues.end(), 0);
    gpDataValues.resize(gpChannelsNum);
    std::fill(gpDataValues.begin(), gpDataValues.end(), 0);
}

void FrameManager::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    rxEnabledTypesMap[type2Pc(messageType)] = flag;
}

bool FrameManager::isRxMessageTypeEnabled(MsgTypeId_t messageType) {
    return rxEnabledTypesMap[type2Pc(messageType)];
}

void FrameManager::setMaxDataMessageSize(uint32_t size) {
    maxDataMessageSize = size;
}

uint32_t FrameManager::getMaxDataMessageSize() {
    return maxDataMessageSize;
}

void FrameManager::setRxWordParams(std::vector <uint16_t> rxWordOffsets, std::vector <uint16_t> rxWordLengths) {
    this->rxWordOffsets = rxWordOffsets;
    this->rxWordLengths = rxWordLengths;
}

void FrameManager::setCurrentBlockLength(uint16_t blockLen) {
    this->blockLen = blockLen;
}

void FrameManager::storeFrameData(uint16_t rxWordOffset) {
    if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataLoad]) {
        this->storeFrameDataType(ACQ_DATA_TYPE, MessageDispatcher::RxMessageDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageThenCurrentDataLoad]) {
        this->storeFrameDataType(ACQ_DATA_TYPE, MessageDispatcher::RxMessageVoltageThenCurrentDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageCurrentDataLoad]) {
        this->storeFrameDataType(ACQ_DATA_TYPE, MessageDispatcher::RxMessageCurrentDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageCurrentBlocksDataLoad]) {
        this->storeFrameDataType(ACQ_DATA_TYPE, MessageDispatcher::RxMessageCurrentBlocksDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageDataLoad]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdInvalid), MessageDispatcher::RxMessageVoltageDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageAndGpDataLoad]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdInvalid), MessageDispatcher::RxMessageVoltageAndGpDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataHeader]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdAcquisitionHeader), MessageDispatcher::RxMessageDataHeader);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataTail]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdAcquisitionTail), MessageDispatcher::RxMessageDataTail);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageStatus]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdDeviceStatus), MessageDispatcher::RxMessageStatus);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageTemperature]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdAcquisitionTemperature), MessageDispatcher::RxMessageTemperature);
    }
}

void FrameManager::storeFrameDataLoss(int32_t dataLossCount) {
    if (dataLossCount > 0 && this->isRxMessageTypeEnabled(MsgTypeIdAcquisitionDataLoss)) {
        msg.typeId = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss;
        msg.data.resize(2);
        msg.data[0] = (uint16_t)(dataLossCount & (0xFFFF));
        msg.data[1] = (uint16_t)((dataLossCount >> 16) & (0xFFFF));
        std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
        this->pushMessage(msg);

        rxMutexLock.unlock();
        rxMsgBufferNotEmpty.notify_all();
    }
}

RxMessage_t FrameManager::getNextMessage(MsgTypeId_t messageType) {
    RxMessage_t ret;
    ret.typeId = type2Pc(MsgTypeIdInvalid);
#ifdef SPT_DISABLE_GET_NEXT_MESSAGE
    return ret;
#endif
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    if (messages.empty()) {
#ifdef NEXT_MESSAGE_NO_WAIT
        return ErrorNoDataAvailable;
#else
        rxMsgBufferNotEmpty.wait_for(rxMutexLock, std::chrono::milliseconds(3));
        if (messages.empty()) {
            return ret;
        }
#endif
    }

    uint16_t uType = type2Pc(messageType);
    if (uType == type2Pc(MsgTypeIdInvalid)) {
        /*! Return first message regardless of type */
        ret = messages.front();
#ifndef SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
        messages.pop_front();
        listSize -= ret.data.size();
#endif
        return ret;
    }
    if (uType == ACQ_DATA_TYPE) {
        /*! Return first data message */
        for (auto it = messages.begin(); it != messages.end(); ++it) {
            if (it->typeId == uType) {
                ret = *it;
#ifndef SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
                messages.erase(it);
                listSize -= ret.data.size();
#endif
                return ret;
            }
        }
        return ret;
    }
    for (auto it = messages.begin(); it != messages.end(); ++it) {
        /*! Look for type specific message */
        if (it->typeId == uType) {
            ret = *it;
#ifndef SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
            if (it != messages.begin() && std::next(it) != messages.end()) {
                this->mergeDataMessages(std::prev(it), std::next(it));
            }
            messages.erase(it);
            listSize -= ret.data.size();
#endif
            return ret;
        }
    }
    return ret;
}

void FrameManager::purgeData() {
    purgeRequest = true;
}

uint16_t FrameManager::type2Pc(MsgTypeId_t messageType) {
    return messageType | MsgDirectionDeviceToPc;
}

void FrameManager::storeFrameDataType(uint16_t rxMsgTypeId, MessageDispatcher::RxMessageTypes_t rxMessageType) {
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    if (purgeRequest) {
        messages.clear();
        listSize = 0;
        lastDataMessageAvailable = false;
        purgeRequest = false;
    }
    uint32_t rxDataWords = rxWordLengths[rxMessageType];
    uint32_t newProtocolItemFirstIndex = 0;

    switch (rxMessageType) {
    case MessageDispatcher::RxMessageDataLoad:
        this->pushLastDataMessage();
        lastDataMessage.typeId = rxMsgTypeId;
        lastDataMessage.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            lastDataMessage.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        lastDataMessageAvailable = true;
        break;

    case MessageDispatcher::RxMessageCurrentDataLoad: {
        /*! Data frame with only current */
        uint32_t packetsNum = rxDataWords/currentChannelsNum;
        uint32_t rxDataBufferWriteIdx = 0;
        this->pushLastDataMessage();
        lastDataMessage.typeId = rxMsgTypeId;
        lastDataMessage.data.resize(packetsNum*(ivChannelsNum/*+gpChannelsNum*/));

        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! For each packet retrieve the last recevied voltage values */
            for (int idx = 0; idx < voltageChannelsNum; idx++) {
                lastDataMessage.data[rxDataBufferWriteIdx++] = voltageDataValues[idx];
            }

            /*! Then store the new current values */
            for (int idx = 0; idx < currentChannelsNum; idx++) {
                lastDataMessage.data[rxDataBufferWriteIdx++] = emd->popUint16FromRxRawBuffer();
            }

            // /*! Finally for each gp packet retrieve the last recevied GP values */
            // for (uint32_t idx = 0; idx < gpChannelsNum; idx++) {
            //     lastDataMessage.data[rxDataBufferWriteIdx++] = gpDataValues[idx];
            // }
        }
        lastDataMessageAvailable = true;
        break;
    }

    case MessageDispatcher::RxMessageCurrentBlocksDataLoad: {
        /*! Data frame with only current */
        uint32_t packetsNum = rxDataWords/currentChannelsNum;
        uint32_t blocksNum = packetsNum/blockLen;
        uint32_t blockSize = blockLen*(ivChannelsNum/*+gpChannelsNum*/);
        uint32_t rxDataBufferWriteIdx = 0;
        this->pushLastDataMessage();
        lastDataMessage.typeId = rxMsgTypeId;
        lastDataMessage.data.resize(packetsNum*(ivChannelsNum/*+gpChannelsNum*/));

        for (uint32_t blockIdx = 0; blockIdx < blocksNum; blockIdx++) {
            for (uint32_t packetIdx = 0; packetIdx < blockLen; packetIdx++) {
                /*! For each packet retrieve the last received voltage values */
                for (int idx = 0; idx < voltageChannelsNum; idx++) {
                    lastDataMessage.data[rxDataBufferWriteIdx+packetIdx*ivChannelsNum+idx] = voltageDataValues[idx];
                }

                /*! Then store the new current values */
                for (int idx = 0; idx < currentChannelsNum; idx++) {
                    lastDataMessage.data[rxDataBufferWriteIdx+voltageChannelsNum+packetIdx*ivChannelsNum+idx] = emd->popUint16FromRxRawBuffer();
                }

                // /*! Finally for each gp packet retrieve the last received GP values */
                // for (uint32_t idx = 0; idx < gpChannelsNum; idx++) {
                //     lastDataMessage.data[rxDataBufferWriteIdx+packetIdx*ivChannelsNum+idx] = gpDataValues[idx];
                // }
            }
            rxDataBufferWriteIdx += blockSize;
        }
        lastDataMessageAvailable = true;
        break;
    }

    case MessageDispatcher::RxMessageVoltageDataLoad:
        for (int idx = 0; idx < voltageChannelsNum; idx++) {
            voltageDataValues[idx] = emd->popUint16FromRxRawBuffer();
        }
        break;

    case MessageDispatcher::RxMessageVoltageAndGpDataLoad:
        for (int idx = 0; idx < voltageChannelsNum; idx++) {
            voltageDataValues[idx] = emd->popUint16FromRxRawBuffer();
        }
        for (int idx = 0; idx < gpChannelsNum; idx++) {
            gpDataValues[idx] = emd->popUint16FromRxRawBuffer();
        }
        break;

    case MessageDispatcher::RxMessageVoltageThenCurrentDataLoad: {
        /*! Data frame with only current */
        uint32_t packetsNum = rxDataWords/totalChannelsNum;
        uint32_t rxDataBufferWriteIdx = 0;
        this->pushLastDataMessage();
        lastDataMessage.typeId = rxMsgTypeId;
        lastDataMessage.data.resize(rxDataWords);

        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! Store the voltage values first */
            for (int idx = 0; idx < voltageChannelsNum; idx++) {
                lastDataMessage.data[rxDataBufferWriteIdx++] = emd->popUint16FromRxRawBuffer();
            }
            /*! Leave space for the current */
            rxDataBufferWriteIdx += currentChannelsNum;
        }

        rxDataBufferWriteIdx = 0;
        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! Leave space for the voltage */
            rxDataBufferWriteIdx += voltageChannelsNum;
            /*! Then store the current values */
            for (int idx = 0; idx < currentChannelsNum; idx++) {
                lastDataMessage.data[rxDataBufferWriteIdx++] = emd->popUint16FromRxRawBuffer();
            }
        }
        lastDataMessageAvailable = true;
        break;
    }

    case MessageDispatcher::RxMessageDataHeader:
        if (rxWordLengths[MessageDispatcher::RxMessageDataHeader] > 4) {
            newProtocolItemFirstIndex = ((uint32_t)emd->readUint16FromRxRawBuffer(4)) + ((emd->readUint16FromRxRawBuffer(5)) << 16);
        }
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        this->pushHeaderMessage(msg, newProtocolItemFirstIndex);
        break;

    case MessageDispatcher::RxMessageDataTail:
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        this->pushMessage(msg);
        break;

    case MessageDispatcher::RxMessageStatus:
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        this->pushMessage(msg);
        break;

    case MessageDispatcher::RxMessageTemperature:
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        this->pushMessage(msg);
        emd->processTemperatureData(msg);
        break;
    }

    rxMutexLock.unlock();
    rxMsgBufferNotEmpty.notify_all();
    std::this_thread::yield();
}

bool FrameManager::mergeDataMessages(std::list <RxMessage_t> ::iterator to, std::list <RxMessage_t> ::iterator from) {
    if (to->typeId != ACQ_DATA_TYPE || from->typeId != to->typeId) {
        /*! Merge only data messages */
        return false;
    }
    to->mergeable = true;
    return true;
}

bool FrameManager::pushMessage(RxMessage_t msg) {
    if (!isPushable(msg)) {
        return false;
    }
    messages.push_back(msg);
    listSize += msg.data.size();
    return true;
}

bool FrameManager::pushHeaderMessage(RxMessage_t msg, uint32_t newProtocolItemFirstIndex) {
    if (!isPushable(msg)
        || (!messages.empty()
            && * std::prev(messages.end()) == msg)) {
        /*! Do not push if the last message is a header as well */
        return false;
    }
    if (!lastDataMessageAvailable) {
        /*! No last data message available, just push the header */
        messages.push_back(msg);
        listSize += msg.data.size();
        return true;
    }
    if (lastDataMessage.data.size() <= newProtocolItemFirstIndex) {
        /*! Last data message has less samples than required by the header, push it entirely and set it not available */
        this->pushLastDataMessage();
        lastDataMessageAvailable = false;
        messages.push_back(msg);
        listSize += msg.data.size();
        return true;
    }
    if (newProtocolItemFirstIndex > 0) {
        /*! Last data message has more samples than required by the header, split it and push the first chunk */
        this->pushDataMessage(this->splitLastDataMessage(newProtocolItemFirstIndex));
        messages.push_back(msg);
        listSize += msg.data.size();
        return true;
    }
    /*! The header requires 0 samples, do not push the last data message */
    messages.push_back(msg);
    listSize += msg.data.size();
    return true;
}

bool FrameManager::pushDataMessage(RxMessage_t msg) {
    if (!isPushable(msg)) {
        return false;
    }
    messages.push_back(msg);
    listSize += msg.data.size();
#ifdef SPT_LOG_PARSE_DATA
    speedTestLog(SpeedTestParseData, msg.data.size()*2);
#endif
    if (messages.size() > 1) {
        /*! If the message is not the only one, try to merge it with the last message in the list */
        this->mergeDataMessages(std::prev(messages.end()), messages.end());
    }
    return true;
}

bool FrameManager::pushLastDataMessage() {
    if (!lastDataMessageAvailable) {
        /*! Do not push if the last data message is not available
            \note avoiding the check if the message is pushable, since it is checked in the pushDataMessage function
            If something changes in the logic the check must be reintroduced here */
        return false;
    }
    return this->pushDataMessage(lastDataMessage);
}

RxMessage_t FrameManager::splitLastDataMessage(uint32_t newProtocolItemFirstIndex) {
    RxMessage_t firstChunk;
    firstChunk.typeId = ACQ_DATA_TYPE;
    firstChunk.data.insert(firstChunk.data.end(), lastDataMessage.data.begin(), lastDataMessage.data.begin()+newProtocolItemFirstIndex);
    lastDataMessage.data.erase(lastDataMessage.data.begin(), lastDataMessage.data.begin() + newProtocolItemFirstIndex);
    return firstChunk;
}

bool FrameManager::isPushable(RxMessage_t msg) {
    /*! A message is not pushable if its type is disabled or if pushing it would exceed the list max size */
    return rxEnabledTypesMap[msg.typeId] && (listSize + msg.data.size() <= MAX_U16_DATA_SIZE);
}
