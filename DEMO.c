/*
 * LMACHINE.c
 *
 * Created: 09-04-2014 14:30:00
 * Author: SERGIO SALAZAR SANTOS
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
// INPUT
#define timertrigger 0x10

// OUTPUT

#define timerstart 0x10
#define timerstop 0x20

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


uint8_t i2c_read_value;

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
	UART uart= UARTenable(103,8,1,NONE);//103 para 9600, 68 para 14400
	//EFSM fsm = EFSMenable(4000,1);//eeprom size 4000 bytes page 1
	VFSM button_1 = VFSMenable(memoria_1,18);
	VFSM button_2 = VFSMenable(memoria_2,3);
	VFSM button_3 = VFSMenable(memoria_3,3);
	VFSM button_4 = VFSMenable(memoria_4,3);
	VLOGIC button_5 = VLOGICenable(memoria_5,3);
	VLOGIC button_6 = VLOGICenable(memoria_6,3);
	VFSM button_7 = VFSMenable(memoria_7,9);
	//SFSMinit(memoria,40);
	//I2C i2c = I2Cenable(85, 1);
	ANALOG analog = ANALOGenable(1, 128, 3, 0, 4, 7);
	
	TIMER0 timer0 = TIMER0enable(0,0,255,3);
	
	/**************trial***begin*************/
	// UART
	uart.puts("ola sergio !!");
	
	/*
	// I2C
	// First we initial the pointer register to address 0x00
	// Start the I2C Write Transmission
	i2c.start(&i2c, DS1307_ID, DS1307_ADDR, I2C_WRITE);
	// Start from Address 0x00
	//i2c.write(&i2c,DS1307_ADDR);
	// Stop I2C Transmission
	i2c.stop(&i2c);
	// Start the I2C Read Transmission
	i2c.start(&i2c, DS1307_ID, DS1307_ADDR, I2C_READ);
	i2c.read(&i2c, (char*)&i2c_read_value, ACK);
	//can read has many times you want the slave increments address automticaly at each read procedure
	i2c.stop(&i2c);
	*/
	
	// EEPROM
	//eeprom_update_byte((uint8_t*)1,(uint8_t)0);
	
	/**************trial***end*************/
	/******/
	//int arg[]={1,2,3};
	char* msg;
	char tmp[16];
	int entrada;
	int ler;
	int cmd;
	int data;
	int rec[4];
	rec[0]=0;
	uint8_t vmfsm[7];
	//unsigned char i2c_read_value;
	
	while(TRUE){
		lcd.detect(&lcd);
		//ignore cancel stop enter delete run
		//TODO:: Please write your application code
		entrada=PINB;
		ler=~PIND&0xFF;
		cmd=PINF;
		
		lcd.gotoxy(&lcd,0,0);
		function.itoa(entrada,tmp);
		lcd.string(&lcd,lcd.resizestr(&lcd,tmp,3));
		
		lcd.gotoxy(&lcd,4,0);
		//function.itoa(&fsm,fsm.quant(&fsm),tmp);
		//function.itoa(fsm.feedback(&fsm),tmp);
		function.itoa(vmfsm[5],tmp);
		//function.itoa(&fsm,uart.tail(),tmp);
		lcd.string(&lcd,lcd.resizestr(&lcd,tmp,3));
		
		lcd.gotoxy(&lcd,8,0);
		//lcd.putch(&lcd,fsm.typeget(&fsm));
		function.itoa(analog.read(0),tmp);
		lcd.string(&lcd,lcd.resizestr(&lcd,tmp,4));
		
		lcd.gotoxy(&lcd,0,1);
		msg=uart.read(&uart);
		lcd.string(&lcd,lcd.resizestr(&lcd,msg,6));
		
		lcd.gotoxy(&lcd,6,1);
		//function.itoa(fsm.quant(&fsm),tmp);
		function.itoa(PORTC,tmp);
		//function.itoa(uart.rxtail(),tmp);
		lcd.string(&lcd,lcd.resizestr(&lcd,tmp,3));
		//lcd.gotoxy(&lcd,9,1);
		//function.itoa(&fsm,uart.GetRxTail(&uart),tmp);
		//lcd.string(&lcd,lcd.resizestr(&lcd,tmp,2));
		
		lcd.gotoxy(&lcd,10,1);
		//function.itoa(ler,tmp);
		function.itoa(analog.read(2),tmp);
		//function.itoa(uart.rxhead(),tmp);
		lcd.string(&lcd,lcd.resizestr(&lcd,tmp,4));
		/**to test analog library**/
		//function.itoa(analog.read(&analog,0),tmp);
		//function.itoa(function.signednumber(analog.read(&analog,16),10),tmp);
		//lcd.string(&lcd,lcd.resizestr(&lcd,tmp,4));
		/***trial*start***/
		
		// I2C
		// First we initial the pointer register to address 0x00
		// Start the I2C Write Transmission
		//i2c.start(&i2c, DS1307_ID, DS1307_ADDR, I2C_WRITE); // DS1307_ID
		// Start from Address 0x00
		//i2c.write(&i2c,DS1307_ADDR);
		// Stop I2C Transmission
		//i2c.stop(&i2c);
		// Start the I2C Read Transmission
		//i2c.start(&i2c, DS1307_ID, DS1307_ADDR, I2C_READ);
		//i2c.read(&i2c, &i2c_read_value, ACK);
		//can read has many times you want the slave increments address automticaly at each read procedure
		//i2c.stop(&i2c);
		
		/***trial***end***/
		
		switch(cmd){
			case 13: // stop
				lcd.gotoxy(&lcd,14,1);
				lcd.string(&lcd,"ST");
				continue;
			case 12: // delete all
				lcd.gotoxy(&lcd,14,1);
				lcd.string(&lcd,"DE");
				//fsm.deleteall(&fsm);
				continue;
			case 15: // run
				lcd.gotoxy(&lcd,14,1);
				lcd.string(&lcd,"RU");
				
				//data=fsm.read(&fsm,entrada,fsm.present(&fsm));
				vmfsm[0]=button_1.read(&button_1, entrada & 1, (PINC & 15));//1
				vmfsm[1]=button_2.read(&button_2, entrada & 3, vmfsm[0]);//3
				vmfsm[2]=button_3.read(&button_3, entrada & 5, vmfsm[1]);//5
				vmfsm[3]=button_4.read(&button_4, entrada & 7, vmfsm[2]);//2
				vmfsm[4]=button_5.read(&button_5, entrada & 2, vmfsm[3]);//4
				vmfsm[5]=button_6.read(&button_6, entrada & 4, vmfsm[4]);//4
				vmfsm[6]=button_7.read(&button_7, timer0.cmpm(&timer0,1000) & 1, vmfsm[5]);
				PORTC =	vmfsm[6];
				
				/***TIMER***/
				if(vmfsm[6]==10 || vmfsm[6]==9 || vmfsm[6]==0){
					timer0.start(&timer0, 1024);
				}else{
					timer0.stop(&timer0);
				}
				
				lcd.gotoxy(&lcd,13,0);
				function.itoa(data,tmp);
				
				//function.itoa(&fsm,uart.head(),tmp);
				lcd.string(&lcd,lcd.resizestr(&lcd,tmp,3));
				
				if(data!=rec[0]){ //oneshot
					rec[0]=data;
					uart.puts(lcd.resizestr(&lcd,tmp,4));
				}
				
				/*
				if(sfsm_data[2]!=rec[1]){//oneshot
					rec[1]=sfsm_data[2];
					//i2c.brain->instruction
					function.itoa(rec[1],tmp);
					uart.puts(lcd.resizestr(&lcd,tmp,4));
				}
				*/				
				/******/
				continue;
			case 5: // remove
				lcd.gotoxy(&lcd,14,1);
				lcd.string(&lcd,"RM");
				//fsm.remove(&fsm,entrada,fsm.present(&fsm));
				continue;
			case 9: // first stop then learn
				lcd.gotoxy(&lcd,14,1);
				lcd.string(&lcd,"EN");
				//fsm.learn(&fsm,entrada,ler,fsm.present(&fsm));
				continue;
			default:
				continue;
		}
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
