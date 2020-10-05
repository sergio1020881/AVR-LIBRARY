/*************************************************************************
Title:    Interrupt ANALOG INPUT
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: atmega324analog.c,v 0.2 2018/08/18 13:00:00 Sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 MHz,
License:  GNU General Public License
Comment:
	Very Stable
*************************************************************************/
/*
** library
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include <inttypes.h>
/***/
#include "atmega324analog.h"
/*
** constant and macro
*/
#if defined(__AVR_ATmega164A__) || defined(__AVR_ATmega164PA__) || defined(__AVR_ATmega324A__) ||\
	defined(__AVR_ATmega324PA__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644PA__) ||\
	defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
// if using differential channels this value has to be greater than one
#define MAX_CHANNEL 8
/******/
#define GLOBAL_INTERRUPT_ENABLE 7
#define ADC_SELECT ADMUX
#define ADC_CONTROL ADCSRA
#define ADC_TRIGGER ADCSRB
#define DIGITAL_INPUT_DISABLE_REGISTER DIDR0
#define MUX_MASK 31
#define ANALOG_INTERRUPT ADC_vect
/*
** variable
*/
static volatile int ADC_VALUE[MAX_CHANNEL];
static volatile int ADC_CHANNEL_GAIN[MAX_CHANNEL];
volatile int ADC_N_CHANNEL;
volatile int ADC_SELECTOR;
volatile int adc_sample;
volatile int adc_tmp;
volatile unsigned char adc_n_sample;
/*
** procedure and function header
*/
int ANALOG_read(int selection);
/*
** procedure and function
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... )
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
	ADC_N_CHANNEL=n_channel;
	ADC_SELECTOR=0;
	adc_n_sample=0;
	//PROTOTIPOS
	//int ANALOG_read(int channel);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	ANALOG analog;
	//import parametros
	//inic parameters
	//Direccionar apontadores para PROTOTIPOS
	analog.read=ANALOG_read;
	/******/
	ADC_SELECT&=~(3<<REFS0);
	switch( Vreff ){
		case 0: //AREF, Internal Vref turned off
			analog.VREFF=0;
			break;
		case 1: //AVCC with external capacitor at AREF pin
			ADC_SELECT|=(1<<REFS0);
			analog.VREFF=1;
			break;
		case 2: //Internal 1.1V Voltage Reference with external capacitor at AREF pin
			ADC_SELECT|=(1<<REFS1);
			analog.VREFF=2;
			break;
		case 3: //Internal 2.56V Voltage Reference with external capacitor at AREF pin
			ADC_SELECT|=(3<<REFS0);
			analog.VREFF=3;
			break;
		default:
			analog.VREFF=0;
			break;
	}
	//
	ADC_SELECT&=~(1<<ADLAR);
	/******/
	va_start(list, n_channel);
	for(i=0;i<n_channel;i++){
		ADC_CHANNEL_GAIN[i] = va_arg(list, int);
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
	ADC_CONTROL&=~(7<<ADPS0);
	switch( Divfactor ){
		case 2://1
			analog.DIVISION_FACTOR=2;
			break;
		case 4://2
			ADC_CONTROL|=(1<<ADPS1);
			analog.DIVISION_FACTOR=4;
			break;
		case 8://3
			ADC_CONTROL|=(3<<ADPS0);
			analog.DIVISION_FACTOR=8;
			break;
		case 16://4
			ADC_CONTROL|=(1<<ADPS2);
			analog.DIVISION_FACTOR=16;
			break;
		case 32://5
			ADC_CONTROL|=(5<<ADPS0);
			analog.DIVISION_FACTOR=32;
			break;
		case 64://6
			ADC_CONTROL|=(3<<ADPS1);
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
		if(ADC_SELECTOR < ADC_N_CHANNEL)
			ADC_SELECTOR++;
		else
			ADC_SELECTOR=0;
		ADC_SELECT &= ~MUX_MASK;
		ADC_SELECT |= (ADC_CHANNEL_GAIN[ADC_SELECTOR] & MUX_MASK);
	}
}
#else
	#error "Not ATmega 164A or 164PA or 324A or 324PA or 644A or 644PA or 1284 or 1284P"
#endif
/***EOF***/
/***COMMENTS
Do not think to much about it or you will get a seizure, lol.
***/
