/*
 * AVR Atmega 128.c
 *
 * Created: 24/05/2017 19:24:55
 * Author : Sergio Santos
 */ 
#include <avr/io.h>
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
uint8_t PORT[NPORTS];
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
	InicPORT();
	STATE=0;
    /* Replace with your application code */
while (1)
	{
		/*LFSM*/
		switch(STATE){
			case 0:
				PORTB=0XFF;
				if(!(PORT[A]&1))
					STATE=1;
				break;
			case 1:
				PORTB=0X00;
				if(!(PORT[A]&2))
					STATE=0;
				if(!(PORT[A]&4))
					STATE=2;
				break;
			case 2:
				PORTB=0X0F;
					if(!(PORT[A]&2))
						STATE=0;
				break;
			default:
				break;
		}		
		/*LOGIC*/
		if(!(PORT[A]&8))
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
	PORT[A]=0;
	PORT[B]=0;
	PORT[C]=0;
	PORT[D]=0;
	PORT[E]=0;
	PORT[F]=0;
	PORT[G]=0;
}
void UpdatePORT(void)
{
	PORT[A]=PINA;
	PORT[B]=PINB;
	PORT[C]=PINC;
	PORT[D]=PIND;
	PORT[E]=PINE;
	PORT[F]=PINF;
	PORT[G]=PING;
}
