#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c_master.h>
#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <util/delay.h>
#include <util/twi.h>

volatile uint8_t i2c_addr;  // Target address
volatile uint8_t i2c_mode;  // Target mode (I2C_TRANSFER or I2C_RECEIVE)
volatile uint8_t i2c_data;  // Global variable to store received data
volatile uint8_t i2c_buffer[10];
volatile uint8_t i2c_size = 0;
volatile uint8_t i2c_index = 0;

ISR(TWI_vect) {
    usart_sendhex(TW_STATUS);
    switch (TW_STATUS) {
        case TW_START:        // Start condition transmitted
        case TW_REP_START:    // Repeated start condition transmitted
        case TW_MT_ARB_LOST:  // Arbitration lost in SLA+W or data
            // Send slave address
            i2c_index = 0;
            i2c_address(i2c_addr, I2C_TRANSFER);
            break;
        case TW_MT_SLA_ACK:   // SLA+W transmitted, ACK received = Slave receiver ACKed address
        case TW_MT_DATA_ACK:  // Data transmitted, ACK received = Slave receiver ACKed data
            // Send data
            if (i2c_index < i2c_size) {
                i2c_sendbyte(i2c_buffer[i2c_index++]);
            } else {
                i2c_stop();
            }
            break;
        case TW_BUS_ERROR:     // Bus error; Illegal START or STOP condition
        case TW_MT_SLA_NACK:   // SLA+W transmitted, NACK received = Slave receiver with transmitted address doesn't exists?
        case TW_MT_DATA_NACK:  // Data transmitted, NACK received
        default:
            i2c_stop();
            break;
    }
}

void setup() {
    // === Ouput ===
    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // === input ===

    // === UART ===
    usart_init();

    // === I2C ===
    i2c_init();

    // === Interrupt ===
    sei();  // Enable interrupts
}

void loop() {
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
