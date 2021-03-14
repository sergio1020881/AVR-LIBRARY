/*************************************************************************
Title:    ATLFSM.c
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: atlfsm.c, v 0.1 2016/08/21 14:00:00 Sergio Exp $
Software: GCC
Hardware:
License:  GNU General Public License
CommentT:
	working on it.
	working pretty good, trial more.
	page=1 is dedicated for global logic,if page>1 is sequencial program.
	purpose is for machine programming, and encoders. General purpose algorithm.
	Moving code to Chip.
*************************************************************************/
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>
#include"atlfsm.h"
/*
** constant and macro
*/
#define EMPTY 0
/*
** variable
*/
LFSMDATA data;
/*
unsigned int mem[]=
{pin state, feedback, pin mask, output}
*/
/*
** procedure and function header
*/
uint8_t LFSMread(struct lfsm *r, uint8_t input);
uint8_t LFSMlearn(struct lfsm *r, uint8_t input, uint8_t next, uint8_t page);
uint16_t LFSMquant(struct lfsm *r);
uint8_t LFSMremove(struct lfsm *r, uint8_t input);
uint8_t LFSMdeleteall(struct lfsm *r);
uint8_t LFSMgetoutput(struct lfsm *r);
void LFSMsetoutput(struct lfsm *r, uint8_t output);
uint8_t LFSMlh(uint8_t xi, uint8_t xf);
uint8_t LFSMhl(uint8_t xi, uint8_t xf);
uint8_t LFSMoutputcalc(uint8_t feedback, uint8_t hl, uint8_t lh);
uint8_t LFSMdiff(uint8_t xi, uint8_t xf);
//void nop(void);
/*
** Object Initialize
*/
LFSM LFSMenable(LFSMDATA *eeprom, uint16_t sizeeeprom)
{
	/***Local Variable***/
	/***Create Object***/
	LFSM r;
	//Inicialize Object variables
	r.mem=eeprom;
	r.sizeeeprom=sizeeeprom;
	r.page=0;
	r.input=255;//input
	r.output=255;//output
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
uint8_t LFSMread(struct lfsm *r, uint8_t input)
{
	uint16_t i1;
	uint8_t keyfound=0;
	uint8_t PAGE=0;
	uint8_t HL;
	uint8_t LH;
	uint8_t OUTPUT;
	uint8_t N;
	N=r->sizeeeprom;
	//printf("LFSMread\n");
	for(i1=0;i1<N;i1++){
		data=r->mem[i1];//upload eeprom data
		if(data.page){
			/******/
			PAGE=r->mem[i1].page;
			switch(PAGE){
				case 1:
					HL=LFSMhl(r->input,input);
					LH=LFSMlh(r->input,input);
					keyfound=(
						data.inhl==HL && 
						data.inlh==LH
						);//bool
					//it keeps track of previous input, not desired in logic
					break;
				default:
					OUTPUT=r->output;
					HL=LFSMhl(r->input,input);
					LH=LFSMlh(r->input,input);
					keyfound=(
						data.feedback==OUTPUT &&
						data.inhl==HL && 
						data.inlh==LH
						);//bool
					//it keeps track of previous input, not desired in logic
					break;
			};
		}
		if(keyfound){
			r->page=data.page;
			//r->input=input; // detailed capture
			r->output=r->outputcalc(data.feedback,data.outhl,data.outlh);
			break;
		}
	}
	//if(!keyfound){
			r->input=input;
	//}
	return r->output;
}
/***learn***/ 
uint8_t LFSMlearn(struct lfsm *r, uint8_t input, uint8_t next, uint8_t page)
{
	uint16_t i1;
	uint8_t keyfound;
	uint8_t status=0;
	//printf("LFSMlearn\n");
	if(page>0){
		for(i1=0;i1<r->sizeeeprom;i1++){
			data=r->mem[i1]; // upload eeprom data
			if(data.page){
				/******/
				//printf("LFSMlearn_page %d\n",page);
				keyfound=(
					(
					data.page==1 &&
					data.inhl==r->hl(r->input,input) && 
					data.inlh==r->lh(r->input,input)
					)
						||
					(
					data.feedback==r->output &&
					data.inhl==r->hl(r->input,input) && 
					data.inlh==r->lh(r->input,input)
					)
				); // bool
				// if there is any logic entry, that entry is taken out from lfsm input options
				/******/
				if(keyfound){
					status=1; // not permitted
					break;
				}
			}
		status=2; // not existent
		}
	}
	switch (status){
		case 0:
			//printf("LFSMlearn No Operation.\n");
			break;
		case 1:
			//printf("LFSMlearn already existent.\n");
			break;
		case 2:
			//printf("LFSMlearn going to try add new program.\n");
			// prepare data to write to eeprom
			data.page=page;
			data.feedback=r->output;
			data.inhl=r->hl(r->input,input);
			data.inlh=r->lh(r->input,input);
			data.outhl=r->hl(r->output,next);
			data.outlh=r->lh(r->output,next);
			//printf("%d  %d  %d  %d  %d  %d  %d  %d\n",data.page,data.feedback,data.input,data.inhl,data.inlh,data.outhl,data.outlh,data.output);
			for(i1=0;i1<r->sizeeeprom;i1++){
				// search empty space em memory
				if(r->mem[i1].page==EMPTY){
					// write data to eeprom
					r->mem[i1]=data;
					status=3; // created
					break;
				}
				status=4; // not possible
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
uint16_t LFSMquant(struct lfsm *r)
{
	uint16_t i1;
	uint16_t programmed;
	//printf("LFSMquant\n");
	for(i1=0,programmed=0;i1<r->sizeeeprom;i1++){
		data=r->mem[i1]; // upload data from eeprom
		if(data.page!=EMPTY){
			//printf("page:%d feedback:%d  input:%d  inhl:%d  inlh:%d  outhl:%d  outlh:%d  output:%d\n",data.page,data.feedback,data.input,data.inhl,data.inlh,data.outhl,data.outlh,data.output);
			programmed++;
		}
	}
	return programmed;
}
/***remove***/
uint8_t LFSMremove(struct lfsm *r, uint8_t input)
{
	uint16_t i1;
	uint8_t keyfound;
	uint8_t status=0;
	//printf("LFSMremove\n");
	for(i1=0;i1<r->sizeeeprom;i1++){
		data=r->mem[i1]; // upload data from eeprom
		if(data.page){
			//printf("LFSMremove\n");
			/******/	
			switch(data.page){
				case 1:
					keyfound=(
						data.inhl==r->hl(r->input,input) && 
						data.inlh==r->lh(r->input,input)
						); // bool
					break;
				default:
					keyfound=(
						data.feedback==r->output &&
						data.inhl==r->hl(r->input,input) && 
						data.inlh==r->lh(r->input,input)
						); // bool
					break;
			};
			/******/
			if(keyfound){
				status=1; // remove
				break;
			}
		}
		status=2; // does not exist
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
uint8_t LFSMdeleteall(struct lfsm *r)
{
	uint16_t i1;
	uint8_t status=0;
	if(!status){ // not removed
		for(i1=0;i1<r->sizeeeprom;i1++){
			//read eeprom check if memory space has data
			if(r->mem[i1].page){
				//desactivate memory space to empty
				r->mem[i1].page=EMPTY;
				status=1; // all deleted
			}
		}
	}
	r->output=0;
	//printf("LFSMdeleteall deleted\n");
	return status;
}
/***get***/
uint8_t LFSMgetoutput(struct lfsm *r)
{
	return r->output;
}
/***set***/
void LFSMsetoutput(struct lfsm *r, uint8_t output)
{
	r->output=output;
}
/***lh***/
uint8_t LFSMlh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xf;
	return i;
}
/***hl***/
uint8_t LFSMhl(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xi;
	return i;
}
/***output***/
uint8_t LFSMoutputcalc(uint8_t feedback, uint8_t hl, uint8_t lh)
{
	feedback&=~hl;
	feedback|=lh;
	return feedback;
}
/***diff***/
uint8_t LFSMdiff(uint8_t xi, uint8_t xf)
{
	return xi^xf;
}
//void nop(void)
//{
//	uint8_t nop;
//	nop=0;
//}
/*
** interrupt
*/
/***EOF***/
/***Comments**
Reality works like this:
keyfound=(
	block[LFSM_feedback]==r->output &&
	block[LFSM_mask]==mask && 
	block[LFSM_maskedinput]==(mask&input)
);//bool, block[1] is masked bits, block[1] is bits state				
**************
NOTES:
unsigned int vect[]=
{
mask,mask&pinstate,feedback,output,
};
**************
try to make more depth choice of database research or option selections.
*************/
