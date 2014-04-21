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
/*
** variable
*/
//B1
uint8_t memoria_1[18]={
//output,	input,		change
5,			0,			10,
2,			0,			6,
1,			0,			5,
6,			0,			9,
10,			0,			6,
9,			0,			5
};
//M1B1-3
uint8_t memoria_2[3]={
//output,	input,	change
0,			0,			0
};
//M2B1-5
uint8_t memoria_3[3]={
//output,	input,	change
0,			0,			0
};
//M2M1B1-5
uint8_t memoria_4[12]={
//output,	input,	change
0,			0,			0
};
//M1-2
uint8_t memoria_5[3]={
//output,	input,		change
5,			0,			2	// 3
};
//M2-4
uint8_t memoria_6[3]={
//output,	input,		change
6,			0,			1	// 5
};
//TIMER-1
uint8_t memoria_7[9]={
//output,	input,		change
0,			1,			5,
10,			1,			5,
9,			1,			5
};
unsigned int TIMER0_COMPARE_MATCH;
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
	VFSM button_1 = VFSMenable(memoria_1,18);
	VFSM button_2 = VFSMenable(memoria_2,3);
	VFSM button_3 = VFSMenable(memoria_3,3);
	VFSM button_4 = VFSMenable(memoria_4,3);
	VLOGIC button_5 = VLOGICenable(memoria_5,3);
	VLOGIC button_6 = VLOGICenable(memoria_6,3);
	VFSM button_7 = VFSMenable(memoria_7,9);
	I2C i2c = I2Cenable(85, 1);
	ANALOG analog = ANALOGenable(1, 128, 3, 0, 4, 7);
	TIMER0 timer0 = TIMER0enable(0,0,2);
	SPI spi = SPIenable(SPI_MASTER_MODE, SPI_MSB_DATA_ORDER, 0, 8);
	uart.puts("OLA SERGIO !!");
	/******/
	int analog_value;
	char tmp[16];
	int entrada;
	uint8_t vmfsm[7];
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
		analog_value=analog.read(0);
		function.itoa(analog_value,tmp);
		lcd.string(function.resizestr(tmp,4));
		lcd.gotoxy(12,1);
		function.itoa(analog.read(2),tmp);
		lcd.string(function.resizestr(tmp,4));
		vmfsm[0]=button_1.read(&button_1, 1, entrada, (PINC & 15));//1
		vmfsm[1]=button_2.read(&button_2, 3, entrada, vmfsm[0]);//3
		vmfsm[2]=button_3.read(&button_3, 5, entrada, vmfsm[1]);//5
		vmfsm[3]=button_4.read(&button_4, 7, entrada, vmfsm[2]);//2
		vmfsm[4]=button_5.read(&button_5, 2, entrada, vmfsm[3]);//4
		vmfsm[5]=button_6.read(&button_6, 4, entrada, vmfsm[4]);//4
		vmfsm[6]=button_7.read(&button_7, 1, trigger(analog_value), vmfsm[5]);
		PORTC =	vmfsm[6];
		spi.fast_shift(vmfsm[6]);
		//trigger(analog_value)
		/***TIMER***/
		if(vmfsm[6]==10 || vmfsm[6]==9 || vmfsm[6]==0){
			timer0.start(255, 1024);
		}else{
			TIMER0_COMPARE_MATCH=0;
			timer0.stop();
		}
		lcd.gotoxy(0,1);
		lcd.string(function.resizestr(uart.read(),12));
		/***/
		i2c.start(TWI_MASTER_MODE);
		i2c.connect(10,TWI_WRITE);
		i2c.write('h');
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
}
/***EOF***/
