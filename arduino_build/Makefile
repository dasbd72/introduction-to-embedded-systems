# BSD licensed, see http://lastyearswishes.com/static/Makefile for full license
HDRS = Arduino.h binary.h Client.h HardwareSerial.h IPAddress.h new.h pins_arduino.h Platform.h Printable.h Print.h Server.h Stream.h Udp.h USBAPI.h USBCore.h USBDesc.h WCharacter.h wiring_private.h WString.h
OBJS = WInterrupts.o wiring_analog.o wiring.o wiring_digital.o wiring_pulse.o wiring_shift.o CDC.o HardwareSerial.o HID.o IPAddress.o main.o new.o Print.o Stream.o Tone.o USBCore.o WMath.o WString.o
# may need to adjust -mmcu if you have an older atmega168
# may also need to adjust F_CPU if your clock isn't set to 16Mhz
PREFIX=.
CFLAGS = -I . -std=gnu99  -DF_CPU=16000000UL -Os -mmcu=atmega328p
CPPFLAGS = -I . -DF_CPU=16000000UL -Os -mmcu=atmega328p

ifeq ($(OS),Windows_NT)

CC=avr-gcc.exe
CPP=avr-g++.exe
AR=avr-ar.exe

default: libarduino.a install

libarduino.a: ${OBJS}
	${AR} crs libarduino.a $(OBJS)

.c.o: ${HDRS}
	${CC} ${CFLAGS} -c $*.c

.cpp.o: ${HDRS}
	${CPP} ${CPPFLAGS} -c $*.cpp

clean:
	del ${OBJS} libarduino.a
	
install: libarduino.a
	mkdir ${PREFIX}\include
	mkdir ${PREFIX}\lib
	copy *.h ${PREFIX}\include 
	copy *.a ${PREFIX}\lib

else

CC=avr-gcc
CPP=avr-g++
AR=avr-ar

default: libarduino.a install

libarduino.a: ${OBJS}
	${AR} crs libarduino.a $(OBJS)

.c.o: ${HDRS}
	${CC} ${CFLAGS} -c $*.c

.cpp.o: ${HDRS}
	${CPP} ${CPPFLAGS} -c $*.cpp

clean:
	rm -rf ${PREFIX}/include
	rm -rf ${PREFIX}/lib
	rm -f ${OBJS} libarduino.a
	
install: libarduino.a
	mkdir -p ${PREFIX}/include
	mkdir -p ${PREFIX}/lib
	cp *.h ${PREFIX}/include 
	cp *.a ${PREFIX}/lib

endif