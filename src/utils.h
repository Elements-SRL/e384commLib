#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#define UTL_SEPARATOR "\\\\"

template<typename I_t> bool allLessThan(std::vector<I_t> myVector, I_t maxValue){
    if(myVector.empty()){
        return false;
    }
    //auto iter = std::max_element(myVector.begin(), myVector.end());
    I_t myMax = *std::max_element(myVector.begin(), myVector.end());

    if(myMax >= maxValue){
        return false;
    } else {
        return true;
    }

}

template<typename I_t> bool allLessThanOrEqualTo(std::vector<I_t> myVector, I_t maxValue){
    if(myVector.empty()){
        return false;
    }
    //auto iter = std::max_element(myVector.begin(), myVector.end());
    I_t myMax = *std::max_element(myVector.begin(), myVector.end());

    if(myMax > maxValue){
        return false;
    } else {
        return true;
    }

}

template<typename I_t> bool allGreaterThan(std::vector<I_t> myVector, I_t minValue){
    if(myVector.empty()){
        return false;
    }
    //auto iter = std::max_element(myVector.begin(), myVector.end());
    I_t myMin = *std::min_element(myVector.begin(), myVector.end());

    if(myMin <= minValue){
        return false;
    } else {
        return true;
    }

}

template<typename I_t> bool allGreaterThanOrEqualTo(std::vector<I_t> myVector, I_t minValue){
    if(myVector.empty()){
        return false;
    }
    //auto iter = std::max_element(myVector.begin(), myVector.end());
    I_t myMin = *std::min_element(myVector.begin(), myVector.end());

    if(myMin < minValue){
        return false;
    } else {
        return true;
    }

}

template<typename I_t> bool allStrictlyInRange(std::vector<I_t> myVector, I_t minValue, I_t maxValue){
    if (allLessThan<I_t>(myVector, maxValue)) {
        if (allGreaterThan<I_t>(myVector, minValue)) {
            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}

template<typename I_t> bool allInRange(std::vector<I_t> myVector, I_t minValue, I_t maxValue){
    if (allLessThanOrEqualTo<I_t>(myVector, maxValue)) {
        if (allGreaterThanOrEqualTo<I_t>(myVector, minValue)) {
            return true;

        } else {
            return false;
        }

    } else {
        return false;
    }
}

static void createDebugFile(FILE * &fid, std::string fileName) {
#ifdef _WIN32
    std::string path = std::string(getenv("HOMEDRIVE"))+std::string(getenv("HOMEPATH"));
#else
    std::string path = std::string(getenv("HOME"));
#endif
    std::stringstream ss;

    for (size_t i = 0; i < path.length(); ++i) {
        if (path[i] == '\\') {
            ss << "\\\\";

        } else {
            ss << path[i];
        }
    }
#ifdef _WIN32
    ss << "\\\\" << fileName << ".txt";
#else
    ss << "/e384CommLib_tx.txt";
#endif

    fid = fopen(ss.str().c_str(), "wb");
}

#endif // UTILS_H
