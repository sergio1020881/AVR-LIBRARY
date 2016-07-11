/*
 * Atmega8535.c
 *
 * Hardware:
 * Atmega8535 at 8Mhz
 *
 * Created: 27/06/2016 20:51:33
 * Author : Host
 */
#ifndef F_CPU
	#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <string.h>

#include "lcd.h"
#include "function.h"
#include "transition.h"
#include "timer.h"
#include "iremote.h"

uint16_t TIMER0_COMPARE_MATCH;

void PORTINIT(void);
//ISR(TIMER0_COMP_vect);
//ISR(INT0_vect);

int main(void)
{
    char number[8];
    uint16_t n=0;
	TIMER0_COMPARE_MATCH=0;
    PORTINIT();
    /* Replace with your application code */
    FUNC func = FUNCenable();
    TRAN trB = TRANenable();
    LCD0 lcd0 = LCD0enable(&DDRC,&PINC,&PORTC);
	//TIMER_COUNTER0 timer0 = TIMER_COUNTER0enable(0,1);
	IR ir = IRenable(IR_CTC_VALUE);
	//timer0.compoutmode(1);
	//timer0.compare(128);
	//timer0.start(1024);
	
	sei();
	
    while (1)
    {
	    trB.oneshot(&trB,PIND);
	    func.i32toa(trB.data,number);
	    
	    lcd0.gotoxy(0,0);
		
	    lcd0.string(func.print_binary(trB.data));
		
		lcd0.gotoxy(0,1);
		if((ir.key()[1]==28) && (ir.key()[2]==14))
			lcd0.string_size("Ola Sergio",20);
	    
		
		
		
		
	    lcd0.gotoxy(0,3);
	    func.i32toa(n,number);
	    lcd0.string_size(number,20);
		lcd0.gotoxy(0,2);
		
		func.i32toa(ir.key()[0],number);
		//func.i32toa(TCNT0,number);
		//func.i32toa(TIMER0_COMPARE_MATCH,number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[1],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[2],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[3],number);
		lcd0.string(number);lcd0.hspace(3);
    }
}
/**********/
void PORTINIT(void){
	//INPUT
	DDRC=0x00;
	PORTC=0xFF;
	DDRD=0xF0;
	PORTD=0xFF;
	SREG|=(1<<7);
}
/**********/
/*
** interrupt
*/
/***EOF***/
