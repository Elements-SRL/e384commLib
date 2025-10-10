#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <iostream>

#define UTL_SEPARATOR "\\\\"
#define UTL_DEFAULT_FW_PATH (std::string("FW") + UTL_SEPARATOR)
#define UTL_DEFAULT_FX3_FW_VERSION (4)
#define UTL_DEFAULT_FX3_FW_NAME (std::string("UDB-FX3_V04.img"))
#define UTL_DEMO_FILE_PATH ""
#define GLB_HERE { std::cout<<__FILE__<<__LINE__<<std::endl; }

typedef enum {
    DebugLevelDevice,
    DebugLevelRxRaw,
    DebugLevelRx,
    DebugLevelTx,
    DebugLevelTemperature,
    DebugLevelDigitallOffsetCompensation,
    DebugLevelMaxSpeed,
    DebugLevelsNum
} DebugLevels_t;

template<typename I_t> bool allLessThan(std::vector <I_t> vec, I_t maxValue) {
    if (vec.empty()) {
        return false;
    }
    I_t maxVal = *std::max_element(vec.begin(), vec.end());

    if (maxVal >= maxValue) {
        return false;
    } else {
        return true;
    }
}

template<typename I_t> bool allLessThanOrEqualTo(std::vector <I_t> vec, I_t maxValue) {
    if (vec.empty()) {
        return false;
    }
    I_t maxVal = *std::max_element(vec.begin(), vec.end());

    if (maxVal > maxValue) {
        return false;
    } else {
        return true;
    }

}

template<typename I_t> bool allGreaterThan(std::vector <I_t> vec, I_t minValue) {
    if (vec.empty()) {
        return false;
    }
    I_t minVal = *std::min_element(vec.begin(), vec.end());

    if (minVal <= minValue) {
        return false;
    } else {
        return true;
    }

}

template<typename I_t> bool allGreaterThanOrEqualTo(std::vector <I_t> vec, I_t minValue) {
    if (vec.empty()) {
        return false;
    }
    I_t minVal = *std::min_element(vec.begin(), vec.end());

    if (minVal < minValue) {
        return false;
    } else {
        return true;
    }

}

template<typename I_t> bool allStrictlyInRange(std::vector <I_t> vec, I_t minValue, I_t maxValue) {
    if (allLessThan<I_t>(vec, maxValue)) {
        if (allGreaterThan<I_t>(vec, minValue)) {
            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}

template<typename I_t> bool allInRange(std::vector <I_t> vec, I_t minValue, I_t maxValue) {
    if (allLessThanOrEqualTo<I_t>(vec, maxValue)) {
        if (allGreaterThanOrEqualTo<I_t>(vec, minValue)) {
            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}

template<typename I_t> bool inRange(I_t value, I_t minValue, I_t maxValue) {
    if (value > maxValue) {
        return false;
    }

    if (value < minValue) {
        return false;
    }

    return true;
}

inline void createDebugFile(FILE * &fid, std::string fileName) {
    const char * home = std::getenv("USERPROFILE");
    std::filesystem::path filePath = std::filesystem::path(home) / (fileName + ".txt");
    fid = fopen(filePath.string().c_str(), "wb");
}

inline bool debugLevelEnabled(DebugLevels_t level) {
    static std::vector <std::optional <bool>> cachedResult(DebugLevelsNum);
    if (!cachedResult[level].has_value()) {
        const char * home = std::getenv("USERPROFILE");
        std::string filename;
        switch (level) {
        case DebugLevelDevice:
            filename = "e384_DEMO.pls";
            break;

        case DebugLevelRxRaw:
            filename = "e384_RX_RAW.pls";
            break;

        case DebugLevelRx:
            filename = "e384_RX.pls";
            break;

        case DebugLevelTx:
            filename = "e384_TX.pls";
            break;

        case DebugLevelTemperature:
            filename = "e384_TEMP.pls";
            break;

        case DebugLevelDigitallOffsetCompensation:
            filename = "e384_DOC.pls";
            break;

        default:
            return false;
        }
        std::filesystem::path filePath = std::filesystem::path(home) / filename;
        cachedResult[level] = std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
    }
    return * cachedResult[level];
}

#endif // UTILS_H
