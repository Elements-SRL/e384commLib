#include "emcrcyusblibloadtestfake.h"

#include <chrono>
#include <thread>

#include "cyusbwrapper.h"

EmcrCyUsbLibLoadTestFake::EmcrCyUsbLibLoadTestFake(std::string id) :
    Emcr10MHz_V01(id) {

}

ErrorCodes_t EmcrCyUsbLibLoadTestFake::startCommunication(std::string) {
    /*! The only thing tested: the same DLL a real UDB device needs must be loadable
     *  (file found, all the functions exported, same API version) */
    if (!CyUsbWrapper::load()) {
        return ErrorCyUsbDriverNotFound;
    }
    return Success;
}

ErrorCodes_t EmcrCyUsbLibLoadTestFake::initializeHW() {
    /*! No FX3/FPGA to configure: just let the communication thread run */
    fwLoadedFlag = true;
    parsingStatus = ParsingParsing;
    return Success;
}

ErrorCodes_t EmcrCyUsbLibLoadTestFake::stopCommunication() {
    /*! No device was opened */
    return Success;
}

bool EmcrCyUsbLibLoadTestFake::writeRegistersAndActivateTriggers(TxTriggerType_t) {
    return true;
}

uint32_t EmcrCyUsbLibLoadTestFake::readDataFromDevice() {
    /*! No data */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}
