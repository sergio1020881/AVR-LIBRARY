/*************************************************************************
Title:    ATMEGA324TIMER
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: atmega324timer.h,v 0.1 2018/08/18 13:00:00 Sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
License:  GNU General Public License
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
	Very Stable
*************************************************************************/
#ifndef ATMEGA324TIMER_H_
  #define ATMEGA324TIMER_H_
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
/*
** variable
*/
struct TIMER_COUNTER0{
	// prototype pointers
	void (*compoutmodeA)(uint8_t compoutmode);
	void (*compoutmodeB)(uint8_t compoutmode);
	void (*compareA)(uint8_t compare);
	void (*compareB)(uint8_t compare);
	void (*start)(uint16_t prescaler);
	void (*stop)(void);
};
typedef struct TIMER_COUNTER0 TIMER_COUNTER0;
/**/
struct TIMER_COUNTER1{
	// prototype pointers
	void (*compoutmodeA)(uint8_t compoutmode);
	void (*compoutmodeB)(uint8_t compoutmode);
	void (*compareA)(uint16_t compare);
	void (*compareB)(uint16_t compare);
	void (*start)(uint16_t prescaler);
	void (*stop)(void);
};
typedef struct TIMER_COUNTER1 TIMER_COUNTER1;
/**/
struct TIMER_COUNTER2{
	// prototype pointers
	void (*compoutmodeA)(uint8_t compoutmode);
	void (*compoutmodeB)(uint8_t compoutmode);
	void (*compareA)(uint8_t compare);
	void (*compareB)(uint8_t compare);
	void (*start)(uint16_t prescaler);
	void (*stop)(void);
};
typedef struct TIMER_COUNTER2 TIMER_COUNTER2;
/*
** procedure and function header
*/
TIMER_COUNTER0 TIMER_COUNTER0enable(uint8_t wavegenmode, uint8_t interrupt);
TIMER_COUNTER1 TIMER_COUNTER1enable(uint8_t wavegenmode, uint8_t interrupt);
TIMER_COUNTER2 TIMER_COUNTER2enable(uint8_t wavegenmode, uint8_t interrupt);
#endif
/***EOF***/
