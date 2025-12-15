#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define WHITE SH110X_WHITE
#define BLACK SH110X_BLACK

class Display : public Adafruit_SH1106G
{
public:
    Display(int width, int height);
    bool initiliaze(int i2c_address);
    void update();

private:
    void setColorInverted(bool inversion);
};
