#ifndef _I2C_H
	#define _I2C_H   1
/************************************************************************* 
* Title:    I2C library
*           
* Author:   sergio manuel santos
* File:     $Id: i2c.h,v 1.10 2014/04/17 15:00:00 sergio Exp $
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   any AVR device
* Usage:    

COMMENT:
	testing phase
	
**************************************************************************/
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constants and macros
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
** global variables
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
** global object function header
*/
I2C I2Cenable(unsigned char device_id, unsigned char prescaler);
/*
** global function header
*/
#endif
/***EOF***/
