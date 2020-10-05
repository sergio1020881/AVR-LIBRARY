/************************************************************************
Title:    TRANSITION
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: transition.h,v 0.1 2016/06/26 15:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
License:  GNU General Public License
Comment:
	Very Stable
************************************************************************/
#ifndef _TRANSITION_H_
	#define _TRANSITION_H_
/*
** constant and macro
*/
/*
** variable
*/
struct TRAN{
	/***Variables***/
	uint8_t data;
	uint8_t hl;
	uint8_t lh;
	/***PROTOTYPES VTABLE***/
	uint8_t (*update)(struct TRAN *tran, uint8_t idata);
	uint8_t (*oneshot)(struct TRAN *tran, uint8_t idata);
};
typedef struct TRAN TRAN;
/*
** procedure and function header
*/
TRAN TRANenable(void);
#endif
/***EOF***/
