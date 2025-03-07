#ifndef TOMLCALIBRATIONMANAGER_H
#define TOMLCALIBRATIONMANAGER_H

#include "e384commlib_global_addendum.h"
#include "e384commlib_errorcodes.h"
#include "utils.h"
#include "toml.hpp"

#define TOML_CAL_ROOT_FOLDER (std::string("C:") + UTL_SEPARATOR + "EMCR_calib_folder" + UTL_SEPARATOR)

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif
class TomlCalibrationManager {
public:
    TomlCalibrationManager(std::string serialNumber,
                           uint16_t currentChannelsNum,
                           uint16_t boardsNum,
                           uint16_t vcCurrentRangesNum,
                           uint16_t vcVoltageRangesNum,
                           uint16_t ccVoltageRangesNum,
                           uint16_t ccCurrentRangesNum,
                           uint16_t samplingRatesNum,
                           uint16_t samplingRateModesNum);

    CalibrationParams_t getCalibrationParams(ErrorCodes_t &error);
    std::string getCalibrationFileName();
    bool getCalibrationFilesOkFlag();

private:
    bool loadCalibrationFile();
    void loadDefaultSamplingRatesModeMapping();
    bool loadSamplingRatesModeMapping(toml::node_view <toml::node> node);
    void loadDefaultParams();
    bool loadSetOfParams(CalibrationTypes_t type, toml::node_view <toml::node> node, uint32_t boardIdx,
                          CalibrationSamplingModes_t &outParams);

    ErrorCodes_t status = Success;

    std::string serialNumber;
    uint16_t currentChannelsNum;
    uint16_t boardsNum;
    uint16_t channelsPerBoard;

    std::map <int, int> srModeMapping;

    uint16_t vcCurrentRangesNum;
    uint16_t vcVoltageRangesNum;
    uint16_t ccVoltageRangesNum;
    uint16_t ccCurrentRangesNum;
    uint16_t samplingRatesNum;
    uint16_t samplingRateModesNum;

    std::string calibrationFileName;

    bool calibrationFilesOkFlag;

    std::vector <int> correctBoardsNumbering;

    CalibrationParams_t calibrationParams;
};
#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif

#endif // TOMLCALIBRATIONMANAGER_H
