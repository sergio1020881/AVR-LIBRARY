/************************************************************************
Title: Interrupt ANALOG INPUT
Author: Sergio Manuel Santos
	<sergio.salazar.santos@gmail.com>
File: $Id: analog.h,v 0.2 2015/04/11 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
License: GNU General Public License
COMMENT:
	Very Stable
************************************************************************/
#ifndef ANALOG_H
	#define ANALOG_H
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
#define ADC_NUMBER_SAMPLE 2 // ADC_NUMBER_SAMPLE^2 gives number of samples, note values can only range from 0 to 4.
/*
** variable
*/
struct ANALOG{
	uint8_t VREFF;
	uint8_t DIVISION_FACTOR;
	int (*read)(int selection);
};
typedef struct ANALOG ANALOG;
/*
** procedure and function header
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... );
#endif
/***EOF***/
