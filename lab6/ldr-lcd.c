#include <analog.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <i2c_master.h>
#include <lcd.h>
#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <util/delay.h>
#include <util/twi.h>

volatile i2c_trxn_t i2c_trxn;

volatile uint8_t lcd_backlight = LCD_NOBACKLIGHT;

void lcd_i2c_sendbyte(uint8_t data) {
    i2c_trxn_setbtye(&i2c_trxn, LCD_ADDRESS, data | lcd_backlight);
    i2c_start();
    i2c_trxn_wait(&i2c_trxn);
}

void lcd_write4bits(uint8_t data) {
    lcd_i2c_sendbyte(data | LCD_EN);
    _delay_us(1);
    lcd_i2c_sendbyte(data | LCD_EN);
    _delay_us(1);
    lcd_i2c_sendbyte(data & ~LCD_EN);
    _delay_us(1);
}

void lcd_sendbyte(uint8_t data, uint8_t mode) {
    uint8_t highnib = data & 0xf0;
    uint8_t lownib = (data << 4) & 0xf0;
    lcd_write4bits(highnib | mode);
    lcd_write4bits(lownib | mode);
}

void lcd_sendcmd(uint8_t data) {
    lcd_sendbyte(data, 0);
}

void lcd_writebyte(uint8_t data) {
    lcd_sendbyte(data, LCD_RS);
}

void lcd_writestr(uint8_t* str) {
    for (int i = 0; i < strlen(str); i++) {
        lcd_sendbyte(str[i], LCD_RS);
    }
}

void lcd_reset(void) {
    _delay_ms(100);

    lcd_sendcmd(0x33);  // 4 bit initialization of LCD
    lcd_sendcmd(0x32);  // 4 bit initialization of LCD
    lcd_sendcmd(LCD_FUNCTIONCMD | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    lcd_sendcmd(LCD_DISPLAYCMD | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    lcd_sendcmd(LCD_ENTRYCMD | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
    lcd_sendcmd(LCD_CLEARDISPLAY);  // Clear display
    lcd_sendcmd(LCD_RETURNHOME);    // Set cursor home

    lcd_backlight = LCD_BACKLIGHT;
    lcd_sendcmd(LCD_CLEARDISPLAY);  // Clear display
}

ISR(TWI_vect) {
    usart_sendhex(TW_STATUS);
    switch (TW_STATUS) {
        case TW_START:        // Start condition transmitted
        case TW_REP_START:    // Repeated start condition transmitted
        case TW_MT_ARB_LOST:  // Arbitration lost in SLA+W or data
            // Send slave address
            i2c_trxn_start(&i2c_trxn);
            break;
        case TW_MT_SLA_ACK:   // SLA+W transmitted, ACK received = Slave receiver ACKed address
        case TW_MT_DATA_ACK:  // Data transmitted, ACK received = Slave receiver ACKed data
            // Send data
            i2c_trxn_send_stop(&i2c_trxn);
            break;
        case TW_BUS_ERROR:     // Bus error; Illegal START or STOP condition
        case TW_MT_SLA_NACK:   // SLA+W transmitted, NACK received = Slave receiver with transmitted address doesn't exists?
        case TW_MT_DATA_NACK:  // Data transmitted, NACK received
        default:
            i2c_stop();
            break;
    }
}

volatile uint8_t prev_PINC = 0x00;
volatile uint8_t activate_PINC = 0x00;
volatile uint8_t deactivate_PINC = 0x00;

ISR(PCINT1_vect) {
    activate_PINC = PINC & ~prev_PINC;
    deactivate_PINC = ~PINC & prev_PINC;

    prev_PINC = PINC;
}

void setup() {
    // === Ouput ===
    // Built in led Output
    DDRB |= (1 << PB5);
    PORTB &= ~(1 << PB5);

    // === input ===
    DDRC &= ~(1 << PC0);
    DDRC &= ~(1 << PC1);

    // === analog ===
    analog_init();

    // === UART ===
    usart_init();

    // === I2C ===
    i2c_init();
    i2c_trxn_init(&i2c_trxn);

    // === Interrupt ===
    sei();  // Enable interrupts

    lcd_reset();
}

void loop() {
    int val = analog_read(0);

    char str[10];
    sprintf(str, "%05d", val);

    lcd_sendcmd(LCD_RETURNHOME);
    lcd_writestr(str);

    _delay_ms(1);
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
