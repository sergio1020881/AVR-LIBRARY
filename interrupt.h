/************************************************************************
Title:    Interrupt
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: interrupt.h,v 0.2 2014/04/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
License:  GNU General Public License 
USAGE:
	Refer to datasheet of microcontroller    
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
************************************************************************/
#ifndef INTERRUPT_H
	#define INTERRUPT_H
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
/*
** variable
*/
struct INTERRUPT{
	void (*set)(uint8_t channel, uint8_t sense);
	void (*off)(uint8_t channel);
};
typedef struct INTERRUPT INTERRUPT;
/*
** procedure and function header
*/
INTERRUPT INTERRUPTenable(void);
#endif
/***EOF***/
