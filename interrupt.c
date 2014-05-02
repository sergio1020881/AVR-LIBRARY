/*************************************************************************
Title:    Interrupt
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: interrupt.c,v 0.2 2014/04/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
USAGE:
    Refere to the header file interrupt.h for a description of the routines. 
NOTES:
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
*************************************************************************/
/*
** library
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <inttypes.h>
/***/
#include "interrupt.h"
/*
** constant and macro
*/
/***TYPE 1***/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)	
	/******/
	#define ATMEGA_INTERRUPT
	#define External_Interrupt_Control_Register_A EICRA
	#define External_Interrupt_Control_Register_B EICRB
	#define External_Interrupt_Mask_Register EIMSK
	#define External_Interrupt_Flag_Register EIFR
	#define MCU_Control_Status_Register MCUCSR
	#define MCU_Control_Status_Register_Mask 0X1F
/***TYPE 2***/
#elif defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
      defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
      defined(__AVR_ATmega328P__) 
	/******/
	#define MEGA_INTERRUPT
	#define External_Interrupt_Control_Register_A EICRA
	#define External_Interrupt_Control_Register_B EICRB
	#define External_Interrupt_Mask_Register EIMSK
	#define External_Interrupt_Flag_Register EIFR
	#define MCU_Control_Status_Register MCUCSR
	#define MCU_Control_Status_Register_Mask 0X1F
/***TYPE 3***/
#elif defined(__AVR_ATmega161__)
	/* ATmega with UART */
 	#error "AVR ATmega161 currently not supported by this libaray !"
#else
	/***TYPE 4***/
 	#error "no ANALOG definition for MCU available"
#endif
/*
** variable
*/
/*
** procedure and function header
*/
void INTERRUPT_set(uint8_t channel, uint8_t sense);
void INTERRUPT_off(uint8_t channel);
uint8_t INTERRUPT_reset_status(void);
/*
** procedure and function
*/
INTERRUPT INTERRUPTenable(void)
/*
* setup blank
*/
{
	INTERRUPT interrupt;
	/***Pre-Processor Case 1***/
	#if defined( ATMEGA_INTERRUPT )
	External_Interrupt_Mask_Register=0X00;
	/***Pre-Processor Case 2***/	
	#elif defined( MEGA_INTERRUPT )
	External_Interrupt_Mask_Register=0X00;
	#endif
	/******/
	interrupt.set=INTERRUPT_set;
	interrupt.off=INTERRUPT_off;
	interrupt.reset_status=INTERRUPT_reset_status;
	return interrupt;
}
uint8_t INTERRUPT_reset_status(void)
{
	uint8_t reset,ret=0;
	reset=(MCU_Control_Status_Register & MCU_Control_Status_Register_Mask);
	switch(reset){
		case 1: // Power-On Reset Flag
			ret=0;
			MCU_Control_Status_Register&=~(1<<PORF);
			break;
		case 2: // External Reset Flag
			MCU_Control_Status_Register&=~(1<<EXTRF);
			ret=1;
			break;
		case 4: // Brown-out Reset Flag
			MCU_Control_Status_Register&=~(1<<BORF);
			ret=2;
			break;
		case 8: // Watchdog Reset Flag
			MCU_Control_Status_Register&=~(1<<WDRF);
			ret=3;
			break;
		case 16: // JTAG Reset Flag
			MCU_Control_Status_Register&=~(1<<JTRF);
			ret=4;
			break;
		default: // clear all status
			MCU_Control_Status_Register&=~(MCU_Control_Status_Register_Mask);
			break;
	}
	return ret;
}
void INTERRUPT_set(uint8_t channel, uint8_t sense)
{
	switch( channel ){
		case 0: 
			External_Interrupt_Mask_Register&=~(1<<INT0);
			External_Interrupt_Control_Register_A&=~((1<<ISC01) | (1<<ISC00));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=(1<<ISC01);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=((1<<ISC01) | (1<<ISC00));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT0);
			break;
		case 1:
			External_Interrupt_Mask_Register&=~(1<<INT1);
			External_Interrupt_Control_Register_A&=~((1<<ISC11) | (1<<ISC10));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=(1<<ISC11);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=((1<<ISC11) | (1<<ISC10));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT1);
			break;
		case 2:
			External_Interrupt_Mask_Register&=~(1<<INT2);
			External_Interrupt_Control_Register_A&=~((1<<ISC21) | (1<<ISC20));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=(1<<ISC21);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=((1<<ISC21) | (1<<ISC20));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT2);
			break;
		case 3:
			External_Interrupt_Mask_Register&=~(1<<INT3);
			External_Interrupt_Control_Register_A&=~((1<<ISC31) | (1<<ISC30));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
				case 1: // The low level of INTn generates an interrupt request.
					break;
				case 2: // The falling edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=(1<<ISC31);
					break;
				case 3: // The rising edge of INTn generates asynchronously an interrupt request.
					External_Interrupt_Control_Register_A|=((1<<ISC31) | (1<<ISC30));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT3);
			break;
		case 4:
			External_Interrupt_Mask_Register&=~(1<<INT4);
			External_Interrupt_Control_Register_B&=~((1<<ISC41) | (1<<ISC40));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B|=(1<<ISC40);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=(1<<ISC41);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=((1<<ISC41) | (1<<ISC40));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT4);
			break;
		case 5:
			External_Interrupt_Mask_Register&=~(1<<INT5);
			External_Interrupt_Control_Register_B&=~((1<<ISC51) | (1<<ISC50));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B|=(1<<ISC50);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=(1<<ISC51);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=((1<<ISC51) | (1<<ISC50));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT5);
			break;
		case 6:
			External_Interrupt_Mask_Register&=~(1<<INT6);
			External_Interrupt_Control_Register_B&=~((1<<ISC61) | (1<<ISC60));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B|=(1<<ISC60);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=(1<<ISC61);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=((1<<ISC61) | (1<<ISC60));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT6);
			break;
		case 7:
			External_Interrupt_Mask_Register&=~(1<<INT7);
			External_Interrupt_Control_Register_B&=~((1<<ISC71) | (1<<ISC70));
			switch(sense){
				case 0: // The low level of INTn generates an interrupt request.
					break;
				case 1: // Any logical change on INTn generates an interrupt request
					External_Interrupt_Control_Register_B|=(1<<ISC70);
					break;
				case 2: // The falling edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=(1<<ISC71);
					break;
				case 3: // The rising edge between two samples of INTn generates an interrupt request.
					External_Interrupt_Control_Register_B|=((1<<ISC71) | (1<<ISC70));
					break;
				default: // The low level of INTn generates an interrupt request.
					break;
			}
			External_Interrupt_Mask_Register|=(1<<INT7);
			break;
		default:
			External_Interrupt_Mask_Register=0X00;
			break;
		}
}
void INTERRUPT_off(uint8_t channel)
{
	switch( channel ){
		case 0: // desable
			External_Interrupt_Mask_Register&=~(1<<INT0);
			break;
		case 1: // desable
			External_Interrupt_Mask_Register&=~(1<<INT1);
			break;
		case 2: // desable
			External_Interrupt_Mask_Register&=~(1<<INT2);
			break;
		case 3: // desable
			External_Interrupt_Mask_Register&=~(1<<INT3);
			break;
		case 4: // desable
			External_Interrupt_Mask_Register&=~(1<<INT4);
			break;
		case 5: // desable
			External_Interrupt_Mask_Register&=~(1<<INT5);
			break;
		case 6: // desable
			External_Interrupt_Mask_Register&=~(1<<INT6);
			break;
		case 7: // desable
			External_Interrupt_Mask_Register&=~(1<<INT7);
			break;
		default: // all desable
			External_Interrupt_Mask_Register=0X00;
			break;
		}
}
/*
** interrupt
*/
/***EOF***/
