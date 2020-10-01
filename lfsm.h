/************************************************************************
Title: LFSM
Author: Sergio Santos
   <sergio.salazar.santos@gmail.com>
File: $Id: lfsm.h,v 0.1 01/10/2020 Exp $
Hardware: Atmel
License: GNU General Public License
Comment:
	very stable
************************************************************************/
#ifndef _LFSM_H_
	#define _LFSM_H_
/******/
#include "../EEPROM/eeprom.h"
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
	EEPROM* eprom;
	uint16_t sizeeeprom;
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
LFSM LFSMenable(EEPROM* eeprom, const uint16_t sizeeeprom);
#endif
/***EOF***/
