/************************************************************************
ATMEGA128 INTERRUPT API START
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
	28092020
************************************************************************/
/***preamble inic***/
#ifndef _ATMEGA128INTERRUPT_H_
	#define _ATMEGA128INTERRUPT_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** Library
*/
#include <inttypes.h>
/************************************************************************
INTERRUPT API START
************************************************************************/
/*
** constant and macro
*/
/*
** variable
*/
struct ntrrpt{
	void (*set)(uint8_t channel, uint8_t sense);
	void (*off)(uint8_t channel);
	uint8_t (*reset_status)(void);
};
typedef struct ntrrpt INTERRUPT;
/*
** procedure and function header
*/
INTERRUPT INTERRUPTenable(void);
/************************************************************************
INTERRUPT API END
************************************************************************/
#endif
/***EOF***/
