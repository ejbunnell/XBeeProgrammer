#include "XBee.h"

XBee::XBee(uint8_t uart_nr, int resetPin) : HardwareSerial(uart_nr)
{
    this->resetPin = resetPin;
}

bool XBee::connect()
{

    delay(1100);
    write("+++");
    delay(1100);

    if (available())
    {
        if (find("OK\r")) return true;
    }

    return false;

}