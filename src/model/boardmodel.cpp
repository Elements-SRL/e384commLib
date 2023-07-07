#include "boardmodel.h"

BoardModel::BoardModel()
{

}

BoardModel::~BoardModel()
{
    int numOfChannels = this->channelsOnBoard.size();
    for(uint16_t i = 0; i< numOfChannels; i++ ){
        if (this->channelsOnBoard[i] != nullptr) {
            delete this->channelsOnBoard[i];
        }
    }
}

uint16_t BoardModel::getId(){
    return this->id;
}

std::vector<ChannelModel*> BoardModel::getChannelsOnBoard(){
    return this->channelsOnBoard;
}

Measurement_t BoardModel::getGateVoltage(){
    return this->gateVoltage;
}

Measurement_t BoardModel::getSourceVoltage(){
    return this->sourceVoltage;
}


void BoardModel::setId(uint16_t id){
    this->id = id;
}

void BoardModel::setChannelsOnBoard (std::vector<ChannelModel*> channelsOnBoard){
    this->channelsOnBoard = channelsOnBoard;
}

void BoardModel::setGateVoltage(Measurement_t gateVoltage){
    this->gateVoltage = gateVoltage;
}

void BoardModel::setSourceVoltage(Measurement_t sourceVoltage){
    this->sourceVoltage = sourceVoltage;
}


void BoardModel::fillChannelList(uint16_t numChannelsOnBoard){
    this->channelsOnBoard.resize(numChannelsOnBoard);
    for(uint16_t i =0; i< numChannelsOnBoard; i++ ){
        uint16_t newChannelId = numChannelsOnBoard*this->getId() + i; // board_0: ch_0 -> ch_15; board_1: ch_16 -> ch_31; ...
        ChannelModel* channel = new ChannelModel;
        channel->setId(newChannelId);
        this->channelsOnBoard[i] = channel;
    }
}
