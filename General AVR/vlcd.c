/*************************************************************************
Title:    VLCD
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: vlcd.c,v 0.2 2014/4/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License
Comment:
	  tested atemga 128 16Mhz, it works but not advised because it 
	  uses stdlib.h malloc for structure and microcontrollers do not 
	  like memory allocation, or mot e~well supported being glichy.
	  Rather use other library it is better and supports two lcd connected.                   
*************************************************************************/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/*
** Library
*/
#include<avr/io.h>
#include<stdlib.h>
#include<util/delay.h>
#include<inttypes.h>
/***/
#include "vlcd.h"
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
/*
** procedure and function header
*/
void VLCD_inic(VLCD this);
void VLCD_write(VLCD this, char c, unsigned short D_I);
char VLCD_read(VLCD this, unsigned short D_I);
void VLCD_BF(VLCD this);
void VLCD_putch(VLCD this, char c);
char VLCD_getch(VLCD this);
void VLCD_string(VLCD this, const char* s);
void VLCD_clear(VLCD this);
void VLCD_gotoxy(VLCD this, unsigned int x, unsigned int y);
void VLCD_strobe(VLCD this, unsigned int num);
void VLCD_reboot(VLCD this);
unsigned int VLCD_ticks(unsigned int num);
/*
** procedure and function
*/
VLCD VLCDenable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	VLCD lcd;
	lcd.parameter=(struct parameter*)malloc(sizeof(struct parameter));
	//import parametros
	lcd.parameter->DDR=ddr;
	lcd.parameter->PIN=pin;
	lcd.parameter->PORT=port;
	lcd.parameter->detect=*lcd.parameter->PIN & (1<<NC);
	//inic variables
	*lcd.parameter->DDR=0x00;
	*lcd.parameter->PORT=0xFF;
	//Direccionar apontadores para PROTOTIPOS
	lcd.write=VLCD_write;
	lcd.read=VLCD_read;
	lcd.BF=VLCD_BF;
	lcd.putch=VLCD_putch;
	lcd.getch=VLCD_getch;
	lcd.string=VLCD_string;
	lcd.clear=VLCD_clear;
	lcd.gotoxy=VLCD_gotoxy;
	lcd.reboot=VLCD_reboot;
	//LCD INIC
	VLCD_inic(lcd);
	SREG=tSREG;
	//
	return lcd;
}
void VLCD_inic(VLCD this)
{
	//LCD INIC
	*this.parameter->DDR=(1<<RS)|(1<<RW)|(1<<EN)|(0<<NC);
	*this.parameter->PORT=(1<<NC);
	/***INICIALIZACAO LCD**datasheet*/
	_delay_ms(40);
	VLCD_write(this,0x33,INST); //function set
	//_delay_ms(40);
	//this.write(lcd,0x33,INST); //function set
	_delay_us(39);
	VLCD_write(this,0x2B,INST); //function set
	_delay_us(39);
	VLCD_write(this,0x2B,INST); //function set
	//_delay_us(39);
	//this.write(0x2B,INST); //function set
	_delay_us(37);
	VLCD_write(this,0x0C,INST);// display on/off control
	_delay_us(37);
	VLCD_write(this,0x01,INST);// clear display
	_delay_ms(1.53);
	VLCD_write(this,0x06,INST);// entry mode set (crazy settings)
	_delay_us(37);
	/***INICIALIZATION END***/
	VLCD_write(this,0x1F,INST);// cursor or display shift
	VLCD_BF(this);
	VLCD_write(this,0x03,INST);// return home
	VLCD_BF(this);
}
void VLCD_write(VLCD this, char c, unsigned short D_I)
{
	*this.parameter->PORT&=~(1<<RW);//lcd as input WRITE INSTRUCTION
	if(D_I==0) *this.parameter->PORT&=~(1<<D_I); else *this.parameter->PORT|=(1<<RS);
	*this.parameter->DDR|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//mcu as output
	*this.parameter->PORT|=(1<<EN);
	if(c & 0x80) *this.parameter->PORT|=1<<DB3; else *this.parameter->PORT&=~(1<<DB3);
	if(c & 0x40) *this.parameter->PORT|=1<<DB2; else *this.parameter->PORT&=~(1<<DB2);
	if(c & 0x20) *this.parameter->PORT|=1<<DB1; else *this.parameter->PORT&=~(1<<DB1);
	if(c & 0x10) *this.parameter->PORT|=1<<DB0; else *this.parameter->PORT&=~(1<<DB0);
	*this.parameter->PORT&=~(1<<EN);
	VLCD_ticks(1);
	*this.parameter->PORT|=(1<<EN);
	if(c & 0x08) *this.parameter->PORT|=1<<DB3; else *this.parameter->PORT&=~(1<<DB3);
	if(c & 0x04) *this.parameter->PORT|=1<<DB2; else *this.parameter->PORT&=~(1<<DB2);
	if(c & 0x02) *this.parameter->PORT|=1<<DB1; else *this.parameter->PORT&=~(1<<DB1);
	if(c & 0x01) *this.parameter->PORT|=1<<DB0; else *this.parameter->PORT&=~(1<<DB0);
	*this.parameter->PORT&=~(1<<EN);
	VLCD_ticks(1);
}
char VLCD_read(VLCD this, unsigned short D_I)
{
	char c=0x00;
	*this.parameter->DDR&=~((1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3));//mcu as input
	*this.parameter->PORT|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//pullup resistors
	*this.parameter->PORT|=(1<<RW);//lcd as output READ INSTRUCTION
	if(D_I==0) *this.parameter->PORT&=~(1<<D_I); else *this.parameter->PORT|=(1<<RS);
	*this.parameter->PORT|=(1<<EN);
	if(*this.parameter->PIN & (1<<DB3)) c|=1<<7; else c&=~(1<<7);
	if(*this.parameter->PIN & (1<<DB2)) c|=1<<6; else c&=~(1<<6);
	if(*this.parameter->PIN & (1<<DB1)) c|=1<<5; else c&=~(1<<5);
	if(*this.parameter->PIN & (1<<DB0)) c|=1<<4; else c&=~(1<<4);
	*this.parameter->PORT&=~(1<<EN);
	VLCD_ticks(1);
	*this.parameter->PORT|=(1<<EN);
	if(*this.parameter->PIN & (1<<DB3)) c|=1<<3; else c&=~(1<<3);
	if(*this.parameter->PIN & (1<<DB2)) c|=1<<2; else c&=~(1<<2);
	if(*this.parameter->PIN & (1<<DB1)) c|=1<<1; else c&=~(1<<1);
	if(*this.parameter->PIN & (1<<DB0)) c|=1<<0; else c&=~(1<<0);
	*this.parameter->PORT&=~(1<<EN);
	VLCD_ticks(1);
	return c;
}
void VLCD_BF(VLCD this)
{
	unsigned int i;
	char inst=0x80;
	for(i=0;0x80&inst;i++){
		inst=VLCD_read(this, INST);
		VLCD_ticks(1);
		if(i>10)// if something goes wrong
			break;
	}
}
void VLCD_putch(VLCD this, char c)
{
	VLCD_write(this, c, DATA);
	VLCD_BF(this);
}
char VLCD_getch(VLCD this)
{
	char c;
	c=VLCD_read(this, DATA);
	VLCD_BF(this);
	return c;
}
void VLCD_string(VLCD this, const char* s)
{
	char tmp;
	while(*s){
		tmp=*(s++);
		VLCD_write(this, tmp, DATA);
		VLCD_BF(this);
	}
}
void VLCD_clear(VLCD this)
{
	VLCD_write(this, 0x01, INST);
	VLCD_BF(this);
}
void VLCD_gotoxy(VLCD this, unsigned int x, unsigned int y)
{
	switch(y){
		case 0:
			VLCD_write(this, (0x80+x), INST);
			VLCD_BF(this);
			break;
		case 1:
			VLCD_write(this, (0xC0+x), INST);
			VLCD_BF(this);
			break;
		default:
			break;
	}
}
void VLCD_strobe(VLCD this, unsigned int num)
{
	*this.parameter->PORT|=(1<<EN);
	VLCD_ticks(num);
	*this.parameter->PORT&=~(1<<EN);
}
void VLCD_reboot(VLCD this)
{
	//low high detect pin NC
	uint8_t i;
	uint8_t tmp;
	tmp=*this.parameter->PIN & (1<<NC);
	i=tmp^this.parameter->detect;
	i&=tmp;
	if(i)
		VLCD_inic(this);
	this.parameter->detect=tmp;
}
unsigned int VLCD_ticks(unsigned int num)
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
