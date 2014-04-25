/*************************************************************************
Title:    LCD
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: lcd.c,v 0.2 2014/4/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
	Atemga 128 at 16Mhz
USAGE:
    Refere to the header file lcd.h for a description of the routines.
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
#include "lcd.h"
/*
** constant and macro
*/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
//CMD RS
#define INST 0
#define DATA 1
/*
** variable
*/
//ticks depends on CPU frequency this case 16Mhz
volatile uint8_t *lcd0_DDR;
volatile uint8_t *lcd0_PIN;
volatile uint8_t *lcd0_PORT;
uint8_t lcd0_detect;
volatile uint8_t *lcd1_DDR;
volatile uint8_t *lcd1_PIN;
volatile uint8_t *lcd1_PORT;
uint8_t lcd1_detect;
/*
** procedure and function header
*/
void LCD0_inic(void);
void LCD0_write(char c, unsigned short D_I);
char LCD0_read(unsigned short D_I);
void LCD0_BF(void);
void LCD0_putch(char c);
char LCD0_getch(void);
void LCD0_string(const char* s);
void LCD0_clear(void);
void LCD0_gotoxy(unsigned int x, unsigned int y);
void LCD0_strobe(unsigned int num);
void LCD0_reboot(void);
void LCD1_inic(void);
void LCD1_write(char c, unsigned short D_I);
char LCD1_read(unsigned short D_I);
void LCD1_BF(void);
void LCD1_putch(char c);
char LCD1_getch(void);
void LCD1_string(const char* s);
void LCD1_clear(void);
void LCD1_gotoxy(unsigned int x, unsigned int y);
void LCD1_strobe(unsigned int num);
void LCD1_reboot(void);
unsigned int LCD_ticks(unsigned int num);
/*
** procedure and function
*/
LCD0 LCD0enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	LCD0 lcd0;
	//import parametros
	lcd0_DDR=ddr;
	lcd0_PIN=pin;
	lcd0_PORT=port;
	//inic variables
	*lcd0_DDR=0x00;
	*lcd0_PORT=0xFF;
	lcd0_detect=*lcd0_PIN & (1<<NC);
	//Direccionar apontadores para PROTOTIPOS
	lcd0.write=LCD0_write;
	lcd0.read=LCD0_read;
	lcd0.BF=LCD0_BF;
	lcd0.putch=LCD0_putch;
	lcd0.getch=LCD0_getch;
	lcd0.string=LCD0_string;
	lcd0.clear=LCD0_clear;
	lcd0.gotoxy=LCD0_gotoxy;
	lcd0.reboot=LCD0_reboot;
	//LCD INIC
	LCD0_inic();
	SREG=tSREG;
	//
	return lcd0;
}
void LCD0_inic(void)
{
	//LCD INIC
	*lcd0_DDR=(1<<RS)|(1<<RW)|(1<<EN)|(0<<NC);
	*lcd0_PORT=(1<<NC);
	/***INICIALIZACAO LCD**datasheet*/
	_delay_ms(40);
	LCD0_write(0x33,INST); //function set
	//_delay_ms(40);
	//lcd->write(lcd,0x33,INST); //function set
	_delay_us(39);
	LCD0_write(0x2B,INST); //function set
	_delay_us(39);
	LCD0_write(0x2B,INST); //function set
	//_delay_us(39);
	//lcd->write(0x2B,INST); //function set
	_delay_us(37);
	LCD0_write(0x0C,INST);// display on/off control
	_delay_us(37);
	LCD0_write(0x01,INST);// clear display
	_delay_ms(1.53);
	LCD0_write(0x06,INST);// entry mode set (crazy settings)
	_delay_us(37);
	/***INICIALIZATION END***/
	LCD0_write(0x1F,INST);// cursor or display shift
	LCD0_BF();
	LCD0_write(0x03,INST);// return home
	LCD0_BF();
}
void LCD0_write(char c, unsigned short D_I)
{
	*lcd0_PORT&=~(1<<RW);//lcd as input WRITE INSTRUCTION
	if(D_I==0) *lcd0_PORT&=~(1<<D_I); else *lcd0_PORT|=(1<<RS);
	*lcd0_DDR|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//mcu as output
	*lcd0_PORT|=(1<<EN);
	if(c & 0x80) *lcd0_PORT|=1<<DB3; else *lcd0_PORT&=~(1<<DB3);
	if(c & 0x40) *lcd0_PORT|=1<<DB2; else *lcd0_PORT&=~(1<<DB2);
	if(c & 0x20) *lcd0_PORT|=1<<DB1; else *lcd0_PORT&=~(1<<DB1);
	if(c & 0x10) *lcd0_PORT|=1<<DB0; else *lcd0_PORT&=~(1<<DB0);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(1);
	*lcd0_PORT|=(1<<EN);
	if(c & 0x08) *lcd0_PORT|=1<<DB3; else *lcd0_PORT&=~(1<<DB3);
	if(c & 0x04) *lcd0_PORT|=1<<DB2; else *lcd0_PORT&=~(1<<DB2);
	if(c & 0x02) *lcd0_PORT|=1<<DB1; else *lcd0_PORT&=~(1<<DB1);
	if(c & 0x01) *lcd0_PORT|=1<<DB0; else *lcd0_PORT&=~(1<<DB0);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(1);
}
char LCD0_read(unsigned short D_I)
{
	char c=0x00;
	*lcd0_DDR&=~((1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3));//mcu as input
	*lcd0_PORT|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//pullup resistors
	*lcd0_PORT|=(1<<RW);//lcd as output READ INSTRUCTION
	if(D_I==0) *lcd0_PORT&=~(1<<D_I); else *lcd0_PORT|=(1<<RS);
	*lcd0_PORT|=(1<<EN);
	if(*lcd0_PIN & (1<<DB3)) c|=1<<7; else c&=~(1<<7);
	if(*lcd0_PIN & (1<<DB2)) c|=1<<6; else c&=~(1<<6);
	if(*lcd0_PIN & (1<<DB1)) c|=1<<5; else c&=~(1<<5);
	if(*lcd0_PIN & (1<<DB0)) c|=1<<4; else c&=~(1<<4);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(1);
	*lcd0_PORT|=(1<<EN);
	if(*lcd0_PIN & (1<<DB3)) c|=1<<3; else c&=~(1<<3);
	if(*lcd0_PIN & (1<<DB2)) c|=1<<2; else c&=~(1<<2);
	if(*lcd0_PIN & (1<<DB1)) c|=1<<1; else c&=~(1<<1);
	if(*lcd0_PIN & (1<<DB0)) c|=1<<0; else c&=~(1<<0);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(1);
	return c;
}
void LCD0_BF(void)
{
	unsigned int i;
	char inst=0x80;
	for(i=0;0x80&inst;i++){
		inst=LCD0_read(INST);
		LCD_ticks(1);
		if(i>10)// if something goes wrong
			break;
	}
}
void LCD0_putch(char c)
{
	LCD0_write(c,DATA);
	LCD0_BF();
}
char LCD0_getch(void)
{
	char c;
	c=LCD0_read(DATA);
	LCD0_BF();
	return c;
}
void LCD0_string(const char* s)
{
	char tmp;
	while(*s){
		tmp=*(s++);
		LCD0_write(tmp,DATA);
		LCD0_BF();
	}
}
void LCD0_clear(void)
{
	LCD0_write(0x01,INST);
	LCD0_BF();
}
void LCD0_gotoxy(unsigned int x, unsigned int y)
{
	switch(y){
		case 0:
			LCD0_write((0x80+x),INST);
			LCD0_BF();
			break;
		case 1:
			LCD0_write((0xC0+x),INST);
			LCD0_BF();
			break;
		default:
			break;
	}
}
void LCD0_strobe(unsigned int num)
{
	*lcd0_PORT|=(1<<EN);
	LCD_ticks(num);
	*lcd0_PORT&=~(1<<EN);
}
void LCD0_reboot(void)
{
	//low high detect pin NC
	uint8_t i;
	uint8_t tmp;
	tmp=*lcd0_PIN & (1<<NC);
	i=tmp^lcd0_detect;
	i&=tmp;
	if(i)
		LCD0_inic();
	lcd0_detect=tmp;
}
/*******************************************************************/
LCD1 LCD1enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	LCD1 lcd1;
	//import parametros
	lcd1_DDR=ddr;
	lcd1_PIN=pin;
	lcd1_PORT=port;
	//inic variables
	*lcd1_DDR=0x00;
	*lcd1_PORT=0xFF;
	lcd1_detect=*lcd1_PIN & (1<<NC);
	//Direccionar apontadores para PROTOTIPOS
	lcd1.write=LCD1_write;
	lcd1.read=LCD1_read;
	lcd1.BF=LCD1_BF;
	lcd1.putch=LCD1_putch;
	lcd1.getch=LCD1_getch;
	lcd1.string=LCD1_string;
	lcd1.clear=LCD1_clear;
	lcd1.gotoxy=LCD1_gotoxy;
	lcd1.reboot=LCD1_reboot;
	//LCD INIC
	LCD1_inic();
	SREG=tSREG;
	//
	return lcd1;
}
void LCD1_inic(void)
{
	//LCD INIC
	*lcd1_DDR=(1<<RS)|(1<<RW)|(1<<EN)|(0<<NC);
	*lcd1_PORT=(1<<NC);
	/***INICIALIZACAO LCD**datasheet*/
	_delay_ms(40);
	LCD1_write(0x33,INST); //function set
	//_delay_ms(40);
	//lcd->write(lcd,0x33,INST); //function set
	_delay_us(39);
	LCD1_write(0x2B,INST); //function set
	_delay_us(39);
	LCD1_write(0x2B,INST); //function set
	//_delay_us(39);
	//lcd->write(0x2B,INST); //function set
	_delay_us(37);
	LCD1_write(0x0C,INST);// display on/off control
	_delay_us(37);
	LCD1_write(0x01,INST);// clear display
	_delay_ms(1.53);
	LCD1_write(0x06,INST);// entry mode set (crazy settings)
	_delay_us(37);
	/***INICIALIZATION END***/
	LCD1_write(0x1F,INST);// cursor or display shift
	LCD1_BF();
	LCD1_write(0x03,INST);// return home
	LCD1_BF();
}
void LCD1_write(char c, unsigned short D_I)
{
	*lcd1_PORT&=~(1<<RW);//lcd as input WRITE INSTRUCTION
	if(D_I==0) *lcd1_PORT&=~(1<<D_I); else *lcd1_PORT|=(1<<RS);
	*lcd1_DDR|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//mcu as output
	*lcd1_PORT|=(1<<EN);
	if(c & 0x80) *lcd1_PORT|=1<<DB3; else *lcd1_PORT&=~(1<<DB3);
	if(c & 0x40) *lcd1_PORT|=1<<DB2; else *lcd1_PORT&=~(1<<DB2);
	if(c & 0x20) *lcd1_PORT|=1<<DB1; else *lcd1_PORT&=~(1<<DB1);
	if(c & 0x10) *lcd1_PORT|=1<<DB0; else *lcd1_PORT&=~(1<<DB0);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(1);
	*lcd1_PORT|=(1<<EN);
	if(c & 0x08) *lcd1_PORT|=1<<DB3; else *lcd1_PORT&=~(1<<DB3);
	if(c & 0x04) *lcd1_PORT|=1<<DB2; else *lcd1_PORT&=~(1<<DB2);
	if(c & 0x02) *lcd1_PORT|=1<<DB1; else *lcd1_PORT&=~(1<<DB1);
	if(c & 0x01) *lcd1_PORT|=1<<DB0; else *lcd1_PORT&=~(1<<DB0);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(1);
}
char LCD1_read(unsigned short D_I)
{
	char c=0x00;
	*lcd1_DDR&=~((1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3));//mcu as input
	*lcd1_PORT|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//pullup resistors
	*lcd1_PORT|=(1<<RW);//lcd as output READ INSTRUCTION
	if(D_I==0) *lcd1_PORT&=~(1<<D_I); else *lcd1_PORT|=(1<<RS);
	*lcd1_PORT|=(1<<EN);
	if(*lcd1_PIN & (1<<DB3)) c|=1<<7; else c&=~(1<<7);
	if(*lcd1_PIN & (1<<DB2)) c|=1<<6; else c&=~(1<<6);
	if(*lcd1_PIN & (1<<DB1)) c|=1<<5; else c&=~(1<<5);
	if(*lcd1_PIN & (1<<DB0)) c|=1<<4; else c&=~(1<<4);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(1);
	*lcd1_PORT|=(1<<EN);
	if(*lcd1_PIN & (1<<DB3)) c|=1<<3; else c&=~(1<<3);
	if(*lcd1_PIN & (1<<DB2)) c|=1<<2; else c&=~(1<<2);
	if(*lcd1_PIN & (1<<DB1)) c|=1<<1; else c&=~(1<<1);
	if(*lcd1_PIN & (1<<DB0)) c|=1<<0; else c&=~(1<<0);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(1);
	return c;
}
void LCD1_BF(void)
{
	unsigned int i;
	char inst=0x80;
	for(i=0;0x80&inst;i++){
		inst=LCD1_read(INST);
		LCD_ticks(1);
		if(i>10)// if something goes wrong
			break;
	}
}
void LCD1_putch(char c)
{
	LCD1_write(c,DATA);
	LCD1_BF();
}
char LCD1_getch(void)
{
	char c;
	c=LCD1_read(DATA);
	LCD1_BF();
	return c;
}
void LCD1_string(const char* s)
{
	char tmp;
	while(*s){
		tmp=*(s++);
		LCD1_write(tmp,DATA);
		LCD1_BF();
	}
}
void LCD1_clear(void)
{
	LCD1_write(0x01,INST);
	LCD1_BF();
}
void LCD1_gotoxy(unsigned int x, unsigned int y)
{
	switch(y){
		case 0:
			LCD1_write((0x80+x),INST);
			LCD1_BF();
			break;
		case 1:
			LCD1_write((0xC0+x),INST);
			LCD1_BF();
			break;
		default:
			break;
	}
}
void LCD1_strobe(unsigned int num)
{
	*lcd1_PORT|=(1<<EN);
	LCD_ticks(num);
	*lcd1_PORT&=~(1<<EN);
}
void LCD1_reboot(void)
{
	//low high detect pin NC
	uint8_t i;
	uint8_t tmp;
	tmp=*lcd1_PIN & (1<<NC);
	i=tmp^lcd1_detect;
	i&=tmp;
	if(i)
		LCD1_inic();
	lcd1_detect=tmp;
}
unsigned int LCD_ticks(unsigned int num)
{
	unsigned int count;
	for(count=0;count<num;count++)
		;
	return count;
}
/*
** interrupt
*/
/***EOF***/
