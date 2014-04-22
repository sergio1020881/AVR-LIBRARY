/*************************************************************************
Title:    TIMER0
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: timer.c,v 0.1 2014/04/09 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR ATmega128 at 16 Mhz, 
License:  GNU General Public License 
DESCRIPTION:
	Atmega 128 at 16MHZ
USAGE:
NOTES:
    Based on Atmel Application Note AVR306
LICENSE:
    Copyright (C) 2014
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
COMMENT:
	Very Stable
*************************************************************************/
#ifndef F_CPU
  #define F_CPU 16000000UL
#endif
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
/***/
#include "timer.h"
/*
** constant and macro
*/
/***TYPE 1***/
#if defined(__AVR_AT90S2313__) \
 || defined(__AVR_AT90S4414__) || defined(__AVR_AT90S4434__) \
 || defined(__AVR_AT90S8515__) || defined(__AVR_AT90S8535__) \
 || defined(__AVR_ATmega103__)
	#error "AVR currently not supported by this libaray !"
/***TYPE 2***/
#elif defined(__AVR_AT90S2333__) || defined(__AVR_AT90S4433__)
	#error "AVR currently not supported by this libaray !"
/***TYPE 3***/
#elif  defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega323__)
	#error "AVR currently not supported by this libaray !"
/***TYPE 4***/
#elif  defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
	#error "AVR currently not supported by this libaray !"
/***TYPE 5***/
#elif defined(__AVR_ATmega163__)
	#error "AVR currently not supported by this libaray !"
/***TYPE 6***/
#elif defined(__AVR_ATmega162__)
	#error "AVR currently not supported by this libaray !"
/***TYPE 7***/
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
#define TIMER0_CONTROL TCCR0
#define TIMER0_COUNTER TCNT0
#define TIMER0_COMPARE OCR0
#define TIMER_STATUS ASSR
#define TIMER_INTERRUPT_MASK TIMSK
#define TIMER_INTERRUPT_FLAG TIFR
#define TIMER_SPECIAL_FUNCTION SFIOR
#define TIMER0_COMPARE_MATCH_INTERRUPT TIMER0_COMP_vect
#define TIMER0_OVERFLOW_INTERRUPT TIMER0_OVF_vect
/***TYPE 8***/
#elif defined(__AVR_ATmega161__)
	#error "AVR currently not supported by this libaray !"
/***TYPE 9***/
#elif defined(__AVR_ATmega169__)
/***TYPE 10***/
#elif defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
      defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
      defined(__AVR_ATmega328P__)
/* TLS-Added 48P/88P/168P/328P */
#define TIMER0_CONTROL TCCR0
#define TIMER0_COUNTER TCNT0
#define TIMER0_COMPARE OCR0
#define TIMER_STATUS ASSR
#define TIMER_INTERRUPT_MASK TIMSK
#define TIMER_INTERRUPT_FLAG TIFR
#define TIMER_SPECIAL_FUNCTION SFIOR
#define TIMER0_COMPARE_MATCH_INTERRUPT TIMER0_COMP_vect
#define TIMER0_OVERFLOW_INTERRUPT TIMER0_OVF_vect
/***TYPE 11***/
#elif defined(__AVR_ATtiny2313__)
/***TYPE 12***/
#elif defined(__AVR_ATmega329__) ||\
      defined(__AVR_ATmega649__) ||\
      defined(__AVR_ATmega325__) ||defined(__AVR_ATmega3250__) ||\
      defined(__AVR_ATmega645__) ||defined(__AVR_ATmega6450__)
/* ATmega with one USART */
/***TYPE 13***/
#elif defined(__AVR_ATmega3290__) || defined(__AVR_ATmega6490__)
/***TYPE 14***/
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega640__)
/***TYPE 15***/
#elif defined(__AVR_ATmega644__)
/***TYPE 16***/
#elif defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644P__)
#else
/***TYPE 17***/
 #error "no UART definition for MCU available"
#endif
/*
** variable
*/
unsigned char timer0_state;
/*
** procedure and function header
*/
void TIMER0_start(unsigned char compare, unsigned int prescaler);
void TIMER0_compare(unsigned char compare);
void TIMER0_stop(void);
/*
** procedure and function
*/
TIMER0 TIMER0enable(unsigned char wavegenmode, unsigned char compoutmode, unsigned char interrupt)
/*
	PARAMETER SETTING
	wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
	compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
	Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
	Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
	default-Normal port operation, OC0 disconnected.
	interrupt: off; overflow; output compare; both; default - non.
*/
{
	//
	struct TIMER0 timer0;
	timer0.wavegenmode=wavegenmode;
	timer0.compoutmode=compoutmode;
	timer0.interrupt=interrupt;
	timer0_state=0;
	//
	switch(timer0.wavegenmode){
		case 0: // Normal
			TIMER0_CONTROL&=~(1<<(WGM00) | (1<<WGM01));
			break;
		case 1: // PWM, Phase Correct
			TIMER0_CONTROL&=~(1<<WGM01);
			TIMER0_CONTROL|=(1<<WGM00);
			break;
		case 2: // CTC
			TIMER0_CONTROL|=(1<<WGM01);
			TIMER0_CONTROL&=~(1<<WGM00);
			break;
		case 3: // Fast PWM
			TIMER0_CONTROL|=((1<<WGM00) | (1<<WGM01));
			break;
		default:
			TIMER0_CONTROL&=~((1<<WGM00) | (1<<WGM01));
			break;
	}
	switch(timer0.compoutmode){
		case 0: // Normal port operation, OC0 disconnected.
			TIMER0_CONTROL&=~(1<<(COM00) | (1<<COM01));
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER0_CONTROL&=~(1<<COM01);
			TIMER0_CONTROL|=(1<<COM00);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER0_CONTROL|=(1<<COM01);
			TIMER0_CONTROL&=~(1<<COM00);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER0_CONTROL|=((1<<COM00) | (1<<COM01));
			break;
		default:
			TIMER0_CONTROL&=~((1<<COM00) | (1<<COM01));
			break;
	}
	TIMER0_CONTROL&=~(7<<CS0);
	TIMER0_COMPARE=0;
	switch(timer0.interrupt){
		// Timer/Counter0 Output Compare Match Interrupt Enable
		// TOIE0: Timer/Counter0 Overflow Interrupt Enable
		case 0: 
			TIMER_INTERRUPT_MASK&=~(1<<(TOIE0));
			TIMER_INTERRUPT_MASK&=~(1<<(OCIE0));
			break;
		case 1:
			TIMER_INTERRUPT_MASK|=(1<<(TOIE0));
			TIMER_INTERRUPT_MASK&=~(1<<(OCIE0));
			break;
		case 2:
			TIMER_INTERRUPT_MASK&=~(1<<(TOIE0));
			TIMER_INTERRUPT_MASK|=(1<<(OCIE0));
			break;
		case 3:
			TIMER_INTERRUPT_MASK|=(1<<(TOIE0));
			TIMER_INTERRUPT_MASK|=(1<<(OCIE0));
			break;
		default:
			TIMER_INTERRUPT_MASK&=~(1<<(TOIE0));
			TIMER_INTERRUPT_MASK&=~(1<<(OCIE0));
			break;
	}
	//
	timer0.start=TIMER0_start;
	timer0.compare=TIMER0_compare;
	timer0.stop=TIMER0_stop;
	//
	return timer0;
}
void TIMER0_start(unsigned char compare, unsigned int prescaler)
/*
	PARAMETER SETTING
	compare: unsigned char value from 0 to 255.
	Frequency oscilator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /32 (From prescaler);
	clk T0S /64 (From prescaler); clk T0S /128 (From prescaler); clk T 0 S /256 (From prescaler);
	clk T 0 S /1024 (From prescaler); default - clk T 0 S /1024 (From prescaler).
*/
{
	if(timer0_state==0){ // oneshot
		TIMER0_COMPARE=compare;
		switch(prescaler){
			case 1: // clk T0S /(No prescaling)
				TIMER0_CONTROL|=(1<<(CS00));
				TIMER0_CONTROL&=~(3<<(CS01));
				break;
			case 8: // clk T0S /8 (From prescaler)
				TIMER0_CONTROL|=(1<<CS01);
				TIMER0_CONTROL&=~(5<<CS00);
				break;
			case 32: // clk T0S /32 (From prescaler)
				TIMER0_CONTROL&=~(1<<CS02);
				TIMER0_CONTROL|=(3<<CS00);
				break;
			case 64: // clk T0S /64 (From prescaler)
				TIMER0_CONTROL|=(4<<CS00);
				TIMER0_CONTROL&=~(3<<CS00);
				break;
			case 128: // clk T0S /128 (From prescaler)
				TIMER0_CONTROL|=(5<<CS00);
				TIMER0_CONTROL&=~(1<<CS01);
				break;
			case 256: // clk T 0 S /256 (From prescaler)
				TIMER0_CONTROL|=(6<<CS00);
				TIMER0_CONTROL&=~(1<<CS00);
				break;
			case 1024: // clk T 0 S /1024 (From prescaler)
				TIMER0_CONTROL|=(7<<CS00);
				break;
			default:
				TIMER0_CONTROL|=(7<<CS00);
				break;
		}
		timer0_state=1;
	}	
}
void TIMER0_compare(unsigned char compare)
{
	TIMER0_COMPARE=compare;
}
void TIMER0_stop(void)
/*
	stops timer by setting prescaler to zero
*/
{
	TIMER0_CONTROL&=~(7<<CS0);
	TIMER0_COUNTER=0X00;
	timer0_state=0;
}
/*
** interrupt
*/
// to be defined in MAIN file
// Note if you configure to use interrupts and do not define them program will block,
// so be carefull when enabling, make sure corect parameters for specified aplication are applied.
/***EOF***/
