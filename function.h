/************************************************************************
Title:    FUNCTION
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: function.h,v 0.1 2013/12/30 15:00:00 sergio Exp $
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
#ifndef FUNCTION_H_
	#define FUNCTION_H_
/*
** constant and macro
*/
/*
** variable
*/
struct FUNC{
	/***PROTOTYPES VTABLE***/
	unsigned int (*power)(uint8_t base, uint8_t n);
	int (*stringlength)(const char string[]);
	void (*reverse)(char s[]);
	unsigned int (*mayia)(unsigned int xi, unsigned int xf, uint8_t nbits);
	uint8_t (*lh)(uint8_t xi, uint8_t xf);
	uint8_t (*hl)(uint8_t xi, uint8_t xf);
	uint8_t (*diff)(uint8_t xi, uint8_t xf);
	uint8_t (*hmerge)(uint8_t X, uint8_t Y);
	uint8_t (*lmerge)(uint8_t X, uint8_t Y);
	void (*swap)(int *px, int *py);
	void (*copy)(char to[], char from[]);
	void (*squeeze)(char s[], int c);
	void (*shellsort)(int v[], int n);
	void (*itoa)(int32_t n, char s[]);
	int (*trim)(char s[]);
	int (*pmax)(int a1, int a2);
	int (*gcd)(int u, int v);
	int (*strToInt)(const char string[]);
	uint8_t (*filter)(uint8_t mask, uint8_t data);
	unsigned int (*ticks)(unsigned int num);
	int (*twocomptoint8bit)(int twoscomp);
	int (*twocomptoint10bit)(int twoscomp);
	int (*twocomptointnbit)(int twoscomp, uint8_t nbits);
	char (*dec2bcd)(char num);
	char (*bcd2dec)(char num);
	char* (*resizestr)(char *string, int size);
	long (*trimmer)(long x, long in_min, long in_max, long out_min, long out_max);
};
typedef struct FUNC FUNC;
/*
** procedure and function header
*/
FUNC FUNCenable( void );
#endif
/***EOF***/
