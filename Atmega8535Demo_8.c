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
#define TRUE 1

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
	uint8_t data=0;
	uint8_t out=0;
	uint8_t inc=0;
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
	
    while (TRUE)
    {
		
	    trB.oneshot(&trB,PIND);
	    func.i32toa(trB.data,number);
	    
	    lcd0.gotoxy(0,0);
	    lcd0.string(func.print_binary(trB.data));
		lcd0.hspace(3);
		//func.i32toa(ir.key()[0],number);
		//lcd0.string(number);lcd0.hspace(2);
		
		lcd0.gotoxy(0,1);
		data=ir.decode();
		lcd0.putch(data);
		
		switch(data){
			case 'U':
				inc++;
				lcd0.hspace(3);
				func.i16toa(inc,number);
				lcd0.string(number);
				lcd0.hspace(3);
				out=inc;
				ir.clear();
				break;
			case 'D':
				inc--;
				out=inc;
				lcd0.hspace(3);
				func.i16toa(inc,number);
				lcd0.string(number);
				lcd0.hspace(3);
				out=inc;
				ir.clear();
				break;
			case '0':
				out=255;
				break;
			case 'S':
				out=255;
				break;
			case 'P':
				out=254;
				break;
			case 'p':
				out=255;
				break;
			case '1':
				out=254;
				break;
			case '2':
				out=253;
				break;
			case '3':
				out=251;
				break;
			case '4':
				out=247;
				break;
			case '5':
				out=239;
				break;
			case '6':
				out=223;
				break;
			case '7':
				out=191;
				break;
			case '8':
				out=127;
				break;
			default:
				if(data)
					out=data;
				break;
		}
		
		PORTA=out;
		
		/*
		lcd0.hspace(2);
		func.i32toa(ir.key()[0],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[1],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[2],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[3],number);
		lcd0.string(number);lcd0.hspace(2);
		*/
		/*
		lcd0.gotoxy(0,3);
		func.i32toa(ir.key()[4],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[5],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[6],number);
		lcd0.string(number);lcd0.hspace(2);
		func.i32toa(ir.key()[7],number);
		lcd0.string(number);lcd0.hspace(4);
		*/
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
/*
** interrupt
*/
ISR(TIMER1_COMPA_vect){
	
}
/***EOF***/
