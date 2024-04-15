#include "udbprogrammer.h"

#include <chrono>
#include <thread>

#include <iostream>

static int fileIdx = 0;

UdbProgrammer::UdbProgrammer() {

}

UdbProgrammer::~UdbProgrammer() {

}

void UdbProgrammer::connect(int idx, bool flag) {
    if (flag) {
        CCyFX3Device bootDev;
        bootDev.Open(idx);
        if (bootDev.IsBootLoaderRunning()) {
            bootDev.DownloadFw(const_cast <char *> ((UTL_DEFAULT_FW_PATH + UTL_DEFAULT_FX3_FW_NAME).c_str()), RAM);
        }
        bootDev.Close();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        dev = new CCyUSBDevice;
        dev->Open(idx);

        UdbUtils::findBulkEndpoints(dev, eptBulkin, eptBulkout);
        UdbUtils::resetBulkEndpoints(eptBulkin, eptBulkout);
        UdbUtils::initEndpoints(readDataTransferSize, eptBulkin, eptBulkout);

    } else {
        dev->Close();
        delete dev;
    }
}

void UdbProgrammer::getDeviceInfo(InfoStruct_t &info) {
    char * buffer = nullptr;
    buffer = new char[UDB_INFO_SIZE];
    readFlashBlock(UdbUtils::BlockInfo, buffer);

    info.deviceVersion = buffer[0];
    info.deviceSubVersion = buffer[1];
    info.fpgaFwVersion = buffer[2];
    for (unsigned int idx = 0; idx < 8; idx++) {
        info.serialNumber[idx] = buffer[3+idx];
    }
    info.fx3FwVersion = buffer[11];

    delete [] buffer;
}

void UdbProgrammer::programFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length) {
    unsigned int totalLength = UdbUtils::getRequiredLength(block);
    char * totalBuffer = new char[totalLength];
    unsigned int offset = UdbUtils::getAddressOffset(block);

    long readBackLength = UdbUtils::getRequiredLength(UdbUtils::BlockFX3);

    switch (block) {
    case UdbUtils::BlockFX3:
        break;

    case UdbUtils::BlockInfo:
        this->readFlashBlock(UdbUtils::BlockInfo, totalBuffer);
        readBackLength += UdbUtils::getRequiredLength(UdbUtils::BlockInfo);
        break;

    case UdbUtils::BlockFPGA:
        this->readFlashBlock(UdbUtils::BlockInfo, totalBuffer);
        readBackLength += UdbUtils::getRequiredLength(UdbUtils::BlockFPGA);
        break;
    }

    for (unsigned int idx = 0; idx < length; idx++) {
        totalBuffer[offset+idx] = buffer[idx];
    }

    FILE * fid = fopen(("file" + std::to_string(fileIdx++) + ".dat").c_str(), "wb");
    fwrite(totalBuffer, 1, totalLength, fid);
    fflush(fid);
    fclose(fid);

    char * readBackBuffer = new char[readBackLength];
    this->readFlashBlock(UdbUtils::BlockFX3, readBackBuffer);
    this->readFlashBlock(block, readBackBuffer+UDB_FX3_SIZE);
    fid = fopen(("file" + std::to_string(fileIdx++) + ".dat").c_str(), "wb");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    fwrite(readBackBuffer, 1, readBackLength, fid);
    fflush(fid);
    fclose(fid);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    this->eraseFlashBlock(block);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    this->readFlashBlock(UdbUtils::BlockFX3, readBackBuffer);
    this->readFlashBlock(block, readBackBuffer+UDB_FX3_SIZE);
    fid = fopen(("file" + std::to_string(fileIdx++) + ".dat").c_str(), "wb");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    fwrite(readBackBuffer, 1, readBackLength, fid);
    fflush(fid);
    fclose(fid);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    this->writeFlashBlock(block, totalBuffer, totalLength);

    delete [] totalBuffer;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    this->readFlashBlock(UdbUtils::BlockFX3, readBackBuffer);
    this->readFlashBlock(block, readBackBuffer+UDB_FX3_SIZE);
    fid = fopen(("file" + std::to_string(fileIdx++) + ".dat").c_str(), "wb");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    fwrite(readBackBuffer, 1, readBackLength, fid);
    fflush(fid);
    fclose(fid);

    delete [] readBackBuffer;
}

bool UdbProgrammer::verifyFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length) {
    char * readBuffer;
    unsigned int offset = UdbUtils::getAddressOffset(block);
    unsigned int size = UdbUtils::getRequiredLength(block);
    bool ok = true;

    readBuffer = new char[size];
    readFlashBlock(block, readBuffer);

    for (unsigned int idx = 0; idx < length; idx++) {
        if (buffer[idx] != readBuffer[offset+idx]) {
            ok = false;
        }
    }

    delete [] readBuffer;
    return ok;
}

int32_t UdbProgrammer::getProgress() {
    return progress;
}

long UdbProgrammer::writeData(unsigned char * buffer, unsigned long length) {
    long wlen = length;
    if (eptBulkout->XferData((PUCHAR)buffer, wlen) == false) {
        eptBulkout->Abort();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "write data error" << std::endl;
    }

    return wlen;
}

long UdbProgrammer::readData(unsigned char * buffer, unsigned long length) {
    long rlen = length;
    if (eptBulkin->XferData((PUCHAR)buffer, rlen, nullptr, true) == false) {
        eptBulkin->Abort();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "read data error " << std::endl;
        return 0;
    }
    std::cout << rlen << " " << length << std::endl;

    return rlen;
}

void UdbProgrammer::readFlashBlock(UdbUtils::FlashBlock_t block, char * buffer) {
    unsigned int startAddress = UdbUtils::getStartAddress(block);
    unsigned int length = UdbUtils::getRequiredLength(block);

    UdbUtils::switchToConfigMode(dev, eptBulkin, eptBulkout);

    long rlen = 0;
    while (rlen < length) {
        UdbUtils::readFlash(dev, startAddress, length);

        rlen = this->readData((unsigned char *)buffer, length);
    }
}

void UdbProgrammer::eraseFlashBlock(UdbUtils::FlashBlock_t block) {
    unsigned int startAddress = UdbUtils::getStartAddress(block);
    unsigned int length = UdbUtils::getRequiredLength(block);

    UdbUtils::switchToConfigMode(dev, eptBulkin, eptBulkout);

    UdbUtils::disableFlashHybridSectors(dev);

    progress = 0;

    for (unsigned int idx = 0; idx < length; idx += UDB_SECTOR_SIZE) {
        while ((UdbUtils::getFlashStatus(dev) & UDB_FLASH_STATUS_WRITE_ENABLE_MASK) == 0) {
            UdbUtils::enableFlashWrite(dev);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "enable flash write retry" << std::endl;
        }

        UdbUtils::eraseFlashSector(dev, startAddress+idx);

        progress = (int32_t)((50*(idx+UDB_SECTOR_SIZE))/length);
        progress = (progress > 50 ? 50 : progress);

        while ((UdbUtils::getFlashStatus(dev) & UDB_FLASH_STATUS_WRITING_MASK) != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void UdbProgrammer::writeFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length) {
    unsigned int startAddress = UdbUtils::getStartAddress(block);
    unsigned int writeSize = 1024;
    int flashWriteLen;

    UdbUtils::switchToConfigMode(dev, eptBulkin, eptBulkout);

    UdbUtils::writeFlash(dev, startAddress, length);

    progress = 0;

    for (unsigned int idx = 0; idx < length;) {
        while ((UdbUtils::getFlashStatus(dev) & UDB_FLASH_STATUS_WRITE_ENABLE_MASK) == 0) {
            UdbUtils::enableFlashWrite(dev);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::cout << "enable flash write retry" << std::endl;
        }

        flashWriteLen = writeData(((unsigned char *)buffer)+idx, length-idx >= writeSize ? writeSize : length-idx);

        if (flashWriteLen == -1) {
            std::cout << "write data failed" << std::endl;
            return;
        }
        idx += flashWriteLen;

        progress = 50+(int32_t)((50*(idx+UDB_SECTOR_SIZE))/length);
        progress = (progress > 100 ? 100 : progress);

        while ((UdbUtils::getFlashStatus(dev) & UDB_FLASH_STATUS_WRITING_MASK) != 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
