#include "modelboard.h"

ModelBoard::ModelBoard()
{

}

ModelBoard::~ModelBoard()
{
    int numOfChannels = this->channelsOnBoard.size();
    for(uint16_t i = 0; i< numOfChannels; i++ ){
        if (this->channelsOnBoard[i] != nullptr) {
            delete this->channelsOnBoard[i];
        }
    }
}

uint16_t ModelBoard::getId(){
    return this->id;
}

std::vector<ModelChannel*> ModelBoard::getChannelsOnBoard(){
    return this->channelsOnBoard;
}

Measurement_t ModelBoard::getGateVoltage(){
    return this->gateVoltage;
}

Measurement_t ModelBoard::getSourceVoltage(){
    return this->sourceVoltage;
}


void ModelBoard::setId(uint16_t id){
    this->id = id;
}

void ModelBoard::setChannelsOnBoard (std::vector<ModelChannel*> channelsOnBoard){
    this->channelsOnBoard = channelsOnBoard;
}

void ModelBoard::setGateVoltage(Measurement_t gateVoltage){
    this->gateVoltage = gateVoltage;
}

void ModelBoard::setSourceVoltage(Measurement_t sourceVoltage){
    this->sourceVoltage = sourceVoltage;
}


void ModelBoard::fillChannelList(uint16_t numChannelsOnBoard){
    this->channelsOnBoard.resize(numChannelsOnBoard);
    for(uint16_t i =0; i< numChannelsOnBoard; i++ ){
        uint16_t newChannelId = numChannelsOnBoard*this->getId() + i; // board_0: ch_0 -> ch_15; board_1: ch_16 -> ch_31; ...
        ModelChannel* channel = new ModelChannel;
        channel->setId(newChannelId);
        this->channelsOnBoard[i] = channel;
    }
}
