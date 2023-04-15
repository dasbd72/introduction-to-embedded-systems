#ifndef _I2C_SLAVE_H
#define _I2C_SLAVE_H

#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#define I2C_CONFIG_F_SCL 100000
#define I2C_BIT_RATE (((F_CPU / I2C_CONFIG_F_SCL) - 16) / 2)

void i2c_init() {
    TWSR = 0x00;                                                    // Set prescaler to 1
    TWBR = I2C_BIT_RATE;                                            // Set bit rate
    TWCR = (1 << TWIE) | (1 << TWEA) | (1 << TWEN) | (1 << TWINT);  // Enable TWI, Acknowledge on
}

void i2c_address(int address) {
    TWAR = (address << 1);  // Set slave address
}

#endif  // _I2C_SLAVE_H