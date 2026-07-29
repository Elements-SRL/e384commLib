#ifndef FRAMEMANAGER_H
#define FRAMEMANAGER_H

#include <list>

#include "e384commlib_global.h"
#include "messagedispatcher.h"

typedef struct RxMessage {
    uint16_t typeId = MsgTypeIdInvalid;
    std::vector <uint16_t> data;
    bool mergeable = false;
    bool operator == (const RxMessage& other) const {
        return typeId == other.typeId && data == other.data;
    }
} RxMessage_t;

typedef struct RxDeviceStatus {
    RxMessage_t lastTemperatureMessage;
    RxMessage_t lastOnTimeMessage;
    RxMessage_t lastSyncStatusMessage;
    RxMessage_t lastDataLossCountMessage;
    RxMessage_t lastCalEepromMessage;
    RxMessage_t lastDataTailMessage;
    std::list <RxMessage_t> lastSpiDataLoadMessages;
} RxDeviceStatus_t;

class EmcrDevice;

class FrameManager {
public:
    FrameManager(MessageDispatcher * md);

    void enableRxMessageType(MsgTypeId_t messageType, bool flag);
    bool isRxMessageTypeEnabled(MsgTypeId_t messageType);

    void setMaxDataMessageSize(uint32_t size);
    uint32_t getMaxDataMessageSize();
    void setRxWordParams(std::vector <uint16_t> rxWordOffsets, std::vector <uint16_t> rxWordLengths);
    void setCurrentBlockLength(uint16_t blockLen);
    void storeFrameData(uint16_t rxWordOffset);
    void storeFrameDataLoss(int32_t dataLossCount);
    RxMessage_t getNextMessage(MsgTypeId_t messageType = MsgTypeIdInvalid);
    RxMessage_t getStoredMessage(MsgTypeId_t messageType);
    void purgeData();

protected:
    static uint16_t typeNoDir(MsgTypeId_t messageType);
    void storeFrameDataType(uint16_t rxMsgTypeId, MessageDispatcher::RxMessageTypes_t rxMessageType);
    template <typename it>
    bool mergeDataMessages(it to, it from) {
        if (to->typeId != MsgTypeIdAcquisitionData || from->typeId != to->typeId) {
            /*! Merge only data messages */
            return false;
        }
        to->mergeable = true;
        return true;
    }
    bool pushMessage(RxMessage_t msg);
    bool storeMessage(RxMessage_t msg);
    bool pushHeaderMessage(RxMessage_t msg, uint32_t newProtocolItemFirstIndex);
    bool pushDataMessage(RxMessage_t msg);
    bool pushLastDataMessage();
    RxMessage_t splitLastDataMessage(uint32_t newProtocolItemFirstIndex);
    bool isPushable(RxMessage_t msg);

    uint32_t maxDataMessageSize = -1;
    std::list <RxMessage_t> messages; /*! \todo FCON Per gestire meglio la quantità di dati salvati si potrebbe estendere la std::list <RxMessage_t> per gestire automaticamente la list size ogni volta che viene chiamata un'operazione
                                            che la modificherebbe, come push_back, erase, clear, insert, pop_front, etc */
    size_t listSize = 0;
    RxMessage_t lastDataMessage;
    bool lastDataMessageAvailable = false;

    RxDeviceStatus rxDeviceStatus; /*! Collects the last message (or several messages, e.g. SpiDataLoad) if they are discarded from the main getNextMessageFlow */

    MessageDispatcher * md = nullptr;
    EmcrDevice * emd = nullptr;
    int currentChannelsNum;
    int voltageChannelsNum;
    int gpChannelsNum;
    int totalChannelsNum;
    int ivChannelsNum;
    int blockLen = 1;
    std::vector <uint16_t> rxWordOffsets;
    std::vector <uint16_t> rxWordLengths;
    std::vector <uint16_t> voltageDataValues; /*! Store voltage data when current data and voltage data are not sent together in a single packet */
    std::vector <uint16_t> gpDataValues; /*! Store GP data when current data and GP data are not sent together in a single packet */
    std::vector <bool> rxEnabledTypesMap; /*! key is any message type ID, value tells if the message should be returned by the getNextMessage method */
    bool purgeRequest = false;

    mutable std::mutex rxMsgMutex; /*! Protects messages and listSize */
    mutable std::mutex rxStatusMutex; /*! Protects rxDeviceStatus */
    std::condition_variable rxMsgBufferNotEmpty;
};

#endif // FRAMEMANAGER_H
