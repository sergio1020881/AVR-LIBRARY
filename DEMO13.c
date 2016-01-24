/*
 *
 *
 * Created: 31/08/2015 18:52:15
 *  Author: SérgioManuel
 */ 
#define F_CPU 16000000UL
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
/**********/
#include"Atmega128API.h"
/*********/
#include<string.h>
/*
** constant and macro
*/
#define TRUE 1
#define FALSE 0
#define GI 7
#define vector_size 16
/*
** variable
*/
uint16_t TIMER0_COMPARE_MATCH;
CLOCK relogio;
/*
** procedure and function header
*/
void PORTINIT(void);
/****MAIN****/
int main(void)
{
	PORTINIT();
	/***INICIALIZE OBJECTS***/
	FUNC function= FUNCenable();
	LCD0 lcd0 = LCD0enable(&DDRA,&PINA,&PORTA);
	LCD1 lcd1 = LCD1enable(&DDRC,&PINC,&PORTC);
	ANALOG analog = ANALOGenable(1, 128, 1, 0); // channel 0 for position
	TIMER_COUNTER0 timer0 = TIMER_COUNTER0enable(2,2); // for clock
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(9,0); // PWM positioning
	relogio=CLOCKenable(12,0,0);
	/******/
	char analogtmp[8];
	int adcvalue;
	/***Parameters timers***/
	timer0.compare(249);
	timer1.compoutmodeB(2);
	timer1.compareA(20000);
	timer1.start(8);
	timer0.start(64);
	/**********/
	while(TRUE){
		//PREAMBLE
		lcd0.reboot();
		//TODO:: Please write your application code
			lcd0.gotoxy(0,0);
			lcd1.gotoxy(0,0);
			adcvalue=analog.read(0);
			function.itoa(adcvalue,analogtmp);
			lcd0.string_size(analogtmp,5);
			lcd1.string_size(analogtmp,5);
			timer1.compareB(function.trimmer(adcvalue,0,1023,450,2450));
		lcd0.hspace(3);
		lcd0.string(relogio.show());
		lcd1.hspace(3);
		lcd1.string(relogio.show());
	}
}
/*
** procedure and function
*/
void PORTINIT()
{
	//INPUT
	DDRF=0x00;
	PORTF=0x0F;
	DDRE=0X00;
	PORTE=0XFF;
	DDRD=0X00;
	PORTD=0XFF;
	//OUTPUT
	DDRC=0XFF;
	PORTC=0x00;
	DDRB|=(1<<5) | (1<<6) | (1<<7);
	//UART0
	//DDRE=0X02;
	SREG|=(1<<GI);
}
/*
** interrupt
*/
ISR(TIMER0_COMP_vect) // TIMER0_COMP_vect used for clock
{
	TIMER0_COMPARE_MATCH++;
	if(TIMER0_COMPARE_MATCH > 999){
		TIMER0_COMPARE_MATCH=0;
		relogio.increment();
	}
}
/***EOF***/
/***COMMENTS
set hour mode
***/

