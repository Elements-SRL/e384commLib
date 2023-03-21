#ifndef COMMANDCODER_H
#define COMMANDCODER_H

#define CMC_BITS_PER_WORD 16

#include <stdint.h>
#include <vector>

#include <e384commlib_global.h>

class CommandCoder {
public:
    CommandCoder(uint16_t initialWord, uint16_t initialBit, uint16_t bitsNum);
    virtual ~CommandCoder();

protected:
    void encodeUint(uint32_t uintValue, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord);

    uint16_t initialWord;
    uint16_t bitsNum;
    uint16_t wordsNum;
    std::vector <uint16_t> bitOffsets;
    std::vector <uint16_t> bitMasks;
};

class BoolCoder : public CommandCoder {
public:
    typedef struct {
        uint16_t initialWord;
        uint16_t initialBit;
        uint16_t bitsNum;
    } CoderConfig_t;

    BoolCoder(CoderConfig_t config);
    virtual ~BoolCoder();

    virtual void encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) = 0;

protected:
    CoderConfig_t config;
};

class BoolArrayCoder : public BoolCoder {
public:
    BoolArrayCoder(CoderConfig_t config);
    virtual ~BoolArrayCoder();

    void encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) override;
};

class BoolNegatedArrayCoder : public BoolArrayCoder {
public:
    BoolNegatedArrayCoder(CoderConfig_t config);
    virtual ~BoolNegatedArrayCoder();

    void encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) override;
};

class BoolRandomArrayCoder : public BoolArrayCoder {
public:
    BoolRandomArrayCoder(CoderConfig_t config);
    virtual ~BoolRandomArrayCoder();

    void encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) override;
    void addMapItem(uint32_t to);

private:
    uint32_t map(uint32_t from);

    std::vector <uint32_t> tos;
    uint32_t toNum;
};

class BoolOneHotCoder : public BoolCoder {
public:
    BoolOneHotCoder(CoderConfig_t config);
    virtual ~BoolOneHotCoder();

    void encode(uint32_t value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) override;
};

class DoubleCoder : public CommandCoder {
public:
    typedef struct CoderConfig {
        uint16_t initialWord = 0;
        uint16_t initialBit = 0;
        uint16_t bitsNum = 0;
        double resolution = 1.0;
        double minValue = 0.0;
        double maxValue = 0.0;
    } CoderConfig_t;

    DoubleCoder(CoderConfig_t config);
    virtual ~DoubleCoder();

    virtual double encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) = 0;

protected:
    double clip(double value);

    CoderConfig_t config;

    double resolution;
    double minValue;
    double maxValue;
};

class DoubleTwosCompCoder : public DoubleCoder {
public:
    DoubleTwosCompCoder(CoderConfig_t config);
    virtual ~DoubleTwosCompCoder();

    double encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) override;
};

class DoubleOffsetBinaryCoder : public DoubleCoder {
public:
    DoubleOffsetBinaryCoder(CoderConfig_t config);
    virtual ~DoubleOffsetBinaryCoder();

    double encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) override;
};

class DoubleSignAbsCoder : public DoubleCoder {
public:
    DoubleSignAbsCoder(CoderConfig_t config);
    virtual ~DoubleSignAbsCoder();

    double encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord) override;
};

//---------------MULTICODER-----------------------------//
class MultiCoder : public CommandCoder{
public:
    typedef struct MultiCoderConfig {
        BoolArrayCoder* boolCoder;
        std::vector<DoubleCoder*> doubleCoderVector;
        std::vector<double> thresholdVector;
    } MultiCoderConfig_t;

    MultiCoder (MultiCoderConfig_t multiConfig);

    double encode(double value, std::vector <uint16_t> &encodingWords, uint16_t &startingWord, uint16_t &endingWord);

private:
    MultiCoderConfig_t multiConfig;
};

#endif // COMMANDCODER_H
