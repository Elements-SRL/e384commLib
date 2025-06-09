#include "framemanager.h"

#include "emcrdevice.h"

FrameManager::FrameManager(MessageDispatcher * md) :
    md(md) {

    emd = static_cast <EmcrDevice *> (md);
    md->getChannelNumberFeatures(voltageChannelsNum, currentChannelsNum, gpChannelsNum);

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

void FrameManager::setMaxDataSize(uint32_t size) {
    maxDataSize = size;
}

void FrameManager::setRxWordParams(std::vector <uint16_t> rxWordOffsets, std::vector <uint16_t> rxWordLengths) {
    this->rxWordOffsets = rxWordOffsets;
    this->rxWordLengths = rxWordLengths;
}

void FrameManager::storeFrameData(uint16_t rxWordOffset) {
    if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataLoad]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdAcquisitionData), MessageDispatcher::RxMessageDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageThenCurrentDataLoad]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdAcquisitionData), MessageDispatcher::RxMessageVoltageThenCurrentDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageCurrentDataLoad]) {
        this->storeFrameDataType(type2Pc(MsgTypeIdAcquisitionData), MessageDispatcher::RxMessageCurrentDataLoad);
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
        std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
        msg.typeId = MsgDirectionDeviceToPc+MsgTypeIdAcquisitionDataLoss;
        msg.data.resize(2);
        msg.data[0] = (uint16_t)(dataLossCount & (0xFFFF));
        msg.data[1] = (uint16_t)((dataLossCount >> 16) & (0xFFFF));
        messages.push_back(msg);

        rxMutexLock.unlock();
        rxMsgBufferNotEmpty.notify_all();
    }
}

std::optional <RxMessage_t> FrameManager::getNextMessage(MsgTypeId_t messageType) {
    std::optional <RxMessage_t> ret = std::nullopt;
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    if (messages.empty()) {
#ifdef NEXT_MESSAGE_NO_WAIT
        return ErrorNoDataAvailable;
#else
        rxMsgBufferNotEmpty.wait_for(rxMutexLock, std::chrono::milliseconds(3));
        if (messages.empty()) {
            return std::nullopt;
        }
#endif
    }

    uint16_t uType = type2Pc(messageType);
    if (uType == type2Pc(MsgTypeIdInvalid)) {
        /*! Return first message regardless of type */
        ret.emplace(messages.front());
        messages.pop_front();
    }
    else if (uType == type2Pc(MsgTypeIdAcquisitionData)) {
        /*! Return first data message */
        for (auto it = messages.begin(); it != messages.end() && !ret.has_value(); ++it) {
            if (it->typeId == uType) {
                ret.emplace(*it);
                messages.erase(it);
            }
        }
    }
    else {
        /*! \todo FCON qui ci andrebbe il caso dell'header, e forse anche il caso del tail che mergiano i pacchetti dati in maniera diversa
            o forse no, perchè si può fare split e merge enllo store */

        /*! Look for type specific message */
        for (auto it = messages.begin(); it != messages.end() && !ret.has_value(); ++it) {
            if (it->typeId == uType) {
                if (it != messages.begin() && std::next(it) != messages.end()) {
                    this->mergeDataMessages(std::prev(it), std::next(it));
                }
                ret.emplace(*it);
                messages.erase(it);
            }
        }
    }

    return ret;
}

void FrameManager::purgeData() {
    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    messages.clear();
    lastDataMessage = std::nullopt;
}

uint16_t FrameManager::type2Pc(MsgTypeId_t messageType) {
    return messageType | MsgDirectionDeviceToPc;
}

void FrameManager::storeFrameDataType(uint16_t rxMsgTypeId, MessageDispatcher::RxMessageTypes_t rxMessageType) {
    uint32_t rxDataWords = rxWordLengths[rxMessageType];
    uint32_t newProtocolItemFirstIndex = 0;

    std::unique_lock <std::mutex> rxMutexLock(rxMsgMutex);
    switch (rxMessageType) {
    case MessageDispatcher::RxMessageDataLoad:
        if (rxEnabledTypesMap[rxMsgTypeId]) {
            this->enlistLastDataMessage();
        }
        lastDataMessage.emplace();
        lastDataMessage->typeId = rxMsgTypeId;
        lastDataMessage->data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            lastDataMessage->data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        break;

    case MessageDispatcher::RxMessageCurrentDataLoad: {
        /*! Data frame with only current */
        uint32_t packetsNum = rxDataWords/currentChannelsNum;
        uint32_t rxDataBufferWriteIdx = 0;
        if (rxEnabledTypesMap[rxMsgTypeId]) {
            this->enlistLastDataMessage();
        }
        lastDataMessage.emplace();
        lastDataMessage->typeId = rxMsgTypeId;
        lastDataMessage->data.resize(packetsNum*totalChannelsNum);

        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! For each packet retrieve the last recevied voltage values */
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                lastDataMessage->data[rxDataBufferWriteIdx++] = voltageDataValues[idx];
            }

            /*! Then store the new current values */
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                lastDataMessage->data[rxDataBufferWriteIdx++] = emd->popUint16FromRxRawBuffer();
            }

            // /*! Finally for each gp packet retrieve the last recevied GP values */
            // for (uint32_t idx = 0; idx < gpChannelsNum; idx++) {
            //     lastDataMessage->data[rxDataBufferWriteIdx++] = gpDataValues[idx];
            // }
        }
        break;
    }

    case MessageDispatcher::RxMessageVoltageDataLoad:
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            voltageDataValues[idx] = emd->popUint16FromRxRawBuffer();
        }
        break;

    case MessageDispatcher::RxMessageVoltageAndGpDataLoad:
        for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
            voltageDataValues[idx] = emd->popUint16FromRxRawBuffer();
        }
        for (uint32_t idx = 0; idx < gpChannelsNum; idx++) {
            gpDataValues[idx] = emd->popUint16FromRxRawBuffer();
        }
        break;

    case MessageDispatcher::RxMessageVoltageThenCurrentDataLoad: {
        /*! Data frame with only current */
        uint32_t packetsNum = rxDataWords/totalChannelsNum;
        uint32_t rxDataBufferWriteIdx = 0;
        if (rxEnabledTypesMap[rxMsgTypeId]) {
            this->enlistLastDataMessage();
        }
        lastDataMessage.emplace();
        lastDataMessage->typeId = rxMsgTypeId;
        lastDataMessage->data.resize(rxDataWords);

        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! Store the voltage values first */
            for (uint32_t idx = 0; idx < voltageChannelsNum; idx++) {
                lastDataMessage->data[rxDataBufferWriteIdx++] = emd->popUint16FromRxRawBuffer();
            }
            /*! Leave space for the current */
            rxDataBufferWriteIdx += currentChannelsNum;
        }

        rxDataBufferWriteIdx = 0;
        for (uint32_t packetIdx = 0; packetIdx < packetsNum; packetIdx++) {
            /*! Leave space for the voltage */
            rxDataBufferWriteIdx += voltageChannelsNum;
            /*! Then store the current values */
            for (uint32_t idx = 0; idx < currentChannelsNum; idx++) {
                lastDataMessage->data[rxDataBufferWriteIdx++] = emd->popUint16FromRxRawBuffer();
            }
        }
        break;
    }

    case MessageDispatcher::RxMessageDataHeader:
        if (rxWordLengths[MessageDispatcher::RxMessageDataHeader] > 4) {
            newProtocolItemFirstIndex = ((uint32_t)emd->readUint16FromRxRawBuffer(4)) + ((emd->readUint16FromRxRawBuffer(5)) << 16);
        }
        /*! \todo FCON splittare il lastDataMessage */
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        if (rxEnabledTypesMap[rxMsgTypeId]) {
            messages.push_back(msg);
        }
        break;

    case MessageDispatcher::RxMessageDataTail:
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        if (rxEnabledTypesMap[rxMsgTypeId]) {
            messages.push_back(msg);
        }
        break;

    case MessageDispatcher::RxMessageStatus:
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        if (rxEnabledTypesMap[rxMsgTypeId]) {
            messages.push_back(msg);
        }
        break;

    case MessageDispatcher::RxMessageTemperature:
        msg.typeId = rxMsgTypeId;
        msg.data.resize(rxDataWords);
        for (uint32_t rxDataBufferWriteIdx = 0; rxDataBufferWriteIdx < rxDataWords; rxDataBufferWriteIdx++) {
            msg.data[rxDataBufferWriteIdx] = emd->popUint16FromRxRawBuffer();
        }
        if (rxEnabledTypesMap[rxMsgTypeId]) {
            messages.push_back(msg);
        }
        /*! \todo FCON effettuare la chiamata al metodo dell'emd per processare i dati di temperatura */
        break;
    }

    rxMutexLock.unlock();
    rxMsgBufferNotEmpty.notify_all();
}

bool FrameManager::mergeDataMessages(std::list <RxMessage_t> ::iterator to, std::list <RxMessage_t> ::iterator from) {
    if (to->typeId != type2Pc(MsgTypeIdAcquisitionData) || from->typeId != to->typeId) {
        /*! Merge only data messages */
        return false;
    }
    if (to->data.size()+from->data.size() > maxDataSize) {
        /*! Do not merge if the final size is too large */
        return false;
    }
    to->data.insert(to->data.end(), from->data.begin(), from->data.end());
    messages.erase(from);
    return true;
}

bool FrameManager::mergeDataMessages(std::list <RxMessage_t> ::iterator to, RxMessage_t from) {
    if (to->typeId != type2Pc(MsgTypeIdAcquisitionData) || from.typeId != to->typeId) {
        /*! Merge only data messages */
        return false;
    }
    if (to->data.size()+from.data.size() > maxDataSize) {
        /*! Do not merge if the final size is too large */
        return false;
    }
    to->data.insert(to->data.end(), from.data.begin(), from.data.end());
    return true;
}

void FrameManager::enlistLastDataMessage() {
    if (!lastDataMessage.has_value()) {
        return;
    }
    if (messages.empty()) {
        messages.push_back(* lastDataMessage);
    }
    this->mergeDataMessages(std::prev(messages.end()), * lastDataMessage);
}
