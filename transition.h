/************************************************************************
	TRANSITION
Author: Sergio Manuel Santos
	<sergio.salazar.santos@gmail.com>
License:  GNU General Public License
Hardware: all
Date: 25102020
Comment:
	Very Stable
************************************************************************/
#ifndef _TRANSITION_H_
	#define _TRANSITION_H_
/***Constant & Macro***/
/***Global Variable***/
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
/***Header***/
TRAN TRANenable(void);
#endif
/***EOF***/
