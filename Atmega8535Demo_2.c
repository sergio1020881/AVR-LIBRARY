/*
 * Atmega8535.c
 *
 * Hardware:
 * Atmega8535 at 8Mhz
 *
 * Created: 27/06/2016 20:51:33
 * Author : Sérgio Santos
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
	TIMER0_COMPARE_MATCH=0;
    PORTINIT();
    /* Replace with your application code */
    FUNC func = FUNCenable();
    TRAN trB = TRANenable();
    LCD0 lcd0 = LCD0enable(&DDRC,&PINC,&PORTC);
	//TIMER_COUNTER0 timer0 = TIMER_COUNTER0enable(0,1);
	IR ir = IRenable();
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
		if((ir.key()[2]==182) && (ir.key()[3]==0)){
			lcd0.string_size("STOP",20);
		}
		if((ir.key()[2]==54) && (ir.key()[3]==1)){
			lcd0.string_size("PLAY",20);
		}
		if((ir.key()[2]==214) && (ir.key()[3]==0)){
			lcd0.string_size("INFO",20);
		}
		if((ir.key()[2]==86) && (ir.key()[3]==1)){
			lcd0.string_size("PAUSE",20);
		}
		if((ir.key()[2]==57) && (ir.key()[3]==1)){
			lcd0.string_size("UP",20);
		}
		if((ir.key()[2]==38) && (ir.key()[3]==1)){
			lcd0.string_size("DOWN",20);
		}
		if((ir.key()[2]==217) && (ir.key()[3]==0)){
			lcd0.string_size("LEFT",20);
		}
		if((ir.key()[2]==166) && (ir.key()[3]==0)){
			lcd0.string_size("RIGHT",20);
		}
		if((ir.key()[2]==89) && (ir.key()[3]==1)){
			lcd0.string_size("OK",20);
		}
		if((ir.key()[2]==74) && (ir.key()[3]==1)){
			lcd0.string_size("C +",20);
		}
		if((ir.key()[2]==186) && (ir.key()[3]==0)){
			lcd0.string_size("C -",20);
		}
		if((ir.key()[2]==58) && (ir.key()[3]==1)){
			lcd0.string_size("VOL +",20);
		}
		if((ir.key()[2]==218) && (ir.key()[3]==0)){
			lcd0.string_size("VOL -",20);
		}
		if((ir.key()[2]==37) && (ir.key()[3]==1)){
			lcd0.string_size("POWER",20);
		}
		if((ir.key()[2]==70) && (ir.key()[3]==1)){
			lcd0.string_size("ESC",20);
		}
		if((ir.key()[2]==69) && (ir.key()[3]==1)){
			lcd0.string_size("ONE",20);
		}
		if((ir.key()[2]==181) && (ir.key()[3]==0)){
			lcd0.string_size("TWO",20);
		}
		
		
		
		
		lcd0.gotoxy(0,3);
		func.i32toa(ir.key()[0],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[1],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[2],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[3],number);
		lcd0.string(number);lcd0.hspace(2);
		
		//lcd0.gotoxy(0,2);
		//func.i32toa(ir.key()[4],number);
		//lcd0.string(number);lcd0.hspace(2);
		//func.i32toa(ir.key()[5],number);
		//lcd0.string(number);lcd0.hspace(2);
		//func.i32toa(ir.key()[6],number);
		//lcd0.string(number);lcd0.hspace(2);
		//func.i32toa(ir.key()[7],number);
		//lcd0.string(number);lcd0.hspace(2);
		
		//lcd0.gotoxy(0,3);
		//func.i32toa(ir.key()[8],number);
		//lcd0.string(number);lcd0.hspace(2);
		//func.i32toa(ir.key()[9],number);
		//lcd0.string(number);lcd0.hspace(2);
		//func.i32toa(ir.key()[10],number);
		//lcd0.string(number);lcd0.hspace(2);
		//func.i32toa(ir.key()[11],number);
		//lcd0.string(number);lcd0.hspace(2);
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
