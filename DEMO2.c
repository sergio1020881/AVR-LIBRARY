/************************************************************************
Title:    LMACHINE.c
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: MAIN,v 1.8.2.1 2014/04/21 13:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: any AVR with built-in UART, tested on ATMEGA 128 at 16 Mhz
License:  GNU General Public License 
Usage:    see Doxygen manual
LICENSE:
    Copyright (C) 2014 Sergio Santos
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
COMMENT:
	stable
************************************************************************/
#define F_CPU 16000000UL
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
/***/
#include "function.h"
#include "lcd.h"
#include "uart.h"
#include "fsm.h"
#include "vfsm.h"
#include "analog.h"
#include "i2c.h"
#include "timer.h"
#include "spi.h"
/*
** constant and macro
*/
#define TRUE 1
#define FALSE 0
#define GI 7
#define vector_size 16
/*
** variable
*/
uint8_t memoria_1[3*vector_size]={
//output,	input,		change
1,			0,			16,
16,			1,			2,
2,			2,			128,
128,			3,			8,
8,			4,			4,
4,			5,			64,
64,			6,			32,
32,			7,			33,
33,			8,			80,
80,			9,			6,
6,			10,			136,
136,			11,			8,
8,			12,			4,
4,			13,			64,
64,			14,			32,
32,			15,			1
};
unsigned int TIMER0_COMPARE_MATCH;
uint8_t count;
int a;
/*
** procedure and funtion header
*/
void PORTINIT();
int trigger(int multipler);
/*
** procedure and function
*/
/*MAIN*/
int main(void)
{
	PORTINIT();
	/***INICIALIZE OBJECTS***/
	FUNC function= FUNCenable();
	LCD lcd = LCDenable(&DDRA,&PINA,&PORTA);
	UART1 uart= UART1enable(103,8,1,NONE);//103 para 9600, 68 para 14400
	VLOGIC timer_0 = VLOGICenable(memoria_1,3*vector_size);
	I2C i2c = I2Cenable(85, 1);
	ANALOG analog = ANALOGenable(1, 128, 3, 0, 4, 7);
	TIMER0 timer0 = TIMER0enable(0,2);
	SPI spi = SPIenable(SPI_MASTER_MODE, SPI_MSB_DATA_ORDER, 0, 8);
	uart.puts("OLA SERGIO !!");
	/******/
	count=0;
	PORTC=1;
	uint8_t mask=31;
	char tmp[16];
	int entrada;
	uint8_t vmfsm[4];
	timer0.start(255, 8);
	/**********/
	while(TRUE){
		lcd.reboot();
		//ignore cancel stop enter delete run
		//TODO:: Please write your application code
		entrada=PINE;
		lcd.gotoxy(0,0);
		function.itoa(entrada,tmp);
		lcd.string(function.resizestr(tmp,3));
		lcd.gotoxy(4,0);
		function.itoa(PORTC,tmp);
		lcd.string(function.resizestr(tmp,3));
		lcd.gotoxy(12,0);
		a=analog.read(0);
		function.itoa(a,tmp);
		lcd.string(function.resizestr(tmp,4));
		lcd.gotoxy(12,1);
		function.itoa(analog.read(2),tmp);
		lcd.string(function.resizestr(tmp,4));
		vmfsm[0]=timer_0.read(&timer_0, mask, count, (PINC & 255));
		PORTC =	vmfsm[0];
		spi.fast_shift(vmfsm[0]);
		/***/
		lcd.gotoxy(0,1);
		lcd.string(function.resizestr(uart.read(),12));
		/***/
		i2c.start(TWI_MASTER_MODE);
		i2c.master_connect(10,TWI_WRITE);
		i2c.master_write('h');
		i2c.stop();
	} 
}
void PORTINIT()
{
	//INPUT
	DDRF=0x00;
	PORTF=0x0F;
	DDRE=0X00;
	PORTE=0XFF;
	DDRD=0X00;
	PORTD=0XFF;
	//OUTPUT
	DDRC=0XFF;
	PORTC=0x00;
	//UART0
	//DDRE=0X02;
	SREG|=(1<<GI);
}
int trigger(int multiplier)
{
	int ret=0;
	if(TIMER0_COMPARE_MATCH > multiplier){
		ret=1;
	}else{
		ret=0;
	}	
	return ret;
}
/*
** interrupt
*/
ISR(TIMER0_COMP_vect)
{
	TIMER0_COMPARE_MATCH++;
	if(TIMER0_COMPARE_MATCH > a){
		TIMER0_COMPARE_MATCH=0;
		count++;
	}
	if(count == vector_size)
		count=0;	
}
/***EOF***/
