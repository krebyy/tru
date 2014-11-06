#ifndef TRIMPOT_H
#define TRIMPOT_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


#define SYSFS_ADC_DIR "/sys/bus/iio/devices/iio:device0"
#define MAX_BUF 64

class Trimpot
{
public:
	Trimpot(unsigned int channel, int out_min, int out_max);

    int getValue(void);
    int map(int in);

private:
    int channel;
    int out_min;
    int out_max;

};


#endif //TRIMPOT_H
