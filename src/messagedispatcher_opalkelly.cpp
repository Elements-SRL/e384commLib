#include "messagedispatcher_opalkelly.h"

using namespace std;

MessageDispatcher_OpalKelly::MessageDispatcher_OpalKelly(string deviceId) :
    MessageDispatcher(deviceId) {

}

MessageDispatcher_OpalKelly::~MessageDispatcher_OpalKelly() {

}

ErrorCodes_t MessageDispatcher_OpalKelly::connect() {
    if (connected) {
        return ErrorDeviceAlreadyConnected;
    }

    dev = new okCFrontPanel;

    okCFrontPanel::ErrorCode error = dev->OpenBySerial(deviceId);
    if (error != okCFrontPanel::NoError) {
        return ErrorDeviceConnectionFailed;
    }

    error = dev->ConfigureFPGA(fwName);

    if (error != okCFrontPanel::NoError) {
        return ErrorDeviceFwLoadingFailed;
    }
    return MessageDispatcher::connect();
}

ErrorCodes_t MessageDispatcher_OpalKelly::disconnect() {
    if (!connected) {
        return ErrorDeviceNotConnected;
    }

    if (dev != nullptr) {
        dev->Close();
        delete dev;
        dev = nullptr;
    }
    return MessageDispatcher::disconnect();
}

void MessageDispatcher_OpalKelly::readDataFromDevice() {

}

void MessageDispatcher_OpalKelly::sendCommandsToDevice() {

}
