/*************************************************************************
Title:    VFSM
Author:   Sergio Salazar Santos <sergio.salazar.santos@gmail.com>
File:     $Id: vfsm.c,v 0.1 2014/04/09 14:30:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR ATmega128 at 16 Mhz, 
License:  GNU General Public License 
          
DESCRIPTION:
    
    
USAGE:
    Refere to the header file analog.h for a description of the routines. 

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

//MADE TO WORK IN ANY MCU FROM ATMEL AVR

/*
** Library
*/
#include <avr/eeprom.h>

/*
** Private Library
*/
#include "vfsm.h"

/*
**  module constants and macros
*/
#define VFSM_SIZE_BLOCK 3
#define VLOGIC_SIZE_BLOCK 3

/*
** module variables
*/

/*
** module function header
*/
uint8_t VFSM_diff(uint8_t xi, uint8_t xf);

/*
** module interrupt header
*/
/***************************************************************************************
****************************************************************************************/
/*
** module procedure and function definitions
*/

/*
**  module object 1 constructor
*/
struct VFSM VFSMenable(uint8_t *veeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	//uint8_t i;
	/***Declare Functions***/
	uint8_t VFSM_read(struct VFSM *r, uint8_t input, uint8_t output);
	//struct
	struct VFSM fsm;
	//Init vars
	fsm.eeprom=veeprom;
	fsm.sizeblock=VFSM_SIZE_BLOCK;
	cells=sizeeeprom/fsm.sizeblock;
	fsm.sizeeeprom=cells*fsm.sizeblock;
	fsm.input=0;
	//for(i=0;i<VFSM_PAGES;i++)
		//fsm.output[i]=0;
	//function pointers
	fsm.read=VFSM_read;
	fsm.diff=VFSM_diff;
	/******/
	return fsm;
}

/*
**  module object 2 constructor
*/
struct VLOGIC VLOGICenable(uint8_t *veeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	//uint8_t i;
	/***Declare Functions***/
	uint8_t VLOGIC_read(struct VLOGIC *l, uint8_t input, uint8_t output);
	//struct
	struct VLOGIC logic;
	//Init vars
	logic.eeprom=veeprom;
	logic.sizeblock=VLOGIC_SIZE_BLOCK;
	cells=sizeeeprom/logic.sizeblock;
	logic.sizeeeprom=cells*logic.sizeblock;
	logic.input=0;
	//for(i=0;i<VFSM_PAGES;i++)
		//fsm.output[i]=0;
	//function pointers
	logic.read=VLOGIC_read;
	logic.diff=VFSM_diff;
	/******/
	return logic;
}

/*
** module object 1 procedures and function definitions
*/
/***read***/
uint8_t VFSM_read(struct VFSM *r, uint8_t input, uint8_t output)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[r->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	
	if(r->input!=input){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=*(r->eeprom+i1);
			for(i2=0;i2<r->sizeblock;i2++){ // get block from eeprom
				ByteofData=*(r->eeprom+(i1+i2));
				block[i2]=ByteofData;
			}
			keyfound=(block[0]==output && block[1]==input); // bool
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

/*
** module object 2 procedures and function definitions
*/
/***read***/
uint8_t VLOGIC_read(struct VLOGIC *l, uint8_t input, uint8_t output)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[l->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	
	for(i1=0;i1<l->sizeeeprom;i1+=l->sizeblock){
		ByteofData=*(l->eeprom+i1);
		for(i2=0;i2<l->sizeblock;i2++){ // get block from eeprom
			ByteofData=*(l->eeprom+(i1+i2));
			block[i2]=ByteofData;
		}
		keyfound=(block[0]==output && block[1]==input); // bool
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
** modules global use functions
*/
/***diff***/
uint8_t VFSM_diff(uint8_t xi, uint8_t xf)
{
	return xf^xi;
}

/*
** module interrupts
*/
/***EOF***/
