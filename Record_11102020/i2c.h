/***************************************************************************************************
	I2C
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
Hardware: Listed in i2c.c file
Date: 20102020
Comment:
	Stable
 **************************************************************************************************/
#ifndef _I2C_H
	#define _I2C_H
/***Library***/
#include <inttypes.h>
/***Constnat & Macros***/
/***Global Variable***/
struct twi{
	void (*Start)();
	void (*Stop)(void);
	void (*Write)(uint8_t );
	uint8_t (*Read)(uint8_t);
	uint8_t (*Status)(void);
};
typedef struct twi I2C;
/***Header***/
I2C I2Cenable(uint8_t prescaler);
#endif
/***EOF***/
