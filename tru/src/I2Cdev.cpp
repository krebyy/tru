/*
 * I2Cdev.cpp
 * Implementation of a class to support the BeagleBone Black implementation over the I2C bus
 * 2014-07-28 - Kleber Lima da Silva <kleber.ufu@hotmail.com>
 */

#include <iostream>
#include "I2Cdev.h"

using namespace std;


/** Default constructor.
 */
I2Cdev::I2Cdev(uint8_t devAddr)
{
    char buf[MAX_BUF];

    snprintf(buf, sizeof(buf), SYSFS_I2C_DIR);
    fd = open(buf, O_RDWR);

    if(ioctl(fd, I2C_SLAVE, devAddr) < 0) fd = -1;
}


/** Read a single bit from an 8-bit device register.
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readBit(uint8_t regAddr, uint8_t bitNum, uint8_t *data)
{
    uint8_t b;
    uint8_t count = readByte(regAddr, &b);
    *data = b & (1 << bitNum);
    return count;
}


/** Read multiple bits from an 8-bit device register.
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data)
{
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t count, b;
    if ((count = readByte(regAddr, &b)) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return count;
}

/** Read single byte from an 8-bit device register.
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @return Status of read operation (true = success)
 */
bool I2Cdev::readByte(uint8_t regAddr, uint8_t *data)
{
    return readBytes(regAddr, 1, data);
}


/** Read multiple bytes from an 8-bit device register.
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @return Number of bytes read (-1 indicates failure)
 */
bool I2Cdev::readBytes(uint8_t regAddr, uint8_t length, uint8_t *data)
{
    if(write(fd, &regAddr, 1) < 0) return false;
    if(read(fd, data, length) < 0) return false;

    return true;
}


/** write a single bit in an 8-bit device register.
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBit(uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    readByte(regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(regAddr, b);
}


/** Write multiple bits in an 8-bit device register.
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBits(uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (readByte(regAddr, &b) != 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return writeByte(regAddr, b);
    } else {
        return false;
    }
}


/** Write single byte to an 8-bit device register.
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeByte(uint8_t regAddr, uint8_t data)
{
    uint8_t buf[2];
    buf[0] = regAddr;
    buf[1] = data;

    if(write(fd, buf, 2) < 0) return false;

    return true;
}


/** Write multiple bytes to an 8-bit device register.
 * @param regAddr First register address to write to
 * @param length Number of bytes to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBytes(uint8_t regAddr, uint8_t length, uint8_t *data)
{
    uint8_t buf[length + 1];
    buf[0] = regAddr;
    for(uint8_t i = 1; i < length; i++) buf[i] = data[i-1];

    if(write(fd, buf, length + 1) < 0) return false;

    return true;
}


/** Write multiple words to a 16-bit device register.
 * @param regAddr First register address to write to
 * @param length Number of words to write
 * @param data Buffer to copy new data from
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeWord(uint8_t regAddr, uint16_t data)
{


    return true;
}
