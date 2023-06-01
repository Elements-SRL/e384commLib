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
    for (auto path : calibrationFileNames) {
        path.resize(2);
    }

    mappingFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + CAL_MAPPING_FILE_NAME;
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
    for (uint32_t boardIdx = 0; boardIdx < boardsNum; boardIdx++) {
        std::string vcCalibrationFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + ".csv";
        vcCalibrationFileStreams[boardIdx].open(vcCalibrationFilePath, std::ios::in);
        if (vcCalibrationFileStreams[boardIdx].is_open()) {
            this->discardCsvLine(vcCalibrationFileStreams[boardIdx]);
            this->loadVcAdc(vcCalibrationFileStreams[boardIdx]);







            vcCalibrationFileStreams[boardIdx].close();
            rets[0].push_back(true);

        } else {
            rets[0].push_back(false);
        }

        std::string ccCalibrationFilePath = CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + calibrationFileNames[boardIdx][1] + "_cc.csv";
    }

    return rets;
}

bool CalibrationManager::loadVcAdc(std::fstream &stream) {
    bool ret;
    std::vector <std::vector <std::string>> strings;
    strings.resize(2);
    strings[0].resize(channelsPerBoard);
    strings[1].resize(channelsPerBoard);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        this->discardCsvLine(stream);
        ret = this->readCsvPortion(stream, strings);
        if (ret) {
            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                calibrationParams.allGainAdcMeas[rangeIdx][idx] = {std::stod(strings[0][idx]), e384CommLib::UnitPfxNone, ""};
                calibrationParams.allOffsetAdcMeas[rangeIdx][idx] = {std::stod(strings[1][idx]), e384CommLib::UnitPfxNone, "V"};
            }

        } else {
//            std::fill(calibrationParams.allGainAdcMeas[rangeIdx].begin(), calibrationParams.allGainAdcMeas[rangeIdx].end(), {1.0, e384CommLib::UnitPfxNone, ""});
        }


    }
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

    while (getline(stream, line) && rowsIdx < rowsNum) {
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

//bool CalibrationManager::readCsvPortion(std::fstream &stream, std::vector <std::vector <double>> &out) {
//    std::vector <std::vector <std::string>> in;
//    in.resize(out.size());
//    for (uint32_t idx = 0; idx < out.size(); idx++) {
//        in[idx].resize(out[idx].size());
//    }
//    readCsvPortion(stream, in);
//    for (uint32_t rowIdx = 0; rowIdx < out.size(); rowIdx++) {
//        for (uint32_t rowIdx = 0; rowIdx < out.size(); rowIdx++) {
//    }
//}
