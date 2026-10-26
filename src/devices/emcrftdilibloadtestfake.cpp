#include "emcrftdilibloadtestfake.h"

#include <chrono>
#include <thread>

#include "ftd2xxwrapper.h"

EmcrFtdiLibLoadTestFake::EmcrFtdiLibLoadTestFake(std::string id) :
    EmcrSuperDuck_PCBV01(id) {

}

ErrorCodes_t EmcrFtdiLibLoadTestFake::startCommunication(std::string) {
    /*! The only thing tested: the same libraries a real FTDI device needs must be loadable */
    if (!Ftd2xxWrapper::loadFtd2xx()) {
        return ErrorFtdiDriverNotFound;
    }
    if (!Ftd2xxWrapper::loadMpsse()) {
        return ErrorFtdiDriverNotFound;
    }
    return Success;
}

ErrorCodes_t EmcrFtdiLibLoadTestFake::stopCommunication() {
    /*! No FTDI handles to close */
    return Success;
}

bool EmcrFtdiLibLoadTestFake::writeRegistersAndActivateTriggers(TxTriggerType_t) {
    return true;
}

uint32_t EmcrFtdiLibLoadTestFake::readDataFromDevice() {
    /*! No data */
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}
