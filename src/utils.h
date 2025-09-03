#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <optional>

#define UTL_SEPARATOR "\\\\"
#define UTL_DEFAULT_FW_PATH (std::string("FW") + UTL_SEPARATOR)
#define UTL_DEFAULT_FX3_FW_VERSION (4)
#define UTL_DEFAULT_FX3_FW_NAME (std::string("UDB-FX3_V04.img"))
#define UTL_DEMO_FILE_PATH ""

template<typename I_t> bool allLessThan(std::vector <I_t> vec, I_t val) {
    if (vec.empty()) {
        return false;
    }
    I_t maxVal = *std::max_element(vec.begin(), vec.end());

    if (maxVal >= val) {
        return false;
    } else {
        return true;
    }
}

template<typename I_t> bool allLessThanOrEqualTo(std::vector <I_t> vec, I_t val) {
    if (vec.empty()) {
        return false;
    }
    I_t maxVal = *std::max_element(vec.begin(), vec.end());

    if (maxVal > val) {
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

template<typename I_t> bool allStrictlyInRange(std::vector <I_t> vec, I_t minValue, I_t val) {
    if (allLessThan<I_t>(vec, val)) {
        if (allGreaterThan<I_t>(vec, minValue)) {
            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}

template<typename I_t> bool allInRange(std::vector <I_t> vec, I_t minValue, I_t val) {
    if (allLessThanOrEqualTo<I_t>(vec, val)) {
        if (allGreaterThanOrEqualTo<I_t>(vec, minValue)) {
            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}

template<typename I_t> bool inRange(I_t value, I_t minValue, I_t val) {
    if (value > val) {
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

inline bool demoDevicesEnabled() {
    static std::optional <bool> cachedResult;
    if (!cachedResult.has_value()) {
        const char * home = std::getenv("USERPROFILE");
        std::filesystem::path filePath = std::filesystem::path(home) / (std::string("e384_DEMO.pls"));
        cachedResult = std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath);
    }
    return * cachedResult;
}

#endif // UTILS_H
