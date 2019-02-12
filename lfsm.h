/************************************************************************
LFSM API START
Author: Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
************************************************************************/
/***preamble inic***/
#ifndef _LFSM_H_
	#define _LFSM_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** Library
*/
#include <inttypes.h>
#include "eeprom.h"
/***preamble inic***/
/*
** constant and macro
*/
/*
** variable
*/
struct lfsmdata{
	uint8_t page;
	uint8_t feedback;
	uint8_t inhl;
	uint8_t inlh;
	uint8_t mask;
	uint8_t outhl;
	uint8_t outlh;
};
/******/
struct lfsm{
	//Local Variables
	EEPROM* eeprom;
	uint8_t sizeeeprom;
	uint8_t sizeblock;
	uint8_t page;
	uint8_t input;
	uint8_t output;
	uint8_t status;
	//Function Pointers
	uint8_t (*read)(struct lfsm *r, uint8_t input);
	uint8_t (*learn)(struct lfsm *r, const uint8_t input, const uint8_t next, const uint8_t mask, const uint8_t page);
	uint8_t (*quant)(struct lfsm *r);
	uint8_t (*remove)(struct lfsm *r, uint8_t input);
	uint8_t (*deleteall)(struct lfsm *r);
	uint8_t (*getoutput)(struct lfsm *r);
	uint8_t (*getstatus)(struct lfsm *r);
	void (*setoutput)(struct lfsm *r,uint8_t output);
	uint8_t (*getpage)(struct lfsm *r);
	void (*setpage)(struct lfsm *r,uint8_t page);
};
typedef struct lfsm LFSM;
/*
** procedure and function header
*/
LFSM LFSMenable(EEPROM* eeprom, const uint8_t sizeeeprom);
/***preamble inic***/
#endif
/***preamble inic***/
/************************************************************************
LFSM API END
************************************************************************/
