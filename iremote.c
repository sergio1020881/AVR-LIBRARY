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
 * GOOD
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
/*
** variable
*/
uint8_t ir_prescaler;
uint8_t ir_state;
volatile uint8_t IR_N_BYTE;
volatile uint8_t IR_N_BIT;
volatile uint8_t IRbyte[IR_BYTE+1];
unsigned char ir_prevalue;
/*
** procedure and function header
*/
volatile uint8_t* IR_KEY(void);
void IR_COUNTER_start(void);
void IR_COUNTER_stop(void);
void IR_INT0_start(void);
void IR_INT0_stop(void);
unsigned char IR_decode(void);
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
	ir_prevalue='N';
	//INTERRUPT
	General_Interrupt_Control_Register|=(1<<INT0);
	MCU_Control_Register|=(1<<ISC01);
	// TIMER
	//TIMER_COUNTER_SPECIAL_FUNCTION_REGISTER|=(1<<PSR2);
	TIMER_COUNTER2_REGISTER=0X00;
	ir_prescaler=0;
	switch(IR_F_DIV){
		case 1: // clk T0S /(No prescaling)
			ir_prescaler=(1<<CS20);
			break;
		case 8: // clk T0S /8 (From prescaler)
			ir_prescaler=(1<<CS21);
			break;
		case 32: // clkI/O/32 (From prescaler)
			ir_prescaler=((1<<CS21) | (1<<CS20));
			break;
		case 64: // clk T0S /64 (From prescaler)
			ir_prescaler=(1<<CS22);
			break;
		case 128: // clkI/O/128 (From prescaler)
			ir_prescaler=((1<<CS22) | (1<<CS20));
			break;
		case 256: // clk T 0 S /256 (From prescaler)
			ir_prescaler=((1<<CS22) | (1<<CS21));
			break;
		case 1024: // clk T 0 S /1024 (From prescaler)
			ir_prescaler=((1<<CS22) | (1<<CS21) | (1<<CS20));
			break;
		default:
			ir_prescaler=((1<<CS22) | (1<<CS21) | (1<<CS20));
			break;
	}
	TIMER_COUNTER2_COMPARE_REGISTER=IR_CTC_VALUE;
	// CTC
	TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM21);
	TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE2);
	ir.key=IR_KEY;
	ir.start=IR_COUNTER_start;
	ir.stop=IR_COUNTER_stop;
	ir.decode=IR_decode;
	return ir;
}
/************************
*************************
************************/
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
	if(ir_state)
		return 0;
	else
		return IRbyte;
}
void IR_COUNTER_start(void)
/*
	PARAMETER SETTING
	Frequency oscillator devision factor or prescaler.
	prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /32 (From prescaler);
	clk T0S /64 (From prescaler); clk T0S /128 (From prescaler); clk T 0 S /256 (From prescaler);
	clk T 0 S /1024 (From prescaler); default - clk T 0 S /1024 (From prescaler).
*/
{
	if(ir_state==0){ // oneshot
		TIMER_COUNTER2_REGISTER=0X00;
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
		TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<CS22) | (1<<CS21) | (1<<CS20)); // No clock source. (Timer/Counter stopped)
		ir_state=0;
	}
}
unsigned char IR_decode(void)
{
	unsigned char value;
	uint8_t code[2];
	if(ir_state){ // FILTER
		code[0]=0;
		code[1]=0;
		value=ir_prevalue;
	}else{
		code[0]=IRbyte[1];
		code[1]=IRbyte[2];
	}
	//DECODE
	if((code[0]==73) && (code[1]==10)){
		value='S';
	}
	if((code[0]==73) && (code[1]==18)){
		value='P';
	}
	if((code[0]==73) && (code[1]==14)){
		value='I';
	}
	if((code[0]==73) && (code[1]==22)){
		value='p';
	}
	if((code[0]==169) && (code[1]==18)){
		value='U';
	}
	if((code[0]==73) && (code[1]==17)){
		value='D';
	}
	if((code[0]==169) && (code[1]==14)){
		value='L';
	}
	if((code[0]==73) && (code[1]==9)){
		value='R';
	}
	if((code[0]==169) && (code[1]==22)){
		value='O';
	}
	if((code[0]==201) && (code[1]==21)){
		value='C';
	}
	if((code[0]==201) && (code[1]==10)){
		value='c';
	}
	if((code[0]==201) && (code[1]==18)){
		value='V';
	}
	if((code[0]==201) && (code[1]==14)){
		value='v';
	}
	if((code[0]==41) && (code[1]==17)){
		value='o';
	}
	if((code[0]==73) && (code[1]==21)){
		value='E';
	}
	if((code[0]==41) && (code[1]==13)){
		value='s';
	}
	if((code[0]==201) && (code[1]==13)){
		value='T';
	}
	if((code[0]==41) && (code[1]==21)){
		value='1';
	}
	if((code[0]==41) && (code[1]==10)){
		value='2';
	}
	if((code[0]==41) && (code[1]==18)){
		value='3';
	}
	if((code[0]==41) && (code[1]==14)){
		value='4';
	}
	if((code[0]==41) && (code[1]==22)){
		value='5';
	}
	if((code[0]==169) && (code[1]==9)){
		value='6';
	}
	if((code[0]==169) && (code[1]==17)){
		value='7';
	}
	if((code[0]==169) && (code[1]==13)){
		value='8';
	}
	if((code[0]==169) && (code[1]==21)){
		value='9';
	}
	if((code[0]==169) && (code[1]==10)){
		value='0';
	}
	if((code[0]==201) && (code[1]==17)){
		value='*';
	}
	if((code[0]==201) && (code[1]==9)){
		value='#';
	}
	if((code[0]==73) && (code[1]==13)){
		value='r';
	}
	ir_prevalue=value;
	return value;	
}
/*
** interrupt
*/
ISR(TIMER_COUNTER2_COMPARE_MATCH_INTERRUPT)
{
	uint8_t entry;
	entry=IR_INPORT;
	if (entry & (1<<IR_PIN))
		IRbyte[IR_N_BYTE] &= ~(1<<IR_N_BIT);
	else
		IRbyte[IR_N_BYTE] |= (1<<IR_N_BIT);
	
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
	IR_COUNTER_start();
	IR_INT0_stop();
	IRbyte[IR_N_BYTE] |= (1<<IR_N_BIT);
	IR_N_BIT++;
}
#endif
/***EOF***/
/***COMMENTS
interrupt to be defined in MAIN file
Note if you configure to use interrupts and do not define them, program will block,
so be carefully when enabling, make sure correct parameters for specified application are applied.
The mind puts the pieces (thoughts) together and when it glimpses into the whole picture an instant 
spark happens in the mind, to manifest it is then intuitive and extremely easy, 
rebuilding the image in reality.
***/
