#include "okprogrammer.h"

OkProgrammer::OkProgrammer() {

}

void OkProgrammer::connect(std::string serial, bool flag) {
    if (flag) {
        dev.OpenBySerial(serial);
    }
    else {
        dev.Close();
    }
}

void OkProgrammer::getDeviceInfo(InfoStruct_t &info) {
    unsigned char * buffer = nullptr;
    buffer = new unsigned char[OK_INFO_SIZE];
    readFlashBlock(BlockInfo, buffer);

    uint16_t * buffer16 = (uint16_t *)buffer;

    info.deviceVersion = buffer16[0];
    info.deviceSubVersion = buffer16[1];
    info.fpgaFwVersion.major = buffer16[2];
    info.fpgaFwVersion.minor = buffer16[3];
    info.fpgaFwVersion.patch = buffer16[4];

    delete [] buffer;
}

// void OkProgrammer::programFlashBlock(FlashBlock_t block, char * buffer, unsigned int length) {
//     unsigned int totalLength = getRequiredLength(block);
//     char * totalBuffer = new char[totalLength];
//     unsigned int offset = getAddressOffset(block);

//     long readBackLength = getRequiredLength(BlockFX3);

//     switch (block) {
//     case BlockFX3:
//         break;

//     case BlockInfo:
//         this->readFlashBlock(BlockInfo, totalBuffer);
//         readBackLength += getRequiredLength(BlockInfo);
//         break;

//     case BlockFPGA:
//         this->readFlashBlock(BlockInfo, totalBuffer);
//         readBackLength += getRequiredLength(BlockFPGA);
//         break;
//     }

//     for (unsigned int idx = 0; idx < length; idx++) {
//         totalBuffer[offset+idx] = buffer[idx];
//     }

//     std::this_thread::sleep_for (std::chrono::milliseconds(200));

//     this->eraseFlashBlock(block);

//     std::this_thread::sleep_for (std::chrono::milliseconds(200));

//     this->writeFlashBlock(block, totalBuffer, totalLength);

//     delete [] totalBuffer;
// }

// bool OkProgrammer::verifyFlashBlock(FlashBlock_t block, char * buffer, unsigned int length) {
//     char * readBuffer;
//     unsigned int offset = getAddressOffset(block);
//     unsigned int size = getRequiredLength(block);
//     bool ok = true;

//     readBuffer = new char[size];
//     readFlashBlock(block, readBuffer);

//     for (unsigned int idx = 0; idx < length; idx++) {
//         if (buffer[idx] != readBuffer[offset+idx]) {
//             ok = false;
//         }
//     }

//     delete [] readBuffer;
//     return ok;
// }

// int32_t UdbProgrammer::getProgress() {
//     return progress;
// }

// long UdbProgrammer::writeData(unsigned char * buffer, unsigned long length) {
//     long wlen = length;
//     if (eptBulkout->XferData((PUCHAR)buffer, wlen) == false) {
//         eptBulkout->Abort();
//         std::this_thread::sleep_for (std::chrono::milliseconds(200));
//     }

//     return wlen;
// }

// long UdbProgrammer::readData(unsigned char * buffer, unsigned long length) {
//     long rlen = length;
//     if (eptBulkin->XferData((PUCHAR)buffer, rlen, nullptr, true) == false) {
//         eptBulkin->Abort();
//         std::this_thread::sleep_for (std::chrono::milliseconds(200));
//         return 0;
//     }

//     return rlen;
// }

void OkProgrammer::readFlashBlock(FlashBlock_t block, unsigned char * buffer) {
    unsigned int startAddress = getStartAddress(block);
    unsigned int length = getRequiredLength(block);

    dev.FlashRead(startAddress, length, buffer);
}

// void UdbProgrammer::eraseFlashBlock(UdbUtils::FlashBlock_t block) {
//     unsigned int startAddress = UdbUtils::getStartAddress(block);
//     unsigned int length = UdbUtils::getRequiredLength(block);

//     UdbUtils::switchToConfigMode(dev, eptBulkin, eptBulkout);

//     progress = 0;

//     int tries;

//     for (unsigned int idx = 0; idx < length; idx += UDB_SECTOR_SIZE) {
//         /*! enableFlashWrite() necessary at each erase */
//         tries = 0;
//         while ((UdbUtils::getFlashStatus(dev) & UDB_FLASH_STATUS_WRITE_ENABLE_MASK) == 0 && ++tries <= 3) {
//             UdbUtils::enableFlashWrite(dev);
//             std::this_thread::sleep_for (std::chrono::milliseconds(1));
//         }

//         UdbUtils::eraseFlashSector(dev, startAddress+idx);

//         progress = (int32_t)((50*(idx+UDB_SECTOR_SIZE))/length);
//         progress = (progress > 50 ? 50 : progress);

//         while ((UdbUtils::getFlashStatus(dev) & UDB_FLASH_STATUS_WRITING_MASK) != 0) {
//             std::this_thread::sleep_for (std::chrono::milliseconds(10));
//         }
//     }
// }

// void UdbProgrammer::writeFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length) {
//     unsigned int startAddress = UdbUtils::getStartAddress(block);
//     unsigned int writeSize = 1024;
//     int flashWriteLen;

//     UdbUtils::switchToConfigMode(dev, eptBulkin, eptBulkout);

//     UdbUtils::writeFlash(dev, startAddress, length);

//     progress = 0;

//     for (unsigned int idx = 0; idx < length;) {
//         /*! enableFlashWrite not necessary since the FW does it when calling the writeFlash() */

//         flashWriteLen = writeData(((unsigned char *)buffer)+idx, length-idx >= writeSize ? writeSize : length-idx);

//         if (flashWriteLen == -1) {
//             return;
//         }
//         idx += flashWriteLen;

//         progress = 50+(int32_t)((50*(idx+UDB_SECTOR_SIZE))/length);
//         progress = (progress > 100 ? 100 : progress);

//         /*! check of UDB_FLASH_STATUS_WRITING_MASK not necessary since the FW does it when calling the writeData() */
//     }
// }

uint32_t OkProgrammer::getRequiredLength(FlashBlock_t block) {
    switch (block) {
    case BlockInfo:
        return OK_INFO_SIZE;

    case BlockFPGA:
        return OK_FPGA_SIZE;
    }
    return -1;
}

uint32_t OkProgrammer::getStartAddress(FlashBlock_t block) {
    switch (block) {
    case BlockInfo:
        return OK_INFO_ADDRESS;

    case BlockFPGA:
        return OK_FPGA_ADDRESS;
    }
    return -1;
}
