#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
// #define MAX 255
#define MAX 25
#define ADJ 5

volatile uint8_t prev_PIND = 0xFF;
volatile uint8_t R = MAX;
volatile uint8_t G = MAX;
volatile uint8_t B = MAX;

ISR(PCINT2_vect) {
    int activate_PIND = PIND & ~prev_PIND;

    if ((activate_PIND >> PD2) & 1) {
        R = R + ADJ > MAX ? MAX : R + ADJ;
    }
    if ((activate_PIND >> PD3) & 1) {
        R = R - ADJ < 0 ? 0 : R - ADJ;
    }
    if ((activate_PIND >> PD4) & 1) {
        G = G + ADJ > MAX ? MAX : G + ADJ;
    }
    if ((activate_PIND >> PD5) & 1) {
        G = G - ADJ < 0 ? 0 : G - ADJ;
    }
    if ((activate_PIND >> PD6) & 1) {
        B = B + ADJ > MAX ? MAX : B + ADJ;
    }
    if ((activate_PIND >> PD7) & 1) {
        B = B - ADJ < 0 ? 0 : B - ADJ;
    }

    prev_PIND = PIND;

    PORTB ^= (1 << PB5);
}

void setup() {
    // Input
    // Initialize input port D2 ~ D7

    for (int i = 2; i <= 7; i++) {
        DDRD &= ~(1 << i);
        PORTD |= (1 << i);
    }

    // === Output ===
    DDRB |= (1 << PB1);  // PWM
    DDRB |= (1 << PB2);  // PWM
    DDRB |= (1 << PB3);  // PWM

    DDRB |= (1 << PB5);  // Debug led
    PORTB |= (1 << PB5);

    // === Interrupt ===
    PCICR |= (1 << PCIE2);     // Enable PCINT[23:16]
    PCMSK2 |= (1 << PCINT18);  // Enable PCINT18 (D2)
    PCMSK2 |= (1 << PCINT19);  // Enable PCINT19 (D3)
    PCMSK2 |= (1 << PCINT20);  // Enable PCINT20 (D4)
    PCMSK2 |= (1 << PCINT21);  // Enable PCINT21 (D5)
    PCMSK2 |= (1 << PCINT22);  // Enable PCINT22 (D6)
    PCMSK2 |= (1 << PCINT23);  // Enable PCINT23 (D7)
    sei();                     // Enable interrupts

    // === PWM ===
    TCCR1A |= (1 << COM1A1);                // Enable PWM (D9)
    TCCR1A |= (1 << COM1B1);                // Enable PWM (D10)
    TCCR1A |= (1 << WGM11);                 // Fast PWM
    TCCR1B |= (1 << WGM12) | (1 << WGM13);  // Fast PWM
    TCCR1B |= (1 << CS11);
    ICR1 = 0xFF;  // Set ICR1 to 0xFF

    TCCR2A |= (1 << COM2A1);                // Enable PWM (D11)
    TCCR2A |= (1 << WGM20) | (1 << WGM21);  // Fast PWM, TOP = 0xFF
    TCCR2B |= (1 << CS20);
}

void loop() {
    OCR1A = R;
    OCR1B = G;
    OCR2A = B;
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
