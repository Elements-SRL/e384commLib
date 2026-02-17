#include "tomlcalibrationmanager.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
namespace e384CommLib {
#endif

std::string getParamName(CalibrationTypes_t type) {
    switch(type) {
    case CalTypesVcGainAdc:
        return "gains";

    case CalTypesVcOffsetAdc:
        return "offsets";

    case CalTypesVcGainDac:
        return "gains";

    case CalTypesVcGainODac:
        return "gains";

    case CalTypesVcOffsetDac:
        return "offsets";

    case CalTypesRsCorrOffsetDac:
        return "offsets";

    case CalTypesRShuntConductance:
        return "offsets";

    case CalTypesCcGainAdc:
        return "gains";

    case CalTypesCcOffsetAdc:
        return "offsets";

    case CalTypesCcGainDac:
        return "gains";

    case CalTypesCcOffsetDac:
        return "offsets";
    }
    return "";
}

std::string getParamUnit(CalibrationTypes_t type) {
    switch(type) {
    case CalTypesVcGainAdc:
        return "";

    case CalTypesVcOffsetAdc:
        return "A";

    case CalTypesVcGainDac:
        return "";

    case CalTypesVcGainODac:
        return "";

    case CalTypesVcOffsetDac:
        return "V";

    case CalTypesRsCorrOffsetDac:
        return "V";

    case CalTypesRShuntConductance:
        return "S";

    case CalTypesCcGainAdc:
        return "";

    case CalTypesCcOffsetAdc:
        return "V";

    case CalTypesCcGainDac:
        return "";

    case CalTypesCcOffsetDac:
        return "A";
    }
    return "";
}

std::string getParamKey(CalibrationTypes_t type) {
    switch(type) {
    case CalTypesVcGainAdc:
        return "current_adc";

    case CalTypesVcOffsetAdc:
        return "current_adc";

    case CalTypesVcGainDac:
        return "voltage_dac";

    case CalTypesVcGainODac:
        return "odac";

    case CalTypesVcOffsetDac:
        return "voltage_dac";

    case CalTypesRsCorrOffsetDac:
        return "rs_correction";

    case CalTypesRShuntConductance:
        return "shunt_resistance";

    case CalTypesCcGainAdc:
        return "voltage_adc";

    case CalTypesCcOffsetAdc:
        return "voltage_adc";

    case CalTypesCcGainDac:
        return "current_dac";

    case CalTypesCcOffsetDac:
        return "current_dac";
    }
    return "";
}

TomlCalibrationManager::TomlCalibrationManager(std::string serialNumber,
                                               uint16_t currentChannelsNum,
                                               uint16_t boardsNum,
                                               uint16_t vcCurrentRangesNum,
                                               uint16_t vcVoltageRangesNum,
                                               uint16_t ccVoltageRangesNum,
                                               uint16_t ccCurrentRangesNum,
                                               uint16_t samplingRatesNum,
                                               uint16_t samplingRateModesNum) :
    serialNumber(serialNumber),
    currentChannelsNum(currentChannelsNum),
    boardsNum(boardsNum),
    vcCurrentRangesNum(vcCurrentRangesNum),
    vcVoltageRangesNum(vcVoltageRangesNum),
    ccVoltageRangesNum(ccVoltageRangesNum),
    ccCurrentRangesNum(ccCurrentRangesNum),
    samplingRatesNum(samplingRatesNum),
    samplingRateModesNum(samplingRateModesNum) {

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

std::string TomlCalibrationManager::getCalibrationFileName() {
    return calibrationFileName;
}

bool TomlCalibrationManager::getCalibrationFilesOkFlag() {
    return calibrationFilesOkFlag;
}

bool TomlCalibrationManager::loadCalibrationFile() {
    auto result = toml::parse_file(calibrationFileName);

    if (!result) {
        status = ErrorCalibrationFileCorrupted;
        return false;
    }

    auto tbl = std::move(result).table();

    if (!tbl.contains("sampling_rates")) {
        this->loadDefaultSamplingRatesModeMapping();

    } else {
        this->loadSamplingRatesModeMapping(tbl["sampling_rates"]);
    }

    if (!tbl.contains("boards")) {
        status = ErrorCalibrationFileCorrupted;
        return false;
    }

    if (tbl["boards"].as_array()->size() != boardsNum) {
        status = ErrorCalibrationFileCorrupted;
        return false;
    }

    for (uint32_t boardIdx = 0; boardIdx < boardsNum; boardIdx++) {
        auto boardNode = tbl["boards"][boardIdx];

        for (int typeInt = 0; typeInt < CalTypesNum; typeInt++) {
            const auto type = (CalibrationTypes_t)typeInt;
            const auto key = getParamKey(type);
            if (boardNode.as_table()->contains(key)) { /*! Check if the field current_adc exists */
                loadSetOfParams(type, boardNode[key], boardIdx, calibrationParams.types[type]); /*! try to load ADC */
            }
        }
    }

    return true;
}

void TomlCalibrationManager::loadDefaultSamplingRatesModeMapping() {
    for (int srIdx = 0; srIdx < samplingRateModesNum; srIdx++) {
        srModeMapping[srIdx] = 0;
    }
}

bool TomlCalibrationManager::loadSamplingRatesModeMapping(toml::node_view <toml::node> node) {
    int modeNum = node.as_array()->size();
    int srOffset = 0;
    for (int modeIdx = 0; modeIdx < modeNum; modeIdx++) {
        auto modeNode = node[modeIdx];
        if (!modeNode.as_table()->contains("values")) {
            this->loadDefaultSamplingRatesModeMapping();
            return false;
        }
        int srNum = modeNode["values"].as_array()->size();
        for (int srIdx = 0; srIdx < srNum; srIdx++) {
            srModeMapping[srOffset++] = modeIdx;
        }
    }
    if (srOffset != samplingRatesNum) {
        this->loadDefaultSamplingRatesModeMapping();
        return false;
    }
    return true;
}

void TomlCalibrationManager::loadDefaultParams() {
    Measurement_t one = {1.0, UnitPfxNone, ""};
    Measurement_t zeroV = {0.0, UnitPfxNone, "V"};
    Measurement_t zeroA = {0.0, UnitPfxNone, "A"};
    Measurement_t zeroS = {0.0, UnitPfxNone, "S"};

    calibrationParams.initialize(CalTypesVcGainAdc, samplingRateModesNum, vcCurrentRangesNum, currentChannelsNum, one);
    calibrationParams.initialize(CalTypesVcOffsetAdc, samplingRateModesNum, vcCurrentRangesNum, currentChannelsNum, zeroA);

    calibrationParams.initialize(CalTypesVcGainDac, 1, vcVoltageRangesNum, currentChannelsNum, one);
    calibrationParams.initialize(CalTypesVcGainODac, 1, 1, currentChannelsNum, one);
    calibrationParams.initialize(CalTypesVcOffsetDac, 1, vcVoltageRangesNum, currentChannelsNum, zeroV);

    calibrationParams.initialize(CalTypesRsCorrOffsetDac, 1, vcCurrentRangesNum, currentChannelsNum, zeroV);

    calibrationParams.initialize(CalTypesRShuntConductance, 1, vcCurrentRangesNum, currentChannelsNum, zeroS);

    calibrationParams.initialize(CalTypesCcGainAdc, samplingRateModesNum, ccVoltageRangesNum, currentChannelsNum, one);
    calibrationParams.initialize(CalTypesCcOffsetAdc, samplingRateModesNum, ccVoltageRangesNum, currentChannelsNum, zeroV);

    calibrationParams.initialize(CalTypesCcGainDac, 1, ccCurrentRangesNum, currentChannelsNum, one);
    calibrationParams.initialize(CalTypesCcOffsetDac, 1, ccCurrentRangesNum, currentChannelsNum, zeroA);
}

bool TomlCalibrationManager::loadSetOfParams(CalibrationTypes_t type, toml::node_view <toml::node> node, uint32_t boardIdx,
                                             CalibrationSamplingModes_t &outParams) {
    bool ret = true;
    if (node.as_array()->size() < outParams.modes[0].ranges.size()) {
        ret = false;
    }
    for (uint32_t rangeIdx = 0; rangeIdx < outParams.modes[0].ranges.size() && ret; rangeIdx++) {
        auto rangeNode = node[rangeIdx];
        if (!rangeNode.as_table()->contains("sampling_rates")) {
            ret = false;
            break;
        }

        if (rangeNode["sampling_rates"].as_array()->size() < outParams.modes.size()) {
            ret = false;
            break;
        }

        for (uint32_t srIdx = 0; srIdx < outParams.modes.size() && ret; srIdx++) {
            auto srNode = rangeNode["sampling_rates"][srIdx];

            if (!srNode.as_table()->contains("calibrations")) {
                ret = false;
                break;
            }

            if (!srNode["calibrations"].as_table()->contains(getParamName(type))) {
                ret = false;
                break;
            }

            auto offsetsArray = srNode["calibrations"][getParamName(type)].as_array();

            if (offsetsArray->size() < channelsPerBoard) {
                ret = false;
                break;
            }
            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                outParams.modes[srIdx].ranges[rangeIdx].channels[idx+boardIdx*channelsPerBoard] = {offsetsArray->get(idx)->value_or(0.0), UnitPfxNone, getParamUnit(type)};
            }
        }
    }

    return ret;
}

#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif
