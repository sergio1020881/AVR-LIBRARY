/*
 * Atmega8535.c
 *
 * Hardware:
 * Atmega8535 at 8Mhz
 *
 * Created: 27/06/2016 20:51:33
 * Author : Sérgio Santos
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
#include "atmega8535timer.h"
#include "iremote.h"


void PORTINIT(void);

int main(void)
{
    char number[8];
    PORTINIT();
    /* Replace with your application code */
    FUNC func = FUNCenable();
    TRAN trB = TRANenable();
    LCD0 lcd0 = LCD0enable(&DDRC,&PINC,&PORTC);
	IR ir = IRenable();
	
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(4,3);
	//timer1.compoutmode(0);
	timer1.compareA(2345);
	timer1.start(64);
	
	sei();
	
    while (1)
    {
		
	    trB.oneshot(&trB,PIND);
	    func.i32toa(trB.data,number);
	    
	    lcd0.gotoxy(0,0);
	    lcd0.string(func.print_binary(trB.data));
		lcd0.hspace(3);
		func.i32toa(ir.key()[0],number);
		lcd0.string(number);lcd0.hspace(2);
		
		
		lcd0.gotoxy(0,1);
		lcd0.putch(ir.decode());
		PORTA=ir.decode();
		
		
		lcd0.gotoxy(0,3);
		func.i32toa(ir.key()[0],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[1],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[2],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[3],number);
		lcd0.string(number);lcd0.hspace(2);
    }
}
/**********/
void PORTINIT(void){
	//INPUT
	DDRC=0x00;
	PORTC=0xFF;
	DDRA=0XFF;
	PORTA=0XFF;
	DDRD=0xF0;
	PORTD=0xFF;
	DDRB=0XFF;
	PORTB=0XFF;
	SREG|=(1<<7);
}
//TIMER_COUNTER0_OVERFLOW_INTERRUPT
//TIMER0_OVF_vect
ISR(TIMER1_COMPA_vect)
{
	PORTB^=(1<<PB3);
}
ISR(TIMER1_OVF_vect)
{
	PORTB^=(1<<PB3);
}
ISR(TIMER1_COMPB_vect)
{
	PORTB^=(1<<PB3);
}
/**********/
/*
** interrupt
*/
/***EOF***/
