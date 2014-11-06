#ifndef LEDS_H
#define LEDS_H

#include "SimpleGPIO.h"

#define ON	LOW
#define OFF	HIGH

enum led_color_t
{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    WHITE,
    NONE
};

class Leds
{
public:
    Leds();

    void setColor(led_color_t color);
    void ledsOff();
    void setFarol(PIN_VALUE);

private:
    unsigned int LED_RED;
    unsigned int LED_GREEN;
    unsigned int LED_BLUE;
    
    unsigned int FAROL;
};

#endif // LEDS_H
