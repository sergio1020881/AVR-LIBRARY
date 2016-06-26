/************************************************************************
Title:    TRANSITION
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: transition.h,v 0.1 2016/06/26 15:00:00 sergio Exp $
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
#ifndef _TRANSITION_H_
	#define _TRANSITION_H_
/*
** constant and macro
*/
/*
** variable
*/
struct TRAN{
	/***Variables***/
	uint8_t data;
	uint8_t hl;
	uint8_t lh;
	/***PROTOTYPES VTABLE***/
	uint8_t (*update)(struct TRAN *tr, uint8_t idata);
};
typedef struct TRAN TRAN;
/*
** procedure and function header
*/
TRAN TRANenable(void);
#endif
/***EOF***/
