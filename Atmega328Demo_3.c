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
#include "function.h"
#include "transition.h"

void PORTINIT(void);
int main(void)
{
	char number[4];
	PORTINIT();
    /* Replace with your application code */
	FUNC func = FUNCenable();
	TRAN trB = TRANenable();
	LCD0 lcd0 = LCD0enable(&DDRD,&PIND,&PORTD);
    while (1)
    {
		trB.update(&trB,PINB);
		func.itoa(trB.data,number);
		
		lcd0.gotoxy(0,0);
		lcd0.string(number);
		
		lcd0.gotoxy(0,1);
		switch(trB.hl){
			case 1:
				lcd0.string_size("Ola_1",10);
				break;
			case 2:
				lcd0.string_size("Ola_2",10);
			break;
			case 4:
				lcd0.string_size("Ola_3",10);
			break;
			case 8:
				lcd0.string_size("Ola_4",10);
			break;
			case 16:
				lcd0.string_size("Ola_5",10);
			break;
			case 3:
				lcd0.string_size("Ola_6",10);
			break;
			case 5:
				lcd0.string_size("Ola_7",10);
			break;
			default:
				lcd0.string("Ola Sergio");
		}
    }
}
/**********/
void PORTINIT(void){
	//INPUT
	DDRD=0x00;
	PORTD=0xFF;
	DDRB=0x00;
	PORTB=0xFF;
	SREG|=(1<<7);
}
/**********/
