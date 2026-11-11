#ifndef UDBUTILS_H
#define UDBUTILS_H

#include <string>
#include <cstdint>

#include "stdafx.h"
#ifdef _WIN32
#include <windows.h> /*! Sleep() and the Windows types used by the UDB devices */
#endif
/*! CyAPI is no longer used directly: the USB access goes through e384_cyusb.dll, loaded at runtime */
#include "cyusbwrapper.h"

#define UDB_BULKIN_ENDPOINT_TIMEOUT ((int)2000)
#define UDB_BULKOUT_ENDPOINT_TIMEOUT ((int)2000)

#define UDB_FLASH_STATUS_WRITE_ENABLE_MASK (0x2)
#define UDB_FLASH_STATUS_WRITING_MASK (0x1)
#define UDB_SECTOR_SIZE (0x10000)
#define UDB_FX3_ADDRESS (0)
#define UDB_FX3_SIZE (0x80000)
#define UDB_INFO_ADDRESS (UDB_FX3_ADDRESS+UDB_FX3_SIZE)
#define UDB_INFO_SIZE UDB_SECTOR_SIZE // the size of the info block is smaller than the flash sections
                                      // so in order to write it a whole sections has to be erased first
                                      // and then rewritten
#define UDB_INFO_ACTUAL_SIZE (0x100)
#define UDB_FPGA_ADDRESS (UDB_FX3_ADDRESS+UDB_FX3_SIZE)
#define UDB_FPGA_ACTUAL_ADDRESS (UDB_FPGA_ADDRESS+UDB_INFO_ACTUAL_SIZE)
#define UDB_FPGA_SIZE (0x400000)
#define UDB_FLASH_STATUS_ERROR (2)

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

/*! USB IDs of the devices handled as UDB (from the Elements CyUSB3 INF).
 *  Other devices may use the CyUSB3 driver too: they are not listed. */
#define UDB_ELEMENTS_VID 0x3523         /*!< Elements devices, any PID */
#define UDB_FX3_BOOTLOADER_VID 0x04B4   /*!< Cypress FX3 running the bootloader (firmware not loaded yet) */
#define UDB_FX3_BOOTLOADER_PID 0x00F3

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
    static bool isSupportedDevice(uint32_t index);
    static int32_t getDeviceIndex(std::string serial);
    static std::string getDeviceSerial(uint32_t index);

    /*! The bulk endpoints are found by cyw_open inside e384_cyusb.dll: 
    no need for explicit findBulkEndpoints */
    static bool resetBulkEndpoints(cyw_device * dev);
    static void initEndpoints(uint32_t &readDataTransferSize, cyw_device * dev);

    static unsigned char getFwStatus(cyw_device * dev);
    static unsigned char getFlashStatus(cyw_device * dev);
    static void switchToConfigMode(cyw_device * dev);
    static void disableFlashHybridSectors(cyw_device * dev);
    static bool bootFpgafromFLASH(cyw_device * dev);
    static unsigned char fpgaLoadBitstreamStatus(cyw_device * dev);

    static void enableFlashWrite(cyw_device * dev);
    static void eraseFlashSector(cyw_device * dev, unsigned int idx);
    static void writeFlash(cyw_device * dev, unsigned int address, unsigned int length);
    static void readFlash(cyw_device * dev, unsigned int address, unsigned int &length);

    static long getRequiredLength(FlashBlock_t block);
    static long getStartAddress(FlashBlock_t block);
    static long getAddressOffset(FlashBlock_t block);
};

#endif // UDBUTILS_H
