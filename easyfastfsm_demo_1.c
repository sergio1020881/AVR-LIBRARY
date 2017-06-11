/*
 * AVR Atmega 128.c
 *
 * Created: 24/05/2017 19:24:55
 * Author : Sergio Santos
 */ 
#include <avr/io.h>
#include "function.h"
/*
**
*/
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define NPORTS 7
uint8_t PORT[2][NPORTS];
/*
**
*/
void PORTINIT(void);
void InicPORT(void);
void UpdatePORT(void);
/*
**
*/
int main(void)
{
	uint8_t STATE;
	PORTINIT();
	STATE=0;
	FUNC func=FUNCenable();
    /* Replace with your application code */
while (1)
	{
		InicPORT();
		/*LFSM*/
		switch(STATE){
			case 0:
				PORTB=0b11111110;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=1;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=7;
				break;
			case 1:
				PORTB=0b11111101;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=2;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=0;
				break;
			case 2:
				PORTB=0b11111011;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=3;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=1;
				break;
			case 3:
				PORTB=0b11110111;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=4;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=2;
				break;
			case 4:
				PORTB=0b11101111;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=5;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=3;
				break;
			case 5:
				PORTB=0b11011111;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=6;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=4;
				break;
			case 6:
				PORTB=0b10111111;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=7;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=5;
				break;
			case 7:
				PORTB=0b01111111;
				if(func.hl(PORT[0][A],PORT[1][A])&1)
					STATE=0;
				if(func.hl(PORT[0][A],PORT[1][A])&2)
					STATE=6;
				break;
			default:
				break;
		}		
		/*LOGIC*/
		if(!(PORT[1][A]&8))
			STATE=2;
		
		UpdatePORT();
    }
}
/*
**
*/
void PORTINIT(){
	DDRA=0X00;
	PORTA=0XFF;
	DDRB=0XFF;
	PORTB=0XFF;
}
void InicPORT(void)
{
	PORT[1][A]=PINA;
	PORT[1][B]=PINB;
	PORT[1][C]=PINC;
	PORT[1][D]=PIND;
	PORT[1][E]=PINE;
	PORT[1][F]=PINF;
	PORT[1][G]=PING;
}
void UpdatePORT(void)
{
	PORT[0][A]=PORT[1][A];
	PORT[0][B]=PORT[1][B];
	PORT[0][C]=PORT[1][C];
	PORT[0][D]=PORT[1][D];
	PORT[0][E]=PORT[1][E];
	PORT[0][F]=PORT[1][F];
	PORT[0][G]=PORT[1][G];
}
