#include "emcroklibloadtestfake.h"

#include <chrono>
#include <thread>

#include "okfrontpanelwrapper.h"

EmcrOkLibLoadTestFake::EmcrOkLibLoadTestFake(std::string id) :
    Emcr4x10MHz_PCBV01_V03(id) {

    /*! No real device behind: no calibration file to look for, start "reading" immediately */
    calibrationFileRequired = false;
    waitingTimeBeforeReadingData = 0;
}

ErrorCodes_t EmcrOkLibLoadTestFake::startCommunication(std::string) {

    if (!OkFrontPanelWrapper::load()) {
        return ErrorOkDriverNotFound;
    }
    return Success;
}

ErrorCodes_t EmcrOkLibLoadTestFake::stopCommunication() {
    /*! No device was opened */
    return Success;
}

bool EmcrOkLibLoadTestFake::writeRegistersAndActivateTriggers(TxTriggerType_t) {
    return true;
}

uint32_t EmcrOkLibLoadTestFake::readDataFromDevice() {
    /*! No data */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}
