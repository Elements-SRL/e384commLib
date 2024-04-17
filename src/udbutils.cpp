#include "udbutils.h"

#include "thread"
#include "chrono"

UdbUtils::UdbUtils() {

}

bool UdbUtils::getDeviceCount(int &numDevs) {
    /*! Get the number of connected devices */
    CCyUSBDevice * tempDev = new CCyUSBDevice;
    numDevs = tempDev->DeviceCount();
    delete tempDev;
    return numDevs > 0;
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
    std::string serial;
    int numDevs;
    UdbUtils::getDeviceCount(numDevs);
    if (index >= numDevs) {
        return "";
    }

    CCyUSBDevice * tempDev = new CCyUSBDevice;
    char serialNumber[255];
    sprintf(serialNumber, "%ls", tempDev->SerialNumber);
    serial = std::string(serialNumber);
    tempDev->Close();
    delete tempDev;
    return serial;
}

void UdbUtils::findBulkEndpoints(CCyUSBDevice * dev, CCyUSBEndPoint * &eptBulkin, CCyUSBEndPoint * &eptBulkout) {
    int endpointcnt = 0;
    CCyUSBEndPoint * ept;

    endpointcnt = dev->EndPointCount();
    for (int e = 0; e < endpointcnt; e++) {
        ept = dev->EndPoints[e];
        // INTR, BULK and ISO endpoints are supported.
        if (ept->Attributes == 2) {
            if (ept->bIn) {
                eptBulkin = dev->EndPoints[e];

            } else {
                eptBulkout = dev->EndPoints[e];
            }
        }
    }
}

bool UdbUtils::resetBulkEndpoints(CCyUSBEndPoint * &eptBulkin, CCyUSBEndPoint * &eptBulkout) {
    if (eptBulkin->Reset() == false) {
        return false;
    }

    if (eptBulkout->Reset() == false) {
        return false;
    }
    return true;
}

void UdbUtils::initEndpoints(uint32_t &readDataTransferSize, CCyUSBEndPoint * &eptBulkin, CCyUSBEndPoint * &eptBulkout) {
    eptBulkin->TimeOut = UDB_BULKIN_ENDPOINT_TIMEOUT;
    eptBulkout->TimeOut = UDB_BULKOUT_ENDPOINT_TIMEOUT;

    /*! The total transfer must be a multiple of eptBulkin->MaxPktSize */
    readDataTransferSize = (readDataTransferSize/eptBulkin->MaxPktSize)*eptBulkin->MaxPktSize;

    eptBulkin->SetXferSize(readDataTransferSize);
    eptBulkout->SetXferSize(eptBulkout->MaxPktSize);
}

unsigned char UdbUtils::getFwStatus(CCyUSBDevice * dev) {
    CCyControlEndPoint * ctrept;
    long ctrlen = 1;
    unsigned char status = 0xFF;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_FROM_DEVICE;
    ctrept->ReqCode		= CYP_CMD_LOAD_FW_STATUS;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    if (ctrept->XferData((PUCHAR)&status, ctrlen) == false) {
        status = fwStatusError;
    }
    return status;
}

unsigned char UdbUtils::getFlashStatus(CCyUSBDevice * dev) {
    CCyControlEndPoint * ctrept;
    LONG ctrlen = 1;
    unsigned char status = 0xFF;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_FROM_DEVICE;
    ctrept->ReqCode		= CYP_CMD_LOAD_FLASH_STATUS;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    if (ctrept->XferData((PUCHAR)&status, ctrlen) == false) {
        /*! \todo FCON manage error */
    }
    return status;
}

void UdbUtils::switchToConfigMode(CCyUSBDevice * dev,CCyUSBEndPoint * &eptBulkin, CCyUSBEndPoint * &eptBulkout) {
    if (UdbUtils::getFwStatus(dev) != UdbUtils::fwStatusConfigMode) {
        CCyControlEndPoint * ctrept;
        LONG ctrlen = 0;

        ctrept = dev->ControlEndPt;
        ctrept->Target		= TGT_DEVICE;
        ctrept->ReqType		= REQ_VENDOR;
        ctrept->Direction	= DIR_TO_DEVICE;
        ctrept->ReqCode		= CYP_CMD_SET_CONFIGMODE;
        ctrept->Value		= 0;
        ctrept->Index		= 0;

        if (ctrept->XferData((PUCHAR)nullptr, ctrlen) == false) {
            /*! unhandled error */
        }

        UdbUtils::resetBulkEndpoints(eptBulkin, eptBulkout);
    }
}

void UdbUtils::disableFlashHybridSectors(CCyUSBDevice * dev) {
    /*! Needed only on factory new devices */
    CCyControlEndPoint * ctrept;
    LONG ctrlen = 0;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_TO_DEVICE;
    ctrept->ReqCode		= CYP_CMD_DISABLE_HYBRID_SECTORS;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    if (ctrept->XferData((PUCHAR)nullptr, ctrlen) == false) {
        /*! unhandled error */
    }
}

bool UdbUtils::bootFpgafromFLASH(CCyUSBDevice * dev) {
    CCyControlEndPoint * ctrept;
    long ctrlen = 0;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_TO_DEVICE;
    ctrept->ReqCode		= CYP_CMD_BOOT_FROM_FLASH;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    return ctrept->XferData((PUCHAR)nullptr, ctrlen);
}

unsigned char UdbUtils::fpgaLoadBitstreamStatus(CCyUSBDevice * dev) {
    CCyControlEndPoint * ctrept;
    long ctrlen = 1;
    unsigned char status = 0xFF;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_FROM_DEVICE;
    ctrept->ReqCode		= CYP_CMD_LOAD_BITSTREAM_STATUS;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    if (ctrept->XferData((PUCHAR)&status, ctrlen) == false) {
        status = fpgaLoadingError;
    }
    return status;
}

void UdbUtils::enableFlashWrite(CCyUSBDevice * dev) {
    CCyControlEndPoint * ctrept;
    LONG ctrlen = 0;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_TO_DEVICE;
    ctrept->ReqCode		= CYP_CMD_ENABLE_FLASH_WRITE;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    if (ctrept->XferData((PUCHAR)nullptr, ctrlen) == false) {
        /*! unhandled error */
    }
}

void UdbUtils::eraseFlashSector(CCyUSBDevice * dev, unsigned int address) {
    CCyControlEndPoint * ctrept;
    LONG ctrlen = sizeof(unsigned int);
    unsigned int payload;

    payload = address;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_TO_DEVICE;
    ctrept->ReqCode		= CYP_CMD_ERASE_FLASH_SECTOR;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    int tries = 0;
    while (ctrept->XferData((PUCHAR)&payload, ctrlen) == false && ++tries <= 3) {
        /*! unhandled error */
    }
}

void UdbUtils::writeFlash(CCyUSBDevice * dev, unsigned int address, unsigned int length) {
    CCyControlEndPoint * ctrept;
    LONG ctrlen = 2 * sizeof(unsigned int);
    unsigned int payload[2];

    payload[0] = address;
    payload[1] = length;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_TO_DEVICE;
    ctrept->ReqCode		= CYP_CMD_WRITE_FLASH;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    int tries = 0;
    while (ctrept->XferData((PUCHAR)payload, ctrlen) == false && ++tries <= 3) {
        /*! unhandled error */
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void UdbUtils::readFlash(CCyUSBDevice * dev, unsigned int address, unsigned int &length) {
    CCyControlEndPoint * ctrept;
    LONG ctrlen = 2 * sizeof(unsigned int);
    unsigned int payload[2];

    unsigned int readSize = 1024;

    length = ((length+readSize-1)/readSize)*readSize; /*!< upper readSize multiple */

    payload[0] = address;
    payload[1] = length;

    ctrept = dev->ControlEndPt;
    ctrept->Target		= TGT_DEVICE;
    ctrept->ReqType		= REQ_VENDOR;
    ctrept->Direction	= DIR_TO_DEVICE;
    ctrept->ReqCode		= CYP_CMD_READ_FLASH;
    ctrept->Value		= 0;
    ctrept->Index		= 0;

    int tries = 0;
    while (ctrept->XferData((PUCHAR)payload, ctrlen) == false && ++ tries <= 3) {
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
