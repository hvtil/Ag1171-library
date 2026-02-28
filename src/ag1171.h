#pragma once
#include <Arduino.h>

class Ag1171
{
public:
    Ag1171(uint8_t pinFR,
           uint8_t pinRM,
           uint8_t pinSHK,
           uint8_t pinPD,
           bool ignoreSHK = false);

    void begin();
    void update();

    void powerUp();
    void powerDown();

    void startRinging(uint16_t frequencyHz,
                      uint16_t onTimeMs = 0,
                      uint16_t offTimeMs = 0);

    void stopRinging();

private:
    uint8_t _pinFR;
    uint8_t _pinRM;
    uint8_t _pinSHK;
    uint8_t _pinPD;

    bool _ignoreSHK;

    // Ring generation
    bool _ringingActive = false;
    uint16_t _ringFrequency = 20;
    unsigned long _lastToggle = 0;
    bool _ringState = false;

    // Cadence
    uint16_t _ringOnTime = 0;
    uint16_t _ringOffTime = 0;
    unsigned long _cadenceStart = 0;
    bool _cadenceActive = false;
};