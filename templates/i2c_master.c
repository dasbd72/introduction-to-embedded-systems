#include <avr/io.h>
#include <util/delay.h>

#define SLAVE_ADDRESS 0x20
#define I2C_BIT_RATE (((F_CPU / 100000UL) - 16) / 2)

void i2c_init() {
    TWSR = 0x00;          // Set prescaler to 1
    TWBR = I2C_BIT_RATE;  // Set bit rate
    TWCR = (1 << TWEN);   // Enable TWI
}

void i2c_start() {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);  // Send start condition
    while (!(TWCR & (1 << TWINT)))
        ;  // Wait for start condition to be transmitted
}

void i2c_stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);  // Send stop condition
}

void i2c_write(uint8_t data) {
    TWDR = data;                        // Load data into TWDR register
    TWCR = (1 << TWINT) | (1 << TWEN);  // Start transmission
    while (!(TWCR & (1 << TWINT)))
        ;  // Wait for data to be transmitted
}

uint8_t i2c_read_ack() {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);  // Enable ACK and start reception
    while (!(TWCR & (1 << TWINT)))
        ;         // Wait for data to be received
    return TWDR;  // Read data from TWDR register
}

uint8_t i2c_read_nack() {
    TWCR = (1 << TWINT) | (1 << TWEN);  // Enable NACK and start reception
    while (!(TWCR & (1 << TWINT)))
        ;         // Wait for data to be received
    return TWDR;  // Read data from TWDR register
}

void i2c_send_data(uint8_t slave_addr, uint8_t data) {
    i2c_start();                 // Send start condition
    i2c_write(slave_addr << 1);  // Send slave address with write bit
    i2c_write(data);             // Send data
    i2c_stop();                  // Send stop condition
}

uint8_t i2c_receive_data() {
    i2c_start();                             // Send start condition
    i2c_write((SLAVE_ADDRESS << 1) | 0x01);  // Send slave address with read bit
    uint8_t receivedData = i2c_read_nack();  // Receive data with NACK
    i2c_stop();                              // Send stop condition
    return receivedData;                     // Return received data
}

void setup() {
    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // === I2C ===
    i2c_init();

    // === Interrupt ===
    sei();  // Enable interrupts
}

void loop() {
    // Send 'H' to Slave and receive response
    i2c_send_data(SLAVE_ADDRESS, 'H');
    uint8_t response = i2c_receive_data();

    // Perform any tasks with the received data
    if (response == 'L') {
        // If received 'L', turn off an LED or do other tasks
        // ...
    } else {
        // Otherwise, turn on the LED or do other tasks
        // ...
    }

    _delay_ms(1000);
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
