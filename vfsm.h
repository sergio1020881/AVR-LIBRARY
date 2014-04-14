#ifndef VFSM_H_
	#define VFSM_H_
/************************************************************************
Title:    VFSM
Author:   Sergio Salazar Santos <sergio.salazar.santos@gmail.com>
File:     $Id: vfsm.h,v 0.1 2014/04/09 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR ATmega128 at 16 Mhz
License:  GNU General Public License 
Usage:    

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
	
COMMENT:
	Very Stable
    
************************************************************************/
/*
** constants and macros
*/
/*
** global variables
*/
/*
** global prototypes
*/
struct VFSM{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	uint8_t input;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct VFSM *r, uint8_t input, uint8_t output);
};
struct VLOGIC{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	uint8_t input;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct VLOGIC *l, uint8_t input, uint8_t output);
};
typedef struct VFSM VFSM;
typedef struct VLOGIC VLOGIC;
/*
** global object function header
*/
VFSM VFSMenable(uint8_t *veeprom, unsigned int sizeeeprom );
VLOGIC VLOGICenable(uint8_t *veeprom, unsigned int sizeeeprom );
/*
** global function header
*/
#endif // FSM_H_
/***EOF***/
