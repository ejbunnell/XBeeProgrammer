#include "XBee.h"

XBee::XBee(uint8_t uart_nr, int reset_pin, Display *display) : HardwareSerial(uart_nr)
{
    this->reset_pin = reset_pin;
    this->display = display;
    switch (uart_nr)
    {
    case 1:
        pinMode(9, INPUT_PULLUP);
        break;
    case 2:
        pinMode(16, INPUT_PULLUP);
        break;
    default:
        pinMode(3, INPUT_PULLUP);
        break;
    }
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

    // If connected, the XBee will respond with "OK\r". If not, there will be no response and the program will move on after the delay
    if (available())
    {
        if (find("OK"))
        {
            is_connected = true;
            display->printOneLine("XBee has successfully entered Command Mode", 1000);
            updateFirmware();
        }
        else
        {
            display->printOneLine("XBee did not confirm entering Command Mode", 1000);
            // If the XBee did not respond with "OK\r", a possible reason could be that it is stuck in bootloader mode.
            // This can happen if the XBee is reset during a firmware upload, so we have to check.
            bool firmwareUpdateSuccess = updateFirmware(false);

            is_connected = firmwareUpdateSuccess; // If the firmware update was successful, then we know that the XBee was stuck in bootloader mode, so we can consider it "connected" even though it did not respond to the original "+++" command
        }
    }
    else
    {
        display->printOneLine("XBee was not found", 1000);
    }

    return isConnected();
}

bool XBee::isConnected()
{
    return is_connected;
}

void XBee::program(Channel::Selections selectedChannel, Bandwidth::Selections selectedBandwidth)
{
    sendATCommand(CHANNEL_AT_CMD, Channel::to_string(selectedChannel).c_str());
    sendATCommand(WRITE_AT_CMD);
    delay(50);
    flushOutSerial();

    sendATCommand(BANDWIDTH_AT_CMD, Bandwidth::to_string(selectedBandwidth).c_str());
    sendATCommand(WRITE_AT_CMD);
    delay(50);
    flushOutSerial();
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

    // If any of the results are empty, then we know that the XBee is no longer connected, so we set is_connected to false
    for (auto &str : results)
    {
        if (str.empty())
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

        if (currentFirmware.find(ALLOWABLE_FIRMWARE_REGEX) != std::string::npos)
        {
            display->printOneLine("XBee firmware is \ncompatible", 1000);
            return true;
        }
        
        display->printOneLine(("Xbee firmware: " + currentFirmware + " \nis out of date").c_str(), 1000);
        display->printOneLine("Invoking \nBootloader Mode\n", 500);

        sendATCommand(INVOKE_BOOTLOADER_AT_CMD);
        updateBaudRate(115200);
        ping();

        if (!find("BL"))
        {
            display->printOneLine("XBee not in \nBootloader Mode", 1000, false);
            updateBaudRate(9600);
            delay(100);
            return false;
        }
        display->printOneLine("XBee in \nBootloader Mode", 500, false);
    }
    else
    {
        updateBaudRate(115200);
        ping();
        display->printOneLine("Checking Bootloader \nMode", 500);
        if (!find("BL"))
        {
            display->printOneLine("XBee not in \nBootloader Mode", 1000);
            updateBaudRate(9600);
            delay(100);
            return false;
        }
        else
        {
            display->printOneLine("XBee in Bootloader \nMode", 500);
        }
    }
    delay(100);

    display->printOneLine("Starting firmware \nupload", 500);
    write('1');
    flush();

    if (!sendXmodemFromFlash())
    {
        display->printOneLine("Firmware upload \nfailed", 1000);
        updateBaudRate(9600);
        return false;
    }

    delay(100);

    std::string response;
    while (available())
    {
        response.push_back((char)read());
    }
    if (response.find("Serial upload complete") == std::string::npos)
    {
        display->printOneLine("Firmware upload \nfailed", 1000);
        updateBaudRate(9600);
        return false;
    }
    else
    {
        display->printOneLine("Firmware upload \nsuccessful", 1000);
    }

    write('2');
    flush();

    display->printOneLine("Loading firmware", 500);

    delay(1000);

    updateBaudRate(9600);

    delay(100);

    connect();

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
    flushOutSerial();

    sendATCommand(command);
    delay(delay_ms);
    buf->clear();
    bool receivedEOT = false;
    while (available())
    {
        char readChar = read();
        if (readChar == '\r')
        {
            receivedEOT = true;
            break;
        }
        buf->push_back(readChar);
    }
    if (!receivedEOT)
    {
        buf->clear();
    }
}

void XBee::resetXBee()
{
    digitalWrite(reset_pin, LOW);
    delay(10);
    digitalWrite(reset_pin, HIGH);
}

uint16_t XBee::crc16(const uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0;
    while (len--)
    {
        crc ^= (uint16_t)*buf++ << 8;
        for (uint8_t i = 0; i < 8; i++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

bool XBee::sendXmodemFromFlash()
{
    const uint8_t *data = _binary_data_firmware_gbl_start;
    size_t length = _binary_data_firmware_gbl_end - _binary_data_firmware_gbl_start;
    uint8_t packet[128];
    uint8_t block = 1;
    size_t offset = 0;

    // Wait for 'C'
    unsigned long start = millis();
    while (millis() - start < 5000)
    {
        if (available() && read() == 'C')
            break;
    }
    int count = 0;
    const int totalCount = std::ceil((float)length / 128.0f);
    while (offset < length)
    {
        size_t chunk = min((size_t)128, length - offset);
        memset(packet, 0x1A, 128);
        memcpy(packet, data + offset, chunk);

        uint16_t crc = crc16(packet, 128);

        write(0x01); // SOH
        write(block);
        write(~block);
        write(packet, 128);
        write(crc >> 8);
        write(crc & 0xFF);

        // wait for ACK
        unsigned long t = millis();
        bool resendLastPacket = false;
        while (millis() - t < 2000)
        {
            if (available())
            {
                char resp = read();
                if (resp == 0x06)
                    break; // ACK
                else if (resp == 0x15)
                {
                    resendLastPacket = true; // NAK
                    break;
                }
            }
        }
        if (resendLastPacket)
            continue;

        offset += chunk;
        block++;

        count++;

        display->clear();
        display->println(F("Updating firmware:"));
        display->printf("%4.2f%% done", (((float)count / (float)totalCount) * 100.0f));
        display->display();
    }

    write(0x04); // EOT
    while (true)
    {
        if (available())
        {
            char resp = read();
            if (resp == 0x06)
                return true;
            else if (resp == 0x15)
            {
                write(0x04); // EOT
            }
        }
    }
}

void XBee::flushOutSerial()
{
    while (available())
    {
        read();
    }
}