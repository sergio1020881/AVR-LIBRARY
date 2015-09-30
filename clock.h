/************************************************************************
Title:    clock.h
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: clock.h,v 0.2 2015/04/11 17:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: ATmega128 at 16 Mhz
License:  GNU General Public License 
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
#ifndef CLOCK_H_
	#define CLOCK_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
#define HORA 24
/*
** variable
*/
struct TIME{
	int8_t hour;
	int8_t minute;
	int8_t second;
};
struct CLOCK{
	void (*set)(uint8_t hour, uint8_t minute, uint8_t second);
	void (*increment)(void);
	void (*decrement)(void);
	uint8_t (*alarm)(uint8_t hour, uint8_t minute, uint8_t second);
	uint8_t (*second_count)(uint16_t second);
	void (*second_count_reset)(void);
	void (*second_count_stop)(void);
	void (*alarm_reset)(void);
	void (*alarm_stop)(void);
	char* (*show)(void);
};
typedef struct CLOCK CLOCK;
/*
** procedure and function header
*/
CLOCK CLOCKenable(uint8_t hour, uint8_t minute, uint8_t second);
#endif /* CLOCK_H_ */
/***EOF***/
