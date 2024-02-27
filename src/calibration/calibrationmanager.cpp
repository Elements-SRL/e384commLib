#include "calibrationmanager.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif
CalibrationManager::CalibrationManager(std::string serialNumber, uint16_t currentChannelsNum, uint16_t boardsNum, uint16_t vcCurrentRangesNum, uint16_t vcVoltageRangesNum, uint16_t ccVoltageRangesNum, uint16_t ccCurrentRangesNum) :
    serialNumber(serialNumber),
    currentChannelsNum(currentChannelsNum),
    boardsNum(boardsNum),
    vcCurrentRangesNum(vcCurrentRangesNum),
    vcVoltageRangesNum(vcVoltageRangesNum),
    ccVoltageRangesNum(ccVoltageRangesNum),
    ccCurrentRangesNum(ccCurrentRangesNum) {

    channelsPerBoard = currentChannelsNum/boardsNum;
    calibrationFileNames.resize(boardsNum);
    for (uint16_t idx = 0; idx < calibrationFileNames.size(); idx++) {
        calibrationFileNames[idx].resize(2);
    }

    mappingFileDir = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR;
    mappingFilePath = mappingFileDir + CAL_MAPPING_FILE_NAME;

    for (int i = 1; i <= boardsNum; i++) {
       correctBoardsNumbering.push_back(i);
    }

    this->loadDefaultParams();
}

CalibrationParams_t CalibrationManager::getCalibrationParams(ErrorCodes_t &error) {
    status = Success;
    if (this->loadMappingFile()) {
        calibrationFilesOkFlags = this->loadCalibrationFiles();

//    } else {
        // Nothing to do, deafult parameters already loaded by constructor
    }
    error = status;
    return calibrationParams;
}

std::vector <std::string> CalibrationManager::getCalibrationFileNames(){
    std::vector <std::string> calibFileNames;
    for (int i = 0; i < calibrationFileNames.size(); i++) {
        calibFileNames.push_back(calibrationFileNames[i][1]);
    }
    return calibFileNames;
}

std::vector <std::vector <bool>> CalibrationManager::getCalibrationFilesOkFlags() {
    /*! First vector has 2 items, one for Voltage clamp one for Current clamp
     *  Inner vectors have one item per board
     *  True means the file is found and ok
     *  False means the file is missing or corrupted and default values have been loaded */
    return calibrationFilesOkFlags;
}

bool CalibrationManager::loadMappingFile() {
    struct stat sb;
    std::vector <int> actualBoardsNumbering;
    if (stat(mappingFileDir.c_str(), &sb) != 0) {
        status = ErrorCalibrationDirMissing;
        return false;
    }

    mappingFileStream.open(mappingFilePath, std::ios::in);
    if (mappingFileStream.is_open()) {
        bool ret = this->readCsvPortion(mappingFileStream, calibrationFileNames);
        if (!ret && status == ErrorCalibrationFileCorrupted) {
            status = ErrorCalibrationMappingCorrupted;
        }

        for (uint16_t idx = 0; idx < boardsNum; idx++) {
            std::string calibrationFileIndex = calibrationFileNames[idx][0];
            std::string calibrationFileIndexChecked;
            for (int k = 0; k < calibrationFileIndex.size(); k++) {
                if (calibrationFileIndex[k] >= '0' && calibrationFileIndex[k] <= '9') {
                    calibrationFileIndexChecked.push_back(calibrationFileIndex[k]);
                }
            }
            int boardNumber = std::stoi(calibrationFileIndexChecked);
            actualBoardsNumbering.push_back(boardNumber);
        }

        if (actualBoardsNumbering != correctBoardsNumbering) {
            status = ErrorCalibrationMappingWrongNumbering;
            ret = false;
        }

        mappingFileStream.close();
        return ret;

    } else {
        status = ErrorCalibrationMappingNotOpened;
        return false;
    }
}

std::string CalibrationManager::getMappingFileDir(){
    return this->mappingFileDir;
}

std::string CalibrationManager::getMappingFilePath(){
    return this->mappingFilePath;
}

std::vector <std::vector <bool>> CalibrationManager::loadCalibrationFiles() {
    std::vector <std::vector <bool>> rets;
    rets.resize(2);
    vcCalibrationFileStreams.resize(boardsNum);
    rsCorrCalibrationFileStreams.resize(boardsNum);
    rShuntCalibrationFileStreams.resize(boardsNum);
    ccCalibrationFileStreams.resize(boardsNum);
    for (uint32_t boardIdx = 0; boardIdx < boardsNum; boardIdx++) {
        std::string vcCalibrationFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + ".csv";
        vcCalibrationFileStreams[boardIdx].open(vcCalibrationFilePath, std::ios::in);
        if (vcCalibrationFileStreams[boardIdx].is_open()) {
            this->discardCsvLine(vcCalibrationFileStreams[boardIdx]); /*! Discard line including board name */
            bool ret = this->loadVcAdc(vcCalibrationFileStreams[boardIdx], boardIdx); /*! try to load ADC */
            if (ret) {
                ret = this->loadVcDac(vcCalibrationFileStreams[boardIdx], boardIdx); /*! try to load DAC */
            }
            vcCalibrationFileStreams[boardIdx].close();
            if (!ret) { /*! if either failed load default for both */
                this->loadVcAdc(vcCalibrationFileStreams[boardIdx], boardIdx, true);
                this->loadVcDac(vcCalibrationFileStreams[boardIdx], boardIdx, true);
            }
            rets[0].push_back(ret);

        } else {
            status = ErrorCalibrationFileMissing;
            rets[0].push_back(false);
        }

        std::string rsCorrCalibrationFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_rs.csv";
        rsCorrCalibrationFileStreams[boardIdx].open(rsCorrCalibrationFilePath, std::ios::in);
        if (rsCorrCalibrationFileStreams[boardIdx].is_open()) {
            this->discardCsvLine(rsCorrCalibrationFileStreams[boardIdx]); /*! Discard line including board name */
            bool ret = this->loadRsCorrOffset(rsCorrCalibrationFileStreams[boardIdx], boardIdx); /*! try to load offsets */
            rsCorrCalibrationFileStreams[boardIdx].close();
            if (!ret) { /*! if failed load default */
                this->loadRsCorrOffset(rsCorrCalibrationFileStreams[boardIdx], boardIdx, true);
            }
            rets[0].push_back(ret);

        } else {
            rets[0].push_back(false);
        }

        std::string rShuntCalibrationFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_sh.csv";
        rShuntCalibrationFileStreams[boardIdx].open(rShuntCalibrationFilePath, std::ios::in);
        if (rShuntCalibrationFileStreams[boardIdx].is_open()) {
            this->discardCsvLine(rShuntCalibrationFileStreams[boardIdx]); /*! Discard line including board name */
            bool ret = this->loadRShuntConductance(rShuntCalibrationFileStreams[boardIdx], boardIdx); /*! try to load offsets */
            rShuntCalibrationFileStreams[boardIdx].close();
            if (!ret) { /*! if failed load default */
                this->loadRShuntConductance(rShuntCalibrationFileStreams[boardIdx], boardIdx, true);
            }
            rets[0].push_back(ret);

        } else {
            rets[0].push_back(false);
        }

        std::string ccCalibrationFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_cc.csv";
        ccCalibrationFileStreams[boardIdx].open(ccCalibrationFilePath, std::ios::in);
        if (ccCalibrationFileStreams[boardIdx].is_open()) {
            this->discardCsvLine(ccCalibrationFileStreams[boardIdx]); /*! Discard line including board name */
            bool ret = this->loadCcAdc(ccCalibrationFileStreams[boardIdx], boardIdx); /*! try to load ADC */
            if (ret) {
                ret = this->loadCcDac(ccCalibrationFileStreams[boardIdx], boardIdx); /*! try to load DAC */
            }
            ccCalibrationFileStreams[boardIdx].close();
            if (!ret) { /*! if either failed load default for both */
                this->loadCcAdc(ccCalibrationFileStreams[boardIdx], boardIdx, true);
                this->loadCcDac(ccCalibrationFileStreams[boardIdx], boardIdx, true);
            }
            rets[1].push_back(ret);

        } else {
            status = ErrorCalibrationFileMissing;
            rets[1].push_back(false);
        }
    }

    return rets;
}

void CalibrationManager::loadDefaultParams() {
    Measurement_t one = {1.0, UnitPfxNone, ""};
    Measurement_t zeroV = {0.0, UnitPfxNone, "V"};
    Measurement_t zeroA = {0.0, UnitPfxNone, "A"};
    Measurement_t zeroS = {0.0, UnitPfxNone, "S"};

    calibrationParams.vcGainAdc.resize(vcCurrentRangesNum);
    calibrationParams.vcOffsetAdc.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.vcGainAdc[rangeIdx].resize(currentChannelsNum);
        calibrationParams.vcOffsetAdc[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.vcGainAdc[rangeIdx].begin(), calibrationParams.vcGainAdc[rangeIdx].end(), one);
        std::fill(calibrationParams.vcOffsetAdc[rangeIdx].begin(), calibrationParams.vcOffsetAdc[rangeIdx].end(), zeroA);
    }

    calibrationParams.vcGainDac.resize(vcVoltageRangesNum);
    calibrationParams.vcOffsetDac.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        calibrationParams.vcGainDac[rangeIdx].resize(currentChannelsNum);
        calibrationParams.vcOffsetDac[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.vcGainDac[rangeIdx].begin(), calibrationParams.vcGainDac[rangeIdx].end(), one);
        std::fill(calibrationParams.vcOffsetDac[rangeIdx].begin(), calibrationParams.vcOffsetDac[rangeIdx].end(), zeroV);
    }

    calibrationParams.rsCorrOffsetDac.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.rsCorrOffsetDac[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.rsCorrOffsetDac[rangeIdx].begin(), calibrationParams.rsCorrOffsetDac[rangeIdx].end(), zeroV);
    }

    calibrationParams.rShuntConductance.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.rShuntConductance[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.rShuntConductance[rangeIdx].begin(), calibrationParams.rShuntConductance[rangeIdx].end(), zeroS);
    }

    calibrationParams.ccGainAdc.resize(ccVoltageRangesNum);
    calibrationParams.ccOffsetAdc.resize(ccVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccVoltageRangesNum; rangeIdx++) {
        calibrationParams.ccGainAdc[rangeIdx].resize(currentChannelsNum);
        calibrationParams.ccOffsetAdc[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.ccGainAdc[rangeIdx].begin(), calibrationParams.ccGainAdc[rangeIdx].end(), one);
        std::fill(calibrationParams.ccOffsetAdc[rangeIdx].begin(), calibrationParams.ccOffsetAdc[rangeIdx].end(), zeroV);
    }

    calibrationParams.ccGainDac.resize(ccCurrentRangesNum);
    calibrationParams.ccOffsetDac.resize(ccCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccCurrentRangesNum; rangeIdx++) {
        calibrationParams.ccGainDac[rangeIdx].resize(currentChannelsNum);
        calibrationParams.ccOffsetDac[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.ccGainDac[rangeIdx].begin(), calibrationParams.ccGainDac[rangeIdx].end(), one);
        std::fill(calibrationParams.ccOffsetDac[rangeIdx].begin(), calibrationParams.ccOffsetDac[rangeIdx].end(), zeroA);
    }
}

bool CalibrationManager::loadVcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, vcCurrentRangesNum, calibrationParams.vcGainAdc, calibrationParams.vcOffsetAdc, "A");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, vcCurrentRangesNum, calibrationParams.vcGainAdc, calibrationParams.vcOffsetAdc, "A");
    }
}

bool CalibrationManager::loadVcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, vcVoltageRangesNum, calibrationParams.vcGainDac, calibrationParams.vcOffsetDac, "V");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, vcVoltageRangesNum, calibrationParams.vcGainDac, calibrationParams.vcOffsetDac, "V");
    }
}

bool CalibrationManager::loadRsCorrOffset(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultOffsets(boardIdx, vcCurrentRangesNum, calibrationParams.rsCorrOffsetDac, "V");
        return true;

    } else {
        return loadSetOfOffsets(stream, boardIdx, vcCurrentRangesNum, calibrationParams.rsCorrOffsetDac, "V");
    }
}

bool CalibrationManager::loadRShuntConductance(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultMeas(boardIdx, vcCurrentRangesNum, calibrationParams.rShuntConductance, "S");
        return true;

    } else {
        return loadSetOfMeas(stream, boardIdx, vcCurrentRangesNum, calibrationParams.rShuntConductance, "S");
    }
}

bool CalibrationManager::loadCcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, ccVoltageRangesNum, calibrationParams.ccGainAdc, calibrationParams.ccOffsetAdc, "V");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, ccVoltageRangesNum, calibrationParams.ccGainAdc, calibrationParams.ccOffsetAdc, "V");
    }
}

bool CalibrationManager::loadCcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, ccCurrentRangesNum, calibrationParams.ccGainDac, calibrationParams.ccOffsetDac, "A");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, ccCurrentRangesNum, calibrationParams.ccGainDac, calibrationParams.ccOffsetDac, "A");
    }
}

bool CalibrationManager::loadSetOfParams(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t>> &outGains, std::vector <std::vector <Measurement_t>> &outOffsets, std::string offsetUnit) {
    bool ret = true;
    std::vector <std::vector <std::string>> strings;
    strings.resize(2);
    strings[0].resize(channelsPerBoard);
    strings[1].resize(channelsPerBoard);
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum && ret; rangeIdx++) {
        this->discardCsvLine(stream); /*! Discard line including range name */
        ret = this->readCsvPortion(stream, strings);
        if (ret) {
            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                outGains[rangeIdx][idx+boardIdx*channelsPerBoard] = {std::stod(strings[0][idx]), UnitPfxNone, ""};
                outOffsets[rangeIdx][idx+boardIdx*channelsPerBoard] = {std::stod(strings[1][idx]), UnitPfxNone, offsetUnit};
            }
        }
    }

    if (!ret) {
        this->loadSetOfDefaultParams(boardIdx, rangesNum, outGains, outOffsets, offsetUnit);
    }

    return ret;
}

void CalibrationManager::loadSetOfDefaultParams(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t>> &outGains, std::vector <std::vector <Measurement_t>> &outOffsets, std::string offsetUnit) {
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
        Measurement_t one = {1.0, UnitPfxNone, ""};
        Measurement_t zero = {0.0, UnitPfxNone, offsetUnit};
        std::fill(outGains[rangeIdx].begin()+boardIdx*channelsPerBoard, outGains[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, one);
        std::fill(outOffsets[rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
    }
}

bool CalibrationManager::loadSetOfOffsets(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t>> &outOffsets, std::string offsetUnit) {
    bool ret = true;
    std::vector <std::vector <std::string>> strings;
    strings.resize(1);
    strings[0].resize(channelsPerBoard);
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum && ret; rangeIdx++) {
        this->discardCsvLine(stream); /*! Discard line including range name */
        ret = this->readCsvPortion(stream, strings);
        if (ret) {
            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                outOffsets[rangeIdx][idx+boardIdx*channelsPerBoard] = {std::stod(strings[0][idx]), UnitPfxNone, offsetUnit};
            }
        }
    }

    if (!ret) {
        this->loadSetOfDefaultOffsets(boardIdx, rangesNum, outOffsets, offsetUnit);
    }

    return ret;
}

void CalibrationManager::loadSetOfDefaultOffsets(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t>> &outOffsets, std::string offsetUnit) {
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
        Measurement_t zero = {0.0, UnitPfxNone, offsetUnit};
        std::fill(outOffsets[rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
    }
}

bool CalibrationManager::loadSetOfMeas(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t>> &outMeas, std::string unit) {
    bool ret = true;
    std::vector <std::vector <std::string>> strings;
    strings.resize(1);
    strings[0].resize(channelsPerBoard);
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum && ret; rangeIdx++) {
        this->discardCsvLine(stream); /*! Discard line including range name */
        ret = this->readCsvPortion(stream, strings);
        if (ret) {
            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                outMeas[rangeIdx][idx+boardIdx*channelsPerBoard] = {std::stod(strings[0][idx]), UnitPfxNone, unit};
            }
        }
    }

    if (!ret) {
        this->loadSetOfDefaultMeas(boardIdx, rangesNum, outMeas, unit);
    }

    return ret;
}

void CalibrationManager::loadSetOfDefaultMeas(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t>> &outMeas, std::string unit) {
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
        Measurement_t zero = {0.0, UnitPfxNone, unit};
        std::fill(outMeas[rangeIdx].begin()+boardIdx*channelsPerBoard, outMeas[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
    }
}

bool CalibrationManager::readCsvPortion(std::fstream &stream, std::vector <std::vector <std::string>> &out) {
    uint32_t rowsNum = out.size();
    uint32_t colsNum = 0;
    if (rowsNum > 0) {
        colsNum = out[0].size();
        if (colsNum == 0) {
            status = ErrorCalibrationSoftwareBug;
            return false;
        }

    } else {
        status = ErrorCalibrationSoftwareBug;
        return false;
    }

    std::string line;
    std::string word;

    uint32_t rowsIdx = 0;
    uint32_t colsIdx = 0;

    while (rowsIdx < rowsNum) {
        if (!getline(stream, line)) {
            break;
        }
        colsIdx = 0;

        std::stringstream str(line);

        while (getline(str, word, ',') && colsIdx < colsNum) {
            out[rowsIdx][colsIdx] = word;
            colsIdx++;
        }

        if (colsIdx < colsNum) {
            status = ErrorCalibrationFileCorrupted;
            return false;
        }

        rowsIdx++;
    }
    if (rowsIdx < rowsNum) {
        status = ErrorCalibrationFileCorrupted;
        return false;

    } else {
        return true;
    }
}

void CalibrationManager::discardCsvLine(std::fstream &stream) {
    std::string word;
    getline(stream, word);
}

#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif
