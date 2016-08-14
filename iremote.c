/*
 * IREMOTE.c
 *
 * Atmega 8535 324
 * this will consist of a interrupt high to low detect at the IR sensor output
 * that will start a timer at determined intervals reading the states composing the receiving code emitted.
 *
 * Created: 10/07/2016 01:05:53
 * Author: Sérgio Santos
 * Observations:
 * GOOD
 * Usando um commando da tv cabo, na opção TV cabo commando, funciona muito bem usar dois ultimos bytes para codigo,
 * os dois primeiros são constantes. Talvez vou alterar de forma a ser mais generico.
 */ 
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "iremote.h"
#if defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
/*
** constant and macro
*/
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
#define TIMER_COUNTER_ASYNCHRONOUS_STATUS_REGISTER ASSR
#define TIMER_COUNTER_INTERRUPT_MASK_REGISTER TIMSK
#define TIMER_COUNTER_INTERRUPT_FLAG_REGISTER TIFR
#define TIMER_COUNTER_SPECIAL_IO_FUNCTION_REGISTER SFIOR
#elif defined(__AVR_ATmega324A__)
/*
** constant and macro
*/
#define External_Interrupt_Mask_Register EIMSK
#define External_Interrupt_Control_RegisterA EICRA
#define External_Interrupt_Flag_Register EIFR
#define External_Interrupt0 INT0_vect
/***2***/
#define TIMER_COUNTER2_INTERRUPT_FLAG_REGISTER TIFR2
#define TIMER_COUNTER2_INTERRUPT_MASK_REGISTER TIMSK2
#define TIMER_COUNTER2A_CONTROL_REGISTER TCCR2A
#define TIMER_COUNTER2B_CONTROL_REGISTER TCCR2B
#define TIMER_COUNTER2_REGISTER TCNT2
#define TIMER_COUNTER2A_COMPARE_REGISTER OCR2A
#define TIMER_COUNTER2B_COMPARE_REGISTER OCR2B
//Interrupt sources
#define TIMER_COUNTER2A_COMPARE_MATCH_INTERRUPT TIMER2_COMPA_vect
#define TIMER_COUNTER2B_COMPARE_MATCH_INTERRUPT TIMER2_COMPB_vect
#define TIMER_COUNTER2_OVERFLOW_INTERRUPT TIMER2_OVF_vect
/***COMMON***/
#endif
/*
** variable
*/
uint8_t ir_prescaler;
volatile uint8_t ir_state;
volatile uint8_t IR_N_BYTE;
volatile uint8_t IR_N_BIT;
volatile uint8_t IRbyte[IR_BYTE+1];
volatile uint8_t ir_prevalue;
/*
** procedure and function header
*/
volatile uint8_t IR_KEY(uint8_t byte);
void IR_COUNTER_start(void);
void IR_COUNTER_stop(void);
void IR_INT0_start(void);
void IR_INT0_stop(void);
volatile uint8_t IR_decode(void);
void IR_clear(void);
/*
**
*/
#if defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
/*
** procedure and function
*/
IR IRenable()
{
	IR ir;
	ir_state=0;
	IR_N_BYTE=0;
	IR_N_BIT=0;
	ir_prevalue=0;
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
	ir.clear=IR_clear;
	return ir;
}
/************************
*************************
************************/
volatile uint8_t IR_decode(void)
{
	volatile uint8_t value;
	volatile uint8_t code[2];
	if(ir_state){ // FILTER
		value=ir_prevalue;
		code[0]=0;
		code[1]=0;
		}else{
		code[0]=IRbyte[2];
		code[1]=IRbyte[3];
	}
	//DECODE
	if((code[0]==102) && (code[1]==1))
	value='S';
	if((code[0]==102) && (code[1]==2))
	value='P';
	if((code[0]==166) && (code[1]==1))
	value='I';
	if((code[0]==166) && (code[1]==2))
	value='p';
	if((code[0]==105) && (code[1]==2))
	value='U';
	if((code[0]==86) && (code[1]==2))
	value='D';
	if((code[0]==169) && (code[1]==1))
	value='L';
	if((code[0]==86) && (code[1]==1))
	value='R';
	if((code[0]==169) && (code[1]==2))
	value='O';
	if((code[0]==154) && (code[1]==2))
	value='C';
	if((code[0]==106) && (code[1]==1))
	value='c';
	if((code[0]==106) && (code[1]==2))
	value='V';
	if((code[0]==170) && (code[1]==1))
	value='v';
	if((code[0]==85) && (code[1]==2))
	value='o';
	if((code[0]==150) && (code[1]==2))
	value='E';
	if((code[0]==149) && (code[1]==1))
	value='s';
	if((code[0]==154) && (code[1]==1))
	value='T';
	if((code[0]==149) && (code[1]==2))
	value='1';
	if((code[0]==101) && (code[1]==1))
	value='2';
	if((code[0]==101) && (code[1]==2))
	value='3';
	if((code[0]==165) && (code[1]==1))
	value='4';
	if((code[0]==165) && (code[1]==2))
	value='5';
	if((code[0]==89) && (code[1]==1))
	value='6';
	if((code[0]==89) && (code[1]==2))
	value='7';
	if((code[0]==153) && (code[1]==1))
	value='8';
	if((code[0]==153) && (code[1]==2))
	value='9';
	if((code[0]==105) && (code[1]==1))
	value='0';
	if((code[0]==90) && (code[1]==2))
	value='*';
	if((code[0]==90) && (code[1]==1))
	value='#';
	if((code[0]==150) && (code[1]==1))
	value='r';
	
	ir_prevalue=value;
	return value;
}
void IR_INT0_start(void)
{
	General_Interrupt_Flag_Register|=(1<<INTF0);
	General_Interrupt_Control_Register|=(1<<INT0);
}
void IR_INT0_stop(void)
{
	General_Interrupt_Control_Register&=~(1<<INT0);
}
// return full code
volatile uint8_t IR_KEY(uint8_t byte)
{
	if(ir_state)
		return 0;
	else
		return IRbyte[byte];
}
void IR_COUNTER_start(void)
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
		// No clock source. (Timer/Counter stopped)
		TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<CS22) | (1<<CS21) | (1<<CS20));
		ir_state=0;
	}
}
void IR_clear(void){
	uint8_t index;
	for(index=0;index<(IR_BYTE+1);index++)
		IRbyte[index]=0;
	ir_prevalue=0;
}
/*
** interrupt
*/
ISR(TIMER_COUNTER2_COMPARE_MATCH_INTERRUPT)
{
	volatile uint8_t entry;
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
	IR_INT0_stop();
	IR_COUNTER_start();
	IRbyte[IR_N_BYTE] |= (1<<IR_N_BIT);
	IR_N_BIT++;
}
/**********************************************************************************************
***********************************************************************************************
***********************************************************************************************/
#elif defined(__AVR_ATmega324A__)
/*
** procedure and function
*/
IR IRenable()
{
	IR ir;
	ir_state=0;
	IR_N_BYTE=0;
	IR_N_BIT=0;
	ir_prevalue=0;
	//INTERRUPT
	External_Interrupt_Mask_Register|=(1<<INT0);
	External_Interrupt_Control_RegisterA|=(1<<ISC01);
	// TIMER
	//TIMER_COUNTER_SPECIAL_FUNCTION_REGISTER|=(1<<PSR2);
	TIMER_COUNTER2_REGISTER=0X00;
	ir_prescaler=0;
	switch(IR_F_DIV){
		case 1: // clkI/O/(No prescaling)
			ir_prescaler=(1<<CS20);
			break;
		case 8: // clkI/O/8 (From prescaler)
			ir_prescaler=(1<<CS21);
			break;
		case 32: // clkI/O/32 (From prescaler)
			ir_prescaler=((1<<CS21) | (1<<CS20));
			break;
		case 64: // clkI/O/64 (From prescaler)
			ir_prescaler=(1<<CS22);
			break;
		case 128: // clkI/O/128 (From prescaler)
			ir_prescaler=((1<<CS22) | (1<<CS20));
			break;
		case 256: // clkI/O/256 (From prescaler)
			ir_prescaler=((1<<CS22) | (1<<CS21));
			break;
		case 1024: // clkI/O/1024 (From prescaler)
			ir_prescaler=((1<<CS22) | (1<<CS21) | (1<<CS20));
			break;
		default:
			ir_prescaler=((1<<CS22) | (1<<CS21) | (1<<CS20));
			break;
	}
	TIMER_COUNTER2A_COMPARE_REGISTER=IR_CTC_VALUE;
	// CTC
	TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<WGM21);
	TIMER_COUNTER2_INTERRUPT_MASK_REGISTER|=(1<<OCIE2A);
	TIMER_COUNTER2A_CONTROL_REGISTER|=(1<<COM2A0);
	ir.key=IR_KEY;
	ir.start=IR_COUNTER_start;
	ir.stop=IR_COUNTER_stop;
	ir.decode=IR_decode;
	ir.clear=IR_clear;
	return ir;
}
/************************
*************************
************************/
volatile uint8_t IR_decode(void)
{
	volatile uint8_t value;
	volatile uint8_t code[2];
	if(ir_state){ // FILTER
		value=ir_prevalue;
		code[0]=0;
		code[1]=0;
		}else{
		code[0]=IRbyte[2];
		code[1]=IRbyte[3];
	}
	//DECODE
	if((code[0]==102) && (code[1]==1))
	value='S';
	if((code[0]==102) && (code[1]==2))
	value='P';
	if((code[0]==166) && (code[1]==1))
	value='I';
	if((code[0]==166) && (code[1]==2))
	value='p';
	if((code[0]==105) && (code[1]==2))
	value='U';
	if((code[0]==86) && (code[1]==2))
	value='D';
	if((code[0]==169) && (code[1]==1))
	value='L';
	if((code[0]==86) && (code[1]==1))
	value='R';
	if((code[0]==169) && (code[1]==2))
	value='O';
	if((code[0]==154) && (code[1]==2))
	value='C';
	if((code[0]==106) && (code[1]==1))
	value='c';
	if((code[0]==106) && (code[1]==2))
	value='V';
	if((code[0]==170) && (code[1]==1))
	value='v';
	if((code[0]==85) && (code[1]==2))
	value='o';
	if((code[0]==150) && (code[1]==2))
	value='E';
	if((code[0]==149) && (code[1]==1))
	value='s';
	if((code[0]==154) && (code[1]==1))
	value='T';
	if((code[0]==149) && (code[1]==2))
	value='1';
	if((code[0]==101) && (code[1]==1))
	value='2';
	if((code[0]==101) && (code[1]==2))
	value='3';
	if((code[0]==165) && (code[1]==1))
	value='4';
	if((code[0]==165) && (code[1]==2))
	value='5';
	if((code[0]==89) && (code[1]==1))
	value='6';
	if((code[0]==89) && (code[1]==2))
	value='7';
	if((code[0]==153) && (code[1]==1))
	value='8';
	if((code[0]==153) && (code[1]==2))
	value='9';
	if((code[0]==105) && (code[1]==1))
	value='0';
	if((code[0]==90) && (code[1]==2))
	value='*';
	if((code[0]==90) && (code[1]==1))
	value='#';
	if((code[0]==150) && (code[1]==1))
	value='r';
	
	ir_prevalue=value;
	return value;
}
void IR_INT0_start(void)
{
	External_Interrupt_Flag_Register|=(1<<INTF0);
	External_Interrupt_Mask_Register|=(1<<INT0);
}
void IR_INT0_stop(void)
{
	External_Interrupt_Mask_Register&=~(1<<INT0);
}
// return full code
volatile uint8_t IR_KEY(uint8_t byte)
{
	if(ir_state)
		return IRbyte[byte];
	else
		return IRbyte[byte];
}
void IR_COUNTER_start(void)
{
	if(ir_state==0){ // oneshot
		TIMER_COUNTER2_REGISTER=0X00;
		TIMER_COUNTER2B_CONTROL_REGISTER|=ir_prescaler;
		ir_state=1;
	}	
}
void IR_COUNTER_stop(void)
{
	if(ir_state==1){ // oneshot
		// No clock source. (Timer/Counter stopped)
		TIMER_COUNTER2B_CONTROL_REGISTER&=~((1<<CS22) | (1<<CS21) | (1<<CS20));
		ir_state=0;
	}
}
void IR_clear(void){
	uint8_t index;
	for(index=0;index<(IR_BYTE+1);index++)
		IRbyte[index]=0;
	ir_prevalue=0;
}
/*
** interrupt
*/
ISR(TIMER_COUNTER2A_COMPARE_MATCH_INTERRUPT)
{
	volatile uint8_t entry;
	entry=IR_INPORT;
	if (entry & (1<<IR_PIN)){
		IRbyte[IR_N_BYTE] &= ~(1<<IR_N_BIT);
	}else
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
ISR(External_Interrupt0)
{
	IR_INT0_stop();
	IR_COUNTER_start();
	IRbyte[IR_N_BYTE] |= (1<<IR_N_BIT);
	IR_N_BIT++;
}
#else
	#error "IREMOTE only supports Atemaga 8535, 8515 and 324A Sorry!!"
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
