#include "tomlcalibrationmanager.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif
TomlCalibrationManager::TomlCalibrationManager(std::string serialNumber,
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

    calibrationFileName = TOML_CAL_ROOT_FOLDER + serialNumber + UTL_SEPARATOR + "calibration_file.toml";

    channelsPerBoard = currentChannelsNum/boardsNum;

    this->loadDefaultParams();
}

CalibrationParams_t TomlCalibrationManager::getCalibrationParams(ErrorCodes_t &error) {
    status = Success;
    calibrationFilesOkFlag = this->loadCalibrationFile();

    error = status;
    return calibrationParams;
}

//std::string getCalibrationFileName();
//bool getCalibrationFilesOkFlag();

bool TomlCalibrationManager::loadCalibrationFile() {
    bool ret;

    auto result = toml::parse_file(calibrationFileName);

    if (!result) {
        status = ErrorCalibrationFileCorrupted;
        return false;
    }

    auto tbl = std::move(result).table();

    if (!tbl.contains("boards")) {
        status = ErrorCalibrationFileCorrupted;
        return false;
    }

    if (tbl["boards"].as_array()->size() != boardsNum) {
        status = ErrorCalibrationFileCorrupted;
        return false;
    }

    for (uint32_t boardIdx = 0; boardIdx < boardsNum; boardIdx++) {
        auto boardCal = tbl["boards"][boardIdx];



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
}

return rets;
}
//void loadDefaultParams();
//bool loadVcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadVcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadRsCorrOffset(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadRShuntConductance(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadCcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadCcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadSetOfParams(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outGains, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);
//void loadSetOfDefaultParams(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outGains, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);
//bool loadSetOfOffsets(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);
//void loadSetOfDefaultOffsets(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);

#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif
