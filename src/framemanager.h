#ifndef FRAMEMANAGER_H
#define FRAMEMANAGER_H

#include "e384commlib_global.h"
#include "messagedispatcher.h"

class FrameManager {
public:
    FrameManager(MessageDispatcher * md);

    void setRxWordParams(std::vector <uint16_t> rxWordOffsets, std::vector <uint16_t> rxWordLengths);
    void storeFrameData(uint16_t rxWordOffset);

protected:
    void storeFrameDataType(uint16_t rxMsgTypeId, MessageDispatcher::RxMessageTypes_t rxMessageType);
};

#endif // FRAMEMANAGER_H
