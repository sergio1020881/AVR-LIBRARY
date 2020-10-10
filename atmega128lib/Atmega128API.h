/************************************************************************
ATMEGA128 API START
Author: Sergio Santos 
	<sergio.salazar.santos@gmail.com>
	28092020
************************************************************************/
/***preamble inic***/
#ifndef _ATMEGA128API_H_
	#define _ATMEGA128API_H_
/**@{*/
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** Library
*/
#include <inttypes.h>
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
struct nlg{
	/***Parameters***/
	uint8_t VREFF;
	uint8_t DIVISION_FACTOR;
	//prototype pointers
	int (*read)(int selection);
};
typedef struct nlg ANALOG;
/*
** procedure and function header
*/
ANALOG ANALOGenable( uint8_t Vreff, uint8_t Divfactor, int n_channel, ... );
/*************************************************************************
ANALOG API END
*************************************************************************/
/************************************************************************
INTERRUPT API START
************************************************************************/
/*
** constant and macro
*/
/*
** variable
*/
struct ntrrpt{
	void (*set)(uint8_t channel, uint8_t sense);
	void (*off)(uint8_t channel);
	uint8_t (*reset_status)(void);
};
typedef struct ntrrpt INTERRUPT;
/*
** procedure and function header
*/
INTERRUPT INTERRUPTenable(void);
/************************************************************************
INTERRUPT API END
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
struct sp{
	/***/
	void (*transfer_sync) (uint8_t * dataout, uint8_t * datain, uint8_t len);
	void (*transmit_sync) (uint8_t * dataout, uint8_t len);
	uint8_t (*fast_shift) (uint8_t data);
};
typedef struct sp SPI;
/*
** procedure and function header
*/
SPI SPIenable(uint8_t master_slave_select, uint8_t data_order,  uint8_t data_modes, uint8_t prescaler);
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
struct tmr_cntr0{
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
typedef struct tmr_cntr0 TIMER_COUNTER0;
/**/
struct tmr_cntr1{
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
typedef struct tmr_cntr1 TIMER_COUNTER1;
/**/
struct tmr_cntr2{
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
typedef struct tmr_cntr2 TIMER_COUNTER2;
/**/
struct tmr_cntr3{
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
typedef struct tmr_cntr3 TIMER_COUNTER3;
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
struct tw{
	/***PROTOTYPES VTABLE***/
	void (*start)(unsigned char mode);
	void (*master_connect)(unsigned char addr, unsigned char rw);
	void (*master_write)(unsigned char data);
	unsigned char (*master_read)(unsigned char request);
	void (*stop)(void);
};
typedef struct tw I2C;
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
struct rt{
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
typedef struct rt UART;
struct rt1{
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
typedef struct rt1 UART1;
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
#endif
/************************************************************************
ATMEGA128 API END
************************************************************************/
