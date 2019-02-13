/*************************************************************************
Title:    Atmega 324 Preamble
Author:   Sergio Manuel Salazar dos Santos <sergio.salazar.santos@gmail.com>
File:     $Id: atmega324preamble.c,v 0.2 2018/08/18 13:00:00 Sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: Atmega 324A
License:  GNU General Public License
DESCRIPTION:
	IO Control
USAGE:
    Refer to the header file atmega324preamble.h for a description of the routines.
NOTES:
    Based on Atmel Application Note AVR306
LICENSE:
    Copyright (C) 2018
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
COMMENT:
	Stable
*************************************************************************/

#include <avr/io.h>
#include <inttypes.h>

#include "Atmega324Preamble.h"

#if defined(__AVR_ATmega324A__) || defined(__AVR_ATmega324PA__)

#define A 0
#define B 1
#define C 2
#define D 3
#define NIO 4

static uint8_t bckIO[2][NIO];

void Atmega324ioread(void);
uint8_t Atmega324hl(char port);
uint8_t Atmega324ll(char port);
uint8_t Atmega324lh(char port);
uint8_t Atmega324hh(char port);

ATMEGA324 ATMEGA324enable(void)
{
	bckIO[1][A]=PINA;
	bckIO[1][B]=PINB;
	bckIO[1][C]=PINC;
	bckIO[1][D]=PIND;

	struct Atmega324 mcu;

	mcu.ioread = Atmega324ioread;
	mcu.hl	= Atmega324hl;
	mcu.ll	= Atmega324ll;
	mcu.lh	= Atmega324lh;
	mcu.hh	= Atmega324hh;

	return mcu;
}
void Atmega324ioread(void) // This function is to be put right after the start of the main cycle or While loop
{
	bckIO[0][A]=bckIO[1][A];
	bckIO[0][B]=bckIO[1][B];
	bckIO[0][C]=bckIO[1][C];
	bckIO[0][D]=bckIO[1][D];
	bckIO[1][A]=PINA;
	bckIO[1][B]=PINB;
	bckIO[1][C]=PINC;
	bckIO[1][D]=PIND;
}
uint8_t Atmega324hl(char port)
{
	uint8_t i;
	switch (port) {
		case 'A':
			i=bckIO[1][A]^bckIO[0][A];
			i&=bckIO[0][A];
			break;
		case 'B':
			i=bckIO[1][B]^bckIO[0][B];
			i&=bckIO[0][B];
			break;
		case 'C':
			i=bckIO[1][C]^bckIO[0][C];
			i&=bckIO[0][C];
			break;
		case 'D':
			i=bckIO[1][D]^bckIO[0][D];
			i&=bckIO[0][D];
			break;
		default:
			i=0;
			break;
	}
	return i;
}
uint8_t Atmega324ll(char port)
{
	uint8_t i;
	switch (port) {
		case 'A':
			i=~(bckIO[0][A]|bckIO[1][A]);
			break;
		case 'B':
			i=~(bckIO[0][B]|bckIO[1][B]);
			break;
		case 'C':
			i=~(bckIO[0][C]|bckIO[1][C]);
			break;
		case 'D':
			i=~(bckIO[0][D]|bckIO[1][D]);
			break;
		default:
			i=0;
			break;
	}
	return i;
}
uint8_t Atmega324lh(char port)
{
	uint8_t i;
	switch (port) {
		case 'A':
			i=bckIO[0][A]^bckIO[1][A];
			i&=bckIO[1][A];
			break;
		case 'B':
			i=bckIO[0][B]^bckIO[1][B];
			i&=bckIO[1][B];
			break;
		case 'C':
			i=bckIO[0][C]^bckIO[1][C];
			i&=bckIO[1][C];
			break;
		case 'D':
			i=bckIO[0][D]^bckIO[1][D];
			i&=bckIO[1][D];
			break;
		default:
			i=0;
			break;
	}
	return i;
}
uint8_t Atmega324hh(char port)
{
	uint8_t i;
	switch (port) {
		case 'A':
			i=bckIO[0][A]&bckIO[1][A];
			break;
		case 'B':
			i=bckIO[0][B]&bckIO[1][B];
			break;
		case 'C':
			i=bckIO[0][C]&bckIO[1][C];
			break;
		case 'D':
			i=bckIO[0][D]&bckIO[1][D];
			break;
		default:
			i=0;
			break;
	}
	return i;
}
#else
	#error "Not ATmega 324A or 324PA"
#endif
/***EOF***/
