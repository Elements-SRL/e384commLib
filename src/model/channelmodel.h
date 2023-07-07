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
    ~ChannelModel();

    uint16_t getId();
    bool isOn();
    bool isCompensatingDoc();
    bool isCompensatingCfast();
    bool isCompensatingCslowRs();
    bool isCompensatingRsCp();
    bool isCompensatingRsPg();
    bool isInStimActive();
    bool isSelected();
    Measurement_t getVhold();
    Measurement_t getVdoc();

    void setId(uint16_t id);
    void setOn(bool on);
    void setCompensatingDoc(bool compensating);
    void setCompensatingCfast(bool compensating);
    void setCompensatingCslowRs(bool compensating);
    void setCompensatingRsCp(bool compensating);
    void setCompensatingRsPg(bool compensating);
    void setCompensatingCcCfast(bool compensating);
    void setInStimActive(bool active);
    void setSelected(bool selected);
    void setVhold(Measurement_t vHold);
    void setChold(Measurement_t cHold);
    void setVdoc(Measurement_t vDoc);

private:
    uint16_t id = 65535;                            // Channel ID
    bool on = false;                                // Channel is ON (true) or off (false)
    bool compensating = false;                      // The channels is currently doing digital offset compensation
    bool compensatingCfast = false;                 // The channel is currently compensating Cfast
    bool compensatingCslowRs = false;               // The channel is currently compensating CslowRs
    bool compensatingRsCp = false;                  // The channel is currently compensating RsCp
    bool compensatingRSPg = false;                  // The channel is currently compensating RsPg
    bool compensatingCcCfast = false;               // The channel is currently compensating Cfast in CURRENT CLAMP
    bool inStimActive = false;                      // The channel currently has active stimulus (true)
    bool selected = false;                          // The channel is selected in the GUI
    Measurement_t vHold = {0.0, UnitPfxMilli, "V"}; // holding voltage
    Measurement_t cHold = {0.0, UnitPfxNano, "A"};  // holding current
    Measurement_t vDoc = {0.0, UnitPfxMilli, "V"};  // digital offset compensation voltage
};

#endif // CHANNELMODEL_H
