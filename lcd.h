/************************************************************************
LCD API START
Author: Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
************************************************************************/
/***preamble inic***/
#ifndef _LCD_H_
	#define _LCD_H_
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
//ASIGN PORT PINS TO LCD (can be setup in any way)
#define RS 0
#define RW 1
#define EN 2
#define NC 3
#define DB4 4
#define DB5 5
#define DB6 6
#define DB7 7
/*
** variable
*/
struct display{
	/******/
	void (*write)(char c, unsigned short D_I);
	char (*read)(unsigned short D_I);
	void (*BF)(void);
	void (*putch)(char c);
	char (*getch)(void);
	void (*string)(const char *s);
	void (*string_size)(const char* s, uint8_t size); // RAW
	void (*hspace)(uint8_t n);
	void (*clear)(void);
	void (*gotoxy)(unsigned int x, unsigned int y);
	void (*reboot)(void);
};
typedef struct display LCD0;
typedef struct display LCD1;
/*
** procedure and function header
*/
LCD0 LCD0enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
LCD1 LCD1enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
/***preamble inic***/
#endif
/***preamble inic***/
/************************************************************************
LCD API END
************************************************************************/
