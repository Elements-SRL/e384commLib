#ifndef FRAMEMANAGER_H
#define FRAMEMANAGER_H

#include <list>
#include <optional>

#include "e384commlib_global.h"
#include "messagedispatcher.h"

typedef struct RxMessage {
    uint16_t typeId;
    std::vector <uint16_t> data;
} RxMessage_t;

class EmcrDevice;

class FrameManager {
public:
    FrameManager(MessageDispatcher * md);

    void enableRxMessageType(MsgTypeId_t messageType, bool flag);
    bool isRxMessageTypeEnabled(MsgTypeId_t messageType);

    void setMaxDataSize(uint32_t size);
    void setRxWordParams(std::vector <uint16_t> rxWordOffsets, std::vector <uint16_t> rxWordLengths);
    void storeFrameData(uint16_t rxWordOffset);
    void storeFrameDataLoss(int32_t dataLossCount);
    std::optional <RxMessage_t> getNextMessage(MsgTypeId_t messageType = MsgTypeIdInvalid);
    void purgeData();

protected:
    static uint16_t type2Pc(MsgTypeId_t messageType);
    void storeFrameDataType(uint16_t rxMsgTypeId, MessageDispatcher::RxMessageTypes_t rxMessageType);
    bool pushMessage(RxMessage_t msg);
    bool mergeDataMessages(std::list <RxMessage_t> ::iterator to, std::list <RxMessage_t> ::iterator from);
    bool mergeDataMessages(std::list <RxMessage_t> ::iterator to, RxMessage_t from);
    void enlistLastDataMessage();

    uint32_t maxDataSize = -1;
    std::list <RxMessage_t> messages;
    std::optional <RxMessage_t> lastDataMessage = std::nullopt;
    RxMessage_t msg;
    MessageDispatcher * md = nullptr;
    EmcrDevice * emd = nullptr;
    int currentChannelsNum;
    int voltageChannelsNum;
    int gpChannelsNum;
    int totalChannelsNum;
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
