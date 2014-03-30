/*****************************************************************************
//  File Name    : i2c.c
//  Version      : 1.0
//  Description  : DS1307 RTC and TC74 AVR Microcontroller
//  Author       : sergio salazar santos
//  Target       : Atmega128 Board
//  Compiler     : AVR-GCC 4.3.0; avr-libc 1.6.2 (WinAVR 20080610)
//  IDE          : Atmel AVR Studio 4.14
//  Programmer   : Ponyprog
//               : AVR Visual Studio 6.1
//  Last Updated : 30 December 2013
*****************************************************************************/
/*
** atmega 128 at 16MHZ
*/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/* I2C clock in Hz */ 
#define I2C_SCL_CLOCK 100000UL
/*
** Library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <compat/twi.h>
/*
** Private Library
*/
#include "i2c.h"
/*
** module constants and macros
*/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
/* size of RX/TX buffers */
#define I2C_RX_BUFFER_MASK ( I2C_RX_BUFFER_SIZE - 1)
#define I2C_TX_BUFFER_MASK ( I2C_TX_BUFFER_SIZE - 1)
#if ( I2C_RX_BUFFER_SIZE & I2C_RX_BUFFER_MASK )
	#error I2C RX buffer size is not a power of 2
#endif
#if ( I2C_TX_BUFFER_SIZE & I2C_TX_BUFFER_MASK )
	#error I2C TX buffer size is not a power of 2
#endif
// I2C register renaming
#define I2C_SLAVE_ADDRESS TWAR
#define I2C_SLAVE_ADDRESS_MASK 0B11111110
#define I2C_VALUE TWDR
#define I2C_STATUS TWSR
#define I2C_CONTROL TWCR
#define I2C_BIT_RATE TWBR
#define I2C_STATUS_MASK 0B11111000
#define i2c_read(ack)  (ack) ? i2c_readAck() : i2c_readNak();
/***TYPE 1***/
#if  defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega323__)
	#define MEGA_I2C
	#define I2C_PORT PORTC
	#define I2C_DDR DDRC
	#define I2C_PIN PINC
	#define I2C_PIN_MASK 3
	#define I2C_PRESCALER_MASK 0B00000001
/***TYPE 2***/
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
	#define ATMEGA_I2C
	#define I2C_PORT PORTD
	#define I2C_DDR DDRD
	#define I2C_PIN PIND
	#define I2C_PIN_MASK 3
	#define I2C_PRESCALER_MASK 0B00000011
/***TYPE 3***/
#elif defined(__AVR_ATmega161__)
	/* ATmega with UART */
 	#error "AVR ATmega161 currently not supported by this libaray !"
#else
	/***TYPE 4***/
	#error "no UART definition for MCU available"
#endif
#define I2C_NUMERO 3
// INPUT (note that I2C_STATUS is also an input therefore do not repeat HEX)
#define I2C_SEND_START 0X01
#define I2C_SEND_SLA_RW 0X02
#define I2C_SEND_DATABYTE 0X03
#define I2C_SEND_DATABYTE_ACK 0X04
#define I2C_SEND_WAIT_START 0X05
#define I2C_SEND_WAIT 0X06
#define I2C_SEND_WORK 0X07
#define I2C_SEND_EXIT 0X09
#define I2C_SEND_STOP 0X0A
#define I2C_SENDING_START 0X0B
#define I2C_SENDING_SLA_RW 0X0C
#define I2C_SENDING_DATABYTE 0X0D
#define I2C_SENDING_DATABYTE_ACK 0X0E
#define I2C_RECEIVE_DATABYTE 0X0F
#define I2C_RECEIVE_DATABYTE_ACK 0X11
#define I2C_RECEIVING_DATABYTE 0X12
#define I2C_RECEIVING_DATABYTE_ACK 0X13
#define I2C_SEND_TIME_OUT 0X14
#define I2C_RECEIVED_DATABYTE_INTERRUPT 0X15
#define I2C_RECEIVED_DATABYTE 0X16
// OUTPUT
#define I2C_START 0X00
#define I2C_SLA_RW 0X01
#define I2C_DATABYTE 0X02
#define I2C_DATABYTE_ACK 0X03
#define I2C_WAIT_START 0X04
#define I2C_WAIT 0X05
#define I2C_WORK 0X06
#define I2C_EXIT 0X07
#define I2C_STOP 0X08
#define I2C_READ_DATABYTE 0X09
/*
** module variables
*/
static volatile unsigned char I2C_TxBuf[I2C_TX_BUFFER_SIZE];
static volatile unsigned char I2C_RxBuf[I2C_RX_BUFFER_SIZE];
static volatile unsigned char I2C_TxHead;
static volatile unsigned char I2C_TxTail;
static volatile unsigned char I2C_RxHead;
static volatile unsigned char I2C_RxTail;
static volatile unsigned char I2C_LastRxError;
static volatile unsigned char I2C_index;
// the brains of the machine
struct I2C_FEEDBACK i2c_packet;
const uint8_t i2c_start_cycles=40;
const unsigned int i2c_sizeeeprom=87;
const unsigned char i2c_eeprom[90]={
// INPUT, OUTPUT_feedback, OUTPUT,
// first set for start and repeated start master transmiter
I2C_SEND_START						,I2C_STOP			,I2C_START, // 1
I2C_SEND_START						,I2C_WORK			,I2C_START, // 2
I2C_SENDING_START					,I2C_START			,I2C_WAIT_START, // 3
I2C_SENT_START						,I2C_WAIT_START		,I2C_SLA_RW, // 4
I2C_SENT_REPEAT_START				,I2C_WAIT_START		,I2C_SLA_RW, // 5
I2C_ARBL_NACK						,I2C_WAIT_START		,I2C_START, // 6
I2C_SENDING_SLA_RW					,I2C_SLA_RW			,I2C_WAIT, // 7
MI2C_SENT_SLA_W_RECEIVED_ACK		,I2C_WAIT			,I2C_WORK, // 8
MI2C_SENT_SLA_W_RECEIVED_NACK		,I2C_WAIT			,I2C_WORK, // I2C_EXIT with device 9
I2C_ARBL_NACK						,I2C_WAIT			,I2C_EXIT, // 10
MI2C_SENT_SLA_R_RECEIVED_ACK		,I2C_WAIT			,I2C_WORK, // 11
MI2C_SENT_SLA_R_RECEIVED_NACK		,I2C_WAIT			,I2C_WORK, // I2C_EXIT with device 12
// transmit DATABYTE
I2C_SEND_DATABYTE					,I2C_WORK			,I2C_DATABYTE, // 13
I2C_SENDING_DATABYTE				,I2C_DATABYTE		,I2C_WAIT, // 14
MI2C_SENT_DATABYTE_RECEIVED_ACK		,I2C_WAIT			,I2C_WORK, // 15
MI2C_SENT_DATABYTE_RECEIVED_NACK	,I2C_WAIT			,I2C_WORK, // I2C_EXIT with device 16
// stop conditions
I2C_SEND_STOP						,I2C_STOP			,I2C_STOP, // 17
I2C_SEND_STOP						,I2C_WORK			,I2C_STOP, // 18
I2C_SEND_STOP						,I2C_EXIT			,I2C_STOP, // 19
// receive DATABYTE
I2C_RECEIVE_DATABYTE				,I2C_WORK			,I2C_DATABYTE, // 20
I2C_RECEIVE_DATABYTE_ACK			,I2C_WORK			,I2C_DATABYTE_ACK, // 21
I2C_RECEIVING_DATABYTE				,I2C_DATABYTE		,I2C_WAIT, // 22
I2C_RECEIVING_DATABYTE_ACK			,I2C_DATABYTE_ACK	,I2C_WAIT, // 23
I2C_RECEIVED_DATABYTE_INTERRUPT		,I2C_WAIT			,I2C_READ_DATABYTE, // 24
I2C_RECEIVED_DATABYTE				,I2C_READ_DATABYTE	,I2C_WAIT, // 25
MI2C_RECEIVED_DATABYTE_SENT_ACK		,I2C_WAIT			,I2C_WORK, // 26
MI2C_RECEIVED_DATABYTE_SENT_NACK	,I2C_WAIT			,I2C_WORK, // I2C_EXIT with device 27
I2C_SEND_TIME_OUT					,I2C_WAIT			,I2C_WORK, // I2C_STOP with device 28
I2C_BUS_ERROR						,I2C_WAIT			,I2C_WORK, // I2C_STOP with device 29
};
const unsigned int i2c_sizeblock=3;
// DS1307 Register Address
// Second: ds1307_addr[0]
// Minute: ds1307_addr[1]
// Hour  : ds1307_addr[2]
// Day   : ds1307_addr[3]
// Date  : ds1307_addr[4]
// Month : ds1307_addr[5]
// Year  : ds1307_addr[6]
/*
** module function header
*/
uint8_t i2c_diff(uint8_t xi, uint8_t xf);
unsigned char i2c_routine(unsigned char cmd);
unsigned char i2c_txpush(unsigned char data);
unsigned char i2c_txpop(void);
unsigned char i2c_rxpush(unsigned char data);
unsigned char i2c_rxpop(void);
/*
** module interrupt header
*/
SIGNAL(SIG_2WIRE_SERIAL);
/*
** module object 1 constructor
*/
struct I2C I2Cenable(uint8_t id, uint8_t prescaler)
{
	// local variables
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	/***Global variable***/
	/***MODULE VARIABLE***/
    I2C_TxHead = 0;
    I2C_TxTail = 0;
    I2C_RxHead = 0;
    I2C_RxTail = 0;
	// local function headers
	unsigned char i2c_transmit(struct I2C *i2c, unsigned char type);
	unsigned char i2c_start(struct I2C *i2c, unsigned char dev_id, unsigned char dev_addr, unsigned char rw_type);
	unsigned char i2c_stop(struct I2C *i2c);
	unsigned char i2c_write(struct I2C *i2c, unsigned char data);
	unsigned char i2c_read2(struct I2C *i2c, unsigned char *data, unsigned char ack_type);
	char I2Cdec2bcd(char num);
	char I2Cbcd2dec(char num);
	void I2CRead_DS1307(struct I2C *i2c);
	//void I2CWrite_DS1307(void);
	//unsigned char I2Ci2c_start(unsigned char address);
	//void I2Ci2c_start_wait(unsigned char address);
	//unsigned char I2Ci2c_rep_start(struct I2C *i2c, unsigned char address);
	//void I2Ci2c_stop(void);
	//unsigned char I2Ci2c_write(unsigned char data);
	unsigned char i2c_readAck(struct I2C *i2c);
	unsigned char i2c_readNak(struct I2C *i2c);
	// object
	struct I2C i2c;
	// function pointers
	i2c.routine=i2c_routine;
	i2c.transmit=i2c_transmit;
	i2c.start=i2c_start;
	i2c.stop=i2c_stop;
	i2c.write=i2c_write;
	i2c.read=i2c_read2;
	i2c.dec2bcd=I2Cdec2bcd;
	i2c.bcd2dec=I2Cbcd2dec;
	i2c.diff=i2c_diff;
	i2c.Read_DS1307=I2CRead_DS1307;
	//i2c.Write_DS1307=I2CWrite_DS1307;
	//i2c.start=i2c_start;
	//i2c.start_wait=i2c_start_wait;
	//i2c.rep_start=i2c_rep_start;
	//i2c.stop=i2c_stop;
	//i2c.write=i2c_write;
	i2c.readAck=i2c_readAck;
	i2c.readNak=i2c_readNak;
	// object variables inicialize;
	i2c.brain=(struct I2C_FEEDBACK*)&i2c_packet;
	i2c.id=0;
	i2c.index=0;
	i2c.msg[0]='\0';
	if(id>0 && id<128){
		i2c.id=(unsigned char)(id<<TWA0);
		I2C_SLAVE_ADDRESS = i2c.id;
	}else{
		id=(1<<TWA0);
		i2c.id=id;
		I2C_SLAVE_ADDRESS = i2c.id;
	}	
	/***Pre-Processor Case 1***/
	#if defined( MEGA_I2C )
		I2C_DDR |= 0X03;
		I2C_PORT |= 0X03;
	switch(prescaler){
		case 1:
			I2C_STATUS &= ~I2C_PRESCALER_MASK;
			break;
		case 4:
			I2C_STATUS |= (1<<TWPS);
			break;
		default:
			prescaler=1;
			I2C_STATUS &= ~I2C_PRESCALER_MASK;
			break;
	}
	i2c.prescaler=(unsigned char)(I2C_STATUS & I2C_PRESCALER_MASK);
	I2C_BIT_RATE = ((F_CPU/I2C_SCL_CLOCK)-16)/(2*prescaler);
	i2c.bitrate=I2C_BIT_RATE;
	// enable TWI (two-wire interface)
	I2C_CONTROL|=( 1<<TWEN );
	// enable TWI interrupt and slave address ACK
	I2C_CONTROL |= ( 1<<TWIE );
	I2C_CONTROL |= ( 1<<TWEA );
	/***Pre-Processor Case 2***/
	#elif defined( ATMEGA_I2C )
		I2C_DDR |= 0X03;
		I2C_PORT |= 0X03;
		switch(prescaler){
		case 1:
			I2C_STATUS &= ~I2C_PRESCALER_MASK;
			break;
		case 4:
			I2C_STATUS |= (1<<TWPS0);
			break;
		case 16:
			I2C_STATUS |= (2<<TWPS0);
			break;
		case 64:
			I2C_STATUS |= (3<<TWPS0);
			break;
		default:
			prescaler=1;
			I2C_STATUS &= ~I2C_PRESCALER_MASK;
			break;
	}
	i2c.prescaler=(unsigned char)( I2C_STATUS & I2C_PRESCALER_MASK );
	I2C_BIT_RATE = ((F_CPU/I2C_SCL_CLOCK)-16)/(2*prescaler);
	i2c.bitrate=I2C_BIT_RATE;
	// enable TWI (two-wire interface)
	I2C_CONTROL|=( 1<<TWEN );
	// enable TWI interrupt and slave address ACK
	I2C_CONTROL |= ( 1<<TWIE );
	I2C_CONTROL |= ( 1<<TWEA );
	#endif
	SREG=tSREG;
	SREG|=(1<<GLOBAL_INTERRUPT_ENABLE);
	/******/
	return i2c;
}
/*
** module object 1 procedure and function difinitions
*/
// transmit
unsigned char i2c_transmit(struct I2C *i2c, unsigned char type)
{
	unsigned char ret=0;
	switch(type){
		case I2C_START:    // Send Start Condition 0
			I2C_CONTROL = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
			break;	
		case I2C_DATABYTE:     // Send Data with No-Acknowledge 1
			I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_DATABYTE_ACK: // Send Data with Acknowledge 2
			I2C_CONTROL = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
			break;
		case I2C_STOP:     // Send Stop Condition 3
			I2C_CONTROL = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
			return 0;
		default:     // Send Stop Condition
			I2C_CONTROL = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
			ret=1;
			break;
	}
	
	while (!(I2C_CONTROL & (1 << TWINT))); // POLLING
	
	return ret;
}
// start
unsigned char i2c_start(struct I2C *i2c, unsigned char dev_id, unsigned char dev_addr, unsigned char rw_type)
{
	unsigned char twi_instruction;
	unsigned char command;
	unsigned char data;
	data=(dev_id & 0xF0) | (dev_addr & 0x07) | rw_type;
	uint8_t tries;
	//uint8_t flag;
	i2c->id=dev_id;
	command=I2C_SEND_START;
	for(tries=0;tries<i2c_start_cycles;tries++){
		twi_instruction=i2c->routine(command);
		// Check the TWI Status
		switch(twi_instruction){
			case I2C_START:
				I2C_CONTROL = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
				command=I2C_SENDING_START;
				continue;		
			case I2C_SLA_RW:
				I2C_VALUE = data;
				//I2C_VALUE = 170 | rw_type; // testing
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_SLA_RW;
				continue;
			case I2C_DATABYTE:
				I2C_VALUE = (dev_id & 0xF0) | (dev_addr & 0x07) | rw_type;
				//I2C_VALUE = 170 | rw_type; // testing
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_DATABYTE;
				continue;
			case I2C_DATABYTE_ACK:
				I2C_CONTROL = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_DATABYTE_ACK;
				continue;
			case I2C_WAIT_START:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
					// do nothing
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
				continue;
			case I2C_WAIT:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
					// do nothing
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
				continue;
			case I2C_WORK:
				break;
			case I2C_EXIT:
				break;
			case I2C_STOP:
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
				break;
			default:
				continue;
		} // switch		
	} // for
	return twi_instruction;
}
// stop
unsigned char i2c_stop(struct I2C *i2c)
{
	unsigned char twi_instruction;
	unsigned char command;
	unsigned char data=0;
	uint8_t tries;
	command=I2C_SEND_STOP;
	for(tries=0;tries<i2c_start_cycles;tries++){
		twi_instruction=i2c->routine(command);
		// Check the TWI Status
		switch(twi_instruction){
			case I2C_START:
				I2C_CONTROL = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
				command=I2C_SENDING_START;
				continue;
			case I2C_SLA_RW:
				I2C_VALUE = data;
				//I2C_VALUE = 170 | rw_type; // testing
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_SLA_RW;
				continue;
			case I2C_DATABYTE:
				I2C_VALUE = data;
				//I2C_VALUE = 170 | rw_type; // testing
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_DATABYTE;
				continue;
			case I2C_DATABYTE_ACK:
				I2C_CONTROL = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_DATABYTE_ACK;
				continue;
			case I2C_WAIT_START:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
					// do nothing
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
				continue;
			case I2C_WAIT:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
					// do nothing
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
				continue;
			case I2C_WORK:
				break;
			case I2C_EXIT:
				break;
			case I2C_STOP:
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
				break;
			default:
				continue;
		} // switch		
	} // for
	return twi_instruction;
}
// write
unsigned char i2c_write(struct I2C *i2c, unsigned char data)
{
	unsigned char twi_instruction;
	unsigned char command;
	uint8_t tries;
	command=I2C_SEND_DATABYTE;
	for(tries=0;tries<i2c_start_cycles;tries++){
		twi_instruction=i2c->routine(command);
		// Check the TWI Status
		switch(twi_instruction){
			case I2C_START:
				I2C_CONTROL = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
				command=I2C_SENDING_START;
			continue;
			case I2C_SLA_RW:
				I2C_VALUE = data;
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_SLA_RW;
			continue;
			case I2C_DATABYTE:
				I2C_VALUE = data;
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_DATABYTE;
			continue;
			case I2C_DATABYTE_ACK:
				I2C_CONTROL = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_DATABYTE_ACK;
			continue;
			case I2C_WAIT_START:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
					// do nothing
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
			continue;
			case I2C_WAIT:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
					// do nothing
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
			continue;
			case I2C_WORK:
				break;
			case I2C_EXIT:
				break;
			case I2C_STOP:
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
				break;
			default:
				continue;
		} // switch		
	} // for
	return twi_instruction;
}
// read
unsigned char i2c_read2(struct I2C *i2c, unsigned char *data, unsigned char ack_type)
{
	unsigned char twi_instruction;
	unsigned char command;
	uint8_t tries;
	//uint8_t flag;
	if(ack_type){
		command=I2C_RECEIVE_DATABYTE_ACK;
	}else{
		command=I2C_RECEIVE_DATABYTE;
	}	
	for(tries=0;tries<i2c_start_cycles;tries++){
		twi_instruction=i2c->routine(command);
		// Check the TWI Status
		switch(twi_instruction){
			case I2C_START:
				I2C_CONTROL = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
				command=I2C_SENDING_START;
			continue;
			case I2C_SLA_RW:
				//I2C_VALUE = 170 | rw_type; // testing
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_SENDING_SLA_RW;
			continue;
			case I2C_DATABYTE:
				*data = I2C_VALUE;
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN);
				command=I2C_RECEIVING_DATABYTE;
			continue;
			case I2C_DATABYTE_ACK:
				I2C_CONTROL = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
				command=I2C_RECEIVING_DATABYTE_ACK;
			continue;
			case I2C_READ_DATABYTE:
				*data = i2c_rxpop();
				command=I2C_RECEIVED_DATABYTE;
			continue;
			case I2C_WAIT_START:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
					// do nothing
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
			continue;
			case I2C_WAIT:
				// Wait for TWINT flag set on Register TWCR
				//for (flag=0;!(I2C_CONTROL & (1 << TWINT));flag++); // POLLING	
				if (!(I2C_CONTROL & (1 << TWINT))){
					if(tries>(i2c_start_cycles-I2C_NUMERO))
						command=I2C_SEND_TIME_OUT;
				}else{
					// Return TWI Status Register, mask the prescaller bits (TWPS1,TWPS0)
					command=(I2C_STATUS & I2C_STATUS_MASK);
				}
			continue;
			case I2C_WORK:
				break;
			case I2C_EXIT:
				break;
			case I2C_STOP:
				I2C_CONTROL = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
				break;
			default:
				continue;
		} // switch			
	} // for
	return twi_instruction;
}
// Convert Decimal to Binary Coded Decimal (BCD)
char I2Cdec2bcd(char num)
{
	return ((num/10 * 16) + (num % 10));
}
// Convert Binary Coded Decimal (BCD) to Decimal
char I2Cbcd2dec(char num)
{
	return ((num/16 * 10) + (num % 16));
}
// I2CRead_DS1307
void I2CRead_DS1307(struct I2C *i2c)
{
	unsigned char data;
	// First we initial the pointer register to address 0x00
	// Start the I2C Write Transmission
	i2c_start(i2c, DS1307_ID, DS1307_ADDR, I2C_WRITE);
	if(i2c->brain->instruction==I2C_WORK){
		// Start from Address 0x00
		i2c->write(i2c, DS1307_ADDR);
		// Stop I2C Transmission
		i2c->stop(i2c);
	}	
	// Start the I2C Read Transmission
	i2c->start(i2c, DS1307_ID, DS1307_ADDR, I2C_READ);
	if(i2c->brain->instruction==I2C_WORK){
		// Read the Second Register, Send Master Acknowledge
		i2c->read(i2c, &data, ACK);
		calendar.second=data;
		// Read the Minute Register, Send Master Acknowledge
		i2c->read(i2c, &data, ACK);
		calendar.minute=data;
		// Read the Hour Register, Send Master Acknowledge
		i2c->read(i2c, &data, ACK);
		calendar.hour=data;
		// Read the Day of Week Register, Send Master Acknowledge
		i2c->read(i2c, &data, ACK);
		calendar.dayofweek=data;
		// Read the Day of Month Register, Send Master Acknowledge
		i2c->read(i2c, &data, ACK);
		calendar.dayofmonth=data;
		// Read the Month Register, Send Master Acknowledge
		i2c->read(i2c, &data, ACK);
		calendar.month=data;
		// Read the Year Register, Send Master No Acknowledge
		i2c->read(i2c, &data, NACK);
		calendar.year=data;
		// Stop I2C Transmission
		i2c->stop(i2c);
	}	
}
/***
// Write_DS1307
void I2CWrite_DS1307(struct I2C *i2c)
{
	unsigned char i, hour_format;
	// Make sure we enable the Oscillator control bit CH=0 on Register 0x00
	ds1307_addr[0]=ds1307_addr[0] & 0x7F;
	// Start the I2C Write Transmission
	i2c->start(i2c, DS1307_ID, DS1307_ADDR, TW_WRITE);
	// Start from Address 0x00
	i2c->write(i2c, 0x00);
	// Write the data to the DS1307 address start at 0x00
	// DS1307 automatically will increase the Address.
	for (i=0; i<7; i++){
		if (i == 2){
			hour_format=i2c->dec2bcd(ds1307_addr[i]);
			if (hour_mode){
				hour_format |= (1 << 6);
				if (ampm_mode)
				hour_format |= (1 << 5);
				else
				hour_format &= ~(1 << 5);
			}else{
				hour_format &= ~(1 << 6);
				i2c->write(i2c, hour_format);
			}
		else{
			i2c->write(i2c, i2c->dec2bcd(ds1307_addr[i]));
		}
		// Stop I2C Transmission
		i2c->stop(i2c);
	}	
}
***/
/*************************************************************************    
  Issues a start condition and sends address and transfer direction. 
  return 0 = device accessible, 1= failed to access device 
*************************************************************************
unsigned char i2c_start(unsigned char address)
{
	uint8_t   twst;
	// send START condition
	I2C_CONTROL = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// wait until transmission completed
	while(!(I2C_CONTROL & (1<<TWINT)));
	// check value of TWI Status Register. Mask prescaler bits.
	twst = I2C_STATUS & I2C_STATUS_MASK;
	if ( (twst != TW_START) && (twst != TW_REP_START))
		return 1;
	// send device address
	I2C_VALUE = address;
	I2C_CONTROL = (1<<TWINT) | (1<<TWEN);
	// wail until transmission completed and ACK/NACK has been received
	while(!(I2C_CONTROL & (1<<TWINT)));
	// check value of TWI Status Register. Mask prescaler bits.
	twst = I2C_STATUS & I2C_STATUS_MASK;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) )
		return 1;
	return 0;
}// i2c_start
***/
/************************************************************************* 
 Issues a start condition and sends address and transfer direction. 
 If device is busy, use ack polling to wait until device is ready 
  
 Input:   address and transfer direction of I2C device 
*************************************************************************
void i2c_start_wait(unsigned char address)
{
	uint8_t   twst;
	while ( 1 ){
		// send START condition
		I2C_CONTROL = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
		// wait until transmission completed
		while(!(I2C_CONTROL & (1<<TWINT)));
		// check value of TWI Status Register. Mask prescaler bits.
		twst = I2C_STATUS & I2C_STATUS_MASK;
		if ( (twst != TW_START) && (twst != TW_REP_START))
			continue;
		// send device address
		I2C_VALUE = address;
		I2C_CONTROL = (1<<TWINT) | (1<<TWEN);
		// wail until transmission completed
		while(!(TWCR & (1<<TWINT)));
		// check value of TWI Status Register. Mask prescaler bits.
		twst = I2C_STATUS & I2C_STATUS_MASK;
		if ( (twst == TW_MT_SLA_NACK )||(twst ==TW_MR_DATA_NACK) ){
			//device busy, send stop condition to terminate write operation
			I2C_CONTROL = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
			// wait until stop condition is executed and bus released
			while(I2C_CONTROL & (1<<TWSTO));
			continue;
		}
		if( twst != TW_MT_SLA_ACK)
			return 1;
		break;
	}
}// i2c_start_wait
***/
/************************************************************************* 
 Issues a repeated start condition and sends address and transfer direction 

 Input:   address and transfer direction of I2C device 
  
 Return:  0 device accessible 
          1 failed to access device 
*************************************************************************
unsigned char i2c_rep_start(struct I2C *i2c, unsigned char address)
{
	return i2c_start(i2c, address);
}// i2c_rep_start
***/
/************************************************************************* 
 Terminates the data transfer and releases the I2C bus 
*************************************************************************
void i2c_stop(void)
{
	// send stop condition
	I2C_CONTROL = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	// wait until stop condition is executed and bus released
	while(I2C_CONTROL & (1<<TWSTO));
}// i2c_stop
***/
/************************************************************************* 
  Send one byte to I2C device 
  
  Input:    byte to be transfered 
  Return:   0 write successful 
            1 write failed 
*************************************************************************
unsigned char i2c_write(unsigned char data)
{
	uint8_t   twst;
	// send data to the previously addressed device
	I2C_VALUE = data;
	I2C_CONTROL = (1<<TWINT) | (1<<TWEN);
	// wait until transmission completed
	while(!(I2C_CONTROL & (1<<TWINT)));
	// check value of TWI Status Register. Mask prescaler bits
	twst = I2C_STATUS & I2C_STATUS_MASK;
	if( twst != TW_MT_DATA_ACK)
		return 1;
	return 0; 
}// i2c_write
***/
/************************************************************************* 
 Read one byte from the I2C device, request more data from device 
  
 Return:  byte read from I2C device 
*************************************************************************/
unsigned char i2c_readAck(struct I2C *i2c)
{
	if(i2c->brain->instruction==I2C_WORK){
		I2C_CONTROL = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
		while(!(I2C_CONTROL & (1<<TWINT)));
	}	
		return I2C_VALUE;
}// i2c_readAck
/***/
/************************************************************************* 
 Read one byte from the I2C device, read is followed by a stop condition 
  
 Return:  byte read from I2C device 
*************************************************************************/
unsigned char i2c_readNak(struct I2C *i2c)
{
	if(i2c->brain->instruction==I2C_WORK){
		I2C_CONTROL = (1<<TWINT) | (1<<TWEN);
		while(!(TWCR & (1<<TWINT)));
	}		
	return I2C_VALUE;
}// i2c_readNak
/***/
/*
** module procedure and function difinitions
*/
/***SAMPLE CODE***
int main(void)
{
    unsigned char ret;
    DDRB  = 0xff;               // use all pins on port B for output 
    PORTB = 0xff;               // (active low LED's )
   _delay_ms(1000);
    i2c_init();                 // init I2C interface
    i2c_start(0x9A);             // set device address and write mode
   i2c_write(0x00); 
    
   i2c_rep_start(0x9B);       // set device address and read mode
   PORTB = i2c_readNak();
   i2c_stop();
   for(;;)
   {
   i2c_start(0x9A);       // set device address and write mode
   i2c_write(0x00); 
   i2c_rep_start(0x9B);       // set device address and read mode 
   PORTB = i2c_readNak();      //Temperature can be read on PORTB pins
   i2c_stop();
   _delay_ms(1000); 
   }
}
The only thing that didn't work was: 
Code:
i2c_start(TC74+I2C_WRITE); 
so I had to put TC74's address + "0" on LSB (0x9A). 
By the way, anyone knows if Atmega8 can use 100 khz TWI with its internal 1 Mhz osc?
******/
/***SAMPLE CODE***
// Assign I/O stream to UART
FILE uart_str = FDEV_SETUP_STREAM(uart_putch, uart_getch, _FDEV_SETUP_RW);
int main(void)
{
  char mode,ichoice;
  int icount;
  // Initial PORT Used
  DDRB = 0xFE;                 // Set PB0=Input, Others Output
  PORTB = 0;
  DDRC = 0;                    // Set PORTC as Input
  PORTC = 0;
  DDRD = 0xFF;                 // Set PORTD as Output
  PORTD = 0;    

  // Define Output/Input Stream
  stdout = stdin = &uart_str;
  // Initial LCD using 4 bits data interface
  initlcd();
  LCD_putcmd(0x0C,LCD_2CYCLE);      // Display On, Cursor Off
  LCD_putcmd(LCD_CLEAR,LCD_2CYCLE); // Clear LCD
  // Initial ATMega168 UART Peripheral
  uart_init();
  // Initial ATMega168 TWI/I2C Peripheral
  TWSR = 0x00;   // Select Prescaler of 1
  // SCL frequency = 11059200 / (16 + 2 * 48 * 1) = 98.743 khz
  TWBR = 0x30;   // 48 Decimal
  // Initial ATMega168 ADC Peripheral
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);
  // Free running ADC Mode
  ADCSRB = 0x00;
  // Disable digital input on ADC0 and ADC1
  DIDR0 = 0x03;
  // Initial ATMega168 PWM using Timer/Counter2 Peripheral
  TCCR2A=0b10000011;            // Fast PWM Mode, Clear on OCRA
  TCCR2B=0b00000100;            // Used fclk/64 prescaller
  OCR2A=0xFF;                   // Initial the OC2A (PB3) Out to 0xFF
  // Initial ATMega168 Timer/Counter0 Peripheral
  TCCR0A=0x00;                  // Normal Timer0 Operation
  TCCR0B=(1<<CS02)|(1<<CS00);   // Use maximum prescaller: Clk/1024
  TCNT0=0x94;                   // Start counter from 0x94, overflow at 10 mSec
  TIMSK0=(1<<TOIE0);            // Enable Counter Overflow Interrupt
  sei();                        // Enable Interrupt
  // Initial mode
  mode=0;
  for(;;)
  {
    // Check if Button is pressed than enter to the Setup Mode
    if (bit_is_clear(PINB, PB0)) {          // if button is pressed
          _delay_us(100);                       // Wait for debouching
          if (bit_is_clear(PINB, PB0)) {        // if button is pressed
                mode = 1;

            cli();  // Disable Interrupt
            LCD_putcmd(LCD_CLEAR,LCD_2CYCLE);   // Clear LCD
            LCD_puts("Setup Mode");
        // Dimmming the LCD
            for (icount=255;icount > 0;icount--) {
              OCR2A=icount;
              _delay_ms(3);
            }
      }
    }
    if (mode) {
      ansi_me();
      ansi_cl();                            // Clear Screen
      ansi_cm(1,1);
      printf("AVRJazz Mega168 DS1307 RTC Setup");
          ansi_cm(3,1);
          printf("1. Time: %02d:%02d:%02d\n",ds1307_addr[2],ds1307_addr[1],ds1307_addr[0]);
          printf("2. Mode 24/12: %d, AM/PM: %d\n",hour_mode,ampm_mode);
          printf("3. Date: %02d-%02d-20%02d, Week Day: %d\n",ds1307_addr[4],ds1307_addr[5],ds1307_addr[6],ds1307_addr[3]);
          printf("4. Save and Exit\n");
          printf("5. Exit\n");
          printf("\nEnter Choice: ");
          if ((ichoice=getnumber(1,5)) < 0) continue;
          switch (ichoice) {
            case 1:  // DS1307 Time Setup
                  printf("\n\nHour [0-24]: ");
                  if ((ds1307_addr[2]=getnumber(0,24)) < 0) continue;
                  printf("\nMinute [0-59]: ");
                  if ((ds1307_addr[1]=getnumber(0,59)) < 0) continue;
                  printf("\nSecond [0-59]: ");
                  if ((ds1307_addr[0]=getnumber(0,59)) < 0) continue;
                  break;
        case 2:  // DS1307 Hour Mode Setup
                  printf("\n\nMode 0> 24, 1> 12: ");
                  if ((hour_mode=getnumber(0,1)) < 0) continue;
                  printf("\nAM/PM 0> AM, 1> PM: ");
                  if ((ampm_mode=getnumber(0,1)) < 0) continue;
                  break;
        case 3:  // DS1307 Date Setup
                  printf("\n\nWeekDay [1-7]: ");
                  if ((ds1307_addr[3]=getnumber(1,7)) < 0) continue;
              printf("\nDate [1-31]: ");
                  if ((ds1307_addr[4]=getnumber(1,31)) < 0) continue;                     ;
                  printf("\nMonth [1-12]: ");
                  if ((ds1307_addr[5]=getnumber(1,12)) < 0) continue;
                  printf("\nYear [0-99]: ");
                  if ((ds1307_addr[6]=getnumber(0,99)) < 0) continue;
                  break;
        case 4:  // Save to DS1307 Register and Exit Setup
                  Write_DS1307();
                case 5:  // Exit Setup
                  mode = 0;
                  ansi_cl();
          		// Illuminating the LCD
                  for (icount=0;icount < 255;icount++) {
                    OCR2A=icount;
                _delay_ms(3);
              }
                  TCNT0=0x94;
                  sei();   // Enable Interrupt
                  break;
          }
    }
  }
  return 0;
}
******/
/**
#ifdef DOXYGEN
 @defgroup pfleury_ic2master I2C Master library
 @code #include <i2cmaster.h> @endcode
 @brief I2C (TWI) Master Software Library
 Basic routines for communicating with I2C slave devices. This single master 
 implementation is limited to one bus master on the I2C bus.
 This I2c library is implemented as a compact assembler software implementation of the I2C protocol 
 which runs on any AVR (i2cmaster.S) and as a TWI hardware interface for all AVR with built-in TWI hardware (twimaster.c).
 Since the API for these two implementations is exactly the same, an application can be linked either against the
 software I2C implementation or the hardware I2C implementation.
 Use 4.7k pull-up resistor on the SDA and SCL pin.
 Adapt the SCL and SDA port and pin definitions and eventually the delay routine in the module 
 i2cmaster.S to your target when using the software I2C implementation !
 Adjust the  CPU clock frequence F_CPU in twimaster.c or in the Makfile when using the TWI hardware implementaion.
 @note 
    The module i2cmaster.S is based on the Atmel Application Note AVR300, corrected and adapted 
    to GNU assembler and AVR-GCC C call interface.
    Replaced the incorrect quarter period delays found in AVR300 with 
    half period delays.   
 @author Peter Fleury pfleury@gmx.ch  http://jump.to/fleury
 @par API Usage Example
  The following code shows typical usage of this library, see example test_i2cmaster.c
 @code
 #include <i2cmaster.h>
 #define Dev24C02  0xA2      // device address of EEPROM 24C02, see datasheet
 int main(void)
 {
     unsigned char ret;
     i2c_init();                             // initialize I2C library
     // write 0x75 to EEPROM address 5 (Byte Write) 
     i2c_start_wait(Dev24C02+I2C_WRITE);     // set device address and write mode
     i2c_write(0x05);                        // write address = 5
     i2c_write(0x75);                        // write value 0x75 to EEPROM
     i2c_stop();                             // set stop conditon = release bus
     // read previously written value back from EEPROM address 5 
     i2c_start_wait(Dev24C02+I2C_WRITE);     // set device address and write mode

     i2c_write(0x05);                        // write address = 5
     i2c_rep_start(Dev24C02+I2C_READ);       // set device address and read mode

     ret = i2c_readNak();                    // read one byte from EEPROM
     i2c_stop();
     for(;;);
 }
 @endcode
#endif // DOXYGEN
**/
/*
** module interrupts
*/
/*
ISR(TIMER0_OVF_vect)
{
  static unsigned char tenms=1;
  int iTemp;
  tenms++;                  // Read DS1307 every 100 x 10ms = 1 sec
  if (tenms >= 100) {
    cli();                                // Disable Interupt
    // Read DS1307
    Read_DS1307();
        // Display the Clock
        LCD_putcmd(LCD_HOME,LCD_2CYCLE);      // LCD Home
        LCD_puts(weekday[ds1307_addr[3] - 1]); LCD_puts(", ");
        LCD_puts(num2str(ds1307_addr[4])); LCD_puts(" ");
        LCD_puts(month[ds1307_addr[5] - 1]); LCD_puts(" ");
        LCD_puts("20"); LCD_puts(num2str(ds1307_addr[6]));
        LCD_putcmd(LCD_NEXT_LINE,LCD_2CYCLE); // Goto Second Line
        if (hour_mode) {
          LCD_puts(num2str(ds1307_addr[2])); LCD_puts(":");
          LCD_puts(num2str(ds1307_addr[1])); LCD_puts(":");
          LCD_puts(num2str(ds1307_addr[0])); 

          if (ampm_mode)
            LCD_puts(" PM");
      else
            LCD_puts(" AM");
        } else {
          LCD_puts(num2str(ds1307_addr[2])); LCD_puts(":");
          LCD_puts(num2str(ds1307_addr[1])); LCD_puts(":");
          LCD_puts(num2str(ds1307_addr[0])); LCD_puts("   ");
    }
        // Set ADMUX Channel for LM35DZ Input
    ADMUX=0x01;
        // Start conversion by setting ADSC on ADCSRA Register
        ADCSRA |= (1<<ADSC);
        // wait until convertion complete ADSC=0 -> Complete
    while (ADCSRA & (1<<ADSC));
        // Get the ADC Result
        iTemp = ADCW;
    // ADC = (Vin x 1024) / Vref, Vref = 1 Volt, LM35DZ Out = 10mv/C
        iTemp = (int)(iTemp) / 10.24;    

    // Dislay Temperature
        LCD_puts(" ");
        LCD_puts(num2str((char)iTemp));   // Display Temperature
        LCD_putch(0xDF);                  // Degree Character
        LCD_putch('C');                   // Centigrade
        tenms=1;
        sei();                            // Enable Interrupt
  }
  TCNT0=0x94;
}
*/
/*
** module procedure and function definitions
*/
// diff
uint8_t i2c_diff(uint8_t xi, uint8_t xf)
{
	return xf^xi;
}
// routine
unsigned char i2c_routine(unsigned char cmd)
{
	unsigned int i1;
	unsigned int i2;
	unsigned char block[i2c_sizeblock];
	unsigned char ByteofData;
	int keyfound;
	int diferenca;
	diferenca=i2c_diff(i2c_packet.command, cmd);
	if(diferenca){ // oneshot
		for(i1=0;i1<i2c_sizeeeprom;i1+=i2c_sizeblock){
			for(i2=0;i2<i2c_sizeblock;i2++){ // get block from eeprom
				ByteofData=*(i2c_eeprom+(i1+i2));
				block[i2]=ByteofData;
			}	
			keyfound=(block[0]==cmd && block[1]==i2c_packet.instruction); // bool
			if(keyfound){
				i2c_packet.command=block[0];
				i2c_packet.feedback=block[1];
				i2c_packet.instruction=block[2];
				break;
			}else{
				i2c_packet.feedback=i2c_packet.instruction;
			}
		}
		i2c_packet.command=cmd; // importante
	}
	// printf("Eoutput -> %d\n",r->present);
	return i2c_packet.instruction;
}
unsigned char i2c_txpush(unsigned char data)
{
	unsigned char ret;
	unsigned char push;
	push = (I2C_TxHead + 1) & I2C_TX_BUFFER_MASK;
	if(I2C_TxTail != push){
		I2C_TxBuf[I2C_TxHead] = data;
		I2C_TxHead = push;
		ret=1;
	}else{
		I2C_TxBuf[push] = '\0';
		ret=0;
	}
	return ret;
}
unsigned char i2c_txpop(void)
{
	unsigned char pop;
	unsigned char data;
	pop = (I2C_TxTail + 1) & I2C_TX_BUFFER_MASK;
	if(I2C_TxTail != I2C_TxHead){
		data=I2C_TxBuf[I2C_TxTail];
		I2C_TxTail=pop;
	}else{
		data='\0';
	}
	return data;
}
unsigned char i2c_rxpush(unsigned char data)
{
	unsigned char ret;
	unsigned char push;
	push = (I2C_RxHead + 1) & I2C_RX_BUFFER_MASK;
	if(I2C_RxTail != push){
		I2C_RxBuf[I2C_RxHead] = data;
		I2C_RxHead = push;
		ret=1;
	}else{
		I2C_RxBuf[push] = '\0';
		ret=0;
	}
	return ret;
}
unsigned char i2c_rxpop(void)
{
	unsigned char pop;
    unsigned char data;
	pop = (I2C_RxTail + 1) & I2C_RX_BUFFER_MASK;
	if(I2C_RxTail != I2C_RxHead){	
		data = I2C_RxBuf[I2C_RxTail];
		I2C_RxTail=pop;
	}else{
		data='\0';
	}
	return data;
}
/*
** module interrupts
*/
//! I2C (TWI) interrupt service routine
SIGNAL(SIG_2WIRE_SERIAL)
{
	// put in circular buffer or ring buffer
	unsigned char command;
	command=I2C_RECEIVED_DATABYTE_INTERRUPT;
	i2c_rxpush(I2C_VALUE);
	i2c_routine(command);
	//i2c_packet.status=(I2C_STATUS & I2C_STATUS_MASK);
	//i2c_packet.control=I2C_CONTROL;
	//command=(I2C_STATUS & I2C_STATUS_MASK);
	/*
	switch(twi_instruction){
		// Master General
		case TW_START: // 0x08: Sent start condition
		case TW_REP_START: // 0x10: Sent repeated start condition
			// send device address
			i2cSendByte(I2cDeviceAddrRW);
			//outb(TWDR, I2cDeviceAddrRW);
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
			break;

		// Master Transmitter & Receiver status codes
		case TW_MT_SLA_ACK: // 0x18: Slave address acknowledged
		case TW_MT_DATA_ACK: // 0x28: Data acknowledged
			if(I2cSendDataIndex < I2cSendDataLength){
				// send data
				i2cSendByte( I2cSendData[I2cSendDataIndex++] );
				//outb(TWDR, I2cSendData[I2cSendDataIndex++]);
				//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
			}else{
				// transmit stop condition, enable SLA ACK
				i2cSendStop();
				//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
				// set state
				I2cState = I2C_IDLE;
			}
			break;
		case TW_MR_DATA_NACK: // 0x58: Data received, NACK reply issued
			// store final received data byte
			I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
		// continue to transmit STOP condition
		case TW_MR_SLA_NACK: // 0x48: Slave address not acknowledged
		case TW_MT_SLA_NACK: // 0x20: Slave address not acknowledged
		case TW_MT_DATA_NACK: // 0x30: Data not acknowledged
			// transmit stop condition, enable SLA ACK
			i2cSendStop();
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
			// set state
			I2cState = I2C_IDLE;
			break;
		case TW_MT_ARB_LOST: // 0x38: Bus arbitration lost
			//case TW_MR_ARB_LOST: // 0x38: Bus arbitration lost
			// release bus
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
			// set state
			I2cState = I2C_IDLE;
			// release bus and transmit start when bus is free
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
			break;
		case TW_MR_DATA_ACK: // 0x50: Data acknowledged
			// store received data byte
			I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
			// fall-through to see if more bytes will be received
		case TW_MR_SLA_ACK: // 0x40: Slave address acknowledged
			if(I2cReceiveDataIndex < (I2cReceiveDataLength-1))
				// data byte will be received, reply with ACK (more bytes in transfer)
				i2cReceiveByte(TRUE);
				//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
			else
				// data byte will be received, reply with NACK (final byte in transfer)
				i2cReceiveByte(FALSE);
				//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
			break;

		// Slave Receiver status codes
		case TW_SR_SLA_ACK: // 0x60: own SLA+W has been received, ACK has been returned
		case TW_SR_ARB_LOST_SLA_ACK: // 0x68: own SLA+W has been received, ACK has been returned
		case TW_SR_GCALL_ACK: // 0x70: GCA+W has been received, ACK has been returned
		case TW_SR_ARB_LOST_GCALL_ACK: // 0x78: GCA+W has been received, ACK has been returned
			// we are being addressed as slave for writing (data will be received from master)
			// set state
			I2cState = I2C_SLAVE_RX;
			// prepare buffer
			I2cReceiveDataIndex = 0;
			// receive data byte and return ACK
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
			break;
		case TW_SR_DATA_ACK: // 0x80: data byte has been received, ACK has been returned
		case TW_SR_GCALL_DATA_ACK: // 0x90: data byte has been received, ACK has been returned
			// get previously received data byte
			I2cReceiveData[I2cReceiveDataIndex++] = inb(TWDR);
			// check receive buffer status
			if(I2cReceiveDataIndex < I2C_RECEIVE_DATA_BUFFER_SIZE){
				// receive data byte and return ACK
				outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
			}else{
				// receive data byte and return NACK
				outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
			}
			break;
		case TW_SR_DATA_NACK: // 0x88: data byte has been received, NACK has been returned
		case TW_SR_GCALL_DATA_NACK: // 0x98: data byte has been received, NACK has been returned
			// receive data byte and return NACK
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
			break;
		case TW_SR_STOP: // 0xA0: STOP or REPEATED START has been received while addressed as slave
			// switch to SR mode with SLA ACK
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
			// i2c receive is complete, call i2cSlaveReceive
			if(i2cSlaveReceive) i2cSlaveReceive(I2cReceiveDataIndex, I2cReceiveData);
				// set state
				I2cState = I2C_IDLE;
			break;

		// Slave Transmitter
		case TW_ST_SLA_ACK: // 0xA8: own SLA+R has been received, ACK has been returned
		case TW_ST_ARB_LOST_SLA_ACK: // 0xB0: GCA+R has been received, ACK has been returned
			// we are being addressed as slave for reading (data must be transmitted back to master)
			// set state
			I2cState = I2C_SLAVE_TX;
			// request data from application
			if(i2cSlaveTransmit) I2cSendDataLength = i2cSlaveTransmit(I2C_SEND_DATA_BUFFER_SIZE, I2cSendData);
				// reset data index
				I2cSendDataIndex = 0;
				// fall-through to transmit first data byte
		case TW_ST_DATA_ACK: // 0xB8: data byte has been transmitted, ACK has been received
			// transmit data byte
			outb(TWDR, I2cSendData[I2cSendDataIndex++]);
			if(I2cSendDataIndex < I2cSendDataLength)
				// expect ACK to data byte
				outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
			else
				// expect NACK to data byte
				outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
			break;
		case TW_ST_DATA_NACK: // 0xC0: data byte has been transmitted, NACK has been received
		case TW_ST_LAST_DATA: // 0xC8:
			// all done
			// switch to open slave
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
			// set state
			I2cState = I2C_IDLE;
		break;

		// Misc
		case TW_NO_INFO: // 0xF8: No relevant state information
			// do nothing
			break;
		case TW_BUS_ERROR: // 0x00: Bus error due to illegal start or stop condition
			// reset internal hardware and release bus
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
			// set state
			I2cState = I2C_IDLE;
			break;
	}
	//outb(PORTB, ~I2cState);
	*/
}
/***EOF***/
