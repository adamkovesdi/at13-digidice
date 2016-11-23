/*
 * Digital dice with attiny13a and TM74HC595 4 digit digital tube display
 * Written by Adam Kovesdi (c)2016
 * All rights reserved
 *
 * File: main.c
 * Version: 1
 */

#include <stdlib.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "digitube4.h"
#include "random.h"

#define WAKECYCLES		10500	// 10500 ~ 30 seconds @ 1.2MHz clock
#define BUTTONDELAY		80		// 80 is ideal 

#define ROLLSWITCH		PB1		// PB1 (MISO/AIN1/OC0B/INT0/PCINT1) pin 6
#define DICESWITCH		PB0		// PB0 (MOSI/AIN0/OC0A/PCINT0) pin 5
#define rollkeypress	(!(GETBIT(PINB,ROLLSWITCH)))
#define dicekeypress	(!(GETBIT(PINB,DICESWITCH)))

volatile unsigned int counter, awake;
volatile unsigned int dice=20;

ISR(PCINT0_vect)
{
	awake=WAKECYCLES;
}

ISR(TIM0_OVF_vect)
{
	counter++;
}

inline void setup()
{
	SETBIT(PORTB,ROLLSWITCH);			// set switch port to input
	SETBIT(PORTB,DICESWITCH);			// set switch port to input
	SETBIT(PCMSK,ROLLSWITCH);			// enable interrupt on switch port
	SETBIT(PCMSK,DICESWITCH);			// enable interrupt on switch port
	SETBIT(GIMSK,PCIE);				// enable Pin Change Interrupt
	TCCR0B |= (1<<CS00);			// timer prescaler
	SETBIT(TIMSK0,TOIE0);			// enable timer
	digitube4_setup();
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // set up low power sleep mode 
}

inline void loop()
{
	// main logic loop
	if(rollkeypress)
	{
		int random;
		random=rng(1,dice);
		digitube4_setvalue(random);
		awake=WAKECYCLES;
	}
	else
	{
		if(awake)awake--;
	}
	if(dicekeypress)
	{
		if(digitube4_led[0]==11) // only switch dice if d is shown
		{
			switch(dice)
			{
				case 100: dice=4; break;
				case 20: dice=100; break;
				case 12: dice=20; break;
				case 10: dice=12; break;
				case 8: dice=10; break;
				case 6: dice=8; break;
				case 4: dice=6; break;
			}
		}
		digitube4_off();
		digitube4_setvalue(dice);
		digitube4_led[0]=11;
		unsigned char i;		// stay here for a while
		for(i=0;i<BUTTONDELAY;i++) digitube4_display();
	}
}

int main(void)
{
	setup();
	for(;;)
	{
		loop();
		digitube4_display();	
		if(!awake)
		{
			// turn off display and sleep
			digitube4_off();
			digitube4_display();
			CLEARBIT(TIMSK0,TOIE0);			// stop timer
			sleep_mode();
			SETBIT(TIMSK0,TOIE0);			// enable timer
		}
	}
	return 0;
}


