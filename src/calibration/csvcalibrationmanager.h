#ifndef CSVCALIBRATIONMANAGER_H
#define CSVCALIBRATIONMANAGER_H

#include <vector>
#include <fstream>

#include "e384commlib_global_addendum.h"
#include "e384commlib_errorcodes.h"
#include "utils.h"

#define CSV_CAL_ROOT_FOLDER (std::string("C:") + UTL_SEPARATOR + "EMCR_calib_folder" + UTL_SEPARATOR)
#define CSV_CAL_MAPPING_FILE_NAME "boardMapping.csv"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif
class CsvCalibrationManager {
public:
    CsvCalibrationManager(std::string serialNumber,
                          uint16_t currentChannelsNum,
                          uint16_t boardsNum,
                          uint16_t vcCurrentRangesNum,
                          uint16_t vcVoltageRangesNum,
                          uint16_t ccVoltageRangesNum,
                          uint16_t ccCurrentRangesNum,
                          uint16_t samplingRatesNum);

    CalibrationParams_t getCalibrationParams(ErrorCodes_t &error);
    std::vector <std::string> getCalibrationFileNames();
    std::vector <std::vector <bool> > getCalibrationFilesOkFlags();
    std::string getMappingFileDir();
    std::string getMappingFilePath();

private:
    bool loadMappingFile();
    std::vector <std::vector <bool>> loadCalibrationFiles();
    void loadDefaultParams();
    bool loadVcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
    bool loadVcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
    bool loadRsCorrOffset(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
    bool loadRShuntConductance(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
    bool loadCcAdc(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
    bool loadCcDac(std::fstream &stream, uint32_t boardIdx, bool defaultFlag = false);
    bool loadSetOfParams(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outGains, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);
    void loadSetOfDefaultParams(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outGains, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);
    bool loadSetOfOffsets(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);
    void loadSetOfDefaultOffsets(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outOffsets, std::string offsetUnit);
    bool loadSetOfMeas(std::fstream &stream, uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outMeas, std::string unit);
    void loadSetOfDefaultMeas(uint32_t boardIdx, uint32_t rangesNum, std::vector <std::vector <Measurement_t> > &outMeas, std::string unit);

    bool readCsvPortion(std::fstream &stream, std::vector <std::vector <std::string> > &out);
    void discardCsvLine(std::fstream &stream);

    ErrorCodes_t status = Success;

    std::string serialNumber;
    uint16_t currentChannelsNum;
    uint16_t boardsNum;
    uint16_t channelsPerBoard;

    uint16_t vcCurrentRangesNum;
    uint16_t vcVoltageRangesNum;
    uint16_t ccVoltageRangesNum;
    uint16_t ccCurrentRangesNum;
    uint16_t samplingRatesNum;

    std::string mappingFileDir;
    std::string mappingFilePath;
    std::fstream mappingFileStream;
    std::vector <std::vector <std::string>> calibrationFileNames;
    std::vector <std::fstream> vcCalibrationFileStreams;
    std::vector <std::fstream> rsCorrCalibrationFileStreams;
    std::vector <std::fstream> rShuntCalibrationFileStreams;
    std::vector <std::fstream> ccCalibrationFileStreams;
    std::vector <std::vector <bool>> calibrationFilesOkFlags;

    std::vector <int> correctBoardsNumbering;

    CalibrationParams_t calibrationParams;
};
#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif

#endif // CSVCALIBRATIONMANAGER_H
