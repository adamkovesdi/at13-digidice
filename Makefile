# Name: Makefile
# Author: Adam Kovesdi 
# Copyright: (c)2016 all rights reserved 
DEVICE		= t13
MMCU			= attiny13
F_CPU			= 1200000 
PROGRAMMER = -c usbtiny
# PROGRAMMER = -c stk500v2 -P avrdoper
# PROGRAMMER = -c usbasp

FUSES			=	-U lfuse:w:0x6a:m -U hfuse:w:0xff:m
OBJECTS		= main.o digitube4.o 

CFLAGS += -DF_CPU=$(F_CPU)

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE = avr-gcc -Wall -Os -mmcu=$(MMCU) $(CFLAGS) 

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -S $< -o $@

flash:	all
	sudo $(AVRDUDE) -U flash:w:main.hex:i

fuse:
	sudo $(AVRDUDE) $(FUSES)

clean:
	rm -f main.hex main.elf $(OBJECTS)

# file targets:
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf

readfuse:
	sudo $(AVRDUDE) -U lfuse:r:-:h -U hfuse:r:-:h -U lock:r:-:h 
# sudo $(AVRDUDE) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h -U lock:r:-:h 


