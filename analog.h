#ifndef ANALOG_H
	#define ANALOG_H
/************************************************************************
Title:    Interrupt ANALOG INPUT
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: analog.h,v 0.2 2014/04/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
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
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constants and macros
*/
/*
** global variables
*/
struct ANALOG{
	/***Parameters***/
	uint8_t VREFF;
	uint8_t DIVISION_FACTOR;
	
	//prototype pointers
	int (*read)(int selection);
};
typedef struct ANALOG ANALOG;
/*
** global object function prototypes
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channels, ... );
/*
** global function prototypes
*/
#endif // ANALOG_H
/***EOF***/
