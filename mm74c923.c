/*************************************************************************
Title:    MM74C923
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: mm74c923.c,v 0.2 2015/2/16 14:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
	Atemga 128 at 16Mhz
USAGE:
    Refere to the header file mm74c923.h for a description of the routines.
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
	working on it              
*************************************************************************/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/*
** Library
*/
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
/***/
#include "mm74c923.h"
#include "function.h"
/*
** constant and macro
*/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
/*
** variable
*/
volatile uint8_t *mm74c923_DDR;
volatile uint8_t *mm74c923_PIN;
volatile uint8_t *mm74c923_PORT;
uint8_t mm74c923_mem;
uint8_t mm74c923_index;
char MM74C923_KEY_CODE[]={
	'A','B','C','E','G','H','I','J','M','N','O','P','Q','R','S','T','V','X','Y','Z',
	'0','0','0','0','0','0','0','0','0','0','0','0','L','-','+','F','7','8','9','#',
	'4','5','6','U','1','2','3','D','0','/','.','*',' '
};
/*
** procedure and function header
*/
char MM74C923_read(void);
/*
** procedure and function
*/
MM74C923 MM74C923enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA Para Estrutura
	MM74C923 mm74c923;
	//import parametros
	mm74c923_DDR=ddr;
	mm74c923_PIN=pin;
	mm74c923_PORT=port;
	//inic variables
	*mm74c923_DDR=0x00;
	*mm74c923_PORT=0xFF;
	mm74c923_mem=0;
	mm74c923_index=0;
	//Direccionar apontadores para PROTOTIPOS
	mm74c923.read=MM74C923_read;
	SREG=tSREG;
	//
	return mm74c923;
}
char MM74C923_read(void)
{
	uint8_t c,index,lh,hl;
	c=*mm74c923_PIN;
	index=0;
	FUNC func=FUNCenable();
	lh=func.lh(mm74c923_mem,c); // one shot low to high
	hl=func.hl(mm74c923_mem,c);
	mm74c923_mem=c;
	if(lh&(1<<MM74C923_DATA_AVAILABLE)){
		*mm74c923_DDR=(1<<MM74C923_OUTPUT_ENABLE);
		*mm74c923_PORT&=~(1<<MM74C923_OUTPUT_ENABLE);
		c=*mm74c923_PIN;
		if(c&1<<MM74C923_DATA_OUT_A) index|=1; else index&=~1;
		if(c&1<<MM74C923_DATA_OUT_B) index|=2; else index&=~2;
		if(c&1<<MM74C923_DATA_OUT_C) index|=4; else index&=~4;
		if(c&1<<MM74C923_DATA_OUT_D) index|=8; else index&=~8;
		if(c&1<<MM74C923_DATA_OUT_E) index|=16; else index&=~16;
		if(c&1<<MM74C923_EXTRA_DATA_OUT_PIN) index|=32; else index&=~32;
		*mm74c923_DDR&=~(1<<MM74C923_OUTPUT_ENABLE);
		*mm74c923_PORT|=0xFF;
		mm74c923_index=index;
	}else if(hl&(1<<MM74C923_DATA_AVAILABLE)){
		index=52;
		mm74c923_index=index;
	}else{
		index=mm74c923_index;
	}
	return MM74C923_KEY_CODE[index];
}
/*
** interrupt
*/
/***EOF***/
