#ifndef UDBPROGRAMMER_H
#define UDBPROGRAMMER_H

#include "udbutils.h"

#include <string>

//#define UDB_LOG_PATH QString(QDir::homePath() + "/UdbFlashUpdater/")

class UdbProgrammer {
public:
    UdbProgrammer();
    ~UdbProgrammer();

    void connect(int idx, bool flag);
    void getDeviceInfo(char &deviceVersion, char &deviceSubVersion, char &fpgaFwVersion, char &fx3FwVersion);
    void programFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length);
    int32_t getProgress();

private:
    long writeData(unsigned char * buffer, unsigned long length);
    long readData(unsigned char * buffer, unsigned long length);

    void readFlashBlock(UdbUtils::FlashBlock_t block, char * buffer);
    void eraseFlashBlock(UdbUtils::FlashBlock_t block);
    void writeFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length);

    CCyUSBDevice * dev;

    /*! endpoints */
    CCyUSBEndPoint * eptBulkin;
    CCyUSBEndPoint * eptBulkout;

    /*! Rx consts */
    uint32_t readDataTransferSize = 0x100000; /*! 1MB, must be lower than 4MB */

    int32_t progress = 0;

//    QFile logFile;
//    QTextStream logStream;
};

#endif // UDBPROGRAMMER_H
