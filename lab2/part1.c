#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#define MAX 39999

volatile int angle = 0;
volatile int pre_up = 0;
volatile int pre_down = 0;
volatile uint8_t pinDhistory = 0xFF;

ISR(PCINT2_vect) {
    int cur_up = (PIND >> PD2) & 1;
    int cur_down = (PIND >> PD3) & 1;
    int up = pre_up & ~cur_up & 1;
    int down = pre_down & ~cur_down & 1;

    if (up) {
        angle += 45;
        if (angle >= 180)
            angle = 180;
    } else if (down) {
        angle -= 45;
        if (angle <= 0)
            angle = 0;
    }

    pre_up = cur_up;
    pre_down = cur_down;
    PORTB ^= (1 << PB5);
}

void setup() {
    DDRB |= (1 << PB1);  // Set pin 13 on arduino to output
    DDRB |= (1 << PB5);  // Debug led
    PORTB &= ~(1 << PB5);

    DDRD &= ~(1 << PD2);  // Initialize input port D2
    PORTD |= (1 << PD2);
    DDRD &= ~(1 << PD3);  // Initialize input port D3
    PORTD |= (1 << PD3);

    /* PWM */
    /* 1. Set Fast PWM mode 14: set WGM11, WGM12, WGM13 to 1 */
    /* 2. Set pre-scaler of 8 */
    /* 3. Set Fast PWM non-inverting mode */
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);

    /* Set ICR1 register: PWM period 20 ms*/
    ICR1 = MAX;
    OCR1A = MAX / 20;

    /* Interrupt */
    PCICR |= (1 << PCIE2);     // enable PCINT[23:16]
    PCMSK2 |= (1 << PCINT18);  // enable PCINT18 (D2)
    PCMSK2 |= (1 << PCINT19);  // enable PCINT19 (D3)
    sei();                     // Enable interrupts
}

void loop() {
    OCR1A = MAX / 20 + angle * MAX / 20 / 180;
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
