#ifndef BUTTONS_H
#define BUTTONS_H

#include "SimpleGPIO.h"
#include <unistd.h>

enum buttons_on_t
{
	FALSE = 0,
	BOTH,
    LEFT = 68,
    RIGHT
};

class Buttons
{
public:
	Buttons();

	buttons_on_t getStatus(void);
};

#endif	// BUTTONS_H
