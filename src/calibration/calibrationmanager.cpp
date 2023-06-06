#include "calibrationmanager.h"

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

    mappingFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + CAL_MAPPING_FILE_NAME;

    this->loadDefaultParams();
}

e384CommLib::CalibrationParams_t CalibrationManager::getCalibrationParams() {
    if (this->loadMappingFile()) {
        this->loadCalibrationFiles();

//    } else {
        // Nothing to do, deafult parameters already loaded by constructor
    }
    return calibrationParams;
}

bool CalibrationManager::loadMappingFile() {
    mappingFileStream.open(mappingFilePath, std::ios::in);
    if (mappingFileStream.is_open()) {
        bool ret = this->readCsvPortion(mappingFileStream, calibrationFileNames);
        mappingFileStream.close();
        return ret;

    } else {
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
            this->loadVcAdc(vcCalibrationFileStreams[boardIdx], boardIdx);
            this->loadVcDac(vcCalibrationFileStreams[boardIdx], boardIdx);
            vcCalibrationFileStreams[boardIdx].close();
            rets[0].push_back(true);

        } else {
            rets[0].push_back(false);
        }

        std::string ccCalibrationFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_cc.csv";
        ccCalibrationFileStreams[boardIdx].open(ccCalibrationFilePath, std::ios::in);
        if (ccCalibrationFileStreams[boardIdx].is_open()) {
            this->discardCsvLine(ccCalibrationFileStreams[boardIdx]); /*! Discard line including board name */
            this->loadCcAdc(ccCalibrationFileStreams[boardIdx], boardIdx);
            this->loadCcDac(ccCalibrationFileStreams[boardIdx], boardIdx);
            ccCalibrationFileStreams[boardIdx].close();
            rets[0].push_back(true);

        } else {
            rets[0].push_back(false);
        }
    }

    return rets;
}

void CalibrationManager::loadDefaultParams() {
    e384CommLib::Measurement_t one = {1.0, e384CommLib::UnitPfxNone, ""};
    e384CommLib::Measurement_t zeroV = {0.0, e384CommLib::UnitPfxNone, "V"};
    e384CommLib::Measurement_t zeroA = {0.0, e384CommLib::UnitPfxNone, "A"};

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

bool CalibrationManager::loadVcAdc(std::fstream &stream, uint32_t boardIdx) {
    return loadSetOfParams(stream, boardIdx, vcCurrentRangesNum, calibrationParams.allGainAdcMeas, calibrationParams.allOffsetAdcMeas, "A");
}

bool CalibrationManager::loadVcDac(std::fstream &stream, uint32_t boardIdx) {
    return loadSetOfParams(stream, boardIdx, vcVoltageRangesNum, calibrationParams.allGainDacMeas, calibrationParams.allOffsetDacMeas, "V");
}

bool CalibrationManager::loadCcAdc(std::fstream &stream, uint32_t boardIdx) {
    return loadSetOfParams(stream, boardIdx, ccVoltageRangesNum, calibrationParams.ccAllGainAdcMeas, calibrationParams.ccAllOffsetAdcMeas, "V");
}

bool CalibrationManager::loadCcDac(std::fstream &stream, uint32_t boardIdx) {
    return loadSetOfParams(stream, boardIdx, ccCurrentRangesNum, calibrationParams.ccAllGainDacMeas, calibrationParams.ccAllOffsetDacMeas, "A");
}

bool CalibrationManager::loadSetOfParams(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <e384CommLib::Measurement_t>> &outGains, std::vector <std::vector <e384CommLib::Measurement_t>> &outOffsets, std::string offsetUnit) {
    bool ret = true;
    std::vector <std::vector <std::string>> strings;
    strings.resize(2);
    strings[0].resize(channelsPerBoard);
    strings[1].resize(channelsPerBoard);
    for (uint32_t rangeIdx = 0; rangeIdx < rangesNum; rangeIdx++) {
        this->discardCsvLine(stream); /*! Discard line including range name */
        ret = this->readCsvPortion(stream, strings);
        if (ret) {
            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                outGains[rangeIdx][idx+boardIdx*channelsPerBoard] = {std::stod(strings[0][idx]), e384CommLib::UnitPfxNone, ""};
                outOffsets[rangeIdx][idx+boardIdx*channelsPerBoard] = {std::stod(strings[1][idx]), e384CommLib::UnitPfxNone, offsetUnit};
            }

        } else {
            e384CommLib::Measurement_t one = {1.0, e384CommLib::UnitPfxNone, ""};
            e384CommLib::Measurement_t zeroA = {0.0, e384CommLib::UnitPfxNone, offsetUnit};
            std::fill(outGains[rangeIdx].begin()+boardIdx*channelsPerBoard, outGains[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, one);
            std::fill(outOffsets[rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zeroA);
            ret = false;
        }
    }
    return ret;
}

bool CalibrationManager::readCsvPortion(std::fstream &stream, std::vector <std::vector <std::string>> &out) {
    uint32_t rowsNum = out.size();
    uint32_t colsNum = 0;
    if (rowsNum > 0) {
        colsNum = out[0].size();
        if (colsNum == 0) {
            return false;
        }

    } else {
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
            return false;
        }

        rowsIdx++;
    }
    if (rowsIdx < rowsNum) {
        return false;

    } else {
        return true;
    }
}

void CalibrationManager::discardCsvLine(std::fstream &stream) {
    std::string word;
    getline(stream, word);
}
