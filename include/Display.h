#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <string>

#include "ChannelSelections.h"

#define WHITE SH110X_WHITE
#define BLACK SH110X_BLACK

class Display : public Adafruit_SH1106G
{
public:
    Display(int width, int height);
    bool initiliaze(int i2c_address);
    void update(bool is_xbee_connected, std::string currentChannel, std::string currentBandwidth, std::string firmwareVersion, 
                     ChannelSelections selectedChannel, BandwidthSelections selectedBandwidth);

private:
    void setColorInverted(bool inversion);
    void setSelectedColor(int var, int selected);
};
