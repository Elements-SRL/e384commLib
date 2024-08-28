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

    channelsPerBoard = currentChannelsNum/boardsNum;

    this->loadDefaultParams();
}

CalibrationParams_t TomlCalibrationManager::getCalibrationParams(ErrorCodes_t &error) {
    status = Success;
    calibrationFilesOkFlag = this->loadCalibrationFiles();

    error = status;
    return calibrationParams;
}

//std::string getCalibrationFileName();
//bool getCalibrationFilesOkFlag();

//private:
//bool loadCalibrationFile();
//void loadDefaultParams();
//bool loadVcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadVcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadRsCorrOffset(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadRShuntConductance(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadCcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
//bool loadCcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);

#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif
