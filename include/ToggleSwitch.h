#pragma once

#include <Arduino.h>

template <typename ReturnValue>
class ToggleSwitch
{
public:
    ToggleSwitch(int bottomPin, int topPin, ReturnValue bottomValue, ReturnValue topValue, ReturnValue nullValue)
    {
        pinMode(bottomPin, INPUT_PULLUP);
        pinMode(topPin, INPUT_PULLUP);
        this->bottomPin = bottomPin;
        this->topPin = topPin;
        this->bottomValue = bottomValue;
        this->topValue = topValue;
        this->nullValue = nullValue;
    }

    
    /// @brief Returns the bottom or top value based on the position of the switch
    /// @tparam ReturnValue
    /// @return bottomValue if switch is set to bottom, topValue if switch is set to top, and ReturnValue::Null if in the middle
    ReturnValue GetValueFromSwitch()
    {
        // When the bottom pin is pulled to ground, the switch is in the up state
        if (digitalRead(bottomPin) == LOW)
        { 
            return topValue;
        }
        else if (digitalRead(topPin) == LOW)
        { 
            return bottomValue;
        }
        else 
        {
            return nullValue;
        }
    }
private:
    int bottomPin;
    int topPin;

    ReturnValue bottomValue;
    ReturnValue topValue;
    ReturnValue nullValue;
};