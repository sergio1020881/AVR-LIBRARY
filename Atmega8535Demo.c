/*
 * Atmega8535.c
 * 
 * Hardware:
 * Atmega8535 at 8Mhz
 *
 * Created: 27/06/2016 20:51:33
 * Author : Host
 */ 
#define F_CPU 8000000UL

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
    char number[8];
    uint16_t n=0;
    PORTINIT();
    /* Replace with your application code */
    FUNC func = FUNCenable();
    TRAN trB = TRANenable();
    LCD0 lcd0 = LCD0enable(&DDRC,&PINC,&PORTC);
    while (1)
    {
	    trB.oneshot(&trB,PIND);
	    func.itoa(trB.data,number);
	    
	    lcd0.gotoxy(0,0);
	    lcd0.string(number);
	    
	    lcd0.gotoxy(0,1);
	    switch(trB.hl){
		    case 1:
		    lcd0.string_size("Ola Sergio",20);
		    n++;
		    break;
		    case 2:
		    lcd0.string_size("Ola Manuel",20);
		    n=0;
		    break;
		    case 4:
		    lcd0.string_size("Ola Salazar",20);
		    break;
		    case 8:
		    lcd0.string_size("Ola Santos",20);
		    break;
		    default:
		    break;
	    }
	    lcd0.gotoxy(0,3);
	    func.itoa(n,number);
	    lcd0.string_size(number,20);
    }
}
/**********/
void PORTINIT(void){
	//INPUT
	DDRC=0x00;
	PORTC=0xFF;
	DDRD=0x00;
	PORTD=0xFF;
	SREG|=(1<<7);
}
/**********/

