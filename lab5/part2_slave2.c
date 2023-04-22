#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c_slave.h>
#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <util/delay.h>
#include <util/twi.h>

volatile uint8_t prev_PINB = 0xFF;
volatile uint8_t prev_PINC = 0xFF;
volatile uint8_t prev_PIND = 0xFF;
volatile uint8_t activate_PINB = 0xFF;
volatile uint8_t activate_PINC = 0xFF;
volatile uint8_t activate_PIND = 0xFF;
volatile uint8_t deactivate_PINB = 0xFF;
volatile uint8_t deactivate_PINC = 0xFF;
volatile uint8_t deactivate_PIND = 0xFF;

ISR(PCINT0_vect) {
    activate_PINB = PINB & ~prev_PINB;
    deactivate_PINB = ~PINB & prev_PINB;

    prev_PINB = PINB;
}
ISR(PCINT1_vect) {
    activate_PINC = PINC & ~prev_PINC;
    deactivate_PINC = ~PINC & prev_PINC;

    prev_PINC = PINC;
}
ISR(PCINT2_vect) {
    activate_PIND = PIND & ~prev_PIND;
    deactivate_PIND = ~PIND & prev_PIND;

    prev_PIND = PIND;
}

#define SLAVE_ADDRESS 0x21
volatile uint8_t receivedData;  // Global variable to store received data
volatile uint8_t changed = 0;

ISR(TWI_vect) {
    usart_sendhex(TW_STATUS);
    switch (TW_STATUS) {
        case TW_SR_SLA_ACK:       // SLA+W received, ACK returned
        case TW_SR_DATA_ACK:      // SLA+W received, ACK returned after a general call
            receivedData = TWDR;  // Read data from TWDR register
            usart_sendhex(receivedData);

            PORTB ^= (1 << PB5);
            break;
        case TW_ST_SLA_ACK:   // SLA+R received, ACK returned
        case TW_ST_DATA_ACK:  // Data byte transmitted, ACK returned
            if (((activate_PIND >> PD2) & 1)) {
                TWDR = '1';  // Write data to TWDR register
            } else {
                TWDR = '0';  // Write data to TWDR register
            }
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

void setup() {
    // === Ouput ===
    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    DDRD &= ~(1 << PD2);

    // === input ===

    // === UART ===
    usart_init();

    // === I2C ===
    i2c_init();
    i2c_sendaddr(SLAVE_ADDRESS);
    receivedData = 0;

    // === Interrupt ===
    PCICR |= (1 << PCIE2);     // Enable PCINT[23:16]
    PCMSK2 |= (1 << PCINT18);  // Enable PCINT18 (D2)
    sei();                     // Enable interrupts
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
