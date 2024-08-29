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

        if (!boardNode.as_table()->contains("current_adc")) { /*! Check if the field current_adc exists */
            this->loadVcAdc(boardNode, boardIdx, true); /*! otherwise load default params */

        } else if (!this->loadVcAdc(boardNode, boardIdx)) { /*! try to load ADC */
            this->loadVcAdc(boardNode, boardIdx, true); /*! otherwise load default params */
        }

        if (!boardNode.as_table()->contains("voltage_dac")) { /*! Check if the field voltage_dac exists */
            this->loadVcDac(boardNode, boardIdx, true); /*! otherwise load default params */

        } else if (!this->loadVcDac(boardNode, boardIdx)) { /*! try to load DAC */
            this->loadVcDac(boardNode, boardIdx, true); /*! otherwise load default params */
        }

        if (!boardNode.as_table()->contains("rs_correction")) { /*! Check if the field rs_correction exists */
            this->loadRsCorrOffset(boardNode, boardIdx, true); /*! otherwise load default params */

        } else if (!this->loadRsCorrOffset(boardNode, boardIdx)) { /*! try to load offsets */
            this->loadRsCorrOffset(boardNode, boardIdx, true); /*! otherwise load default params */
        }

        if (!boardNode.as_table()->contains("shunt_resistance")) { /*! Check if the field shunt_resistance exists */
            this->loadRShuntConductance(boardNode, boardIdx, true); /*! otherwise load default params */

        } else if (!this->loadRShuntConductance(boardNode, boardIdx)) { /*! try to load offsets */
            this->loadRShuntConductance(boardNode, boardIdx, true); /*! otherwise load default params */
        }

        if (!boardNode.as_table()->contains("voltage_adc")) { /*! Check if the field voltage_adc exists */
            this->loadCcAdc(boardNode, boardIdx, true); /*! otherwise load default params */

        } else if (!this->loadCcAdc(boardNode, boardIdx)) { /*! try to load ADC */
            this->loadCcAdc(boardNode, boardIdx, true); /*! otherwise load default params */
        }

        if (!boardNode.as_table()->contains("current_dac")) { /*! Check if the field current_dac exists */
            this->loadCcDac(boardNode, boardIdx, true); /*! otherwise load default params */

        } else if (!this->loadCcDac(boardNode, boardIdx)) { /*! try to load DAC */
            this->loadCcDac(boardNode, boardIdx, true); /*! otherwise load default params */
        }
    }

    return true;
}

void TomlCalibrationManager::loadDefaultSamplingRatesModeMapping() {
    for (int srIdx = 0; srIdx < samplingRatesNum; srIdx++) {
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

    calibrationParams.vcGainAdc.resize(samplingRatesNum);
    calibrationParams.vcOffsetAdc.resize(samplingRatesNum);
    for (uint32_t srIdx = 0; srIdx < samplingRatesNum; srIdx++) {
        calibrationParams.vcGainAdc[srIdx].resize(vcCurrentRangesNum);
        calibrationParams.vcOffsetAdc[srIdx].resize(vcCurrentRangesNum);
        for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
            calibrationParams.vcGainAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            calibrationParams.vcOffsetAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            std::fill(calibrationParams.vcGainAdc[srIdx][rangeIdx].begin(), calibrationParams.vcGainAdc[srIdx][rangeIdx].end(), one);
            std::fill(calibrationParams.vcOffsetAdc[srIdx][rangeIdx].begin(), calibrationParams.vcOffsetAdc[srIdx][rangeIdx].end(), zeroA);
        }
    }

    calibrationParams.vcGainDac.resize(1);
    calibrationParams.vcOffsetDac.resize(1);
    calibrationParams.vcGainDac[0].resize(vcVoltageRangesNum);
    calibrationParams.vcOffsetDac[0].resize(vcVoltageRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcVoltageRangesNum; rangeIdx++) {
        calibrationParams.vcGainDac[0][rangeIdx].resize(currentChannelsNum);
        calibrationParams.vcOffsetDac[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.vcGainDac[0][rangeIdx].begin(), calibrationParams.vcGainDac[0][rangeIdx].end(), one);
        std::fill(calibrationParams.vcOffsetDac[0][rangeIdx].begin(), calibrationParams.vcOffsetDac[0][rangeIdx].end(), zeroV);
    }

    calibrationParams.rsCorrOffsetDac.resize(1);
    calibrationParams.rsCorrOffsetDac[0].resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.rsCorrOffsetDac[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.rsCorrOffsetDac[0][rangeIdx].begin(), calibrationParams.rsCorrOffsetDac[0][rangeIdx].end(), zeroV);
    }

    calibrationParams.rShuntConductance.resize(1);
    calibrationParams.rShuntConductance[0].resize(vcCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < vcCurrentRangesNum; rangeIdx++) {
        calibrationParams.rShuntConductance[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.rShuntConductance[0][rangeIdx].begin(), calibrationParams.rShuntConductance[0][rangeIdx].end(), zeroS);
    }

    calibrationParams.ccGainAdc.resize(samplingRatesNum);
    calibrationParams.ccOffsetAdc.resize(samplingRatesNum);
    for (uint32_t srIdx = 0; srIdx < samplingRatesNum; srIdx++) {
        calibrationParams.ccGainAdc[srIdx].resize(ccVoltageRangesNum);
        calibrationParams.ccOffsetAdc[srIdx].resize(ccVoltageRangesNum);
        for (uint32_t rangeIdx = 0; rangeIdx < ccVoltageRangesNum; rangeIdx++) {
            calibrationParams.ccGainAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            calibrationParams.ccOffsetAdc[srIdx][rangeIdx].resize(currentChannelsNum);
            std::fill(calibrationParams.ccGainAdc[srIdx][rangeIdx].begin(), calibrationParams.ccGainAdc[srIdx][rangeIdx].end(), one);
            std::fill(calibrationParams.ccOffsetAdc[srIdx][rangeIdx].begin(), calibrationParams.ccOffsetAdc[srIdx][rangeIdx].end(), zeroV);
        }
    }

    calibrationParams.ccGainDac.resize(1);
    calibrationParams.ccOffsetDac.resize(1);
    calibrationParams.ccGainDac[0].resize(ccCurrentRangesNum);
    calibrationParams.ccOffsetDac[0].resize(ccCurrentRangesNum);
    for (uint32_t rangeIdx = 0; rangeIdx < ccCurrentRangesNum; rangeIdx++) {
        calibrationParams.ccGainDac[0][rangeIdx].resize(currentChannelsNum);
        calibrationParams.ccOffsetDac[0][rangeIdx].resize(currentChannelsNum);
        std::fill(calibrationParams.ccGainDac[0][rangeIdx].begin(), calibrationParams.ccGainDac[0][rangeIdx].end(), one);
        std::fill(calibrationParams.ccOffsetDac[0][rangeIdx].begin(), calibrationParams.ccOffsetDac[0][rangeIdx].end(), zeroA);
    }
}

bool TomlCalibrationManager::loadVcAdc(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, calibrationParams.vcGainAdc, calibrationParams.vcOffsetAdc, "A");
        ret = true;

    } else {
        ret = loadSetOfParams(node["current_adc"], boardIdx, calibrationParams.vcGainAdc, calibrationParams.vcOffsetAdc, "A");
    }
    return ret;
}

bool TomlCalibrationManager::loadVcDac(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, calibrationParams.vcGainDac, calibrationParams.vcOffsetDac, "V");
        ret = true;

    } else {
        ret = loadSetOfParams(node["voltage_dac"], boardIdx, calibrationParams.vcGainDac, calibrationParams.vcOffsetDac, "V");
    }
    return ret;
}

bool TomlCalibrationManager::loadRsCorrOffset(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultOffsets(boardIdx, calibrationParams.rsCorrOffsetDac, "V");
        ret = true;

    } else {
        ret = loadSetOfOffsets(node["rs_correction"], boardIdx, calibrationParams.rsCorrOffsetDac, "V");
    }
    return ret;
}

bool TomlCalibrationManager::loadRShuntConductance(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultOffsets(boardIdx, calibrationParams.rShuntConductance, "S");
        ret = true;

    } else {
        ret = loadSetOfOffsets(node["shunt_resistance"], boardIdx, calibrationParams.rShuntConductance, "S");
    }
    return ret;
}

bool TomlCalibrationManager::loadCcAdc(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, calibrationParams.ccGainAdc, calibrationParams.ccOffsetAdc, "V");
        ret = true;

    } else {
        ret = loadSetOfParams(node["voltage_adc"], boardIdx, calibrationParams.ccGainAdc, calibrationParams.ccGainAdc, "V");
    }
    return ret;
}

bool TomlCalibrationManager::loadCcDac(toml::node_view <toml::node> node, uint32_t boardIdx, bool defaultFlag) {
    bool ret;
    if (defaultFlag) {
        this->loadSetOfDefaultParams(boardIdx, calibrationParams.ccGainDac, calibrationParams.ccOffsetDac, "A");
        ret = true;

    } else {
        ret = loadSetOfParams(node["current_dac"], boardIdx, calibrationParams.ccGainDac, calibrationParams.ccGainDac, "A");
    }
    return ret;
}

bool TomlCalibrationManager::loadSetOfParams(toml::node_view <toml::node> node, uint32_t boardIdx, std::vector <std::vector <std::vector <Measurement_t> > > &outGains, std::vector <std::vector <std::vector <Measurement_t> > > &outOffsets, std::string offsetUnit) {
    bool ret = true;
    if (node.as_array()->size() < outGains[0].size()) {
        ret = false;
    }
    for (uint32_t rangeIdx = 0; rangeIdx < outGains[0].size() && ret; rangeIdx++) {
        auto rangeNode = node[rangeIdx];
        if (!rangeNode.as_table()->contains("sampling_rates")) {
            ret = false;
            break;
        }

        for (uint32_t srIdx = 0; srIdx < outGains.size() && ret; srIdx++) {
            if (rangeNode["sampling_rates"].as_array()->size() <= srModeMapping[srIdx]) {
                ret = false;
                break;
            }

            auto srNode = rangeNode["sampling_rates"][srModeMapping[srIdx]];

            if (!srNode.as_table()->contains("calibrations")) {
                ret = false;
                break;
            }

            if (!(srNode["calibrations"].as_table()->contains("gains") && srNode["calibrations"].as_table()->contains("offsets"))) {
                ret = false;
                break;
            }

            auto gainsArray = srNode["calibrations"]["gains"].as_array();
            auto offsetsArray = srNode["calibrations"]["offsets"].as_array();

            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                outGains[srIdx][rangeIdx][idx+boardIdx*channelsPerBoard] = {gainsArray->get(0)->value_or(1.0), UnitPfxNone, ""};
                outOffsets[srIdx][rangeIdx][idx+boardIdx*channelsPerBoard] = {offsetsArray->get(0)->value_or(0.0), UnitPfxNone, offsetUnit};
            }
        }
    }

    if (!ret) {
        this->loadSetOfDefaultParams(boardIdx, outGains, outOffsets, offsetUnit);
    }

    return ret;
}

void TomlCalibrationManager::loadSetOfDefaultParams(uint32_t boardIdx, std::vector <std::vector <std::vector <Measurement_t> > > &outGains, std::vector <std::vector <std::vector <Measurement_t> > > &outOffsets, std::string offsetUnit) {
    for (uint32_t srIdx = 0; srIdx < outGains.size(); srIdx++) {
        for (uint32_t rangeIdx = 0; rangeIdx < outGains[srIdx].size(); rangeIdx++) {
            Measurement_t one = {1.0, UnitPfxNone, ""};
            Measurement_t zero = {0.0, UnitPfxNone, offsetUnit};
            std::fill(outGains[srIdx][rangeIdx].begin()+boardIdx*channelsPerBoard, outGains[srIdx][rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, one);
            std::fill(outOffsets[srIdx][rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[srIdx][rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
        }
    }
}

bool TomlCalibrationManager::loadSetOfOffsets(toml::node_view <toml::node> node, uint32_t boardIdx, std::vector <std::vector <std::vector <Measurement_t> > > &outOffsets, std::string offsetUnit) {
    bool ret = true;
    if (node.as_array()->size() < outOffsets[0].size()) {
        ret = false;
    }
    for (uint32_t rangeIdx = 0; rangeIdx < outOffsets[0].size() && ret; rangeIdx++) {
        auto rangeNode = node[rangeIdx];
        if (!rangeNode.as_table()->contains("sampling_rates")) {
            ret = false;
            break;
        }

        if (rangeNode["sampling_rates"].as_array()->size() < outOffsets.size()) {
            ret = false;
            break;
        }

        for (uint32_t srIdx = 0; srIdx < outOffsets.size() && ret; srIdx++) {
            auto srNode = rangeNode["sampling_rates"][srIdx];

            if (!srNode.as_table()->contains("calibrations")) {
                ret = false;
                break;
            }

            if (!srNode["calibrations"].as_table()->contains("offsets")) {
                ret = false;
                break;
            }

            auto offsetsArray = srNode["calibrations"]["offsets"].as_array();

            for (uint32_t idx = 0; idx < channelsPerBoard; idx++) {
                outOffsets[srIdx][rangeIdx][idx+boardIdx*channelsPerBoard] = {offsetsArray->get(0)->value_or(0.0), UnitPfxNone, offsetUnit};
            }
        }
    }

    if (!ret) {
        this->loadSetOfDefaultOffsets(boardIdx, outOffsets, offsetUnit);
    }

    return ret;
}

void TomlCalibrationManager::loadSetOfDefaultOffsets(uint32_t boardIdx, std::vector <std::vector <std::vector <Measurement_t> > > &outOffsets, std::string offsetUnit) {
    for (uint32_t srIdx = 0; srIdx < outOffsets.size(); srIdx++) {
        for (uint32_t rangeIdx = 0; rangeIdx < outOffsets[srIdx].size(); rangeIdx++) {
            Measurement_t zero = {0.0, UnitPfxNone, offsetUnit};
            std::fill(outOffsets[srIdx][rangeIdx].begin()+boardIdx*channelsPerBoard, outOffsets[srIdx][rangeIdx].begin()+(boardIdx+1)*channelsPerBoard, zero);
        }
    }
}

#ifndef E384COMMLIB_LABVIEW_WRAPPER
}
#endif
