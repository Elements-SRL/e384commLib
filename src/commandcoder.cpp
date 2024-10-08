#include "commandcoder.h"

CommandCoder::CommandCoder(uint16_t initialWord, uint16_t initialBit, uint16_t bitsNum):
    initialWord(initialWord),
    bitsNum(bitsNum) {

    /*! +CMC_BITS_PER_WORD-1 is used to make a ceil rounding of the division result,
     *  even if we're left with a single bit we need a word to handle it */
    wordsNum = (bitsNum+initialBit+CMC_BITS_PER_WORD-1)/CMC_BITS_PER_WORD;
    bitOffsets.resize(wordsNum);
    bitMasks.resize(wordsNum);
    uint16_t bitsInNthWord;
    for (uint16_t wordIdx = 0; wordIdx < wordsNum; wordIdx++) {
        bitOffsets[wordIdx] = initialBit;
        bitsInNthWord = (bitsNum+initialBit < CMC_BITS_PER_WORD ? bitsNum+initialBit : CMC_BITS_PER_WORD)-initialBit;
        bitMasks[wordIdx] = ((1 << bitsInNthWord)-1) << initialBit;
        bitsNum -= bitsInNthWord;
        initialBit = 0;
    }
}

void CommandCoder::encodeUint(uint32_t uintValue, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    if (startingWord > initialWord) {
        startingWord = initialWord;
    }

    if (endingWord < initialWord+wordsNum) {
        endingWord = initialWord+wordsNum;
    }

    for (uint16_t wordIdx = 0; wordIdx < wordsNum; wordIdx++) {
        encodingWords[wordIdx+initialWord] &= ~bitMasks[wordIdx];
        encodingWords[wordIdx+initialWord] |= (uintValue << bitOffsets[wordIdx]) & bitMasks[wordIdx];
        uintValue >>= (CMC_BITS_PER_WORD-bitOffsets[wordIdx]);
    }
}

BoolCoder::BoolCoder(CoderConfig_t config) :
    CommandCoder(config.initialWord, config.initialBit, config.bitsNum),
    config(config) {

}

BoolArrayCoder::BoolArrayCoder(CoderConfig_t config) :
    BoolCoder(config) {

}

void BoolArrayCoder::encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    this->encodeUint(value, encodingWords, startingWord, endingWord);
}

BoolNegatedArrayCoder::BoolNegatedArrayCoder(CoderConfig_t config) :
    BoolArrayCoder(config) {

}

void BoolNegatedArrayCoder::encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    this->encodeUint(~value, encodingWords, startingWord, endingWord);
}

BoolRandomArrayCoder::BoolRandomArrayCoder(CoderConfig_t config) :
    BoolArrayCoder(config) {

    tos.resize(0);
    toNum = 0;
}

void BoolRandomArrayCoder::encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    this->encodeUint(this->map(value), encodingWords, startingWord, endingWord);
}

void BoolRandomArrayCoder::addMapItem(uint32_t to) {
    tos.push_back(to);
    toNum++;
}

uint32_t BoolRandomArrayCoder::map(uint32_t from) {
    if (from >= toNum) {
        from = 0;
    }
    return tos[from];
}

BoolOneHotCoder::BoolOneHotCoder(CoderConfig_t config) :
    BoolCoder(config) {

}

void BoolOneHotCoder::encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    this->encodeUint(1 << value, encodingWords, startingWord, endingWord);
}

DoubleCoder::DoubleCoder(CoderConfig_t config) :
    CommandCoder(config.initialWord, config.initialBit, config.bitsNum),
    config(config),
    resolution(config.resolution),
    minValue(config.minValue),
    maxValue(config.maxValue) {

    if (maxValue < minValue) {
        double tempValue = minValue;
        minValue = maxValue;
        maxValue = tempValue;
        invertedScale = true;
    }

    if (resolution < 0.0) {
        resolution = -resolution;
        invertedScale = true;
    }
}

double DoubleCoder::clip(double value) {
    return (value > maxValue ? maxValue : (value < minValue ? minValue : value));
}

DoubleTwosCompCoder::DoubleTwosCompCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

double DoubleTwosCompCoder::encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    if (!invertedScale) {
        value = this->clip(value);
        int32_t intValue = (int32_t)round(value/resolution);
        this->encodeUint((uint32_t)intValue, encodingWords, startingWord, endingWord);
        return resolution*(double)intValue;

    } else {
        value = this->clip(-value);
        int32_t intValue = (int32_t)round(value/resolution);
        this->encodeUint((uint32_t)intValue, encodingWords, startingWord, endingWord);
        return -resolution*(double)intValue;
    }
}

DoubleOffsetBinaryCoder::DoubleOffsetBinaryCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

double DoubleOffsetBinaryCoder::encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    if (!invertedScale) {
        value = this->clip(value);
        uint32_t uintValue = (uint32_t)round((value-minValue)/resolution);
        this->encodeUint(uintValue, encodingWords, startingWord, endingWord);
        return minValue+resolution*(double)uintValue;

    } else {
        value = this->clip(-value);
        uint32_t uintValue = (uint32_t)round((value+maxValue)/resolution);
        this->encodeUint(uintValue, encodingWords, startingWord, endingWord);
        return maxValue-resolution*(double)uintValue;
    }
}

DoubleSignAbsCoder::DoubleSignAbsCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

double DoubleSignAbsCoder::encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    if (!invertedScale) {
        value = this->clip(value);
        uint32_t uintValue = (uint32_t)round(fabs(value)/resolution);
        uint32_t signValue = (value < 0.0 ? 1 << (bitsNum-1) : 0);
        this->encodeUint(uintValue+signValue, encodingWords, startingWord, endingWord);
        if (value < 0.0) {
            return -resolution*(double)uintValue;

        } else {
            return resolution*(double)uintValue;
        }

    } else {
        value = this->clip(-value);
        uint32_t uintValue = (uint32_t)round(fabs(value)/resolution);
        uint32_t signValue = (value < 0.0 ? 1 << (bitsNum-1) : 0);
        this->encodeUint(uintValue+signValue, encodingWords, startingWord, endingWord);
        if (value < 0.0) {
            return resolution*(double)uintValue;

        } else {
            return -resolution*(double)uintValue;
        }
    }
}

FloatCoder::FloatCoder(CoderConfig_t config) :
    CommandCoder(config.initialWord, config.initialBit, 32),
    config(config) {

}

double FloatCoder::encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    float fltValue = (float)value;
    uint32_t uintValue = *(uint32_t *)(&fltValue);
    this->encodeUint(uintValue, encodingWords, startingWord, endingWord);
    return (double)fltValue;
}

MultiCoder::MultiCoder(MultiCoderConfig_t multiConfig) :
    CommandCoder(0, 0, 0),
    multiConfig(multiConfig) {

    rangesNum = multiConfig.thresholdVector.size();
}

double MultiCoder::encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    double ret;
    if (rangeSelectedFlag) {
        multiConfig.boolCoder->encode(selectedRange, encodingWords, startingWord, endingWord);
        ret = multiConfig.doubleCoderVector[selectedRange]->encode(value, encodingWords, startingWord, endingWord);

    } else {
        bool done = false;
        int i;
        for (i = 0; i < rangesNum; i++) {
            if (value < multiConfig.thresholdVector[i] && !done) {
                multiConfig.boolCoder->encode(i, encodingWords, startingWord, endingWord);
                ret = multiConfig.doubleCoderVector[i]->encode(value, encodingWords, startingWord, endingWord);
                done = true;
            }
        }
        if (!done) {
            multiConfig.boolCoder->encode(i, encodingWords, startingWord, endingWord);
            ret = multiConfig.doubleCoderVector[i]->encode(value, encodingWords, startingWord, endingWord);
            done = true;
        }
    }
    return ret;
}

void MultiCoder::setEncodingRange(int rangeIdx) {
    if (rangeIdx < 0 || rangeIdx > rangesNum) {
        rangeSelectedFlag = false;

    } else {
        rangeSelectedFlag = true;
        selectedRange = rangeIdx;
    }
}

void MultiCoder::getMultiConfig(MultiCoderConfig_t &multiConfig) {
    multiConfig = this->multiConfig;
}
