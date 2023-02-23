#ifndef UTILS_H
#define UTILS_H

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




#endif // UTILS_H
