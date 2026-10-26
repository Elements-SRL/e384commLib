#ifndef EMCROKLIBLOADTESTFAKE_H
#define EMCROKLIBLOADTESTFAKE_H

#include "emcr4x10mhz.h"

/*! \brief Fake Opal Kelly device (DEMO_OK_LibLoadTest), listed when %USERPROFILE%\e384_DEMO.pls exists.
 *  It only tests the runtime loading of the REAL okFrontPanel.dll, searched exactly as for a real
 *  device (folder of the module containing e384commLib, then System32):
 *  - DLL available: the connection succeeds, no data is produced;
 *  - DLL missing (or too old): the connection fails with ErrorOkDriverNotFound (startCommunication).
 *  To test the second case remove okFrontPanel.dll from the folder of the executable.
 *  The other DEMO devices do not use the DLL at all. No device is opened, no okCFrontPanel and no
 *  device monitor are created. Based on Emcr4x10MHz_PCBV01_V03 only to reuse a minimal channel
 *  configuration, as Emcr4x10MHzFake does. */
class EmcrOkLibLoadTestFake : public Emcr4x10MHz_PCBV01_V03 {
public:
    EmcrOkLibLoadTestFake(std::string id);

protected:
    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual ErrorCodes_t stopCommunication() override;

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;
};

#endif // EMCROKLIBLOADTESTFAKE_H
