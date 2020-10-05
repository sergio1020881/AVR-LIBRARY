/************************************************************************
ATMEGA128 ANALOG API START
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
	28092020
************************************************************************/
/***preamble inic***/
#ifndef _ATMEGA128ANALOG_H_
	#define _ATMEGA128ANALOG_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** Library
*/
#include <inttypes.h>
/************************************************************************
ANALOG API START
************************************************************************/
/*
** constant and macro
*/
#define ADC_NUMBER_SAMPLE 2 // ADC_NUMBER_SAMPLE^2 gives number of samples, note values can only range from 0 to 4.
/*
** variable
*/
struct nlg{
	/***Parameters***/
	uint8_t VREFF;
	uint8_t DIVISION_FACTOR;
	//prototype pointers
	int (*read)(int selection);
};
typedef struct nlg ANALOG;
/*
** procedure and function header
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... );
/*************************************************************************
ANALOG API END
*************************************************************************/
#endif
/***EOF***/
