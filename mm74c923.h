/************************************************************************
MM74C923 API START
Author: Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
************************************************************************/
/***preamble inic***/
#ifndef _MM74C923_H_
	#define _MM74C923_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** Library
*/
#include <inttypes.h>
/***preamble inic***/
/*
** constant and macro
*/
#define MM74C923_DATA_OUT_A 7 //ic pin 19 LSB
#define MM74C923_DATA_OUT_B 6 //ic pin 18
#define MM74C923_DATA_OUT_C 5 //ic pin 17
#define MM74C923_DATA_OUT_D 4 //ic pin 16
#define MM74C923_DATA_OUT_E 3 //ic pin 15
#define MM74C923_EXTRA_DATA_OUT_PIN 2 // MSB
#define MM74C923_OUTPUT_ENABLE 1 //ic pin 14
#define MM74C923_DATA_AVAILABLE 0 //ic pin 13
/*
** variable
*/
struct mm74c923{
	void (*activate)(void);
	char (*getch)(void);
	char* (*gets)(void);
	char* (*data)(void);
	void (*data_clear)(void);
};
typedef struct mm74c923 MM74C923;
/*
** procedure and function header
*/
MM74C923 MM74C923enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
/***preamble inic***/
#endif
/***preamble inic***/
/************************************************************************
MM74C923 API END
************************************************************************/
