/************************************************************************
Title:    I2C library
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: i2c.c,v 0.2 2014/04/12 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR with built-in ADC, tested on ATmega128 at 16 Mhz
License:  GNU General Public License
DESCRIPTION:
	Atmega 128 at 16MHZ
USAGE:    
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
	testing phase
************************************************************************/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/* I2C clock in Hz */ 
#define I2C_SCL_CLOCK 100000UL
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
//#include <compat/twi.h>
/***/
#include "i2c.h"
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
/***TYPE 1***/
#if  defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega323__)
	#define MEGA_I2C
	#define I2C_PORT PORTC
	#define I2C_DDR DDRC
	#define I2C_PIN PINC
	#define I2C_PIN_MASK 3
	#define TWI_PRESCALER_MASK 0B00000001
/***TYPE 2***/
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
	#define ATMEGA_I2C
	#define I2C_PORT PORTD
	#define I2C_DDR DDRD
	#define I2C_PIN PIND
	#define I2C_PIN_MASK 3
	#define TWI_PRESCALER_MASK 0B00000011
/***TYPE 3***/
#elif defined(__AVR_ATmega161__)
	/* ATmega with UART */
 	#error "AVR ATmega161 currently not supported by this libaray !"
#else
	/***TYPE 4***/
	#error "no TWI definition for MCU available"
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
struct I2C I2Cenable(unsigned char device_id, unsigned char prescaler)
{
	// local variables
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// object
	struct I2C i2c;
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
	/***Pre-Processor Case 1***/
	#if defined( MEGA_I2C )
		I2C_DDR |= 0X03;
		I2C_PORT |= 0X03;
	switch(prescaler){
		case 1:
			TWI_STATUS_REGISTER &= ~TWI_PRESCALER_MASK;
			break;
		case 4:
			TWI_STATUS_REGISTER |= (1<<TWPS);
			break;
		default:
			prescaler=1;
			TWI_STATUS_REGISTER &= ~TWI_PRESCALER_MASK;
			break;
	}
	I2C_BIT_RATE = ((F_CPU/I2C_SCL_CLOCK)-16)/(2*prescaler);
	/***Pre-Processor Case 2***/
	#elif defined( ATMEGA_I2C )
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
		case TWI_DATA_ACK:	// Data with Acknowledge 2
			TWI_CONTROL_REGISTER |= (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
			break;
		case TWI_STOP_CONDITION:			// Stop Condition 3
			TWI_CONTROL_REGISTER |= (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
			break;
		default:				// Stop Condition
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
SIGNAL(SIG_2WIRE_SERIAL)
{
	/***SLAVE CODE***/
	switch(twi_status()){
		case TWI_SLAVE_RECEIVED_OSLA_W_SENT_ACK:
			break;
		default:
			break;
	}
}
/***EOF***/
