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
volatile uint8_t receivedData;  // Global variable to store received data
volatile uint8_t mode = 0;

static void i2c_wait(void) {
    while (!(TWCR & _BV(TWINT)))
        ;
}

int8_t i2c_tx_byte(uint8_t byte) {
    TWDR = byte;
    TWCR = _BV(TWEN) | _BV(TWINT);
    i2c_wait();
    return (TWSR & TW_STATUS_MASK) != TW_MT_DATA_ACK;
}

int8_t i2c_stop(void) {
    TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWSTO);
    while (TWCR & _BV(TWSTO))
        ;
    return 0;
}

ISR(TWI_vect) {
    usart_sendhex(TW_STATUS);
    switch (TW_STATUS) {
        case TW_SR_SLA_ACK:       // SLA+W received, ACK returned
        case TW_SR_DATA_ACK:      // SLA+W received, ACK returned after a general call
            receivedData = TWDR;  // Read data from TWDR register
            if (receivedData == '0') {
                mode += 1;
            }
            break;
        case TW_ST_SLA_ACK:       // SLA+R received, ACK returned
        case TW_ST_DATA_ACK:      // Data byte transmitted, ACK returned
            TWDR = receivedData;  // Write data to TWDR register
            break;
        case TW_ST_DATA_NACK:  // Data byte transmitted, NACK returned
        case TW_ST_LAST_DATA:  // Last data byte transmitted, ACK returned
            break;
        case TW_BUS_ERROR:     // Bus error occurred
        case TW_MT_SLA_NACK:   // SLA+W received, NACK returned
        case TW_MT_DATA_NACK:  // Data byte transmitted, NACK returned
        default:               // default
            break;
    }
    TWCR = (1 << TWIE) | (1 << TWEA) | (1 << TWEN) | (1 << TWINT);  // Enable TWI, Acknowledge on
}

volatile uint8_t prev_PINC = 0xFF;
volatile uint8_t activate_PINC = 0xFF;
volatile uint8_t deactivate_PINC = 0xFF;

ISR(PCINT1_vect) {
    activate_PINC = PINC & ~prev_PINC;
    deactivate_PINC = ~PINC & prev_PINC;

    if ((activate_PINC >> PC0) & 1) {
        i2c_address(SLAVE_ADDRESS_1);
        i2c_tx_byte('0');
        i2c_stop();
    }
    if ((activate_PINC >> PC1) & 1) {
        i2c_address(SLAVE_ADDRESS_2);
        i2c_tx_byte('1');
        i2c_stop();
    }

    prev_PINC = PINC;
}

void setup() {
    // === Ouput ===
    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    DDRC &= ~(1 << PC0);
    DDRC &= ~(1 << PC1);

    // === input ===

    // === UART ===
    usart_init();

    // === I2C ===
    i2c_init();
    receivedData = 0;

    // === Interrupt ===
    sei();  // Enable interrupts
}

void loop() {
    // if (mode == 1) {
    //     PORTB |= (1 << PB5);
    // }
    // if (mode == 2) {
    //     PORTD |= (1 << PD2);
    // }
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
