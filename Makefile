SOURCE ?= $(wildcard *.c) $(wildcard lab*/*.c) $(wildcard test/*.c) $(wildcard templates/*.c)
TARGET ?= $(SOURCE:.c=)

AVR_PATH = ./arduino_build
PORT ?= /dev/tty.usbserial-120
BAUDRATE=9600
I2C_CONFIG_F_SCL=100000UL

CC = avr-gcc
CXX = avr-g++
DEFINE = -DF_CPU=16000000UL -DBAUDRATE=$(BAUDRATE) -DI2C_CONFIG_F_SCL=$(I2C_CONFIG_F_SCL)
CFLAGS = -L $(AVR_PATH) -I $(AVR_PATH) -I ./include -Wall $(DEFINE) -Os -mmcu=atmega328p

ifeq ($(OS),Windows_NT)
	RM = del
else
	RM = rm -f
endif

HEX = $(TARGET:=.hex)

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $^
%.out: %.cpp
	$(CXX) $(CFLAGS) -o $@ $^
%.hex: %.out
	avr-objcopy -O ihex -R .eeprom $^ $@

.PHONY: default clean upload all

default: $(HEX)
clean:
	$(RM) *.out *.hex lab*/*.out lab*/*.hex test/*.out test/*.hex
	$(RM) $(TARGET)
upload: $(lastword $(HEX))
	avrdude -c arduino -p m328p -b 115200 -P $(PORT) -U flash:w:$(HEX)
all: default upload

.PHONY: test monitor um
monitor: 
	@screen $(PORT) $(BAUDRATE)
um: upload monitor
test:
	@echo $(lastword $(HEX))