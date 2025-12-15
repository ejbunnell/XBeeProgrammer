#include "Debounce.h"

// The constructor sets the pinMode of the pin to INPUT_PULLUP
Debounce::Debounce(int pin, unsigned int debounceDelay) {
  pinMode(pin, INPUT_PULLUP);
  this->pin = pin;
  this->debounceDelay = debounceDelay;
}

bool Debounce::IsPressed()
{
    return GetState() == LOW;
}

// Gets the current state of the input after applying a debounce
// If digitalRead returns HIGH, it will return HIGH
// If digitalRead returns LOW, it will determine if it has returned LOW within the debounceDelay. If it has, then it will return LOW
int Debounce::GetState() {
  int reading = digitalRead(pin);

  if (canReadLow) state = reading;
  else state = HIGH;

  if (reading == LOW) {
    debounceTime = millis();
    canReadLow = false;
  }

  if ((millis() - debounceTime) > debounceDelay) {
    canReadLow = true;
  }

  return state;
}