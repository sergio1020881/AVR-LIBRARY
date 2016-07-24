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


void PORTINIT(void);
//ISR(TIMER0_COMP_vect);
//ISR(INT0_vect);

int main(void)
{
    char number[8];
	uint8_t code[2];
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
		
		code[0]=ir.key()[1];
		code[1]=ir.key()[2];
		
		lcd0.gotoxy(0,1);
		if((code[0]==73) && (code[1]==10)){
			lcd0.string_size("STOP",20);
			PORTA=10;
		}
		if((code[0]==73) && (code[1]==18)){
			lcd0.string_size("PLAY",20);
			PORTA=11;
		}
		if((code[0]==73) && (code[1]==14)){
			lcd0.string_size("INFO",20);
			PORTA=12;
		}
		if((code[0]==73) && (code[1]==22)){
			lcd0.string_size("PAUSE",20);
			PORTA=13;
		}
		if((code[0]==169) && (code[1]==18)){
			lcd0.string_size("UP",20);
			PORTA=14;
		}
		if((code[0]==73) && (code[1]==17)){
			lcd0.string_size("DOWN",20);
			PORTA=15;
		}
		if((code[0]==169) && (code[1]==14)){
			lcd0.string_size("LEFT",20);
			PORTA=16;
		}
		if((code[0]==73) && (code[1]==9)){
			lcd0.string_size("RIGHT",20);
			PORTA=17;
		}
		if((code[0]==169) && (code[1]==22)){
			lcd0.string_size("OK",20);
			PORTA=18;
		}
		if((code[0]==201) && (code[1]==21)){
			lcd0.string_size("C +",20);
			PORTA=19;
		}
		if((code[0]==201) && (code[1]==10)){
			lcd0.string_size("C -",20);
			PORTA=20;
		}
		if((code[0]==201) && (code[1]==18)){
			lcd0.string_size("VOL +",20);
			PORTA=21;
		}
		if((code[0]==201) && (code[1]==14)){
			lcd0.string_size("VOL -",20);
			PORTA=22;
		}
		if((code[0]==41) && (code[1]==17)){
			lcd0.string_size("POWER",20);
			PORTA=23;
		}
		if((code[0]==73) && (code[1]==21)){
			lcd0.string_size("ESC",20);
			PORTA=24;
		}
		if((code[0]==41) && (code[1]==13)){
			lcd0.string_size("SNOOZE",20);
			PORTA=25;
		}
		if((code[0]==201) && (code[1]==13)){
			lcd0.string_size("TV RAD",20);
			PORTA=26;
		}
		if((code[0]==41) && (code[1]==21)){
			lcd0.string_size("ONE",20);
			PORTA=1;
		}
		if((code[0]==41) && (code[1]==10)){
			lcd0.string_size("TWO",20);
			PORTA=2;
		}
		if((code[0]==41) && (code[1]==18)){
			lcd0.string_size("THREE",20);
			PORTA=3;
		}
		if((code[0]==41) && (code[1]==14)){
			lcd0.string_size("FOUR",20);
			PORTA=4;
		}
		if((code[0]==41) && (code[1]==22)){
			lcd0.string_size("FIVE",20);
			PORTA=5;
		}
		if((code[0]==169) && (code[1]==9)){
			lcd0.string_size("SIX",20);
			PORTA=6;
		}
		if((code[0]==169) && (code[1]==17)){
			lcd0.string_size("SEVEN",20);
			PORTA=7;
		}
		if((code[0]==169) && (code[1]==13)){
			lcd0.string_size("EIGHT",20);
			PORTA=8;
		}
		if((code[0]==169) && (code[1]==21)){
			lcd0.string_size("NINE",20);
			PORTA=9;
		}
		if((code[0]==169) && (code[1]==10)){
			lcd0.string_size("ZERO",20);
			PORTA=0;
		}
		if((code[0]==201) && (code[1]==17)){
			lcd0.string_size("ASTERIX",20);
			PORTA=27;
		}
		if((code[0]==201) && (code[1]==9)){
			lcd0.string_size("HASHTAG",20);
			PORTA=28;
		}
		if((code[0]==73) && (code[1]==13)){
			lcd0.string_size("ROTATE",20);
			PORTA=29;
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
	DDRA=0XFF;
	PORTA=0XFF;
	DDRD=0xF0;
	PORTD=0xFF;
	SREG|=(1<<7);
}
/**********/
/*
** interrupt
*/

/***EOF***/
