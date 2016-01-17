/************************************************************************
Title:    ATMEGA128API
Author:   Sergio Manuel Santos <sergio.salazar.santos@gmail.com>
File:     $Id: Atmega128API.h,v 0.2 2015/04/11 00:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: AVR ATmega128
License:  GNU General Public License 
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
	
************************************************************************/
#ifndef ATMEGA128API_H
	#define ATMEGA128API_H
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
#include <inttypes.h>
#include <avr/eeprom.h>
/************************************************************************
ANALOG API START
************************************************************************/
/*
** constant and macro
*/
#define ADC_NUMBER_SAMPLE 2 // ADC_NUMBER_SAMPLE^2 gives number of samples, note values can only range from 0 to 4.
/*
** variable
*/
struct ANALOG{
	/***Parameters***/
	uint8_t VREFF;
	uint8_t DIVISION_FACTOR;
	//prototype pointers
	int (*read)(int selection);
};
typedef struct ANALOG ANALOG;
/*
** procedure and function header
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... );
/***EOF***/
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
struct EEPROM{
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
typedef struct EEPROM EEPROM;
/*
** procedure and function header
*/
EEPROM EEPROMenable(void);
/***EOF***/
/*************************************************************************
EEPROM API END
*************************************************************************/
/************************************************************************
FUNCTION API START
************************************************************************/
/*
** constant and macro
*/
/*
** variable
*/
struct FUNC{
	/***PROTOTYPES VTABLE***/
	unsigned int (*power)(uint8_t base, uint8_t n);
	int (*stringlength)(const char string[]);
	void (*reverse)(char s[]);
	unsigned int (*mayia)(unsigned int xi, unsigned int xf, uint8_t nbits);
	uint8_t (*pinmatch)(uint8_t match, uint8_t pin, uint8_t HL);
	uint8_t (*lh)(uint8_t xi, uint8_t xf);
	uint8_t (*hl)(uint8_t xi, uint8_t xf);
	uint8_t (*diff)(uint8_t xi, uint8_t xf);
	uint8_t (*hmerge)(uint8_t X, uint8_t Y);
	uint8_t (*lmerge)(uint8_t X, uint8_t Y);
	void (*swap)(long *px, long *py);
	void (*copy)(char to[], char from[]);
	void (*squeeze)(char s[], int c);
	void (*shellsort)(int v[], int n);
	void (*itoa)(int32_t n, char s[]);
	int (*trim)(char s[]);
	int (*pmax)(int a1, int a2);
	int (*gcd)(int u, int v);
	int (*strToInt)(const char string[]);
	uint8_t (*filter)(uint8_t mask, uint8_t data);
	unsigned int (*ticks)(unsigned int num);
	int (*twocomptoint8bit)(int twoscomp);
	int (*twocomptoint10bit)(int twoscomp);
	int (*twocomptointnbit)(int twoscomp, uint8_t nbits);
	char (*dec2bcd)(char num);
	char (*bcd2dec)(char num);
	char* (*resizestr)(char *string, int size);
	long (*trimmer)(long x, long in_min, long in_max, long out_min, long out_max);
	unsigned char (*bcd2bin)(unsigned char val);
	unsigned char (*bin2bcd)(unsigned val);
	long (*gcd1)(long a, long b);
	uint8_t (*pincheck)(uint8_t port, uint8_t pin);
	char* (*print_binary)(int number);
};
typedef struct FUNC FUNC;
/*
** procedure and function header
*/
FUNC FUNCenable(void);
/***EOF***/
/************************************************************************
FUNCTION API END
************************************************************************/
/************************************************************************
INTERRUPT API START
************************************************************************/
/*
** constant and macro
*/
/*
** variable
*/
struct INTERRUPT{
	void (*set)(uint8_t channel, uint8_t sense);
	void (*off)(uint8_t channel);
	uint8_t (*reset_status)(void);
};
typedef struct INTERRUPT INTERRUPT;
/*
** procedure and function header
*/
INTERRUPT INTERRUPTenable(void);
/***EOF***/
/************************************************************************
INTERRUPT API END
************************************************************************/
/************************************************************************
LCD API START
************************************************************************/
/*
** constant and macro
*/
//ASIGN PORT PINS TO LCD (can be setup in any way)
#define RS 0
#define RW 1
#define EN 2
#define NC 3
#define DB0 4
#define DB1 5
#define DB2 6
#define DB3 7
/*
** variable
*/
struct display{
	/******/
	void (*write)(char c, unsigned short D_I);
	char (*read)(unsigned short D_I);
	void (*BF)(void);
	void (*putch)(char c);
	char (*getch)(void);
	void (*string)(const char *s);
	void (*string_size)(const char* s, uint8_t size); // RAW
	void (*hspace)(uint8_t n);
	void (*clear)(void);
	void (*gotoxy)(unsigned int x, unsigned int y);
	void (*reboot)(void);
};
typedef struct display LCD0;
typedef struct display LCD1;
/*
** procedure and function header
*/
LCD0 LCD0enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
LCD1 LCD1enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
/***EOF***/
/************************************************************************
LCD API END
************************************************************************/
/*************************************************************************
SPI API START
*************************************************************************/
/*
** constant and macro
*/
#define SPI_LSB_DATA_ORDER 1
#define SPI_MSB_DATA_ORDER 0
#define SPI_MASTER_MODE 1
#define SPI_SLAVE_MODE 0
/*
** variable
*/
struct SPI{
	/***/
	void (*transfer_sync) (uint8_t * dataout, uint8_t * datain, uint8_t len);
	void (*transmit_sync) (uint8_t * dataout, uint8_t len);
	uint8_t (*fast_shift) (uint8_t data);
};
typedef struct SPI SPI;
/*
** procedure and function header
*/
SPI SPIenable(uint8_t master_slave_select, uint8_t data_order,  uint8_t data_modes, uint8_t prescaler);
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
/*
** variable
*/
struct TIMER_COUNTER0{
	// prototype pointers
	void (*compoutmode)(unsigned char compoutmode);
	void (*compoutmodeA)(unsigned char compoutmode);
	void (*compoutmodeB)(unsigned char compoutmode);
	void (*compare)(unsigned char compare);
	void (*compareA)(unsigned char compare);
	void (*compareB)(unsigned char compare);
	void (*start)(unsigned int prescaler);
	void (*stop)(void);
};
typedef struct TIMER_COUNTER0 TIMER_COUNTER0;
/**/
struct TIMER_COUNTER1{
	// prototype pointers
	void (*compoutmodeA)(unsigned char compoutmode);
	void (*compoutmodeB)(unsigned char compoutmode);
	void (*compoutmodeC)(unsigned char compoutmode);
	void (*compareA)(uint16_t compareA);
	void (*compareB)(uint16_t compareB);
	void (*compareC)(uint16_t compareC);
	void (*start)(unsigned int prescaler);
	void (*stop)(void);
};
typedef struct TIMER_COUNTER1 TIMER_COUNTER1;
/**/
struct TIMER_COUNTER2{
	// prototype pointers
	void (*compoutmode)(unsigned char compoutmode);
	void (*compoutmodeA)(unsigned char compoutmode);
	void (*compoutmodeB)(unsigned char compoutmode);
	void (*compare)(unsigned char compare);
	void (*compareA)(unsigned char compare);
	void (*compareB)(unsigned char compare);
	void (*start)(unsigned int prescaler);
	void (*stop)(void);
};
typedef struct TIMER_COUNTER2 TIMER_COUNTER2;
/**/
struct TIMER_COUNTER3{
	// prototype pointers
	void (*compoutmodeA)(unsigned char compoutmode);
	void (*compoutmodeB)(unsigned char compoutmode);
	void (*compoutmodeC)(unsigned char compoutmode);
	void (*compareA)(uint16_t compareA);
	void (*compareB)(uint16_t compareB);
	void (*compareC)(uint16_t compareC);
	void (*start)(unsigned int prescaler);
	void (*stop)(void);
};
typedef struct TIMER_COUNTER3 TIMER_COUNTER3;
/*
** procedure and function header
*/
TIMER_COUNTER0 TIMER_COUNTER0enable(unsigned char wavegenmode, unsigned char interrupt);
TIMER_COUNTER1 TIMER_COUNTER1enable(unsigned char wavegenmode, unsigned char interrupt);
TIMER_COUNTER2 TIMER_COUNTER2enable(unsigned char wavegenmode, unsigned char interrupt);
TIMER_COUNTER3 TIMER_COUNTER3enable(unsigned char wavegenmode, unsigned char interrupt);
/***EOF***/
/*************************************************************************
TIMER API END
*************************************************************************/
/************************************************************************
I2C API START
testing phase
************************************************************************/
/*
** constant and macro
*/
// devices
#define DS1307_ID 0xD0			// I2C DS1307 Device Identifier
#define DS1307_ADDR 0X00		// I2C DS1307 Device Address offset
#define TC74_ID 0x9A			// device address of TC74
#define TC74_ADDR 0X00
#define Dev24C02_ID 0xA2		//device address 24C02
#define Dev24C02_ADDR 0x00
#define LM73_ID 0x90			//LM73 address temperature sensor
/***/
#define TWI_WRITE 0
#define TWI_READ 1
#define TWI_ACK 1
#define TWI_NACK 0
#define TWI_MASTER_MODE 0
#define TWI_SLAVE_MODE 1
/*
** variable
*/
unsigned char i2c_output;
struct I2C{
	/***PROTOTYPES VTABLE***/
	void (*start)(unsigned char mode);
	void (*master_connect)(unsigned char addr, unsigned char rw);
	void (*master_write)(unsigned char data);
	unsigned char (*master_read)(unsigned char request);
	void (*stop)(void);
};
typedef struct I2C I2C;
/*
** procedure and function header
*/
I2C I2Cenable(unsigned char device_id, unsigned char prescaler);
/***EOF***/
/************************************************************************
I2C API END
testing phase
************************************************************************/
/************************************************************************
UART API START
************************************************************************/
/*
** constant and macro
*/
/** @brief  UART Baudrate Expression
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)
/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz           
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)
/** Size of the circular receive buffer, must be power of 2 */
#ifndef UART_RX_BUFFER_SIZE
	#define UART_RX_BUFFER_SIZE 32
#endif
/** Size of the circular transmit buffer, must be power of 2 */
#ifndef UART_TX_BUFFER_SIZE
	#define UART_TX_BUFFER_SIZE 32
#endif
/* test if the size of the circular buffers fits into SRAM */
#if ( (UART_RX_BUFFER_SIZE+UART_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
	#error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif
/* 
** high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /* no receive data available   */
/***Parity choices***/
#define NONE 0
#define EVEN 2
#define ODD 3
/*
** variable
*/
struct UART{
	/***Parameters***/
	unsigned int ubrr;
	unsigned int FDbits;
	unsigned int Stopbits;
	unsigned int Parity;
	// prototype pointers
	char* (*read)(void);
	unsigned int (*getc)(void);
	void (*putc)(unsigned char data);
	void (*puts)(const char *s );
	int (*available)(void);
	void (*flush)(void);
};
typedef struct UART UART;
struct UART1{
	/***Parameters***/
	unsigned int ubrr;
	unsigned int FDbits;
	unsigned int Stopbits;
	unsigned int Parity;
	//prototype pointers
	char* (*read)(void);
	unsigned int (*getc)(void);
	void (*putc)(unsigned char data);
	void (*puts)(const char *s );
	int (*available)(void);
	void (*flush)(void);
};
typedef struct UART1 UART1;
/*
** procedure and function header
*/
UART UARTenable(unsigned int baudrate, unsigned int FDbits, unsigned int Stopbits, unsigned int Parity );
UART1 UART1enable(unsigned int baudrate, unsigned int FDbits, unsigned int Stopbits, unsigned int Parity );
/**
   @brief   Initialize UART and set baudrate 
   @param   baudrate Specify baudrate using macro UART_BAUD_SELECT()
   @return  none
*/
//extern void uart_init(unsigned int baudrate);
/**
 *  @brief   Get received byte from ringbuffer
 *
 * Returns in the lower byte the received character and in the 
 * higher byte the last receive error.
 * UART_NO_DATA is returned when no data is available.
 *
 *  @param   void
 *  @return  lower byte:  received byte from ringbuffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from UART
 *           - \b UART_NO_DATA           
 *             <br>no receive data available
 *           - \b UART_BUFFER_OVERFLOW   
 *             <br>Receive ringbuffer overflow.
 *             We are not reading the receive buffer fast enough, 
 *             one or more received character have been dropped 
 *           - \b UART_OVERRUN_ERROR     
 *             <br>Overrun condition by UART.
 *             A character already present in the UART UDR register was 
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b UART_FRAME_ERROR       
 *             <br>Framing Error by UART
 */
//extern unsigned int uart_getc(void);
/**
 *  @brief   Put byte to ringbuffer for transmitting via UART
 *  @param   data byte to be transmitted
 *  @return  none
 */
//extern void uart_putc(unsigned char data);
/**
 *  @brief   Put string to ringbuffer for transmitting via UART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 * 
 *  @param   s string to be transmitted
 *  @return  none
 */
//extern void uart_puts(const char *s );
/**
 * @brief    Put string from program memory to ringbuffer for transmitting via UART.
 *
 * The string is buffered by the uart library in a circular buffer
 * and one character at a time is transmitted to the UART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uart_puts_P
 */
extern void uart_puts_p(const char *s );
/**
 * @brief    Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))
/**
 *  @brief   Return number of bytes waiting in the receive buffer
 *  @param   none
 *  @return  bytes waiting in the receive buffer
 */
//extern int uart_available(void);
/**
 *  @brief   Flush bytes waiting in receive buffer
 *  @param   none
 *  @return  none
 */
//extern void uart_flush(void);
/** @brief  Initialize USART1 (only available on selected ATmegas) @see uart_init */
//extern void uart1_init(unsigned int baudrate);
/** @brief  Get received byte of USART1 from ringbuffer. (only available on selected ATmega) @see uart_getc */
//extern unsigned int uart1_getc(void);
/** @brief  Put byte to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_putc */
//extern void uart1_putc(unsigned char data);
/** @brief  Put string to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts */
//extern void uart1_puts(const char *s );
/** @brief  Put string from program memory to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts_p */
extern void uart1_puts_p(const char *s );
/** @brief  Macro to automatically put a string constant into program memory */
#define uart1_puts_P(__s)       uart1_puts_p(PSTR(__s))
/** @brief   Return number of bytes waiting in the receive buffer */
//extern int uart1_available(void);
/** @brief   Flush bytes waiting in receive buffer */
//extern void uart1_flush(void);
/***EOF***/
/************************************************************************
UART API END
************************************************************************/
/************************************************************************
VFSM API START
************************************************************************/
/*
** constant and macro
*/
/*
** variable
*/
struct VFSM{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	uint8_t input;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct VFSM *r, uint8_t mask, uint8_t input, uint8_t output);
};
struct VLOGIC{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct VLOGIC *l, uint8_t mask, uint8_t input, uint8_t output);
};
typedef struct VFSM VFSM;
typedef struct VLOGIC VLOGIC;
/*
** procedure and function header
*/
VFSM VFSMenable(uint8_t *veeprom, unsigned int sizeeeprom );
VLOGIC VLOGICenable(uint8_t *veeprom, unsigned int sizeeeprom );
/***EOF***/
/************************************************************************
VFSM API END
************************************************************************/
/************************************************************************
FSM API START
************************************************************************/
/*
** constant and macro
*/
#define EMPTY 255
#define SFSM_PAGES 8
/*
** variable
*/
struct EFSM{
	uint8_t sizeeeprom;
	uint8_t sizeblock;
	uint8_t page;
	uint8_t record[5];
	char type;
	/***PROTOTYPES VTABLE***/
	uint8_t (*read)(struct EFSM* fsm, uint8_t input, uint8_t feedback);
	int (*learn)(struct EFSM *r, uint8_t input, uint8_t next, uint8_t feedback);
	unsigned int (*quant)(struct EFSM *r);
	int (*remove)(struct EFSM *r, uint8_t input, uint8_t state);
	int (*deleteall)(struct EFSM *r);
	uint8_t (*feedback)(struct EFSM *r);
	uint8_t (*present)(struct EFSM *r);
	char (*typeget)(struct EFSM *r);
	uint8_t (*diff)(uint8_t xi, uint8_t xf);
};
typedef struct EFSM EFSM;
struct SFSM{
	uint8_t *eeprom;
	unsigned int sizeeeprom;
	unsigned int sizeblock;
	uint8_t input[SFSM_PAGES];
	uint8_t state[SFSM_PAGES];
};
typedef struct SFSM SFSM;
struct LFSM{
	int *eeprom;
	unsigned int sizeeeprom;
	unsigned int sizeblock;
	int command;
	int feedback;
	int instruction;
};
/*
** procedure and function header
*/
EFSM EFSMenable(unsigned int sizeeeprom, uint8_t prog );
void SFSMinit(uint8_t *eeprom, unsigned int sizeeeprom);
uint8_t sfsm_read(uint8_t input, uint8_t feedback, uint8_t page);
uint8_t sfsm_state(uint8_t page);
/***EOF***/
/************************************************************************
FSM API END
************************************************************************/
/************************************************************************
CLOCK API START
************************************************************************/
/*
** constant and macro
*/
#define HORA 24
/*
** variable
*/
struct TIME{
	int8_t hour;
	int8_t minute;
	int8_t second;
};
struct CLOCK{
	void (*set)(uint8_t hour, uint8_t minute, uint8_t second);
	void (*increment)(void);
	void (*decrement)(void);
	uint8_t (*alarm)(uint8_t hour, uint8_t minute, uint8_t second);
	uint8_t (*second_count)(uint16_t second);
	void (*second_count_reset)(void);
	void (*second_count_stop)(void);
	void (*alarm_reset)(void);
	void (*alarm_stop)(void);
	char* (*show)(void);
};
typedef struct CLOCK CLOCK;
/*
** procedure and function header
*/
CLOCK CLOCKenable(uint8_t hour, uint8_t minute, uint8_t second);
/***EOF***/
/************************************************************************
CLOCK API END
************************************************************************/
/************************************************************************
MM74C923 API START
************************************************************************/
/*
** constant and macro
*/
#define MM74C923_DATA_OUT_A 7 //ic pin 19 LSB
#define MM74C923_DATA_OUT_B 6 //ic pin 18
#define MM74C923_DATA_OUT_C 5 //ic pin 17
#define MM74C923_DATA_OUT_D 4 //ic pin 16
#define MM74C923_DATA_OUT_E 3 //ic pin 15
#define MM74C923_EXTRA_DATA_OUT_PIN 2 // MSB
#define MM74C923_OUTPUT_ENABLE 1 //ic pin 14
#define MM74C923_DATA_AVAILABLE 0 //ic pin 13
/*
** variable
*/
struct mm74c923{
	void (*activate)(void);
	char (*getch)(void);
	char* (*gets)(void);
	char* (*data)(void);
	void (*data_clear)(void);
};
typedef struct mm74c923 MM74C923;
/*
** procedure and function header
*/
MM74C923 MM74C923enable(volatile uint8_t *ddr, volatile uint8_t *pin, volatile uint8_t *port);
/***EOF***/
/************************************************************************
MM74C923 API END
************************************************************************/
#endif
