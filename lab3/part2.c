#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#define AUTO 0
#define FREQ 16000000
#define VOL 0.3

uint16_t melody[] = {262, 262, 392, 392, 440, 440, 392,
                     349, 349, 330, 330, 294, 294, 262,
                     392, 392, 349, 349, 330, 330, 294,
                     392, 392, 349, 349, 330, 330, 294,
                     262, 262, 392, 392, 440, 440, 392,
                     349, 349, 330, 330, 294, 294, 262};

volatile int counter = 0;
volatile uint8_t prev_PIND = 0xFF;
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
    ICR1 = FREQ / (1 * melody[counter]) - 1;

    if (play == 1 | AUTO)
        OCR1A = ICR1 * VOL;

    if (counter == 41)
        _delay_ms(5000);
    else if (counter % 7 == 6)
        _delay_ms(650);
    else
        _delay_ms(300);
    OCR1A = 0;
    _delay_ms(50);

    counter++;
    if (counter >= 42)
        counter = 0;
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
