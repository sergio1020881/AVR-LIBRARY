/*
 * Easy328_1.c
 *
 * Created: 05/03/2016 15:51:44
 * Hardware:
 * ETT-Base AVR EASY328 XTAL 16.00Mhz ATMEGA328 32kBYTE FLASH
 * Manufacture:
 * FUTURLEC http://www.futurlec.com/ET-Easy328_Controller_Technical.shtml
 * Author : sergio
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <string.h>

#include "lcd.h"

void PORTINIT(void);
int main(void)
{
	PORTINIT();
    /* Replace with your application code */
	LCD0 lcd0 = LCD0enable(&DDRD,&PIND,&PORTD);
    while (1)
    {
		lcd0.gotoxy(0,0);
		lcd0.string("Ola Sergio");
		
		
		
    }
}
/**********/
void PORTINIT(void){
	//INPUT
	DDRD=0x00;
	PORTD=0xFF;
		
	SREG|=(1<<7);
}
/**********/
