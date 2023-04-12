#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (F_CPU / 16 / BAUDRATE - 1)

volatile uint8_t cur_row;

char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
volatile uint8_t prev_down_cols[4];

void set_row_low(int row) {
    switch (row) {
        case 0:
            PORTB &= ~(1 << PB1);
            break;
        case 1:
            PORTB &= ~(1 << PB0);
            break;
        case 2:
            PORTD &= ~(1 << PD7);
            break;
        case 3:
            PORTD &= ~(1 << PD6);
            break;
        default:
            break;
    }
}

void set_row_high(int row) {
    switch (row) {
        case 0:
            PORTB |= (1 << PB1);
            break;
        case 1:
            PORTB |= (1 << PB0);
            break;
        case 2:
            PORTD |= (1 << PD7);
            break;
        case 3:
            PORTD |= (1 << PD6);
            break;
        default:
            break;
    }
}

void set_all_row_high() {
    set_row_high(0);
    set_row_high(1);
    set_row_high(2);
    set_row_high(3);
}

char keypad_scan() {
    char key = '\0';
    int cur_col = -1;
    if (((PIND >> PD5) & 1) == 0) {
        cur_col = 0;
    } else if (((PIND >> PD4) & 1) == 0) {
        cur_col = 1;
    } else if (((PIND >> PD3) & 1) == 0) {
        cur_col = 2;
    } else if (((PIND >> PD2) & 1) == 0) {
        cur_col = 3;
    }
    if (cur_col != -1 && prev_down_cols[cur_row] != cur_col) {
        key = keys[cur_row][cur_col];
    }
    prev_down_cols[cur_row] = cur_col;
    return key;
}

ISR(PCINT2_vect) {
    char key = keypad_scan();
    if (key != '\0') {
        PORTB ^= (1 << PB5);
        uart_sendbyte(key);
    }
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
    // Membrane Rows Output
    for (int i = 6; i <= 7; i++) {
        DDRD |= (1 << i);
    }
    for (int i = 0; i <= 1; i++) {
        DDRB |= (1 << i);
    }
    set_all_row_high();
    cur_row = 0;
    set_row_low(0);
    for (int c = 0; c < 4; c++) {
        prev_down_cols[c] = -1;
    }

    // Membrane Columns Input
    for (int i = 2; i <= 5; i++) {
        DDRD &= ~(1 << i);
        PORTD |= (1 << i);
    }

    // Built in led ouptut
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // === Interrupt ===
    PCICR |= (1 << PCIE2);     // Enable PCINT[23:16]
    PCMSK2 |= (1 << PCINT18);  // Enable PCINT18 (D2)
    PCMSK2 |= (1 << PCINT19);  // Enable PCINT19 (D3)
    PCMSK2 |= (1 << PCINT20);  // Enable PCINT20 (D4)
    PCMSK2 |= (1 << PCINT21);  // Enable PCINT21 (D5)
    sei();                     // Enable interrupts
}

void loop() {
    sei();          // Enable interrupts
    _delay_ms(1);   // Time for interrupts
    cli();          // Disable interrupts
    keypad_scan();  // Disable interrupts
    set_row_high(cur_row);
    cur_row = cur_row + 1 > 3 ? 0 : cur_row + 1;
    set_row_low(cur_row);
}

int main(void) {
    setup();
    uart_init();
    while (1) {
        loop();
    }
    return 0;
}
