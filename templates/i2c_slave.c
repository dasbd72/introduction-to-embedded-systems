#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define SLAVE_ADDRESS 0x20
#define I2C_BIT_RATE (((F_CPU / 100000UL) - 16) / 2)

volatile uint8_t receivedData;  // Global variable to store received data

void i2c_init() {
    TWSR = 0x00;                                      // Set prescaler to 1
    TWBR = I2C_BIT_RATE;                              // Set bit rate
    TWAR = (SLAVE_ADDRESS << 1);                      // Set slave address
    TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);  // Enable TWI, Acknowledge on
    receivedData = 0;                                 // Initialize received data to 0
}

ISR(TWI_vect) {
    uint8_t status = TWSR & 0xF8;  // Read TWI status register and mask out prescaler bits

    switch (status) {
        case 0x60:                               // SLA+W received, ACK returned
        case 0x70:                               // SLA+W received, ACK returned after a general call
            receivedData = TWDR;                 // Read data from TWDR register
            TWCR |= (1 << TWEA) | (1 << TWINT);  // Enable TWI, Acknowledge on
            break;
        case 0xA8:                               // SLA+R received, ACK returned
        case 0xB8:                               // Data byte transmitted, ACK returned
            TWDR = receivedData;                 // Write data to TWDR register
            TWCR |= (1 << TWEA) | (1 << TWINT);  // Enable TWI, Acknowledge on
            break;
        case 0xC0:                               // Data byte transmitted, NACK returned
        case 0xC8:                               // Last data byte transmitted, ACK returned
            TWCR |= (1 << TWEA) | (1 << TWINT);  // Enable TWI, Acknowledge on
            break;
        case 0x00:                                              // Bus error occurred
        case 0x20:                                              // SLA+W received, NACK returned
        case 0x30:                                              // Data byte transmitted, NACK returned
        default:                                                // default
            TWCR |= (1 << TWEA) | (1 << TWINT) | (1 << TWSTO);  // Enable TWI, Acknowledge on, Stop condition
            break;
    }
}

void setup() {
    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // === UART ===
    // uart_init();

    // === I2C ===
    i2c_init();

    // === Interrupt ===
    sei();  // Enable interrupts
}

void loop() {
    // Perform any tasks with the received data
    if (receivedData == 'H') {
        // If received 'H', turn on the LED
        PORTB |= (1 << PB5);
    } else {
        // Otherwise, turn off the LED
        PORTB &= ~(1 << PB5);
    }

    _delay_ms(100);
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
