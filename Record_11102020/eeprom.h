/*************************************************************************
Title: eeprom wrapper
Author: Sergio Manuel Santos
 * <sergio.salazar.santos@gmail.com>
File: $Id: eeprom.h,v 0.2 01/10/2020 Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: ATmega 
License: GNU General Public License
Comment:
   Very Stable
*************************************************************************/
#ifndef EEPROM_H
	#define EEPROM_H
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** library
*/
#include <avr/eeprom.h>
/*
** constant and macro
*/
/*
** variable
*/
struct prm{
	/***vtable***/
	uint8_t (*read_byte) ( const uint8_t * addr );
	void (*write_byte) ( uint8_t *addr , uint8_t value );
	void (*update_byte) ( uint8_t *addr , uint8_t value );
	uint16_t (*read_word) ( const uint16_t * addr );
	void (*write_word) ( uint16_t *addr , uint16_t value );
	void (*update_word) ( uint16_t *addr , uint16_t value );
	uint16_t (*read_dword) ( const uint32_t * addr );
	void (*write_dword) ( uint32_t *addr , uint32_t value );
	void (*update_dword) ( uint32_t *addr , uint32_t value );
	uint16_t (*read_float) ( const float * addr );
	void (*write_float) ( float *addr , float value );
	void (*update_float) ( float *addr , float value );
	void (*read_block) ( void * pointer_ram , const void * pointer_eeprom , size_t n);
	void (*write_block) ( const void * pointer_ram , void * pointer_eeprom , size_t n);
	void (*update_block) ( const void * pointer_ram , void * pointer_eeprom , size_t n);
};
typedef struct prm EEPROM;
/*
** procedure and function header
*/
EEPROM EEPROMenable();
#endif
/***EOF***/
