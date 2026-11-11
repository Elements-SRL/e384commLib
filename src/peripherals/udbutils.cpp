#include "udbutils.h"

#include "thread"
#include "chrono"

/*! Vendor request on the control endpoint of the device (Target = device, ReqType = vendor),
 *  value and index always 0, as for all the CYP_CMD_* commands.
 *  Replaces the ControlEndPt->Target/ReqType/Direction/... + XferData sequence of CyAPI.
 *  length: in = bytes to send/receive, out = bytes transferred. */
static bool vendorRequest(cyw_device * dev, int direction, unsigned char request, unsigned char * buffer, long &length) {
    return CyUsbWrapper::vendorRequest(dev, direction, request, 0, 0, buffer, &length) == CYW_OK;
}

UdbUtils::UdbUtils() {

}

bool UdbUtils::getDeviceCount(int &numDevs) {
    /*! Get the number of connected devices (any device bound to the CyUSB3 driver).
     *  Fails if e384_cyusb.dll is not loaded. */
    numDevs = 0;
    return CyUsbWrapper::deviceCount(&numDevs) == CYW_OK;
}

bool UdbUtils::isSupportedDevice(uint32_t index) {
    unsigned short vid = 0;
    unsigned short pid = 0;
    if (CyUsbWrapper::deviceInfo(index, &vid, &pid, nullptr, 0) != CYW_OK) {
        return false;
    }
    return vid == UDB_ELEMENTS_VID || (vid == UDB_FX3_BOOTLOADER_VID && pid == UDB_FX3_BOOTLOADER_PID);
}

int32_t UdbUtils::getDeviceIndex(std::string serial) {
    /*! Gets number of devices */
    int numDevs;
    bool devCountOk = UdbUtils::getDeviceCount(numDevs);
    if (!devCountOk) {
        return -1;

    } else if (numDevs == 0) {
        return -1;
    }

    for (int index = 0; index < numDevs; index++) {
        std::string deviceId = getDeviceSerial(index);
        if (deviceId == serial) {
            return index;
        }
    }
    return -1;
}

std::string UdbUtils::getDeviceSerial(uint32_t index) {
    /*! cyw_device_info opens the device at this index: the previous CyAPI code read always the
     *  serial of device 0, because the CCyUSBDevice constructor opens device 0 */
    unsigned short vid = 0;
    unsigned short pid = 0;
    char serialNumber[255];
    if (CyUsbWrapper::deviceInfo(index, &vid, &pid, serialNumber, static_cast <int> (sizeof(serialNumber))) != CYW_OK) {
        return "";
    }
    return std::string(serialNumber);
}

bool UdbUtils::resetBulkEndpoints(cyw_device * dev) {
    /*! Resets the IN endpoint, then the OUT one, stops at the first failure */
    return CyUsbWrapper::bulkReset(dev) == CYW_OK;
}

void UdbUtils::initEndpoints(uint32_t &readDataTransferSize, cyw_device * dev) {
    /*! The DLL sets the timeouts, rounds the IN transfer size down to a multiple of the IN
     *  MaxPktSize and sets the OUT transfer size to the OUT MaxPktSize (same steps as before) */
    unsigned long actualTransferSize = readDataTransferSize;
    if (CyUsbWrapper::bulkConfig(dev, readDataTransferSize, &actualTransferSize,
                                 UDB_BULKIN_ENDPOINT_TIMEOUT, UDB_BULKOUT_ENDPOINT_TIMEOUT) == CYW_OK) {
        readDataTransferSize = actualTransferSize;
    }
}

unsigned char UdbUtils::getFwStatus(cyw_device * dev) {
    long ctrlen = 1;
    unsigned char status = 0xFF;

    if (vendorRequest(dev, CYW_DIR_FROM_DEVICE, CYP_CMD_LOAD_FW_STATUS, &status, ctrlen) == false) {
        status = fwStatusError;
    }
    return status;
}

unsigned char UdbUtils::getFlashStatus(cyw_device * dev) {
    long ctrlen = 1;
    unsigned char status = 0xFF;

    if (vendorRequest(dev, CYW_DIR_FROM_DEVICE, CYP_CMD_LOAD_FLASH_STATUS, &status, ctrlen) == false) {
        /*! \todo FCON manage error */
    }
    return status;
}

void UdbUtils::switchToConfigMode(cyw_device * dev) {
    if (UdbUtils::getFwStatus(dev) != UdbUtils::fwStatusConfigMode) {
        long ctrlen = 0;

        if (vendorRequest(dev, CYW_DIR_TO_DEVICE, CYP_CMD_SET_CONFIGMODE, nullptr, ctrlen) == false) {
            /*! unhandled error */
        }

        UdbUtils::resetBulkEndpoints(dev);
    }
}

void UdbUtils::disableFlashHybridSectors(cyw_device * dev) {
    /*! Needed only on factory new devices */
    long ctrlen = 0;

    if (vendorRequest(dev, CYW_DIR_TO_DEVICE, CYP_CMD_DISABLE_HYBRID_SECTORS, nullptr, ctrlen) == false) {
        /*! unhandled error */
    }
}

bool UdbUtils::bootFpgafromFLASH(cyw_device * dev) {
    long ctrlen = 0;

    return vendorRequest(dev, CYW_DIR_TO_DEVICE, CYP_CMD_BOOT_FROM_FLASH, nullptr, ctrlen);
}

unsigned char UdbUtils::fpgaLoadBitstreamStatus(cyw_device * dev) {
    long ctrlen = 1;
    unsigned char status = 0xFF;

    if (vendorRequest(dev, CYW_DIR_FROM_DEVICE, CYP_CMD_LOAD_BITSTREAM_STATUS, &status, ctrlen) == false) {
        status = fpgaLoadingError;
    }
    return status;
}

void UdbUtils::enableFlashWrite(cyw_device * dev) {
    long ctrlen = 0;

    if (vendorRequest(dev, CYW_DIR_TO_DEVICE, CYP_CMD_ENABLE_FLASH_WRITE, nullptr, ctrlen) == false) {
        /*! unhandled error */
    }
}

void UdbUtils::eraseFlashSector(cyw_device * dev, unsigned int address) {
    long ctrlen = sizeof(unsigned int);
    unsigned int payload;

    payload = address;

    int tries = 0;
    while (vendorRequest(dev, CYW_DIR_TO_DEVICE, CYP_CMD_ERASE_FLASH_SECTOR, (unsigned char *)&payload, ctrlen) == false && ++tries <= 3) {
        /*! unhandled error */
        ctrlen = sizeof(unsigned int); /*!< XferData may have changed it */
    }
}

void UdbUtils::writeFlash(cyw_device * dev, unsigned int address, unsigned int length) {
    long ctrlen = 2 * sizeof(unsigned int);
    unsigned int payload[2];

    payload[0] = address;
    payload[1] = length;

    int tries = 0;
    while (vendorRequest(dev, CYW_DIR_TO_DEVICE, CYP_CMD_WRITE_FLASH, (unsigned char *)payload, ctrlen) == false && ++tries <= 3) {
        /*! unhandled error */
        ctrlen = 2 * sizeof(unsigned int); /*!< XferData may have changed it */
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(500));
}

void UdbUtils::readFlash(cyw_device * dev, unsigned int address, unsigned int &length) {
    long ctrlen = 2 * sizeof(unsigned int);
    unsigned int payload[2];

    unsigned int readSize = 1024;

    length = ((length+readSize-1)/readSize)*readSize; /*!< upper readSize multiple */

    payload[0] = address;
    payload[1] = length;

    int tries = 0;
    while (vendorRequest(dev, CYW_DIR_TO_DEVICE, CYP_CMD_READ_FLASH, (unsigned char *)payload, ctrlen) == false && ++ tries <= 3) {
        ctrlen = 2 * sizeof(unsigned int); /*!< XferData may have changed it */
    }

    std::this_thread::sleep_for (std::chrono::milliseconds(100));
}

long UdbUtils::getRequiredLength(FlashBlock_t block) {
    switch (block) {
    case UdbUtils::BlockFX3:
        return UDB_FX3_SIZE;

    case UdbUtils::BlockInfo:
        return UDB_INFO_SIZE;

    case UdbUtils::BlockFPGA:
        return UDB_FPGA_SIZE;
    }
    return -1;
}

long UdbUtils::getStartAddress(FlashBlock_t block) {
    switch (block) {
    case UdbUtils::BlockFX3:
        return UDB_FX3_ADDRESS;

    case UdbUtils::BlockInfo:
        return UDB_INFO_ADDRESS;

    case UdbUtils::BlockFPGA:
        return UDB_FPGA_ADDRESS;
    }
    return -1;
}

long UdbUtils::getAddressOffset(FlashBlock_t block) {
    switch (block) {
    case UdbUtils::BlockFX3:
        return 0;

    case UdbUtils::BlockInfo:
        return 0;

    case UdbUtils::BlockFPGA:
        return UDB_INFO_ACTUAL_SIZE;
    }
    return -1;
}
