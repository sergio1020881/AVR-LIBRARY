/*************************************************************************
Title:    LCD
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: lcd.c,v 0.2 2014/4/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
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
	  tested atemga 128 16Mhz, Very Stable                    
*************************************************************************/
//atmega 128 at 16MHZ
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

/*
** Library
*/
#include <avr/io.h>
#include <util/delay.h>

/*
** Private Library
*/
#include "lcd.h"

/*
** module constants and macros
*/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif

/*
** module variables
*/
//ticks depends on CPU frequency this case 16Mhz

/*
** module function header
*/
unsigned int LCD_ticks(unsigned int num);

/*
** module interrupt header
*/

/*
** module object 1 constructor
*/
LCD LCDenable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile
uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	
	//PROTOTIPOS
	void LCD_inic(struct display* lcd);
	void LCD_write(struct display* lcd, char c, unsigned short D_I);
	char LCD_read(struct display* lcd, unsigned short D_I);
	void LCD_BF(struct display* lcd);
	void LCD_putch(struct display* lcd, char c);
	char LCD_getch(struct display* lcd);
	void LCD_string(struct display* lcd, const char* s);
	void LCD_clear(struct display* lcd);
	void LCD_gotoxy(struct display* lcd, unsigned int x, unsigned int y);
	void LCD_strobe(struct display* lcd, unsigned int num);
	void LCD_reboot(struct display *lcd);
	
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	struct display lcd;
	
	//import parametros
	lcd.DDR=ddr;
	lcd.PIN=pin;
	lcd.PORT=port;
	//inic parameters
	*lcd.DDR=0x00;
	*lcd.PORT=0xFF;
	//inic variables
	lcd.detect=*lcd.PIN & (1<<NC);
	
	//Direccionar apontadores para PROTOTIPOS
	lcd.inic=LCD_inic;
	lcd.write=LCD_write;
	lcd.read=LCD_read;
	lcd.BF=LCD_BF;
	lcd.putch=LCD_putch;
	lcd.getch=LCD_getch;
	lcd.string=LCD_string;
	lcd.clear=LCD_clear;
	lcd.gotoxy=LCD_gotoxy;
	lcd.ticks=LCD_ticks;
	lcd.strobe=LCD_strobe;
	lcd.reboot=LCD_reboot;
	
	//LCD INIC
	lcd.inic(&lcd);
	
	SREG=tSREG;
	
	//
	return lcd;
}

/*
**  module object 1 procedure and function difinitions
*/
void LCD_inic(struct display* lcd)
{
	//LCD INIC
	*lcd->DDR=(1<<RS)|(1<<RW)|(1<<EN)|(0<<NC);
	*lcd->PORT=(1<<NC);
	
	/***INICIALIZACAO LCD**datasheet*/
	_delay_ms(40);
	lcd->write(lcd,0x33,INST); //function set
	//_delay_ms(40);
	//lcd->write(lcd,0x33,INST); //function set
	_delay_us(39);
	lcd->write(lcd,0x2B,INST); //function set
	_delay_us(39);
	lcd->write(lcd,0x2B,INST); //function set
	//_delay_us(39);
	//lcd->write(lcd,0x2B,INST); //function set
	_delay_us(37);
	lcd->write(lcd,0x0C,INST);// display on/off control
	_delay_us(37);
	lcd->write(lcd,0x01,INST);// clear display
	_delay_ms(1.53);
	lcd->write(lcd,0x06,INST);// entry mode set (crazy settings)
	_delay_us(37);

	/***INICIALIZATION END***/
	lcd->write(lcd,0x1F,INST);// cursor or display shift
	lcd->BF(lcd);
	lcd->write(lcd,0x03,INST);// return home
	lcd->BF(lcd);
}

void LCD_write(struct display* lcd, char c, unsigned short D_I)
{
	*lcd->PORT&=~(1<<RW);//lcd as input WRITE INSTRUCTION
	if(D_I==0) *lcd->PORT&=~(1<<D_I); else *lcd->PORT|=(1<<RS);
	*lcd->DDR|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//mcu as output
	*lcd->PORT|=(1<<EN);
	if(c & 0x80) *lcd->PORT|=1<<DB3; else *lcd->PORT&=~(1<<DB3);
	if(c & 0x40) *lcd->PORT|=1<<DB2; else *lcd->PORT&=~(1<<DB2);
	if(c & 0x20) *lcd->PORT|=1<<DB1; else *lcd->PORT&=~(1<<DB1);
	if(c & 0x10) *lcd->PORT|=1<<DB0; else *lcd->PORT&=~(1<<DB0);
	*lcd->PORT&=~(1<<EN);
	lcd->ticks(1);
	*lcd->PORT|=(1<<EN);
	if(c & 0x08) *lcd->PORT|=1<<DB3; else *lcd->PORT&=~(1<<DB3);
	if(c & 0x04) *lcd->PORT|=1<<DB2; else *lcd->PORT&=~(1<<DB2);
	if(c & 0x02) *lcd->PORT|=1<<DB1; else *lcd->PORT&=~(1<<DB1);
	if(c & 0x01) *lcd->PORT|=1<<DB0; else *lcd->PORT&=~(1<<DB0);
	*lcd->PORT&=~(1<<EN);
	lcd->ticks(1);
}

char LCD_read(struct display* lcd, unsigned short D_I)
{
	char c=0x00;
	*lcd->DDR&=~((1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3));//mcu as input
	*lcd->PORT|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//pullup resistors
	*lcd->PORT|=(1<<RW);//lcd as output READ INSTRUCTION
	if(D_I==0) *lcd->PORT&=~(1<<D_I); else *lcd->PORT|=(1<<RS);
	*lcd->PORT|=(1<<EN);
	if(*lcd->PIN & (1<<DB3)) c|=1<<7; else c&=~(1<<7);
	if(*lcd->PIN & (1<<DB2)) c|=1<<6; else c&=~(1<<6);
	if(*lcd->PIN & (1<<DB1)) c|=1<<5; else c&=~(1<<5);
	if(*lcd->PIN & (1<<DB0)) c|=1<<4; else c&=~(1<<4);
	*lcd->PORT&=~(1<<EN);
	lcd->ticks(1);
	*lcd->PORT|=(1<<EN);
	if(*lcd->PIN & (1<<DB3)) c|=1<<3; else c&=~(1<<3);
	if(*lcd->PIN & (1<<DB2)) c|=1<<2; else c&=~(1<<2);
	if(*lcd->PIN & (1<<DB1)) c|=1<<1; else c&=~(1<<1);
	if(*lcd->PIN & (1<<DB0)) c|=1<<0; else c&=~(1<<0);
	*lcd->PORT&=~(1<<EN);
	lcd->ticks(1);
	return c;
}

void LCD_BF(struct display* lcd)
{
	unsigned int i;
	char inst=0x80;
	for(i=0;0x80&inst;i++){
		inst=lcd->read(lcd,INST);
		lcd->ticks(1);
		if(i>10)// if something goes wrong
			break;
	}
}

void LCD_putch(struct display* lcd, char c)
{
	lcd->write(lcd,c,DATA);
	lcd->BF(lcd);
}

char LCD_getch(struct display* lcd)
{
	char c;
	c=lcd->read(lcd,DATA);
	lcd->BF(lcd);
	return c;
}

void LCD_string(struct display* lcd, const char* s)
{
	char tmp;
	while(*s){
		tmp=*(s++);
		lcd->write(lcd,tmp,DATA);
		lcd->BF(lcd);
	}
}

void LCD_clear(struct display* lcd)
{
	lcd->write(lcd,0x01,INST);
	lcd->BF(lcd);
}

void LCD_gotoxy(struct display* lcd, unsigned int x, unsigned int y)
{
	switch(y){
		case 0:
			lcd->write(lcd,(0x80+x),INST);
			lcd->BF(lcd);
			break;
		case 1:
			lcd->write(lcd,(0xC0+x),INST);
			lcd->BF(lcd);
			break;
		default:
			break;
	}
}

unsigned int LCD_ticks(unsigned int num)
{
	unsigned int count;
	for(count=0;count<num;count++)
		;
	return count;
}

void LCD_strobe(struct display *lcd, unsigned int num)
{
	*lcd->PORT|=(1<<EN);
	lcd->ticks(num);
	*lcd->PORT&=~(1<<EN);
}

void LCD_reboot(struct display *lcd)
{
	//low high detect pin NC
	uint8_t i;
	uint8_t tmp;
	tmp=*lcd->PIN & (1<<NC);
	i=tmp^lcd->detect;
	i&=tmp;
	if(i)
		lcd->inic(lcd);
	lcd->detect=tmp;
}

/*
** module object 1 interrupts
*/

/*
**  module procedure and function difinitions
*/

/*
** module interrupts
*/
/***EOF***/
