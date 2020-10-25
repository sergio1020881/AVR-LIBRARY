/***************************************************************************************************
I2C API START
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com>
Date: 20102020
Comment:
	Stable
 **************************************************************************************************/
#ifndef _ATMEGA128I2C_H
	#define _ATMEGA128I2C_H
/***Library***/
#include <inttypes.h>
/***Function Prototypes***/
struct twi{
	void (*Start)();
	void (*Stop)(void);
	void (*Write)(uint8_t );
	uint8_t (*Read)(uint8_t);
	uint8_t (*Status)(void);
};
typedef struct twi I2C;
I2C I2Cenable(uint8_t prescaler);
/******/
#endif
/***EOF***/
