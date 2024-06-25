#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include "e384commlib_global.h"
#include "e384commlib_global_addendum.h"

#ifndef E384COMMLIB_LABVIEW_WRAPPER
using namespace e384CommLib;
#endif

class ChannelModel {
public:
    ChannelModel();

    uint16_t getId();
    bool isOn();
    bool isExpanded();
    bool isRecalibratingReadoutOffset();
    bool isCompensatingLiquidJunction();
    bool isCompensatingCfast();
    bool isCompensatingCslowRs();
    bool isCompensatingRsCp();
    bool isCompensatingRsPg();
    bool isInStimActive();
    bool isSelected();
    Measurement_t getVhold();
    Measurement_t getChold();
    Measurement_t getVhalf();
    Measurement_t getChalf();
    Measurement_t getLiquidJunctionVoltage();

    void setId(uint16_t id);
    void setOn(bool on);
    void setExpandedTrace(bool expanded);
    void setRecalibratingReadoutOffset(bool recalibrating);
    void setCompensatingLiquidJunction(bool compensating);
    void setCompensatingCfast(bool compensating);
    void setCompensatingCslowRs(bool compensating);
    void setCompensatingRsCp(bool compensating);
    void setCompensatingRsPg(bool compensating);
    void setCompensatingCcCfast(bool compensating);
    void setInStimActive(bool active);
    void setSelected(bool selected);
    void setVhold(Measurement_t vHold);
    void setChold(Measurement_t cHold);
    void setVhalf(Measurement_t vHalf);
    void setChalf(Measurement_t cHalf);
    void setLiquidJunctionVoltage(Measurement_t voltage);

private:
    uint16_t id = 65535;                                                // Channel ID
    bool on = false;                                                    // Channel is ON (true) or off (false)
    bool expanded = false;                                              // The channel is currently shown in the big central plot
    bool recalibratingReadoutOffset = false;                            // The channel is currently being recalibrated
    bool compensatingLiquidJunction = false;                            // The channel is currently compensating the liquid junction
    bool compensatingCfast = false;                                     // The channel is currently compensating Cfast
    bool compensatingCslowRs = false;                                   // The channel is currently compensating CslowRs
    bool compensatingRsCp = false;                                      // The channel is currently compensating RsCp
    bool compensatingRSPg = false;                                      // The channel is currently compensating RsPg
    bool compensatingCcCfast = false;                                   // The channel is currently compensating Cfast in CURRENT CLAMP
    bool inStimActive = false;                                          // The channel currently has active stimulus (true)
    bool selected = false;                                              // The channel is selected in the GUI
    Measurement_t vHold = {0.0, UnitPfxMilli, "V"};                     // holding voltage
    Measurement_t cHold = {0.0, UnitPfxNano, "A"};                      // holding current
    Measurement_t vHalf = {0.0, UnitPfxMilli, "V"};                     // voltage half
    Measurement_t cHalf = {0.0, UnitPfxNano, "A"};                      // current half
    Measurement_t liquidJunctionVoltage = {0.0, UnitPfxMilli, "V"};     // digital offset compensation voltage
};

#endif // CHANNELMODEL_H
