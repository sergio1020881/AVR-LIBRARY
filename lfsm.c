/*************************************************************************
Title:    LFSM
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: lfsm.c, v 0.1 2018/02/24 11:00:00 Sergio Exp $
Software: GCC
Hardware:
License:  GNU General Public License
DESCRIPTION:
	PC emulation
USAGE:
NOTES:
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
	working pretty good, trial more.
	page=1 is dedicated for global logic, page=2 local logic, if page>2 is sequencial program.
	purpose is for machine programming, and encoders. General purpose algorithm.
*************************************************************************/
/*
** library
*/
#include"lfsm.h"
/*
** constant and macro
*/
#define EMPTY 0
/*
** variable
*/
struct lfsmdata data;
struct	lfsmreport report;
/*
unsigned int mem[]=
{pin state, feedback, pin mask, output}
*/
/*
** procedure and function header
*/
struct	lfsmreport LFSMread(struct lfsm *r, unsigned int input);
unsigned int LFSMlearn(struct lfsm *r, unsigned int input, unsigned int next, unsigned int page);
unsigned int LFSMquant(struct lfsm *r);
unsigned int LFSMremove(struct lfsm *r, unsigned int input);
unsigned int LFSMdeleteall(struct lfsm *r);
/*
** Object Initialize
*/
LFSM LFSMenable(struct lfsmdata* eeprom, unsigned int sizeeeprom)
{
	/***Local Variable***/
	//printf("\tLFSMenable\n");
	/***Local Function Header***/
	unsigned int LFSMgetoutput(struct lfsm *r);
	void LFSMsetoutput(struct lfsm *r, unsigned int output);
	unsigned int LFSMlh(unsigned int xi, unsigned int xf);
	unsigned int LFSMhl(unsigned int xi, unsigned int xf);
	unsigned int LFSMoutputcalc(unsigned int feedback, unsigned int hl, unsigned int lh);
	unsigned int LFSMdiff(unsigned int xi, unsigned int xf);
	/***Create Object***/
	LFSM r;
	//Initialize variables
	r.mem=eeprom;
	r.sizeeeprom=sizeeeprom;
	r.page=0;//page
	r.input=0;//input
	r.output=0;//output
	//Function Vtable
	r.read=LFSMread;
	r.learn=LFSMlearn;
	r.quant=LFSMquant;
	r.remove=LFSMremove;
	r.deleteall=LFSMdeleteall;
	r.get=LFSMgetoutput;
	r.set=LFSMsetoutput;
	r.lh=LFSMlh;
	r.hl=LFSMhl;
	r.outputcalc=LFSMoutputcalc;
	r.diff=LFSMdiff;
	/******/
	return r;
}
/*
** procedure and function
*/
/***read***/
struct	lfsmreport LFSMread(struct lfsm *r, unsigned int input)
{
	unsigned int i1;
	unsigned int status=0;
	unsigned int keyfound;
	unsigned int HL,LH;
	//printf("\tLFSMread\n");
	HL=r->hl(r->input,input);
	LH=r->lh(r->input,input);
	if(HL || LH){ //to not waste time
		for(i1=0;i1<r->sizeeeprom;i1++){
			data=r->mem[i1];//upload eeprom data
			if(data.page){
				/******/
				switch(r->mem[i1].page){
					case 1:
						keyfound=(
							data.inhl==HL &&
							data.inlh==LH
							);//bool
						break;
					case 2:
						keyfound=(
							data.feedback==r->output &&
							data.inhl==HL &&
							data.inlh==LH
							);//bool
						break;
					default:
						keyfound=(
							data.feedback==r->output &&
							data.input==r->input &&
							data.inhl==HL &&
							data.inlh==LH
							);//bool
						break;
				};
				/******/
				if(keyfound){
					status=1; //exist
					break;
				}else status=2; //learning mode
			}
		}
	}else status=3; //repeat
	switch (status){
		case 0:
			//printf("LFSMread MEM empty\n");
			r->input=input;//detailed capture
			break;
		case 1:
			r->page=data.page;
			r->input=input;//detailed capture
			r->output=r->outputcalc(data.feedback,data.outhl,data.outlh);
			break;
		case 2:
			//printf("LFSMread not existent\n");
			//send signal if in learning mode
			r->input=input;//detailed capture
			break;
		case 3:
			//printf("LFSMread repeat\n");
			//r->input=input;//detailed capture
			break;
		default:
			//impossible situation
			break;
	}
	//printf("input: %d\n",r->input);
	report.output=r->output;
	report.status=status;
	return report;
}
/***learn***/
unsigned int LFSMlearn(struct lfsm *r, unsigned int input, unsigned int next, unsigned int page)
{
	unsigned int i1;
	unsigned int keyfound;
	unsigned int status=0;
	unsigned int HL,LH;
	//printf("\tLFSMlearn\n");
	HL=r->hl(r->input,input);
	LH=r->lh(r->input,input);
	if(page>0){//enable
		if(HL || LH){//there is a change ?
			for(i1=0;i1<r->sizeeeprom;i1++){
				data=r->mem[i1];//upload eeprom data one by one
				if(data.page){//find if it exists already
					/******/
					keyfound=(
						(
						data.page==1 &&
						data.inhl==HL &&
						data.inlh==LH
						)
							||
						(
						data.page==2 &&
						data.feedback==r->output &&
						data.inhl==HL &&
						data.inlh==LH
						)
							||
						(
						data.feedback==r->output &&
						data.input==r->input &&
						data.inhl==HL &&
						data.inlh==LH
						)
					);//bool
					//if there is any logic entry, that entry is taken out from lfsm input options
					/******/
					if(keyfound){
						status=1;//not permitted
						break;
					}
				}
				status=2;//not existent
			}
		}
	}
	switch (status){
		case 0://not enabled
			//printf("LFSMlearn No Operation.\n");
			break;
		case 1:
			//printf("LFSMlearn not permitted.\n");
			break;
		case 2://get ready to record
			//printf("LFSMlearn going to try add new program.\n");
			//prepare data to write to eeprom
			data.page=page;
			data.feedback=r->output;
			data.input=r->input;
			data.inhl=HL;
			data.inlh=LH;
			data.outhl=r->hl(r->output,next);
			data.outlh=r->lh(r->output,next);
			//printf("%d  %d  %d  %d  %d  %d  %d\n",data.page,data.feedback,data.input,data.inhl,data.inlh,data.outhl,data.outlh);
			for(i1=0;i1<r->sizeeeprom;i1++){
				//search empty space in memory
				if(r->mem[i1].page==EMPTY){
					//write data to eeprom
					r->mem[i1]=data;
					status=3;//created
					break;
				}
				status=4;//not possible memory full
			}
		case 3:
			//printf("LFSMlearn successfully added.\n");
			break;
		case 4:
			//printf("LFSMlearn memory full.\n");
			break;
		default:
			break;
	}
	return status;
}
/***quant***/
unsigned int LFSMquant(struct lfsm *r)
{
	unsigned int i1;
	unsigned int programmed;
	//printf("\tLFSMquant\n");
	for(i1=0,programmed=0;i1<r->sizeeeprom;i1++){
		data=r->mem[i1];//upload data from eeprom
		if(data.page!=EMPTY){//count memory used
			//printf("page:%d feedback:%d  input:%d  inhl:%d  inlh:%d  outhl:%d  outlh:%d\n",data.page,data.feedback,data.input,data.inhl,data.inlh,data.outhl,data.outlh);
			programmed++;
		}
	}
	return programmed;
}
/***remove***/
unsigned int LFSMremove(struct lfsm *r, unsigned int input)
{
	unsigned int i1;
	unsigned int keyfound;
	unsigned int status=0;
	unsigned int HL,LH;
	//printf("\tLFSMremove\n");
	HL=r->hl(r->input,input);
	LH=r->lh(r->input,input);
	for(i1=0;i1<r->sizeeeprom;i1++){
		data=r->mem[i1];//upload data from eeprom
		if(data.page){
			/******/
			switch(data.page){
				case 1:
					keyfound=(
						data.inhl==HL &&
						data.inlh==LH
						);//bool
					break;
				case 2:
					keyfound=(
						data.feedback==r->output &&
						data.inhl==HL &&
						data.inlh==LH
						);//bool
					break;
				default:
					keyfound=(
						data.feedback==r->output &&
						data.input==r->input &&
						data.inhl==HL &&
						data.inlh==LH
						);//bool
					break;
			};
			/******/
			if(keyfound){
				status=1;//remove
				break;
			}
		}
		status=2;//does not exist
	}
	switch (status){
		case 0:
			//printf("LFSMremove -> No operation\n");
			break;
		case 1:
			//printf("LFSMremove removed: %d\n",status);
			//descativate memory space, write to eeprom empty space.
			r->mem[i1].page=EMPTY;
			break;
		case 2:
			//printf("LFSMremove not existent: %d\n",status);
			break;
		default:
			break;
	}
	return status;
}
/***deleteall***/
unsigned int LFSMdeleteall(struct lfsm *r)
{
	unsigned int i1;
	unsigned int status=0;
	//printf("\tLFSMdeleteall\n");
	if(!status){//not removed
		for(i1=0;i1<r->sizeeeprom;i1++){
			//read eeprom check if memory space has data
			if(r->mem[i1].page){
				//deactivate memory space to empty
				r->mem[i1].page=EMPTY;
				status=1;//all deleted
			}
		}
	}
	r->output=0;
	//printf("Done\n");
	return status;
}
/***get***/
unsigned int LFSMgetoutput(struct lfsm *r)
{
	//printf("LFSMgetoutput\n");
	return r->output;
}
/***set***/
void LFSMsetoutput(struct lfsm *r, unsigned int output)
{
	//printf("LFSMsetoutput\n");
	r->output=output;
}
/***lh***/
unsigned int LFSMlh(unsigned int xi, unsigned int xf)
{
	unsigned int i;
	//printf("LFSMlh\n");
	i=xf^xi;
	i&=xf;
	return i;
}
/***hl***/
unsigned int LFSMhl(unsigned int xi, unsigned int xf)
{
	unsigned int i;
	//printf("LFSMhl\n");
	i=xf^xi;
	i&=xi;
	return i;
}
/***output***/
unsigned int LFSMoutputcalc(unsigned int feedback, unsigned int hl, unsigned int lh)
{
	//printf("LFSMoutputcalc\n");
	feedback|=lh;
	feedback&=~hl;
	return feedback;
}
/***diff***/
unsigned int LFSMdiff(unsigned int xi, unsigned int xf)
{
	//printf("LFSMdiff\n");
	return xi^xf;
}
/*
** interrupt
*/
/***EOF***/
