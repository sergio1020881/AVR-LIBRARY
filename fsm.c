/*************************************************************************
Title:    FSM
Author:   Sergio Salazar Santos <sergio1020881@gmail.com>
File:     $Id: fsm.c,v 0.1 2013/12/30 15:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License 
          
DESCRIPTION:
    
    
USAGE:
    Refere to the header file analog.h for a description of the routines. 

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
                        
*************************************************************************/
//MADE TO WORK IN ANY MCU FROM ATMEL AVR
/*
** Library
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
/*
** Private Library
*/
#include "fsm.h"
/*
**  module constants and macros
*/
#define SFSM_SIZE_BLOCK 5
#define EFSM_SIZE_BLOCK 5
#define INPUT 0
#define INDIFF 1
#define INLH 2
#define FEEDBACK 3
#define PRESENT 4
/*
** module variables
*/
struct LFSM lfsm;
struct SFSM sfsm;
/*
** module function header
*/
uint8_t FSM_diff(uint8_t xi, uint8_t xf);
/*
** module interrupt header
*/
/***************************************************************************************
****************************************************************************************/
/*
** module procedure and function definitions
*/
uint8_t FSM_routine(uint8_t cmd)
{
	unsigned int i1;
	unsigned int i2;
	unsigned char block[lfsm.sizeblock];
	unsigned char ByteofData;
	int keyfound;
	int diferenca;
	diferenca=FSM_diff(lfsm.command, cmd);
	if(diferenca){ // oneshot
		for(i1=0;i1<lfsm.sizeeeprom;i1+=lfsm.sizeblock){
			for(i2=0;i2<lfsm.sizeblock;i2++){ // get block from eeprom
				ByteofData=*(lfsm.eeprom+(i1+i2));
				block[i2]=ByteofData;
			}
			keyfound=(block[0]==cmd && block[1]==lfsm.instruction); // bool
			if(keyfound){
				lfsm.command=block[0];
				lfsm.feedback=block[1];
				lfsm.instruction=block[2];
				break;
			}else{
				lfsm.feedback=lfsm.instruction;
			}
		}
		lfsm.command=cmd; // importante
	}
	// printf("Eoutput -> %d\n",r->present);
	return lfsm.instruction;
}
// SFSM INIT
void SFSMinit(uint8_t *eeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	uint8_t i;
	//Init vars
	sfsm.eeprom=eeprom;
	sfsm.sizeblock=SFSM_SIZE_BLOCK;
	cells=sizeeeprom/sfsm.sizeblock;
	sfsm.sizeeeprom=cells*sfsm.sizeblock;
	for(i=0;i<SFSM_PAGES;i++)
		sfsm.state[i]=0;
}
/***read***/
uint8_t sfsm_read(uint8_t input, uint8_t feedback, uint8_t page)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[sfsm.sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int diferenca;
	diferenca=FSM_diff(sfsm.input[page],input);
	if(diferenca){
		for(i1=0;i1<sfsm.sizeeeprom;i1+=sfsm.sizeblock){
			ByteofData=*(sfsm.eeprom+i1);
			if(ByteofData==page){
				for(i2=0;i2<sfsm.sizeblock;i2++){//get block from eeprom
					ByteofData=*(sfsm.eeprom+(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==feedback); // bool
				if(keyfound){
					sfsm.state[page]=block[4];
					break;
				}else{
					// do nothing
				}
			}			
		}
		sfsm.input[page]=input; // very importante
	}
	//printf("Eoutput -> %d\n",r->present);
	return sfsm.state[page];
}
/***get***/
uint8_t sfsm_state(uint8_t page)
{
	//printf("FSMpresent: %d\n",r->record[PRESENT]);
	return sfsm.state[page];
}
/*
** module local use
*/
/***diff***/
uint8_t FSM_diff(uint8_t xi, uint8_t xf)
{
	return xf^xi;
}
/*
**  module object 1 constructor
*/
struct EFSM EFSMenable(unsigned int sizeeeprom, uint8_t prog)
{
	unsigned int cells;
	/***Declare Functions***/
	uint8_t EFSM_read(struct EFSM* fsm, uint8_t input, uint8_t feedback);
	int EFSM_learn(struct EFSM *r, uint8_t input, uint8_t next, uint8_t feedback);
	unsigned int EFSM_quant(struct EFSM *r);
	int EFSM_remove(struct EFSM *r, uint8_t input, uint8_t present);
	int EFSM_deleteall(struct EFSM *r);
	uint8_t EFSM_feedback(struct EFSM *r);
	uint8_t EFSM_present(struct EFSM *r);
	char EFSM_typeget(struct EFSM *r);
	//struct
	struct EFSM fsm;
	//Init vars
	fsm.sizeblock=5;
	cells=sizeeeprom/fsm.sizeblock;
	fsm.sizeeeprom=cells*fsm.sizeblock;
	fsm.page=prog;
	fsm.record[INPUT]=0;
	fsm.record[INDIFF]=0;
	fsm.record[INLH]=0;
	fsm.record[FEEDBACK]=0;
	fsm.record[PRESENT]=0;
	fsm.type='X';
	//function pointers
	fsm.read=EFSM_read;
	fsm.learn=EFSM_learn;
	fsm.quant=EFSM_quant;
	fsm.remove=EFSM_remove;
	fsm.deleteall=EFSM_deleteall;
	fsm.feedback=EFSM_feedback;
	fsm.present=EFSM_present;
	fsm.typeget=EFSM_typeget;
	fsm.diff=FSM_diff;
	/******/
	return fsm;
}
/*
** module object 1 procedures and function definitions
*/
/***read***/
uint8_t EFSM_read(struct EFSM *r, uint8_t input, uint8_t feedback)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[r->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int diferenca;
	
	diferenca=r->diff(r->record[INPUT],input);
	if(diferenca){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==r->page){
				for(i2=0;i2<r->sizeblock;i2++){//get block from eeprom
					ByteofData=eeprom_read_byte((uint8_t*)(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==feedback);//bool
				if(keyfound){
					r->record[INDIFF]=block[1];
					r->record[INLH]=block[2];
					r->record[FEEDBACK]=block[3];
					r->record[PRESENT]=block[4];
					r->type='=';
					break;
				}else{
					r->record[FEEDBACK]=r->record[PRESENT];
					r->type='X';
				}
			}
		}
		r->record[INPUT]=input;//importante
	}
	//printf("Eoutput -> %d\n",r->present);
	return r->record[PRESENT];
}
/***learn***/
int EFSM_learn(struct EFSM *r, uint8_t input, uint8_t next, uint8_t feedback)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[r->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int status=0;
	int diferenca;
	diferenca=r->diff(r->record[INPUT],input);
	if(diferenca){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==r->page){
				for(i2=0;i2<r->sizeblock;i2++){//get block from eeprom
					ByteofData=eeprom_read_byte((uint8_t*)(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==feedback);//bool
				if(keyfound){
					status=1;//not permited
					break;
				}
			}
			status=3;//not existente
		}
	}
	if(status==3){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==EMPTY){
				eeprom_update_byte((uint8_t*)i1,r->page);
				eeprom_update_byte((uint8_t*)(i1+1),diferenca);
				eeprom_update_byte((uint8_t*)(i1+2),(diferenca & input));
				eeprom_update_byte((uint8_t*)(i1+3),feedback);
				eeprom_update_byte((uint8_t*)(i1+4),next);
				status=2;//created
				break;
			}
			status=4;//not possible
		}
	}
	r->record[INPUT]=input;//see
	//printf("learn status: %d\n",status);
	return status;
}
/***quant***/
unsigned int EFSM_quant(struct EFSM *r)
{
	unsigned int i1;
	unsigned int programmed;
	uint8_t ByteofData;
	for(i1=0,programmed=0;i1<r->sizeeeprom;i1+=r->sizeblock){
		ByteofData=eeprom_read_byte((uint8_t*)i1);
		if(ByteofData!=EMPTY)
			programmed++;
	}
	//printf("there are %d programmed\n",programmed);
	return programmed;
}
/***remove***/
int EFSM_remove(struct EFSM *r, uint8_t input, uint8_t state)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[r->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int status=0;
	int diferenca;
	diferenca=r->diff(r->record[INPUT],input);
	if(diferenca){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==r->page){
				for(i2=0;i2<r->sizeblock;i2++){//get block from eeprom
					ByteofData=eeprom_read_byte((uint8_t*)(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==state);//bool
				if(keyfound){
					eeprom_update_byte((uint8_t*)i1,EMPTY);
					status=1;//removed
					break;
				}
			}
		}
		r->record[INPUT]=input;
	}
	//printf("remove status: %d\n",status);
	return status;
}
/***deleteall***/
int EFSM_deleteall(struct EFSM *r)
{
	unsigned int i1;
	uint8_t ByteofData;
	int status=0;
	if(!status){//not removed
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData!=EMPTY){
				eeprom_update_byte((uint8_t*)i1,EMPTY);
				status=1;//all deleted
			}
		}
	}
	//printf("deleteall status: %d\n",status);
	return status;
}
/***get***/
uint8_t EFSM_present(struct EFSM *r)
{
	//printf("FSMpresent: %d\n",r->record[PRESENT]);
	return r->record[PRESENT];
}

uint8_t EFSM_feedback(struct EFSM *r)
{
	//printf("FSMfeedback: %d\n",r->record[FEEDBACK]);
	return r->record[FEEDBACK];
}
/***typeget***/
char EFSM_typeget(struct EFSM *r)
{
	return r->type;
}
/*
** module interrupts
*/
/***EOF***/
