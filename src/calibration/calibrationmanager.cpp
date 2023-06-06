#include "calibrationmanager.h"

namespace e384CommLib {
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

    std::string mappingFileDir = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR;
    mappingFilePath = mappingFileDir + CAL_MAPPING_FILE_NAME;

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
    std::vector<std::string> calibFileNames;
    for(int i = 0; i < calibrationFileNames.size(); i++){
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
            int boardNumber = std::stod(calibrationFileNames[idx][0]);
            if (boardNumber < 0 || boardNumber > boardsNum) {
                status = ErrorCalibrationMappingNotOpened;
                ret = false;
            }
        }

        mappingFileStream.close();
        return ret;

    } else {
        status = ErrorCalibrationMappingNotOpened;
        return false;
    }
}

std::vector <std::vector <bool>> CalibrationManager::loadCalibrationFiles() {
    std::vector <std::vector <bool>> rets;
    rets.resize(2);
    vcCalibrationFileStreams.resize(boardsNum);
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

    calibrationParams.allGainAdcMeas.resize(vcCurrentRangesNum);
    calibrationParams.allOffsetAdcMeas.resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.allGainAdcMeas[rangeIdx].resize(currentChannelsNum);
        calibrationParams.allOffsetAdcMeas[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.allGainAdcMeas[rangeIdx].begin(), calibrationParams.allGainAdcMeas[rangeIdx].end(), one);
        std::fill(calibrationParams.allOffsetAdcMeas[rangeIdx].begin(), calibrationParams.allOffsetAdcMeas[rangeIdx].end(), zeroA);
    }

    calibrationParams.allGainDacMeas.resize(vcVoltageRangesNum);
    calibrationParams.allOffsetDacMeas.resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        calibrationParams.allGainDacMeas[rangeIdx].resize(currentChannelsNum);
        calibrationParams.allOffsetDacMeas[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.allGainDacMeas[rangeIdx].begin(), calibrationParams.allGainDacMeas[rangeIdx].end(), one);
        std::fill(calibrationParams.allOffsetDacMeas[rangeIdx].begin(), calibrationParams.allOffsetDacMeas[rangeIdx].end(), zeroV);
    }

    calibrationParams.ccAllGainAdcMeas.resize(ccVoltageRangesNum);
    calibrationParams.ccAllOffsetAdcMeas.resize(ccVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccVoltageRangesNum; rangeIdx++) {
        calibrationParams.ccAllGainAdcMeas[rangeIdx].resize(currentChannelsNum);
        calibrationParams.ccAllOffsetAdcMeas[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.ccAllGainAdcMeas[rangeIdx].begin(), calibrationParams.ccAllGainAdcMeas[rangeIdx].end(), one);
        std::fill(calibrationParams.ccAllOffsetAdcMeas[rangeIdx].begin(), calibrationParams.ccAllOffsetAdcMeas[rangeIdx].end(), zeroV);
    }

    calibrationParams.ccAllGainDacMeas.resize(ccCurrentRangesNum);
    calibrationParams.ccAllOffsetDacMeas.resize(ccCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccCurrentRangesNum; rangeIdx++) {
        calibrationParams.ccAllGainDacMeas[rangeIdx].resize(currentChannelsNum);
        calibrationParams.ccAllOffsetDacMeas[rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.ccAllGainDacMeas[rangeIdx].begin(), calibrationParams.ccAllGainDacMeas[rangeIdx].end(), one);
        std::fill(calibrationParams.ccAllOffsetDacMeas[rangeIdx].begin(), calibrationParams.ccAllOffsetDacMeas[rangeIdx].end(), zeroA);
    }
}

bool CalibrationManager::loadVcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, vcCurrentRangesNum, calibrationParams.allGainAdcMeas, calibrationParams.allOffsetAdcMeas, "A");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, vcCurrentRangesNum, calibrationParams.allGainAdcMeas, calibrationParams.allOffsetAdcMeas, "A");
    }
}

bool CalibrationManager::loadVcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, vcVoltageRangesNum, calibrationParams.allGainDacMeas, calibrationParams.allOffsetDacMeas, "V");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, vcVoltageRangesNum, calibrationParams.allGainDacMeas, calibrationParams.allOffsetDacMeas, "V");
    }
}

bool CalibrationManager::loadCcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, ccVoltageRangesNum, calibrationParams.ccAllGainAdcMeas, calibrationParams.ccAllOffsetAdcMeas, "V");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, ccVoltageRangesNum, calibrationParams.ccAllGainAdcMeas, calibrationParams.ccAllOffsetAdcMeas, "V");
    }
}

bool CalibrationManager::loadCcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag) {
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, ccCurrentRangesNum, calibrationParams.ccAllGainDacMeas, calibrationParams.ccAllOffsetDacMeas, "A");
        return true;

    } else {
        return loadSetOfParams(stream, boardIdx, ccCurrentRangesNum, calibrationParams.ccAllGainDacMeas, calibrationParams.ccAllOffsetDacMeas, "A");
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
        Measurement_t zeroA = {0.0, UnitPfxNone, offsetUnit};
        std::fill(outGains[rangeIdx].begin()+boardIdx*channelsPerBoard, outGains[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, one);
        std::fill(outOffsets[rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zeroA);
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
}
