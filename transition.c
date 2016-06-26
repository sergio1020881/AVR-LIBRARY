/*************************************************************************
Title:    TRANSITION
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: transition.c,v 0.1 2016/06/26 15:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
	Atmega 128 at 16MHZ
USAGE:
    Refere to the header file transition.h for a description of the routines. 
NOTES:
    Based on Atmel Application Note AVR306
LICENSE:
    Copyright (C) 2013
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
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdarg.h>
#include <inttypes.h>
/***pc use***
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
*/
/***/
#include"transition.h"
/*
** constant and macro
*/
#ifndef GLOBAL_INTERRUPT_ENABLE
 #define GLOBAL_INTERRUPT_ENABLE 7
#endif
/*
** variable
*/
/*
** procedure and function header
*/
uint8_t TRANlh(uint8_t xi, uint8_t xf);
uint8_t TRANhl(uint8_t xi, uint8_t xf);
uint8_t TRANupdate(struct TRAN *tr, uint8_t idata);
/*
** procedure and function
*/
TRAN TRANenable( void )
{
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// struct object
	TRAN tran;
	// local var
	tran.data=0;
	tran.hl=0;
	tran.lh=0;
	// function pointers
	tran.update=TRANupdate;
	SREG=tSREG;
	/******/
	return tran;
}
// lh
uint8_t TRANlh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi^xf;
	i&=xf;
	return i;
}
// hl
uint8_t TRANhl(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xi;
	return i;
}
// TRANupdate
uint8_t TRANupdate(struct TRAN *tran, uint8_t idata)
{
	uint8_t r;
	if(tran->data == idata)
		r=0;
	else{
		tran->lh = TRANlh(tran->data, idata);
		tran->hl = TRANhl(tran->data, idata);
		tran->data=idata;
		r=1;
	}
	return r;
}
/*
** interrupt
*/
/***EOF***/
/***COMMENTS

***/
