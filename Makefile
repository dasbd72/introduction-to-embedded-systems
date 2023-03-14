SOURCE = $(wildcard *.c) $(wildcard lab*/*.c)
TARGET ?= $(SOURCE:.c=)

AVR_PATH = ./arduino_build
PORT = /dev/tty.usbserial-10

CC = avr-gcc
CXX = avr-g++
CFLAGS = -L $(AVR_PATH) -I $(AVR_PATH) -Wall -DF_CPU=16000000UL -Os -mmcu=atmega328p

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
	$(RM) *.out *.hex lab*/*.out lab*/*.hex
	$(RM) $(TARGET)
upload: $(lastword $(HEX))
	avrdude -c arduino -p m328p -b 115200 -P $(PORT) -U flash:w:$(HEX)
all: default upload

.PHONY: test
test:
	@echo $(lastword $(HEX))