/************************************************************************
Title:    LFSM
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: lfsm.h,v 0.1 2015/08/12 14:00:00 Sergio Exp $
Software: GCC
Hardware:
License:  GNU General Public License
Usage:
LICENSE:
    Copyright (C) 2015
    This program is free software; you can redistribute it and/or modify
    it under the consent of the code developer, in case of commercial use
    need license.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
COMMENT:
	review
************************************************************************/
#ifndef _LFSM_H_
	#define _LFSM_H_
/*
** constant and macro
*/
/*
** variable
*/
struct lfsmdata{
	unsigned int page;
	unsigned int feedback;
	unsigned int input;
	unsigned int inhl;
	unsigned int inlh;
	unsigned int outhl;
	unsigned int outlh;
};
struct	lfsmreport{
	unsigned int output;
	unsigned int status;
};
/******/
struct lfsm{
	//Local Variables
	struct lfsmdata* mem;
	unsigned int sizeeeprom;
	unsigned int page;
	unsigned int input;
	unsigned int output;
	//Function Pointers
	struct lfsmreport (*read)(struct lfsm *r, unsigned int input);
	unsigned int (*learn)(struct lfsm *r, unsigned int input, unsigned int next, unsigned int page);
	unsigned int (*quant)(struct lfsm *r);
	unsigned int (*remove)(struct lfsm *r, unsigned int input);
	unsigned int (*deleteall)(struct lfsm *r);
	unsigned int (*get)(struct lfsm *r);
	void (*set)(struct lfsm *r,unsigned int output);
	unsigned int (*lh)(unsigned int xi, unsigned int xf);
	unsigned int (*hl)(unsigned int xi, unsigned int xf);
	unsigned int (*outputcalc)(unsigned int feeddback, unsigned int hl,unsigned int lh);
	unsigned int (*diff)(unsigned int xi, unsigned int xf);
};
typedef struct lfsm LFSM;
/*
** procedure and function header
*/
LFSM LFSMenable(struct lfsmdata* eeprom, unsigned int sizeeeprom);
#endif
/***EOF***/
