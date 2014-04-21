/*************************************************************************
Title:    FUNCTION
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: function.c,v 0.1 2013/12/30 15:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz, 
License:  GNU General Public License        
DESCRIPTION:
	Atmega 128 at 16MHZ
USAGE:
    Refere to the header file function.h for a description of the routines. 
NOTES:
    Based on Atmel Application Note AVR306
LICENSE:
    Copyright (C) 2013
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
COMMENT:
	Very Stable
*************************************************************************/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
/***/
#include"function.h"
/*
** constant and macro
*/
#ifndef GLOBAL_INTERRUPT_ENABLE
 #define GLOBAL_INTERRUPT_ENABLE 7
#endif
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
uint8_t FUNChmerge(uint8_t X, uint8_t Y);
uint8_t FUNClmerge(uint8_t X, uint8_t Y);
uint8_t FUNClh(uint8_t xi, uint8_t xf);
uint8_t FUNChl(uint8_t xi, uint8_t xf);
uint8_t FUNCdiff(uint8_t xi, uint8_t xf);
void FUNCswap(int *px, int *py);
void FUNCcopy(char to[], char from[]);
void FUNCsqueeze(char s[], int c);
void FUNCshellsort(int v[], int n);
void FUNCitoa(int n, char s[]);
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
/*
** procedure and function
*/
struct FUNC FUNCenable( void )
{
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// struct object
	struct FUNC func;
	// function pointers
	func.power=Power;
	func.stringlength=StringLength;
	func.reverse=Reverse;
	func.mayia=FUNCmayia;
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
void FUNCswap(int *px, int *py)
{
	int temp;
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
void FUNCitoa(int n, char s[])
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
  mask = Power(2,(nbits-1))-1;
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
/*
** interrupt
*/
/***EOF***/
