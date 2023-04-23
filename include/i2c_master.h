#ifndef _I2C_SLAVE_H
#define _I2C_SLAVE_H

#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#define I2C_BIT_RATE (((F_CPU / I2C_CONFIG_F_SCL) - 16) / 2)
#define I2C_CONFIG_DDR DDRC
#define I2C_CONFIG_PORT PORTC
#define I2C_CONFIG_SDA PC4
#define I2C_CONFIG_SCL PC5
#define I2C_TRANSFER 0
#define I2C_RECEIVE 1

typedef struct i2c_trxn_t {
    uint8_t addr;  // Target address
    uint8_t mode;  // Target mode (I2C_TRANSFER or I2C_RECEIVE)
    uint8_t buf[100];
    uint8_t size;
    uint8_t idx;
} i2c_trxn_t;

void i2c_trxn_init(i2c_trxn_t* volatile trxn) {
    trxn->size = 0;
    trxn->idx = 0;
}

void i2c_trxn_setbtye(i2c_trxn_t* volatile trxn, uint8_t addr, uint8_t data) {
    trxn->addr = addr;
    trxn->mode = I2C_TRANSFER;
    trxn->buf[0] = data;
    trxn->size = 1;
    trxn->idx = 0;
}

void i2c_trxn_wait(i2c_trxn_t* volatile trxn) {
    while (trxn->idx < trxn->size)
        ;
}

void i2c_trxn_start(i2c_trxn_t* volatile trxn) {
    trxn->idx = 0;
    i2c_sendaddr(trxn->addr, trxn->mode);
}

void i2c_trxn_send_stop(i2c_trxn_t* volatile trxn) {
    if (trxn->idx < trxn->size) {
        i2c_sendbyte(trxn->buf[trxn->idx++]);
    } else {
        i2c_stop();
    }
}

void i2c_init() {
    TWBR = I2C_BIT_RATE;                                                                                        // Set I2C bit rate
    TWCR = (0 << TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (1 << TWIE);  // Enable TWI interrupt, Acknowledge on, Enable TWI, Call TWI interrupt
    TWSR = 0x00;                                                                                                // Set prescaler to 1
    TWAR = 0x00;                                                                                                // Disable slave mode
}

void i2c_init_with_pullups(void) {
    I2C_CONFIG_DDR &= ~((1 << I2C_CONFIG_SDA) | (1 << I2C_CONFIG_SCL));
    I2C_CONFIG_PORT |= (1 << I2C_CONFIG_SDA) | (1 << I2C_CONFIG_SCL);
    i2c_init();
}

void i2c_start() {
    // Send start condition
    TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWSTA) | (0 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (1 << TWIE);
}

static void i2c_wait(void) {
    while (!(TWCR & (1 << TWINT)))
        ;
}

void i2c_stop() {
    // Send stop condition
    TWCR = (1 << TWINT) | (1 << TWEA) | (0 << TWSTA) | (1 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (1 << TWIE);
}

void i2c_uninit(void) {
    TWCR &= ~(1 << TWEN);
}

void i2c_sendaddr(int address, int op) {
    TWDR = address << 1;
    TWCR = (1 << TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (1 << TWIE);
}

void i2c_sendbyte(uint8_t data) {
    // Write data to the I2C bus
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEA) | (0 << TWSTA) | (0 << TWSTO) | (0 << TWWC) | (1 << TWEN) | (1 << TWIE);
}

#endif  // _I2C_SLAVE_H