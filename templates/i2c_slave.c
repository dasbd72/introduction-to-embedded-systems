#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c_slave.h>
#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <util/delay.h>
#include <util/twi.h>

#define SLAVE_ADDRESS 0x20
volatile uint8_t receivedData;  // Global variable to store received data

ISR(TWI_vect) {
    usart_sendhex(TW_STATUS);
    switch (TW_STATUS) {
        case TW_SR_SLA_ACK:       // SLA+W received, ACK returned
        case TW_SR_DATA_ACK:      // SLA+W received, ACK returned after a general call
            receivedData = TWDR;  // Read data from TWDR register
            usart_sendint(receivedData);
            if (receivedData >= 1 && receivedData <= 9) {
                PORTB |= (1 << PB5);
                usart_sendbyte(receivedData);
            } else {
                PORTB &= ~(1 << PB5);
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
    i2c_sendaddr(SLAVE_ADDRESS);
    receivedData = 0;

    // === Interrupt ===
    sei();  // Enable interrupts
}

void loop() {
    _delay_ms(1000);
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
