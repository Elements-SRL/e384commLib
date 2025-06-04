#include "framemanager.h"

FrameManager::FrameManager() {}

void storeFrameData(uint16_t rxWordOffset) {
    if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataLoad]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, MessageDispatcher::RxMessageDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageThenCurrentDataLoad]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, MessageDispatcher::RxMessageVoltageThenCurrentDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageCurrentDataLoad]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionData, MessageDispatcher::RxMessageCurrentDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageDataLoad]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdInvalid, MessageDispatcher::RxMessageVoltageDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageVoltageAndGpDataLoad]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdInvalid, MessageDispatcher::RxMessageVoltageAndGpDataLoad);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataHeader]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionHeader, MessageDispatcher::RxMessageDataHeader);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageDataTail]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTail, MessageDispatcher::RxMessageDataTail);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageStatus]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdDeviceStatus, MessageDispatcher::RxMessageStatus);
    }
    else if (rxWordOffset == rxWordOffsets[MessageDispatcher::RxMessageTemperature]) {
        this->storeFrameDataType(MsgDirectionDeviceToPc+MsgTypeIdAcquisitionTemperature, MessageDispatcher::RxMessageTemperature);
    }
}
