/*
 * LMACHINE.c
 *
 * Created: 13-04-2014
 * Author: SERGIO MANUEL SANTOS
 */ 
/*
** TYPE OF CHIP
*/
//Atmega 128 at 16MHZ
#define F_CPU 16000000UL
/*
** Library
*/
#include <avr/io.h>
/*
** constants and macros
*/
#define TRUE 1
#define FALSE 0
#define GI 7
/*
** Private Library
*/
#include "function.h"
#include "lcd.h"
#include "uart.h"
#include "fsm.h"
#include "vfsm.h"
#include "analog.h"
#include "i2c.h"
#include "timer.h"
/*
** global variables
*/
//B1
uint8_t memoria_1[18]={
//output,	input,		change
5,			0,			10,
2,			0,			6,
1,			0,			5,
6,			0,			9,
10,			0,			6,
9,			0,			5
};
//M1B1-3
uint8_t memoria_2[3]={
//output,	input,	change
0,			0,			0
};
//M2B1-5
uint8_t memoria_3[3]={
//output,	input,	change
0,			0,			0
};
//M2M1B1-5
uint8_t memoria_4[12]={
//output,	input,	change
0,			0,			0
};
//M1-2
uint8_t memoria_5[3]={
//output,	input,		change
5,			0,			2	// 3
};
//M2-4
uint8_t memoria_6[3]={
//output,	input,		change
6,			0,			1	// 5
};
//TIMER-1
uint8_t memoria_7[9]={
//output,	input,		change
0,			1,			5,
10,			1,			6,
9,			1,			5
};
/*
** Procedure and Funtion Main Prototypes
*/
void PORTINIT();
int main(void)
{
	PORTINIT();
	/***INICIALIZE OBJECTS***/
	FUNC function= FUNCenable();
	LCD lcd = LCDenable(&DDRA,&PINA,&PORTA);
	UART1 uart= UART1enable(103,8,1,NONE);//103 para 9600, 68 para 14400
	VFSM button_1 = VFSMenable(memoria_1,18);
	VFSM button_2 = VFSMenable(memoria_2,3);
	VFSM button_3 = VFSMenable(memoria_3,3);
	VFSM button_4 = VFSMenable(memoria_4,3);
	VLOGIC button_5 = VLOGICenable(memoria_5,3);
	VLOGIC button_6 = VLOGICenable(memoria_6,3);
	VFSM button_7 = VFSMenable(memoria_7,9);
	I2C i2c = I2Cenable(85, 1);
	ANALOG analog = ANALOGenable(1, 128, 3, 0, 4, 7);
	TIMER0 timer0 = TIMER0enable(0,0,3);
	uart.puts("OLA SERGIO !!");
	/******/
	char tmp[16];
	int entrada;
	uint8_t vmfsm[7];
	while(TRUE){
		lcd.reboot();
		//ignore cancel stop enter delete run
		//TODO:: Please write your application code
		entrada=PINB;
		lcd.gotoxy(0,0);
		function.itoa(entrada,tmp);
		lcd.string(function.resizestr(tmp,3));
		lcd.gotoxy(4,0);
		function.itoa(PORTC,tmp);
		lcd.string(function.resizestr(tmp,3));
		lcd.gotoxy(12,0);
		function.itoa(analog.read(0),tmp);
		lcd.string(function.resizestr(tmp,4));
		lcd.gotoxy(12,1);
		function.itoa(analog.read(2),tmp);
		lcd.string(function.resizestr(tmp,4));
		vmfsm[0]=button_1.read(&button_1, entrada & 1, (PINC & 15));//1
		vmfsm[1]=button_2.read(&button_2, entrada & 3, vmfsm[0]);//3
		vmfsm[2]=button_3.read(&button_3, entrada & 5, vmfsm[1]);//5
		vmfsm[3]=button_4.read(&button_4, entrada & 7, vmfsm[2]);//2
		vmfsm[4]=button_5.read(&button_5, entrada & 2, vmfsm[3]);//4
		vmfsm[5]=button_6.read(&button_6, entrada & 4, vmfsm[4]);//4
		vmfsm[6]=button_7.read(&button_7, timer0.cmpm(100) & 1, vmfsm[5]);
		PORTC =	vmfsm[6];
		/***TIMER***/
		if(vmfsm[6]==10 || vmfsm[6]==9 || vmfsm[6]==0){
			timer0.start(255, 1024);
		}else{
			timer0.stop();
		}
		lcd.gotoxy(0,1);
		lcd.string(function.resizestr(uart.read(),12));
		/***/
		i2c.start();
		i2c.connect(10,TWI_WRITE);
		i2c.write('h');
		i2c.stop();
	} 
}
/*
** Procedure and Funtion Definitions
*/
void PORTINIT()
{
	//INPUT
	DDRF=0x00;
	PORTF=0x0F;
	DDRB=0XF0;
	PORTB=0XFF;
	DDRD=0X00;
	PORTD=0XFF;
	//OUTPUT
	DDRC=0XFF;
	PORTC=0x00;
	//UART0
	//DDRE=0X02;
	SREG|=(1<<GI);
}
/***EOF***/
