#include "Display.h"

Display::Display(int width, int height) : Adafruit_SH1106G(width, height) {}

bool Display::initiliaze(int i2c_address)
{
    if (!begin(i2c_address, true)) return false;

    clearDisplay();
    setTextSize(1);
    setTextColor(WHITE);
    setCursor(0, 0);
    display();
    return true;
}

void Display::update()
{
    clearDisplay();
    setCursor(0, 0);
    println("This is a test!");
    display();
}