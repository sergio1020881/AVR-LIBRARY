/*************************************************************************
Title:    SPI
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: spi.h,v 0.2 2014/04/20 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in SPI, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
	Atmega 128 at 16Mhz
USAGE:
    file spi.h for a description of the routines. 
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
	Easy
*************************************************************************/
#ifndef _SPI_H_
	#define _SPI_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
#define SPI_LSB_DATA_ORDER 1
#define SPI_MSB_DATA_ORDER 0
#define SPI_MASTER_MODE 1
#define SPI_SLAVE_MODE 0
/*
** variable
*/
struct SPI{
	/***/
	void (*transfer_sync) (uint8_t * dataout, uint8_t * datain, uint8_t len);
	void (*transmit_sync) (uint8_t * dataout, uint8_t len);
	uint8_t (*fast_shift) (uint8_t data);
};
typedef struct SPI SPI;
/*
** procedure and function header
*/
SPI SPIenable(uint8_t master_slave_select, uint8_t data_order,  uint8_t data_modes, uint8_t prescaler);
#endif
/***EOF***/
