/************************************************************************
Title:    FSM
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: fsm.h,v 0.1 2015/04/11 15:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
License:  GNU General Public License 
USAGE:    
LICENSE:
    Copyright (C) 2014
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
************************************************************************/
#ifndef FSM_H_
	#define FSM_H_
/*
** constant and macro
*/
#define EMPTY 255
#define SFSM_PAGES 8
/*
** variable
*/
struct EFSM{
	uint8_t sizeeeprom;
	uint8_t sizeblock;
	uint8_t page;
	uint8_t record[5];
	char type;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct EFSM* fsm, uint8_t input, uint8_t feedback);
	int (*learn)(struct EFSM *r, uint8_t input, uint8_t next, uint8_t feedback);
	unsigned int (*quant)(struct EFSM *r);
	int (*remove)(struct EFSM *r, uint8_t input, uint8_t state);
	int (*deleteall)(struct EFSM *r);
	uint8_t (*feedback)(struct EFSM *r);
	uint8_t (*present)(struct EFSM *r);
	char (*typeget)(struct EFSM *r);
	uint8_t (*diff)(uint8_t xi, uint8_t xf);
};
typedef struct EFSM EFSM;
struct SFSM{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	unsigned int sizeblock;
	uint8_t input[SFSM_PAGES];
	uint8_t state[SFSM_PAGES];
};
typedef struct SFSM SFSM;
struct LFSM{
	int *eeprom;
	unsigned int sizeeeprom;
	unsigned int sizeblock;
	int command;
	int feedback;
	int instruction;
};
/*
** procedure and function header
*/
EFSM EFSMenable(unsigned int sizeeeprom, uint8_t prog );
void SFSMinit(uint8_t *eeprom, unsigned int sizeeeprom);
uint8_t sfsm_read(uint8_t input, uint8_t feedback, uint8_t page);
uint8_t sfsm_state(uint8_t page);
#endif // FSM_H_
/***EOF***/
