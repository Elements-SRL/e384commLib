#include "udbprogrammer.h"

#include <chrono>
#include <thread>

UdbProgrammer::UdbProgrammer() {
//    logFile.setFileName(UDB_LOG_PATH + "Log" + QDateTime().currentDateTime().toString("yyyyMMdd-hhmmss") + ".txt");
//    logFile.open(QFile::WriteOnly | QFile::Truncate);
//    logStream.setDevice(&logFile);
}

UdbProgrammer::~UdbProgrammer() {
//    logStream.flush();
//    logStream.setDevice(nullptr);
//    logFile.close();
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
    char * buffer;
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
    char * totalBuffer;
    unsigned int totalLength;
    unsigned int offset;

    switch (block) {
    case UdbUtils::BlockFX3:
        offset = 0;
        totalLength = length;
        totalBuffer = new char[totalLength];
        break;

    case UdbUtils::BlockInfo:
        offset = 0;
        totalLength = UDB_INFO_SIZE;
        totalBuffer = new char[totalLength];
        this->readFlashBlock(UdbUtils::BlockInfo, totalBuffer);
        break;

    case UdbUtils::BlockFPGA:
        offset = UDB_FPGA_ACTUAL_ADDRESS-UDB_FPGA_ADDRESS;
        totalLength = offset+length;
        totalBuffer = new char[totalLength];
        this->readFlashBlock(UdbUtils::BlockInfo, totalBuffer);
        break;
    }

    for (unsigned int idx = 0; idx < length; idx ++) {
        totalBuffer[offset+idx] = buffer[idx];
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    this->eraseFlashBlock(block);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    this->writeFlashBlock(block, totalBuffer, totalLength);

    delete [] totalBuffer;
}

int32_t UdbProgrammer::getProgress() {
    return progress;
}

long UdbProgrammer::writeData(unsigned char * buffer, unsigned long length) {
    long wlen = length;
    if (eptBulkout->XferData((PUCHAR)buffer, wlen) == false) {
        //  logStream << "writeData\n\terror" << endl;
        /*! \todo FCON manage error */
        eptBulkout->Abort();
    }

    return wlen;
}

long UdbProgrammer::readData(unsigned char * buffer, unsigned long length) {
    long rlen = length;
    if (eptBulkin->XferData((PUCHAR)buffer, rlen, nullptr, true) == false) {
        //  logStream << "readData\n\terror" << endl;
        /*! \todo FCON manage error */
        eptBulkin->Abort();
    }

    return rlen;
}

void UdbProgrammer::readFlashBlock(UdbUtils::FlashBlock_t block, char * buffer) {
    //  logStream << "readFlashBlock";

    unsigned int startAddress = 0;
    unsigned int length = 0;

    switch (block) {
    case UdbUtils::BlockFX3:
        startAddress = UDB_FX3_ADDRESS;
        length = UDB_FX3_SIZE;
        break;

    case UdbUtils::BlockInfo:
        startAddress = UDB_INFO_ADDRESS;
        length = UDB_INFO_SIZE;
        break;

    case UdbUtils::BlockFPGA:
        startAddress = UDB_FPGA_ADDRESS;
        length = UDB_FPGA_SIZE;
        break;
    }

    if (UdbUtils::getFwStatus(dev) != UdbUtils::fwStatusConfigMode) {
        UdbUtils::switchToConfigMode(dev);
        UdbUtils::resetBulkEndpoints(eptBulkin, eptBulkout);
    }

    UdbUtils::disableFlashHybridSectors(dev);

    UdbUtils::readFlash(dev, startAddress, length);

    //  logStream << "\n\treading ";
    this->readData((unsigned char *)buffer, length);

    //  logStream << "done\n";
    //  logStream.flush();
}

void UdbProgrammer::eraseFlashBlock(UdbUtils::FlashBlock_t block) {
    //  logStream << "eraseFlashBlock";
    unsigned char flashStatus;

    unsigned int startAddress = 0;
    unsigned int endAddress = 0;
    unsigned int length = 0;

    switch (block) {
    case UdbUtils::BlockFX3:
        startAddress = UDB_FX3_ADDRESS;
        length = UDB_FX3_SIZE;
        endAddress = startAddress+length;
        break;

    case UdbUtils::BlockInfo:
        startAddress = UDB_INFO_ADDRESS;
        length = UDB_INFO_SIZE;
        endAddress = startAddress+length;
        break;

    case UdbUtils::BlockFPGA:
        startAddress = UDB_FPGA_ADDRESS;
        length = UDB_FPGA_SIZE;
        endAddress = startAddress+length;
        break;
    }

    if (UdbUtils::getFwStatus(dev) != UdbUtils::fwStatusConfigMode) {
        UdbUtils::switchToConfigMode(dev);
        UdbUtils::resetBulkEndpoints(eptBulkin, eptBulkout);
    }

    UdbUtils::disableFlashHybridSectors(dev);

    progress = 0;

    for (unsigned int idx = 0; idx < length; idx += UDB_SECTOR_SIZE) {
        //  logStream << "\n\terasing " << idx << " / " << length << "\n";
        flashStatus = UdbUtils::getFlashStatus(dev);
        if ((flashStatus & UDB_FLASH_STATUS_WRITE_ENABLE_MASK) == 0) {
            UdbUtils::enableFlashWrite(dev);
            flashStatus = UdbUtils::getFlashStatus(dev);
            if ((flashStatus & UDB_FLASH_STATUS_WRITE_ENABLE_MASK) == 0) {
                //  logStream << "\n\terror" << endl;
            }
        }
        UdbUtils::eraseFlashSector(dev, startAddress+idx);

        progress = (int32_t)((50*(idx+UDB_SECTOR_SIZE))/length);
        progress = (progress > 50 ? 50 : progress);

        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            flashStatus = UdbUtils::getFlashStatus(dev);
        } while ((flashStatus & UDB_FLASH_STATUS_WRITING_MASK) != 0);
    }
    //  logStream.flush();
}

void UdbProgrammer::writeFlashBlock(UdbUtils::FlashBlock_t block, char * buffer, unsigned int length) {
    //  logStream << "programFlashBlock";

    unsigned int startAddress = 0;
    unsigned int writeSize = 1024;
    int flashWriteLen;

    switch (block) {
    case UdbUtils::BlockFX3:
        startAddress = UDB_FX3_ADDRESS;
        break;

    case UdbUtils::BlockInfo:
        startAddress = UDB_INFO_ADDRESS;
        break;

    case UdbUtils::BlockFPGA:
        startAddress = UDB_FPGA_ADDRESS;
        break;
    }

    if (UdbUtils::getFwStatus(dev) != UdbUtils::fwStatusConfigMode) {
        UdbUtils::switchToConfigMode(dev);
        UdbUtils::resetBulkEndpoints(eptBulkin, eptBulkout);
    }

    //  logStream << "\n\tsize " << length << "\n";

    UdbUtils::writeFlash(dev, startAddress, length);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    eptBulkout->SetXferSize(eptBulkout->MaxPktSize);

    progress = 0;

    for (unsigned int idx = 0; idx < length;) {
        if (length-idx >= writeSize) {
            flashWriteLen = writeData(((unsigned char *)buffer)+idx, writeSize);

        } else {
            flashWriteLen = writeData(((unsigned char *)buffer)+idx, length-idx);
        }

        if (flashWriteLen == -1) {
            //  logStream << "\n\terror" << endl;
            return;
        }
        idx += flashWriteLen;

        progress = 50+(int32_t)((50*(idx+UDB_SECTOR_SIZE))/length);
        progress = (progress > 100 ? 100 : progress);

        if (idx % UDB_SECTOR_SIZE == 0) {
            //  logStream << "\n\t" << idx << " / " << length << "\n";
        }
    }

    //  logStream << "\n\t" << length << " / " << length << endl;
}
