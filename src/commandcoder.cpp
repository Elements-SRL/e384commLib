#include "commandcoder.h"

using namespace std;

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

CommandCoder::~CommandCoder() {

}

void CommandCoder::encodeUint(uint32_t uintValue, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
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

BoolCoder::~BoolCoder() {

}

BoolArrayCoder::BoolArrayCoder(CoderConfig_t config) :
    BoolCoder(config) {

}

BoolArrayCoder::~BoolArrayCoder() {

}

void BoolArrayCoder::encode(uint32_t value, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    this->encodeUint(value, encodingWords, startingWord, endingWord);
}

BoolNegatedArrayCoder::BoolNegatedArrayCoder(CoderConfig_t config) :
    BoolArrayCoder(config) {

}

BoolNegatedArrayCoder::~BoolNegatedArrayCoder() {

}

void BoolNegatedArrayCoder::encode(uint32_t value, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    this->encodeUint(~value, encodingWords, startingWord, endingWord);
}

BoolRandomArrayCoder::BoolRandomArrayCoder(CoderConfig_t config) :
    BoolArrayCoder(config) {

    tos.resize(0);
    toNum = 0;
}

BoolRandomArrayCoder::~BoolRandomArrayCoder() {

}

void BoolRandomArrayCoder::encode(uint32_t value, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
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

BoolOneHotCoder::~BoolOneHotCoder() {

}

void BoolOneHotCoder::encode(uint32_t value, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    this->encodeUint(1 << value, encodingWords, startingWord, endingWord);
}

DoubleCoder::DoubleCoder(CoderConfig_t config) :
    CommandCoder(config.initialWord, config.initialBit, config.bitsNum),
    config(config),
    resolution(config.resolution),
    minValue(config.minValue),
    maxValue(config.maxValue),
    offset(config.offset) {

}

DoubleCoder::~DoubleCoder() {

}

double DoubleCoder::clip(double value) {
    return (value > maxValue ? maxValue : (value < minValue ? minValue : value))-offset;
}

DoubleTwosCompCoder::DoubleTwosCompCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

DoubleTwosCompCoder::~DoubleTwosCompCoder() {

}

void DoubleTwosCompCoder::encode(double value, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    value = this->clip(value);
    int32_t intValue = (int32_t)round(value/resolution);
    this->encodeUint((uint32_t)intValue, encodingWords, startingWord, endingWord);
}

DoubleOffsetBinaryCoder::DoubleOffsetBinaryCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

DoubleOffsetBinaryCoder::~DoubleOffsetBinaryCoder() {

}

void DoubleOffsetBinaryCoder::encode(double value, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    value = this->clip(value);
    uint32_t uintValue = (uint32_t)round((value-minValue)/resolution);
    this->encodeUint(uintValue, encodingWords, startingWord, endingWord);
}

DoubleSignAbsCoder::DoubleSignAbsCoder(CoderConfig_t config) :
    DoubleCoder(config) {

}

DoubleSignAbsCoder::~DoubleSignAbsCoder() {

}

void DoubleSignAbsCoder::encode(double value, vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) {
    value = this->clip(value);
    uint32_t uintValue = (uint32_t)round(fabs(value)/resolution);
    uintValue += (value < 0.0 ? 1 << (bitsNum-1) : 0);
    this->encodeUint(uintValue, encodingWords, startingWord, endingWord);
}