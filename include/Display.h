#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <string>

#include "Selections.h"

#define WHITE SH110X_WHITE
#define BLACK SH110X_BLACK

class Display : public Adafruit_SH1106G
{
public:
    Display(int width, int height);
    bool initiliaze(int i2c_address);
    void update(bool is_xbee_connected, std::string currentChannel, std::string currentBandwidth, std::string firmwareVersion, 
                     Channel::Selections selectedChannel, Bandwidth::Selections selectedBandwidth);
    void displayDots(uint64_t length);
    void clear();
    void printOneLine(const char *text, uint16_t delayTime, bool clear = true);

private:
    void setColorInverted(bool inversion);
    void setSelectedColor(int var, int selected);
};
