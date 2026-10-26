#ifndef EMCRCYUSBLIBLOADTESTFAKE_H
#define EMCRCYUSBLIBLOADTESTFAKE_H

#include "emcr10mhz.h"

/*! \brief Fake UDB device (DEMO_CYUSB_LibLoadTest), listed when %USERPROFILE%\e384_DEMO.pls exists.
 *  It only tests the runtime loading of the REAL e384_cyusb.dll during the connection, searched
 *  exactly as for a real device (folder of the module containing e384commLib, then System32):
 *  - DLL available: the connection succeeds, no data is produced;
 *  - DLL missing (or wrong API version): the connection fails with ErrorCyUsbDriverNotFound.
 *  To test the second case remove e384_cyusb.dll from the folder of the executable.
 *  No USB access at all. Based on Emcr10MHz_V01 (the simplest UDB device) only to reuse a
 *  minimal channel configuration, as EmcrFtdiLibLoadTestFake does with SuperDuck. */
class EmcrCyUsbLibLoadTestFake : public Emcr10MHz_V01 {
public:
    EmcrCyUsbLibLoadTestFake(std::string id);

protected:
    virtual ErrorCodes_t startCommunication(std::string fwPath) override;
    virtual ErrorCodes_t initializeHW() override;
    virtual ErrorCodes_t stopCommunication() override;

    virtual bool writeRegistersAndActivateTriggers(TxTriggerType_t type) override;
    virtual uint32_t readDataFromDevice() override;
};

#endif // EMCRCYUSBLIBLOADTESTFAKE_H
