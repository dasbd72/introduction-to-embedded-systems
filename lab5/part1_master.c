#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c_master.h>
#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <util/delay.h>
#include <util/twi.h>

#define SLAVE_ADDRESS_1 0x20
#define SLAVE_ADDRESS_2 0x21

volatile uint8_t i2c_addr;  // Global variable to store received data
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
            i2c_sendaddr(i2c_addr, I2C_TRANSFER);
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

volatile uint8_t prev_PINC = 0xFF;
volatile uint8_t activate_PINC = 0xFF;
volatile uint8_t deactivate_PINC = 0xFF;

ISR(PCINT1_vect) {
    activate_PINC = PINC & ~prev_PINC;
    deactivate_PINC = ~PINC & prev_PINC;

    PORTB ^= (1 << PB5);

    if ((activate_PINC >> PC0) & 1) {
        i2c_size = 1;
        i2c_buffer[0] = '0';
        i2c_addr = SLAVE_ADDRESS_1;
        i2c_start();
    }
    if ((activate_PINC >> PC1) & 1) {
        i2c_size = 1;
        i2c_buffer[0] = '0';
        i2c_addr = SLAVE_ADDRESS_2;
        i2c_start();
    }

    prev_PINC = PINC;
}

void setup() {
    // === Ouput ===
    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // === input ===
    DDRC &= ~(1 << PC0);
    DDRC &= ~(1 << PC1);

    // === UART ===
    usart_init();

    // === I2C ===
    i2c_init();

    // === Interrupt ===
    PCICR |= (1 << PCIE1);    // Enable PCINT[15:8]
    PCMSK1 |= (1 << PCINT8);  // Enable PCINT8 (C0)
    PCMSK1 |= (1 << PCINT9);  // Enable PCINT9 (C1)
    sei();                    // Enable interrupts
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
