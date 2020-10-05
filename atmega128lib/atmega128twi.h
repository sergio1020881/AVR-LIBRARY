/************************************************************************
ATMEGA128 TWI API START
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
	28092020
************************************************************************/
/***preamble inic***/
#ifndef _ATMEGA128TWI_H_
	#define _ATMEGA128TWI_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** Library
*/
#include <inttypes.h>
/************************************************************************
I2C API START
testing phase
************************************************************************/
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
struct tw{
	/***PROTOTYPES VTABLE***/
	void (*start)(unsigned char mode);
	void (*master_connect)(unsigned char addr, unsigned char rw);
	void (*master_write)(unsigned char data);
	unsigned char (*master_read)(unsigned char request);
	void (*stop)(void);
};
typedef struct tw I2C;
/*
** procedure and function header
*/
I2C I2Cenable(unsigned char device_id, unsigned char prescaler);
/***EOF***/
/************************************************************************
I2C API END
testing phase
************************************************************************/
#endif
/***EOF***/
