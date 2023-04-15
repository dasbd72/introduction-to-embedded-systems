#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <usart.h>
#include <util/delay.h>

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

volatile uint8_t usart_rx_data[100];
volatile int usart_rx_idx = 0;
volatile uint8_t usart_rxc = 0;

void int_usart_recievebyte() {
    uint8_t data = UDR0;
    if (data == '\r') {
        usart_rx_data[usart_rx_idx] = '\0';
        usart_rx_idx = 0;
        usart_rxc = 1;
    } else {
        usart_rx_data[usart_rx_idx++] = data;
        usart_rxc = 0;
    }
}

ISR(USART_RX_vect) {
    cli();
    if (UCSR0A & (1 << RXC0)) {
        int_usart_recievebyte();
    }
    sei();
}

// #define SLAVE_ADDRESS 0x20
// volatile uint8_t receivedData;

// ISR(TWI_vect) {
//     switch (TW_STATUS) {
//         case TW_SR_SLA_ACK:       // SLA+W received, ACK returned
//         case TW_SR_DATA_ACK:      // SLA+W received, ACK returned after a general call
//             receivedData = TWDR;  // Read data from TWDR register
//             break;
//         case TW_ST_SLA_ACK:       // SLA+R received, ACK returned
//         case TW_ST_DATA_ACK:      // Data byte transmitted, ACK returned
//             TWDR = receivedData;  // Write data to TWDR register
//             break;
//         case TW_ST_DATA_NACK:  // Data byte transmitted, NACK returned
//         case TW_ST_LAST_DATA:  // Last data byte transmitted, ACK returned
//             break;
//         case TW_BUS_ERROR:     // Bus error occurred
//         case TW_MT_SLA_NACK:   // SLA+W received, NACK returned
//         case TW_MT_DATA_NACK:  // Data byte transmitted, NACK returned
//         default:               // default
//             break;
//     }
//     TWCR = (1 << TWIE) | (1 << TWEA) | (1 << TWEN) | (1 << TWINT);  // Enable TWI, Acknowledge on
// }

void loop() {
    if (usart_rxc == 1) {
        cli();
        usart_sendstr("RECEIVED: ");
        usart_sendstr(usart_rx_data);
        usart_sendstr("LISTEN");
        usart_rxc = 0;
        sei();
    }
}

void setup() {
    // === Output ===
    // for (int i = PD0; i <= PD7; i++) {
    //     DDRD |= _BV(i);
    //     PORTD &= ~_BV(i);
    // }
    // for (int i = PC0; i <= PC6; i++) {
    //     DDRC |= _BV(i);
    //     PORTC &= ~_BV(i);
    // }
    // for (int i = PB0; i <= PB7; i++) {
    //     if (i != PB5) {
    //         DDRB |= _BV(i);
    //         PORTB &= ~_BV(i);
    //     }
    // }

    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // Timer Output
    // DDRB |= _BV(PB1);  // OC1A (D9)
    // DDRB |= _BV(PB2);  // OC1B (D10)
    // DDRB |= _BV(PB3);  // OC2A (D11)
    // DDRD |= _BV(PD3);  // OC2B (D3)
    // DDRD |= _BV(PD5);  // OC0B (D5)
    // DDRD |= _BV(PD6);  // OC0A (D6)

    // === Input ===
    // for (int i = PD0; i <= PD7; i++) {
    //     DDRD &= ~_BV(i);
    // }
    // for (int i = PC0; i <= PC6; i++) {
    //     DDRC &= ~_BV(i);
    // }
    // for (int i = PB0; i <= PB7; i++) {
    //     DDRB &= ~_BV(i);
    // }

    // === PWM ===
    // TCCR1A |= (1 << COM1A1);                // Enable PWM (D9)
    // TCCR1A |= (1 << COM1B1);                // Enable PWM (D10)
    // TCCR1A |= (1 << WGM11);                 // Fast PWM
    // TCCR1B |= (1 << WGM12) | (1 << WGM13);  // Fast PWM
    // TCCR1B |= (1 << CS10);
    // ICR1 = 0xFF;
    // TCCR2A |= (1 << COM2A1);                // Enable PWM (D11)
    // TCCR2A |= (1 << WGM20) | (1 << WGM21);  // Fast PWM, TOP = 0xFF
    // TCCR2B |= (1 << CS20);

    // === UART ===
    // usart_init();
    int_usart_init();

    // === I2C ===
    // i2c_init();
    // receivedData = 0;
    // i2c_address(SLAVE_ADDRESS);

    // === Interrupt ===
    // PCICR |= (1 << PCIE0);     // Enable PCINT[7:0]
    // PCMSK0 |= (1 << PCINT0);   // Enable PCINT0 (B0)
    // PCMSK0 |= (1 << PCINT1);   // Enable PCINT1 (B1)
    // PCMSK0 |= (1 << PCINT2);   // Enable PCINT2 (B2)
    // PCMSK0 |= (1 << PCINT3);   // Enable PCINT3 (B3)
    // PCMSK0 |= (1 << PCINT4);   // Enable PCINT4 (B4)
    // PCMSK0 |= (1 << PCINT5);   // Enable PCINT5 (B5)
    // PCMSK0 |= (1 << PCINT6);   // Enable PCINT6 (B6)
    // PCMSK0 |= (1 << PCINT7);   // Enable PCINT7 (B7)
    // PCICR |= (1 << PCIE1);     // Enable PCINT[15:8]
    // PCMSK1 |= (1 << PCINT8);   // Enable PCINT8 (C0)
    // PCMSK1 |= (1 << PCINT9);   // Enable PCINT9 (C1)
    // PCMSK1 |= (1 << PCINT10);  // Enable PCINT10 (C2)
    // PCMSK1 |= (1 << PCINT11);  // Enable PCINT11 (C3)
    // PCMSK1 |= (1 << PCINT12);  // Enable PCINT12 (C4)
    // PCMSK1 |= (1 << PCINT13);  // Enable PCINT13 (C5)
    // PCMSK1 |= (1 << PCINT14);  // Enable PCINT14 (C6)
    // PCICR |= (1 << PCIE2);     // Enable PCINT[23:16]
    // PCMSK2 |= (1 << PCINT16);  // Enable PCINT16 (D0)
    // PCMSK2 |= (1 << PCINT17);  // Enable PCINT17 (D1)
    // PCMSK2 |= (1 << PCINT18);  // Enable PCINT18 (D2)
    // PCMSK2 |= (1 << PCINT19);  // Enable PCINT19 (D3)
    // PCMSK2 |= (1 << PCINT20);  // Enable PCINT20 (D4)
    // PCMSK2 |= (1 << PCINT21);  // Enable PCINT21 (D5)
    // PCMSK2 |= (1 << PCINT22);  // Enable PCINT22 (D6)
    // PCMSK2 |= (1 << PCINT23);  // Enable PCINT23 (D7)
    sei();  // Enable interrupts
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
