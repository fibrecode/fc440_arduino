#ifndef BLINKSERVICE_H
#define BLINKSERVICE_H

#include "Service.h"
#include <Arduino.h>

typedef enum {
    BOARD_LED_COLOR_RED = 0,
    BOARD_LED_COLOR_GREEN = 1,
    BOARD_LED_COLOR_ORANGE = 2 
} BOARD_LED_COLOR;


class LedBlinkService : public Service {
    private:
        int ledRed;
        int ledGreen;
        int cnt;
        void setColor(BOARD_LED_COLOR color, bool isOn);    // static color
public:
    LedBlinkService(int pinLedRed, int pinLedGreen, unsigned long intervalMS);

    void setup() override;
    void run() override;
};

#endif
