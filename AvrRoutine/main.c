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

#define SIZE 10 

unsigned int mem[SIZE]=
{
9,1,
73,1,
6,2,
50,2,
37,2
};

unsigned int N=8;
unsigned int Y=0;


void PORTINIT(void);

unsigned int routine(unsigned int entry);
unsigned int search(unsigned int entry, unsigned int data[], unsigned int size);

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
		
		
		func.i16toa(func.hl(pleitura,leitura),Buffer);
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

unsigned int routine(unsigned int entry)
{
	unsigned int i;
	unsigned int index;
	unsigned int n;
	unsigned int y;
	n=N+1;
	y=Y;
	for(i=0;i<n;i++){
		index=y*n+(i+1);
		if(entry==index){
			y=i;
			break;
		}
	}
	return y;
}
unsigned int search(unsigned int entry, unsigned int data[], unsigned int size)
{
	unsigned int i;
	unsigned int j;
	unsigned int n;
	unsigned int p;
	unsigned int y;
	unsigned int z;
	n=N+1;
	p=0;
	y=Y;
	for(i=0;i<n;i++){
		z=y*n+(i+1);
		for(j=0;j<size;j+=2){
			if(z==data[j] && entry==data[j+1]){
				p=data[j];
				break;
			}
		}
	}
	return p;
}




/*
** Interrupt
*/

/*
** Comment
*/

