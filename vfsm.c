/*************************************************************************
Title:    VFSM
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: vfsm.c,v 0.1 2015/04/11 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR ATmega128 at 16 Mhz, 
License:  GNU General Public License 
DESCRIPTION:
	Used to create complex state machines, MADE TO WORK IN ANY MCU FROM ATMEL AVR.
USAGE:
    Refere to the header file vfsm.h for a description of the routines. 
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
** Library
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <inttypes.h>
/***/
#include "vfsm.h"
/*
** constant and macro
*/
#define VFSM_SIZE_BLOCK 3
#define VLOGIC_SIZE_BLOCK 3
/*
** variable
*/
/*
** procedure and function header
*/
uint8_t VFSM_read(struct VFSM *r, uint8_t mask, uint8_t input, uint8_t output);
uint8_t VLOGIC_read(struct VLOGIC *l, uint8_t mask, uint8_t input, uint8_t output);
/*
** procedure and function
*/
struct VFSM VFSMenable(uint8_t *veeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	//struct
	VFSM fsm;
	//Init vars
	fsm.eeprom=veeprom;
	cells=sizeeeprom/VFSM_SIZE_BLOCK;
	fsm.sizeeeprom=cells*VFSM_SIZE_BLOCK;
	fsm.input=0;
	//function pointers
	fsm.read=VFSM_read;
	/******/
	return fsm;
}
struct VLOGIC VLOGICenable(uint8_t *veeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	//struct
	VLOGIC logic;
	//Init vars
	logic.eeprom=veeprom;
	cells=sizeeeprom/VLOGIC_SIZE_BLOCK;
	logic.sizeeeprom=cells*VLOGIC_SIZE_BLOCK;
	//function pointers
	logic.read=VLOGIC_read;
	/******/
	return logic;
}
uint8_t VFSM_read(struct VFSM *r, uint8_t mask, uint8_t input, uint8_t output)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[VFSM_SIZE_BLOCK];
	uint8_t ByteofData;
	int keyfound;
	if(((mask & r->input) ^ (mask & input)) == mask){ // condition: all masked pins change state
		for(i1=0;i1<r->sizeeeprom;i1+=VFSM_SIZE_BLOCK){
			ByteofData=*(r->eeprom+i1);
			for(i2=0;i2<VFSM_SIZE_BLOCK;i2++){ // get block from eeprom
				ByteofData=*(r->eeprom+(i1+i2));
				block[i2]=ByteofData;
			}
			keyfound=(block[0]==output && block[1]==(mask & input)); // bool
			if(keyfound){
				//r->input=input;
				output=block[2];
				break;
			}else{
				// do nothing
			}
		}
		r->input=input; // importante X
	}
	// printf("Eoutput -> %d\n",r->present);
	return output;
}
uint8_t VLOGIC_read(struct VLOGIC *l, uint8_t mask, uint8_t input, uint8_t output)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[VLOGIC_SIZE_BLOCK];
	uint8_t ByteofData;
	int keyfound;
	// conditions: masked input is programmed
	for(i1=0;i1<l->sizeeeprom;i1+=VLOGIC_SIZE_BLOCK){
		ByteofData=*(l->eeprom+i1);
		for(i2=0;i2<VLOGIC_SIZE_BLOCK;i2++){ // get block from eeprom
			ByteofData=*(l->eeprom+(i1+i2));
			block[i2]=ByteofData;
		}
		keyfound=(block[0]==output && block[1]==(input & mask)); // bool
		if(keyfound){
			output=block[2];
			break;
		}else{
			// do nothing
		}
	}
	return output;
}
/*
** interrupt
*/
/***EOF***/
/***COMMENT

***/
