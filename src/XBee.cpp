#include "XBee.h"


XBee::XBee(uint8_t uart_nr, int reset_pin, Display *display) : HardwareSerial(uart_nr)
{
    this->reset_pin = reset_pin;
    this->display = display;
    resetXBee();
}

bool XBee::connect()
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
        if (find("OK"))
        {
            is_connected = true;
            display->println(F("XBee has successfully entered Command Mode"));
            updateFirmware();
        }
        else
        {
            display->println(F("XBee did not confirm entering Command Mode"));

            bool firmwareUpdateSuccess = updateFirmware(false);
            
            is_connected = firmwareUpdateSuccess;
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

bool XBee::updateFirmware(bool invokeBootloader)
{
    if (invokeBootloader)
    {
        std::vector<std::string> pingResults = ping();
        std::string currentFirmware = pingResults[2];
        // if (currentFirmware == LATEST_FIRMWARE) return;
        display->clear();
        display->println(F("Invoking Bootloader"));
        display->display();
        delay(250);

        sendATCommand(INVOKE_BOOTLOADER_AT_CMD);
        updateBaudRate(115200);
        ping();

        if (!find("BL"))
        {
            display->println(F("XBee did not enter Bootloader Mode"));
            display->display();
            updateBaudRate(9600);
            delay(1000);
            return false;
        }

        display->println(F("XBee in Bootloader Mode"));
        display->display();
        delay(250);
    }
    else
    {
        updateBaudRate(115200);
    }
    delay(100);
    while (available())
    {
        Serial.print((char)read());
    }
    display->clear();
    display->println(F("Initiating Firmware \nUpdate"));
    display->display();
    write('1');

    sendXmodemFromFlash();

    delay(100);

    std::string response;
    while (available())
    {
        response.push_back((char)read());
    }
    Serial.println(response.c_str()); // "Serial upload complete"

    write('2');

    delay(100);

    updateBaudRate(9600);

    delay(100);

    while (available())
    {

        char resp = read();
        Serial.print(resp);
    }

    delay(100);
    

    return true;
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

void XBee::resetXBee()
{
    digitalWrite(reset_pin, LOW);
    delay(10);
    digitalWrite(reset_pin, HIGH);
}
