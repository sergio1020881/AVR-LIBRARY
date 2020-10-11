/*************************************************************************
Title: eeprom wrapper
Author: Sergio Santos
 * <sergio.salazar.santos@gmail.com>
File: $Id: eeprom.c,v 0.2 01/10/2020 Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: ATmega
License: GNU General Public License
Comment:
   Very Stable
*************************************************************************/
/*
** library
*/
#include "eeprom.h"
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
	EEPROM eprom;
	eprom.read_byte=eeprom_read_byte;
	eprom.write_byte=eeprom_write_byte;
	eprom.update_byte=eeprom_update_byte;
	eprom.read_word=eeprom_read_word;
	eprom.write_word=eeprom_write_word;
	eprom.update_word=eeprom_update_word;
	//eeprom.read_dword=eeprom_read_dword;
	eprom.write_dword=eeprom_write_dword;
	eprom.write_dword=eeprom_write_dword;
	//eeprom.read_float=eeprom_read_float;
	eprom.write_float=eeprom_write_float;
	eprom.update_float=eeprom_update_float;
	eprom.read_block=eeprom_read_block;
	eprom.write_block=eeprom_write_block;
	eprom.update_block=eeprom_update_block;
	return eprom;
}
/*
** interrupt
*/
/***EOF***/
