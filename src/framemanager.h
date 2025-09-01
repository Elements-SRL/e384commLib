#ifndef FRAMEMANAGER_H
#define FRAMEMANAGER_H

#include <list>

#include "e384commlib_global.h"
#include "messagedispatcher.h"

typedef struct RxMessage {
    uint16_t typeId;
    std::vector <uint16_t> data;
    bool operator == (const RxMessage& other) const {
        return typeId == other.typeId && data == other.data;
    }
} RxMessage_t;

class EmcrDevice;

class FrameManager {
public:
    FrameManager(MessageDispatcher * md);

    void enableRxMessageType(MsgTypeId_t messageType, bool flag);
    bool isRxMessageTypeEnabled(MsgTypeId_t messageType);

    void setMaxDataSize(uint32_t size);
    void setRxWordParams(std::vector <uint16_t> rxWordOffsets, std::vector <uint16_t> rxWordLengths);
    void setCurrentBlockLength(uint16_t blockLen);
    void storeFrameData(uint16_t rxWordOffset);
    void storeFrameDataLoss(int32_t dataLossCount);
    RxMessage_t getNextMessage(MsgTypeId_t messageType = MsgTypeIdInvalid);
    void purgeData();

protected:
    static uint16_t type2Pc(MsgTypeId_t messageType);
    void storeFrameDataType(uint16_t rxMsgTypeId, MessageDispatcher::RxMessageTypes_t rxMessageType);
    bool mergeDataMessages(std::list <RxMessage_t> ::iterator to, std::list <RxMessage_t> ::iterator from);
    void mergeNewDataMessage(std::list <RxMessage_t> ::iterator to, RxMessage_t from);
    bool pushMessage(RxMessage_t msg);
    bool pushHeaderMessage(RxMessage_t msg, uint32_t newProtocolItemFirstIndex);
    bool pushDataMessage(RxMessage_t msg);
    bool pushLastDataMessage();
    RxMessage_t splitLastDataMessage(uint32_t newProtocolItemFirstIndex);
    bool isPushable(RxMessage_t msg);

    uint32_t maxDataSize = -1;
    std::list <RxMessage_t> messages; /*! Per gestire meglio la quantità di dati salvati si potrebbe estendere la std::list <RxMessage_t> per gestire automaticamente la list size ogni volta che viene chiamata un'operazione
                                            che la modificherebbe, come push_back, erase, clear, insert, pop_front, etc */
    size_t listSize = 0;
    RxMessage_t lastDataMessage;
    bool lastDataMessageAvailable = false;
    RxMessage_t msg;
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

    mutable std::mutex rxMsgMutex;
    std::condition_variable rxMsgBufferNotEmpty;
};

#endif // FRAMEMANAGER_H
