#ifndef _LCD_H_
  #define _LCD_H_
/************************************************************************
Title:    LCD
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: lcd.h,v 0.2 2014/04/12 00:00:00 sergio Exp $
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
	  tested atemga 128 16Mhz, Very Stable
************************************************************************/
/*
** constants and macros
*/
//ASIGN PORT PINS TO LCD (can be setup in any way)
#define RS 0
#define RW 1
#define EN 2
#define NC 3
#define DB0 4
#define DB1 5
#define DB2 6
#define DB3 7
/*
** global variables
*/
/*
** global prototypes
*/
struct display{
	/******/
	unsigned int (*ticks)(unsigned int num);
	void (*strobe)(unsigned int num);
	void (*inic)(void);
	void (*write)(char c, unsigned short D_I);
	char (*read)(unsigned short D_I);
	void (*BF)(void);
	void (*putch)(char c);
	char (*getch)(void);
	void (*string)(const char *s);
	void (*clear)(void);
	void (*gotoxy)(unsigned int x, unsigned int y);
	void (*reboot)(void);
};
typedef struct display LCD;
/*
** global object function header
*/
LCD LCDenable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
/*
** global function header
*/
#endif // LCD_H_
/***EOF***/
