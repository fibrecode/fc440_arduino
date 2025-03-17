#if !defined(LEDS_H_)
#define LEDS_H_


#include <Arduino.h>
#include "elapsedMillis.h"

typedef enum {
    BOARD_LED_MODE_OFF = 0,
    BOARD_LED_MODE_ON = 1,
    BOARD_LED_MODE_BLINK_SLOW = 2,
    BOARD_LED_MODE_BLINK_FAST = 3,
    BOARD_LED_MODE_FLASHING = 4
} BOARD_LED_MODE;

typedef enum {
    BOARD_LED_COLOR_RED = 0,
    BOARD_LED_COLOR_GREEN = 1,
    BOARD_LED_COLOR_ORANGE = 2 
} BOARD_LED_COLOR;

#define LED_SLOW_BLINK_DELAY    600
#define LED_FAST_BLINK_DELAY    200
#define LED_FLASH_TIME          100

class BoardLed
{
    public:
        void begin(int pinLedRed, int pinLedGreen);
        void setLed(BOARD_LED_COLOR color, BOARD_LED_MODE mode);
        void process();

    private:
        int _pinLedRed;
        int _pinLedGreen;

        BOARD_LED_MODE _ledMode = BOARD_LED_MODE_OFF;
        BOARD_LED_COLOR _ledColor = BOARD_LED_COLOR_RED;
        int _ledState = 0;

        elapsedMillis _ledTimer;

        void setColor(BOARD_LED_COLOR color, bool isOn);
};



#endif /* LEDS_H_ */
