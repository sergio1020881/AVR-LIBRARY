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
#define IR_INPORT PIND
#define IR_PIN 2
/***TIMER***/
#define IR_F_DIV 128 // 64
#define IR_CTC_VALUE 74 // 62  74
/***DATA***/
#define IR_BYTE 7
#define IR_BIT 7
/*
** variable
*/
struct iremote{
	// prototype pointers
	volatile uint8_t* (*key)(void);
	void (*start)(void);
	void (*stop)(void);
};
typedef struct iremote IR;
/*
** procedure and function header
*/
IR IRenable(void);
#endif /* IREMOTE_H_ */
