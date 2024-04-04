#ifndef UDBUTILS_H
#define UDBUTILS_H

#include <string>

#include "stdafx.h"
#ifdef _WIN32
#include <windows.h> /*! This has to be included before CyAPI.h, otherwise it won't be correctly interpreted */
#endif
#include "CyAPI.h"

#define UDB_BULKIN_ENDPOINT_TIMEOUT ((int)2000)
#define UDB_BULKOUT_ENDPOINT_TIMEOUT ((int)2000)

#define UDB_FLASH_STATUS_WRITE_ENABLE_MASK (0x2)
#define UDB_FLASH_STATUS_WRITING_MASK (0x1)
#define UDB_FX3_ADDRESS (0)
#define UDB_FX3_SIZE (0x80000)
#define UDB_INFO_ADDRESS (UDB_FX3_ADDRESS+UDB_FX3_SIZE)
#define UDB_INFO_SIZE (0x10000)
#define UDB_INFO_ACTUAL_SIZE (0x100)
#define UDB_FPGA_ADDRESS (UDB_FX3_ADDRESS+UDB_FX3_SIZE)
#define UDB_FPGA_ACTUAL_ADDRESS (UDB_FPGA_ADDRESS+UDB_INFO_ACTUAL_SIZE)
#define UDB_FPGA_SIZE (0x400000)
#define UDB_FLASH_STATUS_ERROR (2)
#define UDB_SECTOR_SIZE (65536)

#define CYP_CMD_GET_TUPLE 0xC1
#define CYP_CMD_BOOT_FROM_FLASH 0xBF
#define CYP_CMD_LOAD_BITSTREAM_STATUS 0xB1
#define CYP_CMD_SET_CONFIGMODE 0xB3
#define CYP_CMD_LOAD_FW_STATUS 0xB4
#define CYP_CMD_READ_FLASH 0xB7
#define CYP_CMD_ENABLE_FLASH_WRITE 0xB9
#define CYP_CMD_WRITE_FLASH 0xBA
#define CYP_CMD_LOAD_FLASH_STATUS 0xBB
#define CYP_CMD_DISABLE_HYBRID_SECTORS 0xBC
#define CYP_CMD_ERASE_FLASH_SECTOR 0xBE

class UdbUtils {
public:
    typedef enum fwStatus {
        fwStatusConfigMode = 0,
        fwStatusSlaveFifo = 1,
        fwStatusError = 2
    } fwStatus_t;

    typedef enum fpgaLoadingStatus {
        fpgaLoadingInProgress = 0,
        fpgaLoadingSuccess = 1,
        fpgaLoadingError = 2
    } fpgaLoadingStatus_t;

    typedef enum FlashBlock {
        BlockFX3,
        BlockInfo,
        BlockFPGA
    } FlashBlock_t;

    UdbUtils();

    static bool getDeviceCount(int &numDevs);
    static int32_t getDeviceIndex(std::string serial);
    static std::string getDeviceSerial(uint32_t index);

    static void findBulkEndpoints(CCyUSBDevice * dev, CCyUSBEndPoint * &eptBulkin, CCyUSBEndPoint * &eptBulkout);
    static bool resetBulkEndpoints(CCyUSBEndPoint * &eptBulkin, CCyUSBEndPoint * &eptBulkout);
    static void initEndpoints(uint32_t &readDataTransferSize, CCyUSBEndPoint * &eptBulkin, CCyUSBEndPoint * &eptBulkout);

    static unsigned char getFwStatus(CCyUSBDevice * dev);
    static unsigned char getFlashStatus(CCyUSBDevice * dev);
    static void switchToConfigMode(CCyUSBDevice * dev);
    static void disableFlashHybridSectors(CCyUSBDevice * dev);
    static bool bootFpgafromFLASH(CCyUSBDevice * dev);
    static unsigned char fpgaLoadBitstreamStatus(CCyUSBDevice * dev);

    static void enableFlashWrite(CCyUSBDevice * dev);
    static void eraseFlashSector(CCyUSBDevice * dev, unsigned int idx);
    static void writeFlash(CCyUSBDevice * dev, unsigned int address, unsigned int length);
    static void readFlash(CCyUSBDevice * dev, unsigned int address, unsigned int length);
};

#endif // UDBUTILS_H
