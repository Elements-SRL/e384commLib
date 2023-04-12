#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

template<typename I_t> bool areAllTheVectorElementsLessThan(std::vector<I_t> myVector, I_t maxValue){
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

template<typename I_t> bool areAllTheVectorElementsGreaterThan(std::vector<I_t> myVector, I_t minValue){
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

template<typename I_t> bool areAllTheVectorElementsInRange(std::vector<I_t> myVector, I_t minValue, I_t maxValue){
    if (areAllTheVectorElementsLessThan<I_t>(myVector, maxValue)) {
        if (areAllTheVectorElementsGreaterThan<I_t>(myVector, minValue)) {
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
    string path = string(getenv("HOME"));
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
