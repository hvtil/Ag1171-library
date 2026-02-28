#include "ag1171.h"

Ag1171::Ag1171(uint8_t pinFR,
               uint8_t pinRM,
               uint8_t pinSHK,
               uint8_t pinPD,
               bool ignoreSHK)
    : _pinFR(pinFR),
      _pinRM(pinRM),
      _pinSHK(pinSHK),
      _pinPD(pinPD),
      _ignoreSHK(ignoreSHK)
{
}

void Ag1171::begin()
{
    pinMode(_pinFR, OUTPUT);
    pinMode(_pinRM, OUTPUT);
    pinMode(_pinSHK, INPUT);
    pinMode(_pinPD, INPUT);   // v2 safety

    digitalWrite(_pinFR, LOW);
    digitalWrite(_pinRM, LOW); // idle: not ringing
}

void Ag1171::update()
{
    if (!_ringingActive)
        return;

    unsigned long now = millis();

    // ---- Cadence ----
    if (_cadenceActive)
    {
        unsigned long elapsed = now - _cadenceStart;
        unsigned long cycle = (unsigned long)_ringOnTime + _ringOffTime;

        if (elapsed >= cycle)
        {
            _cadenceStart = now;
            elapsed = 0;
        }

        if (elapsed >= _ringOnTime)
        {
            // OFF phase
            digitalWrite(_pinFR, LOW);
            return;
        }
    }

    // ---- 20Hz toggle ----
    unsigned long interval = 1000UL / (_ringFrequency * 2);

    if (now - _lastToggle >= interval)
    {
        _lastToggle = now;
        _ringState = !_ringState;
        digitalWrite(_pinFR, _ringState);
    }
}

void Ag1171::powerUp()
{
    // PD nooit HIGH
    pinMode(_pinPD, INPUT);
}

void Ag1171::powerDown()
{
    // actief LOW
    pinMode(_pinPD, OUTPUT);
    digitalWrite(_pinPD, LOW);
}

void Ag1171::startRinging(uint16_t frequencyHz,
                          uint16_t onTimeMs,
                          uint16_t offTimeMs)
{
    _ringFrequency = frequencyHz;

    _ringOnTime  = onTimeMs;
    _ringOffTime = offTimeMs;

    _cadenceActive = (onTimeMs > 0 && offTimeMs > 0);

    _ringingActive = true;
    _lastToggle = millis();
    _cadenceStart = millis();
    _ringState = false;

    // ✅ NEW: enable ring mode
    digitalWrite(_pinRM, HIGH);
}

void Ag1171::stopRinging()
{
    _ringingActive = false;

    digitalWrite(_pinFR, LOW);

    // ✅ NEW: disable ring mode
    digitalWrite(_pinRM, LOW);
}