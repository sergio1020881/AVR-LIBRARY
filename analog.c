/*************************************************************************
Title:    Interrupt ANALOG INPUT
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: analog.c,v 0.2 2014/04/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
	reads selected analog channels
USAGE:
    Refere to the header file analog.h for a description of the routines. 
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
/*
** library
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <inttypes.h>
/***/
#include "analog.h"
/*
** constant and macro
*/
// if using differential channels this value has to be greater than one
#define ADC_NUMBER_INTERRUPT 2
#define MAX_CHANNELS 8
/***TYPE 1***/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)	
	/******/
	#define ADC_SELECT ADMUX
	#define ADC_CONTROL ADCSRA
	#define MUX_MASK 31
	#define GLOBAL_INTERRUPT_ENABLE 7
	#define ATMEGA_ANALOG
	#define ANALOG_INTERRUPT ADC_vect
/***TYPE 2***/
#elif defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
      defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
      defined(__AVR_ATmega328P__) 
	/******/
	#define ADC_SELECT ADMUX
	#define ADC_CONTROL ADCSRA
	#define ADC_TRIGGER ADCSRB
	#define DIGITAL_INPUT_DISABLE_REGISTER DIDR0
	#define MUX_MASK 15
	#define GLOBAL_INTERRUPT_ENABLE 7
	#define MEGA_ANALOG
	#define ANALOG_INTERRUPT ADC_vect
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
static volatile int ADC_VALUE[MAX_CHANNELS];
static volatile int ADC_CHANNEL_GAIN[MAX_CHANNELS];
static volatile int ADC_N_CHANNELS;
static volatile int ADC_SELECTOR;
static volatile int adc_sample;
static volatile int adc_tmp;
static volatile unsigned char adc_n_sample;
/*
** procedure and function header
*/
int ANALOG_read(int selection);
/*
** procedure and function
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channels, ... )
/*
* Interrupt running mode setup
* setup, and list of channels to be probed
*/
{
	/***LOCAL VARIABLES***/
	uint8_t tSREG;
	va_list list;
	int i;
	//inic variables
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	/***GLOBAL VARIABLES INICIALIZE***/
	ADC_N_CHANNELS=n_channels;
	ADC_SELECTOR=0;
	adc_n_sample=0;
	//PROTOTIPOS
	int ANALOG_read(int channel);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	ANALOG analog;
	//import parametros
	//inic parameters
	//Direccionar apontadores para PROTOTIPOS
	analog.read=ANALOG_read;
	/***Pre-Processor Case 1***/
	#if defined( ATMEGA_ANALOG )
		/******/
		switch( Vreff ){
			case 0:
				ADC_SELECT&=~(3<<REFS0);
				analog.VREFF=0;
				break;
			case 1:
				ADC_SELECT|=(1<<REFS0);
				ADC_SELECT&=~(1<<REFS1);
				analog.VREFF=1;
				break;
			case 3:
				ADC_SELECT|=(3<<REFS0);
				analog.VREFF=3;
				break;
			default:
				ADC_SELECT&=~(3<<REFS0);
				analog.VREFF=0;
				break;
		}
		//
		ADC_SELECT&=~(1<<ADLAR);
		/******/
		va_start(list, n_channels);
		for(i=0;i<n_channels;i++){
			ADC_CHANNEL_GAIN[i] = va_arg(list, int);
		}
		va_end(list);
		ADC_SELECT&=~MUX_MASK;
		ADC_SELECT|=(MUX_MASK & ADC_CHANNEL_GAIN[ADC_SELECTOR]);
		/******/
		ADC_CONTROL|=(1<<ADEN);
		ADC_CONTROL|=(1<<ADSC);
		ADC_CONTROL&=~(1<<ADFR);
		ADC_CONTROL|=(1<<ADIE);
		/******/
		switch( Divfactor ){
			case 2://1
				ADC_CONTROL|=(1<<ADPS0);
				ADC_CONTROL&=~(3<<ADPS1);
				analog.DIVISION_FACTOR=2;
				break;
			case 4://2
				ADC_CONTROL|=(2<<ADPS0);
				ADC_CONTROL&=~(1<<ADPS2);
				analog.DIVISION_FACTOR=4;
				break;
			case 8://3
				ADC_CONTROL|=(3<<ADPS0);
				ADC_CONTROL&=~(1<<ADPS2);
				analog.DIVISION_FACTOR=8;
				break;
			case 16://4
				ADC_CONTROL|=(4<<ADPS0);
				analog.DIVISION_FACTOR=16;
				break;
			case 32://5
				ADC_CONTROL|=(5<<ADPS0);
				analog.DIVISION_FACTOR=32;
				break;
			case 64://6
				ADC_CONTROL|=(6<<ADPS0);
				analog.DIVISION_FACTOR=64;
				break;
			case 128://7
				ADC_CONTROL|=(7<<ADPS0);
				analog.DIVISION_FACTOR=128;
				break;
			default:
				ADC_CONTROL|=(7<<ADPS0);
				analog.DIVISION_FACTOR=128;
				break;
		}		
	/***Pre-Processor Case 2***/	
	#elif defined( MEGA_ANALOG )
		/******/
		switch( Vreff ){
			case 0:
				ADC_SELECT&=~(3<<REFS0);
				analog.VREFF=0;
				break;
			case 1:
				ADC_SELECT|=(1<<REFS0);
				ADC_SELECT&=~(1<<REFS1);
				analog.VREFF=1;
				break;
			case 3:
				ADC_SELECT|=(3<<REFS0);
				analog.VREFF=3;
				break;
			default:
				ADC_SELECT&=~(3<<REFS0);
				analog.VREFF=0;
				break;
		}
		//
		ADC_SELECT&=~(1<<ADLAR);
		/******/
		va_start(list, n_channels);
		for(i=0;i<n_channels;i++){
			ADC_CHANNEL_GAIN[i] = va_arg(list, uint8_t);
		}
		va_end(list);
		ADC_SELECT&=~MUX_MASK;
		ADC_SELECT|=(MUX_MASK & ADC_CHANNEL_GAIN[ADC_SELECTOR]);
		/******/
		ADC_CONTROL|=(1<<ADEN);
		ADC_CONTROL|=(1<<ADSC);
		ADC_CONTROL&=~(1<<ADATE);
		ADC_TRIGGER&=~(7<<ADTS0);
		ADC_CONTROL|=(1<<ADIE);
		/******/
		switch( Divfactor ){
			case 2://1
				ADC_CONTROL&=~(7<<ADPS0);
				analog.DIVISION_FACTOR=2;
				break;
			case 4://2
				ADC_CONTROL|=(2<<ADPS0);
				ADC_CONTROL&=~(1<<ADPS2);
				analog.DIVISION_FACTOR=4;
				break;
			case 8://3
				ADC_CONTROL|=(3<<ADPS0);
				ADC_CONTROL&=~(1<<ADPS2);
				analog.DIVISION_FACTOR=8;
				break;
			case 16://4
				ADC_CONTROL|=(4<<ADPS0);
				analog.DIVISION_FACTOR=16;
				break;
			case 32://5
				ADC_CONTROL|=(5<<ADPS0);
				analog.DIVISION_FACTOR=32;
				break;
			case 64://6
				ADC_CONTROL|=(6<<ADPS0);
				analog.DIVISION_FACTOR=64;
				break;
			case 128://7
				ADC_CONTROL|=(7<<ADPS0);
				analog.DIVISION_FACTOR=128;
				break;
			default:
				ADC_CONTROL|=(7<<ADPS0);
				analog.DIVISION_FACTOR=128;
				break;
		}
	#endif
	SREG=tSREG;
	SREG|=(1<<GLOBAL_INTERRUPT_ENABLE);
	/******/
	return analog;
}
int ANALOG_read(int selection)
/*
* 
* Returns selected Channel ADC_VALUE
*/
{
	uint8_t ADSC_FLAG;
	ADSC_FLAG=(1<<ADSC);
	if( !(ADC_CONTROL & ADSC_FLAG) ){
		//ADC_SELECT
		/***/
		ADC_CONTROL|=(1<<ADSC);
	}	
	return ADC_VALUE[selection];
}
/*
** interrupt
*/
ISR(ANALOG_INTERRUPT)
/*************************************************************************
Function: ANALOG interrupt
Purpose:  Read Analog Input
**************************************************************************/
{
	/******/
	adc_tmp=ADCL;
	adc_tmp|=(ADCH<<8);
	if(adc_n_sample < (1<<ADC_NUMBER_SAMPLE)){
		adc_n_sample++;
		adc_sample+=adc_tmp;
	}else{
		ADC_VALUE[ADC_SELECTOR]=adc_sample>>ADC_NUMBER_SAMPLE;
		adc_n_sample=adc_sample=0;
		/******/
		if(ADC_SELECTOR < ADC_N_CHANNELS)
			ADC_SELECTOR++;
		else
			ADC_SELECTOR=0;
		ADC_SELECT &= ~MUX_MASK;
		ADC_SELECT |= (ADC_CHANNEL_GAIN[ADC_SELECTOR] & MUX_MASK);
	}		
}
/***EOF***/
