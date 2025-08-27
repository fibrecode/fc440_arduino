
#include "leds.h"


void BoardLed::begin(int pinLedRed, int pinLedGreen)
{
    this->_pinLedRed = pinLedRed;
    this->_pinLedGreen = pinLedGreen;
    pinMode(this->_pinLedRed, OUTPUT_12MA);
    pinMode(this->_pinLedGreen, OUTPUT_12MA);

    setColor(BOARD_LED_COLOR_RED, true);
    setColor(BOARD_LED_COLOR_GREEN, false);
}

void BoardLed::setLed(BOARD_LED_COLOR color, BOARD_LED_MODE mode)
{
    this->_ledColor = color;
    if(this->_ledMode != mode)
    {
        this->_ledMode = mode;
        this->_ledState = 0;
    }
}

void BoardLed::setLed2(BOARD_LED_COLOR color1, BOARD_LED_COLOR color2, BOARD_LED_MODE mode)
{
    this->_ledColor = color1;
    this->_ledColor2 = color2;

    if(this->_ledMode != mode)
    {
        this->_ledMode = mode;
        this->_ledState = 0;
    }
}

void BoardLed::process()
{            
    unsigned long blinkDelay = LED_SLOW_BLINK_DELAY;

    switch(this->_ledMode)
    {
        case BOARD_LED_MODE_OFF:
            setColor(this->_ledColor, false);
            return;
        case BOARD_LED_MODE_ON:
            setColor(this->_ledColor, true);
            return;
        case BOARD_LED_MODE_BLINK_FAST:
            blinkDelay = LED_FAST_BLINK_DELAY;
            break;
    }

    if (this->_ledState == 0)
    {
        if (this->_ledTimer > blinkDelay)
        {
            this->_ledState = 1;
            this->_ledTimer = 0;
            
            if (this->_ledMode == BOARD_LED_MODE_FLASHING_BI)
            {
                setColor(this->_ledColor2, false);
                setColor(this->_ledColor, true);
            }
            else
                setColor(this->_ledColor, true);
        }
    }
    else
    {
        if (this->_ledMode == BOARD_LED_MODE_FLASHING)
        {
            if (this->_ledTimer > LED_FLASH_TIME)
            {
                this->_ledState = 0;
                this->_ledTimer = 0;
                setColor(this->_ledColor, false);
            }
        } else if (this->_ledMode == BOARD_LED_MODE_FLASHING_BI)
        {
            if (this->_ledTimer > LED_FLASH_TIME)
            {
                this->_ledState = 0;
                this->_ledTimer = 0;
                if (this->_ledToggle == 0)
                {
                    setColor(this->_ledColor, false);
                    setColor(this->_ledColor2, true);
                    this->_ledToggle = 1;
                }
                else
                {
                    setColor(this->_ledColor2, false);
                    setColor(this->_ledColor, true);
                    this->_ledToggle = 0;
                }
            }
        }
        else
        {
            if (this->_ledTimer > blinkDelay)
            {
                this->_ledState = 0;
                this->_ledTimer = 0;
                setColor(this->_ledColor, false);
            }
        }
    }
}


void BoardLed::setColor(BOARD_LED_COLOR color, bool isOn)
{
    if(!isOn)
    {
        digitalWrite(this->_pinLedRed, 0);
        digitalWrite(this->_pinLedGreen, 0);
        return;
    }

    switch(color)
    {
        case BOARD_LED_COLOR_RED:
            digitalWrite(this->_pinLedRed, 1);
            digitalWrite(this->_pinLedGreen, 0);
            break;
        case BOARD_LED_COLOR_GREEN:
            digitalWrite(this->_pinLedRed, 0);
            digitalWrite(this->_pinLedGreen, 1);
            break;
        case BOARD_LED_COLOR_ORANGE:
            digitalWrite(this->_pinLedRed, 1);
            digitalWrite(this->_pinLedGreen, 1);
            break;
        default:
            digitalWrite(this->_pinLedRed, 0);
            digitalWrite(this->_pinLedGreen, 0);
            break;
    }
}
