/*************************************************************************
Title:    TIMER
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: atmega328timer.c,v 0.1 2017/06/14 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware:
	AVR ATmega328 at 16 Mhz,
License:  GNU General Public License
Comment:
	Very Stable
*************************************************************************/
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
/***/
#include "atmega328timer.h"
/*
** constant and macro
*/
#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
	/***0***/
	#define SYSTEM_REGISTER SREG
	#define GLOBAL_INTERRUPT_ENABLE 7
	#define TIMER_COUNTER0A_CONTROL_REGISTER TCCR0A
	#define TIMER_COUNTER0B_CONTROL_REGISTER TCCR0B
	#define TIMER_COUNTER0_REGISTER TCNT0
	#define TIMER_COUNTER0A_COMPARE_REGISTER OCR0A
	#define TIMER_COUNTER0B_COMPARE_REGISTER OCR0B
	#define TIMER_COUNTER0A_COMPARE_MATCH_INTERRUPT TIMER0_COMPA_vect
	#define TIMER_COUNTER0B_COMPARE_MATCH_INTERRUPT TIMER0_COMPB_vect
	#define TIMER_COUNTER0_OVERFLOW_INTERRUPT TIMER0_OVF_vect
	/***1***/
	#define TIMER_COUNTER1A_CONTROL_REGISTER TCCR1A
	#define TIMER_COUNTER1B_CONTROL_REGISTER TCCR1B
	#define TIMER_COUNTER1C_CONTROL_REGISTER TCCR1C
	#define TIMER_COUNTER1_REGISTER TCNT1 // H and L register
	#define TIMER_COUNTER1A_COMPARE_REGISTER OCR1A
	#define TIMER_COUNTER1B_COMPARE_REGISTER OCR1B
	#define TIMER_COUNTER1_INPUT_CAPTURE_REGISTER ICR1
	#define TIMER_COUNTER1A_COMPARE_MATCH_INTERRUPT TIMER1_COMPA_vect
	#define TIMER_COUNTER1B_COMPARE_MATCH_INTERRUPT TIMER1_COMPB_vect
	#define TIMER_COUNTER1_CAPTURE_EVENT_INTERRUPT TIMER1_CAPT_vect
	#define TIMER_COUNTER1_OVERFLOW_INTERRUPT TIMER1_OVF_vect
	/***2***/
	#define TIMER_COUNTER2A_CONTROL_REGISTER TCCR2A
	#define TIMER_COUNTER2B_CONTROL_REGISTER TCCR2B
	#define TIMER_COUNTER2_REGISTER TCNT2
	#define TIMER_COUNTER2A_COMPARE_REGISTER OCR2A
	#define TIMER_COUNTER2B_COMPARE_REGISTER OCR2B
	#define TIMER_COUNTER2A_COMPARE_MATCH_INTERRUPT TIMER2_COMPA_vect
	#define TIMER_COUNTER2B_COMPARE_MATCH_INTERRUPT TIMER2_COMPB_vect
	#define TIMER_COUNTER2_OVERFLOW_INTERRUPT TIMER2_OVF_vect
	/***COMMON***/
	#define TIMER_COUNTER0_INTERRUPT_MASK_REGISTER TIMSK0
	#define TIMER_COUNTER1_INTERRUPT_MASK_REGISTER TIMSK1
	#define TIMER_COUNTER2_INTERRUPT_MASK_REGISTER TIMSK2
	#define TIMER_COUNTER0_INTERRUPT_FLAG_REGISTER TIFR0
	#define TIMER_COUNTER1_INTERRUPT_FLAG_REGISTER TIFR1
	#define TIMER_COUNTER2_INTERRUPT_FLAG_REGISTER TIFR2
	#define ASYNCHRONOUS_STATUS_REGISTER ASSR
	#define GENERAL_TIMER_COUNTER_CONTROL_REGISTER GTCCR
/*
** variable
*/
static unsigned char timer0_state;
static unsigned char timer1_state;
static unsigned char timer2_state;
/*
** procedure and function header
*/
void TIMER_COUNTER0_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER0_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER0_compareA(unsigned char compare);
void TIMER_COUNTER0_compareB(unsigned char compare);
void TIMER_COUNTER0_start(unsigned int prescaler);
void TIMER_COUNTER0_stop(void);
/******/
void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER1_compareA(uint16_t compare);
void TIMER_COUNTER1_compareB(uint16_t compare);
void TIMER_COUNTER1_start(unsigned int prescaler);
void TIMER_COUNTER1_stop(void);
/******/
void TIMER_COUNTER2_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER2_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER2_compareA(unsigned char compare);
void TIMER_COUNTER2_compareB(unsigned char compare);
void TIMER_COUNTER2_start(unsigned int prescaler);
void TIMER_COUNTER2_stop(void);
/*
** procedure and function
*/
TIMER_COUNTER0 TIMER_COUNTER0enable(unsigned char wavegenmode, unsigned char interrupt)
/*
	PARAMETER SETTING
	wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
	interrupt: off; overflow; output compare; both; default - non.
*/
{
	TIMER_COUNTER0 timer0;
	timer0_state=0;
	TIMER_COUNTER0A_CONTROL_REGISTER&=~((1<<WGM01) | (1<<WGM00));
	TIMER_COUNTER0B_CONTROL_REGISTER&=~(1<<WGM02);
	switch(wavegenmode){
		case 0: // Normal
			break;
		case 1: // PWM, Phase Correct, 8-bit
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<WGM00);
			break;
		case 2: // PWM, Phase Correct, 9-bit
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<WGM01);
			break;
		case 3: // Fast PWM
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<WGM00) | (1<<WGM01);
			break;
		case 5: // PWM, Phase Correct
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<WGM00);
			TIMER_COUNTER0B_CONTROL_REGISTER|=(1<<WGM02);
			break;
		case 7: // Fast PWM
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<WGM00) | (1<<WGM01);
			TIMER_COUNTER0B_CONTROL_REGISTER|=(1<<WGM02);
			break;
		default:
			break;
	}
	TIMER_COUNTER0_INTERRUPT_MASK_REGISTER&=~((1<<OCIE0B) | (1<<OCIE0A) | (1<<TOIE0));
	switch(interrupt){
		case 0: 
			break;
		case 1:
			TIMER_COUNTER0_INTERRUPT_MASK_REGISTER|=(1<<TOIE0);
			break;
		case 2:
			TIMER_COUNTER0_INTERRUPT_MASK_REGISTER|=(1<<OCIE0A);
			break;
		case 3:
			TIMER_COUNTER0_INTERRUPT_MASK_REGISTER|=(1<<OCIE0B);
			break;
		case 4:
			TIMER_COUNTER0_INTERRUPT_MASK_REGISTER|=(1<<TOIE0) | (1<<OCIE0A);
			break;
		case 5:
			TIMER_COUNTER0_INTERRUPT_MASK_REGISTER|=(1<<TOIE0) | (1<<OCIE0B);
			break;
		case 6:
			TIMER_COUNTER0_INTERRUPT_MASK_REGISTER|=(1<<TOIE0) | (1<<OCIE0A) | (1<<OCIE0B);
			break;
		case 7:
			TIMER_COUNTER0_INTERRUPT_MASK_REGISTER|=(1<<OCIE0A) | (1<<OCIE0B);
			break;
		default:
			break;
	}
	timer0.compoutmodeA=TIMER_COUNTER0_compoutmodeA;
	timer0.compoutmodeB=TIMER_COUNTER0_compoutmodeB;
	timer0.compareA=TIMER_COUNTER0_compareA;
	timer0.compareB=TIMER_COUNTER0_compareB;
	timer0.start=TIMER_COUNTER0_start;
	timer0.stop=TIMER_COUNTER0_stop;
	return timer0;
}
void TIMER_COUNTER0_start(unsigned int prescaler)
/*
	PARAMETER SETTING
	Frequency oscillator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /32 (From prescaler);
	clk T0S /64 (From prescaler); clk T0S /128 (From prescaler); clk T 0 S /256 (From prescaler);
	clk T 0 S /1024 (From prescaler); default - clk T 0 S /1024 (From prescaler).
*/
{
	if(timer0_state==0){ // oneshot
		TIMER_COUNTER0A_COMPARE_REGISTER=0XFF;
		TIMER_COUNTER0B_CONTROL_REGISTER&=~(7<<CS00); // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clk T0S /(No prescaling)
				TIMER_COUNTER0B_CONTROL_REGISTER|=(1<<CS00);
				break;
			case 8: // clk T0S /8 (From prescaler)
				TIMER_COUNTER0B_CONTROL_REGISTER|=(1<<CS01);
				break;
			case 64: // clk T0S /64 (From prescaler)
				TIMER_COUNTER0B_CONTROL_REGISTER|=(3<<CS00);
				break;
			case 256: // clk T 0 S /256 (From prescaler)
				TIMER_COUNTER0B_CONTROL_REGISTER|=(4<<CS00);
				break;
			case 1024: // clk T 0 S /1024 (From prescaler)
				TIMER_COUNTER0B_CONTROL_REGISTER|=(5<<CS00);
				break;
			case 3: // External clock source on T0 pin. Clock on falling edge.
				TIMER_COUNTER0B_CONTROL_REGISTER|=(6<<CS00);
				break;
			case 5: // External clock source on T0 pin. Clock on rising edge.
				TIMER_COUNTER0B_CONTROL_REGISTER|=(7<<CS00);
				break;
			default:
				break;
		}
		SYSTEM_REGISTER|=(1<<GLOBAL_INTERRUPT_ENABLE);
		timer0_state=1;
	}	
}
void TIMER_COUNTER0_compoutmodeA(unsigned char compoutmode)
/*
	compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
	Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
	Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
	default-Normal port operation, OC0 disconnected.
*/
{
	TIMER_COUNTER0A_CONTROL_REGISTER&=~((1<<COM0A0) | (1<<COM0A1));
	switch(compoutmode){
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<COM0A0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<COM0A1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<COM0A0) | (1<<COM0A1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER0_compoutmodeB(unsigned char compoutmode)
/*
	compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
	Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
	Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
	default-Normal port operation, OC0 disconnected.
*/
{
	TIMER_COUNTER0A_CONTROL_REGISTER&=~((1<<COM0B0) | (1<<COM0B1));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<COM0B0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<COM0B1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER0A_CONTROL_REGISTER|=(1<<COM0B0) | (1<<COM0B1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER0_compareA(unsigned char compare)
{
	TIMER_COUNTER0A_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER0_compareB(unsigned char compare)
{
	TIMER_COUNTER0B_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER0_stop(void)
/*
	stops timer by setting prescaler to zero
*/
{
	TIMER_COUNTER0B_CONTROL_REGISTER&=~(7<<CS00); // No clock source. (Timer/Counter stopped)
	TIMER_COUNTER0_REGISTER=0X00;
	timer0_state=0;
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
	TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<WGM11) | (1<<WGM10));
	TIMER_COUNTER1B_CONTROL_REGISTER&=~((1<<WGM13) | (1<<WGM12));
	switch(wavegenmode){
		case 0: // Normal
			break;
		case 1: // PWM, Phase Correct, 8-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			break;
		case 2:	// PWM, Phase Correct, 9-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			break;
		case 3:	// PWM, Phase Correct, 10-bit
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
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
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
			break;
		case 8:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 9:	// PWM, Phase and Frequency Correct
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 10: // PWM, Phase Correct
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 11: // PWM, Phase Correct
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
			break;
		case 12: // CTC
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		case 13: // (Reserved)
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		case 14: // Fast PWM
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		case 15: // Fast PWM
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
			TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
			break;
		default:
			break;
	}
	TIMER_COUNTER1A_CONTROL_REGISTER&=~((3<<COM1A0) | (3<<COM1B0));
	TIMER_COUNTER1_INTERRUPT_MASK_REGISTER&=~((1<<ICIE1)  | (1<<OCIE1B) | (1<<OCIE1A) | (1<<TOIE1));
	switch(interrupt){
		case 0:
			break;
		case 1:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<TOIE1);
			break;
		case 2:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A);
			break;
		case 3:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<OCIE1B);
			break;
		case 4:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<ICIE1);
			break;
		case 5:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B);
			break;
		case 6:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<TOIE1);
			break;
		case 7:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<OCIE1B) | (1<<TOIE1);
			break;
		case 8:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<ICIE1) | (1<<TOIE1);
			break;
		case 9:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1);
			break;
		case 10:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<ICIE1) | (1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1);
			break;
		case 11:
			TIMER_COUNTER1_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B);
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
void TIMER_COUNTER1_start(unsigned int prescaler)
/*
	PARAMETER SETTING
	Frequency oscillator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
	clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
	External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
*/
{
	if(timer1_state==0){ // oneshot
		TIMER_COUNTER1A_COMPARE_REGISTER=0XFFFF;
		TIMER_COUNTER1B_CONTROL_REGISTER&=~(7<<CS10); // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clkI/O/1 (No prescaling)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS10);
				break;
			case 8: // clkI/O/8 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS11);
				break;
			case 64: // clkI/O/64 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(3<<CS10);
				break;
			case 256: // clkI/O/256 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS12);
				break;
			case 1024: // clkI/O/1024 (From prescaler)
				TIMER_COUNTER1B_CONTROL_REGISTER|=(5<<CS10);
				break;
			case 3: // External clock source on Tn pin. Clock on falling edge
				TIMER_COUNTER1B_CONTROL_REGISTER|=(6<<CS10);
				break;
			case 5: // External clock source on Tn pin. Clock on rising edge
				TIMER_COUNTER1B_CONTROL_REGISTER|=(7<<CS10);
				break;
			default:
				TIMER_COUNTER1B_CONTROL_REGISTER|=(5<<CS10);
				break;
		}
		SYSTEM_REGISTER|=(1<<GLOBAL_INTERRUPT_ENABLE);
		timer1_state=1;
	}	
}
void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode)
{
	TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1A0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
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
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A0) | (1<<COM1A1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode)
{
	TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1B0);
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
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
			TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B0) | (1<<COM1B1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER1_compareA(uint16_t compare)
{
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
	TIMER_COUNTER1B_CONTROL_REGISTER&=~(7<<CS10); // No clock source. (Timer/Counter stopped)
	TIMER_COUNTER1_REGISTER=0X0000;
	timer1_state=0;
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
	TIMER_COUNTER2A_CONTROL_REGISTER&=~((1<<WGM21) | (1<<WGM20));
	TIMER_COUNTER2B_CONTROL_REGISTER&=~(1<<WGM22);
	switch(wavegenmode){
		case 0: // Normal
			break;
		case 1: // PWM, Phase Correct
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<WGM20);
			break;
		case 2: // CTC
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<WGM21);
			break;
		case 3: // Fast PWM
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<WGM20) | (1<<WGM21);
			break;
		case 5: // PWM, Phase Correct
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<WGM20);
			TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<WGM22);
			break;
		case 7: // Fast PWM
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<WGM20) | (1<<WGM21);
			TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<WGM22);
			break;
		default:
			break;
	}
	TIMER_COUNTER2_INTERRUPT_MASK_REGISTER&=~((1<<OCIE2B) | (1<<OCIE2A) | (1<<TOIE2));
	switch(interrupt){
		case 0: 
			break;
		case 1:
			TIMER_COUNTER2_INTERRUPT_MASK_REGISTER|=(1<<TOIE2);
			break;
		case 2:
			TIMER_COUNTER2_INTERRUPT_MASK_REGISTER|=(1<<OCIE2A);
			break;
		case 3:
			TIMER_COUNTER2_INTERRUPT_MASK_REGISTER|=(1<<TOIE2) | (1<<OCIE2A);
			break;
		case 4:
			TIMER_COUNTER2_INTERRUPT_MASK_REGISTER|=(1<<TOIE2) | (1<<OCIE2B);
			break;
		case 5:
			TIMER_COUNTER2_INTERRUPT_MASK_REGISTER|=(1<<OCIE2B) | (1<<OCIE2A);
			break;
		case 6:
			TIMER_COUNTER2_INTERRUPT_MASK_REGISTER|=(1<<OCIE2B) | (1<<OCIE2A) | (1<<TOIE2);
			break;
		default:
			break;
	}
	timer2.compoutmodeA=TIMER_COUNTER2_compoutmodeA;
	timer2.compoutmodeB=TIMER_COUNTER2_compoutmodeB;
	timer2.compareA=TIMER_COUNTER2_compareA;
	timer2.compareB=TIMER_COUNTER2_compareB;
	timer2.start=TIMER_COUNTER2_start;
	timer2.stop=TIMER_COUNTER2_stop;
	return timer2;
}
void TIMER_COUNTER2_start(unsigned int prescaler)
/*
	PARAMETER SETTING
	Frequency oscillator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
	clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
	External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
*/
{
	if(timer2_state==0){ // oneshot
		TIMER_COUNTER2A_COMPARE_REGISTER=0XFF;
		TIMER_COUNTER2B_CONTROL_REGISTER&=~(7<<CS20); // No clock source. (Timer/Counter stopped)
		switch(prescaler){
			case 1: // clkI/O/(No prescaling)
				TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<CS20);
				break;
			case 8: // clkI/O/8 (From prescaler)
				TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<CS21);
				break;
			case 32: // clkI/O/32 (From prescaler)
				TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<CS21) | (1<<CS20);
				break;
			case 64: // clkI/O/64 (From prescaler)
				TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<CS22);
				break;
			case 128: // clkI/O/128 (From prescaler)
				TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<CS22) | (1<<CS20);
				break;
			case 256: // clkI/O/256 (From prescaler)
				TIMER_COUNTER2B_CONTROL_REGISTER|=(1<<CS22) | (1<<CS21);
				break;
			case 1024: // clkI/O/1024 (From prescaler)
				TIMER_COUNTER2B_CONTROL_REGISTER|=(7<<CS20);
				break;
			default:
				TIMER_COUNTER2B_CONTROL_REGISTER|=(7<<CS20);
				break;
		}
		SYSTEM_REGISTER|=(1<<GLOBAL_INTERRUPT_ENABLE);
		timer2_state=1;
	}	
}
void TIMER_COUNTER2_compoutmodeA(unsigned char compoutmode)
/*
	compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
	Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
	Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
	default-Normal port operation, OC0 disconnected.
*/
{
	TIMER_COUNTER2A_CONTROL_REGISTER&=~((1<<COM2A0) | (1<<COM2A1));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<COM2A0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<COM2A1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<COM2A0) | (1<<COM2A1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER2_compoutmodeB(unsigned char compoutmode)
/*
	compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
	Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
	Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
	default-Normal port operation, OC0 disconnected.
*/
{
	TIMER_COUNTER2A_CONTROL_REGISTER&=~((1<<COM2B0) | (1<<COM2B1));
	switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
		case 0: // Normal port operation, OC0 disconnected.
			break;
		case 1: // Reserved
				// Toggle OC0 on compare match
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<COM2B0);
			break;
		case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
				// match when downcounting.
				// Clear OC0 on compare match
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<COM2B1);
			break;
		case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
				// match when downcounting.
				// Set OC0 on compare match
			TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<COM2B0) | (1<<COM2B1);
			break;
		default:
			break;
	}
}
void TIMER_COUNTER2_compareA(unsigned char compare)
{
	TIMER_COUNTER2A_COMPARE_REGISTER=compare;
}
	void TIMER_COUNTER2_compareB(unsigned char compare)
{
	TIMER_COUNTER2B_COMPARE_REGISTER=compare;
}
void TIMER_COUNTER2_stop(void)
/*
	stops timer by setting prescaler to zero
*/
{
	TIMER_COUNTER2B_CONTROL_REGISTER&=~(7<<CS20); // No clock source. (Timer/Counter stopped)
	TIMER_COUNTER2_REGISTER=0X00;
	timer2_state=0;
}
/*
** interrupt
*/
#else
	#error "Not Atmega328"
#endif
/***EOF***/
/***COMMENTS
interrupt to be defined in MAIN file
Note if you configure to use interrupts and do not define them, program will block,
so be careful when enabling, make sure correct parameters for specified application are applied.
***/
