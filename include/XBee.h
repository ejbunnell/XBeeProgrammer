#pragma once

// C:\Users\Elliott\AppData\Local\Digi\XCTU-NG\radio_firmwares\XB3-24A\XB3-24A_2014-th.gbl --- location of the firmware file

#include <Arduino.h>
#include <vector>
#include <string>
#include <cstring>

#include "Display.h"

#include "ChannelSelections.h"

#define COMMAND_CHARACTERS "+++" //  The default command character needed to enter command mode

#define CHANNEL_AT_CMD "CH"          // AT (attention) identifier for the channel command
#define BANDWIDTH_AT_CMD "ID"        // AT identifier for the bandwidth (or pan ID) command
#define FIRMWARE_VERSION_AT_CMD "VR" // AT identifier for the firmware version command
#define WRITE_AT_CMD "WR"            // AT identifier for the write-to-flash command
#define INVOKE_BOOTLOADER_AT_CMD "%P"

#define NO_PARAMETERS "____NO_PARAMETERS____" // A constant char array that allows the sendATCommand function to have a default value for parameters

extern const uint8_t _binary_data_firmware_gbl_start[] asm("_binary_data_firmware_gbl_start");
extern const uint8_t _binary_data_firmware_gbl_end[] asm("_binary_data_firmware_gbl_end");

class XBee : public HardwareSerial
{
public:
    XBee(uint8_t uart_nr, int reset_pin, Display *display);

    // Attempt to connect to the XBee by sending the Command Character (CC), '+' three times with one second (GT) of no communication before and after
    // The XBee will then respond with "OK\r", which tells us that we are connected. If it does not respond, the screen goes to a "No XBee" screen and waits for the user to press the action button to try again
    bool connect();
    // Returns whether the XBee is currently connected to the ESP32
    bool isConnected();
    // Takes the currently selected options for both channel and bandwidth and writes (WR) them to the XBee
    void program(ChannelSelections selectedChannel, BandwidthSelections selectedBandwidth);
    // Pings the XBee to both get the current values of the channel and bandwidth and the make sure that the XBee stays in Command Mode by pinging it within the Command Mode Timeout (CT)
    std::vector<std::string> ping();
    bool updateFirmware(bool invokeBootloader = true);

private:
    // Sends an AT command to the  Command is the two character identifier that the XBee uses to determine what to reply with or what to program
    // When reading, there are no parameters so you can leave that blank
    // When programming the XBee, the parameters are whatever values you are trying to set ('C', 'F', '555', '3332', etc)
    void sendATCommand(const char *command, const char *parameters = NO_PARAMETERS);
    // Uses sendATCommand with no parameters, but waits for all the returning Serial data and puts it into the char buffer
    // The delayMs determines how long the program waits for the Serial signal to come back
    // If there is no Serial data to be read, the program puts a -1 in index zero of the buffer
    void readATCommand(std::string *buf, const char *command, int delay_ms);

    void resetXBee();

    uint16_t crc16(const uint8_t *buf, uint16_t len) 
    {
        uint16_t crc = 0;
        while (len--) {
            crc ^= (uint16_t)*buf++ << 8;
            for (uint8_t i = 0; i < 8; i++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
        }
        return crc;
    }

    bool sendXmodemFromFlash() 
    {
        const uint8_t *data = _binary_data_firmware_gbl_start;
        size_t length = _binary_data_firmware_gbl_end - _binary_data_firmware_gbl_start;
        uint8_t packet[128];
        uint8_t block = 1;
        size_t offset = 0;

        Serial.print(length);

        // Wait for 'C'
        unsigned long start = millis();
        while (millis() - start < 5000) {
            if (available() && read() == 'C') break;
        }
        int count = 0;
        const int totalCount = 2765;
        const int sections = 16;
        const int sectional = totalCount / 20;
        while (offset < length) {
            size_t chunk = min((size_t)128, length - offset);
            memset(packet, 0x1A, 128);
            memcpy(packet, data + offset, chunk);

            uint16_t crc = crc16(packet, 128);

            write(0x01);        // SOH
            write(block);
            write(~block);
            write(packet, 128);
            write(crc >> 8);
            write(crc & 0xFF);

            // wait for ACK
            unsigned long t = millis();
            while (millis() - t < 2000) 
            {
                if (available() && read() == 0x06) break;
            }

            offset += chunk;
            block++;
            

            count++;
            
            display->clear();

            display->printf("%4.2f%%", (((float)count / (float)totalCount) * 100.0f));
            display->display();
        }

        write(0x04); // EOT
        // while (read() != 0x06);

        Serial.printf("Count: %d\n", count);
        Serial.flush();

        return true;
    }

    int reset_pin;
    bool is_connected = false;

    const std::string LATEST_FIRMWARE = "2014"; // The latest firmware version for 802.15.4

    Display *display;
};
