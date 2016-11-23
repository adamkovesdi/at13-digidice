/*
 * Digital dice with attiny13a and TM74HC595 4 digit digital tube display
 * Written by Adam Kovesdi (c)2016
 * All rights reserved
 *
 * File: main.c
 * Version: 2
 */

#include <stdlib.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "digitube4.h"
#include "random.h"

#define WAKECYCLES		10500	// 10500 ~ 25 seconds @ 1.2MHz clock
#define BUTTONDELAY		80		// 80 is ideal 

#define ROLLSWITCH		PB1		// PB1 (MISO/AIN1/OC0B/INT0/PCINT1) pin 6
#define DICESWITCH		PB0		// PB0 (MOSI/AIN0/OC0A/PCINT0) pin 5
#define rollkeypress	(!(GETBIT(PINB,ROLLSWITCH)))
#define dicekeypress	(!(GETBIT(PINB,DICESWITCH)))

#define D2D6					5
const unsigned char dices[] = { 4, 6, 8, 10, 12, 0xFF, 20, 100 }; // 0xFF is just a placeholder for 2D6

volatile unsigned int awake;
volatile unsigned char dice=6;

ISR(PCINT0_vect)
{
	awake=WAKECYCLES;
}

inline void setup()
{
	SETBIT(PORTB,ROLLSWITCH);			// set switch port to input
	SETBIT(PORTB,DICESWITCH);			// set switch port to input
	// SETBIT(PCMSK,ROLLSWITCH);			// enable interrupt on switch port
	SETBIT(PCMSK,DICESWITCH);			// enable interrupt on switch port
	SETBIT(GIMSK,PCIE);				// enable Pin Change Interrupt
	digitube4_setup();
	sei();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); // set up low power sleep mode 
}

inline void loop()
{
	// main logic loop
	if((rollkeypress) && (dicekeypress))
	{
		// both keys pressed - rng dry run 
		unsigned int random;
		random=rng(1,9999);
		digitube4_setvalue(random);
		// end of both keypress handler
		return;
	}
	if(rollkeypress)
	{
		unsigned int random;
		if(dice==D2D6)	// 2d6
		{
			random=rng(0,35);
			digitube4_led[0]=10;
			digitube4_led[1]=random/6+1;
			digitube4_led[2]=10;
			digitube4_led[3]=random%6+1;
		}
		else
		{
			random=rng(1,dices[dice]);
			digitube4_setvalue(random);
		}
		awake=WAKECYCLES;
		// end of rollkey handler
		return;
	}
	if(dicekeypress)
	{
		if((digitube4_led[0]==11)||(digitube4_led[2]==11)) // only if d is displayed
		{
			if(dicekeypress) if(++dice==sizeof(dices)) dice=0; // advance in dice array with rollover detection
		}
		// display selected dice
		if(dice==D2D6)	// 2d6
		{
			digitube4_led[0]=10;
			digitube4_led[1]=2;
			digitube4_led[2]=11; // letter d
			digitube4_led[3]=6;
		}
		else
		{
			digitube4_setvalue(dices[dice]);
			digitube4_led[0]=11; // letter d
		}
		unsigned int i;		// stay here for a while
		for(i=0;i<BUTTONDELAY;i++) digitube4_display();
		// end of dicekey handler
		return;
	}
	// no keys pressed, count down towards sleep
	if(awake)awake--;
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
			sleep_mode();
		}
	}
	return 0;
}


