#ifndef EMCRFTDILIBLOADTESTFAKE_H
#define EMCRFTDILIBLOADTESTFAKE_H

#include "emcrsuperduck_pcbv01.h"

/*! \brief Fake FTDI device (DEMO_FTDI_LibLoadTest), listed when %USERPROFILE%\e384_DEMO.pls exists.
 *  It only tests the runtime loading of the FTDI libraries (FTD2XX and libMPSSE) during the connection:
 *  - libraries available: the connection succeeds, no data is produced;
 *  - libraries missing: the connection fails with ErrorFtdiDriverNotFound.
 *  Combine with %USERPROFILE%\e384_NOFTDI.pls to simulate missing libraries.
 *  Based on SuperDuck (1 channel, no FPGA load via SPI) only to reuse a minimal channel configuration. */
class EmcrFtdiLibLoadTestFake : public EmcrSuperDuck_PCBV01 {
public:
    EmcrFtdiLibLoadTestFake(std::string id);

protected:
    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual ErrorCodes_t stopCommunication() override;

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;
};

#endif // EMCRFTDILIBLOADTESTFAKE_H
