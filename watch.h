/************************************************************************
Title:    watch.h
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: watch.h,v 0.2 2017/07/01 17:00:00 Sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware:
License:  GNU General Public License
Comment:
	
************************************************************************/
#ifndef WATCH_H_
	#define WATCH_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constant and macro
*/
/*
** Data types
*/
struct TIME{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t seconds;
};
struct WATCH{
	uint8_t (*start_delay)(uint8_t n_delay, uint16_t seconds);
	void (*reset_delay)(uint8_t n_delay);
	uint8_t (*hour)(void);
	uint8_t (*minute)(void);
	uint8_t (*second)(void);
	uint16_t (*seconds)(void);
	void (*preset)(uint8_t hour, uint8_t minute, uint8_t second);
	void (*setminute)(void);
	void (*sethour)(void);
	void (*increment)(void);
	void (*decrement)(void);
	char* (*show)(void);
};
typedef struct WATCH WATCH;
/*
** procedure and function header
*/
WATCH WATCHenable(void);
#endif /* WATCH_H_ */
/***EOF***/
