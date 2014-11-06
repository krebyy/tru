#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#include <stdint.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#define SYSFS_I2C_DIR "/dev/i2c-1"
#define MAX_BUF 64
#define I2CDEV_DEFAULT_READ_TIMEOUT     0

class I2Cdev {
    public:
        I2Cdev(uint8_t devAddr);

        int8_t readBit(uint8_t regAddr, uint8_t bitNum, uint8_t *data);
        int8_t readBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
        bool readByte(uint8_t regAddr, uint8_t *data);
        bool readBytes(uint8_t regAddr, uint8_t length, uint8_t *data);

        bool writeBit(uint8_t regAddr, uint8_t bitNum, uint8_t data);
        bool writeBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        bool writeByte(uint8_t regAddr, uint8_t data);
        bool writeBytes(uint8_t regAddr, uint8_t length, uint8_t *data);
        bool writeWord(uint8_t regAddr, uint16_t data);

        uint32_t fd;
};


#endif /* _I2CDEV_H_ */
