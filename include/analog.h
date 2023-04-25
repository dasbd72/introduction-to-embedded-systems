#ifndef _ANALOG_H
#define _ANALOG_H

#include <avr/io.h>

#define ANALOG_DIV_2 ((0 << ADPS2) | (0 << ADPS1) | (0 << ADPS0))
#define ANALOG_DIV_8 ((0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#define ANALOG_DIV_128 ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))

#define ANALOG_REFINT ((0 << REFS1) | (0 << REFS0))
#define ANALOG_REFAVCC ((0 << REFS1) | (1 << REFS0))
#define ANALOG_REF1p1 ((1 << REFS1) | (1 << REFS0))

void analog_init() {
    PRR &= ~(1 << PRADC);  // Clear ADC bit in power reduction register (turns on ADC)
    ADCSRA |= (1 << ADEN);  // Enable ADC

    // ADCSRB |= (1 << ACME);
    // ADCSRB |= (1 << ADTS0);
}

void analog_uninit() {
    ADCSRA &= ~(1 << ADEN);  // Enable ADC
}

int analog_read(uint8_t pin) {
    ADMUX = ANALOG_REFAVCC | (pin & 0x07);  // Use Internal 1.1V voltage as reference, Set using pin
    ADCSRA |= ANALOG_DIV_8;                 // Set speed (Division factor)

    ADCSRA |= (1 << ADSC);          // Set ADSC to start conversion.
    while ((ADCSRA & (1 << ADSC)))  // It returns automatically to 0 when conversion is complete.
        ;

    // int val = 0;
    // val = ADCL;          // read low byte first
    // val += (ADCH << 8);  // then read high byte
    // return val;
    return ADC;
}

#endif  // _ANALOG_H