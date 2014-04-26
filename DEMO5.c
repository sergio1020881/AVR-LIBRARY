/************************************************************************
Title:    DEMO5.c
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: MAIN,v 1.8.2.1 2014/04/21 13:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: 
	ATMEGA 128 at 16 Mhz
	leds as output portc
	lcd display 16X2 porta
	servo motor 0 - 180º portb
	using uart1 threw ftdi usb to connect pc
	4 buttons on porte
	using potenciometer portf
License:  GNU General Public License
Usage:    see Doxygen manual
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
//#include "fsm.h"
//#include "vfsm.h"
#include "analog.h"
#include "i2c.h"
#include "timer.h"
#include "spi.h"
#include "eeprom.h"
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
uint8_t memoria_1[16]={
1,16,2,128,8,4,64,32,33,80,6,136,8,4,64,32
};
unsigned int TIMER0_COMPARE_MATCH;
uint8_t count;
int a;
uint8_t EEMEM NonVolatileChar;
uint16_t EEMEM NonVolatileInt;
uint8_t EEMEM NonVolatileString[16];
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
	LCD0 lcd0 = LCD0enable(&DDRA,&PINA,&PORTA);
	//LCD1 lcd1 = LCD1enable(&DDRC,&PINC,&PORTC);
	UART1 uart= UART1enable(103,8,1,NONE);//103 para 9600, 68 para 14400
	I2C i2c = I2Cenable(85, 1);
	ANALOG analog = ANALOGenable(1, 128, 3, 0, 4, 7);
	TIMER_COUNTER3 timer3 = TIMER_COUNTER3enable(0,1);
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(9,0);
	//SPI spi = SPIenable(SPI_MASTER_MODE, SPI_MSB_DATA_ORDER, 0, 8);
	//EEPROM eeprom = EEPROMenable();
	/******/
	count=0;
	char tmp[16];
	uart.putc('>');
	uart.puts("SERGIO CODE");
	int entrada;
	timer1.compoutmodeB(2);
	timer1.compareA(20000);
	timer1.start(8);
	timer3.start(1);
	/**********/
	while(TRUE){
		lcd0.reboot();
		//lcd1.reboot();
		//TODO:: Please write your application code
		entrada=PINE;
		lcd0.gotoxy(0,0);
		function.itoa(entrada,tmp);
		lcd0.string(function.resizestr(tmp,3));
		lcd0.gotoxy(4,0);
		function.itoa(PORTC,tmp);
		lcd0.string(function.resizestr(tmp,3));
		//lcd1.gotoxy(0,0);
		//lcd1.string("Ola Sergio");
		lcd0.gotoxy(11,0);
		a=function.trimmer(analog.read(0),0,1023,0,255);
		timer1.compareB(function.trimmer(analog.read(0),0,1023,450,2400));
		function.itoa(a,tmp);
		lcd0.string(function.resizestr(tmp,5));
		lcd0.gotoxy(12,1);
		function.itoa(analog.read(2),tmp);
		lcd0.string(function.resizestr(tmp,4));
		//spi.fast_shift(10);
		/***/
		lcd0.gotoxy(0,1);
		lcd0.string(function.resizestr(uart.read(),12));
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
	DDRB|=(1<<5) | (1<<6) | (1<<7);
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
ISR(TIMER3_OVF_vect) // TIMER0_COMP_vect
{
	PORTC=memoria_1[count];
	TIMER0_COMPARE_MATCH++;
	if(TIMER0_COMPARE_MATCH > a){
		TIMER0_COMPARE_MATCH=0;
		count++;
	}
	if(count == vector_size)
		count=0;
	
}
/***EOF***/
