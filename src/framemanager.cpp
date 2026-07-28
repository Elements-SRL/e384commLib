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

    rxEnabledTypesMap.resize(MsgDirectionDeviceToPc);
    rxEnabledTypesMap[MsgTypeIdAck] = false;
    rxEnabledTypesMap[MsgTypeIdNack] = false;
    rxEnabledTypesMap[MsgTypeIdPing] = false;
    rxEnabledTypesMap[MsgTypeIdFpgaReset] = true;
    rxEnabledTypesMap[MsgTypeIdLiquidJunctionComp] = true;
    rxEnabledTypesMap[MsgTypeIdAcquisitionHeader] = false;
    rxEnabledTypesMap[MsgTypeIdAcquisitionData] = true;
    rxEnabledTypesMap[MsgTypeIdAcquisitionTail] = false;
    rxEnabledTypesMap[MsgTypeIdAcquisitionSaturation] = false;
    rxEnabledTypesMap[MsgTypeIdAcquisitionDataLoss] = false;
    rxEnabledTypesMap[MsgTypeIdAcquisitionDataOverflow] = false;
    rxEnabledTypesMap[MsgTypeIdAcquisitionSyncStatus] = true;
    rxEnabledTypesMap[MsgTypeIdSpiDataLoad] = true;
    rxEnabledTypesMap[MsgTypeIdCalEeprom] = true;
    rxEnabledTypesMap[MsgTypeIdInvalid] = false;
    rxEnabledTypesMap[MsgTypeIdDeviceStatus] = false;
    rxEnabledTypesMap[MsgTypeIdTemperature] = true;
    rxEnabledTypesMap[MsgTypeIdOnTime] = false;

    /*! Allocate memory for voltage values for devices that send only data current in standard data frames */
    voltageDataValues.resize(voltageChannelsNum);
    std::fill(voltageDataValues.begin(), voltageDataValues.end(), 0);
    gpDataValues.resize(gpChannelsNum);
    std::fill(gpDataValues.begin(), gpDataValues.end(), 0);
}

void FrameManager::enableRxMessageType(MsgTypeId_t messageType, bool flag) {
    rxEnabledTypesMap[typeNoDir(messageType)] = flag;
}

bool FrameManager::isRxMessageTypeEnabled(MsgTypeId_t messageType) {
    return rxEnabledTypesMap[typeNoDir(messageType)];
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
        this->storeFrameDataType(MsgTypeIdAcquisitionData, MessageDispatcher::RxMessageDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageThenCurrentDataLoad]) {
        this->storeFrameDataType(MsgTypeIdAcquisitionData, MessageDispatcher::RxMessageVoltageThenCurrentDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageCurrentDataLoad]) {
        this->storeFrameDataType(MsgTypeIdAcquisitionData, MessageDispatcher::RxMessageCurrentDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageCurrentBlocksDataLoad]) {
        this->storeFrameDataType(MsgTypeIdAcquisitionData, MessageDispatcher::RxMessageCurrentBlocksDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageDataLoad]) {
        this->storeFrameDataType(MsgTypeIdInvalid, MessageDispatcher::RxMessageVoltageDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageAndGpDataLoad]) {
        this->storeFrameDataType(MsgTypeIdInvalid, MessageDispatcher::RxMessageVoltageAndGpDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataHeader]) {
        this->storeFrameDataType(MsgTypeIdAcquisitionHeader, MessageDispatcher::RxMessageDataHeader);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataTail]) {
        this->storeFrameDataType(MsgTypeIdAcquisitionTail, MessageDispatcher::RxMessageDataTail);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageStatus]) {
        this->storeFrameDataType(MsgTypeIdDeviceStatus, MessageDispatcher::RxMessageStatus);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageTemperature]) {
        this->storeFrameDataType(MsgTypeIdTemperature, MessageDispatcher::RxMessageTemperature);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageOnTime]) {
        this->storeFrameDataType(MsgTypeIdOnTime, MessageDispatcher::RxMessageOnTime);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageSyncStatus]) {
        this->storeFrameDataType(MsgTypeIdAcquisitionSyncStatus, MessageDispatcher::RxMessageSyncStatus);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDoubleSyncStatus]) {
        this->storeFrameDataType(MsgTypeIdAcquisitionSyncStatus, MessageDispatcher::RxMessageDoubleSyncStatus);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageSpiDataLoad]) {
        this->storeFrameDataType(MsgTypeIdSpiDataLoad, MessageDispatcher::RxMessageSpiDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageCalEeepromDataLoad]) {
        this->storeFrameDataType(MsgTypeIdCalEeprom, MessageDispatcher::RxMessageCalEeepromDataLoad);
    }
}

void FrameManager::storeFrameDataLoss(int32_t dataLossCount) {
    if (dataLossCount > 0 && this->isRxMessageTypeEnabled(MsgTypeIdAcquisitionDataLoss)) {
        RxMessage_t msg;
        msg.typeId = MsgTypeIdAcquisitionDataLoss;
        msg.data.resize(2);
        msg.data[0] = (uint16_t)(dataLossCount & (0xFFFF));
        msg.data[1] = (uint16_t)((dataLossCount >> 16) & (0xFFFF));
        this->pushMessage(msg);

        rxMsgBufferNotEmpty.notify_all();
    }
}

RxMessage_t FrameManager::getNextMessage(MsgTypeId_t messageType) {
    RxMessage_t ret;
    ret.typeId = MsgTypeIdInvalid;
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

    uint16_t uType = messageType;
    if (uType == MsgTypeIdInvalid) {
        /*! Return first message regardless of type */
        ret = messages.front();
#ifndef SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
        messages.pop_front();
        listSize -= ret.data.size();
#endif
        return ret;
    }
    size_t totalDeletion = 0;
    for (auto it = messages.begin(); it != messages.end(); ++it) {
        /*! Look for type specific message */
        if (it->typeId == uType) {
            ret = *it;
#ifndef SPT_DISABLE_PARSE_DATA_AFTER_A_WHILE
            messages.erase(messages.begin(), std::next(it));
            listSize -= ret.data.size()+totalDeletion;
#endif
            return ret;
        }
        else {
            /*! Delete all other messages found along the way */
            totalDeletion += (*it).data.size();
        }
    }
    /*! No desired message found, clear the list */
    messages.clear();
    listSize = 0;
    return ret;
}

RxMessage_t FrameManager::getStoredMessage(MsgTypeId_t messageType) {
    std::unique_lock <std::mutex> rxMutexLock(rxStatusMutex);
    RxMessage_t msg;
    switch (messageType) {
    case MsgTypeIdTemperature:
        return rxDeviceStatus.lastTemperatureMessage;

    case MsgTypeIdOnTime:
        return rxDeviceStatus.lastOnTimeMessage;

    case MsgTypeIdAcquisitionSyncStatus:
        return rxDeviceStatus.lastSyncStatusMessage;

    case MsgTypeIdAcquisitionDataLoss:
        return rxDeviceStatus.lastDataLossCountMessage;

    case MsgTypeIdCalEeprom:
        return rxDeviceStatus.lastCalEepromMessage;

    case MsgTypeIdAcquisitionTail:
        return rxDeviceStatus.lastDataTailMessage;

    case MsgTypeIdSpiDataLoad:
        if (rxDeviceStatus.lastSpiDataLoadMessages.empty()) {
            return msg;
        }
        msg = rxDeviceStatus.lastSpiDataLoadMessages.front();
        rxDeviceStatus.lastSpiDataLoadMessages.pop_front();
        return msg;
    }
    return msg;
}

void FrameManager::purgeData() {
    purgeRequest = true;
}

uint16_t FrameManager::typeNoDir(MsgTypeId_t messageType) {
    return messageType & ~MsgDirectionDeviceToPc;
}

void FrameManager::storeFrameDataType(uint16_t rxMsgTypeId, MessageDispatcher::RxMessageTypes_t rxMessageType) {
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    if (purgeRequest) {
        messages.clear();
        listSize = 0;
        lastDataMessageAvailable = false;
        purgeRequest = false;
    }
    rxMutexLock.unlock();

    uint32_t rxDataWords = rxWordLengths[rxMessageType];
    uint32_t newProtocolItemFirstIndex = 0;
    RxMessage_t msg;

    switch (rxMessageType) {
    case MessageDispatcher::RxMessageDataLoad:
        this->pushLastDataMessage();
        lastDataMessage.typeId = rxMsgTypeId;
        lastDataMessage.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            lastDataMessage.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        lastDataMessageAvailable = true;
        emd->processLiquidJunctionData(lastDataMessage);
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
        emd->processLiquidJunctionData(lastDataMessage);
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
            /*! For each packet retrieve the last received voltage values */
            for (int idx = 0; idx < voltageChannelsNum; idx++) {
                for (uint32_t packetIdx = 0; packetIdx < blockLen; packetIdx++) {
                    lastDataMessage.data[rxDataBufferWriteIdx+packetIdx*ivChannelsNum+idx] = voltageDataValues[idx];
                }
            }

            /*! Then store the new current values */
            for (int idx = 0; idx < currentChannelsNum; idx++) {
                for (uint32_t packetIdx = 0; packetIdx < blockLen; packetIdx++) {
                    lastDataMessage.data[rxDataBufferWriteIdx+voltageChannelsNum+packetIdx*ivChannelsNum+idx] = emd->popUint16FromRxRawBuffer();
                }
            }

            // /*! Finally for each gp packet retrieve the last received GP values */
            // for (int idx = 0; idx < currentChannelsNum; idx++) {
            //     for (uint32_t idx = 0; idx < gpChannelsNum; idx++) {
            //         lastDataMessage.data[rxDataBufferWriteIdx+packetIdx*ivChannelsNum+idx] = gpDataValues[idx];
            //     }
            // }
            rxDataBufferWriteIdx += blockSize;
        }
        lastDataMessageAvailable = true;
        emd->processLiquidJunctionData(lastDataMessage);
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
        emd->processLiquidJunctionData(lastDataMessage);
        break;
    }

    case MessageDispatcher::RxMessageDataHeader:
        if (rxWordLengths[MessageDispatcher::RxMessageDataHeader] > 4) {
            newProtocolItemFirstIndex = (((uint32_t)emd->readUint16FromRxRawBuffer(4*2)) + ((emd->readUint16FromRxRawBuffer(5*2)) << 16))*totalChannelsNum;
        }
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        this->pushHeaderMessage(msg, newProtocolItemFirstIndex);
        break;

    case MessageDispatcher::RxMessageDataTail:
    case MessageDispatcher::RxMessageStatus:
    case MessageDispatcher::RxMessageOnTime:
    case MessageDispatcher::RxMessageSyncStatus:
    case MessageDispatcher::RxMessageSpiDataLoad:
    case MessageDispatcher::RxMessageCalEeepromDataLoad:
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        this->pushMessage(msg);
        break;

    case MessageDispatcher::RxMessageDoubleSyncStatus:{
        msg.typeId = rxMsgTypeId;
        int rxDataWords2 = rxDataWords/2;
        msg.data.resize(rxDataWords2);
        /*! The order of the bits is inversed with respect to other similar messages, the code below puts them in the same format */
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords2; rxDataBufferWriteIdx++) {
            uint16_t dat = emd->popUint16FromRxRawBuffer();
            dat = (dat | (dat << 1)) & 0xAAAA;
            dat = (dat | (dat << 1)) & 0xCCCC;
            dat = (dat | (dat << 2)) & 0xF0F0;
            dat = (dat | (dat << 4)) & 0xFF00;
            msg.data[rxDataBufferWriteIdx] = dat;
            dat = emd->popUint16FromRxRawBuffer();
            dat = (dat | (dat >> 1)) & 0x5555;
            dat = (dat | (dat >> 1)) & 0x3333;
            dat = (dat | (dat >> 2)) & 0x0F0F;
            dat = (dat | (dat >> 4)) & 0x00FF;
            msg.data[rxDataBufferWriteIdx] |= dat;
        }
        this->pushMessage(msg);
        break;
    }

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

    rxMsgBufferNotEmpty.notify_all();
    std::this_thread::yield();
}

bool FrameManager::pushMessage(RxMessage_t msg) {
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    if (!isPushable(msg)) {
        this->storeMessage(msg);
        return false;
    }
    messages.push_back(msg);
    listSize += msg.data.size();
    return true;
}

bool FrameManager::storeMessage(RxMessage_t msg) {
    std::unique_lock <std::mutex> rxMutexLock(rxStatusMutex);
    switch (msg.typeId) {
    case MsgTypeIdTemperature:
        rxDeviceStatus.lastTemperatureMessage = msg;
        break;

    case MsgTypeIdOnTime:
        rxDeviceStatus.lastOnTimeMessage = msg;
        break;

    case MsgTypeIdAcquisitionSyncStatus:
        rxDeviceStatus.lastSyncStatusMessage = msg;
        break;

    case MsgTypeIdAcquisitionDataLoss:
        rxDeviceStatus.lastDataLossCountMessage = msg;
        break;

    case MsgTypeIdCalEeprom:
        rxDeviceStatus.lastCalEepromMessage = msg;
        break;

    case MsgTypeIdAcquisitionTail:
        rxDeviceStatus.lastDataTailMessage = msg;
        break;

    case MsgTypeIdSpiDataLoad:
        rxDeviceStatus.lastSpiDataLoadMessages.push_back(msg);
        break;
    }
    return true;
}

bool FrameManager::pushHeaderMessage(RxMessage_t msg, uint32_t newProtocolItemFirstIndex) {
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    if (!isPushable(msg)
        || (!messages.empty()
            && * messages.rbegin() == msg)) {
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
        rxMutexLock.unlock();
        this->pushLastDataMessage();
        lastDataMessageAvailable = false;
        rxMutexLock.lock();
        messages.push_back(msg);
        listSize += msg.data.size();
        return true;
    }
    if (newProtocolItemFirstIndex > 0) {
        /*! Last data message has more samples than required by the header, split it and push the first chunk */
        rxMutexLock.unlock();
        this->pushDataMessage(this->splitLastDataMessage(newProtocolItemFirstIndex));
        rxMutexLock.lock();
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
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    if (!isPushable(msg)) {
        return false;
    }
#ifndef SPT_DISABLE_GET_NEXT_MESSAGE
    messages.push_back(msg);
    listSize += msg.data.size();
#endif
    if (messages.size() > 1) {
        /*! If the message is not the only one, try to merge it with the last message in the list */
        this->mergeDataMessages(++messages.rbegin(), messages.rbegin());
    }
#ifdef SPT_LOG_PARSE_DATA
    rxMutexLock.unlock();
    speedTestLog(SpeedTestParseData, msg.data.size()*2);
#endif
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
    firstChunk.typeId = MsgTypeIdAcquisitionData;
    firstChunk.data.insert(firstChunk.data.end(), lastDataMessage.data.begin(), lastDataMessage.data.begin()+newProtocolItemFirstIndex);
    lastDataMessage.data.erase(lastDataMessage.data.begin(), lastDataMessage.data.begin() + newProtocolItemFirstIndex);
    return firstChunk;
}

bool FrameManager::isPushable(RxMessage_t msg) {
    /*! A message is not pushable if its type is disabled or if pushing it would exceed the list max size */
    return rxEnabledTypesMap[msg.typeId] && (listSize + msg.data.size() <= MAX_U16_DATA_SIZE);
}
