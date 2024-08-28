#ifndef TOMLCALIBRATIONMANAGER_H
#define TOMLCALIBRATIONMANAGER_H

#include <vector>

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
                           uint16_t samplingRatesNum);

    CalibrationParams_t getCalibrationParams(ErrorCodes_t &error);
    std::string getCalibrationFileName();
    bool getCalibrationFilesOkFlag();

private:
    bool loadCalibrationFile();
    void loadDefaultParams();
    bool loadVcAdc(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag = false);
    bool loadVcDac(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag = false);
    bool loadRsCorrOffset(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag = false);
    bool loadRShuntConductance(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag = false);
    bool loadCcAdc(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag = false);
    bool loadCcDac(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag = false);
    bool loadSetOfParams(toml::node_view <toml::node> node, uint32_t boardIdx,
                         std::vector <std::vector <std::vector <Measurement_t> > > &outGains,
                         std::vector <std::vector <std::vector <Measurement_t> > > &outOffsets, std::string offsetUnit);
    void loadSetOfDefaultParams(uint32_t boardIdx,
                                std::vector <std::vector <std::vector <Measurement_t> > > &outGains,
                                std::vector <std::vector <std::vector <Measurement_t> > > &outOffsets, std::string offsetUnit);
    bool loadSetOfOffsets(toml::node_view <toml::node> node, uint32_t boardIdx,
                          std::vector <std::vector <std::vector <Measurement_t> > > &outOffsets, std::string offsetUnit);
    void loadSetOfDefaultOffsets(uint32_t boardIdx,
                                 std::vector <std::vector <std::vector <Measurement_t>  >> &outOffsets, std::string offsetUnit);

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

    std::string calibrationFileName;

    bool calibrationFilesOkFlag;

    std::vector <int> correctBoardsNumbering;

    CalibrationParams_t calibrationParams;
};
#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif

#endif // TOMLCALIBRATIONMANAGER_H
