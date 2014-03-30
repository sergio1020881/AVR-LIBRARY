/*************************************************************************
Title:    Interrupt ANALOG INPUT
Author:   Sergio Salazar Santos <sergio1020881@gmail.com>
File:     $Id: analog.c,v 0.1 2013/12/30 15:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License 
          
DESCRIPTION:
    
    
USAGE:
    Refere to the header file analog.h for a description of the routines. 

NOTES:
    Based on Atmel Application Note AVR306
                    
LICENSE:
    Copyright (C) 2013

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                        
*************************************************************************/

/*
** Library
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


/*
** Private Library
*/

#include "analog.h"


/*
**  module constants and macros
*/

// if using differential channels this value has to be greater than one
#define ADC_NUMBER_INTERRUPT 2

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
**  module variables
*/



/*
** module object 1 constructor
*/

struct ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor )
/*
* Interrupt running mode setup
*/
{
	/***LOCAL VARIABLES***/
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	
	/***GLOBAL VARIABLES INICIALIZE***/
	ADC_CHANNEL=0;
	ADC_FLAG=0;
	ADC_INC=0;
	
		
	
	//PROTOTIPOS
	int ANALOG_read(struct ANALOG* analog, uint8_t Channel);
	unsigned int ANALOG_ticks(unsigned int num);

	//ALLOCAÇÂO MEMORIA PARA Estrutura
	struct ANALOG analog;
	//import parametros
	//inic parameters
	//inic variables


	//Direccionar apontadores para PROTOTIPOS
	analog.read=ANALOG_read;
	analog.ticks=ANALOG_ticks;


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
		ADC_SELECT&=~(MUX_MASK<<MUX0);
		/*
		switch( Channel ){
			case 0:
				ADC_SELECT&=~(MUX_MASK<<MUX0);
				analog.CHANNEL_SELECT=0;
				break;
			//still many more options


			default:
				ADC_SELECT&=~(MUX_MASK<<MUX0);
				analog.CHANNEL_SELECT=0;
				break;
		}
		*/
		/*
		* maybe should read all channels in circulor buffer
		* so a function "read" indicating ADC input is more logical
		* or just one analog select allowed 
		*/
		
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
		ADC_SELECT&=~(MUX_MASK<<MUX0);
		/*
		switch( Channel ){
			case 0:
				ADC_SELECT&=~(MUX_MASK<<MUX0);
				analog.CHANNEL_SELECT=0;
				break;
			//still many more options


			default:
				ADC_SELECT&=~(MUX_MASK<<MUX0);
				analog.CHANNEL_SELECT=0;
				break;
		}
		*/
		/*
		* maybe should read all channels in circulor buffer
		* so a function "read" indicating ADC input is more logical
		* or just one analog select allowed 
		*/
		
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


/*
** module object 1 procedure and function definitions
*/

int ANALOG_read(struct ANALOG* analog, uint8_t Channel)
/*
* Reads all analog inputs one after the other
* Returns selected Channel ADC_VALUE
*/
{
	uint8_t ADSC_FLAG;
	ADSC_FLAG=(1<<ADSC);
	
	if( !(ADC_CONTROL & ADSC_FLAG) ){
		analog->ADC_DATA[ADC_CHANNEL]=ADC_VALUE;
		//
		if( ADC_FLAG > ADC_NUMBER_INTERRUPT ){
			ADC_INC=ADC_CHANNEL+1;
			ADC_INC&=MUX_MASK;
			ADC_SELECT&=~MUX_MASK;
			ADC_SELECT|=ADC_INC;
			ADC_FLAG=0;
		}		
		/******/
		ADC_CONTROL|=(1<<ADSC);
	}
		
	return analog->ADC_DATA[Channel];
}


unsigned int ANALOG_ticks(unsigned int num)
{
	unsigned int count;
	for(count=0;count<num;count++)
		;
	return count;
}


/*
** module object 1 interrupts
*/

ISR(ANALOG_INTERRUPT)
/*************************************************************************
Function: ANALOG interrupt
Purpose:  Read Analog Input
**************************************************************************/
{
	/******/
	ADC_CHANNEL=(ADC_SELECT & MUX_MASK);
	ADC_FLAG++;
	/******/
	//ADC_INC=ADC_CHANNEL+1;
	//ADC_INC&=MUX_MASK;
	//ADC_SELECT&=~MUX_MASK;
	//ADC_SELECT|=ADC_INC;
	/******/
	ADC_VALUE=ADCL;
	ADC_VALUE|=(ADCH<<8);
	/******/
}


/*
** module procedure and function definitions
*/


/*
** module interrupts
*/

/***EOF***/
