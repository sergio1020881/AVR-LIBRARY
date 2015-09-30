/************************************************************************
Title:    MM74C923
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: mm74c923.h,v 0.1 2015/04/11 14:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
License:  GNU General Public License
DESCRIPTION:
	Atemga 128 16Mhz
USAGE:
	Supports 20 key encoder
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
	  stable
************************************************************************/
#ifndef _MM74C923_H_
	#define _MM74C9232_H_
/*
** constant and macro
*/
#define MM74C923_DATA_OUT_A 7 //ic pin 19 LSB
#define MM74C923_DATA_OUT_B 6 //ic pin 18
#define MM74C923_DATA_OUT_C 5 //ic pin 17
#define MM74C923_DATA_OUT_D 4 //ic pin 16
#define MM74C923_DATA_OUT_E 3 //ic pin 15
#define MM74C923_EXTRA_DATA_OUT_PIN 2 // MSB
#define MM74C923_OUTPUT_ENABLE 1 //ic pin 14
#define MM74C923_DATA_AVAILABLE 0 //ic pin 13
/*
** variable
*/
struct mm74c923{
	void (*activate)(void);
	char (*getch)(void);
	char* (*gets)(void);
	char* (*data)(void);
	void (*data_clear)(void);
};
typedef struct mm74c923 MM74C923;
/*
** procedure and function header
*/
MM74C923 MM74C923enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
#endif
/***EOF***/
