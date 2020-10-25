/************************************************************************
	ANALOG
Author: Sergio Manuel Santos
	<sergio.salazar.santos@gmail.com>
License: GNU General Public License
Hardware: Listed in analog.c file
Date: 25102020
Comment:
	Very Stable
************************************************************************/
#ifndef ANALOG_H
	#define ANALOG_H
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/***Constant & Macro***/
#define ADC_NUMBER_SAMPLE 2 // ADC_NUMBER_SAMPLE^2 gives number of samples, note values can only range from 0 to 4.
/***Global Variable***/
struct ANALOG{
	uint8_t VREFF;
	uint8_t DIVISION_FACTOR;
	int (*read)(int selection);
};
typedef struct ANALOG ANALOG;
/***Header***/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... );
#endif
/***EOF***/
