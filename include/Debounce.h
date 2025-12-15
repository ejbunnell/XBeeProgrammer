#pragma once

#include <Arduino.h>

// A quick class to add a debounce to all digital inputs
class Debounce {
  public:
    Debounce(int pin, unsigned int debounceDelay);
    
    bool IsPressed();
    
  private:
    int GetState();

    int pin;
    int state = HIGH;
    unsigned long debounceTime = 0;
    unsigned int debounceDelay;
    bool canReadLow = true;
};

