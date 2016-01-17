/*************************************************************************
Title:    ATMEGA128API
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: Atemga128API.c,v 0.2 2016/01/16 14:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR ATmega128, 
License:  GNU General Public License        
DESCRIPTION:
	API
USAGE:
	Datasheet Atmega128
NOTES:
    Atmega128
LICENSE:
    Copyright (C) 2014
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
COMMENT:
	
*************************************************************************/
/*
** Define
*/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/*
** library
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdarg.h>
/*
** constant and macro
*/

#ifndef GLOBAL_INTERRUPT_ENABLE
    #define GLOBAL_INTERRUPT_ENABLE 7
#endif
#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
/*
** library
*/
#include "Atmega128API.h"
/*
** VAR
*/
uint8_t PORT[8];
/***/
/*************************************************************************
ANALOG API START
*************************************************************************/
/*
** constant and macro
*/
// if using differential channels this value has to be greater than one
#define MAX_CHANNEL 8
/***TYPE 1***/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)	
	/******/
	#define ADC_SELECT ADMUX
	#define ADC_CONTROL ADCSRA
	#define MUX_MASK 31
	#define GLOBAL_INTERRUPT_ENABLE 7
	#define ATMEGA_ANALOG
	#define ANALOG_INTERRUPT ADC_vect
#else
 	#error "Not Atmega 128"
#endif
/*
** variable
*/
static volatile int ADC_VALUE[MAX_CHANNEL];
static volatile int ADC_CHANNEL_GAIN[MAX_CHANNEL];
static volatile int ADC_N_CHANNEL;
static volatile int ADC_SELECTOR;
static volatile int adc_sample;
static volatile int adc_tmp;
static volatile unsigned char adc_n_sample;
/*
** procedure and function header
*/
int ANALOG_read(int selection);
/*
** procedure and function
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... )
/*
* Interrupt running mode setup
* setup, and list of channels to be probed
*/
{
	/***LOCAL VARIABLES***/
	uint8_t tSREG;
	va_list list;
	int i;
	//inic variables
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	/***GLOBAL VARIABLES INICIALIZE***/
	ADC_N_CHANNEL=n_channel;
	ADC_SELECTOR=0;
	adc_n_sample=0;
	//PROTOTIPOS
	//int ANALOG_read(int channel);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	ANALOG analog;
	//import parametros
	//inic parameters
	//Direccionar apontadores para PROTOTIPOS
	analog.read=ANALOG_read;
		/******/
		ADC_SELECT&=~(3<<REFS0);
		switch( Vreff ){
			case 0:
				analog.VREFF=0;
				break;
			case 1:
				ADC_SELECT|=(1<<REFS0);
				analog.VREFF=1;
				break;
			case 3:
				ADC_SELECT|=(3<<REFS0);
				analog.VREFF=3;
				break;
			default:
				analog.VREFF=0;
				break;
		}
		//
		ADC_SELECT&=~(1<<ADLAR);
		/******/
		va_start(list, n_channel);
		for(i=0;i<n_channel;i++){
			ADC_CHANNEL_GAIN[i] = va_arg(list, int);
		}
		va_end(list);
		ADC_SELECT&=~MUX_MASK;
		ADC_SELECT|=(MUX_MASK & ADC_CHANNEL_GAIN[ADC_SELECTOR]);
		/******/
		ADC_CONTROL|=(1<<ADEN);
		ADC_CONTROL|=(1<<ADSC);
		ADC_CONTROL&=~(1<<ADFR);
		ADC_CONTROL|=(1<<ADIE);
		/******/
		ADC_CONTROL&=~(7<<ADPS0);
		switch( Divfactor ){
			case 2://1
				ADC_CONTROL|=(1<<ADPS0);
				analog.DIVISION_FACTOR=2;
				break;
			case 4://2
				ADC_CONTROL|=(1<<ADPS1);
				analog.DIVISION_FACTOR=4;
				break;
			case 8://3
				ADC_CONTROL|=(3<<ADPS0);
				analog.DIVISION_FACTOR=8;
				break;
			case 16://4
				ADC_CONTROL|=(1<<ADPS2);
				analog.DIVISION_FACTOR=16;
				break;
			case 32://5
				ADC_CONTROL|=(5<<ADPS0);
				analog.DIVISION_FACTOR=32;
				break;
			case 64://6
				ADC_CONTROL|=(6<<ADPS0);
				analog.DIVISION_FACTOR=64;
				break;
			case 128://7
				ADC_CONTROL|=(7<<ADPS0);
				analog.DIVISION_FACTOR=128;
				break;
			default:
				ADC_CONTROL|=(7<<ADPS0);
				analog.DIVISION_FACTOR=128;
				break;
		}		
		
	SREG=tSREG;
	SREG|=(1<<GLOBAL_INTERRUPT_ENABLE);
	/******/
	return analog;
}
int ANALOG_read(int selection)
/*
* 
* Returns selected Channel ADC_VALUE
*/
{
	uint8_t ADSC_FLAG;
	ADSC_FLAG=(1<<ADSC);
	if( !(ADC_CONTROL & ADSC_FLAG) ){
		//ADC_SELECT
		/***/
		ADC_CONTROL|=(1<<ADSC);
	}	
	return ADC_VALUE[selection];
}
/*
** interrupt
*/
ISR(ANALOG_INTERRUPT)
/*************************************************************************
Function: ANALOG interrupt
Purpose:  Read Analog Input
**************************************************************************/
{
	/******/
	adc_tmp=ADCL;
	adc_tmp|=(ADCH<<8);
	if(adc_n_sample < (1<<ADC_NUMBER_SAMPLE)){
		adc_n_sample++;
		adc_sample+=adc_tmp;
	}else{
		ADC_VALUE[ADC_SELECTOR]=adc_sample>>ADC_NUMBER_SAMPLE;
		adc_n_sample=adc_sample=0;
		/******/
		if(ADC_SELECTOR < ADC_N_CHANNEL)
			ADC_SELECTOR++;
		else
			ADC_SELECTOR=0;
		ADC_SELECT &= ~MUX_MASK;
		ADC_SELECT |= (ADC_CHANNEL_GAIN[ADC_SELECTOR] & MUX_MASK);
	}		
}
/***EOF***/
/***COMMENTS

***/
/*************************************************************************
ANALOG API END
*************************************************************************/
/*************************************************************************
EEPROM API START
*************************************************************************/
/*
** constant and macro
*/
/*
** variable
*/
/*
** procedure and function header
*/
/*
** procedure and function
*/
EEPROM EEPROMenable(void){
	EEPROM eeprom;
	eeprom.read_byte=eeprom_read_byte;
	eeprom.write_byte=eeprom_write_byte;
	eeprom.update_byte=eeprom_update_byte;
	eeprom.read_word=eeprom_read_word;
	eeprom.write_word=eeprom_write_word;
	eeprom.update_word=eeprom_update_word;
	//eeprom.read_dword=eeprom_read_dword;
	eeprom.write_dword=eeprom_write_dword;
	eeprom.write_dword=eeprom_write_dword;
	//eeprom.read_float=eeprom_read_float;
	eeprom.write_float=eeprom_write_float;
	eeprom.update_float=eeprom_update_float;
	eeprom.read_block=eeprom_read_block;
	eeprom.write_block=eeprom_write_block;
	eeprom.update_block=eeprom_update_block;
	return eeprom;
}
/*
** interrupt
*/
/***EOF***/
/*************************************************************************
EEPROM API END
*************************************************************************/
/*************************************************************************
FUNCTION API START
*************************************************************************/
/*
** variable
*/
char FUNCstr[16];
/*
** procedure and function header
*/
unsigned int Power(uint8_t base, uint8_t n);
int StringLength (const char string[]);
void Reverse(char s[]);
/******/
unsigned int FUNCmayia(unsigned int xi, unsigned int xf, uint8_t nbits);
uint8_t FUNCpinmatch(uint8_t match, uint8_t pin, uint8_t HL);
uint8_t FUNChmerge(uint8_t X, uint8_t Y);
uint8_t FUNClmerge(uint8_t X, uint8_t Y);
uint8_t FUNClh(uint8_t xi, uint8_t xf);
uint8_t FUNChl(uint8_t xi, uint8_t xf);
uint8_t FUNCdiff(uint8_t xi, uint8_t xf);
void FUNCswap(long *px, long *py);
void FUNCcopy(char to[], char from[]);
void FUNCsqueeze(char s[], int c);
void FUNCshellsort(int v[], int n);
void FUNCitoa(int32_t n, char s[]);
int FUNCtrim(char s[]);
int FUNCpmax(int a1, int a2);
int FUNCgcd (int u, int v);
int FUNCstrToInt (const char string[]);
uint8_t FUNCfilter(uint8_t mask, uint8_t data);
unsigned int FUNCticks(unsigned int num);
int FUNCtwocomptoint8bit(int twoscomp);
int FUNCtwocomptoint10bit(int twoscomp);
int FUNCtwocomptointnbit(int twoscomp, uint8_t nbits);
char FUNCdec2bcd(char num);
char FUNCbcd2dec(char num);
char* FUNCresizestr(char *string, int size);
long FUNCtrimmer(long x, long in_min, long in_max, long out_min, long out_max);
unsigned char FUNCbcd2bin(unsigned char val);
unsigned char FUNCbin2bcd(unsigned val);
long FUNCgcd1(long a, long b);
uint8_t FUNCpincheck(uint8_t port, uint8_t pin);
char* FUNCprint_binary(int number);
uint8_t leap_year_check(uint16_t year);
/*
** procedure and function
*/
FUNC FUNCenable( void )
{
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// struct object
	FUNC func;
	// function pointers
	func.power=Power;
	func.stringlength=StringLength;
	func.reverse=Reverse;
	func.mayia=FUNCmayia;
	func.pinmatch=FUNCpinmatch;
	func.hmerge=FUNChmerge;
	func.lmerge=FUNClmerge;
	func.lh=FUNClh;
	func.hl=FUNChl;
	func.diff=FUNCdiff;
	func.swap=FUNCswap;
	func.copy=FUNCcopy;
	func.squeeze=FUNCsqueeze;
	func.shellsort=FUNCshellsort;
	func.itoa=FUNCitoa;
	func.trim=FUNCtrim;
	func.pmax=FUNCpmax;
	func.gcd=FUNCgcd;
	func.strToInt=FUNCstrToInt;
	func.filter=FUNCfilter;
	func.ticks=FUNCticks;
	func.twocomptoint8bit=FUNCtwocomptoint8bit;
	func.twocomptoint10bit=FUNCtwocomptoint10bit;
	func.twocomptointnbit=FUNCtwocomptointnbit;
	func.dec2bcd=FUNCdec2bcd;
	func.bcd2dec=FUNCbcd2dec;
	func.resizestr=FUNCresizestr;
	func.trimmer=FUNCtrimmer;
	func.bcd2bin=FUNCbcd2bin;
	func.bin2bcd=FUNCbin2bcd;
	func.gcd1=FUNCgcd1;
	func.pincheck=FUNCpincheck;
	func.print_binary=FUNCprint_binary;
	SREG=tSREG;
	/******/
	return func;
}
// mayia
unsigned int FUNCmayia(unsigned int xi, unsigned int xf, uint8_t nbits)
{//magic formula
	unsigned int mask;
	unsigned int diff;
	unsigned int trans;
	mask=Power(2,nbits)-1;
	xi&=mask;
	xf&=mask;
	diff=xf^xi;
	trans=diff&xf;
	return (trans<<nbits)|diff;
}
//pinmatch
uint8_t FUNCpinmatch(uint8_t match, uint8_t pin, uint8_t HL)
{
	uint8_t result;
	result=match&pin;
	if(HL){
		if(result==match);
		else
			result=0;
	}else{
		if(result)
			result=0;
		else
			result=match;
	}
	return result;
}
// hmerge
uint8_t FUNChmerge(uint8_t X, uint8_t Y)
{
	return (X | Y);
}
// lmerge
uint8_t FUNClmerge(uint8_t X, uint8_t Y)
{
	return (X & Y);
}
// lh
uint8_t FUNClh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi^xf;
	i&=xf;
	return i;
}
// hl
uint8_t FUNChl(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xi;
	return i;
}
// diff
uint8_t FUNCdiff(uint8_t xi, uint8_t xf)
{
	return xf^xi;
}
// interchange *px and *py
void FUNCswap(long *px, long *py)
{
	long temp;
	temp = *px;
	*px = *py;
	*py = temp;
}
// copy: copy 'from' into 'to'; assume to is big enough
void FUNCcopy(char to[], char from[])
{
	int i;
	i = 0;
	while ((to[i] = from[i]) != '\0')
		++i;
}
// squeeze: delete all c from s
void FUNCsqueeze(char s[], int c)
{
	int i, j;
	for (i = j = 0; s[i] != '\0'; i++)
		if (s[i] != c)
			s[j++] = s[i];
		s[j] = '\0';
}
// shellsort: sort v[0]...v[n-1] into increasing order
void FUNCshellsort(int v[], int n)
{
	int gap, i, j, temp;
	for (gap = n/2; gap > 0; gap /= 2)
		for (i = gap; i < n; i++)
			for (j=i-gap; j>=0 && v[j]>v[j+gap]; j-=gap){
				temp = v[j];
				v[j] = v[j+gap];
				v[j+gap] = temp;
			}
}
// itoa: convert n to characters in s
void FUNCitoa(int32_t n, char s[])
{
	int i, sign;
	if ((sign = n) < 0) // record sign
		n = -n; // make n positive
	i = 0;
	do { // generate digits in reverse order
		s[i++] = n % 10 + '0'; // get next digit
	}while ((n /= 10) > 0); // delete it
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	Reverse(s);
}
// trim: remove trailing blanks, tabs, newlines
int FUNCtrim(char s[])
{
	int n;
	for (n = StringLength(s)-1; n >= 0; n--)
		if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
			break;
	s[n+1] = '\0';
	return n;
}
// larger number of two
int FUNCpmax(int a1, int a2)
{
	int biggest;
	if(a1 > a2){
		biggest = a1;
	}else{
		biggest = a2;
	}
	return biggest;
}
// common divisor
int FUNCgcd (int u, int v)
{
	int temp;
	while ( v != 0 ) {
		temp = u % v;
		u = v;
		v = temp;
	}
	return u;
}
// Function to convert a string to an integer
int FUNCstrToInt (const char string[])
{
	int i, intValue, result = 0;
	for ( i = 0; string[i] >= '0' && string[i] <= '9'; ++i ){
		intValue = string[i] - '0';
		result = result * 10 + intValue;
	}
	return result;
}
// filter
uint8_t FUNCfilter(uint8_t mask, uint8_t data)
{
	uint8_t Z;
	Z=mask & data;
	return Z;
}
// ticks
unsigned int FUNCticks(unsigned int num)
{
	unsigned int count;
	for(count=0;count<num;count++)
		;
	return count;
}
// Two's Complement function
int FUNCtwocomptoint8bit(int twoscomp){
  
  //Let's see if the byte is negative
  if (twoscomp & 0B10000000){
    //Invert
    twoscomp = ~twoscomp + 1;
	twoscomp = (twoscomp & 0B11111111);
    //Cast as int and multiply by negative one
    int value = (int)(twoscomp)*(-1);
    return value;
  }
  else{
    //Byte is non-negative, therefore convert to decimal and display
    //Make sure we are never over 1279
    twoscomp = (twoscomp & 0B01111111);
    //Cast as int and return
    int value = (int)(twoscomp);
    return value;
  }
}
// Two's Complement function, shifts 10 bit binary to signed integers (-512 to 512)
int FUNCtwocomptoint10bit(int twoscomp){
	
  //Let's see if the byte is negative
  if (twoscomp & 0x200){
    //Invert
    twoscomp = ~twoscomp + 1;
    twoscomp = (twoscomp & 0x3FF);
    //Cast as int and multiply by negative one
    int value = (int)(twoscomp)*(-1);
    return value;
  }
  else{
    //Serial.println("We entered the positive loop");
    //Byte is non-negative, therefore convert to decimal and display
    twoscomp = (twoscomp & 0x1FF);
    //Cast as int and return
    //Serial.println(twoscomp);
    int value = (int)(twoscomp);
    return value;
  }
}
// Two's Complement function, nbits
int FUNCtwocomptointnbit(int twoscomp, uint8_t nbits){
  unsigned int signmask;
  unsigned int mask;
  signmask = (1<<(nbits-1));
  mask=signmask-1;
  //Let's see if the number is negative
  if (twoscomp & signmask){
	twoscomp &= mask;
    twoscomp -= signmask;
  }else{
	  twoscomp &= mask;
  }
  return twoscomp;
}
// Convert Decimal to Binary Coded Decimal (BCD)
char FUNCdec2bcd(char num)
{
	return ((num/10 * 16) + (num % 10));
}
// Convert Binary Coded Decimal (BCD) to Decimal
char FUNCbcd2dec(char num)
{
	return ((num/16 * 10) + (num % 16));
}
char* FUNCresizestr(char *string, int size)
{
	int i;
	FUNCstr[size]='\0';
	for(i=0;i<size;i++){
		if(*(string+i)=='\0'){
			for(;i<size;i++){
				FUNCstr[i]=' ';
			}
			break;
		}
		FUNCstr[i]=*(string+i);
	}
	return FUNCstr;
}
long FUNCtrimmer(long x, long in_min, long in_max, long out_min, long out_max)
/*
	same as arduino map function.
*/
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// power: raise base to n-th power; n >= 0
unsigned int Power(uint8_t base, uint8_t n)
{
    unsigned int i, p;
    p = 1;
    for (i = 1; i <= n; ++i)
        p = p * base;
    return p;
}
// Function to count the number of characters in a string
int StringLength (const char string[])
{
	int count = 0;
	while ( string[count] != '\0' )
		++count;
	return count;
}
// reverse: reverse string s in place
void Reverse(char s[])
{
	int c, i, j;
	for (i = 0, j = StringLength(s)-1; i < j; i++, j--){
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
unsigned char FUNCbcd2bin(unsigned char val)
{
	return (val & 0x0f) + (val >> 4) * 10;
}
unsigned char FUNCbin2bcd(unsigned val)
{
	return ((val / 10) << 4) + val % 10;
}
long FUNCgcd1(long a, long b)
{
	long r;
	if (a < b)
		FUNCswap(&a, &b);
	if (!b){
		while ((r = a % b) != 0) {
			a = b;
			b = r;
		}
	}	
	return b;
}
uint8_t FUNCpincheck(uint8_t port, uint8_t pin)
{
	uint8_t lh;
	if(port & (1<<pin))
		lh=1;
	else
		lh=0;
	return lh;
}
char* FUNCprint_binary(int number)
{
	int i,c;
    for(i=128,c=0;i;i>>=1,c++){
	(number & i) ? (FUNCstr[c]='1') : (FUNCstr[c]='0');
	}
	FUNCstr[c]='\0';
	return FUNCstr;
}
uint8_t leap_year_check(uint16_t year){
	uint8_t i;
	if ( !(year%400))
    	i=1;
  	else if ( !(year%100))
    	i=0;
  	else if ( !(year%4) )
    	i=1;
  	else
    	i=0;
	return i;
}
/*
int gcd( int a, int b ) {
    int result ;
    // Compute Greatest Common Divisor using Euclid's Algorithm
    __asm__ __volatile__ ( "movl %1, %%eax;"
                          "movl %2, %%ebx;"
                          "CONTD: cmpl $0, %%ebx;"
                          "je DONE;"
                          "xorl %%edx, %%edx;"
                          "idivl %%ebx;"
                          "movl %%ebx, %%eax;"
                          "movl %%edx, %%ebx;"
                          "jmp CONTD;"
                          "DONE: movl %%eax, %0;" : "=g" (result) : "g" (a), "g" (b)
    );
    return result ;
}
//
float sinx( float degree ) {
    float result, two_right_angles = 180.0f ;
    // Convert angle from degrees to radians and then calculate sin value
    __asm__ __volatile__ ( "fld %1;"
                            "fld %2;"
                            "fldpi;"
                            "fmul;"
                            "fdiv;"
                            "fsin;"
                            "fstp %0;" : "=g" (result) : 
				"g"(two_right_angles), "g" (degree)
    ) ;
    return result ;
}
//
float cosx( float degree ) {
    float result, two_right_angles = 180.0f, radians ;
    // Convert angle from degrees to radians and then calculate cos value
    __asm__ __volatile__ ( "fld %1;"
                            "fld %2;"
                            "fldpi;"
                            "fmul;"
                            "fdiv;"
                            "fstp %0;" : "=g" (radians) : 
				"g"(two_right_angles), "g" (degree)
    ) ;
    __asm__ __volatile__ ( "fld %1;"
                            "fcos;"
                            "fstp %0;" : "=g" (result) : "g" (radians)
    ) ;
    return result ;
}
//
float square_root( float val ) {
    float result ;
    __asm__ __volatile__ ( "fld %1;"
                            "fsqrt;"
                            "fstp %0;" : "=g" (result) : "g" (val)
    ) ;
    return result ;
}
*/
/***pc use***
char* FUNCfltos(FILE* stream)
{
	int i, block, NBytes;
	char caracter;
	char* value=NULL;
	for(i=0, block=4, NBytes=0; (caracter=getc(stream)) != EOF; i++){
		if(i==NBytes){
			NBytes+=block;
			value=(char*)realloc(value, NBytes*sizeof(char));
			if(value==NULL){
				perror("fltos at calloc");
				break;
			}
		}
		*(value+i)=caracter;
		if(caracter=='\n'){
			*(value+i)='\0';
			break;
		}
	}
	return value;
}
char* FUNCftos(FILE* stream)
{
	int i, block, NBytes;
	char caracter;
	char* value=NULL;
	for(i=0, block=8, NBytes=0; (caracter=getc(stream)) != EOF; i++){
		if(i==NBytes){
			NBytes+=block;
			value=(char*)realloc(value, NBytes*sizeof(char));
			if(value==NULL){
				perror("ftos at calloc");
				break;
			}
		}
		*(value+i)=caracter;
	}
	return value;
}
int FUNCstrtotok(char* line,char* token[],const char* parser)
{
	int i;
	char *str;
	str=(char*)calloc(strlen(line),sizeof(char));
	for (i = 0, strcpy(str,line); ; i++, str = NULL) {
		token[i] = strtok(str, parser);
		if (token[i] == NULL)
			break;
		printf("%d: %s\n", i, token[i]);
	}
	free(str);
	return i;
}
char* FUNCputstr(char* str)
{
	int i; char* ptr;
	ptr = (char*)calloc(strlen(str), sizeof(char));
	if(ptr == NULL){
		perror("NULL!\n");
		return NULL;
	}
	for(i=0; (ptr[i] = str[i]); i++){
		if(ptr[i] == '\0')
			break;
	}
	return (ptr);
}
int FUNCgetnum(char* x)
{
	int num;
	if(!sscanf(x, "%d", &num))
		num=0;
	return num;
}
unsigned int FUNCgetnumv2(char* x)
{
	unsigned int RETURN;
	unsigned int value;
	unsigned int n;
	errno=0;
	n=sscanf(x,"%d",&value);
	if(n==1){
		RETURN=value;
	}else if(errno != 0){
		perror("getnum at sscanf");
		RETURN=0;
	}else{
		RETURN=0;
	}
	return RETURN;
}
int FUNCreadint(int nmin, int nmax)
{
	int num;
	int flag;
	for(flag=1; flag;){
		for( num=0; !scanf("%d",&num);getchar())
			;
		//printf("num: %d nmin: %d nmax: %d\n",num, nmin, nmax);
		if((num < nmin) || (num > nmax))
			continue;
		flag=0;
	}
		return num;
}
*/
/*
** interrupt
*/
/***EOF***/
/***COMMENTS

***/
/*************************************************************************
FUNCTION API END
*************************************************************************/
/*************************************************************************
INTERRUPT API START
*************************************************************************/
/*
** constant and macro
*/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)	
	/******/
	#define ATMEGA_INTERRUPT
	#define External_Interrupt_Control_Register_A EICRA
	#define External_Interrupt_Control_Register_B EICRB
	#define External_Interrupt_Mask_Register EIMSK
	#define External_Interrupt_Flag_Register EIFR
	#define MCU_Control_Status_Register MCUCSR
	#define MCU_Control_Status_Register_Mask 0X1F
#else
 	#error "Not Atmega 128"
#endif
/*
** variable
*/
/*
** procedure and function header
*/
void INTERRUPT_set(uint8_t channel, uint8_t sense);
void INTERRUPT_off(uint8_t channel);
uint8_t INTERRUPT_reset_status(void);
/*
** procedure and function
*/
INTERRUPT INTERRUPTenable(void)
/*
* setup blank
*/
{
	INTERRUPT interrupt;
	External_Interrupt_Mask_Register=0X00;
	/******/
	interrupt.set=INTERRUPT_set;
	interrupt.off=INTERRUPT_off;
	interrupt.reset_status=INTERRUPT_reset_status;
	return interrupt;
}
	uint8_t INTERRUPT_reset_status(void)
	{
		uint8_t reset,ret=0;
		reset=(MCU_Control_Status_Register & MCU_Control_Status_Register_Mask);
		switch(reset){
			case 1: // Power-On Reset Flag
				ret=0;
				MCU_Control_Status_Register&=~(1<<PORF);
				break;
			case 2: // External Reset Flag
				MCU_Control_Status_Register&=~(1<<EXTRF);
				ret=1;
				break;
			case 4: // Brown-out Reset Flag
				MCU_Control_Status_Register&=~(1<<BORF);
				ret=2;
				break;
			case 8: // Watchdog Reset Flag
				MCU_Control_Status_Register&=~(1<<WDRF);
				ret=3;
				break;
			case 16: // JTAG Reset Flag
				MCU_Control_Status_Register&=~(1<<JTRF);
				ret=4;
				break;
			default: // clear all status
				MCU_Control_Status_Register&=~(MCU_Control_Status_Register_Mask);
				break;
		}
		return ret;
	}
	void INTERRUPT_set(uint8_t channel, uint8_t sense)
	{
		switch( channel ){
			case 0: 
				External_Interrupt_Mask_Register&=~(1<<INT0);
				External_Interrupt_Control_Register_A&=~((1<<ISC01) | (1<<ISC00));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
					case 1: // The low level of INTn generates an interrupt request.
						break;
					case 2: // The falling edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=(1<<ISC01);
						break;
					case 3: // The rising edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=((1<<ISC01) | (1<<ISC00));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT0);
				break;
			case 1:
				External_Interrupt_Mask_Register&=~(1<<INT1);
				External_Interrupt_Control_Register_A&=~((1<<ISC11) | (1<<ISC10));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
					case 1: // The low level of INTn generates an interrupt request.
						break;
					case 2: // The falling edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=(1<<ISC11);
						break;
					case 3: // The rising edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=((1<<ISC11) | (1<<ISC10));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT1);
				break;
			case 2:
				External_Interrupt_Mask_Register&=~(1<<INT2);
				External_Interrupt_Control_Register_A&=~((1<<ISC21) | (1<<ISC20));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
					case 1: // The low level of INTn generates an interrupt request.
						break;
					case 2: // The falling edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=(1<<ISC21);
						break;
					case 3: // The rising edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=((1<<ISC21) | (1<<ISC20));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT2);
				break;
			case 3:
				External_Interrupt_Mask_Register&=~(1<<INT3);
				External_Interrupt_Control_Register_A&=~((1<<ISC31) | (1<<ISC30));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
					case 1: // The low level of INTn generates an interrupt request.
						break;
					case 2: // The falling edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=(1<<ISC31);
						break;
					case 3: // The rising edge of INTn generates asynchronously an interrupt request.
						External_Interrupt_Control_Register_A|=((1<<ISC31) | (1<<ISC30));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT3);
				break;
			case 4:
				External_Interrupt_Mask_Register&=~(1<<INT4);
				External_Interrupt_Control_Register_B&=~((1<<ISC41) | (1<<ISC40));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
						break;
					case 1: // Any logical change on INTn generates an interrupt request
						External_Interrupt_Control_Register_B|=(1<<ISC40);
						break;
					case 2: // The falling edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=(1<<ISC41);
						break;
					case 3: // The rising edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=((1<<ISC41) | (1<<ISC40));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT4);
				break;
			case 5:
				External_Interrupt_Mask_Register&=~(1<<INT5);
				External_Interrupt_Control_Register_B&=~((1<<ISC51) | (1<<ISC50));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
						break;
					case 1: // Any logical change on INTn generates an interrupt request
						External_Interrupt_Control_Register_B|=(1<<ISC50);
						break;
					case 2: // The falling edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=(1<<ISC51);
						break;
					case 3: // The rising edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=((1<<ISC51) | (1<<ISC50));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT5);
				break;
			case 6:
				External_Interrupt_Mask_Register&=~(1<<INT6);
				External_Interrupt_Control_Register_B&=~((1<<ISC61) | (1<<ISC60));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
						break;
					case 1: // Any logical change on INTn generates an interrupt request
						External_Interrupt_Control_Register_B|=(1<<ISC60);
						break;
					case 2: // The falling edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=(1<<ISC61);
						break;
					case 3: // The rising edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=((1<<ISC61) | (1<<ISC60));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT6);
				break;
			case 7:
				External_Interrupt_Mask_Register&=~(1<<INT7);
				External_Interrupt_Control_Register_B&=~((1<<ISC71) | (1<<ISC70));
				switch(sense){
					case 0: // The low level of INTn generates an interrupt request.
						break;
					case 1: // Any logical change on INTn generates an interrupt request
						External_Interrupt_Control_Register_B|=(1<<ISC70);
						break;
					case 2: // The falling edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=(1<<ISC71);
						break;
					case 3: // The rising edge between two samples of INTn generates an interrupt request.
						External_Interrupt_Control_Register_B|=((1<<ISC71) | (1<<ISC70));
						break;
					default: // The low level of INTn generates an interrupt request.
						break;
				}
				External_Interrupt_Mask_Register|=(1<<INT7);
				break;
			default:
				External_Interrupt_Mask_Register=0X00;
				break;
		}
	}
	void INTERRUPT_off(uint8_t channel)
	{
		switch( channel ){
			case 0: // desable
				External_Interrupt_Mask_Register&=~(1<<INT0);
				break;
			case 1: // desable
				External_Interrupt_Mask_Register&=~(1<<INT1);
				break;
			case 2: // desable
				External_Interrupt_Mask_Register&=~(1<<INT2);
				break;
			case 3: // desable
				External_Interrupt_Mask_Register&=~(1<<INT3);
				break;
			case 4: // desable
				External_Interrupt_Mask_Register&=~(1<<INT4);
				break;
			case 5: // desable
				External_Interrupt_Mask_Register&=~(1<<INT5);
				break;
			case 6: // desable
				External_Interrupt_Mask_Register&=~(1<<INT6);
				break;
			case 7: // desable
				External_Interrupt_Mask_Register&=~(1<<INT7);
				break;
			default: // all disable
				External_Interrupt_Mask_Register=0X00;
				break;
		}
	}

/*
** interrupt
*/
/***EOF***/
/*************************************************************************
INTERRUPT API END
*************************************************************************/
/*************************************************************************
LCD API START                    
*************************************************************************/
//CMD RS
#define INST 0
#define DATA 1
//ticks depends on CPU frequency this case 16Mhz
#define LCD_N_TICKS 1
/*
** variable
*/
volatile uint8_t *lcd0_DDR;
volatile uint8_t *lcd0_PIN;
volatile uint8_t *lcd0_PORT;
uint8_t lcd0_detect;
volatile uint8_t *lcd1_DDR;
volatile uint8_t *lcd1_PIN;
volatile uint8_t *lcd1_PORT;
uint8_t lcd1_detect;
/*
** procedure and function header
*/
void LCD0_inic(void);
void LCD0_write(char c, unsigned short D_I);
char LCD0_read(unsigned short D_I);
void LCD0_BF(void);
void LCD0_putch(char c);
char LCD0_getch(void);
void LCD0_string(const char* s); // RAW
void LCD0_string_size(const char* s, uint8_t size); // RAW
void LCD0_hspace(uint8_t n);
void LCD0_clear(void);
void LCD0_gotoxy(unsigned int x, unsigned int y);
void LCD0_strobe(unsigned int num);
void LCD0_reboot(void);
void LCD1_inic(void);
void LCD1_write(char c, unsigned short D_I);
char LCD1_read(unsigned short D_I);
void LCD1_BF(void);
void LCD1_putch(char c);
char LCD1_getch(void);
void LCD1_string(const char* s);
void LCD1_string_size(const char* s, uint8_t size); // RAW
void LCD1_hspace(uint8_t n);
void LCD1_clear(void);
void LCD1_gotoxy(unsigned int x, unsigned int y);
void LCD1_strobe(unsigned int num);
void LCD1_reboot(void);
unsigned int LCD_ticks(unsigned int num);
/*
** procedure and function
*/
LCD0 LCD0enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	LCD0 lcd0;
	//import parametros
	lcd0_DDR=ddr;
	lcd0_PIN=pin;
	lcd0_PORT=port;
	//inic variables
	*lcd0_DDR=0x00;
	*lcd0_PORT=0xFF;
	lcd0_detect=*lcd0_PIN & (1<<NC);
	//Direccionar apontadores para PROTOTIPOS
	lcd0.write=LCD0_write;
	lcd0.read=LCD0_read;
	lcd0.BF=LCD0_BF;
	lcd0.putch=LCD0_putch;
	lcd0.getch=LCD0_getch;
	lcd0.string=LCD0_string;
	lcd0.string_size=LCD0_string_size; // RAW
	lcd0.hspace=LCD0_hspace;
	lcd0.clear=LCD0_clear;
	lcd0.gotoxy=LCD0_gotoxy;
	lcd0.reboot=LCD0_reboot;
	//LCD INIC
	LCD0_inic();
	SREG=tSREG;
	//
	return lcd0;
}
void LCD0_inic(void)
{
	//LCD INIC
	*lcd0_DDR=(1<<RS)|(1<<RW)|(1<<EN)|(0<<NC);
	*lcd0_PORT=(1<<NC);
	/***INICIALIZACAO LCD**datasheet*/
	_delay_ms(40);
	LCD0_write(0x33,INST); //function set
	//_delay_ms(40);
	//lcd->write(lcd,0x33,INST); //function set
	_delay_us(39);
	LCD0_write(0x2B,INST); //function set
	_delay_us(39);
	LCD0_write(0x2B,INST); //function set
	//_delay_us(39);
	//lcd->write(0x2B,INST); //function set
	_delay_us(37);
	LCD0_write(0x0C,INST);// display on/off control
	_delay_us(37);
	LCD0_write(0x01,INST);// clear display
	_delay_ms(1.53);
	LCD0_write(0x06,INST);// entry mode set (crazy settings)
	_delay_us(37);
	/***INICIALIZATION END***/
	LCD0_write(0x1F,INST);// cursor or display shift
	LCD0_BF();
	LCD0_write(0x03,INST);// return home
	LCD0_BF();
}
void LCD0_write(char c, unsigned short D_I)
{
	*lcd0_PORT&=~(1<<RW);//lcd as input WRITE INSTRUCTION
	if(D_I) *lcd0_PORT|=(1<<RS); else *lcd0_PORT&=~(1<<RS);
	*lcd0_DDR|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//mcu as output
	*lcd0_PORT|=(1<<EN);
	if(c & 0x80) *lcd0_PORT|=1<<DB3; else *lcd0_PORT&=~(1<<DB3);
	if(c & 0x40) *lcd0_PORT|=1<<DB2; else *lcd0_PORT&=~(1<<DB2);
	if(c & 0x20) *lcd0_PORT|=1<<DB1; else *lcd0_PORT&=~(1<<DB1);
	if(c & 0x10) *lcd0_PORT|=1<<DB0; else *lcd0_PORT&=~(1<<DB0);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
	*lcd0_PORT|=(1<<EN);
	if(c & 0x08) *lcd0_PORT|=1<<DB3; else *lcd0_PORT&=~(1<<DB3);
	if(c & 0x04) *lcd0_PORT|=1<<DB2; else *lcd0_PORT&=~(1<<DB2);
	if(c & 0x02) *lcd0_PORT|=1<<DB1; else *lcd0_PORT&=~(1<<DB1);
	if(c & 0x01) *lcd0_PORT|=1<<DB0; else *lcd0_PORT&=~(1<<DB0);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
}
char LCD0_read(unsigned short D_I)
{
	char c=0x00;
	*lcd0_DDR&=~((1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3));//mcu as input
	*lcd0_PORT|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//pullup resistors
	*lcd0_PORT|=(1<<RW);//lcd as output READ INSTRUCTION
	if(D_I) *lcd0_PORT|=(1<<RS); else *lcd0_PORT&=~(1<<RS);
	*lcd0_PORT|=(1<<EN);
	if(*lcd0_PIN & (1<<DB3)) c|=1<<7; else c&=~(1<<7);
	if(*lcd0_PIN & (1<<DB2)) c|=1<<6; else c&=~(1<<6);
	if(*lcd0_PIN & (1<<DB1)) c|=1<<5; else c&=~(1<<5);
	if(*lcd0_PIN & (1<<DB0)) c|=1<<4; else c&=~(1<<4);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
	*lcd0_PORT|=(1<<EN);
	if(*lcd0_PIN & (1<<DB3)) c|=1<<3; else c&=~(1<<3);
	if(*lcd0_PIN & (1<<DB2)) c|=1<<2; else c&=~(1<<2);
	if(*lcd0_PIN & (1<<DB1)) c|=1<<1; else c&=~(1<<1);
	if(*lcd0_PIN & (1<<DB0)) c|=1<<0; else c&=~(1<<0);
	*lcd0_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
	return c;
}
void LCD0_BF(void)
{
	unsigned int i;
	char inst=0x80;
	for(i=0;0x80&inst;i++){
		inst=LCD0_read(INST);
		LCD_ticks(LCD_N_TICKS);
		if(i>10)// if something goes wrong
			break;
	}
}
void LCD0_putch(char c)
{
	LCD0_write(c,DATA);
	LCD0_BF();
}
char LCD0_getch(void)
{
	char c;
	c=LCD0_read(DATA);
	LCD0_BF();
	return c;
}
void LCD0_string(const char* s)
{
	char tmp;
	while(*s){
		tmp=*(s++);
		LCD0_write(tmp,DATA);
		LCD0_BF();
	}
}
void LCD0_string_size(const char* s, uint8_t size)
{
	char tmp;
	uint8_t pos=0;
	while(*s){
		tmp=*(s++);
		pos++;
		if(pos>size) // 1 TO SIZE+1
			break;
		LCD0_write(tmp,DATA);
		LCD0_BF();
	}
	while(pos<size){ // TO SIZE
		pos++;
		LCD0_write(' ',DATA);
		LCD0_BF();
	}
}
void LCD0_hspace(uint8_t n)
{
	for(;n;n--){
		LCD0_write(' ',DATA);
		LCD0_BF();
	}
}
void LCD0_clear(void)
{
	LCD0_write(0x01,INST);
	LCD0_BF();
}
void LCD0_gotoxy(unsigned int x, unsigned int y)
{
	switch(y){
		case 0:
			LCD0_write((0x80+x),INST);
			LCD0_BF();
			break;
		case 1:
			LCD0_write((0xC0+x),INST);
			LCD0_BF();
			break;
		default:
			break;
	}
}
void LCD0_strobe(unsigned int num)
{
	*lcd0_PORT|=(1<<EN);
	LCD_ticks(num);
	*lcd0_PORT&=~(1<<EN);
}
void LCD0_reboot(void)
{
	//low high detect pin NC
	uint8_t i;
	uint8_t tmp;
	tmp=*lcd0_PIN & (1<<NC);
	i=tmp^lcd0_detect;
	i&=tmp;
	if(i)
		LCD0_inic();
	lcd0_detect=tmp;
}
/*******************************************************************/
LCD1 LCD1enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCAÇÂO MEMORIA PARA Estrutura
	LCD1 lcd1;
	//import parametros
	lcd1_DDR=ddr;
	lcd1_PIN=pin;
	lcd1_PORT=port;
	//inic variables
	*lcd1_DDR=0x00;
	*lcd1_PORT=0xFF;
	lcd1_detect=*lcd1_PIN & (1<<NC);
	//Direccionar apontadores para PROTOTIPOS
	lcd1.write=LCD1_write;
	lcd1.read=LCD1_read;
	lcd1.BF=LCD1_BF;
	lcd1.putch=LCD1_putch;
	lcd1.getch=LCD1_getch;
	lcd1.string=LCD1_string;
	lcd1.string_size=LCD1_string_size; // RAW
	lcd1.hspace=LCD1_hspace;
	lcd1.clear=LCD1_clear;
	lcd1.gotoxy=LCD1_gotoxy;
	lcd1.reboot=LCD1_reboot;
	//LCD INIC
	LCD1_inic();
	SREG=tSREG;
	//
	return lcd1;
}
void LCD1_inic(void)
{
	//LCD INIC
	*lcd1_DDR=(1<<RS)|(1<<RW)|(1<<EN)|(0<<NC);
	*lcd1_PORT=(1<<NC);
	/***INICIALIZACAO LCD**datasheet*/
	_delay_ms(40);
	LCD1_write(0x33,INST); //function set
	//_delay_ms(40);
	//lcd->write(lcd,0x33,INST); //function set
	_delay_us(39);
	LCD1_write(0x2B,INST); //function set
	_delay_us(39);
	LCD1_write(0x2B,INST); //function set
	//_delay_us(39);
	//lcd->write(0x2B,INST); //function set
	_delay_us(37);
	LCD1_write(0x0C,INST);// display on/off control
	_delay_us(37);
	LCD1_write(0x01,INST);// clear display
	_delay_ms(1.53);
	LCD1_write(0x06,INST);// entry mode set (crazy settings)
	_delay_us(37);
	/***INICIALIZATION END***/
	LCD1_write(0x1F,INST);// cursor or display shift
	LCD1_BF();
	LCD1_write(0x03,INST);// return home
	LCD1_BF();
}
void LCD1_write(char c, unsigned short D_I)
{
	*lcd1_PORT&=~(1<<RW);//lcd as input WRITE INSTRUCTION
	if(D_I) *lcd1_PORT|=(1<<RS); else *lcd1_PORT&=~(1<<D_I);
	*lcd1_DDR|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//mcu as output
	*lcd1_PORT|=(1<<EN);
	if(c & 0x80) *lcd1_PORT|=1<<DB3; else *lcd1_PORT&=~(1<<DB3);
	if(c & 0x40) *lcd1_PORT|=1<<DB2; else *lcd1_PORT&=~(1<<DB2);
	if(c & 0x20) *lcd1_PORT|=1<<DB1; else *lcd1_PORT&=~(1<<DB1);
	if(c & 0x10) *lcd1_PORT|=1<<DB0; else *lcd1_PORT&=~(1<<DB0);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
	*lcd1_PORT|=(1<<EN);
	if(c & 0x08) *lcd1_PORT|=1<<DB3; else *lcd1_PORT&=~(1<<DB3);
	if(c & 0x04) *lcd1_PORT|=1<<DB2; else *lcd1_PORT&=~(1<<DB2);
	if(c & 0x02) *lcd1_PORT|=1<<DB1; else *lcd1_PORT&=~(1<<DB1);
	if(c & 0x01) *lcd1_PORT|=1<<DB0; else *lcd1_PORT&=~(1<<DB0);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
}
char LCD1_read(unsigned short D_I)
{
	char c=0x00;
	*lcd1_DDR&=~((1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3));//mcu as input
	*lcd1_PORT|=(1<<DB0)|(1<<DB1)|(1<<DB2)|(1<<DB3);//pullup resistors
	*lcd1_PORT|=(1<<RW);//lcd as output READ INSTRUCTION
	if(D_I) *lcd1_PORT|=(1<<RS); else *lcd1_PORT&=~(1<<D_I);
	*lcd1_PORT|=(1<<EN);
	if(*lcd1_PIN & (1<<DB3)) c|=1<<7; else c&=~(1<<7);
	if(*lcd1_PIN & (1<<DB2)) c|=1<<6; else c&=~(1<<6);
	if(*lcd1_PIN & (1<<DB1)) c|=1<<5; else c&=~(1<<5);
	if(*lcd1_PIN & (1<<DB0)) c|=1<<4; else c&=~(1<<4);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
	*lcd1_PORT|=(1<<EN);
	if(*lcd1_PIN & (1<<DB3)) c|=1<<3; else c&=~(1<<3);
	if(*lcd1_PIN & (1<<DB2)) c|=1<<2; else c&=~(1<<2);
	if(*lcd1_PIN & (1<<DB1)) c|=1<<1; else c&=~(1<<1);
	if(*lcd1_PIN & (1<<DB0)) c|=1<<0; else c&=~(1<<0);
	*lcd1_PORT&=~(1<<EN);
	LCD_ticks(LCD_N_TICKS);
	return c;
}
void LCD1_BF(void)
{
	unsigned int i;
	char inst=0x80;
	for(i=0;0x80&inst;i++){
		inst=LCD1_read(INST);
		LCD_ticks(LCD_N_TICKS);
		if(i>10)// if something goes wrong
			break;
	}
}
void LCD1_putch(char c)
{
	LCD1_write(c,DATA);
	LCD1_BF();
}
char LCD1_getch(void)
{
	char c;
	c=LCD1_read(DATA);
	LCD1_BF();
	return c;
}
void LCD1_string(const char* s)
{
	char tmp;
	while(*s){
		tmp=*(s++);
		LCD1_write(tmp,DATA);
		LCD1_BF();
	}
}
void LCD1_string_size(const char* s, uint8_t size)
{
	char tmp;
	uint8_t pos=0;
	while(*s){
		tmp=*(s++);
		pos++;
		if(pos>size)
			break;
		LCD1_write(tmp,DATA);
		LCD1_BF();
	}
	while(pos<size){
		pos++;
		LCD1_write(' ',DATA);
		LCD1_BF();
	}
}
void LCD1_hspace(uint8_t n)
{
	for(;n;n--){
		LCD1_write(' ',DATA);
		LCD1_BF();
	}
}
void LCD1_clear(void)
{
	LCD1_write(0x01,INST);
	LCD1_BF();
}
void LCD1_gotoxy(unsigned int x, unsigned int y)
{
	switch(y){
		case 0:
			LCD1_write((0x80+x),INST);
			LCD1_BF();
			break;
		case 1:
			LCD1_write((0xC0+x),INST);
			LCD1_BF();
			break;
		default:
			break;
	}
}
void LCD1_strobe(unsigned int num)
{
	*lcd1_PORT|=(1<<EN);
	LCD_ticks(num);
	*lcd1_PORT&=~(1<<EN);
}
void LCD1_reboot(void)
{
	//low high detect pin NC
	uint8_t i;
	uint8_t tmp;
	tmp=*lcd1_PIN & (1<<NC);
	i=tmp^lcd1_detect;
	i&=tmp;
	if(i)
		LCD1_inic();
	lcd1_detect=tmp;
}
unsigned int LCD_ticks(unsigned int num)
{
	unsigned int count;
	for(count=0;count<num;count++)
		;
	return count;
}
/*
** interrupt
*/
/***EOF***/
/***COMMENTS
DO NOT BOTHER SAYING THERE IS REPEATED CODE, I KNOW.
***/
/*************************************************************************
LCD API END                    
*************************************************************************/
/*************************************************************************
SPI API START
*************************************************************************/
/*
** constant and macro
*/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
	#define ATMEGA_SPI
	#define SPI_CONTROL_REGISTER SPCR
	#define SPI_STATUS_REGISTER SPSR
	#define SPI_DATA_REGISTER SPDR
	#define SPI_PORT PORTB
	#define SPI_DDR DDRB
	#define SPI_PIN PINB
	#define SPI_PIN_MASK 15
	#define DD_SS 0
	#define DD_SCK 1
	#define DD_MOSI 2
	#define DD_MISO 3
#else
	#error "Not Atmega 128"
#endif
/*
** variable
*/
/*
** procedure and function header
*/
void spi_default(void);
void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len);
void spi_transmit_sync (uint8_t * dataout, uint8_t len);
uint8_t spi_fast_shift (uint8_t data);
/*
** procedure and function
*/
SPI SPIenable(uint8_t master_slave_select, uint8_t data_order,  uint8_t data_modes, uint8_t prescaler)
{
	SPI spi;
	spi.transfer_sync = spi_transfer_sync;
	spi.transmit_sync = spi_transmit_sync;
	spi.fast_shift = spi_fast_shift;
	/***/
	SPI_DDR &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
	switch(master_slave_select){
		case SPI_MASTER_MODE:
			SPI_CONTROL_REGISTER |= (1<<MSTR);
			SPI_DDR |= (1<<DD_SS) | (1<<DD_MOSI) | (1<<DD_SCK);
			SPI_PORT |= (1<<DD_SS);
			break;
		case SPI_SLAVE_MODE:
			SPI_CONTROL_REGISTER |= (1<<MSTR);
			SPI_DDR |= (1<<DD_MISO);
			break;
		default:
			SPI_CONTROL_REGISTER |= (1<<MSTR);
			SPI_DDR |= (1<<DD_SS) | (1<<DD_MOSI) | (1<<DD_SCK);
			break;
	}
	switch(data_order){
		case SPI_LSB_DATA_ORDER:
			SPI_CONTROL_REGISTER |= (1<<DORD);
			break;
		case SPI_MSB_DATA_ORDER:
			SPI_CONTROL_REGISTER &= ~(1<<DORD);
			break;
		default:
			SPI_CONTROL_REGISTER &= ~(1<<DORD);
			break;
	}
	switch(data_modes){
		case 0:
			SPI_CONTROL_REGISTER &= ~((1<<CPOL) | (1<<CPHA));
			break;
		case 1:
			SPI_CONTROL_REGISTER |= (1<<CPHA);
			break;
		case 2:
			SPI_CONTROL_REGISTER |= (1<<CPOL);
			break;
		case 3:
			SPI_CONTROL_REGISTER |= (1<<CPOL) | (1<<CPHA);
			break;
		default:
			SPI_CONTROL_REGISTER &= ~((1<<CPOL) | (1<<CPHA));
			break;
	}
	switch(prescaler){
		case 2:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER &= ~((1<<SPR1) | (1<<SPR0));
			break;
		case 4:
			SPI_STATUS_REGISTER &= ~(1<<SPI2X);
			SPI_CONTROL_REGISTER &= ~((1<<SPR1) | (1<<SPR0));
			break;
		case 8:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR0);
			break;
		case 16:
			SPI_STATUS_REGISTER &= ~(1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR0);
			break;
		case 32:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR1);
			break;
		case 64:
			SPI_STATUS_REGISTER &= ~(1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR1);
			break;
		case 128:
			SPI_STATUS_REGISTER &= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR1) | (1<<SPR0);
			break;
		default:
			SPI_STATUS_REGISTER |= (1<<SPI2X);
			SPI_CONTROL_REGISTER |= (1<<SPR0);
			break;
	}
	SPI_CONTROL_REGISTER |= (1<<SPE);
	return spi;
}
void spi_default()
// Initialize pins for spi communication
{
    SPI_DDR &= ~((1<<DD_MOSI)|(1<<DD_MISO)|(1<<DD_SS)|(1<<DD_SCK));
    // Define the following pins as output
    SPI_DDR |= ((1<<DD_MOSI)|(1<<DD_SS)|(1<<DD_SCK)); 
    SPI_CONTROL_REGISTER =	((1<<SPE)|              // SPI Enable
							(0<<SPIE)|              // SPI Interupt Enable
							(0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
							(1<<MSTR)|              // Master/Slave select   
							(0<<SPR1)|(1<<SPR0)|    // SPI Clock Rate
							(0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
							(0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)
    SPI_STATUS_REGISTER =	(1<<SPI2X);             // Double Clock Rate  
}
void spi_transfer_sync (uint8_t * dataout, uint8_t * datain, uint8_t len)
// Shift full array through target device
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPI_DATA_REGISTER = dataout[i];
             while((SPI_STATUS_REGISTER & (1<<SPIF))==0); // polling, serial transfer is complete interrupt.
             datain[i] = SPI_DATA_REGISTER;
       }
}
void spi_transmit_sync (uint8_t * dataout, uint8_t len)
// Shift full array to target device without receiving any byte
{
       uint8_t i;      
       for (i = 0; i < len; i++) {
             SPI_DATA_REGISTER = dataout[i];
             while((SPI_STATUS_REGISTER & (1<<SPIF))==0); // polling, serial transfer is complete interrupt.
       }
}
uint8_t spi_fast_shift (uint8_t data)
// Clocks only one byte to target device and returns the received one
{
    SPI_DATA_REGISTER = data;
    while((SPI_STATUS_REGISTER & (1<<SPIF))==0)
		; // polling, serial transfer is complete interrupt.
    return SPI_DATA_REGISTER;
}
/*
** interrupt
*/
/***EOF***/
/*************************************************************************
SPI API END
*************************************************************************/
/*************************************************************************
TIMER API START
*************************************************************************/
/*
** constant and macro
*/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	/***0***/
	#define ATMEGA_TIMER_COUNTER
	#define TIMER_COUNTER0_CONTROL_REGISTER TCCR0
	#define TIMER_COUNTER0_REGISTER TCNT0
	#define TIMER_COUNTER0_COMPARE_REGISTER OCR0
	#define TIMER_COUNTER0_COMPARE_MATCH_INTERRUPT TIMER0_COMP_vect
	#define TIMER_COUNTER0_OVERFLOW_INTERRUPT TIMER0_OVF_vect
	/***1***/
	#define TIMER_COUNTER1A_CONTROL_REGISTER TCCR1A
	#define TIMER_COUNTER1B_CONTROL_REGISTER TCCR1B
	#define TIMER_COUNTER1C_CONTROL_REGISTER TCCR1C
	#define TIMER_COUNTER1_REGISTER TCNT1 // H and L register
	#define TIMER_COUNTER1A_COMPARE_REGISTER OCR1A
	#define TIMER_COUNTER1B_COMPARE_REGISTER OCR1B
	#define TIMER_COUNTER1C_COMPARE_REGISTER OCR1C
	#define TIMER_COUNTER1_INPUT_CAPTURE_REGISTER ICR1
	#define TIMER_COUNTER1A_COMPARE_MATCH_INTERRUPT TIMER1_COMPA_vect
	#define TIMER_COUNTER1B_COMPARE_MATCH_INTERRUPT TIMER1_COMPB_vect
	#define TIMER_COUNTER1C_COMPARE_MATCH_INTERRUPT TIMER1_COMPC_vect
	#define TIMER_COUNTER1_CAPTURE_EVENT_INTERRUPT TIMER1_CAPT_vect
	#define TIMER_COUNTER1_OVERFLOW_INTERRUPT TIMER1_OVF_vect
	/***2***/
	#define TIMER_COUNTER2_CONTROL_REGISTER TCCR2
	#define TIMER_COUNTER2_REGISTER TCNT2
	#define TIMER_COUNTER2_COMPARE_REGISTER OCR2
	#define TIMER_COUNTER2_COMPARE_MATCH_INTERRUPT TIMER2_COMP_vect
	#define TIMER_COUNTER2_OVERFLOW_INTERRUPT TIMER2_OVF_vect
	/***3***/
	#define TIMER_COUNTER3A_CONTROL_REGISTER TCCR3A
	#define TIMER_COUNTER3B_CONTROL_REGISTER TCCR3B
	#define TIMER_COUNTER3C_CONTROL_REGISTER TCCR3C
	#define TIMER_COUNTER3_REGISTER TCNT3 // H and L register
	#define TIMER_COUNTER3A_COMPARE_REGISTER OCR3A
	#define TIMER_COUNTER3B_COMPARE_REGISTER OCR3B
	#define TIMER_COUNTER3C_COMPARE_REGISTER OCR3C
	#define TIMER_COUNTER3_INPUT_CAPTURE_REGISTER ICR3
	#define TIMER_COUNTER3A_COMPARE_MATCH_INTERRUPT TIMER3_COMPA_vect
	#define TIMER_COUNTER3B_COMPARE_MATCH_INTERRUPT TIMER3_COMPB_vect
	#define TIMER_COUNTER3C_COMPARE_MATCH_INTERRUPT TIMER3_COMPC_vect
	#define TIMER_COUNTER3_CAPTURE_EVENT_INTERRUPT TIMER3_CAPT_vect
	#define TIMER_COUNTER3_OVERFLOW_INTERRUPT TIMER3_OVF_vect
	/***COMMON***/
	#define TIMER_COUNTER_STATUS_REGISTER ASSR
	#define TIMER_COUNTER_INTERRUPT_MASK_REGISTER TIMSK
	#define EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER ETIMSK
	#define TIMER_COUNTER_INTERRUPT_FLAG_REGISTER TIFR
	#define EXTENDED_TIMER_COUNTER_INTERRUPT_FLAG_REGISTER ETIFR
	#define TIMER_COUNTER_SPECIAL_FUNCTION_REGISTER SFIOR
	#define ASYNCHRONOUS_STATUS_REGISTER ASSR
	#define SPECIAL_FUNCTION_IO_REGISTER SFIOR
#else
	#error "Not Atmega 128"
#endif
/*
** variable
*/
unsigned char timer0_state;
unsigned char timer1_state;
unsigned char timer2_state;
unsigned char timer3_state;
/*
** procedure and function header
*/
void TIMER_COUNTER0_compoutmode(unsigned char compoutmode);
void TIMER_COUNTER0_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER0_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER0_compare(unsigned char compare);
void TIMER_COUNTER0_compareA(unsigned char compare);
void TIMER_COUNTER0_compareB(unsigned char compare);
void TIMER_COUNTER0_start(unsigned int prescaler);
void TIMER_COUNTER0_stop(void);
/******/
void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER1_compoutmodeC(unsigned char compoutmode);
void TIMER_COUNTER1_compareA(uint16_t compare);
void TIMER_COUNTER1_compareB(uint16_t compare);
void TIMER_COUNTER1_compareC(uint16_t compare);
void TIMER_COUNTER1_start(unsigned int prescaler);
void TIMER_COUNTER1_stop(void);
/******/
void TIMER_COUNTER2_compoutmode(unsigned char compoutmode);
void TIMER_COUNTER2_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER2_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER2_compare(unsigned char compare);
void TIMER_COUNTER2_compareA(unsigned char compare);
void TIMER_COUNTER2_compareB(unsigned char compare);
void TIMER_COUNTER2_start(unsigned int prescaler);
void TIMER_COUNTER2_stop(void);
/******/
void TIMER_COUNTER3_compoutmodeA(unsigned char compoutmode);
void TIMER_COUNTER3_compoutmodeB(unsigned char compoutmode);
void TIMER_COUNTER3_compoutmodeC(unsigned char compoutmode);
void TIMER_COUNTER3_compareA(uint16_t compare);
void TIMER_COUNTER3_compareB(uint16_t compare);
void TIMER_COUNTER3_compareC(uint16_t compare);
void TIMER_COUNTER3_start(unsigned int prescaler);
void TIMER_COUNTER3_stop(void);
/*
** procedure and function
*/
TIMER_COUNTER0 TIMER_COUNTER0enable(unsigned char wavegenmode, unsigned char interrupt)
/*
	PARAMETER SETTING
	wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
	interrupt: off; overflow; output compare; both; default - non.
*/
{
	TIMER_COUNTER0 timer0;
	timer0_state=0;
		TIMER_COUNTER0_CONTROL_REGISTER&=~((1<<WGM00) | (1<<WGM01));
		switch(wavegenmode){
			case 0: // Normal
				break;
			case 1: // PWM, Phase Correct
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM00);
				break;
			case 2: // CTC
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM01);
				break;
			case 3: // Fast PWM
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<WGM00) | (1<<WGM01);
				break;
			default:
				break;
		}
		TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~(1<<TOIE0);
		TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~(1<<OCIE0);
		switch(interrupt){
			case 0: 
				break;
			case 1:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE0);
				break;
			case 2:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE0);
				break;
			case 3:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE0);
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE0);
				break;
			default:
				break;
		}
		timer0.compoutmode=TIMER_COUNTER0_compoutmode;
		timer0.compare=TIMER_COUNTER0_compare;
		timer0.start=TIMER_COUNTER0_start;
		timer0.stop=TIMER_COUNTER0_stop;
	
	return timer0;
}
/*****************************************************************************************/
TIMER_COUNTER1 TIMER_COUNTER1enable(unsigned char wavegenmode, unsigned char interrupt)
/*
	PARAMETER SETTING
	wavegen mode: Normal; PWM, Phase Correct, 8-bit; PWM, Phase Correct, 9-bit; PWM, Phase Correct, 10-bit;
	CTC; Fast PWM, 8-bit; Fast PWM, 9-bit; Fast PWM, 10-bit; PWM, Phase and Frequency Correct; PWM, Phase and Frequency Correct;
	PWM, Phase Correct; PWM, Phase Correct; CTC; (Reserved); Fast PWM; Fast PWM.
	interrupt: off; overflow; output compare; both; default - non.
	for more information read datasheet.
*/
{
	TIMER_COUNTER1 timer1;
	timer1_state=0;
		TIMER_COUNTER1A_CONTROL_REGISTER&=~((1<<WGM11) | (1<<WGM10));
		TIMER_COUNTER1B_CONTROL_REGISTER&=~((1<<WGM13) | (1<<WGM12));
		switch(wavegenmode){
			case 0: // Normal
				break;
			case 1: // PWM, Phase Correct, 8-bit
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
				break;
			case 2:	// PWM, Phase Correct, 9-bit
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
				break;
			case 3:	// PWM, Phase Correct, 10-bit
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
				break;
			case 4:	// CTC
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
				break;
			case 5:	// Fast PWM, 8-bit
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
				break;
			case 6:	// Fast PWM, 9-bit
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
				break;
			case 7:	// Fast PWM, 10-bit
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM12);
				break;
			case 8:	// PWM, Phase and Frequency Correct
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
				break;
			case 9:	// PWM, Phase and Frequency Correct
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
				break;
			case 10: // PWM, Phase Correct
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
				break;
			case 11: // PWM, Phase Correct
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13);
				break;
			case 12: // CTC
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
				break;
			case 13: // (Reserved)
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM10);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
				break;
			case 14: // Fast PWM
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
				break;
			case 15: // Fast PWM
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<WGM11) | (1<<WGM10);
				TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<WGM13) | (1<<WGM12);
				break;
			default:
				break;
		}
		TIMER_COUNTER1A_CONTROL_REGISTER&=~((3<<COM1A0) | (3<<COM1B0) | (3<<COM1C0));
		TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TICIE1) | (1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1));
		EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~(1<<OCIE1C);
		switch(interrupt){
			case 0:
				break;
			case 1:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE1);
				break;
			case 2:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A);
				break;
			case 3:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1B);
				break;
			case 4:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
				break;
			case 5:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE1);
				break;
			case 6:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<TOIE1);
				break;
			case 7:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1B) | (1<<TOIE1);
				break;
			case 8:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE1);
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
				break;
			case 9:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE1) | (1<<TOIE1);
				break;
			case 10:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1);
				break;
			case 11:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B) | (1<<TOIE1);
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
				break;
			case 12:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1A) | (1<<OCIE1B);
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE1C);
				break;
			default:
				break;
		}
		//
		timer1.compoutmodeA=TIMER_COUNTER1_compoutmodeA;
		timer1.compoutmodeB=TIMER_COUNTER1_compoutmodeB;
		timer1.compoutmodeC=TIMER_COUNTER1_compoutmodeC;
		timer1.compareA=TIMER_COUNTER1_compareA;
		timer1.compareB=TIMER_COUNTER1_compareB;
		timer1.compareC=TIMER_COUNTER1_compareC;
		timer1.start=TIMER_COUNTER1_start;
		timer1.stop=TIMER_COUNTER1_stop;
	
	//
	return timer1;
}
/*****************************************************************************************/
TIMER_COUNTER2 TIMER_COUNTER2enable(unsigned char wavegenmode, unsigned char interrupt)
	/*
		PARAMETER SETTING
		wavegen mode: Normal; PWM phase correct; Fast PWM; default-Normasl;
		interrupt: off; overflow; output compare; both; default - non.
	*/
{
	TIMER_COUNTER2 timer2;
	timer2_state=0;
		TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<WGM20) | (1<<WGM21));
		switch(wavegenmode){
			case 0: // Normal
				break;
			case 1: // PWM, Phase Correct
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM20);
				break;
			case 2: // CTC
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM21);
				break;
			case 3: // Fast PWM
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<WGM20) | (1<<WGM21);
				break;
			default:
				break;
		}
		TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TOIE2) | (1<<OCIE2));
		switch(interrupt){
			case 0: 
				break;
			case 1:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE2);
				break;
			case 2:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE2);
				break;
			case 3:
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE2);
				TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE2);
				break;
			default:
				break;
		}
		timer2.compoutmode=TIMER_COUNTER2_compoutmode;
		timer2.compare=TIMER_COUNTER2_compare;
		timer2.start=TIMER_COUNTER2_start;
		timer2.stop=TIMER_COUNTER2_stop;
	
	return timer2;
}
	void TIMER_COUNTER0_start(unsigned int prescaler)
	/*
		PARAMETER SETTING
		Frequency oscilator devision factor or prescaler.
		prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /32 (From prescaler);
		clk T0S /64 (From prescaler); clk T0S /128 (From prescaler); clk T 0 S /256 (From prescaler);
		clk T 0 S /1024 (From prescaler); default - clk T 0 S /1024 (From prescaler).
	*/
	{
		if(timer0_state==0){ // oneshot
			TIMER_COUNTER0_COMPARE_REGISTER=0XFF;
			TIMER_COUNTER0_CONTROL_REGISTER&=~(7<<CS00); // No clock source. (Timer/Counter stopped)
			switch(prescaler){
				case 1: // clk T0S /(No prescaling)
					TIMER_COUNTER0_CONTROL_REGISTER|=(1<<CS00);
					break;
				case 8: // clk T0S /8 (From prescaler)
					TIMER_COUNTER0_CONTROL_REGISTER|=(1<<CS01);
					break;
				case 32: // clk T0S /32 (From prescaler)
					TIMER_COUNTER0_CONTROL_REGISTER|=(3<<CS00);
					break;
				case 64: // clk T0S /64 (From prescaler)
					TIMER_COUNTER0_CONTROL_REGISTER|=(4<<CS00);
					break;
				case 128: // clk T0S /128 (From prescaler)
					TIMER_COUNTER0_CONTROL_REGISTER|=(5<<CS00);
					break;
				case 256: // clk T 0 S /256 (From prescaler)
					TIMER_COUNTER0_CONTROL_REGISTER|=(6<<CS00);
					break;
				case 1024: // clk T 0 S /1024 (From prescaler)
					TIMER_COUNTER0_CONTROL_REGISTER|=(7<<CS00);
					break;
				default:
					TIMER_COUNTER0_CONTROL_REGISTER|=(7<<CS00);
					break;
			}
			timer0_state=1;
		}	
	}
	void TIMER_COUNTER0_compoutmode(unsigned char compoutmode)
	/*
		compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
		Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
		Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
		default-Normal port operation, OC0 disconnected.
	*/
	{
		TIMER_COUNTER0_CONTROL_REGISTER&=~((1<<COM00) | (1<<COM01));
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM00);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM01);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER0_CONTROL_REGISTER|=(1<<COM00) | (1<<COM01);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER0_compare(unsigned char compare)
	{
		TIMER_COUNTER0_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER0_stop(void)
	/*
		stops timer by setting prescaler to zero
	*/
	{
		TIMER_COUNTER0_CONTROL_REGISTER&=~(7<<CS00); // No clock source. (Timer/Counter stopped)
		TIMER_COUNTER0_REGISTER=0X00;
		timer0_state=0;
	}
	/*****************************************************************************************/
	void TIMER_COUNTER1_start(unsigned int prescaler)
	/*
		PARAMETER SETTING
		Frequency oscilator devision factor or prescaler.
		prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
		clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
		External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
	*/
	{
		if(timer1_state==0){ // oneshot
			TIMER_COUNTER1A_COMPARE_REGISTER=0XFFFF;
			TIMER_COUNTER1B_CONTROL_REGISTER&=~(7<<CS10); // No clock source. (Timer/Counter stopped)
			switch(prescaler){
				case 1: // clkI/O/1 (No prescaling
					TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS10);
					break;
				case 8: // clkI/O/8 (From prescaler)
					TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS11);
					break;
				case 64: // clkI/O/64 (From prescaler)
					TIMER_COUNTER1B_CONTROL_REGISTER|=(3<<CS10);
					break;
				case 256: // clkI/O/256 (From prescaler)
					TIMER_COUNTER1B_CONTROL_REGISTER|=(1<<CS12);
					break;
				case 1024: // clkI/O/1024 (From prescaler)
					TIMER_COUNTER1B_CONTROL_REGISTER|=(5<<CS10);
					break;
				case 3: // External clock source on Tn pin. Clock on falling edge
					TIMER_COUNTER1B_CONTROL_REGISTER|=(6<<CS10);
					break;
				case 5: // External clock source on Tn pin. Clock on rising edge
					TIMER_COUNTER1B_CONTROL_REGISTER|=(7<<CS10);
					break;
				default:
					TIMER_COUNTER1B_CONTROL_REGISTER|=(5<<CS10);
					break;
			}
			timer1_state=1;
		}	
	}
	void TIMER_COUNTER1_compoutmodeA(unsigned char compoutmode)
	{
		TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1A0);
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A0);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A1);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1A0) | (1<<COM1A1);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER1_compoutmodeB(unsigned char compoutmode)
	{
		TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1B0);
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B0);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B1);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1B0) | (1<<COM1B1);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER1_compoutmodeC(unsigned char compoutmode)
	{
		TIMER_COUNTER1A_CONTROL_REGISTER&=~(3<<COM1C0);
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1C0);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1C1);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER1A_CONTROL_REGISTER|=(1<<COM1C0) | (1<<COM1C1);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER1_compareA(uint16_t compare)
	{
		TIMER_COUNTER1A_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER1_compareB(uint16_t compare)
	{
		TIMER_COUNTER1B_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER1_compareC(uint16_t compare)
	{
		TIMER_COUNTER1C_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER1_stop(void)
	/*
		stops timer by setting prescaler to zero
	*/
	{
		TIMER_COUNTER1B_CONTROL_REGISTER&=~(7<<CS10); // No clock source. (Timer/Counter stopped)
		TIMER_COUNTER1_REGISTER=0X0000;
		timer1_state=0;
	}
	/*****************************************************************************************/
	void TIMER_COUNTER2_start(unsigned int prescaler)
	/*
		PARAMETER SETTING
		Frequency oscilator devision factor or prescaler.
		prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
		clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
		External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
	*/
	{
		if(timer2_state==0){ // oneshot
			TIMER_COUNTER2_COMPARE_REGISTER=0XFF;
			TIMER_COUNTER2_CONTROL_REGISTER&=~(7<<CS20); // No clock source. (Timer/Counter stopped)
			switch(prescaler){
				case 1: // clkI/O/(No prescaling)
					TIMER_COUNTER2_CONTROL_REGISTER|=(1<<CS20);
					break;
				case 8: // clkI/O/8 (From prescaler)
					TIMER_COUNTER2_CONTROL_REGISTER|=(1<<CS21);
					break;
				case 64: // clkI/O/64 (From prescaler)
					TIMER_COUNTER2_CONTROL_REGISTER|=(3<<CS20);
					break;
				case 256: // clkI/O/256 (From prescaler)
					TIMER_COUNTER2_CONTROL_REGISTER|=(1<<CS22);
					break;
				case 1024: // clkI/O/1024 (From prescaler)
					TIMER_COUNTER2_CONTROL_REGISTER|=(5<<CS20);
					break;
				case 3: // External clock source on T2 pin. Clock on falling edge
					TIMER_COUNTER2_CONTROL_REGISTER|=(6<<CS20);
					break;
				case 5: // External clock source on T2 pin. Clock on rising edge
					TIMER_COUNTER2_CONTROL_REGISTER|=(7<<CS20);
					break;
				default:
					TIMER_COUNTER2_CONTROL_REGISTER|=(5<<CS20);
					break;
			}
			timer2_state=1;
		}	
	}
	void TIMER_COUNTER2_compoutmode(unsigned char compoutmode)
	/*
		compoutmode: Normal port operation, OC0 disconnected; Toggle OC0 on compare match; 
		Clear OC0 on compare match when up-counting. Set OC0 on compare match when downcounting. Clear OC0 on compare match;
		Set OC0 on compare match when up-counting. Clear OC0 on compare match when downcounting. Set OC0 on compare match ;
		default-Normal port operation, OC0 disconnected.
	*/
	{
		TIMER_COUNTER2_CONTROL_REGISTER&=~((1<<COM20) | (1<<COM21));
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM20);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM21);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER2_CONTROL_REGISTER|=(1<<COM20) | (1<<COM21);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER2_compare(unsigned char compare)
	{
		TIMER_COUNTER2_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER2_stop(void)
	/*
		stops timer by setting prescaler to zero
	*/
	{
		TIMER_COUNTER2_CONTROL_REGISTER&=~(7<<CS20); // No clock source. (Timer/Counter stopped)
		TIMER_COUNTER2_REGISTER=0X00;
		timer2_state=0;
	}
	/*****************************************************************************************/
	TIMER_COUNTER3 TIMER_COUNTER3enable(unsigned char wavegenmode, unsigned char interrupt)
	/*
		PARAMETER SETTING
		wavegen mode: Normal; PWM, Phase Correct, 8-bit; PWM, Phase Correct, 9-bit; PWM, Phase Correct, 10-bit;
		CTC; Fast PWM, 8-bit; Fast PWM, 9-bit; Fast PWM, 10-bit; PWM, Phase and Frequency Correct; PWM, Phase and Frequency Correct;
		PWM, Phase Correct; PWM, Phase Correct; CTC; (Reserved); Fast PWM; Fast PWM.
		interrupt: off; overflow; output compare; both; default - non.
		for more information read datasheet.
	*/
	{
		TIMER_COUNTER3 timer3;
		timer3_state=0;
		TIMER_COUNTER3A_CONTROL_REGISTER&=~((1<<WGM31) | (1<<WGM30));
		TIMER_COUNTER3B_CONTROL_REGISTER&=~((1<<WGM33) | (1<<WGM32));
		switch(wavegenmode){
			case 0: // Normal
				break;
			case 1: // PWM, Phase Correct, 8-bit
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
				break;
			case 2:	// PWM, Phase Correct, 9-bit
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
				break;
			case 3:	// PWM, Phase Correct, 10-bit
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
				break;
			case 4:	// CTC
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
				break;
			case 5:	// Fast PWM, 8-bit
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
				break;
			case 6:	// Fast PWM, 9-bit
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
				break;
			case 7:	// Fast PWM, 10-bit
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM32);
				break;
			case 8:	// PWM, Phase and Frequency Correct
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
				break;
			case 9:	// PWM, Phase and Frequency Correct
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
				break;
			case 10: // PWM, Phase Correct
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
				break;
			case 11: // PWM, Phase Correct
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33);
				break;
			case 12: // CTC
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
				break;
			case 13: // (Reserved)
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM30);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
				break;
			case 14: // Fast PWM
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
				break;
			case 15: // Fast PWM
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<WGM31) | (1<<WGM30);
				TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<WGM33) | (1<<WGM32);
				break;
			default:
				break;
		}
		TIMER_COUNTER3A_CONTROL_REGISTER&=~((3<<COM3A0) | (3<<COM3B0)| (3<<COM3C0));
		EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER&=~((1<<TICIE3) | (1<<OCIE3A) | (1<<OCIE3B) | (1<<TOIE3) | (1<<OCIE3C));
		switch(interrupt){
			case 0:
				break;
			case 1:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER=(1<<TOIE3);
				break;
			case 2:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A);
				break;
			case 3:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3B);
				break;
			case 4:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3C);
				break;
			case 5:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE3);
				break;
			case 6:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<TOIE3);
				break;
			case 7:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3B) | (1<<TOIE3);
				break;
			case 8:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TOIE3) | (1<<OCIE3C);
				break;
			case 9:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<TICIE3) | (1<<TOIE3);
				break;
			case 10:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<OCIE3B) | (1<<TOIE3);
				break;
			case 11:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<OCIE3B) | (1<<TOIE3) | (1<<OCIE3C);
				break;
			case 12:
				EXTENDED_TIMER_COUNTER_INTERRUPT_MASK_REGISTER|=(1<<OCIE3A) | (1<<OCIE3B) | (1<<OCIE3C);
				break;
			default:
				break;
		}
		//
		timer3.compoutmodeA=TIMER_COUNTER3_compoutmodeA;
		timer3.compoutmodeB=TIMER_COUNTER3_compoutmodeB;
		timer3.compoutmodeC=TIMER_COUNTER3_compoutmodeC;
		timer3.compareA=TIMER_COUNTER3_compareA;
		timer3.compareB=TIMER_COUNTER3_compareB;
		timer3.compareC=TIMER_COUNTER3_compareC;
		timer3.start=TIMER_COUNTER3_start;
		timer3.stop=TIMER_COUNTER3_stop;
		//
		return timer3;
	}
	void TIMER_COUNTER3_start(unsigned int prescaler)
	/*
		PARAMETER SETTING
		Frequency oscilator devision factor or prescaler.
		prescaler: clk T0S /(No prescaling); clk T0S /8 (From prescaler); clk T0S /64 (From prescaler);
		clk T0S /256 (From prescaler); clk T0S /1024 (From prescaler); External clock source on Tn pin. Clock on falling edge;
		External clock source on Tn pin. Clock on rising edge; default - clk T 0 S /1024 (From prescaler).
	*/
	{
		if(timer3_state==0){ // oneshot
			TIMER_COUNTER3A_COMPARE_REGISTER=0XFFFF;
			TIMER_COUNTER3B_CONTROL_REGISTER&=~(7<<CS30); // No clock source. (Timer/Counter stopped)
			switch(prescaler){
				case 1: // clkI/O/1 (No prescaling
					TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<CS30);
					break;
				case 8: // clkI/O/8 (From prescaler)
					TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<CS31);
					break;
				case 64: // clkI/O/64 (From prescaler)
					TIMER_COUNTER3B_CONTROL_REGISTER|=(3<<CS30);
					break;
				case 256: // clkI/O/256 (From prescaler)
					TIMER_COUNTER3B_CONTROL_REGISTER|=(1<<CS32);
					break;
				case 1024: // clkI/O/1024 (From prescaler)
					TIMER_COUNTER3B_CONTROL_REGISTER|=(5<<CS30);
					break;
				case 3: // External clock source on Tn pin. Clock on falling edge
					TIMER_COUNTER3B_CONTROL_REGISTER|=(6<<CS30);
					break;
				case 5: // External clock source on Tn pin. Clock on rising edge
					TIMER_COUNTER3B_CONTROL_REGISTER|=(7<<CS30);
					break;
				default:
					TIMER_COUNTER3B_CONTROL_REGISTER|=(5<<CS30);
					break;
			}
			timer3_state=1;
		}	
	}
	void TIMER_COUNTER3_compoutmodeA(unsigned char compoutmode)
	{
		TIMER_COUNTER3A_CONTROL_REGISTER&=~(3<<COM3A0);
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3A0);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3A1);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3A0) | (1<<COM3A1);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER3_compoutmodeB(unsigned char compoutmode)
	{
		TIMER_COUNTER3A_CONTROL_REGISTER&=~(3<<COM3B0);
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3B0);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3B1);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3B0) | (1<<COM3B1);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER3_compoutmodeC(unsigned char compoutmode)
	{
		TIMER_COUNTER3A_CONTROL_REGISTER&=~(3<<COM3C0);
		switch(compoutmode){ // see table 53, 54, 55 in datasheet for more information
			case 0: // Normal port operation, OC0 disconnected.
				break;
			case 1: // Reserved
					// Toggle OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3C0);
				break;
			case 2: // Clear OC0 on compare match when up-counting. Set OC0 on compare
					// match when downcounting.
					// Clear OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3C1);
				break;
			case 3: // Set OC0 on compare match when up-counting. Clear OC0 on compare
					// match when downcounting.
					// Set OC0 on compare match
				TIMER_COUNTER3A_CONTROL_REGISTER|=(1<<COM3C0) | (1<<COM3C1);
				break;
			default:
				break;
		}
	}
	void TIMER_COUNTER3_compareA(uint16_t compare)
	{
		TIMER_COUNTER3A_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER3_compareB(uint16_t compare)
	{
		TIMER_COUNTER3B_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER3_compareC(uint16_t compare)
	{
		TIMER_COUNTER3C_COMPARE_REGISTER=compare;
	}
	void TIMER_COUNTER3_stop(void)
	/*
		stops timer by setting prescaler to zero
	*/
	{
		TIMER_COUNTER3B_CONTROL_REGISTER&=~(7<<CS30); // No clock source. (Timer/Counter stopped)
		TIMER_COUNTER3_REGISTER=0X0000;
		timer3_state=0;
	}

/*
** interrupt
*/
/***EOF***/
/***COMMENTS
interrupt to be defined in MAIN file
Note if you configure to use interrupts and do not define them, program will block,
so be carefull when enabling, make sure correct parameters for specified aplication are applied.
***/
/*************************************************************************
TIMER API END
*************************************************************************/
/************************************************************************
I2C API START
testing phase
************************************************************************/
/* I2C clock in Hz */ 
#define I2C_SCL_CLOCK 100000UL
/*
** constant and macro
*/
/*******************************************************************************/
/*
** Status Codes for MASTER Transmitter Mode
*/
#define TWI_SENT_START 0X08 //A START condition has been transmitted
// SLA+W will be transmitted; ACK or NOT ACK will be received
// Status Codes for Master Transmitter Mode and Status Codes for Master Receiver Mode
#define TWI_SENT_REPEAT_START 0X10 //A repeated START condition has been transmitted
// SLA+W will be transmitted; ACK or NOT ACK will be received SLA+R will be transmitted;
// Logic will switch to master receiver mode
// Status Codes for Master Transmitter Mode and Status Codes for Master Receiver Mode
#define TWI_MASTER_SENT_SLA_W_RECEIVED_ACK 0X18 // SLA+W has been transmitted; ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received
// Repeated START will be transmitted STOP condition will be transmitted and
// TWSTO flag will be reset STOP condition followed by a START condition will be
// transmitted and TWSTO flag will be reset
#define TWI_MASTER_SENT_SLA_W_RECEIVED_NACK 0X20 // SLA+W has been transmitted; NOT ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define TWI_MASTER_SENT_DATABYTE_RECEIVED_ACK 0X28 // Data byte has been transmitted; ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received
// Repeated START will be transmitted STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define TWI_MASTER_SENT_DATABYTE_RECEIVED_NACK 0X30 // Data byte has been transmitted; NOT ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define TWI_ARBL_NACK 0X38 // Arbitration lost in SLA+W or data bytes. Arbitration lost in SLA+R or NOT ACK bit
// Two-wire Serial Bus will be released and not addressed slave mode entered
// A START condition will be transmitted when the bus be-comes free
// Two-wire Serial Bus will be released and not addressed
// slave mode will be entered
// A START condition will be transmitted when the bus
// becomes free
// Status Codes for Master Transmitter Mode and Status Codes for Master Receiver Mode
/*
** Status Codes for Master Receiver Mode
*/
#define TWI_MASTER_SENT_SLA_R_RECEIVED_ACK 0X40 // SLA+R has been transmitted; ACK has been received
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_MASTER_SENT_SLA_R_RECEIVED_NACK 0X48 // SLA+R has been transmitted; NOT ACK has been received
// Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define TWI_MASTER_RECEIVED_DATABYTE_SENT_ACK 0X50 // Data byte has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_MASTER_RECEIVED_DATABYTE_SENT_NACK 0X58 // Data byte has been received; NOT ACK has been returned
// Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
/*
** Status Codes for SLAVE Receiver Mode
*/
#define TWI_SLAVE_RECEIVED_OSLA_W_SENT_ACK 0X60 // Own SLA+W has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_SLAVE_MARBL_RECEIVED_OSLA_W_SENT_ACK 0X68 // Arbitration lost in SLA+R/W as master; own SLA+W has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_SLAVE_RECEIVED_GCALL_SENT_ACK 0X70 // General call address has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_SLAVE_MARBL_RECEIVED_GCALL_SENT_ACK 0X78 // Arbitration lost in SLA+R/W as master; General call address has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_SLAVE_PAOSLA_W_RECEIVED_DATABYTE_SENT_ACK 0X80 // Previously addressed with own SLA+W; data has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_SLAVE_PAOSLA_W_RECEIVED_DATABYTE_SENT_NACK 0X88 // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA;
// a START condition will be transmitted when the bus
// becomes free
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”;
// a START condition will be transmitted when the bus
// becomes free
#define TWI_SLAVE_PGCALL_RECEIVE_DATABYTE_SENT_ACK 0X90 // Previously addressed with general call; data has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define TWI_SLAVE_PGCALL_RECEIVE_DATABYTE_SENT_NACK 0X98 // Previously addressed with general call; data has been received; NOT ACK has been returned
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA;
// a START condition will be transmitted when the bus
// becomes free
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”;
// a START condition will be transmitted when the bus
// becomes free
#define TWI_SLAVE_RECEIVED_INTERRUPTION 0XA0 // A STOP condition or repeated START condition has been received while still addressed as slave
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA;
// a START condition will be transmitted when the bus
// becomes free
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”;
// a START condition will be transmitted when the bus
// becomes free
/*
** Status Codes for Slave Transmitter Mode
*/
#define TWI_SLAVE_RECEIVED_OSLA_R_SENT_ACK 0XA8 // Own SLA+R has been received; ACK has been returned
// Last data byte will be transmitted and NOT ACK should be received
// Data byte will be transmitted and ACK should be received
#define TWI_SLAVE_MARBL_RECEIVED_OSLA_R_SENT_ACK 0XB0 // Arbitration lost in SLA+R/W as master; own SLA+R has been received; ACK has been returned
// Last data byte will be transmitted and NOT ACK should be received
// Data byte will be transmitted and ACK should be received
#define TWI_SLAVE_SENT_DATABYTE_RECEIVED_ACK 0XB8 // Data byte in TWDR has been transmitted; ACK has been received
// Last data byte will be transmitted and NOT ACK should be received
// Data byte will be transmitted and ACK should be received
#define TWI_SLAVE_SENT_DATABYTE_RECEIVED_NACK 0XC0 // Data byte in TWDR has been transmitted; NOT ACK has been received
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA;
// a START condition will be transmitted when the bus
// becomes free
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”;
// a START condition will be transmitted when the bus becomes free
#define TWI_SLAVE_SENT_LAST_DATABYTE_RECEIVED_ACK 0XC8 // Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”
// Switched to the not addressed slave mode;
// no recognition of own SLA or GCA;
// a START condition will be transmitted when the bus becomes free
// Switched to the not addressed slave mode;
// own SLA will be recognized;
// GCA will be recognized if TWGCE = “1”;
// a START condition will be transmitted when the bus becomes free
/*
** Miscellaneous States
*/
#define TWI_TWINT_AT_ZERO 0XF8 // No relevant state information available; TWINT = “0”
// Wait or proceed current transfer
#define TWI_BUS_ERROR 0X00 // Bus error due to an illegal START or STOP condition
// Only the internal hardware is affected, no STOP condi-tion is sent on the bus. In all cases, the bus is released and TWSTO is cleared.
/*******************************************************************************/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
// I2C register renaming
#define TWI_BIT_RATE_REGISTER TWBR
#define TWI_CONTROL_REGISTER TWCR
#define TWI_STATUS_REGISTER TWSR
#define TWI_STATUS_MASK 0B11111000
#define TWI_DATA_REGISTER TWDR
#define TWI_ADDRESS_REGISTER TWAR
#define TWI_ADDRESS_REGISTER_MASK 0B11111110
#define i2c_read(ack)  (ack) ? i2c_readAck() : i2c_readNak();
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
	#define ATMEGA_I2C
	#define I2C_PORT PORTD
	#define I2C_DDR DDRD
	#define I2C_PIN PIND
	#define I2C_PIN_MASK 3
	#define TWI_PRESCALER_MASK 0B00000011
#else
	#error "Not Atmega 128"
#endif
/***/
#define TWI_START_CONDITION 0
#define TWI_DATA_NO_ACK 1
#define TWI_DATA_ACK 2
#define TWI_STOP_CONDITION 3
/*
** variable
*/
unsigned char twi_chip_id;
/*
** procedure function header
*/
void twi_transmit(unsigned char type);
unsigned char twi_status(void);
void twi_poll(unsigned int ticks);
void twi_start(unsigned char mode);
void twi_master_connect(unsigned char addr, unsigned char rw);
void twi_master_write(unsigned char data);
unsigned char twi_master_read(unsigned char request);
void twi_stop(void);
/*
** procedure and function
*/
I2C I2Cenable(unsigned char device_id, unsigned char prescaler)
{
	// local variables
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// object
	I2C i2c;
	// function pointers
	i2c.start=twi_start;
	i2c.master_connect=twi_master_connect;
	i2c.master_write=twi_master_write;
	i2c.master_read=twi_master_read;
	i2c.stop=twi_stop;
	/***/
	if(device_id>0 && device_id<128){
		twi_chip_id = device_id;
		TWI_ADDRESS_REGISTER = (twi_chip_id<<TWA0) | (1<<TWGCE);
	}else{
		twi_chip_id=(1<<TWA0);
	}
	#if defined( ATMEGA_I2C )
		I2C_DDR |= 0X03;
		I2C_PORT |= 0X03;
		switch(prescaler){
		case 1:
			TWI_STATUS_REGISTER &= ~TWI_PRESCALER_MASK;
			break;
		case 4:
			TWI_STATUS_REGISTER |= (1<<TWPS0);
			break;
		case 16:
			TWI_STATUS_REGISTER |= (2<<TWPS0);
			break;
		case 64:
			TWI_STATUS_REGISTER |= (3<<TWPS0);
			break;
		default:
			prescaler=1;
			TWI_STATUS_REGISTER &= ~TWI_PRESCALER_MASK;
			break;
	}
	TWI_BIT_RATE_REGISTER = ((F_CPU/I2C_SCL_CLOCK)-16)/(2*prescaler);
	#endif
	SREG=tSREG;
	SREG |= (1<<GLOBAL_INTERRUPT_ENABLE);
	/******/
	return i2c;
}
void twi_transmit(unsigned char type)
{
	switch(type){
		case TWI_START_CONDITION:	// Start Condition 0
			TWI_CONTROL_REGISTER |= (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
			break;	
		case TWI_DATA_NO_ACK:		// Data with No-Acknowledge 1
			TWI_CONTROL_REGISTER |= (1 << TWINT) | (1 << TWEN);
			break;
		case TWI_DATA_ACK:			// Data with Acknowledge 2
			TWI_CONTROL_REGISTER |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
			break;
		case TWI_STOP_CONDITION:	// Stop Condition 3
			TWI_CONTROL_REGISTER |= (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
			break;
		default:					// Stop Condition
			TWI_CONTROL_REGISTER = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
			break;
	}
}
unsigned char twi_status(void){
	unsigned char status;
	status = TWI_STATUS_REGISTER & TWI_STATUS_MASK;
	return status;
}
void twi_poll(unsigned int ticks)
{
	unsigned int i;
	for(i=0;!(TWI_CONTROL_REGISTER & (1<<TWINT));i++) // polling.
		if(i > ticks)
			break;
}
void twi_start(unsigned char mode)
{
	if(mode){
		TWI_CONTROL_REGISTER = (1<<TWEA) | (1<<TWEN) | (1<<TWIE);
		TWI_CONTROL_REGISTER &= ~((1<<TWSTA) | (1<<TWSTO));
	}else{
		twi_transmit(TWI_START_CONDITION);
		twi_poll(680);
	}	
}
void twi_master_connect(unsigned char addr, unsigned char rw)
{
	switch(twi_status()){
		case TWI_SENT_START:
			TWI_CONTROL_REGISTER &= ~(1<<TWSTA);
			if(addr>0 && addr<128){
				TWI_DATA_REGISTER=(addr<<TWA0) | rw;
			}else{
				TWI_DATA_REGISTER=0;
			}
			twi_transmit(TWI_DATA_NO_ACK);
			twi_poll(680);
			break;
		default:
			break;
	}
}
void twi_master_write(unsigned char data)
{
	switch(twi_status()){
		case TWI_MASTER_SENT_SLA_W_RECEIVED_ACK:
			TWI_DATA_REGISTER=data; // 8 bit data + ack = 9bit
			twi_transmit(TWI_DATA_NO_ACK);
			twi_poll(680);
			break;
		case TWI_MASTER_SENT_DATABYTE_RECEIVED_ACK:
			TWI_DATA_REGISTER=data; // 8 bit data + ack = 9bit
			twi_transmit(TWI_DATA_NO_ACK);
			twi_poll(680);
			break;
		default:
			break;
	}					
}
unsigned char twi_master_read(unsigned char request)
{
	unsigned char data='X';
	switch(twi_status()){
		case TWI_MASTER_SENT_SLA_R_RECEIVED_ACK:
			switch(request){
				case TWI_DATA_ACK:
					twi_transmit(TWI_DATA_ACK);
					twi_poll(680);
					data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
					break;
				case TWI_DATA_NO_ACK:
					twi_transmit(TWI_DATA_NO_ACK); // last byte to read
					twi_poll(680);
					data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
					break;
				default:
					twi_transmit(TWI_DATA_NO_ACK); // last byte to read
					twi_poll(680);
					data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
					break;
			}
		case TWI_MASTER_RECEIVED_DATABYTE_SENT_ACK:
			switch(request){
				case TWI_DATA_ACK:
					twi_transmit(TWI_DATA_ACK);
					twi_poll(680);
					data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
					break;
				case TWI_DATA_NO_ACK:
					twi_transmit(TWI_DATA_NO_ACK); // last byte to read
					twi_poll(680);
					data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
					break;
				default:
					twi_transmit(TWI_DATA_NO_ACK); // last byte to read
					twi_poll(680);
					data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
					break;
			}
		default:
			break;
	}
	return data;
}
void twi_stop(void)
{
	switch(twi_status()){
		case TWI_MASTER_SENT_SLA_W_RECEIVED_NACK:
			//TWI_DATA_REGISTER=data; // 8 bit data + ack = 9bit
		case TWI_MASTER_SENT_DATABYTE_RECEIVED_NACK:
			//TWI_DATA_REGISTER=data; // 8 bit data + ack = 9bit
		case TWI_MASTER_SENT_SLA_R_RECEIVED_NACK:
			//data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
		case TWI_MASTER_RECEIVED_DATABYTE_SENT_NACK:
			//data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
		case TWI_ARBL_NACK:
			//data=TWI_DATA_REGISTER; // 8 bit data + ack = 9bit
			twi_transmit(TWI_STOP_CONDITION);
		default:
			twi_transmit(TWI_STOP_CONDITION);
			break;
	}
}
/*
** interrupt
*/
//! I2C (TWI) interrupt service routine
/*
SIGNAL(SIG_2WIRE_SERIAL)
{
	//SLAVE CODE
	switch(twi_status()){
		case TWI_SLAVE_RECEIVED_OSLA_W_SENT_ACK:
			break;
		default:
			break;
	}
}
*/
/***EOF***/
/************************************************************************
I2C API END
testing phase
************************************************************************/
/*************************************************************************
UART API START
*************************************************************************/
/*
** constant and macro
*/
/* size of RX/TX buffers */
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1)
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
	#error "RX buffer size is not a power of 2"
#endif
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
	#error "TX buffer size is not a power of 2"
#endif
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
	/* ATmega with two USART */
	 #define ATMEGA_USART0
	 #define ATMEGA_USART1
	 #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
	 #define UART1_RECEIVE_INTERRUPT   USART1_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
	 #define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
	 #define UART0_STATUS   UCSR0A
	 #define UART0_CONTROL  UCSR0B
	 #define UART0_DATA     UDR0
	 #define UART0_UDRIE    UDRIE0
	 #define UART1_STATUS   UCSR1A
	 #define UART1_CONTROL  UCSR1B
	 #define UART1_DATA     UDR1
	 #define UART1_UDRIE    UDRIE1
#else
 #error "Not Atmega 128"
#endif
/*
** variable
*/
static volatile unsigned char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART_TxHead;
static volatile unsigned char UART_TxTail;
static volatile unsigned char UART_RxHead;
static volatile unsigned char UART_RxTail;
static volatile unsigned char UART_LastRxError;
static volatile unsigned char UART1_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART1_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART1_TxHead;
static volatile unsigned char UART1_TxTail;
static volatile unsigned char UART1_RxHead;
static volatile unsigned char UART1_RxTail;
static volatile unsigned char UART1_LastRxError;
int uart_index;
char uart_msg[UART_RX_BUFFER_SIZE];
int uart1_index;
char uart1_msg[UART_RX_BUFFER_SIZE];
/*
**	procedure and function header
*/
char* uart_read(void);
unsigned int uart_getc(void);
void uart_putc(unsigned char data);
void uart_puts(const char *s );
int uart_available(void);
void uart_flush(void);
unsigned char UART_Rx_pop(void);
void UART_Tx_push(unsigned char data);
/******/
char* uart1_read(void);
unsigned int uart1_getc(void);
void uart1_putc(unsigned char data);
void uart1_puts(const char *s );
int uart1_available(void);
void uart1_flush(void);
unsigned char UART1_Rx_pop(void);
void UART1_Tx_push(unsigned char data);
/*
** procedure and function
*/
/*************************************************************************
Function: UARTenable() 1
Purpose:  initialize UART and set baudrate
Input:    baudrate using macro UART_BAUD_SELECT()
Returns:  none
**************************************************************************/
UART UARTenable(unsigned int baudrate, unsigned int FDbits, unsigned int Stopbits, unsigned int Parity )
{
	/***LOCAL VARIABLES***/
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	/***GLOBAL VARIABLES***/
    UART_TxHead = 0;
    UART_TxTail = 0;
    UART_RxHead = 0;
    UART_RxTail = 0;
	uart_index=0;
	uart_msg[0]='\0';
	/***struct***/
	UART uart;
	//local variables
	uart.ubrr=baudrate;
	/***FUNCTION POINTER***/
	uart.read=uart_read;
	uart.getc=uart_getc;
	uart.putc=uart_putc;
	uart.puts=uart_puts;
	uart.available=uart_available;
	uart.flush=uart_flush;
		/* Set baud rate */
		if ( baudrate & 0x8000 ) 
		{
   			UART0_STATUS = (1<<U2X0);  //Enable 2x speed 
   			baudrate &= ~0x8000;
   		}
		UBRR0H = (unsigned char)(baudrate>>8);
		UBRR0L = (unsigned char) baudrate;
		/* Enable USART receiver and transmitter and receive complete interrupt */
		UART0_CONTROL = _BV(RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
		/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
		#ifdef URSEL0
			UCSR0C = (1<<URSEL0)|(3<<UCSZ00);
			uart.FDbits=8;
			uart.Stopbits=1;
			uart.Parity=0;
		#else
			switch(FDbits){
				case 9:
					UART0_CONTROL |= (1<<UCSZ02);
					UCSR0C |= (3<<UCSZ00);
					uart.FDbits=9;
					break;
				case 8:
					UART0_CONTROL &= ~(1<<UCSZ02);
					UCSR0C |= (3<<UCSZ00);
					uart.FDbits=8;
					break;
				case 7:	
					UART0_CONTROL &= ~(1<<UCSZ02);
					UCSR0C |= (1<<UCSZ01);
					UCSR0C &= ~(1<<UCSZ00);
					uart.FDbits=7;
					break;
				case 6:	
					UART0_CONTROL &= ~(1<<UCSZ02);
					UCSR0C &= ~(1<<UCSZ01);
					UCSR0C |= (1<<UCSZ00);
					uart.FDbits=6;
					break;
				case 5:	
					UART0_CONTROL &= ~(1<<UCSZ02);
					UCSR0C &= ~(3<<UCSZ00);
					uart.FDbits=5;
					break;
				default:
					UART0_CONTROL &= ~(1<<UCSZ02);
					UCSR0C |= (3<<UCSZ00);
					uart.FDbits=8;
					break;
			}
			switch(Stopbits){
				case 1:
					UCSR0C &= ~(1<<USBS0);
					uart.Stopbits=1;
					break;
				case 2:
					UCSR0C |= (1<<USBS0);
					uart.Stopbits=2;
					break;	
				default:
					UCSR0C &= ~(1<<USBS0);
					uart.Stopbits=1;
					break;
			}
			switch(Parity){
				case 0:
					UCSR0C &= ~(3<<UPM00);
					uart.Parity=0;
					break;
				case 2:
					UCSR0C |= (1<<UPM01);
					UCSR0C &= ~(1<<UPM00);
					uart.Parity=2;
					break;
				case 3:
					UCSR0C |= (3<<UPM00);
					uart.Parity=3;
					break;	
				default:
					UCSR0C &= ~(3<<UPM00);
					uart.Parity=0;
					break;
			}
		#endif
	SREG=tSREG;
	SREG|=(1<<GLOBAL_INTERRUPT_ENABLE);
	/******/
	return uart;
}// UARTenable
/*************************************************************************
Function: uart_getc()
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
unsigned int uart_getc(void)
{
    unsigned char data;
    if ( UART_RxHead == UART_RxTail ) {
        return UART_NO_DATA;   /* no data available */
    }
    /* calculate /store buffer index */
    UART_RxTail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
    /* get data from receive buffer */
    data = UART_RxBuf[UART_RxTail];
    return (UART_LastRxError << 8) + data;
}/* uart_getc */
/*************************************************************************
Function: uart_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:    byte to be transmitted
Returns:  none          
**************************************************************************/
void uart_putc(unsigned char data)
{
    UART_Tx_push(data);
    /* enable UDRE interrupt */
    UART0_CONTROL |= _BV(UART0_UDRIE);
}/* uart_putc */
/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to UART
Input:    string to be transmitted
Returns:  none          
**************************************************************************/
void uart_puts(const char *s )
{
    while (*s) 
      uart_putc(*s++);
}/* uart_puts */
/*************************************************************************
Function: uart_puts_p()
Purpose:  transmit string from program memory to UART
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart_puts_p(const char *progmem_s )
{
    register char c;
    while ( (c = pgm_read_byte(progmem_s++)) ) 
      uart_putc(c);
}/* uart_puts_p */
/*************************************************************************
Function: uart_available()
Purpose:  Determine the number of bytes waiting in the receive buffer
Input:    None
Returns:  Integer number of bytes in the receive buffer
**************************************************************************/
int uart_available(void)
{
	return (UART_RX_BUFFER_MASK + UART_RxHead - UART_RxTail) % UART_RX_BUFFER_MASK;
}/* uart_available */
/*************************************************************************
Function: uart_flush()
Purpose:  Flush bytes waiting the receive buffer.  Acutally ignores them.
Input:    None
Returns:  None
**************************************************************************/
void uart_flush(void)
{
	UART_RxHead = UART_RxTail;
}/* uart_flush */
/*************************************************************************
Function: uart_read(&uart)
Purpose:  Get Data from Circular Buffer
Input:    Address of struct UART
Returns:  Buffer Data
**************************************************************************/
char* uart_read(void)
{
	char* ret;
	ret="empty";
	if((UART_RxTail != UART_RxHead) && (uart_index < UART_RX_BUFFER_MASK)){
		uart_msg[uart_index]=UART_Rx_pop();
		uart_index++;
		uart_msg[uart_index]='\0';
	//max index = UART_RX_BUFFER_MASK therefore UART_RX_BUFFER_MASK-1 max caracters more implies overflow.
	}else{	
		uart_index=0;
		ret=uart_msg;
	}
	return ret;
}
unsigned char UART_Rx_pop(void)
{
	if( UART_RxHead == UART_RxTail )
		return (unsigned char)UART_NO_DATA;
	UART_RxTail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
	return UART_RxBuf[UART_RxTail];
}
void UART_Tx_push(unsigned char data)
{
    UART_TxHead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
    while ( UART_TxHead == UART_TxTail ){
		; /* wait for free space in buffer, Tx interrupt will send to free space [Poll] */
	}	
    UART_TxBuf[UART_TxHead] = data;
}
/*
** interrupt
*/
ISR(UART0_RECEIVE_INTERRUPT)
/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
{
    unsigned char tmphead;
	unsigned char data;
	unsigned char bit9;
    unsigned char usr;
    /* read UART status register and UART data register */
	usr  = UART0_STATUS;
    bit9 = UART0_CONTROL;
    bit9 = 0x01 & (bit9>>1);
    /* */
    UART_LastRxError = (usr & (_BV(FE0)|_BV(DOR0)) );
	/* calculate buffer index */ 
    tmphead = ( UART_RxHead + 1) & UART_RX_BUFFER_MASK;
    if ( tmphead == UART_RxTail ) {	
        /* error: receive buffer overflow */
        UART_LastRxError = UART_BUFFER_OVERFLOW >> 8; 
	}else{
		data = UART0_DATA;
        /* store new index */
        UART_RxHead = tmphead;
		if(UART_LastRxError){
			/* store indication data error in buffer */
			
			UART_RxBuf[tmphead] = 'X';
		}else{
			/* store received data in buffer */
			UART_RxBuf[tmphead] = data;
		}
    }
}
ISR(UART0_TRANSMIT_INTERRUPT)
/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
{
    unsigned char tmptail;
    if ( UART_TxHead == UART_TxTail) {
		/* tx buffer empty, disable UDRE interrupt */
        UART0_CONTROL &= ~_BV(UART0_UDRIE);
    }else{
		/* calculate and store new buffer index */
        tmptail = (UART_TxTail + 1) & UART_TX_BUFFER_MASK;
        UART_TxTail = tmptail;
		/* get one byte from buffer and write it to UART */
        UART0_DATA = UART_TxBuf[tmptail];  /* start transmission */
    }
}
/*
** these functions are only for ATmegas with two USART
*/
/*************************************************************************
Function: UART1enable() 2
Purpose:  initialize UART1 and set baudrate
Input:    baudrate using macro UART_BAUD_SELECT()
Returns:  none
**************************************************************************/
UART1 UART1enable(unsigned int baudrate, unsigned int FDbits, unsigned int Stopbits, unsigned int Parity )
{
	/***LOCAL VARIABLES***/
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	/***GLOBAL VARIABLES INIC***/
    UART1_TxHead = 0;
    UART1_TxTail = 0;
    UART1_RxHead = 0;
    UART1_RxTail = 0;
	uart1_index=0;
	uart1_msg[0]='\0';
	/***struct***/
	UART1 uart;
	//local variables
	uart.ubrr=baudrate;
	/***FUNCTION POINTER***/
	uart.read=uart1_read;
	uart.getc=uart1_getc;
	uart.putc=uart1_putc;
	uart.puts=uart1_puts;
	uart.available=uart1_available;
	uart.flush=uart1_flush;
    /* Set baud rate */
    if ( baudrate & 0x8000 ) 
    {
    	UART1_STATUS = (1<<U2X1);  //Enable 2x speed 
		baudrate &= ~0x8000;
    }
    UBRR1H = (unsigned char)(baudrate>>8);
    UBRR1L = (unsigned char) baudrate;
    /* Enable USART receiver and transmitter and receive complete interrupt */
    UART1_CONTROL = _BV(RXCIE1)|(1<<RXEN1)|(1<<TXEN1);
    /* Set frame format: asynchronous, 8data, no parity, 1stop bit */   
    #ifdef URSEL1
    	UCSR1C = (1<<URSEL1)|(3<<UCSZ10);
		uart.FDbits=8;
		uart.Stopbits=1;
		uart.Parity=0;
    #else
    /***Parameters***/
    	switch(FDbits){
			case 9:
				UART1_CONTROL |= (1<<UCSZ12);
				UCSR1C |= (3<<UCSZ10);
				uart.FDbits=9;
				break;
			case 8:
				UART1_CONTROL &= ~(1<<UCSZ12);
				UCSR1C |= (3<<UCSZ10);
				uart.FDbits=8;
				break;
			case 7:	
				UART1_CONTROL &= ~(1<<UCSZ12);
				UCSR1C |= (1<<UCSZ11);
				UCSR1C &= ~(1<<UCSZ10);
				uart.FDbits=7;
				break;
			case 6:	
				UART1_CONTROL &= ~(1<<UCSZ12);
				UCSR1C &= ~(1<<UCSZ11);
				UCSR1C |= (1<<UCSZ10);
				uart.FDbits=6;
				break;
			case 5:	
				UART1_CONTROL &= ~(1<<UCSZ12);
				UCSR1C &= ~(3<<UCSZ10);
				uart.FDbits=5;
				break;
			default:
				UART1_CONTROL &= ~(1<<UCSZ12);
				UCSR1C |= (3<<UCSZ10);
				uart.FDbits=8;
				break;
		}
		switch(Stopbits){
				case 1:
					UCSR1C &= ~(1<<USBS1);
					uart.Stopbits=1;
					break;
				case 2:
					UCSR1C |= (1<<USBS1);
					uart.Stopbits=2;
					break;	
				default:
					UCSR1C &= ~(1<<USBS1);
					uart.Stopbits=1;
					break;
		}
		switch(Parity){
			case 0://NONE
				UCSR1C &= ~(3<<UPM10);
				uart.Parity=0;
				break;
			case 2://EVEN
				UCSR1C |= (1<<UPM11);
				UCSR1C &= ~(1<<UPM10);
				uart.Parity=2;
				break;
			case 3://ODD
				UCSR1C |= (3<<UPM10);
				uart.Parity=3;
				break;	
			default:
				UCSR1C &= ~(3<<UPM10);
				uart.Parity=0;
				break;
		}
    #endif
	SREG=tSREG;
	SREG|=(1<<GLOBAL_INTERRUPT_ENABLE);
	/******/
	return uart;
}// UART1enable
/*************************************************************************
Function: uart1_getc()
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
unsigned int uart1_getc(void)
{
    unsigned char data;
    if ( UART1_RxHead == UART1_RxTail )
        return UART_NO_DATA;   /* no data available */
    /* calculate /store buffer index */
    UART1_RxTail = (UART1_RxTail + 1) & UART_RX_BUFFER_MASK;
    /* get data from receive buffer */
    data = UART1_RxBuf[UART1_RxTail];
    return (UART1_LastRxError << 8) + data;
}/* uart1_getc */
/*************************************************************************
Function: uart1_putc()
Purpose:  write byte to ringbuffer for transmitting via UART
Input:    byte to be transmitted
Returns:  none          
**************************************************************************/
void uart1_putc(unsigned char data)
{
    UART1_Tx_push(data);
    /* enable UDRE interrupt */
    UART1_CONTROL |= _BV(UART1_UDRIE);
}/* uart1_putc */
/*************************************************************************
Function: uart1_puts()
Purpose:  transmit string to UART1
Input:    string to be transmitted
Returns:  none          
**************************************************************************/
void uart1_puts(const char *s )
{
    while (*s) 
      uart1_putc(*s++);
}/* uart1_puts */
/*************************************************************************
Function: uart1_puts_p()
Purpose:  transmit string from program memory to UART1
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart1_puts_p(const char *progmem_s )
{
    register char c;
    while ( (c = pgm_read_byte(progmem_s++)) ) 
      uart1_putc(c);
}/* uart1_puts_p */
/*************************************************************************
Function: uart1_available()
Purpose:  Determine the number of bytes waiting in the receive buffer
Input:    None
Returns:  Integer number of bytes in the receive buffer
**************************************************************************/
int uart1_available(void)
{
	return (UART_RX_BUFFER_MASK + UART1_RxHead - UART1_RxTail) % UART_RX_BUFFER_MASK;
}/* uart1_available */
/*************************************************************************
Function: uart1_flush()
Purpose:  Flush bytes waiting the receive buffer.  Acutally ignores them.
Input:    None
Returns:  None
**************************************************************************/
void uart1_flush(void)
{
	UART1_RxHead = UART1_RxTail;
}/* uart1_flush */
/*************************************************************************
Function: uart1_read(&uart1)
Purpose:  Get Data from Circular Buffer
Input:    Address of struct UART1
Returns:  Buffer Data
**************************************************************************/
char* uart1_read(void)
{
	char* ret;
	ret="empty";
	if((UART1_RxTail != UART1_RxHead) && (uart1_index < UART_RX_BUFFER_MASK)){
		uart1_msg[uart1_index]=UART1_Rx_pop();
		uart1_index++;
		uart1_msg[uart1_index]='\0';
	}else{
		uart1_index=0;
		ret=uart1_msg;
	}
	return ret;
}
unsigned char UART1_Rx_pop(void)
{
	if( UART1_RxHead == UART1_RxTail )
		return (unsigned char)UART_NO_DATA;
	UART1_RxTail = (UART1_RxTail + 1) & UART_RX_BUFFER_MASK;
	return UART1_RxBuf[UART1_RxTail];
}
void UART1_Tx_push(unsigned char data)
{
    UART1_TxHead = (UART1_TxHead + 1) & UART_TX_BUFFER_MASK;
    while ( UART1_TxHead == UART1_TxTail ){
		; /* wait for free space in buffer, Tx interrupt will send to free space [Poll] */
	}	
    UART1_TxBuf[UART1_TxHead] = data;
}
/*
** interrupt
*/
SIGNAL(UART1_RECEIVE_INTERRUPT)
/*************************************************************************
Function: UART1 Receive Complete interrupt
Purpose:  called when the UART1 has received a character
**************************************************************************/
{
    unsigned char tmphead;
	unsigned char data;
	unsigned char bit9;
    unsigned char usr;
    /* read UART status register and UART data register */
	usr  = UART1_STATUS;
    bit9 = UART1_CONTROL;
    bit9 = 0x01 & (bit9>>1);
    /* */
    UART1_LastRxError = (usr & (_BV(FE1)|_BV(DOR1)));
	/* calculate buffer index */ 
    tmphead = ( UART1_RxHead + 1) & UART_RX_BUFFER_MASK;
    if ( tmphead == UART1_RxTail ) {	
        /* error: receive buffer overflow, caracter is lost*/
        UART1_LastRxError = UART_BUFFER_OVERFLOW >> 8; 
	}else{
		data = UART1_DATA;
        /* store new index */
        UART1_RxHead = tmphead;
		if(UART1_LastRxError){
			/* store indication data error in buffer */
			UART1_RxBuf[tmphead] = 'X';
		}else{
			/* store received data in buffer */
			UART1_RxBuf[tmphead] = data;
		}
    }
}
SIGNAL(UART1_TRANSMIT_INTERRUPT)
/*************************************************************************
Function: UART1 Data Register Empty interrupt
Purpose:  called when the UART1 is ready to transmit the next byte
**************************************************************************/
{
    if ( UART1_TxHead == UART1_TxTail ) {
        /* tx buffer empty, disable UDRE interrupt */
        UART1_CONTROL &= ~_BV(UART1_UDRIE);
    }else{
		/* calculate and store new buffer index */
        UART1_TxTail = (UART1_TxTail + 1) & UART_TX_BUFFER_MASK;
        /* get one byte from buffer and write it to UART */
		UART1_DATA = UART1_TxBuf[UART1_TxTail];  /* start transmission */
	}	
}

/***EOF***/
/*************************************************************************
UART API END
*************************************************************************/
/*************************************************************************
VFSM API START
*************************************************************************/
/*
** constant and macro
*/
#define VFSM_SIZE_BLOCK 3
#define VLOGIC_SIZE_BLOCK 3
/*
** variable
*/
/*
** procedure and function header
*/
uint8_t VFSM_read(struct VFSM *r, uint8_t mask, uint8_t input, uint8_t output);
uint8_t VLOGIC_read(struct VLOGIC *l, uint8_t mask, uint8_t input, uint8_t output);
/*
** procedure and function
*/
struct VFSM VFSMenable(uint8_t *veeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	//struct
	VFSM fsm;
	//Init vars
	fsm.eeprom=veeprom;
	cells=sizeeeprom/VFSM_SIZE_BLOCK;
	fsm.sizeeeprom=cells*VFSM_SIZE_BLOCK;
	fsm.input=0;
	//function pointers
	fsm.read=VFSM_read;
	/******/
	return fsm;
}
struct VLOGIC VLOGICenable(uint8_t *veeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	//struct
	VLOGIC logic;
	//Init vars
	logic.eeprom=veeprom;
	cells=sizeeeprom/VLOGIC_SIZE_BLOCK;
	logic.sizeeeprom=cells*VLOGIC_SIZE_BLOCK;
	//function pointers
	logic.read=VLOGIC_read;
	/******/
	return logic;
}
uint8_t VFSM_read(struct VFSM *r, uint8_t mask, uint8_t input, uint8_t output)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[VFSM_SIZE_BLOCK];
	uint8_t ByteofData;
	int keyfound;
	if(((mask & r->input) ^ (mask & input)) == mask){ // condition: all masked pins change state
		for(i1=0;i1<r->sizeeeprom;i1+=VFSM_SIZE_BLOCK){
			ByteofData=*(r->eeprom+i1);
			for(i2=0;i2<VFSM_SIZE_BLOCK;i2++){ // get block from eeprom
				ByteofData=*(r->eeprom+(i1+i2));
				block[i2]=ByteofData;
			}
			keyfound=(block[0]==output && block[1]==(mask & input)); // bool
			if(keyfound){
				//r->input=input;
				output=block[2];
				break;
			}else{
				// do nothing
			}
		}
		r->input=input; // importante X
	}
	// printf("Eoutput -> %d\n",r->present);
	return output;
}
uint8_t VLOGIC_read(struct VLOGIC *l, uint8_t mask, uint8_t input, uint8_t output)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[VLOGIC_SIZE_BLOCK];
	uint8_t ByteofData;
	int keyfound;
	// conditions: masked input is programmed
	for(i1=0;i1<l->sizeeeprom;i1+=VLOGIC_SIZE_BLOCK){
		ByteofData=*(l->eeprom+i1);
		for(i2=0;i2<VLOGIC_SIZE_BLOCK;i2++){ // get block from eeprom
			ByteofData=*(l->eeprom+(i1+i2));
			block[i2]=ByteofData;
		}
		keyfound=(block[0]==output && block[1]==(input & mask)); // bool
		if(keyfound){
			output=block[2];
			break;
		}else{
			// do nothing
		}
	}
	return output;
}
/*
** interrupt
*/
/***EOF***/
/*************************************************************************
VFSM API END
*************************************************************************/
/*************************************************************************
FSM API START
*************************************************************************/
/*
** constant and macro
*/
#define SFSM_SIZE_BLOCK 5
#define EFSM_SIZE_BLOCK 5
#define INPUT 0
#define INDIFF 1
#define INLH 2
#define FEEDBACK 3
#define PRESENT 4
/*
** variable
*/
struct LFSM lfsm;
struct SFSM sfsm;
/*
** procedure and function header
*/
uint8_t FSM_routine(uint8_t cmd);
uint8_t FSM_diff(uint8_t xi, uint8_t xf);
uint8_t EFSM_read(struct EFSM* fsm, uint8_t input, uint8_t feedback);
int EFSM_learn(struct EFSM *r, uint8_t input, uint8_t next, uint8_t feedback);
unsigned int EFSM_quant(struct EFSM *r);
int EFSM_remove(struct EFSM *r, uint8_t input, uint8_t present);
int EFSM_deleteall(struct EFSM *r);
uint8_t EFSM_feedback(struct EFSM *r);
uint8_t EFSM_present(struct EFSM *r);
char EFSM_typeget(struct EFSM *r);
/*
** procedure and function
*/
uint8_t FSM_routine(uint8_t cmd)
{
	unsigned int i1;
	unsigned int i2;
	unsigned char block[lfsm.sizeblock];
	unsigned char ByteofData;
	int keyfound;
	int diferenca;
	diferenca=FSM_diff(lfsm.command, cmd);
	if(diferenca){ // oneshot
		for(i1=0;i1<lfsm.sizeeeprom;i1+=lfsm.sizeblock){
			for(i2=0;i2<lfsm.sizeblock;i2++){ // get block from eeprom
				ByteofData=*(lfsm.eeprom+(i1+i2));
				block[i2]=ByteofData;
			}
			keyfound=(block[0]==cmd && block[1]==lfsm.instruction); // bool
			if(keyfound){
				lfsm.command=block[0];
				lfsm.feedback=block[1];
				lfsm.instruction=block[2];
				break;
			}else{
				lfsm.feedback=lfsm.instruction;
			}
		}
		lfsm.command=cmd; // importante
	}
	// printf("Eoutput -> %d\n",r->present);
	return lfsm.instruction;
}
// SFSM INIT
void SFSMinit(uint8_t *eeprom, unsigned int sizeeeprom)
{
	unsigned int cells;
	uint8_t i;
	//Init vars
	sfsm.eeprom=eeprom;
	sfsm.sizeblock=SFSM_SIZE_BLOCK;
	cells=sizeeeprom/sfsm.sizeblock;
	sfsm.sizeeeprom=cells*sfsm.sizeblock;
	for(i=0;i<SFSM_PAGES;i++)
		sfsm.state[i]=0;
}
/***read***/
uint8_t sfsm_read(uint8_t input, uint8_t feedback, uint8_t page)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[sfsm.sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int diferenca;
	diferenca=FSM_diff(sfsm.input[page],input);
	if(diferenca){
		for(i1=0;i1<sfsm.sizeeeprom;i1+=sfsm.sizeblock){
			ByteofData=*(sfsm.eeprom+i1);
			if(ByteofData==page){
				for(i2=0;i2<sfsm.sizeblock;i2++){//get block from eeprom
					ByteofData=*(sfsm.eeprom+(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==feedback); // bool
				if(keyfound){
					sfsm.state[page]=block[4];
					break;
				}else{
					// do nothing
				}
			}			
		}
		sfsm.input[page]=input; // very importante
	}
	//printf("Eoutput -> %d\n",r->present);
	return sfsm.state[page];
}
/***get***/
uint8_t sfsm_state(uint8_t page)
{
	//printf("FSMpresent: %d\n",r->record[PRESENT]);
	return sfsm.state[page];
}
/***diff***/
uint8_t FSM_diff(uint8_t xi, uint8_t xf)
{
	return xf^xi;
}
EFSM EFSMenable(unsigned int sizeeeprom, uint8_t prog)
{
	unsigned int cells;
	/***Declare Functions***/
	//struct
	EFSM fsm;
	//Init vars
	fsm.sizeblock=5;
	cells=sizeeeprom/fsm.sizeblock;
	fsm.sizeeeprom=cells*fsm.sizeblock;
	fsm.page=prog;
	fsm.record[INPUT]=0;
	fsm.record[INDIFF]=0;
	fsm.record[INLH]=0;
	fsm.record[FEEDBACK]=0;
	fsm.record[PRESENT]=0;
	fsm.type='X';
	//function pointers
	fsm.read=EFSM_read;
	fsm.learn=EFSM_learn;
	fsm.quant=EFSM_quant;
	fsm.remove=EFSM_remove;
	fsm.deleteall=EFSM_deleteall;
	fsm.feedback=EFSM_feedback;
	fsm.present=EFSM_present;
	fsm.typeget=EFSM_typeget;
	fsm.diff=FSM_diff;
	/******/
	return fsm;
}
/***read***/
uint8_t EFSM_read(struct EFSM *r, uint8_t input, uint8_t feedback)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[r->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int diferenca;
	diferenca=r->diff(r->record[INPUT],input);
	if(diferenca){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==r->page){
				for(i2=0;i2<r->sizeblock;i2++){//get block from eeprom
					ByteofData=eeprom_read_byte((uint8_t*)(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==feedback);//bool
				if(keyfound){
					r->record[INDIFF]=block[1];
					r->record[INLH]=block[2];
					r->record[FEEDBACK]=block[3];
					r->record[PRESENT]=block[4];
					r->type='=';
					break;
				}else{
					r->record[FEEDBACK]=r->record[PRESENT];
					r->type='X';
				}
			}
		}
		r->record[INPUT]=input;//importante
	}
	//printf("Eoutput -> %d\n",r->present);
	return r->record[PRESENT];
}
/***learn***/
int EFSM_learn(struct EFSM *r, uint8_t input, uint8_t next, uint8_t feedback)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[r->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int status=0;
	int diferenca;
	diferenca=r->diff(r->record[INPUT],input);
	if(diferenca){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==r->page){
				for(i2=0;i2<r->sizeblock;i2++){//get block from eeprom
					ByteofData=eeprom_read_byte((uint8_t*)(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==feedback);//bool
				if(keyfound){
					status=1;//not permited
					break;
				}
			}
			status=3;//not existente
		}
	}
	if(status==3){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==EMPTY){
				eeprom_update_byte((uint8_t*)i1,r->page);
				eeprom_update_byte((uint8_t*)(i1+1),diferenca);
				eeprom_update_byte((uint8_t*)(i1+2),(diferenca & input));
				eeprom_update_byte((uint8_t*)(i1+3),feedback);
				eeprom_update_byte((uint8_t*)(i1+4),next);
				status=2;//created
				break;
			}
			status=4;//not possible
		}
	}
	r->record[INPUT]=input;//see
	//printf("learn status: %d\n",status);
	return status;
}
/***quant***/
unsigned int EFSM_quant(struct EFSM *r)
{
	unsigned int i1;
	unsigned int programmed;
	uint8_t ByteofData;
	for(i1=0,programmed=0;i1<r->sizeeeprom;i1+=r->sizeblock){
		ByteofData=eeprom_read_byte((uint8_t*)i1);
		if(ByteofData!=EMPTY)
			programmed++;
	}
	//printf("there are %d programmed\n",programmed);
	return programmed;
}
/***remove***/
int EFSM_remove(struct EFSM *r, uint8_t input, uint8_t state)
{
	unsigned int i1;
	unsigned int i2;
	uint8_t block[r->sizeblock];
	uint8_t ByteofData;
	int keyfound;
	int status=0;
	int diferenca;
	diferenca=r->diff(r->record[INPUT],input);
	if(diferenca){
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData==r->page){
				for(i2=0;i2<r->sizeblock;i2++){//get block from eeprom
					ByteofData=eeprom_read_byte((uint8_t*)(i1+i2));
					block[i2]=ByteofData;
				}
				keyfound=(block[1]==diferenca && block[2]==(diferenca&input) && block[3]==state);//bool
				if(keyfound){
					eeprom_update_byte((uint8_t*)i1,EMPTY);
					status=1;//removed
					break;
				}
			}
		}
		r->record[INPUT]=input;
	}
	//printf("remove status: %d\n",status);
	return status;
}
/***deleteall***/
int EFSM_deleteall(struct EFSM *r)
{
	unsigned int i1;
	uint8_t ByteofData;
	int status=0;
	if(!status){//not removed
		for(i1=0;i1<r->sizeeeprom;i1+=r->sizeblock){
			ByteofData=eeprom_read_byte((uint8_t*)i1);
			if(ByteofData!=EMPTY){
				eeprom_update_byte((uint8_t*)i1,EMPTY);
				status=1;//all deleted
			}
		}
	}
	//printf("deleteall status: %d\n",status);
	return status;
}
/***get***/
uint8_t EFSM_present(struct EFSM *r)
{
	//printf("FSMpresent: %d\n",r->record[PRESENT]);
	return r->record[PRESENT];
}

uint8_t EFSM_feedback(struct EFSM *r)
{
	//printf("FSMfeedback: %d\n",r->record[FEEDBACK]);
	return r->record[FEEDBACK];
}
/***typeget***/
char EFSM_typeget(struct EFSM *r)
{
	return r->type;
}
/*
** interrupt
*/
/***EOF***/
/*************************************************************************
FSM API END
*************************************************************************/
/*************************************************************************
CLOCK API START
*************************************************************************/
/*
** constant and macro
*/
/*
** variable
*/
struct TIME time;
struct TIME laptime;
struct TIME alarmtime;
char CLOCK_timp[9];
uint8_t CLOCK_alarm_flag;
uint8_t CLOCK_compare_active;
/*
** procedure and function header
*/
void CLOCK_set(uint8_t hour, uint8_t minute, uint8_t second);
void CLOCK_increment(void);
void CLOCK_decrement(void);
uint8_t CLOCK_alarm(uint8_t hour, uint8_t minute, uint8_t second);
uint8_t CLOCK_second_count(uint16_t second);
void CLOCK_second_count_reset(void);
void CLOCK_second_count_stop(void);
void CLOCK_alarm_reset(void);
void CLOCK_alarm_stop(void);
char* CLOCK_show(void);
/*
** procedure and function
*/
CLOCK CLOCKenable(uint8_t hour, uint8_t minute, uint8_t second)
{
	CLOCK clock;
	time.hour=hour;
	time.minute=minute;
	time.second=second;
	CLOCK_alarm_flag=0X0F;
	CLOCK_compare_active=0X0F;
	clock.set=CLOCK_set;
	clock.increment=CLOCK_increment;
	clock.decrement=CLOCK_decrement;
	clock.alarm=CLOCK_alarm;
	clock.second_count=CLOCK_second_count;
	clock.second_count_reset=CLOCK_second_count_reset;
	clock.second_count_stop=CLOCK_second_count_stop;
	clock.alarm_reset=CLOCK_alarm_reset;
	clock.alarm_stop=CLOCK_alarm_stop;
	clock.show=CLOCK_show;
	return clock;
}
void CLOCK_set(uint8_t hour, uint8_t minute, uint8_t second)
{
	time.hour=hour;
	time.minute=minute;
	time.second=second;
}
void CLOCK_increment(void)
{
	time.second++;
	if(time.second>59){
		time.minute++;
		time.second=0;
		if(time.minute>59){
			time.hour++;
			time.minute=0;
			switch (HORA){
				case 24:
					if(time.hour>23)
						time.hour=0;
					break;
				case 12:
					if (time.hour>12)
						time.hour=1;
					break;
				default:
					if(time.hour>23)
						time.hour=0;
					break;
			}
		}
	}
	/******************************************************************************************************************/
	if(time.hour==laptime.hour && CLOCK_compare_active==4)
		CLOCK_compare_active=3;
	if(time.hour==laptime.hour && time.minute==laptime.minute && CLOCK_compare_active==3)
		CLOCK_compare_active=2;
	if(time.hour==laptime.hour && time.minute==laptime.minute && time.second==laptime.second && CLOCK_compare_active==2)
		CLOCK_compare_active=1;
	/******************************************************************************************************************/
	if(time.hour==alarmtime.hour && CLOCK_alarm_flag==4)
		CLOCK_alarm_flag=3;
	if(time.hour==alarmtime.hour && time.minute==alarmtime.minute && CLOCK_alarm_flag==3)
		CLOCK_alarm_flag=2;
	if(time.hour==alarmtime.hour && time.minute==alarmtime.minute && time.second==alarmtime.second && CLOCK_alarm_flag==2)
		CLOCK_alarm_flag=1;
}
void CLOCK_decrement(void)
{
	time.second--;
	if(time.second<0){
		time.minute--;
		time.second=59;
		if(time.minute<0){
			time.hour--;
			time.minute=59;
			switch (HORA){
				case 24:
					if(time.hour<0)
						time.hour=23;
					break;
				case 12:
					if (time.hour<1)
						time.hour=12;
					break;
				default:
					if(time.hour<0)
						time.hour=23;
					break;
			}
		}
	}
	/******************************************************************************************************************/
	if(time.hour==alarmtime.hour && CLOCK_alarm_flag==4)
		CLOCK_alarm_flag=3;
	if(time.hour==alarmtime.hour && time.minute==alarmtime.minute && CLOCK_alarm_flag==3)
		CLOCK_alarm_flag=2;
	if(time.hour==alarmtime.hour && time.minute==alarmtime.minute && time.second==alarmtime.second && CLOCK_alarm_flag==2)
		CLOCK_alarm_flag=1;		
}
uint8_t CLOCK_alarm(uint8_t hour, uint8_t minute, uint8_t second)
{
	if(!CLOCK_alarm_flag){
		alarmtime.hour=hour;
		alarmtime.minute=minute;
		alarmtime.second=second;
		CLOCK_alarm_flag=4;
	}	
	return CLOCK_alarm_flag;
}
uint8_t CLOCK_second_count(uint16_t second)
{
	uint16_t tmp;
	uint8_t tmp_1;
	uint8_t tmp_2;
	uint8_t tmp_3;
	if(!CLOCK_compare_active){
		tmp_3=(int8_t)(second % 60);
		tmp=second/60;
		tmp_2=(int8_t)(tmp % 60);
		tmp=tmp/60;
		tmp_1=(int8_t)tmp;
		laptime.second=(time.second + tmp_3) % 60;
		tmp_3=(time.second + tmp_3)/60;
		laptime.minute=(time.minute + tmp_2 + tmp_3) % 60;
		tmp_2=(time.minute + tmp_2 + tmp_3)/60;
		tmp_1=time.hour+ tmp_1 + tmp_2;
		if(tmp_1 > HORA)
			laptime.hour=HORA-tmp_1;
		else
			laptime.hour=tmp_1;
		CLOCK_compare_active=4;
	}
	return CLOCK_compare_active;
}
void CLOCK_alarm_reset(void)
{
	CLOCK_alarm_flag=0;
}
void CLOCK_alarm_stop(void)
{
	CLOCK_alarm_flag=0X0F;
}
void CLOCK_second_count_reset(void)
{
	CLOCK_compare_active=0;
}
void CLOCK_second_count_stop(void)
{
	CLOCK_compare_active=0X0F;
}
char* CLOCK_show(void)
{
	uint8_t tmp;
	CLOCK_timp[8]='\0';
	CLOCK_timp[7]=time.second % 10 + '0';
	tmp = time.second / 10;
	CLOCK_timp[6]=tmp % 10 + '0';
	CLOCK_timp[5]=':';
	CLOCK_timp[4]=time.minute % 10 + '0';
	tmp = time.minute / 10;
	CLOCK_timp[3]=tmp % 10 + '0';
	CLOCK_timp[2]=':';
	CLOCK_timp[1]=time.hour % 10 + '0';
	tmp = time.hour / 10;
	CLOCK_timp[0]=tmp % 10 + '0';
	return CLOCK_timp;
}
/*
** interrupt
*/
/***EOF***/
/***COMMENTS

***/
/*************************************************************************
CLOCK API END
*************************************************************************/
/*************************************************************************
MM74C923 API START
*************************************************************************/
/*
** constant and macro
*/
#define MM74C923_KEY_BUFFER_SIZE 16
/*
** variable
*/
FUNC MM74C923func;
volatile uint8_t *mm74c923_DDR;
volatile uint8_t *mm74c923_PIN;
volatile uint8_t *mm74c923_PORT;
uint8_t mm74c923_tmp;
uint8_t mm74c923_mem;
uint8_t MM74C923_KEY_CODE_INDEX;
char MM74C923_KEY_CODE[]={
	'A','B','C','E','G','H','I','J','M','N','O','P','Q','R','S','T','V','X','Y','Z',
	'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','L','-','+','F','7','8','9','#',
	'4','5','6','U','1','2','3','D','0','/','.','*','\0'
};
uint8_t MM74C923_KEY_BUFFER_INDEX;
char MM74C923_KEY_BUFFER[MM74C923_KEY_BUFFER_SIZE];
char MM74C923_KEY_BUFFER_EMPTY[]="";
char* MM74C923_pointer;
/*
** procedure and function header
*/
void MM74C923_activate(void);
char MM74C923_getch(void);
char* MM74C923_gets(void);
char* MM74C923_data(void);
void MM74C923_data_clear(void);
/*
** procedure and function
*/
MM74C923 MM74C923enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port)
{
	//LOCAL VARIABLES
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	//ALLOCACAO MEMORIA Para Estrutura
	MM74C923func=FUNCenable();
	MM74C923 mm74c923;
	//import parametros
	mm74c923_DDR=ddr;
	mm74c923_PIN=pin;
	mm74c923_PORT=port;
	//inic variables
	*mm74c923_DDR=(1<<MM74C923_OUTPUT_ENABLE);
	*mm74c923_PORT=0xFF;
	mm74c923_tmp&=~(1<<MM74C923_DATA_AVAILABLE);
	mm74c923_mem&=~(1<<MM74C923_DATA_AVAILABLE);
	MM74C923_pointer=MM74C923_KEY_BUFFER_EMPTY;
	//Direccionar apontadores para PROTOTIPOS
	mm74c923.activate=MM74C923_activate;
	mm74c923.getch=MM74C923_getch;
	mm74c923.gets=MM74C923_gets;
	mm74c923.data=MM74C923_data;
	mm74c923.data_clear=MM74C923_data_clear;
	SREG=tSREG;
	//
	return mm74c923;
}
void MM74C923_activate(void){
	mm74c923_mem=mm74c923_tmp;
	mm74c923_tmp=*mm74c923_PIN;
}
char MM74C923_getch(void)
{
	uint8_t c,lh;
	//uint8_t hl;
	lh=MM74C923func.lh(mm74c923_mem,mm74c923_tmp); // low to high bit mask
	//hl=func.hl(mm74c923_mem,mm74c923_tmp); // high to low bit mask
	if(lh&(1<<MM74C923_DATA_AVAILABLE)){
		*mm74c923_PORT&=~(1<<MM74C923_OUTPUT_ENABLE);
		c=*mm74c923_PIN;
		if(c&1<<MM74C923_DATA_OUT_A) MM74C923_KEY_CODE_INDEX|=1; else MM74C923_KEY_CODE_INDEX&=~1;
		if(c&1<<MM74C923_DATA_OUT_B) MM74C923_KEY_CODE_INDEX|=2; else MM74C923_KEY_CODE_INDEX&=~2;
		if(c&1<<MM74C923_DATA_OUT_C) MM74C923_KEY_CODE_INDEX|=4; else MM74C923_KEY_CODE_INDEX&=~4;
		if(c&1<<MM74C923_DATA_OUT_D) MM74C923_KEY_CODE_INDEX|=8; else MM74C923_KEY_CODE_INDEX&=~8;
		if(c&1<<MM74C923_DATA_OUT_E) MM74C923_KEY_CODE_INDEX|=16; else MM74C923_KEY_CODE_INDEX&=~16;
		if(c&1<<MM74C923_EXTRA_DATA_OUT_PIN) MM74C923_KEY_CODE_INDEX|=32; else MM74C923_KEY_CODE_INDEX&=~32;
	//}else if(hl&(1<<MM74C923_DATA_AVAILABLE)){
		*mm74c923_PORT|=(1<<MM74C923_OUTPUT_ENABLE);
		//MM74C923_KEY_CODE_INDEX=52;
	}else
		MM74C923_KEY_CODE_INDEX=52;
	return MM74C923_KEY_CODE[MM74C923_KEY_CODE_INDEX];
}
char* MM74C923_gets(void)
{
	char c;
	c=MM74C923_getch();
	if(c=='*'){ // used has enter key
		MM74C923_KEY_BUFFER_INDEX=0;
	}else if(c!='\0'){
		if(MM74C923_KEY_BUFFER_INDEX<MM74C923_KEY_BUFFER_SIZE-1){ // BUFFER FULL IGNORE DATA
			MM74C923_KEY_BUFFER[MM74C923_KEY_BUFFER_INDEX]=c;
			MM74C923_KEY_BUFFER_INDEX++;
			MM74C923_KEY_BUFFER[MM74C923_KEY_BUFFER_INDEX]='\0';
		}
	}
	return MM74C923_KEY_BUFFER;
}
char* MM74C923_data(void){
	if(MM74C923_KEY_BUFFER_INDEX)
		MM74C923_pointer=MM74C923_KEY_BUFFER_EMPTY;
	else
		MM74C923_pointer=MM74C923_KEY_BUFFER;
	return MM74C923_pointer;
}
void MM74C923_data_clear(void){
	MM74C923_KEY_BUFFER_INDEX=0;
	MM74C923_KEY_BUFFER[MM74C923_KEY_BUFFER_INDEX]='\0';
}

/*
** interrupt
*/
/***EOF***/
/***COMMENTS

***/
/*************************************************************************
MM74C923 API END
*************************************************************************/
