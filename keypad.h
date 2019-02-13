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
