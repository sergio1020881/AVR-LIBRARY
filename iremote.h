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
#define IR_PIN 2
#define IR_F_DIV 256
#define IR_CTC_VALUE 70
#define IR_BYTE 3
#define IR_BIT 7
/*
** variable
*/
uint8_t IRbyte[4];
volatile uint8_t IR_N_BYTE;
volatile uint8_t IR_N_BIT;
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
IR IRenable(uint8_t ir_ctc_value);
#endif /* IREMOTE_H_ */
