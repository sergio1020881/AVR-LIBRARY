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
volatile uint8_t *lcd_DDR;
volatile uint8_t *lcd_PIN;
volatile uint8_t *lcd_PORT;
uint8_t lcd_detect;
/*
** procedure and function header
*/
unsigned int LCD_ticks(unsigned int num);
void LCD_inic(void);
void LCD_write(char c, unsigned short D_I);
char LCD_read(unsigned short D_I);
void LCD_BF(void);
void LCD_putch(char c);
char LCD_getch(void);
void LCD_string(const char* s);
void LCD_clear(void);
void LCD_gotoxy(unsigned int x, unsigned int y);
void LCD_strobe(unsigned int num);
void LCD_reboot(void);
/*
** procedure and function
*/
LCD LCDenable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	struct display lcd;
	//import parametros
	lcd_DDR=ddr;
	lcd_PIN=pin;
	lcd_PORT=port;
	//inic variables
	*lcd_DDR=0x00;
	*lcd_PORT=0xFF;
	lcd_detect=*lcd_PIN & (1<<NC);
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
	lcd.inic();
	SREG=tSREG;
	//
	return lcd;
}
void LCD_inic(void)
{
	//LCD INIC
	*lcd_DDR=(1<<RS)|(1<<RW)|(1<<EN)|(0<<NC);
	*lcd_PORT=(1<<NC);
	/***INICIALIZACAO LCD**datasheet*/
	_delay_ms(40);
	LCD_write(0x33,INST); //function set
	//_delay_ms(40);
	//lcd->write(lcd,0x33,INST); //function set
	_delay_us(39);
	LCD_write(0x2B,INST); //function set
	_delay_us(39);
	LCD_write(0x2B,INST); //function set
	//_delay_us(39);
	//lcd->write(0x2B,INST); //function set
	_delay_us(37);
	LCD_write(0x0C,INST);// display on/off control
	_delay_us(37);
	LCD_write(0x01,INST);// clear display
	_delay_ms(1.53);
	LCD_write(0x06,INST);// entry mode set (crazy settings)
	_delay_us(37);
	/***INICIALIZATION END***/
	LCD_write(0x1F,INST);// cursor or display shift
	LCD_BF();
	LCD_write(0x03,INST);// return home
	LCD_BF();
}
void LCD_write(char c, unsigned short D_I)
{
	*lcd_PORT&=~(1<<RW);//lcd as input WRITE INSTRUCTION
	if(D_I==0) *lcd_PORT&=~(1<<D_I); else *lcd_PORT|=(1<<RS);
	*lcd_DDR|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//mcu as output
	*lcd_PORT|=(1<<EN);
	if(c & 0x80) *lcd_PORT|=1<<DB3; else *lcd_PORT&=~(1<<DB3);
	if(c & 0x40) *lcd_PORT|=1<<DB2; else *lcd_PORT&=~(1<<DB2);
	if(c & 0x20) *lcd_PORT|=1<<DB1; else *lcd_PORT&=~(1<<DB1);
	if(c & 0x10) *lcd_PORT|=1<<DB0; else *lcd_PORT&=~(1<<DB0);
	*lcd_PORT&=~(1<<EN);
	LCD_ticks(1);
	*lcd_PORT|=(1<<EN);
	if(c & 0x08) *lcd_PORT|=1<<DB3; else *lcd_PORT&=~(1<<DB3);
	if(c & 0x04) *lcd_PORT|=1<<DB2; else *lcd_PORT&=~(1<<DB2);
	if(c & 0x02) *lcd_PORT|=1<<DB1; else *lcd_PORT&=~(1<<DB1);
	if(c & 0x01) *lcd_PORT|=1<<DB0; else *lcd_PORT&=~(1<<DB0);
	*lcd_PORT&=~(1<<EN);
	LCD_ticks(1);
}
char LCD_read(unsigned short D_I)
{
	char c=0x00;
	*lcd_DDR&=~((1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3));//mcu as input
	*lcd_PORT|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//pullup resistors
	*lcd_PORT|=(1<<RW);//lcd as output READ INSTRUCTION
	if(D_I==0) *lcd_PORT&=~(1<<D_I); else *lcd_PORT|=(1<<RS);
	*lcd_PORT|=(1<<EN);
	if(*lcd_PIN & (1<<DB3)) c|=1<<7; else c&=~(1<<7);
	if(*lcd_PIN & (1<<DB2)) c|=1<<6; else c&=~(1<<6);
	if(*lcd_PIN & (1<<DB1)) c|=1<<5; else c&=~(1<<5);
	if(*lcd_PIN & (1<<DB0)) c|=1<<4; else c&=~(1<<4);
	*lcd_PORT&=~(1<<EN);
	LCD_ticks(1);
	*lcd_PORT|=(1<<EN);
	if(*lcd_PIN & (1<<DB3)) c|=1<<3; else c&=~(1<<3);
	if(*lcd_PIN & (1<<DB2)) c|=1<<2; else c&=~(1<<2);
	if(*lcd_PIN & (1<<DB1)) c|=1<<1; else c&=~(1<<1);
	if(*lcd_PIN & (1<<DB0)) c|=1<<0; else c&=~(1<<0);
	*lcd_PORT&=~(1<<EN);
	LCD_ticks(1);
	return c;
}
void LCD_BF(void)
{
	unsigned int i;
	char inst=0x80;
	for(i=0;0x80&inst;i++){
		inst=LCD_read(INST);
		LCD_ticks(1);
		if(i>10)// if something goes wrong
			break;
	}
}
void LCD_putch(char c)
{
	LCD_write(c,DATA);
	LCD_BF();
}
char LCD_getch(void)
{
	char c;
	c=LCD_read(DATA);
	LCD_BF();
	return c;
}
void LCD_string(const char* s)
{
	char tmp;
	while(*s){
		tmp=*(s++);
		LCD_write(tmp,DATA);
		LCD_BF();
	}
}
void LCD_clear(void)
{
	LCD_write(0x01,INST);
	LCD_BF();
}
void LCD_gotoxy(unsigned int x, unsigned int y)
{
	switch(y){
		case 0:
			LCD_write((0x80+x),INST);
			LCD_BF();
			break;
		case 1:
			LCD_write((0xC0+x),INST);
			LCD_BF();
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
void LCD_strobe(unsigned int num)
{
	*lcd_PORT|=(1<<EN);
	LCD_ticks(num);
	*lcd_PORT&=~(1<<EN);
}
void LCD_reboot(void)
{
	//low high detect pin NC
	uint8_t i;
	uint8_t tmp;
	tmp=*lcd_PIN & (1<<NC);
	i=tmp^lcd_detect;
	i&=tmp;
	if(i)
		LCD_inic();
	lcd_detect=tmp;
}
/*
** interrupt
*/
/***EOF***/
