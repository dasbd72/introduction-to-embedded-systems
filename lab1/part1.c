#include <avr/io.h>
#include <util/delay.h>
void wait_ms(int);
int main(void) {
    int wait = 500;

    int up_reg = 0;
    int down_reg = 0;
    int up_nreg = 0;
    int down_nreg = 0;
    int up_debounce, down_debounce;
    int up_ndebounce, down_ndebounce;
    int up, down;

    DDRB |= _BV(DDB5);       // Initialize output port
    DDRD &= ~(1 << PORTD2);  // Initialize input port

    while (1) {
        up_nreg = ~up_reg & 1;
        down_nreg = ~down_reg & 1;
        up_reg = (PIND >> PORTD2) & 1;
        down_reg = (PIND >> PORTD3) & 1;
        
        // up_ndebounce = ~up_debounce;
        // down_ndebounce = ~down_debounce;
        
        // up_debounce = up_reg & (up_reg >> 1) & (up_reg >> 2) & (up_reg >> 3) & 1;
        // down_debounce = down_reg & (down_reg >> 1) & (down_reg >> 2) & (down_reg >> 3) & 1;

        // up = up_debounce & up_ndebounce & 1;
        // down = down_debounce & down_ndebounce & 1;

        // up = (PIND >> PORTD2) & 1;
        // down = (PIND >> PORTD3) & 1;

        // up = up_reg & up_nreg;
        // down = down_reg & down_nreg;

        up = up_reg;
        down = down_reg;
        
        if (up) {
            wait = 1000;
            // wait = wait >= 2000 ? 2000 : wait + 100;
        } else if (down) {
            wait = 100;
            // wait = wait <= 100 ? 100 : wait - 100;
        }
        PORTB |= 1 << PORTB5;
        wait_ms(wait);
        PORTB &= ~(1 << PORTB5);
        wait_ms(wait);
    }
}

void wait_ms(int x) {
    for (int i = 0; i < x; i++) {
        _delay_ms(1);
    }
}