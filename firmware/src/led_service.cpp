# include "service.h"
#include "led_service.h"

// constructor

LedBlinkService::LedBlinkService(int pinLedRed, int pinLedGreen, unsigned long intervalMs)
    : Service(intervalMs), ledRed(pinLedRed), ledGreen(pinLedGreen) {}

// inherited implementations

void LedBlinkService::setup()
{
    pinMode(this->ledRed, OUTPUT_12MA);
    pinMode(this->ledGreen, OUTPUT_12MA);

    setColor(BOARD_LED_COLOR_RED, true);
    setColor(BOARD_LED_COLOR_GREEN, false);
}

void LedBlinkService::run()
{
    if (this->cnt > 4)
    {
        this->setColor(BOARD_LED_COLOR_RED, false);
        this->cnt = 0;
    }
    else if (this->cnt == 1)
    {
        this->setColor(BOARD_LED_COLOR_RED, true);
    }
    this->cnt++;
}

// private implementation

void LedBlinkService::setColor(BOARD_LED_COLOR color, bool isOn)    // static color
{
    if(!isOn)
    {
        digitalWrite(this->ledRed, 0);
        digitalWrite(this->ledGreen, 0);
        return;
    }

    switch(color)
    {
        case BOARD_LED_COLOR_RED:
            digitalWrite(this->ledRed, 1);
            digitalWrite(this->ledGreen, 0);
            break;
        case BOARD_LED_COLOR_GREEN:
            digitalWrite(this->ledRed, 0);
            digitalWrite(this->ledGreen, 1);
            break;
        case BOARD_LED_COLOR_ORANGE:
            digitalWrite(this->ledRed, 1);
            digitalWrite(this->ledGreen, 1);
            break;
        default:
            digitalWrite(this->ledRed, 0);
            digitalWrite(this->ledGreen, 0);
            break;
    }
}

