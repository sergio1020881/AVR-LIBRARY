/************************************************************************
Title: 74hc595 API
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
File: $Id: 74hc595.h Exp $
License: GNU General Public License
Comment:
	tested Atemga128 16Mhz and Atmega328 8Mhz
	reviewed 09/10/2020
************************************************************************/
#ifndef _74HC595_H_
	#define _74HC595_H_
/*
** Library
*/
#include <inttypes.h>
/*
** constant and macro
*/
/*
** variable
*/
struct hc595{
	/******/
	void (*bit)(uint8_t bool);
	void (*byte)(uint8_t byte);
	void (*out)(void);
};
typedef struct hc595 HC595;
/*
** procedure and function header
*/
HC595 HC595enable(volatile uint8_t *ddr, volatile uint8_t *port, uint8_t datapin, uint8_t clkpin, uint8_t outpin);
#endif
/***COMMENT
HC595 API HEADER END
***/
/***EOF***/
