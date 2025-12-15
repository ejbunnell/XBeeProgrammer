#pragma once

#include <Arduino.h>

#define COMMAND_CHARACTERS "+++" //  The default command character needed to enter command mode

#define CHANNEL_AT_CMD "CH" // AT (attention) identifier for the channel command
#define BANDWIDTH_AT_CMD "ID" // AT identifier for the bandwidth (or pan ID) command
#define FIRMWARE_VERSION_AT_CMD "VR" // AT identifier for the firmware version command
#define WRITE_AT_CMD "WR" // AT identifier for the write-to-flash command
#define INVOKE_BOOTLOADER_AT_CMD "%P"

#define NO_PARAMETERS "____NO_PARAMETERS____" // A constant char array that allows the sendATCommand function to have a default value for parameters

#define LATEST_FIRMWARE "2014" // The latest firmware version for 802.15.4

class XBee : public HardwareSerial
{
public:
    XBee(uint8_t uart_nr, int resetPin);

    bool connect();
    void Program();
    void Ping();
    void UpdateFirmware();

private:
    void SendATCommand();
    void ReadATCommand();

    int resetPin;
};