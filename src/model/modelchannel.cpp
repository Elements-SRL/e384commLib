#include "modelchannel.h"

ModelChannel::ModelChannel()
{

}

ModelChannel::~ModelChannel(){

}

uint16_t ModelChannel::getId(){
    return this->id;
}

bool ModelChannel::isOn(){
    return this->on;
}

bool ModelChannel::isCompensatingDoc(){
    return this->compensating;
}

bool ModelChannel::isCompensatingCfast(){
    return this->compensatingCfast;
}

bool ModelChannel::isCompensatingCslowRs(){
    return this->compensatingCslowRs;
}

bool ModelChannel::isCompensatingRsCp(){
    return this->compensatingRsCp;
}

bool ModelChannel::isCompensatingRsPg(){
    return this->compensatingRSPg;
}

bool ModelChannel::isInStimActive(){
    return this->inStimActive;
}

bool ModelChannel::isSelected(){
    return this->selected;
}

Measurement_t ModelChannel::getVhold(){
    return this->vHold;
}

Measurement_t ModelChannel::getVdoc(){
    return this->vDoc;
}


void ModelChannel::setId(uint16_t id){
    this->id = id;
}

void ModelChannel::setOn(bool on){
    this->on = on;
}

void ModelChannel::setCompensatingDoc(bool compensating){
    this->compensating = compensating;
}

void ModelChannel::setCompensatingCfast(bool compensating){
    this->compensatingCfast = compensating;
}

void ModelChannel::setCompensatingCslowRs(bool compensating){
    this->compensatingCslowRs = compensating;
}

void ModelChannel::setCompensatingRsCp(bool compensating){
    this->compensatingRsCp = compensating;
}

void ModelChannel::setCompensatingRsPg(bool compensating){
    this->compensatingRSPg = compensating;
}

void ModelChannel::setCompensatingCcCfast(bool compensating){
    this->compensatingCcCfast = compensating;
}

void ModelChannel::setInStimActive(bool active){
    this->inStimActive = active;
}

void ModelChannel::setSelected(bool selected){
    this->selected = selected;
}

void ModelChannel::setVhold(Measurement_t vHold){
    this->vHold = vHold;
}

void ModelChannel::setChold(Measurement_t cHold){
    this->cHold = cHold;
}

void ModelChannel::setVdoc(Measurement_t vDoc){
    this->vDoc = vDoc;
}
