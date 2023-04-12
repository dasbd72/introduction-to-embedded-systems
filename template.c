#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (F_CPU / 16 / BAUDRATE - 1)

volatile uint8_t activate_PINB = 0xFF;
volatile uint8_t activate_PINC = 0xFF;
volatile uint8_t activate_PIND = 0xFF;
volatile uint8_t prev_PINB = 0xFF;
volatile uint8_t prev_PINC = 0xFF;
volatile uint8_t prev_PIND = 0xFF;

ISR(PCINT0_vect) {
    activate_PINB = PINB & ~prev_PINB;
    prev_PINB = PINB;
}
ISR(PCINT1_vect) {
    activate_PINC = PINC & ~prev_PINC;
    prev_PINC = PINC;
}
ISR(PCINT2_vect) {
    activate_PIND = PIND & ~prev_PIND;
    prev_PIND = PIND;
}

void uart_init(void) {
    UBRR0H = (uint8_t)(BAUD_PRESCALLER >> 8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (3 << UCSZ00);
}

void uart_sendbyte(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

uint8_t uart_receivebyte(void) {
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

void setup() {
    // Membrane Rows
    for (int i = 6; i <= 7; i++) {
        DDRD |= (1 << i);
    }
    for (int i = 0; i <= 1; i++) {
        DDRB |= (1 << i);
    }

    // Membrane Columns
    for (int i = 2; i <= 5; i++) {
        DDRD &= ~(1 << i);
    }

    // Built in led ouptut
    DDRB |= (1 << PB5);  // Debug led
    PORTB &= ~(1 << PB5);

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
    // sei();                     // Enable interrupts
}

void loop() {
}

int main(void) {
    setup();
    uart_init();
    while (1) {
        loop();
    }
    return 0;
}
