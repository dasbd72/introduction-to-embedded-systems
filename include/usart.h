#ifndef _USART_H
#define _USART_H

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (F_CPU / 16 / BAUDRATE - 1)

void usart_init(void) {
    UBRR0H = (uint8_t)(BAUD_PRESCALLER >> 8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);
    // Enable Transmitter and Receiver
    UCSR0B = (0 << RXCIE0) | (0 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0);
    // asynchronous mode, disable parity, 1-bit stop bit, 8-bit size
    UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) | (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (0 << UCSZ02) | (1 << UCSZ01) | (1 << UCSZ00);
}

void usart_sendbyte(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = data;
}

uint8_t usart_receivebyte(void) {
    while (!(UCSR0A & (1 << RXC0)))
        ;
    return UDR0;
}

void usart_sendint(int num) {
    char str[100];
    sprintf(str, "%d", num);
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        usart_sendbyte(str[i]);
    }
    usart_sendbyte('\n');
    usart_sendbyte('\r');
}

void usart_sendhex(int num) {
    char str[100];
    sprintf(str, "0x%x", num);
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        usart_sendbyte(str[i]);
    }
    usart_sendbyte('\n');
    usart_sendbyte('\r');
}

void usart_sendstr(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        usart_sendbyte(str[i]);
    }
    usart_sendbyte('\n');
    usart_sendbyte('\r');
}

void int_usart_init(void) {
    usart_init();
    // Enable Interrupt on receive complete
    UCSR0B |= (1 << RXCIE0) | (0 << TXCIE0);
}

#endif  // _USART_H
