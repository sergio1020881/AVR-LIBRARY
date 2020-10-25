/*************************************************************************
	TRANSITION
Author: Sergio Manuel Santos
	<sergio.salazar.santos@gmail.com> 
License: GNU General Public License
Hardware: all
Date: 25102020
Comment:
	Very Stable
*************************************************************************/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/***Library***/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdarg.h>
#include <inttypes.h>
/***pc use***
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
***/
#include"transition.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
/***Global File Variable***/
/***Header***/
uint8_t TRANlh(uint8_t xi, uint8_t xf);
uint8_t TRANhl(uint8_t xi, uint8_t xf);
uint8_t TRANupdate(struct TRAN *tran, uint8_t idata);
uint8_t TRANoneshot(struct TRAN *tran, uint8_t idata);
/***Procedure & Function***/
TRAN TRANenable( void )
{
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// struct object
	TRAN tran;
	// local var
	tran.data=0;
	tran.hl=0;
	tran.lh=0;
	// function pointers
	tran.update=TRANupdate;
	tran.oneshot=TRANoneshot;
	SREG=tSREG;
	/******/
	return tran;
}
// lh
uint8_t TRANlh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi^xf;
	i&=xf;
	return i;
}
// hl
uint8_t TRANhl(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xi;
	return i;
}
// TRANupdate
uint8_t TRANupdate(struct TRAN *tran, uint8_t idata)
{
	uint8_t r;
	if(tran->data == idata)
	r=0;
	else{
		tran->lh = TRANlh(tran->data, idata);
		tran->hl = TRANhl(tran->data, idata);
		tran->data=idata;
		r=1;
	}
	return r;
}
// TRANoneshot
uint8_t TRANoneshot(struct TRAN *tran, uint8_t idata)
{
	uint8_t r;
	if(tran->data == idata)
		r=0;
	else
		r=1;
	tran->lh = TRANlh(tran->data, idata);
	tran->hl = TRANhl(tran->data, idata);
	tran->data=idata;
	return r;
}
/***Interrupt***/
/***EOF***/

