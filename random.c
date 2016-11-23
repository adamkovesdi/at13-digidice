/*
 * Linear Feedback Shift Register based RNG 
 *
 * Written by Adam Kovesdi (c)2016
 * All rights reserved
 *
 * File: main.c
 * Version: 1
 */

// LFSR feed
unsigned int lfsr = 0xACE1u;
unsigned int lfsr_get()
{
	unsigned int lsb = lfsr & 1;  /* Get lsb (i.e., the output bit). */
	lfsr >>= 1;               /* Shift register */
	if (lsb == 1)             /* Only apply toggle mask if output bit is 1. */
		lfsr ^= 0xB400u;        /* Apply toggle mask, value has 1 at bits corresponding
														 * to taps, 0 elsewhere. */
	return lfsr;
}

// Return a random number in the range min <= n <= max
volatile unsigned int rng(unsigned int min, unsigned int max)
{
	return (min + (lfsr_get() % (max-min+1))); // my own modulo pseudo random
}
