#ifndef OKPROGRAMMER_H
#define OKPROGRAMMER_H

#define OK_INFO_ADDRESS 0x100000
#define OK_INFO_SIZE    0x10000
#define OK_FPGA_ADDRESS 0x110000
#define OK_FPGA_SIZE    0xEF0000

#include "utils.h"
#include "okFrontPanelDLL.h"

#include <string>

class OkProgrammer {
public:    
    typedef enum FlashBlock {
        BlockInfo,
        BlockFPGA
    } FlashBlock_t;

    typedef struct FwVersion {
        uint16_t major;
        uint16_t minor;
        uint16_t patch;
    } FwVersion_t;

    typedef struct InfoStruct {
        uint16_t deviceVersion;
        uint16_t deviceSubVersion;
        FwVersion_t fpgaFwVersion;
    } InfoStruct_t;

    OkProgrammer();

    void connect(std::string serial, bool flag);
    void getDeviceInfo(InfoStruct_t &info);

    static uint32_t getRequiredLength(FlashBlock_t block);
    static uint32_t getStartAddress(FlashBlock_t block);
    // void programFlashBlock(FlashBlock_t block, char * buffer, unsigned int length);
    // bool verifyFlashBlock(FlashBlock_t block, char * buffer, unsigned int length);
    // int32_t getProgress();

private:
    // long writeData(unsigned char * buffer, unsigned long length);
    // long readData(unsigned char * buffer, unsigned long length);

    void readFlashBlock(FlashBlock_t block, unsigned char * buffer);
    // void eraseFlashBlock(FlashBlock_t block);
    // void writeFlashBlock(FlashBlock_t block, char * buffer, unsigned int length);

    okCFrontPanel dev;
};

#endif // OKPROGRAMMER_H
