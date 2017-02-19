/*
 * Atmega324_Demo_1.c
 *
 * Created: 22/01/2017 21:29:31
 * Author : sergio
 */ 
#define F_CPU 8000000UL
#define TRUE 1

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <string.h>

#include "lcd.h"
#include "function.h"
//#include "transition.h"
//#include "atmega8535timer.h"
//#include "iremote.h"
#include "AVRoutine.h"

#define SIZE 16 

//See matrix table 8 X 8
unsigned int mem[SIZE]={
9,1,
9,4,
73,1,
6,2,
50,2,
50,4,
37,2,
45,1
};

void PORTINIT(void);

int main(void)
{
	PORTINIT();
	FUNC func=FUNCenable();
	LCD0 lcd=LCD0enable(&DDRC,&PINC,&PORTC);
	ROUTINE run=ROUTINEenable(8);
	char Buffer[8];
	unsigned int leitura;
	unsigned int pleitura=255;
    /* Replace with your application code */
    while (TRUE)
    {
		lcd.gotoxy(0,0);
		lcd.string("Ola Sergio Manuel");
		lcd.gotoxy(0,1);
		lcd.string("PINA");
		lcd.hspace(2);
		
		leitura=PINA;
		
		
		func.i16toa(leitura,Buffer);
		lcd.string_size(Buffer,4);
		
		func.i16toa(run.routine(&run,run.search(&run,func.hl(pleitura,leitura),mem,SIZE)),Buffer);
		
		lcd.gotoxy(0,2);
		lcd.string(Buffer);
		lcd.gotoxy(0,3);
		lcd.string("PORTA");
		
		pleitura=leitura;
    }
}

/*
** Procedures and Functions
*/

void PORTINIT(void)
{
	DDRA=0X00;
	PORTA=0XFF;
}

/*
** Interrupt
*/

/*
** Comment
*/

