/************************************************************************
Title:    Atmega 324 Preamble
Author:   Sergio Manuel Salazar dos Santos <sergio.salazar.santos@gmail.com>
File:     $Id: atmega324preamble.h,v 0.2 2018/08/13 20:30:00 Sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: Atmega 324A
License:  GNU General Public License
Comment:
	Stable
************************************************************************/

#ifndef ATMEGA324PREAMBLE_H_
	#define ATMEGA324PREAMBLE_H_

/*
** constant and macro
*/

/*
** variable
*/

struct Atmega324{
	//Variables
	//Function Pointers
	void (*ioread)(void);
	uint8_t (*hl)(char port);
	uint8_t (*ll)(char port);
	uint8_t (*lh)(char port);
	uint8_t (*hh)(char port);
};
typedef struct Atmega324 ATMEGA324;

/*
** procedure and function header
*/

ATMEGA324 ATMEGA324enable(void);
#endif /* ATMEGA324PREAMBLE_H_ */
