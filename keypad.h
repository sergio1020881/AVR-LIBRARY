<<<<<<< HEAD
/*
 * keypad.h
 *
 * Created: 08/03/2018 20:36:39
 * Author: Sergio Santos
 *
 * perfection at its best
 */
#ifndef KEYPAD_H_
	#define KEYPAD_H_
/*
** constant and macro
*/
#define KEYPADLINE_1 7
#define KEYPADLINE_2 0
#define KEYPADLINE_3 1
#define KEYPADLINE_4 3
#define KEYPADDATA_1 2
#define KEYPADDATA_2 4
#define KEYPADDATA_3 5
#define KEYPADDATA_4 6
#define KEYPADSTRINGSIZE 15
#define KEYPADENTERKEY 'D'
/*
** variable
*/
struct keypadata{
	char* string;
	char character;
};
/******/
struct keypad{
	//Local Variables
	//Function Pointers
	char (*getkey)(void);
	struct keypadata (*get)(void);
	void (*flush)(void);
};
typedef struct keypad KEYPAD;
/*
** procedure and function header
*/
KEYPAD KEYPADenable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
#endif /* KEYPAD_H_ */
/************************************************************************
The matrix buttons should have a diode in series so each button would only let current flow in one direction not allowing
feedbacks. Little defect of keypads !
************************************************************************/
=======
/************************************************************************
KEYPAD API START
Author: Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
************************************************************************/
/***preamble inic***/
#ifndef _KEYPAD_H_
	#define _KEYPAD_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** Library
*/
#include <inttypes.h>
/***preamble inic***/
/*
** constant and macro
*/
#define KEYPADLINE_1 7
#define KEYPADLINE_2 0
#define KEYPADLINE_3 1
#define KEYPADLINE_4 3
#define KEYPADDATA_1 2
#define KEYPADDATA_2 4
#define KEYPADDATA_3 5
#define KEYPADDATA_4 6
#define KEYPADSTRINGSIZE 15
#define KEYPADENTERKEY 'D'
/*
** variable
*/
struct keypadata{
	char* string;
	char character;
};
/******/
struct keypad{
	//Local Variables
	//Function Pointers
	char (*getkey)(void);
	struct keypadata (*get)(void);
	void (*flush)(void);
};
typedef struct keypad KEYPAD;
/*
** procedure and function header
*/
KEYPAD KEYPADenable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
/***preamble inic***/
#endif
/***preamble inic***/
/************************************************************************
The matrix buttons should have a diode in series so each button would only let current flow in one direction not allowing
feedbacks. Little defect of keypads !
************************************************************************/
/************************************************************************
KEYPAD API END
************************************************************************/
>>>>>>> de2a0e3c69d79501f78d6430c9d7ca8dab5fd5c2
