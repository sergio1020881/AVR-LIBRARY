/*************************************************************************
Title:    watch.c
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: watch.c,v 0.2 2017/07/01 17:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware:  
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
COMMENT:
	
*************************************************************************/
/*
** library
*/
#include <avr/io.h>
#include <inttypes.h>
/***/
#include "watch.h"
/*
** constant and macro
*/
/*
** variable
*/
struct TIME time;
char WATCH_vector[9];
/*
** procedure and function header
*/
uint8_t WATCH_hour(void);
uint8_t WATCH_minute(void);
uint8_t WATCH_second(void);
uint16_t WATCH_seconds(void);
void WATCH_set(uint8_t hour, uint8_t minute, uint8_t second);
void WATCH_setminute(void);
void WATCH_sethour(void);
void WATCH_increment(void);
void WATCH_decrement(void);
void WATCH_result(void);
char* WATCH_show(void);
/*
** procedure and function
*/
WATCH WATCHenable(void)
{
	WATCH watch;
	watch.hour=WATCH_hour;
	watch.minute=WATCH_minute;
	watch.second=WATCH_second;
	watch.seconds=WATCH_seconds;
	watch.set=WATCH_set;
	watch.setminute=WATCH_setminute;
	watch.sethour=WATCH_sethour;
	watch.increment=WATCH_increment;
	watch.decrement=WATCH_decrement;
	watch.show=WATCH_show;
	return watch;
}
uint8_t WATCH_hour(void)
{
	return time.hour;
}
uint8_t WATCH_minute(void)
{
	return time.minute;
}
uint8_t WATCH_second(void)
{
	return time.second;
}
uint16_t WATCH_seconds(void)
{
	return time.seconds;
}
void WATCH_set(uint8_t hour, uint8_t minute, uint8_t second)
{
	if( hour>=0 && hour<13 ){
		if(hour>0 && hour<12)
			time.hour=hour;
		else
			time.hour=12;
	}else
		time.hour=0;
	if( minute>=0 && minute<60 )
		time.minute=minute;
	else
		time.minute=0;
	if( second>=0 && second<60 );
	else
		time.second=0;
	time.seconds=hour*3600+minute*60+second;
}
void WATCH_setminute(void){
	uint16_t segundos;
	segundos=time.seconds;
	segundos+=60;
	if(segundos>43199)
		time.seconds=segundos-43200;
	else
		time.seconds=segundos;
}
void WATCH_sethour(void){
	uint16_t segundos;
	segundos=time.seconds;
	segundos+=3600;
	if(segundos>43199)
		time.seconds=segundos-43200;
	else
		time.seconds=segundos;
}
void WATCH_increment(void)
{
	if(time.seconds > 43199)
		time.seconds=0;
	else
		time.seconds++;
}
void WATCH_decrement(void)
{
	if(time.seconds)
		time.seconds--;
	else
		time.seconds=43199;
}
void WATCH_result(void)
{
	uint16_t segundos;
	uint8_t hour;
	segundos=time.seconds;
	hour=segundos/3600;
	if(hour)
		time.hour=hour;
	else
		time.hour=12;
	segundos%=3600;
	time.minute=segundos/60;
	time.second=segundos%60;
}
char* WATCH_show(void)
{
	uint8_t tmp;
	WATCH_result();
	WATCH_vector[8]='\0';
	WATCH_vector[7]=time.second % 10 + '0';
	tmp = time.second / 10;
	WATCH_vector[6]=tmp % 10 + '0';
	WATCH_vector[5]=':';
	WATCH_vector[4]=time.minute % 10 + '0';
	tmp = time.minute / 10;
	WATCH_vector[3]=tmp % 10 + '0';
	WATCH_vector[2]=':';
	WATCH_vector[1]=time.hour % 10 + '0';
	tmp = time.hour / 10;
	WATCH_vector[0]=tmp % 10 + '0';
	return WATCH_vector;
}
/*
** interrupt
*/
/***EOF***/
/***COMMENTS

***/
