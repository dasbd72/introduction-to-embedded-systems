#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

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

int main(void) {
    uart_init();
    DDRD &= ~_BV(DDD2);  // Input
    DDRB |= _BV(DDB5);   // Built in LED output
    DDRB |= _BV(DDB1);   // PWM output

    /* 1. Set Fast PWM mode 14: set WGM11, WGM12, WGM13 to 1 */
    /* 2. Set pre-scaler of 8 */
    /* 3. Set Fast PWM non-inverting mode */
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);

    /* Set ICR1 register: PWM period 20 ms*/
    ICR1 = 39999;

    int angle = 90;
    OCR1A = ICR1 / 20 + 90 * ICR1 / 20 / 180;
    int offset = 800;

    char str[100];
    int ind = 0;
    while (1) {
        ind = 0;
        while (1) {
            char c = uart_receivebyte();
            if (c == '\r') {
                str[ind] = '\0';
                break;
            }
            str[ind++] = c;
        }
        int up = (PIND >> PORTD2) & 1;
        if (up || strcmp(str, "on\n") == 0) {
            PORTB |= _BV(PORTB5);
            // OCR1A = MAX / 20 + angle * MAX / 20 / 180;
            OCR1A = 3999 + offset;
        } else if (up == 0 || strcmp(str, "off\n") == 0) {
            PORTB &= ~_BV(PORTB5);
            OCR1A = 1999 - offset;
            // OCR1A = MAX / 20 + 0 * MAX / 20 / 180;
        }
    }

    return 0;
}