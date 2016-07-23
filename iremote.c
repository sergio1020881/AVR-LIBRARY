/*
 * IREMOTE.c
 *
 * Atmega 8535
 * this will consist of a interrupt high to low detect at the IR sensor output
 * that will start a timer at determined intervals reading the states composing the receiving code emitted.
 *
 * Created: 10/07/2016 01:05:53
 * Author: Sérgio Santos
 * Observations:
 * In progress
 * testing and adjustments
 * Usando um commando da tv cabo, na opção TV cabo commando, funciona muito bem usar dois ultimos bytes para codigo,
 * os dois primeiros são constantes. Talvez vou alterar de forma a ser mais generico.
 */ 
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
#include "iremote.h"
/*
** constant and macro
*/
/***TYPE 4***/
#if defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
	/***0***/
	#define General_Interrupt_Control_Register GICR
	#define General_Interrupt_Flag_Register GIFR
	#define MCU_Control_Register MCUCR
	#define MCU_Control_Status_Register MCUCSR
	#define MCU_Control_Status_Register_Mask 0X0F
	/***0***/
	#define ATMEGA_TIMER_COUNTER_85XX
	#define TIMER_COUNTER2_CONTROL_REGISTER TCCR2
	#define TIMER_COUNTER2_REGISTER TCNT2
	#define TIMER_COUNTER2_COMPARE_REGISTER OCR2
	#define TIMER_COUNTER2_COMPARE_MATCH_INTERRUPT TIMER2_COMP_vect
	#define TIMER_COUNTER2_OVERFLOW_INTERRUPT TIMER2_OVF_vect
	/***COMMON***/
	#define TIMER_COUNTER_STATUS_REGISTER ASSR
	#define TIMER_COUNTER_INTERRUPT_MASK_REGISTER TIMSK
	#define TIMER_COUNTER_INTERRUPT_FLAG_REGISTER TIFR
	#define TIMER_COUNTER_SPECIAL_FUNCTION_REGISTER SFIOR
	#define ASYNCHRONOUS_STATUS_REGISTER ASSR
	#define SPECIAL_FUNCTION_IO_REGISTER SFIOR
#endif
/*
** variable
*/
uint8_t ir_prescaler;
uint8_t ir_state;
volatile uint8_t IR_N_BYTE;
volatile uint8_t IR_N_BIT;
volatile uint8_t IRbyte[IR_BYTE+1];
/*
** procedure and function header
*/
volatile uint8_t* IR_KEY(void);
void IR_COUNTER_start(void);
void IR_COUNTER_stop(void);
void IR_INT0_start(void);
void IR_INT0_stop(void);
/*
** procedure and function
*/
IR IRenable()
/*
	PARAMETER SETTING
	wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
	interrupt: off; overflow; output compare; both; default - non.
*/
{
	IR ir;
	ir_state=0;
	IR_N_BYTE=0;
	IR_N_BIT=0;
	/***TYPE 4***/
	#if defined( ATMEGA_TIMER_COUNTER_85XX )
		//INTERRUPT
		General_Interrupt_Control_Register|=(1<<INT0);
		MCU_Control_Register|=(1<<ISC01);
		// TIMER
		//TIMER_COUNTER_SPECIAL_FUNCTION_REGISTER|=(1<<PSR2);
		switch(IR_F_DIV){
			case 1: // clk T0S /(No prescaling)
			ir_prescaler|=(1<<CS20);
			break;
			case 8: // clk T0S /8 (From prescaler)
			ir_prescaler|=(1<<CS21);
			break;
			case 64: // clk T0S /64 (From prescaler)
			ir_prescaler|=(3<<CS20);
			break;
			case 256: // clk T 0 S /256 (From prescaler)
			ir_prescaler|=(1<<CS22);
			break;
			case 1024: // clk T 0 S /1024 (From prescaler)
			ir_prescaler|=(5<<CS20);
			break;
			default:
			ir_prescaler|=(5<<CS20);
			break;
		}
		TIMER_COUNTER2_COMPARE_REGISTER=IR_CTC_VALUE;
		// CTC
		TIMER_COUNTER2_CONTROL_REGISTER|=((1<<COM20) | (1<<WGM21));
		TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE2);
		ir.key=IR_KEY;
		ir.start=IR_COUNTER_start;
		ir.stop=IR_COUNTER_stop;
	#endif
	return ir;
}
/***TYPE 4***/
#if defined( ATMEGA_TIMER_COUNTER_85XX )
	void IR_INT0_start(void)
	{
		General_Interrupt_Control_Register|=(1<<INT0);
		MCU_Control_Register|=(1<<ISC01);
	}
	void IR_INT0_stop(void)
	{
		MCU_Control_Register&=~(1<<ISC01);
		General_Interrupt_Control_Register&=~(1<<INT0);
	}
	// return full code
	volatile uint8_t* IR_KEY(void)
	{
			return IRbyte;
	}
	void IR_COUNTER_start(void)
	/*
		PARAMETER SETTING
		Frequency oscilator devision factor or prescaler.
		prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /32 (From prescaler);
		clk T0S /64 (From prescaler); clk T0S /128 (From prescaler); clk T 0 S /256 (From prescaler);
		clk T 0 S /1024 (From prescaler); default - clk T 0 S /1024 (From prescaler).
	*/
	{
		if(ir_state==0){ // oneshot
			TIMER_COUNTER2_CONTROL_REGISTER|=ir_prescaler;
			ir_state=1;
		}	
	}
	void IR_COUNTER_stop(void)
	/*
		stops timer by setting prescaler to zero
	*/
	{
		if(ir_state==1){ // oneshot
			TIMER_COUNTER2_CONTROL_REGISTER&=~(7<<CS20); // No clock source. (Timer/Counter stopped)
			TIMER_COUNTER2_REGISTER=0X00;
			ir_state=0;
		}
	}
#endif
/*
** interrupt
*/
ISR(TIMER_COUNTER2_COMPARE_MATCH_INTERRUPT)
{
	uint8_t entry;
	entry=PIND;
	if (entry & (1<<IR_PIN))
		IRbyte[IR_N_BYTE] &= ~(1<<IR_N_BIT);
	else
		IRbyte[IR_N_BYTE] |= (1<<IR_N_BIT);
	/******/
	if(IR_N_BIT<IR_BIT)
		IR_N_BIT++;
	else{
		IR_N_BIT=0;
		if(IR_N_BYTE<IR_BYTE)
			IR_N_BYTE++;
		else{
			IR_N_BYTE=0;
			IR_COUNTER_stop();
			IR_INT0_start();
		}
	}
}
ISR(INT0_vect)
{
	//uint8_t i;
	//for(i=0;i<(IR_BYTE+1);IRbyte[i]=0,i++);
	IR_COUNTER_start();
	IR_INT0_stop();
	IRbyte[IR_N_BYTE] |= (1<<IR_N_BIT);
	IR_N_BIT++;
}
/***EOF***/
/***COMMENTS
interrupt to be defined in MAIN file
Note if you configure to use interrupts and do not define them, program will block,
so be carefull when enabling, make sure correct parameters for specified aplication are applied.
***/
