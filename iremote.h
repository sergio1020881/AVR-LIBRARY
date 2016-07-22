/*
 * IREMOTE.h
 *
 * Created: 10/07/2016 01:06:18
 *  Author: Sérgio Santos
 * testing and adjustments
 */ 
#ifndef IREMOTE_H_
	#define IREMOTE_H_
/*
** constant and macro
*/
/***CONFIG***/
#define IR_PIN 2
/***TIMER***/
#define IR_F_DIV 64 // 64
#define IR_CTC_VALUE 62 // 62
/***DATA***/
#define IR_BYTE 11
#define IR_BIT 7
/*
** variable
*/
uint8_t IRbyte[IR_BYTE+1];
uint8_t ir_prescaler;

struct iremote{
	// prototype pointers
	uint8_t* (*key)(void);
	void (*start)(void);
	void (*stop)(void);
};
typedef struct iremote IR;
/*
** procedure and function header
*/
IR IRenable(void);
#endif /* IREMOTE_H_ */
