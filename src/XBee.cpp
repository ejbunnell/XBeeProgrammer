#include "XBee.h"

XBee::XBee(uint8_t uart_nr, int reset_pin) : HardwareSerial(uart_nr)
{
    this->reset_pin = reset_pin;
}

bool XBee::connect(Display *display)
{
    is_connected = false;

    display->clear();
    display->print(F("Connecting to XBee"));
    display->displayDots(1100);
    write("+++");
    display->displayDots(1100);

    display->clear();
    if (available())
    {
        if (find("OK\r"))
        {
            is_connected = true;
            display->println(F("XBee has successfully entered Command Mode"));
        }
        else
        {
            display->println(F("XBee did not confirm entering Command Mode"));
        }
    }
    else
    {
        display->println(F("XBee was not found"));
    }

    display->display();
    delay(1000);

    return is_connected;
}

bool XBee::isConnected()
{
    return is_connected;
}

void XBee::program(ChannelSelections selectedChannel, BandwidthSelections selectedBandwidth)
{
    if (selectedChannel == ChannelSelections::C)
        sendATCommand(CHANNEL_AT_CMD, "C");
    else
        sendATCommand(CHANNEL_AT_CMD, "F");
    sendATCommand(WRITE_AT_CMD);
    delay(50);
    while (available())
    {
        read();
    }

    if (selectedBandwidth == BandwidthSelections::B555)
        sendATCommand(BANDWIDTH_AT_CMD, "555");
    else
        sendATCommand(BANDWIDTH_AT_CMD, "3332");
    sendATCommand(WRITE_AT_CMD);
    delay(50);
    while (available())
    {
        read();
    }
}

std::vector<std::string> XBee::ping()
{
    std::vector<std::string> results;

    std::string channel;
    readATCommand(&channel, CHANNEL_AT_CMD, 20);
    results.push_back(channel);

    std::string bandwidth;
    readATCommand(&bandwidth, BANDWIDTH_AT_CMD, 40);
    results.push_back(bandwidth);

    std::string firmware;
    readATCommand(&firmware, FIRMWARE_VERSION_AT_CMD, 40);
    results.push_back(firmware);

    for (auto &str : results)
    {
        if (str.empty() || str[0] == -1)
        {
            is_connected = false;
            break;
        }
    }

    return results;
}

void XBee::updateFirmware()
{
    // Implementation of updating the firmware
}

void XBee::sendATCommand(const char *command, const char *parameters)
{
    write("AT");
    write(command);
    if (parameters != NO_PARAMETERS)
        write(parameters);
    write('\r');
    flush();
}

void XBee::readATCommand(std::string *buf, const char *command, int delay_ms)
{
    sendATCommand(command);
    delay(delay_ms);
    buf->clear();
    while (available())
    {
        char readChar = read();
        buf->push_back(readChar);
    }
}
