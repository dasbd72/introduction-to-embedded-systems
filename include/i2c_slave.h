#ifndef _I2C_SLAVE_H
#define _I2C_SLAVE_H

#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#define I2C_BIT_RATE (((F_CPU / I2C_CONFIG_F_SCL) - 16) / 2)

void i2c_init() {
    TWBR = I2C_BIT_RATE;                                                                                        // Set I2C bit rate
    TWCR = (1 << TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (1 << TWIE);  // Enable TWI interrupt, Acknowledge on, Enable TWI, Call TWI interrupt
    TWSR = 0x00;                                                                                                // Set prescaler to 1
}

void i2c_sendaddr(int address) {
    TWAR = (address << 1);  // Set slave address
}

#endif  // _I2C_SLAVE_H