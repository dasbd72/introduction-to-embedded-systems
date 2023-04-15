#ifndef _UART_H
#define _UART_H

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (F_CPU / 16 / BAUDRATE - 1)

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

void uart_sendint(int num) {
    char str[100];
    sprintf(str, "%d", num);
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        uart_sendbyte(str[i]);
    }
    uart_sendbyte('\n');
    uart_sendbyte('\r');
}

void uart_sendhex(int num) {
    char str[100];
    sprintf(str, "0x%x", num);
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        uart_sendbyte(str[i]);
    }
    uart_sendbyte('\n');
    uart_sendbyte('\r');
}

void uart_sendstr(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        uart_sendbyte(str[i]);
    }
    uart_sendbyte('\n');
    uart_sendbyte('\r');
}

#endif  // _UART_H
