#ifndef _DIGITUBE4_H
#define _DIGITUBE4_H

#define	DT_SCLK		PIN2
#define	DT_RCLK		PIN3
#define	DT_DIO		PIN4

#define SETBIT(theport,thebit) ((theport) |= (1<<thebit))
#define GETBIT(theport,thebit) ((theport) & (1<<thebit))
#define INVERTBIT(theport,thebit) ((theport) ^= (1<<thebit))
#define CLEARBIT(theport,thebit) ((theport) &= (~(1<<thebit)))

void digitube4_setup();
void digitube4_off();
void digitube4_display();
void digitube4_setvalue(unsigned int);

volatile unsigned char digitube4_led[4];	

#endif
