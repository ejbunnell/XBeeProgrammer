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

void Display::update(bool is_xbee_connected, std::string currentChannel, std::string currentBandwidth, std::string firmwareVersion, 
                     ChannelSelections selectedChannel, BandwidthSelections selectedBandwidth)
{
    if (is_xbee_connected)
    {
        clearDisplay();

        setColorInverted(false);
        setCursor(0, 0);
        print(F("Channel: "));
        println(currentChannel.c_str());

        print(F("Bandwidth: "));
        println(currentBandwidth.c_str());

        print(F("Firmware: "));
        println(firmwareVersion.c_str());
        
        println();

        println(F("Desired Channel: ")); // C or F
        print("     ");
        // Sets either normal or inverted depending on which is selected

        setSelectedColor(ChannelSelections::C, selectedChannel);
        print(F(" C "));

        setColorInverted(false);
        print(F("      "));

        setSelectedColor(ChannelSelections::F, selectedChannel);
        println(F(" F "));

        setColorInverted(false);
        println(F("Desired Bandwidth: ")); // 555 or 3332
        print(F("    "));
        setSelectedColor(BandwidthSelections::B555, selectedBandwidth);
        print(F(" 555 "));

        setColorInverted(false);
        print(F("   "));

        setSelectedColor(BandwidthSelections::B3332, selectedBandwidth);
        println(F(" 3332 "));

        display();
    }
    else
    {
        clearDisplay();
        setCursor(0, 0);
        setColorInverted(false);
        print("       ");
        setColorInverted(true);
        println("No XBee");
        display();
    }
    
}

void Display::setColorInverted(bool inversion)
{
    if (inversion)
    {
        setTextColor(BLACK, WHITE);
    }
    else
    {
        setTextColor(WHITE);
    }
}

void Display::setSelectedColor(int var, int selected)
{
    if (var == selected) setColorInverted(true);
    else setColorInverted(false);
}