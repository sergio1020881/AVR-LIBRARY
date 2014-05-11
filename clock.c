/*************************************************************************
Title:    clock.c
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: clock.c,v 0.2 2014/05/10 17:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
USAGE:
    Refer to the header file clock.h for a description of the routines. 
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
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
/***/
#include "clock.h"
/*
** constant and macro
*/
/*
** variable
*/
struct TIME time;
char CLOCK_timp[8];
uint8_t CLOCK_alarm_flag;
/*
** procedure and function header
*/
void CLOCK_increment(void);
void CLOCK_decrement(void);
uint8_t CLOCK_alarm(uint8_t hour, uint8_t minute, uint8_t second);
void CLOCK_alarm_reset(void);
char* CLOCK_show(void);
/*
** procedure and function
*/
CLOCK CLOCKenable(uint8_t hour, uint8_t minute, uint8_t second)
{
	CLOCK clock;
	time.hour=hour;
	time.minute=minute;
	time.second=second;
	CLOCK_alarm_flag=0;
	clock.increment=CLOCK_increment;
	clock.decrement=CLOCK_decrement;
	clock.alarm=CLOCK_alarm;
	clock.alarm_reset=CLOCK_alarm_reset;
	clock.show=CLOCK_show;
	return clock;
}
void CLOCK_increment(void)
{
	time.second++;
	if(time.second>59){
		time.minute++;
		time.second=0;
		if(time.minute>59){
			time.hour++;
			time.minute=0;
			switch (HORA){
				case 24:
					if(time.hour>23)
						time.hour=0;
					break;
				case 12:
					if (time.hour>12)
						time.hour=1;
					break;
				default:
					if(time.hour>23)
						time.hour=0;
					break;
			}
		}
	}		
}
void CLOCK_decrement(void)
{
	time.second--;
	if(time.second<0){
		time.minute--;
		time.second=59;
		if(time.minute<0){
			time.hour--;
			time.minute=59;
			switch (HORA){
				case 24:
					if(time.hour<0)
						time.hour=23;
					break;
				case 12:
					if (time.hour<1)
						time.hour=12;
					break;
				default:
					if(time.hour<0)
						time.hour=23;
					break;
			}
		}
	}		
}
uint8_t CLOCK_alarm(uint8_t hour, uint8_t minute, uint8_t second)
{
	if(time.hour==hour)
		CLOCK_alarm_flag=3;
	if(time.hour==hour && time.minute==minute)
		CLOCK_alarm_flag=2;
	if(time.hour==hour && time.minute==minute && time.second==second)
		CLOCK_alarm_flag=1;
	return CLOCK_alarm_flag;
}
void CLOCK_alarm_reset(void)
{
	CLOCK_alarm_flag=0;
}
char* CLOCK_show(void)
{
	uint8_t tmp;
	CLOCK_timp[8]='\0';
	CLOCK_timp[7]=time.second % 10 + '0';
	tmp = time.second / 10;
	CLOCK_timp[6]=tmp % 10 + '0';
	CLOCK_timp[5]=':';
	CLOCK_timp[4]=time.minute % 10 + '0';
	tmp = time.minute / 10;
	CLOCK_timp[3]=tmp % 10 + '0';
	CLOCK_timp[2]=':';
	CLOCK_timp[1]=time.hour % 10 + '0';
	tmp = time.hour / 10;
	CLOCK_timp[0]=tmp % 10 + '0';
	return CLOCK_timp;
}
/*
** interrupt
*/
/***EOF***/
