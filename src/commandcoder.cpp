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

void CommandCoder::encodeUint(uint32_t uintValue, CommandStatus_t &status) {
    /*! Ensure that the words are changed in blocks of, since the devices might accept only data in blocks of 32 bits */
    if ((initialWord & 0x0001) > 0x0000) {
        status.changedWords[initialWord-1] = true;
    }
    if (((initialWord+wordsNum) & 0x0001) > 0x0000) {
        status.changedWords[initialWord+wordsNum] = true;
    }
    status.anyChanged = true;
    for (uint16_t wordIdx = 0; wordIdx < wordsNum; wordIdx++) {
        status.encodingWords[wordIdx+initialWord] &= ~bitMasks[wordIdx];
        status.encodingWords[wordIdx+initialWord] |= (uintValue << bitOffsets[wordIdx]) & bitMasks[wordIdx];
        status.changedWords[wordIdx+initialWord] = true;
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

void BoolArrayCoder::encode(uint32_t value, CommandStatus_t &status) {
    this->encodeUint(value, status);
}

BoolNegatedArrayCoder::BoolNegatedArrayCoder(CoderConfig_t config) :
    BoolArrayCoder(config) {

}

void BoolNegatedArrayCoder::encode(uint32_t value, CommandStatus_t &status) {
    this->encodeUint(~value, status);
}

BoolRandomArrayCoder::BoolRandomArrayCoder(CoderConfig_t config) :
    BoolArrayCoder(config) {

    tos.resize(0);
    toNum = 0;
}

void BoolRandomArrayCoder::encode(uint32_t value, CommandStatus_t &status) {
    this->encodeUint(this->map(value), status);
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

void BoolOneHotCoder::encode(uint32_t value, CommandStatus_t &status) {
    this->encodeUint(1 << value, status);
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

double DoubleTwosCompCoder::encode(double value, CommandStatus_t &status) {
    if (!invertedScale) {
        value = this->clip(value);
        int32_t intValue = (int32_t)round(value/resolution);
        this->encodeUint((uint32_t)intValue, status);
        return resolution*(double)intValue;

    } else {
        value = this->clip(-value);
        int32_t intValue = (int32_t)round(value/resolution);
        this->encodeUint((uint32_t)intValue, status);
        return -resolution*(double)intValue;
    }
}

DoubleOffsetBinaryCoder::DoubleOffsetBinaryCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

double DoubleOffsetBinaryCoder::encode(double value, CommandStatus_t &status) {
    if (!invertedScale) {
        value = this->clip(value);
        uint32_t uintValue = (uint32_t)round((value-minValue)/resolution);
        this->encodeUint(uintValue, status);
        return minValue+resolution*(double)uintValue;

    } else {
        value = this->clip(-value);
        uint32_t uintValue = (uint32_t)round((value+maxValue)/resolution);
        this->encodeUint(uintValue, status);
        return maxValue-resolution*(double)uintValue;
    }
}

DoubleSignAbsCoder::DoubleSignAbsCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

double DoubleSignAbsCoder::encode(double value, CommandStatus_t &status) {
    if (!invertedScale) {
        value = this->clip(value);
        uint32_t uintValue = (uint32_t)round(fabs(value)/resolution);
        uint32_t signValue = (value < 0.0 ? 1 << (bitsNum-1) : 0);
        this->encodeUint(uintValue+signValue, status);
        if (value < 0.0) {
            return -resolution*(double)uintValue;

        } else {
            return resolution*(double)uintValue;
        }

    } else {
        value = this->clip(-value);
        uint32_t uintValue = (uint32_t)round(fabs(value)/resolution);
        uint32_t signValue = (value < 0.0 ? 1 << (bitsNum-1) : 0);
        this->encodeUint(uintValue+signValue, status);
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

double FloatCoder::encode(double value, CommandStatus_t &status) {
    float fltValue = (float)value;
    uint32_t uintValue = *(uint32_t *)(&fltValue);
    this->encodeUint(uintValue, status);
    return (double)fltValue;
}

MultiCoder::MultiCoder(CoderConfig_t config) :
    CommandCoder(0, 0, 0),
    config(config) {

    rangesNum = config.thresholdVector.size();
}

double MultiCoder::encode(double value, CommandStatus_t &status) {
    double ret;
    if (rangeSelectedFlag) {
        config.boolCoder->encode(selectedRange, status);
        ret = config.doubleCoderVector[selectedRange]->encode(value, status);

    } else {
        bool done = false;
        int i;
        for (i = 0; i < rangesNum; i++) {
            if (value < config.thresholdVector[i] && !done) {
                config.boolCoder->encode(i, status);
                ret = config.doubleCoderVector[i]->encode(value, status);
                done = true;
            }
        }
        if (!done) {
            config.boolCoder->encode(i, status);
            ret = config.doubleCoderVector[i]->encode(value, status);
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

void MultiCoder::getConfig(CoderConfig_t &config) {
    config = this->config;
}
