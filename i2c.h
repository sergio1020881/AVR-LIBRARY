/************************************************************************
Title:    I2C library
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: analog.h,v 0.2 2014/04/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
License:  GNU General Public License
DESCRIPTION:
	Atmega 128 at 16MHZ 
USAGE:    
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
	testing phase
************************************************************************/
#ifndef _I2C_H
	#define _I2C_H   1
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
// devices
#define DS1307_ID 0xD0			// I2C DS1307 Device Identifier
#define DS1307_ADDR 0X00		// I2C DS1307 Device Address offset
#define TC74_ID 0x9A			// device address of TC74
#define TC74_ADDR 0X00
#define Dev24C02_ID 0xA2		//device address 24C02
#define Dev24C02_ADDR 0x00
#define LM73_ID 0x90			//LM73 address temperature sensor
/***/
#define TWI_WRITE 0
#define TWI_READ 1
#define TWI_ACK 1
#define TWI_NACK 0
#define TWI_MASTER_MODE 0
#define TWI_SLAVE_MODE 1
/*
** variable
*/
unsigned char i2c_output;
struct I2C{
	/***PROTOTYPES VTABLE***/
	void (*start)(unsigned char mode);
	void (*connect)(unsigned char addr, unsigned char rw);
	void (*write)(unsigned char data);
	unsigned char (*read)(unsigned char request);
	void (*stop)(void);
};
typedef struct I2C I2C;
/*
** procedure and function header
*/
I2C I2Cenable(unsigned char device_id, unsigned char prescaler);
#endif
/***EOF***/
