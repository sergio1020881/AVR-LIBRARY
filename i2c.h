#ifndef _I2C_H
	#define _I2C_H   1
/************************************************************************* 
* Title:    C include file for the I2C
*           (i2cmaster.S or twimaster.c)
* Author:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
* File:     $Id: i2c.h,v 1.10 2013/12/30 15:00:00 Peter Exp $
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   any AVR device
* Usage:    see Doxygen manual
**************************************************************************/
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constants and macros
*/
/** Size of the circular receive buffer, must be power of 2 */
#ifndef I2C_RX_BUFFER_SIZE
	#define I2C_RX_BUFFER_SIZE 16
#endif
/** Size of the circular transmit buffer, must be power of 2 */
#ifndef I2C_TX_BUFFER_SIZE
	#define I2C_TX_BUFFER_SIZE 16
#endif
/* test if the size of the circular buffers fits into SRAM */
#if ( (I2C_RX_BUFFER_SIZE+I2C_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
	#error "size of I2C_RX_BUFFER_SIZE + I2C_TX_BUFFER_SIZE larger than size of SRAM"
#endif
/*
** Status Codes for MASTER Transmitter Mode
*/
#define I2C_SENT_START 0X08 //A START condition has been transmitted
// SLA+W will be transmitted; ACK or NOT ACK will be received
// Status Codes for Master Transmitter Mode and Status Codes for Master Receiver Mode
#define I2C_SENT_REPEAT_START 0X10 //A repeated START condition has been transmitted
// SLA+W will be transmitted; ACK or NOT ACK will be received SLA+R will be transmitted;
// Logic will switch to master receiver mode
// Status Codes for Master Transmitter Mode and Status Codes for Master Receiver Mode
#define MI2C_SENT_SLA_W_RECEIVED_ACK 0X18 // SLA+W has been transmitted; ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received
// Repeated START will be transmitted STOP condition will be transmitted and
// TWSTO flag will be reset STOP condition followed by a START condition will be
// transmitted and TWSTO flag will be reset
#define MI2C_SENT_SLA_W_RECEIVED_NACK 0X20 // SLA+W has been transmitted; NOT ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define MI2C_SENT_DATABYTE_RECEIVED_ACK 0X28 // Data byte has been transmitted; ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received
// Repeated START will be transmitted STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define MI2C_SENT_DATABYTE_RECEIVED_NACK 0X30 // Data byte has been transmitted; NOT ACK has been received
// Data byte will be transmitted and ACK or NOT ACK will be received Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define I2C_ARBL_NACK 0X38 // Arbitration lost in SLA+W or data bytes. Arbitration lost in SLA+R or NOT ACK bit
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
#define MI2C_SENT_SLA_R_RECEIVED_ACK 0X40 // SLA+R has been transmitted; ACK has been received
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define MI2C_SENT_SLA_R_RECEIVED_NACK 0X48 // SLA+R has been transmitted; NOT ACK has been received
// Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will be reset
// STOP condition followed by a START condition will be transmitted and TWSTO flag will be reset
#define MI2C_RECEIVED_DATABYTE_SENT_ACK 0X50 // Data byte has been received; ACK has been returned
// Data byte will be received and NOT ACK will be returned
// Data byte will be received and ACK will be returned
#define MI2C_RECEIVED_DATABYTE_SENT_NACK 0X58 // Data byte has been received; NOT ACK has been returned
// Repeated START will be transmitted
// STOP condition will be transmitted and TWSTO flag will
// be reset
// STOP condition followed by a START condition will be
//transmitted and TWSTO flag will be reset
/*
** Status Codes for SLAVE Receiver Mode
*/
#define SI2C_RECEIVED_OSLA_W_SENT_ACK 0X60 // Own SLA+W has been received; ACK has been returned
// Data byte will be received and NOT ACK will be
// returned
// Data byte will be received and ACK will be returned
#define SI2C_MARBL_RECEIVED_OSLA_W_SENT_ACK 0X68 // Arbitration lost in SLA+R/W as master; own SLA+W has been received; ACK has been returned
// Data byte will be received and NOT ACK will be
// returned
// Data byte will be received and ACK will be returned
#define SI2C_RECEIVED_GCALL_SENT_ACK 0X70 // General call address has been received; ACK has been returned
// Data byte will be received and NOT ACK will be
// returned
// Data byte will be received and ACK will be returned
#define SI2C_MARBL_RECEIVED_GCALL_SENT_ACK 0X78 // Arbitration lost in SLA+R/W as master; General call address has been received; ACK has been returned
// Data byte will be received and NOT ACK will be
// returned
// Data byte will be received and ACK will be returned
#define SI2C_PAOSLA_W_RECEIVED_DATABYTE_SENT_ACK 0X80 // Previously addressed with own SLA+W; data has been received; ACK has been returned
// Data byte will be received and NOT ACK will be
// returned
// Data byte will be received and ACK will be returned
#define SI2C_PAOSLA_W_RECEIVED_DATABYTE_SENT_NACK 0X88 // Previously addressed with own SLA+W; data has been received; NOT ACK has been returned
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
#define SI2C_PGCALL_RECEIVE_DATABYTE_SENT_ACK 0X90 // Previously addressed with general call; data has been received; ACK has been returned
// Data byte will be received and NOT ACK will be
// returned
// Data byte will be received and ACK will be returned
#define SI2C_PGCALL_RECEIVE_DATABYTE_SENT_NACK 0X98 // Previously addressed with general call; data has been received; NOT ACK has been returned
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
#define SI2C_RECEIVED_INTERRUPTION 0XA0 // A STOP condition or repeated START condition has been received while still addressed as slave
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
#define SI2C_RECEIVED_OSLA_R_SENT_ACK 0XA8 // Own SLA+R has been received; ACK has been returned
// Last data byte will be transmitted and NOT ACK should
// be received
// Data byte will be transmitted and ACK should be received
#define SI2C_MARBL_RECEIVED_OSLA_R_SENT_ACK 0XB0 // Arbitration lost in SLA+R/W as master; own SLA+R has been received; ACK has been returned
// Last data byte will be transmitted and NOT ACK should
// be received
// Data byte will be transmitted and ACK should be received
#define SI2C_SENT_DATABYTE_RECEIVED_ACK 0XB8 // Data byte in TWDR has been transmitted; ACK has been received
// Last data byte will be transmitted and NOT ACK should
// be received
// Data byte will be transmitted and ACK should be received
#define SI2C_SENT_DATABYTE_RECEIVED_NACK 0XC0 // Data byte in TWDR has been transmitted; NOT ACK has been received
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
#define SI2C_SENT_LAST_DATABYTE_RECEIVED_ACK 0XC8 // Last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
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
#define I2C_TWINT_AT_ZERO 0XF8 // No relevant state information available; TWINT = “0”
// Wait or proceed current transfer
#define I2C_BUS_ERROR 0X00 // Bus error due to an illegal START or STOP condition
// Only the internal hardware is affected, no STOP condi-
// tion is sent on the bus. In all cases, the bus is released
// and TWSTO is cleared.
//
// device
#define DS1307_ID 0xD0		// I2C DS1307 Device Identifier
#define DS1307_ADDR 0X00		// I2C DS1307 Device Address offset
#define TC74_ID 0x9A			// device address of TC74
#define TC74_ADDR 0X00
#define Dev24C02_ID 0xA2		//device address 24C02
#define Dev24C02_ADDR 0x00
#define LM73_ID 0x90			//LM73 address temperature sensor
//
#define I2C_WRITE 0
#define I2C_READ 1
#define ACK 1
#define NACK 0
#define HOUR_24 0
#define HOUR_12 1
/*
** global variables
*/
/*
** global prototypes
*/
struct Calendario{
uint8_t second;
uint8_t minute;
uint8_t hour;
uint8_t dayofweek;
uint8_t dayofmonth;
uint8_t month;
uint8_t millenium;
uint8_t year;
}calendar;
struct I2C_FEEDBACK{
	unsigned char command;
	unsigned char feedback;
	unsigned char instruction;
};
struct I2C{
	/***local variables***/
	unsigned char id;
	unsigned char prescaler;
	unsigned char bitrate;
	struct I2C_FEEDBACK *brain;
	int index;
	unsigned char msg[I2C_RX_BUFFER_SIZE];
	/***PROTOTYPES VTABLE***/
	unsigned char (*routine)(unsigned char cmd);
	unsigned char (*transmit)(struct I2C *i2c, unsigned char type);
	unsigned char (*start)(struct I2C *i2c, unsigned char dev_id, unsigned char dev_addr, unsigned char rw_type);
	unsigned char (*stop)(struct I2C *i2c);
	unsigned char (*write)(struct I2C *i2c, unsigned char data);
	unsigned char (*read)(struct I2C *i2c, unsigned char *data, unsigned char ack_type);
	char (*dec2bcd)(char num);
	char (*bcd2dec)(char num);
	uint8_t (*diff)(uint8_t xi, uint8_t xf);
	void (*Read_DS1307)(struct I2C *i2c);
	//void (*Write_DS1307)(void);
	//unsigned char (*start)(unsigned char address);
	//void (*start_wait)(unsigned char address);
	//unsigned char (*rep_start)(struct I2C *i2c, unsigned char address);
	//void (*stop)(void);
	//unsigned char (*write)(unsigned char data);
	unsigned char (*readAck)(struct I2C *i2c);
	unsigned char (*readNak)(struct I2C *i2c);
};
typedef struct I2C I2C;
/*
** global object function header
*/
I2C I2Cenable(uint8_t id, uint8_t prescaler);
/*
** global function header
*/
#endif
/***EOF***/
