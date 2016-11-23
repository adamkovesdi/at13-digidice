/*
 * TM74HC595 based digital tube AVR driver
 *
 * Written by Adam Kovesdi (c)2016
 * All rights reserved
 *
 * File: digitube4.c
 * Version: 1
 */

#include <avr/io.h> 
#include "digitube4.h"

//===============================================0=====1=====2=====3=====4=====5=====6=====7=====8=====9===== =====d===
unsigned const char digitube4_font[]				= { 0x03, 0x9f, 0x25, 0x0d, 0x99, 0x49, 0x41, 0x1f, 0x01, 0x09, 0xff, 0x85 };
unsigned const char digitube4_digitselect[]	= { 0x10, 0x20, 0x40, 0x80 };

void digitube4_setup()
{
	DDRB |= (1<<DT_SCLK) | (1<<DT_RCLK) | (1<<DT_DIO);
}

void digitube4_off()
{
	digitube4_led[0]=10;
	digitube4_led[1]=10;
	digitube4_led[2]=10;
	digitube4_led[3]=10;
}

void digitube4_setvalue(unsigned int number) // program space greedy (division)
{
	digitube4_led[0]=number/1000%10;
	digitube4_led[1]=number/100%10;
	digitube4_led[2]=number/10%10;
	digitube4_led[3]=number%10;
	if (digitube4_led[0]==0)
	{
		digitube4_led[0]=10;
		if (digitube4_led[1]==0)
		{
			digitube4_led[1]=10;
			if (digitube4_led[2]==0)
			{
				digitube4_led[2]=10;
			}
		}
	}
}

void digitube4_display() // serious serial bitbang
{
	unsigned char todisplay,digit,bit;
	for(digit=0;digit<4;digit++)
	{
		todisplay=digitube4_font[digitube4_led[digit]]; // display segments 
		for(bit=0;bit<8;bit++)
		{
			GETBIT(todisplay,bit) ? SETBIT(PORTB,DT_DIO) : CLEARBIT(PORTB,DT_DIO);
			SETBIT(PORTB,DT_SCLK); CLEARBIT(PORTB,DT_SCLK); // SCLK HILO 
		}
		todisplay=digitube4_digitselect[digit]; // digit selector 
		for(bit=0;bit<8;bit++)
		{
			GETBIT(todisplay,bit) ? SETBIT(PORTB,DT_DIO) : CLEARBIT(PORTB,DT_DIO);
			SETBIT(PORTB,DT_SCLK); CLEARBIT(PORTB,DT_SCLK); // SCLK HILO 
		}
		SETBIT(PORTB,DT_RCLK); CLEARBIT(PORTB,DT_RCLK); // RCLK HILO 
	}
}

