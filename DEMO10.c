/************************************************************************
Title:    SAMPLE.c
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: MAIN,v 1.8.2.1 2015/04/09 13:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: 
	ATMEGA 128 at 16 Mhz
	leds as output portc
	lcd display 16X2 porta
	servo motor 0 - 180º portb
	using uart1 threw ftdi usb to connect pc
	4 buttons on porte
	using potenciometer portf
	simulation of adc, timer, interrupt, lcd, eeprom, uart0, uart1, i2c, 
	servomotor, ftdi usb, potenciometer, and spi, using ponyprog has 
	programmer software, and Atmega128 by ETT ET-BASE.
	Little else to add on.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
/**********/
#include "function.h"
#include "lcd.h"
//#include "vlcd.h"
#include "uart.h"
//#include "fsm.h"
//#include "vfsm.h"
#include "analog.h"
#include "i2c.h"
#include "timer.h"
#include "spi.h"
#include "eeprom.h"
#include "interrupt.h"
#include "clock.h"
#include "mm74c923.h"
/*********/
#include<string.h>
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
//eeprom space
uint8_t EEMEM NonVolatileChar;
uint16_t EEMEM NonVolatileInt;
uint8_t EEMEM NonVolatileString[16];
INTERRUPT int6;
CLOCK relogio;
/*
** procedure and function header
*/
void PORTINIT();
/****MAIN****/
int main(void)
{
	PORTINIT();
	/***INICIALIZE OBJECTS***/
	FUNC function= FUNCenable();
	LCD0 lcd0 = LCD0enable(&DDRA,&PINA,&PORTA);
	//LCD1 lcd1 = LCD1enable(&DDRC,&PINC,&PORTC);
	MM74C923 keypad = MM74C923enable(&DDRC,&PINC,&PORTC);
	UART1 uart= UART1enable(103,8,1,NONE);//103 para 9600, 68 para 14400
	//I2C i2c = I2Cenable(85, 1);
	ANALOG analog = ANALOGenable(1, 128, 1, 0);
	TIMER_COUNTER3 timer3 = TIMER_COUNTER3enable(0,1);
	TIMER_COUNTER1 timer1 = TIMER_COUNTER1enable(9,0);
	//SPI spi = SPIenable(SPI_MASTER_MODE, SPI_MSB_DATA_ORDER, 0, 8);
	//EEPROM eeprom = EEPROMenable();
	int6=INTERRUPTenable();
	relogio=CLOCKenable(0,0,0);
	/******/
	//eeprom.update_block( "Sergio !!" , (uint8_t*)&NonVolatileString, 16);
	char* keydata;
	count=0;
	char tmp[16];
	char keytmp[24];
	uart.putc('>');
	uart.puts("CODE");
	char entrada;
	timer1.compoutmodeB(2);
	timer1.compareA(20000);
	timer1.start(8);
	timer3.start(1);
	int6.set(6,1);
	/**********/
	while(TRUE){
		//TODO:: Please write your application code
		lcd0.reboot();
		keypad.activate();
		entrada=keypad.getch();
		//lcd1.reboot();
		//TODO:: Please write your application code
		if(entrada=='L')
			relogio.second_count_reset();
		if(entrada=='2')
			relogio.second_count_stop();
		lcd0.gotoxy(0,0);
		lcd0.string(keypad.gets());
		//lcd0.gotoxy(4,0);
		//function.itoa(PINC,tmp);
		//lcd0.string(function.resizestr(tmp,3));
		//lcd1.gotoxy(0,0);
		//lcd1.string("Ola Sergio");
		//lcd0.gotoxy(13,0);
		//a=function.trimmer(analog.read(0),0,1023,0,255);
		a=function.trimmer(function.strToInt(keytmp),0,1023,0,255);
		if(!strcmp(keypad.data(),""));
		else
			strcpy(keytmp,keypad.data());
		timer1.compareB(function.trimmer(function.strToInt(keytmp),0,1023,450,2450));
		//timer1.compareB(function.trimmer(analog.read(0),0,1023,450,2450));
		//function.itoa(a,tmp);
		//lcd0.string(function.resizestr(tmp,5));
		lcd0.gotoxy(0,1);
		lcd0.string(relogio.show());
		//lcd0.gotoxy(12,1);
		//function.itoa(analog.read(2),tmp);
		//lcd0.string(function.resizestr(tmp,4));
		//spi.fast_shift(10);
		/***/
		lcd0.gotoxy(12,1);
		//lcd0.string(function.resizestr(uart.read(),12));
		if(relogio.second_count(210)==1)
			lcd0.string("WALA");
		else
			lcd0.string(function.resizestr(uart.read(),12));
		//eeprom.read_block((char*)tmp, (uint8_t*)NonVolatileString, 16);
		//lcd0.string(function.resizestr(tmp,12));
		/***/
		//i2c.start(TWI_MASTER_MODE);
		//i2c.master_connect(10,TWI_WRITE);
		//i2c.master_write('h');
		//i2c.stop();
	} 
}
/*
** procedure and function
*/
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
/*
** interrupt
*/
ISR(TIMER3_OVF_vect) // TIMER0_COMP_vect
{
	//PORTC=memoria_1[count];
	TIMER0_COMPARE_MATCH++;
	if(TIMER0_COMPARE_MATCH > a){
		TIMER0_COMPARE_MATCH=0;
		count++;
		relogio.increment();
	}
	if(count == vector_size)
		count=0;
}
ISR(INT6_vect)
{
	int6.set(6,3);
	PORTC=0XFF;
}
/***EOF***/
