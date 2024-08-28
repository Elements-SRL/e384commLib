#include "calibrationmanager.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif
CsvCalibrationManager::CsvCalibrationManager(std::string serialNumber,
                                       uint16_t currentChannelsNum,
                                       uint16_t boardsNum,
                                       uint16_t vcCurrentRangesNum,
                                       uint16_t vcVoltageRangesNum,
                                       uint16_t ccVoltageRangesNum,
                                       uint16_t ccCurrentRangesNum,
                                       uint16_t samplingRatesNum) :
    serialNumber(serialNumber),
    currentChannelsNum(currentChannelsNum),
    boardsNum(boardsNum),
    vcCurrentRangesNum(vcCurrentRangesNum),
    vcVoltageRangesNum(vcVoltageRangesNum),
    ccVoltageRangesNum(ccVoltageRangesNum),
    ccCurrentRangesNum(ccCurrentRangesNum),
    samplingRatesNum(samplingRatesNum) {

    channelsPerBoard = currentChannelsNum/boardsNum;
    calibrationFileNames.resize(boardsNum);
    for (uint16_t idx = 0; idx < calibrationFileNames.size(); idx++) {
        calibrationFileNames[idx].resize(2);
    }

    mappingFileDir = CSV_CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR;
    mappingFilePath = mappingFileDir + CSV_CAL_MAPPING_FILE_NAME;

    for (int i = 1; i <= boardsNum; i++) {
       correctBoardsNumbering.push_back(i);
    }

    this->loadDefaultParams();
}

CalibrationParams_t CsvCalibrationManager::getCalibrationParams(ErrorCodes_t &error) {
    status = Success;
    if (this->loadMappingFile()) {
        calibrationFilesOkFlags = this->loadCalibrationFiles();

//    } else {
        // Nothing to do, deafult parameters already loaded by constructor
    }
    error = status;
    return calibrationParams;
}

std::vector <std::string> CsvCalibrationManager::getCalibrationFileNames() {
    std::vector <std::string> calibFileNames;
    for (int i = 0; i < calibrationFileNames.size(); i++) {
        calibFileNames.push_back(calibrationFileNames[i][1]);
    }
    return calibFileNames;
}

std::vector <std::vector <bool>> CsvCalibrationManager::getCalibrationFilesOkFlags() {
    /*! First vector has 2 items, one for Voltage clamp one for Current clamp
     *  Inner vectors have one item per board
     *  True means the file is found and ok
     *  False means the file is missing or corrupted and default values have been loaded */
    return calibrationFilesOkFlags;
}

bool CsvCalibrationManager::loadMappingFile() {
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

std::string CsvCalibrationManager::getMappingFileDir() {
    return this->mappingFileDir;
}

std::string CsvCalibrationManager::getMappingFilePath() {
    return this->mappingFilePath;
}

std::vector <std::vector <bool>> CsvCalibrationManager::loadCalibrationFiles() {
    std::vector <std::vector <bool> > rets;
    rets.resize(2);
    vcCalibrationFileStreams.resize(boardsNum);
    rsCorrCalibrationFileStreams.resize(boardsNum);
    rShuntCalibrationFileStreams.resize(boardsNum);
    ccCalibrationFileStreams.resize(boardsNum);
    for (uint32_t boardIdx = 0; boardIdx < boardsNum; boardIdx++) {
        std::string vcCalibrationFilePath = CSV_CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + ".csv";
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

        std::string rsCorrCalibrationFilePath = CSV_CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_rs.csv";
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

        std::string rShuntCalibrationFilePath = CSV_CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_sh.csv";
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

        std::string ccCalibrationFilePath = CSV_CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_cc.csv";
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

void CsvCalibrationManager::loadDefaultParams() {
    Measurement_t one = {1.0, UnitPfxNone, ""};
    Measurement_t zeroV = {0.0, UnitPfxNone, "V"};
    Measurement_t zeroA = {0.0, UnitPfxNone, "A"};
    Measurement_t zeroS = {0.0, UnitPfxNone, "S"};

    calibrationParams.vcGainAdc.resize(samplingRatesNum);
    calibrationParams.vcOffsetAdc.resize(samplingRatesNum);
    for (uint32_t srIdx = 0; srIdx < samplingRatesNum; srIdx++) {
        calibrationParams.vcGainAdc[srIdx].resize(vcCurrentRangesNum);
        calibrationParams.vcOffsetAdc[srIdx].resize(vcCurrentRangesNum);
        for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
            calibrationParams.vcGainAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            calibrationParams.vcOffsetAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            std::fill(calibrationParams.vcGainAdc[srIdx][rangeIdx].begin(), calibrationParams.vcGainAdc[srIdx][rangeIdx].end(), one);
            std::fill(calibrationParams.vcOffsetAdc[srIdx][rangeIdx].begin(), calibrationParams.vcOffsetAdc[srIdx][rangeIdx].end(), zeroA);
        }
    }

    calibrationParams.vcGainDac.resize(1);
    calibrationParams.vcOffsetDac.resize(1);
    calibrationParams.vcGainDac[0].resize(vcVoltageRangesNum);
    calibrationParams.vcOffsetDac[0].resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        calibrationParams.vcGainDac[0][rangeIdx].resize(currentChannelsNum);
        calibrationParams.vcOffsetDac[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.vcGainDac[0][rangeIdx].begin(), calibrationParams.vcGainDac[0][rangeIdx].end(), one);
        std::fill(calibrationParams.vcOffsetDac[0][rangeIdx].begin(), calibrationParams.vcOffsetDac[0][rangeIdx].end(), zeroV);
    }

    calibrationParams.rsCorrOffsetDac.resize(1);
    calibrationParams.rsCorrOffsetDac[0].resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.rsCorrOffsetDac[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.rsCorrOffsetDac[0][rangeIdx].begin(), calibrationParams.rsCorrOffsetDac[0][rangeIdx].end(), zeroV);
    }

    calibrationParams.rShuntConductance.resize(1);
    calibrationParams.rShuntConductance[0].resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.rShuntConductance[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.rShuntConductance[0][rangeIdx].begin(), calibrationParams.rShuntConductance[0][rangeIdx].end(), zeroS);
    }

    calibrationParams.ccGainAdc.resize(samplingRatesNum);
    calibrationParams.ccOffsetAdc.resize(samplingRatesNum);
    for (uint32_t srIdx = 0; srIdx < samplingRatesNum; srIdx++) {
        calibrationParams.ccGainAdc[srIdx].resize(ccVoltageRangesNum);
        calibrationParams.ccOffsetAdc[srIdx].resize(ccVoltageRangesNum);
        for (uint32_t rangeIdx = 0; rangeIdx < ccVoltageRangesNum; rangeIdx++) {
            calibrationParams.ccGainAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            calibrationParams.ccOffsetAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            std::fill(calibrationParams.ccGainAdc[srIdx][rangeIdx].begin(), calibrationParams.ccGainAdc[srIdx][rangeIdx].end(), one);
            std::fill(calibrationParams.ccOffsetAdc[srIdx][rangeIdx].begin(), calibrationParams.ccOffsetAdc[srIdx][rangeIdx].end(), zeroV);
        }
    }

    calibrationParams.ccGainDac.resize(1);
    calibrationParams.ccOffsetDac.resize(1);
    calibrationParams.ccGainDac[0].resize(ccCurrentRangesNum);
    calibrationParams.ccOffsetDac[0].resize(ccCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccCurrentRangesNum; rangeIdx++) {
        calibrationParams.ccGainDac[0][rangeIdx].resize(currentChannelsNum);
        calibrationParams.ccOffsetDac[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.ccGainDac[0][rangeIdx].begin(), calibrationParams.ccGainDac[0][rangeIdx].end(), one);
        std::fill(calibrationParams.ccOffsetDac[0][rangeIdx].begin(), calibrationParams.ccOffsetDac[0][rangeIdx].end(), zeroA);
    }
}

bool CsvCalibrationManager::loadVcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, vcCurrentRangesNum, calibrationParams.vcGainAdc[0], calibrationParams.vcOffsetAdc[0], "A");
        ret = true;

    } else {
        ret = loadSetOfParams(stream, boardIdx, vcCurrentRangesNum, calibrationParams.vcGainAdc[0], calibrationParams.vcOffsetAdc[0], "A");
    }

    if (ret) {
        for (uint16_t srIdx = 1; srIdx < samplingRatesNum; srIdx++) {
            calibrationParams.vcGainAdc[srIdx] = calibrationParams.vcGainAdc[0];
            calibrationParams.vcOffsetAdc[srIdx] = calibrationParams.vcOffsetAdc[0];
        }
    }
    return ret;
}

bool CsvCalibrationManager::loadVcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, vcVoltageRangesNum, calibrationParams.vcGainDac[0], calibrationParams.vcOffsetDac[0], "V");
        ret = true;

    } else {
        ret = loadSetOfParams(stream, boardIdx, vcVoltageRangesNum, calibrationParams.vcGainDac[0], calibrationParams.vcOffsetDac[0], "V");
    }
    return ret;
}

bool CsvCalibrationManager::loadRsCorrOffset(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultOffsets(boardIdx, vcCurrentRangesNum, calibrationParams.rsCorrOffsetDac[0], "V");
        ret = true;

    } else {
        ret = loadSetOfOffsets(stream, boardIdx, vcCurrentRangesNum, calibrationParams.rsCorrOffsetDac[0], "V");
    }
    return ret;
}

bool CsvCalibrationManager::loadRShuntConductance(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultMeas(boardIdx, vcCurrentRangesNum, calibrationParams.rShuntConductance[0], "S");
        ret = true;

    } else {
        ret = loadSetOfMeas(stream, boardIdx, vcCurrentRangesNum, calibrationParams.rShuntConductance[0], "S");
    }
    return ret;
}

bool CsvCalibrationManager::loadCcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, ccVoltageRangesNum, calibrationParams.ccGainAdc[0], calibrationParams.ccOffsetAdc[0], "V");
        ret = true;

    } else {
        ret = loadSetOfParams(stream, boardIdx, ccVoltageRangesNum, calibrationParams.ccGainAdc[0], calibrationParams.ccOffsetAdc[0], "V");
    }

    if (ret) {
        for (uint16_t srIdx = 1; srIdx < samplingRatesNum; srIdx++) {
            calibrationParams.ccGainAdc[srIdx] = calibrationParams.ccGainAdc[0];
            calibrationParams.ccOffsetAdc[srIdx] = calibrationParams.ccOffsetAdc[0];
        }
    }
    return ret;
}

bool CsvCalibrationManager::loadCcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, ccCurrentRangesNum, calibrationParams.ccGainDac[0], calibrationParams.ccOffsetDac[0], "A");
        ret = true;

    } else {
        ret = loadSetOfParams(stream, boardIdx, ccCurrentRangesNum, calibrationParams.ccGainDac[0], calibrationParams.ccOffsetDac[0], "A");
    }
    return ret;
}

bool CsvCalibrationManager::loadSetOfParams(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outGains, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit) {
    bool ret = true;
    std::vector <std::vector <std::string> > strings;
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

void CsvCalibrationManager::loadSetOfDefaultParams(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outGains, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit) {
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
        Measurement_t one = {1.0, UnitPfxNone, ""};
        Measurement_t zero = {0.0, UnitPfxNone, offsetUnit};
        std::fill(outGains[rangeIdx].begin()+boardIdx*channelsPerBoard, outGains[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, one);
        std::fill(outOffsets[rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
    }
}

bool CsvCalibrationManager::loadSetOfOffsets(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit) {
    bool ret = true;
    std::vector <std::vector <std::string> > strings;
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

void CsvCalibrationManager::loadSetOfDefaultOffsets(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit) {
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
        Measurement_t zero = {0.0, UnitPfxNone, offsetUnit};
        std::fill(outOffsets[rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
    }
}

bool CsvCalibrationManager::loadSetOfMeas(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outMeas, std::string unit) {
    bool ret = true;
    std::vector <std::vector <std::string> > strings;
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

void CsvCalibrationManager::loadSetOfDefaultMeas(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outMeas, std::string unit) {
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
        Measurement_t zero = {0.0, UnitPfxNone, unit};
        std::fill(outMeas[rangeIdx].begin()+boardIdx*channelsPerBoard, outMeas[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
    }
}

bool CsvCalibrationManager::readCsvPortion(std::fstream &stream, std::vector <std::vector <std::string> > &out) {
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

void CsvCalibrationManager::discardCsvLine(std::fstream &stream) {
    std::string word;
    getline(stream, word);
}

#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif
