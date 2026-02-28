#include "ag1171.h"

/*
   D1 Mini (ESP8266)

   FR  = D7 (GPIO13)
   RM  = D6 (GPIO12)
   PD  = D1 (GPIO5)
   SHK = D5 (GPIO14)
   BTN = D2 (GPIO4)
*/

constexpr uint8_t PIN_FR   = D7;
constexpr uint8_t PIN_RM   = D6;
constexpr uint8_t PIN_PD   = D1;
constexpr uint8_t PIN_SHK  = D5;
constexpr uint8_t PIN_BTN  = D2;

Ag1171 slic(PIN_FR, PIN_RM, PIN_SHK, PIN_PD, true);

constexpr uint32_t RING_DURATION_MS = 16000;
constexpr uint16_t RING_FREQUENCY   = 20;

// Dutch cadence
constexpr uint16_t RING_ON_MS  = 1000;
constexpr uint16_t RING_OFF_MS = 3000;

unsigned long ringStartTime = 0;
bool ringingActive = false;

// Button debounce
unsigned long lastButtonChange = 0;
bool buttonStable = HIGH;
constexpr uint16_t BUTTON_DEBOUNCE_MS = 20;

void setup()
{
    Serial.begin(115200);
    delay(100);

    pinMode(PIN_BTN, INPUT_PULLUP);

    slic.begin();

    Serial.println("Ready. Press button (D2) to ring.");
}

void loop()
{
    slic.update();

    bool raw = digitalRead(PIN_BTN);

    if (raw != buttonStable)
    {
        if (millis() - lastButtonChange > BUTTON_DEBOUNCE_MS)
        {
            buttonStable = raw;
            lastButtonChange = millis();

            if (buttonStable == LOW && !ringingActive)
            {
                Serial.println("Start ringing");

                slic.powerUp();
                slic.startRinging(RING_FREQUENCY,
                                  RING_ON_MS,
                                  RING_OFF_MS);

                ringStartTime = millis();
                ringingActive = true;
            }
        }
    }
    else
    {
        lastButtonChange = millis();
    }

    if (ringingActive && millis() - ringStartTime >= RING_DURATION_MS)
    {
        Serial.println("Stopping ring");

        slic.stopRinging();
        slic.powerDown();

        ringingActive = false;
    }
}