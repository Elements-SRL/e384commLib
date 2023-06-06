#ifndef CALIBRATIONMANAGER_H
#define CALIBRATIONMANAGER_H

#include <vector>
#include <fstream>

#include "e384commlib_global_addendum.h"
#include "utils.h"

#define CAL_ROOT_FOLDER (std::string("C:") + UTL_SEPARATOR + "EMCR_calib_folder" + UTL_SEPARATOR)
#define CAL_MAPPING_FILE_NAME "boardMapping.csv"

class CalibrationManager {
public:
    CalibrationManager(std::string serialNumber, uint16_t currentChannelsNum, uint16_t boardsNum, uint16_t vcCurrentRangesNum, uint16_t vcVoltageRangesNum, uint16_t ccVoltageRangesNum, uint16_t ccCurrentRangesNum);

    e384CommLib::CalibrationParams_t getCalibrationParams();

private:
    bool loadMappingFile();
    std::vector <std::vector <bool>> loadCalibrationFiles();
    void loadDefaultParams();
    bool loadVcAdc(std::fstream &stream, uint32_t boardIdx);
    bool loadVcDac(std::fstream &stream, uint32_t boardIdx);
    bool loadCcAdc(std::fstream &stream, uint32_t boardIdx);
    bool loadCcDac(std::fstream &stream, uint32_t boardIdx);
    bool loadSetOfParams(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <e384CommLib::Measurement_t>> &outGains, std::vector <std::vector <e384CommLib::Measurement_t>> &outOffsets, std::string offsetUnit);

    bool readCsvPortion(std::fstream &stream, std::vector <std::vector <std::string>> &out);
    void discardCsvLine(std::fstream &stream);

    std::string serialNumber;
    uint16_t currentChannelsNum;
    uint16_t boardsNum;
    uint16_t channelsPerBoard;

    uint16_t vcCurrentRangesNum;
    uint16_t vcVoltageRangesNum;
    uint16_t ccVoltageRangesNum;
    uint16_t ccCurrentRangesNum;

    std::string mappingFilePath;
    std::fstream mappingFileStream;
    std::vector <std::vector <std::string>> calibrationFileNames;
    std::vector <std::fstream> vcCalibrationFileStreams;
    std::vector <std::fstream> ccCalibrationFileStreams;

    e384CommLib::CalibrationParams_t calibrationParams;
};

#endif // CALIBRATIONMANAGER_H
