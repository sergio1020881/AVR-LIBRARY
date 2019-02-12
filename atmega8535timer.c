/*************************************************************************
Title:    ATMEGA8535TIMER
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: timer.c,v 0.1 2016/07/26 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware:
	Atmega 8535 at 8Mhz
License:  GNU General Public License 
DESCRIPTION:
	Refer to datasheet
USAGE:
	function oriented
NOTES:
    Based on Atmega
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
	TESTED
	Very Stable
*************************************************************************/
#ifndef F_CPU
  #define F_CPU 8000000UL
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
#include "atmega8535timer.h"
/*
** constant and macro
*/
#if  defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
	
	#define TIMER_COUNTER0_CONTROL_REGISTER TCCR0
	#define TIMER_COUNTER0_REGISTER TCNT0
	#define TIMER_COUNTER0_COMPARE_REGISTER OCR0
	#define TIMER_COUNTER0_OVERFLOW_INTERRUPT TIMER0_OVF_vect
	/***1***/
	#define TIMER_COUNTER1A_CONTROL_REGISTER TCCR1A
	#define TIMER_COUNTER1B_CONTROL_REGISTER TCCR1B
	#define TIMER_COUNTER1_REGISTER TCNT1
	#define TIMER_COUNTER1A_COMPARE_REGISTER OCR1A
	#define TIMER_COUNTER1B_COMPARE_REGISTER OCR1B
	#define TIMER_COUNTER1_INPUT_CAPTURE_REGISTER ICR1
	#define TIMER_COUNTER1A_COMPARE_MATCH_INTERRUPT TIMER1_COMPA_vect
	#define TIMER_COUNTER1B_COMPARE_MATCH_INTERRUPT TIMER1_COMPB_vect
	#define TIMER_COUNTER1_CAPTURE_EVENT_INTERRUPT TIMER1_CAPT_vect
	#define TIMER_COUNTER1_OVERFLOW_INTERRUPT TIMER1_OVF_vect
	/***2***/
	#define TIMER_COUNTER2_CONTROL_REGISTER TCCR2
	#define TIMER_COUNTER2_REGISTER TCNT2
	#define TIMER_COUNTER2_COMPARE_REGISTER OCR2
	#define TIMER_COUNTER2_COMPARE_MATCH_INTERRUPT TIMER2_COMP_vect
	#define TIMER_COUNTER2_OVERFLOW_INTERRUPT TIMER2_OVF_vect
	/***COMMON***/
	#define TIMER_COUNTER_ASYNCHRONOUS_STATUS_REGISTER ASSR
	#define TIMER_COUNTER_INTERRUPT_MASK_REGISTER TIMSK
	#define TIMER_COUNTER_INTERRUPT_FLAG_REGISTER TIFR
	#define TIMER_COUNTER_SPECIAL_FUNCTION_IO_REGISTER SFIOR
/*
** variable
*/
volatile uint8_t timer0_state;
uint8_t timer0_prescaler;
volatile uint8_t timer1_state;
uint8_t timer1_prescaler;
volatile uint8_t timer2_state;
uint8_t timer2_prescaler;
/*
** procedure and function header
*/
void TIMER_COUNTER0_compoutmode(unsigned char compoutmode);
void TIMER_COUNTER0_compare(unsigned char compare);
void TIMER_COUNTER0_start(uint16_t prescaler);
void TIMER_COUNTER0_stop(void);
/******/
void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER1_compareA(uint16_t compare);
void TIMER_COUNTER1_compareB(uint16_t compare);
void TIMER_COUNTER1_start(uint16_t prescaler);
void TIMER_COUNTER1_stop(void);
/******/
void TIMER_COUNTER2_compoutmode(unsigned char compoutmode);
void TIMER_COUNTER2_compare(unsigned char compare);
void TIMER_COUNTER2_start(uint16_t prescaler);
void TIMER_COUNTER2_stop(void);
/*
** procedure and function
*/
TIMER_COUNTER0 TIMER_COUNTER0enable(unsigned char wavegenmode, unsigned char interrupt)
/*
	No Output Compare Interrupt on timer 0.
	PARAMETER SETTING
	wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
	interrupt: off; overflow; output compare; both; default - non.
*/
{
	TIMER_COUNTER0 timer0;
	timer0_state=0;
	
	TIMER_COUNTER0_COMPARE_REGISTER=0XFF;
	TIMER_COUNTER0_CONTROL_REGISTER&=~((1<<WGM00) | (1<<WGM01));
	switch(wavegenmode){
		case 1: // PWM, Phase Correct
			TIMER_COUNTER0_CONTROL_REGISTER&=~(1<<FOC0);
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM00);
			break;
		case 2: // CTC
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM01);
			break;
		case 3: // Fast PWM
			TIMER_COUNTER0_CONTROL_REGISTER&=~(1<<FOC0);
			TIMER_COUNTER0_CONTROL_REGISTER|=((1<<WGM00) | (1<<WGM01));
			break;
		default: // Normal
			break;
	}
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<OCIE0) | (1<<TOIE0));
	switch(interrupt){
		case 1:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE0);
			break;
		case 2:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE0);
			break;
		case 3:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=((1<<OCIE0) | (1<<TOIE0));
			break;
		default:
			break;
	}
	timer0.compoutmode=TIMER_COUNTER0_compoutmode;
	timer0.compare=TIMER_COUNTER0_compare;
	timer0.start=TIMER_COUNTER0_start;
	timer0.stop=TIMER_COUNTER0_stop;
	
	return timer0;
}
/*****************************************************************************************/
TIMER_COUNTER1 TIMER_COUNTER1enable(unsigned char wavegenmode, unsigned char interrupt)
/*
	PARAMETER SETTING
	wavegen mode: Normal; PWM, Phase Correct, 8-bit; PWM, Phase Correct, 9-bit; PWM, Phase Correct, 10-bit;
	CTC; Fast PWM, 8-bit; Fast PWM, 9-bit; Fast PWM, 10-bit; PWM, Phase and Frequency Correct; PWM, Phase and Frequency Correct;
	PWM, Phase Correct; PWM, Phase Correct; CTC; (Reserved); Fast PWM; Fast PWM.
	interrupt: off; overflow; output compare; both; default - non.
	for more information read datasheet.
*/
{
	TIMER_COUNTER1 timer1;
	timer1_state=0;
	
	TIMER_COUNTER1A_COMPARE_REGISTER=0XFFFF;
	TIMER_COUNTER1B_COMPARE_REGISTER=0XFFFF;
	TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<WGM11) | (1<<WGM10));
	TIMER_COUNTER1B_CONTROL_REGISTER&=~((1<<WGM13) | (1<<WGM12));
	switch(wavegenmode){
		case 1: // PWM, Phase Correct, 8-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			break;
		case 2:	// PWM, Phase Correct, 9-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			break;
		case 3:	// PWM, Phase Correct, 10-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=((1<<WGM11) | (1<<WGM10));
			break;
		case 4:	// CTC
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 5:	// Fast PWM, 8-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 6:	// Fast PWM, 9-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 7:	// Fast PWM, 10-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=((1<<WGM11) | (1<<WGM10));
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 8:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 9:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<FOC1A) | (1<<FOC1B));
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 10: // PWM, Phase Correct
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 11: // PWM, Phase Correct
			TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<FOC1A) | (1<<FOC1B));
			TIMER_COUNTER1A_CONTROL_REGISTER|=((1<<WGM11) | (1<<WGM10));
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 12: // CTC
			TIMER_COUNTER1B_CONTROL_REGISTER|=((1<<WGM13) | (1<<WGM12));
			break;
		case 13: // (Reserved)
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=((1<<WGM13) | (1<<WGM12));
			break;
		case 14: // Fast PWM
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=((1<<WGM13) | (1<<WGM12));
			break;
		case 15: // Fast PWM
			TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<FOC1A) | (1<<FOC1B));
			TIMER_COUNTER1A_CONTROL_REGISTER|=((1<<WGM11) | (1<<WGM10));
			TIMER_COUNTER1B_CONTROL_REGISTER|=((1<<WGM13) | (1<<WGM12));
			break;
		default: // Normal
			break;
	}
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TICIE1) | (1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1));
	switch(interrupt){
		case 1:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE1);
			break;
		case 2:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1B);
			break;
		case 3:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A);
		break;
			case 4:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE1);
		break;
			case 6:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=((1<<OCIE1B) | (1<<TOIE1));
			break;
		case 7:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=((1<<OCIE1A) | (1<<TOIE1));
			break;
		case 8:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=((1<<OCIE1A) | (1<<OCIE1B));
			break;
		case 9:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=((1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1));
			break;
		default:
			break;
	}
	//
	timer1.compoutmodeA=TIMER_COUNTER1_compoutmodeA;
	timer1.compoutmodeB=TIMER_COUNTER1_compoutmodeB;
	timer1.compareA=TIMER_COUNTER1_compareA;
	timer1.compareB=TIMER_COUNTER1_compareB;
	timer1.start=TIMER_COUNTER1_start;
	timer1.stop=TIMER_COUNTER1_stop;
	
	return timer1;
}
/*****************************************************************************************/
TIMER_COUNTER2 TIMER_COUNTER2enable(unsigned char wavegenmode, unsigned char interrupt)
	/*
		PARAMETER SETTING
		wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
		interrupt: off; overflow; output compare; both; default - non.
	*/
{
	TIMER_COUNTER2 timer2;
	timer2_state=0;
	
	TIMER_COUNTER2_COMPARE_REGISTER=0XFF;
	TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<WGM20) | (1<<WGM21));
	switch(wavegenmode){
		case 1: // PWM, Phase Correct
			TIMER_COUNTER2_CONTROL_REGISTER&=~(1<<FOC2);
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM20);
			break;
		case 2: // CTC
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM21);
			break;
		case 3: // Fast PWM
			TIMER_COUNTER2_CONTROL_REGISTER&=~(1<<FOC2);
			TIMER_COUNTER2_CONTROL_REGISTER|=((1<<WGM20) | (1<<WGM21));
			break;
		default: //// Normal
			break;
	}
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TOIE2) | (1<<OCIE2));
	switch(interrupt){
		case 1:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE2);
			break;
		case 2:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE2);
			break;
		case 3:
			TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=((1<<TOIE2) | (1<<OCIE2));
			break;
		default:
			break;
	}
	timer2.compoutmode=TIMER_COUNTER2_compoutmode;
	timer2.compare=TIMER_COUNTER2_compare;
	timer2.start=TIMER_COUNTER2_start;
	timer2.stop=TIMER_COUNTER2_stop;
	
	return timer2;
}
/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
void TIMER_COUNTER0_start(uint16_t prescaler)
/*
	PARAMETER SETTING
	Frequency oscillator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /32 (From prescaler);
	clk T0S /64 (From prescaler); clk T0S /128 (From prescaler); clk T 0 S /256 (From prescaler);
	clk T 0 S /1024 (From prescaler); default - clk T 0 S /1024 (From prescaler).
*/
{
	if(timer0_state==0){ // oneshot
		TIMER_COUNTER0_REGISTER=0X00;
		timer0_prescaler=0; // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clk T0S /(No prescaler)
				timer0_prescaler=(1<<CS00);
				break;
			case 8: // clk T0S /8 (From prescaler)
				timer0_prescaler=(1<<CS01);
				break;
			case 64: // clk T0S /64 (From prescaler)
				timer0_prescaler=((1<<CS01) | (1<<CS00));
				break;
			case 256: // clk T 0 S /256 (From prescaler)
				timer0_prescaler=(1<<CS02);
				break;
			case 1024: // clk T 0 S /1024 (From prescaler)
				timer0_prescaler=((1<<CS02) | (1<<CS00));
				break;
			default: // clk T 0 S /1024 (From prescaler)
				timer0_prescaler=((1<<CS02) | (1<<CS00));
				break;
		}
		TIMER_COUNTER0_CONTROL_REGISTER|=timer0_prescaler;
		timer0_state=1;
	}	
}
void TIMER_COUNTER0_compoutmode(unsigned char compoutmode)
/*
	// Only for NON PWM Modes
	compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
	Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
	Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
	default-Normal port operation, OC0 disconnected.
*/
{
	TIMER_COUNTER0_CONTROL_REGISTER&=~((1<<COM01) | (1<<COM00));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 1: // Reserved
			// Toggle OC0 on compare match
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM00);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
			// match when downcounting.
			// Clear OC0 on compare match
			TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM01);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
			// match when downcounting.
			// Set OC0 on compare match
			TIMER_COUNTER0_CONTROL_REGISTER|=((1<<COM00) | (1<<COM01));
			break;
		default: // Normal port operation, OC0 disconnected.
			break;
	}
}
void TIMER_COUNTER0_compare(unsigned char compare)
{
	TIMER_COUNTER0_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER0_stop(void)
/*
	stops timer by setting prescaler to zero
*/
{
	TIMER_COUNTER0_CONTROL_REGISTER&=~((1<<CS02) | (1<<CS01) | (1<<CS00)); // No clock source. (Timer/Counter stopped)
	timer0_state=0;
}
/*****************************************************************************************/
void TIMER_COUNTER1_start(uint16_t prescaler)
/*
	PARAMETER SETTING
	Frequency oscilator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
	clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
	External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
*/
{
	if(timer1_state==0){ // oneshot
		TIMER_COUNTER1_REGISTER=0X0000;
		timer1_prescaler=0; // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clkI/O/1 (No prescaling)
				timer1_prescaler=(1<<CS10);
				break;
			case 8: // clkI/O/8 (From prescaler)
				timer1_prescaler=(1<<CS11);
				break;
			case 64: // clkI/O/64 (From prescaler)
				timer1_prescaler=((1<<CS11) | (1<<CS10));
				break;
			case 256: // clkI/O/256 (From prescaler)
				timer1_prescaler=(1<<CS12);
				break;
			case 1024: // clkI/O/1024 (From prescaler)
				timer1_prescaler=((1<<CS12) | (1<<CS10));
				break;
			case 3: // External clock source on Tn pin. Clock on falling edge
				timer1_prescaler=((1<<CS12) | (1<<CS11));
				break;
			case 5: // External clock source on Tn pin. Clock on rising edge
				timer1_prescaler=((1<<CS12) | (1<<CS11) | (7<<CS10));
				break;
			default:
				timer1_prescaler=((1<<CS12) | (1<<CS10));
				break;
		}
		TIMER_COUNTER1B_CONTROL_REGISTER|=timer1_prescaler;
		timer1_state=1;
	}	
}
void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode)
{
	TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<COM1A1) | (1<<COM1A0));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 1: // Reserved
			// Toggle OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
			// match when downcounting.
			// Clear OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
			// match when downcounting.
			// Set OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=((1<<COM1A0) | (1<<COM1A1));
			break;
		default: // Normal port operation, OC0 disconnected.
			break;
	}
}
void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode)
{
	TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<COM1B1) | (3<<COM1B0));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 1: // Reserved
			// Toggle OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
			// match when downcounting.
			// Clear OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
			// match when downcounting.
			// Set OC0 on compare match
			TIMER_COUNTER1A_CONTROL_REGISTER|=((1<<COM1B0) | (1<<COM1B1));
			break;
		default: // Normal port operation, OC0 disconnected.
			break;
	}
}
void TIMER_COUNTER1_compareA(uint16_t compare)
{
	// TOP VALUE
	TIMER_COUNTER1A_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER1_compareB(uint16_t compare)
{
	TIMER_COUNTER1B_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER1_stop(void)
/*
	stops timer by setting prescaler to zero
*/
{
	TIMER_COUNTER1B_CONTROL_REGISTER&=~((1<<CS12) | (1<<CS11) | (1<<CS10)); // No clock source. (Timer/Counter stopped)
	timer1_state=0;
}
/*****************************************************************************************/
void TIMER_COUNTER2_start(uint16_t prescaler)
/*
	PARAMETER SETTING
	Frequency oscilator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
	clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
	External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
*/
{
	if(timer2_state==0){ // oneshot
		TIMER_COUNTER2_REGISTER=0X00;
		timer2_prescaler=0; // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clkI/O/(No prescaling)
				timer2_prescaler=(1<<CS20);
				break;
			case 8: // clkI/O/8 (From prescaler)
				timer2_prescaler=(1<<CS21);
				break;
			case 32: // clkI/O/32 (From prescaler)
				timer2_prescaler=((1<<CS21) | (1<<CS20));
				break;
			case 64: // clkI/O/64 (From prescaler)
				timer2_prescaler=(1<<CS22);
				break;
			case 128: // clkI/O/128 (From prescaler)
				timer2_prescaler=((1<<CS22) | (1<<CS20));
				break;
			case 256: // clkI/O/256 (From prescaler)
				timer2_prescaler=((1<<CS22) | (1<<CS21));
				break;
			case 1024: // clkI/O/1024 (From prescaler)
				timer2_prescaler=((1<<CS22) | (1<<CS21) | (1<<CS20));
				break;
			default:
				timer2_prescaler=((1<<CS22) | (1<<CS21) | (1<<CS20));
				break;
		}
		TIMER_COUNTER2_CONTROL_REGISTER|=timer2_prescaler;
		timer2_state=1;
	}	
}
void TIMER_COUNTER2_compoutmode(unsigned char compoutmode)
/*
	// Only for NON PWM Modes
	compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
	Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
	Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
	default-Normal port operation, OC0 disconnected.
*/
{
	TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<COM20) | (1<<COM21));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 1: // Reserved
			// Toggle OC0 on compare match
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM20);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
			// match when downcounting.
			// Clear OC0 on compare match
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM21);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
			// match when downcounting.
			// Set OC0 on compare match
			TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM20) | (1<<COM21);
			break;
		default: // Normal port operation, OC0 disconnected.
			break;
	}
}
void TIMER_COUNTER2_compare(unsigned char compare)
{
	TIMER_COUNTER2_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER2_stop(void)
/*
	stops timer by setting prescaler to zero
*/
{
	TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<CS22) | (1<<CS21) | (1<<CS20)); // No clock source. (Timer/Counter stopped)
	timer2_state=0;
}
/*
** interrupt
*/
#else
	#error "Only Supports Atemaga 8535 Sorry!!"
#endif
/***EOF***/
/***COMMENTS
interrupt to be defined in MAIN file
Note if you configure to use interrupts and do not define them, program will block,
so be carefull when enabling, make sure correct parameters for specified aplication are applied.
***/
