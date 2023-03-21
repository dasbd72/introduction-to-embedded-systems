#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#define AUTO 0
#define FREQ 16000000
#define VOL 0.5

uint16_t C = 262;

volatile uint8_t prev_PIND = 0x0;
volatile int play = 0;

ISR(PCINT2_vect) {
    int activate_PIND = PIND & ~prev_PIND;
    int deactivate_PIND = ~PIND & prev_PIND;

    if ((activate_PIND >> PD2) & 1) {
        play = 1;
        OCR1A = ICR1 * VOL;
    } else if ((deactivate_PIND >> PD2) & 1) {
        play = 0;
        OCR1A = 0;
    }

    prev_PIND = PIND;

    PORTB ^= (1 << PB5);
}

void setup() {
    // Input
    // Initialize input port D2
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    // === Output ===
    DDRB |= (1 << PB1);  // PWM
    PORTB |= (1 << PB1);

    DDRB |= (1 << PB5);  // Debug led
    PORTB |= (1 << PB5);

    // === PWM ===
    TCCR1A |= (1 << COM1A1);                // Enable PWM (D9)
    TCCR1A |= (1 << WGM11);                 // Fast PWM
    TCCR1B |= (1 << WGM12) | (1 << WGM13);  // Fast PWM
    TCCR1B |= (1 << CS10);
    ICR1 = 0;

    // === Interrupt ===
    PCICR |= (1 << PCIE2);     // Enable PCINT[23:16]
    PCMSK2 |= (1 << PCINT18);  // Enable PCINT18 (D2)
    sei();                     // Enable interrupts
}

void loop() {
    ICR1 = FREQ / (1 * C) - 1;

    // if (play == 1 | AUTO)
    //     OCR1A = ICR1 * VOL;

    if ((PIND >> PD2) & 1)
        OCR1A = ICR1 * VOL;
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
