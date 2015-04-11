/************************************************************************
Title:    VFSM
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: vfsm.h,v 0.1 2015/04/11 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR ATmega128 at 16 Mhz
License:  GNU General Public License
DESCRIPTION:
	Any AVR microcontroller.
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
COMMENT:
	Very Stable
************************************************************************/
#ifndef VFSM_H_
	#define VFSM_H_
/*
** constant and macro
*/
/*
** variable
*/
struct VFSM{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	uint8_t input;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct VFSM *r, uint8_t mask, uint8_t input, uint8_t output);
};
struct VLOGIC{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct VLOGIC *l, uint8_t mask, uint8_t input, uint8_t output);
};
typedef struct VFSM VFSM;
typedef struct VLOGIC VLOGIC;
/*
** procedure and function header
*/
VFSM VFSMenable(uint8_t *veeprom, unsigned int sizeeeprom );
VLOGIC VLOGICenable(uint8_t *veeprom, unsigned int sizeeeprom );
#endif
/***EOF***/
