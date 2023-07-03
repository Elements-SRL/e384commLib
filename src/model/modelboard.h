#ifndef MODELBOARD_H
#define MODELBOARD_H

#include <vector>

#include "e384commlib_global.h"
#include "e384commlib_global_addendum.h"
#include "modelchannel.h"

class ModelBoard {
public:
    ModelBoard();
    ~ModelBoard();

    uint16_t getId();
    std::vector<ModelChannel*> getChannelsOnBoard();
    Measurement_t getGateVoltage();
    Measurement_t getSourceVoltage();

    void setId(uint16_t id);
    void setChannelsOnBoard (std::vector<ModelChannel*> channelsOnBoard);
    void setGateVoltage(Measurement_t gateVoltage);
    void setSourceVoltage(Measurement_t sourceVoltage);

    void fillChannelList(uint16_t numChannelsOnBoard);

private:
    uint16_t id = 65535;
    std::vector<ModelChannel*> channelsOnBoard;
    Measurement_t gateVoltage = {0.0, UnitPfxMilli, "V"};
    Measurement_t sourceVoltage = {0.0, UnitPfxMilli, "V"};

};

#endif // MODELBOARD_H
