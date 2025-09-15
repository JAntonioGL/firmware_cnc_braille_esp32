// Solenoid.h
#pragma once
#include <Arduino.h>

class Solenoid {
public:
  void begin(uint8_t pin, bool activeHigh = true) {
    _pin = pin; _activeHigh = activeHigh;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, _activeHigh ? LOW : HIGH); // reposo
  }
  // Pulso “golpe” seguro
  void pulse(uint16_t t_hold_ms, uint16_t t_release_ms = 5) {
    digitalWrite(_pin, _activeHigh ? HIGH : LOW);
    delay(t_hold_ms);
    digitalWrite(_pin, _activeHigh ? LOW : HIGH);
    delay(t_release_ms);
  }
private:
  uint8_t _pin = 255;
  bool _activeHigh = true;
};
