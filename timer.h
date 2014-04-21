/*************************************************************************
Title:    TIMER0
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: vfsm.c,v 0.1 2014/04/09 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR ATmega128 at 16 Mhz, 
License:  GNU General Public License 
DESCRIPTION:
	Atmega 128 at 16MHZ
USAGE:
NOTES:
    Based on Atmel Application Note AVR306
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
*************************************************************************/
#ifndef TIMER_H_
  #define TIMER_H_
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
/*
** variable
*/
struct TIMER0{
	/***Parameters***/
	unsigned char wavegenmode;
	unsigned char compoutmode;
	unsigned char interrupt;
	// prototype pointers
	void (*start)(unsigned char compare, unsigned int prescaler);
	void (*stop)(void);
};
typedef struct TIMER0 TIMER0;
/*
** procedure and function header
*/
TIMER0 TIMER0enable(unsigned char wavegenmode, unsigned char compoutmod, unsigned char interrupt);
#endif
/***EOF***/
