/*************************************************************************
Title:    AVROUTINE
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: AVRoutine.c, v 0.1 2017/01/22 15:00:00 sergio Exp $
Software: GCC
Hardware:  
License:  GNU General Public License        
DESCRIPTION:
	PC emulation
USAGE: 
NOTES:
LICENSE:
    Copyright (C) 2017
    This program is free software; you can redistribute it and/or modify
    it under the consent of the code developer, in case of commercial use
    need licence.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
COMMENT:
	improve
*************************************************************************/

/*
** library
*/

// fopen perror fread fwrite feof fseek ferror fclose rewind scanf sscanf getchar scanf fscanf
#include <stdio.h>
// calloc free realloc malloc
//#include <stdlib.h>
// strcpy strcmp strcat memcmp
//#include <string.h>
// termios tcflush
//#include <termios.h>
// nanosleep sleep
//#include <time.h>
// tcflsuh read write close
//#include <unistd.h>
// perror
//#include <errno.h>
// open
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <stdio_ext.h>
//#include <math.h>
/***/
#include"AVRoutine.h"

/*
** constant and macro
*/

#define EMPTY 0

/*
** variable
*/

/*
** procedure and function header
*/

unsigned int ROUTINEroutine(struct routine* this, unsigned int entry);
unsigned int ROUTINEsearch(struct routine* this, unsigned int entry, unsigned int data[], unsigned int size);
unsigned int ROUTINEnumber(struct routine* this);
unsigned int ROUTINEpos(struct routine* this);

/*
** Object Inicialize
*/

struct routine ROUTINEenable(unsigned int N)
{
	/***Local Variable***/
	/***Local Function Header***/
	/***Create Object***/
	struct routine r;
	//Inicialize variables
	r.N=N;
	r.index=0;
	r.Y=0;
	//Function Vtable
	r.routine=ROUTINEroutine;
	r.search=ROUTINEsearch;
	r.number=ROUTINEnumber;
	r.pos=ROUTINEpos;
	/******/
	return r;
}

/*
** procedure and function
*/

//routine
unsigned int ROUTINEroutine(struct routine* this, unsigned int entry)
{
	unsigned int i;
	unsigned int index;
	unsigned int n;
	unsigned int y;
	n=this->N+1;
	y=this->Y;
	for(i=0;i<n;i++){
		index=y*n+(i+1);
		if(entry==index){
			y=i;
			this->Y=y;
			break;
		}
	}
	return y;
}
//search
unsigned int ROUTINEsearch(struct routine* this, unsigned int entry, unsigned int data[], unsigned int size)
{
	unsigned int i;
	unsigned int j;
	unsigned int n;
	unsigned int y;
	unsigned int position;
	unsigned int index;
	n=this->N+1;
	position=0;
	y=this->Y;
	for(i=0;i<n;i++){
		index=y*n+(i+1);
		for(j=0;j<size;j+=2){
			if(index==data[j] && entry==data[j+1]){
				position=index;
				this->index=position;
				break;
			}
		}
	}
	return position;
}
//number
unsigned int ROUTINEnumber(struct routine* this)
{
	return this->Y;
}
//pos
unsigned int ROUTINEpos(struct routine* this)
{
	return this->index;
}


/*
** interrupt
*/

/***EOF***/

/***Comments***/
// Y*(N+1)+(X+1)=Z
//n=N+1
//Y*n+(X+1)=Z
