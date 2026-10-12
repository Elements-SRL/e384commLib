#ifndef EMCRFTDILIBLOADTESTFAKE_H
#define EMCRFTDILIBLOADTESTFAKE_H

#include "emcrsuperduck_pcbv01.h"

/*! \brief Fake FTDI device (DEMO_FTDI_LibLoadTest), listed when %USERPROFILE%\e384_DEMO.pls exists.
 *  It only tests the runtime loading of the REAL FTDI libraries (FTD2XX and libMPSSE) during the
 *  connection, searched exactly as for a real device (folder of the executable, then System32):
 *  - libraries available: the connection succeeds, no data is produced;
 *  - libraries missing: the connection fails with ErrorFtdiDriverNotFound.
 *  To test the second case remove MPSSE.dll from the folder of the executable
 *  (FTD2XX.dll is usually also in System32, installed by the FTDI driver).
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
