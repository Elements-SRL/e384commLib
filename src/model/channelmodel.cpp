#include "channelmodel.h"

ChannelModel::ChannelModel() {

}

uint16_t ChannelModel::getId() {
    return this->id;
}

bool ChannelModel::isOn() {
    return this->on;
}

bool ChannelModel::isExpanded() {
    return expanded;
}

bool ChannelModel::isRecalibratingReadoutOffset() {
    return recalibratingReadoutOffset;
}

bool ChannelModel::isCompensatingLiquidJunction() {
    return compensatingLiquidJunction;
}

bool ChannelModel::isCompensatingCfast() {
    return this->compensatingCfast;
}

bool ChannelModel::isCompensatingCslowRs() {
    return this->compensatingCslowRs;
}

bool ChannelModel::isCompensatingRsCp() {
    return this->compensatingRsCp;
}

bool ChannelModel::isCompensatingRsPg() {
    return this->compensatingRSPg;
}

bool ChannelModel::isStimActive() {
    return this->stimActive;
}

bool ChannelModel::isSelected() {
    return this->selected;
}

Measurement_t ChannelModel::getVhold() {
    return this->vHold;
}

Measurement_t ChannelModel::getChold() {
    return this->cHold;
}

Measurement_t ChannelModel::getVhalf() {
    return this->vHalf;
}

Measurement_t ChannelModel::getChalf() {
    return this->cHalf;
}

Measurement_t ChannelModel::getLiquidJunctionVoltage() {
    return this->liquidJunctionVoltage;
}

void ChannelModel::setId(uint16_t id) {
    this->id = id;
}

void ChannelModel::setOn(bool on) {
    this->on = on;
}

void ChannelModel::setExpandedTrace(bool expanded) {
    this->expanded = expanded;
}

void ChannelModel::setRecalibratingReadoutOffset(bool recalibrating) {
    recalibratingReadoutOffset = recalibrating;
}

void ChannelModel::setCompensatingLiquidJunction(bool compensating) {
    compensatingLiquidJunction = compensating;
}

void ChannelModel::setCompensatingCfast(bool compensating) {
    this->compensatingCfast = compensating;
}

void ChannelModel::setCompensatingCslowRs(bool compensating) {
    this->compensatingCslowRs = compensating;
}

void ChannelModel::setCompensatingRsCp(bool compensating) {
    this->compensatingRsCp = compensating;
}

void ChannelModel::setCompensatingRsPg(bool compensating) {
    this->compensatingRSPg = compensating;
}

void ChannelModel::setCompensatingCcCfast(bool compensating) {
    this->compensatingCcCfast = compensating;
}

void ChannelModel::setStimActive(bool active) {
    this->stimActive = active;
}

void ChannelModel::setSelected(bool selected) {
    this->selected = selected;
}

void ChannelModel::setVhold(Measurement_t vHold) {
    this->vHold = vHold;
}

void ChannelModel::setChold(Measurement_t cHold) {
    this->cHold = cHold;
}

void ChannelModel::setVhalf(Measurement_t vHalf) {
    this->vHalf = vHalf;
}

void ChannelModel::setChalf(Measurement_t cHalf) {
    this->cHalf = cHalf;
}

void ChannelModel::setLiquidJunctionVoltage(Measurement_t voltage) {
    this->liquidJunctionVoltage = voltage;
}
