#ifndef PWM_H
#define PWM_H

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#define SYSFS_PWM_DIR "/sys/devices/ocp.2/pwm_test_"
#define MAX_BUF 64
#define SERVO "P8_13.10"
#define SERVO_PERIODO 20000000
#define SERVO_NEUTRO 1550000
#define ESC "P9_16.12"
#define ESC_PERIODO 3000000
#define ESC_NEUTRO 1500000


class Pwm
{
public:
	Pwm();

    bool initServo(void);
    bool initESC(void);
    bool updateServo(int duty);
    bool updateESC(int duty);
    bool calibraESC(void);
    bool freiaESC(void);

private:
    bool configPWM(const char* pino, unsigned long periodo, unsigned long duty, int polaridade);
    bool setDuty(const char* pino, unsigned long duty);
};

#endif // PWM_H
