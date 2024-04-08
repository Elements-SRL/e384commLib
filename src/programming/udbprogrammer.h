#ifndef UDBPROGRAMMER_H
#define UDBPROGRAMMER_H

#include "udbutils.h"
#include "utils.h"

#include <string>

//#define UDB_LOG_PATH QString(QDir::homePath() + "/UdbFlashUpdater/")

class UdbProgrammer {
public:
    typedef struct InfoStruct {
        unsigned char deviceVersion;
        unsigned char deviceSubVersion;
        unsigned char fpgaFwVersion;
        char serialNumber[8];
        unsigned char fx3FwVersion;
    } InfoStruct_t;

    UdbProgrammer();
    ~UdbProgrammer();

    void connect(int idx, bool flag);
    void getDeviceInfo(InfoStruct_t &info);
    void programFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length);
    bool verifyFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length);
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
