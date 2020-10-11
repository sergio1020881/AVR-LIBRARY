/*************************************************************************
Title:    Interrupt UART library with receive/transmit circular buffers
Author:   Sergio Manuel Santos 
    <sergio.salazar.santos@gmail.com>
          Peter Fleury
File:     $Id: uart.c,v 1.6.2.1 2014/04/21 13:00:00 sergio Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: any AVR with built-in UART,
License:  GNU General Public License
Comment:
	stable
*************************************************************************/
/*
** library
*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <inttypes.h>
/***/
#include "uart.h"
/*
** constant and macro
*/
/* size of RX/TX buffers */
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1)
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
	#error RX buffer size is not a power of 2
#endif
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
	#error TX buffer size is not a power of 2
#endif
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
/***MCU SELECTOR PRE-PROCESSOR SYNTACTICAL SUGAR***/
/***TYPE 1***/
#if defined(__AVR_AT90S2313__) \
 || defined(__AVR_AT90S4414__) || defined(__AVR_AT90S4434__) \
 || defined(__AVR_AT90S8515__) || defined(__AVR_AT90S8535__) \
 || defined(__AVR_ATmega103__)
	/* old AVR classic or ATmega103 with one UART */
	 #define AT90_UART
	 #define UART0_RECEIVE_INTERRUPT   UART_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  UART_UDRE_vect
	 #define UART0_STATUS   USR
	 #define UART0_CONTROL  UCR
	 #define UART0_DATA     UDR  
	 #define UART0_UDRIE    UDRIE
/***TYPE 2***/
#elif defined(__AVR_AT90S2333__) || defined(__AVR_AT90S4433__)
	/* old AVR classic with one UART */
	 #define AT90_UART
	 #define UART0_RECEIVE_INTERRUPT   UART_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  UART_UDRE_vect
	 #define UART0_STATUS   UCSRA
	 #define UART0_CONTROL  UCSRB
	 #define UART0_DATA     UDR 
	 #define UART0_UDRIE    UDRIE
/***TYPE 3***/
#elif  defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega323__)
	/* ATmega with one USART */
	 #define ATMEGA_USART
	 #define UART0_RECEIVE_INTERRUPT   USART_RXC_vect
	 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
	 #define UART0_STATUS   UCSRA
	 #define UART0_CONTROL  UCSRB
	 #define UART0_DATA     UDR
	 #define UART0_UDRIE    UDRIE
/***TYPE 4***/
#elif  defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
	/* ATmega with one USART */
	 #define ATMEGA_USART
	 #define UART0_RECEIVE_INTERRUPT   USART_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
	 #define UART0_STATUS   UCSRA
	 #define UART0_CONTROL  UCSRB
	 #define UART0_DATA     UDR
	 #define UART0_UDRIE    UDRIE
/***TYPE 5***/
#elif defined(__AVR_ATmega163__) 
	/* ATmega163 with one UART */
	 #define ATMEGA_UART
	 #define UART0_RECEIVE_INTERRUPT   UART_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  UART_UDRE_vect
	 #define UART0_STATUS   UCSRA
	 #define UART0_CONTROL  UCSRB
	 #define UART0_DATA     UDR
	 #define UART0_UDRIE    UDRIE
/***TYPE 6***/
#elif defined(__AVR_ATmega162__) 
	/* ATmega with two USART */
	 #define ATMEGA_USART0
	 #define ATMEGA_USART1
	 #define UART0_RECEIVE_INTERRUPT   USART0_RXC_vect
	 #define UART1_RECEIVE_INTERRUPT   USART1_RXC_vect
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
/***TYPE 7***/
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
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
/***TYPE 8***/
#elif defined(__AVR_ATmega161__)
	/* ATmega with UART */
	 #error "AVR ATmega161 currently not supported by this libaray !"
/***TYPE 9***/
#elif defined(__AVR_ATmega169__) 
	/* ATmega with one USART */
	 #define ATMEGA_USART
	 #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
	 #define UART0_STATUS   UCSRA
	 #define UART0_CONTROL  UCSRB
	 #define UART0_DATA     UDR
	 #define UART0_UDRIE    UDRIE
/***TYPE 10***/
#elif defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
      defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
      defined(__AVR_ATmega328P__) 
	/* TLS-Added 48P/88P/168P/328P */
	/* ATmega with one USART */
	 #define ATMEGA_USART0
	 #define UART0_RECEIVE_INTERRUPT   USART_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
	 #define UART0_STATUS   UCSR0A
	 #define UART0_CONTROL  UCSR0B
	 #define UART0_DATA     UDR0
	 #define UART0_UDRIE    UDRIE0
/***TYPE 11***/
#elif defined(__AVR_ATtiny2313__)
	 #define ATMEGA_USART
	 #define UART0_RECEIVE_INTERRUPT   USART_RX_vect 
	 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
	 #define UART0_STATUS   UCSRA
	 #define UART0_CONTROL  UCSRB
	 #define UART0_DATA     UDR
	 #define UART0_UDRIE    UDRIE
/***TYPE 12***/
#elif defined(__AVR_ATmega329__) ||\
      defined(__AVR_ATmega649__) ||\
      defined(__AVR_ATmega325__) ||defined(__AVR_ATmega3250__) ||\
      defined(__AVR_ATmega645__) ||defined(__AVR_ATmega6450__)
	/* ATmega with one USART */
	  #define ATMEGA_USART0
	  #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
	  #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
	  #define UART0_STATUS   UCSR0A
	  #define UART0_CONTROL  UCSR0B
	  #define UART0_DATA     UDR0
	  #define UART0_UDRIE    UDRIE0
/***TYPE 13***/
#elif defined(__AVR_ATmega3290__) || defined(__AVR_ATmega6490__)
	/* TLS-Separated these two from the previous group because of inconsistency in the USART_RX */
	/* ATmega with one USART */
	  #define ATMEGA_USART0
	  #define UART0_RECEIVE_INTERRUPT   USART_RX_vect
	  #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
	  #define UART0_STATUS   UCSR0A
	  #define UART0_CONTROL  UCSR0B
	  #define UART0_DATA     UDR0
	  #define UART0_UDRIE    UDRIE0
/***TYPE 14***/
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega640__)
	/* ATmega with two USART */
	  #define ATMEGA_USART0
	  #define ATMEGA_USART1
	  #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
	  #define UART1_RECEIVE_INTERRUPT   USART0_UDRE_vect
	  #define UART0_TRANSMIT_INTERRUPT  USART1_RX_vect
	  #define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
	  #define UART0_STATUS   UCSR0A
	  #define UART0_CONTROL  UCSR0B
	  #define UART0_DATA     UDR0
	  #define UART0_UDRIE    UDRIE0
	  #define UART1_STATUS   UCSR1A
	  #define UART1_CONTROL  UCSR1B
	  #define UART1_DATA     UDR1
	  #define UART1_UDRIE    UDRIE1  
/***TYPE 15***/
#elif defined(__AVR_ATmega644__)
	/* ATmega with one USART */
	 #define ATMEGA_USART0
	 #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
	 #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
	 #define UART0_STATUS   UCSR0A
	 #define UART0_CONTROL  UCSR0B
	 #define UART0_DATA     UDR0
	 #define UART0_UDRIE    UDRIE0
/***TYPE 16***/
#elif defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644P__)
	/* ATmega with two USART */
	 #define ATMEGA_USART0
	 #define ATMEGA_USART1
	 #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
	 #define UART1_RECEIVE_INTERRUPT   USART0_UDRE_vect
	 #define UART0_TRANSMIT_INTERRUPT  USART1_RX_vect
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
/***TYPE 17***/
 #error "no UART definition for MCU available"
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
#if defined( ATMEGA_USART1 )
	static volatile unsigned char UART1_TxBuf[UART_TX_BUFFER_SIZE];
	static volatile unsigned char UART1_RxBuf[UART_RX_BUFFER_SIZE];
	static volatile unsigned char UART1_TxHead;
	static volatile unsigned char UART1_TxTail;
	static volatile unsigned char UART1_RxHead;
	static volatile unsigned char UART1_RxTail;
	static volatile unsigned char UART1_LastRxError;
#endif
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
	/***Pre-Processor Case 1***/
	#if defined( AT90_UART )
		/* set baud rate */
		UBRR = (unsigned char)baudrate; 
		/* enable UART receiver and transmmitter and receive complete interrupt */
		UART0_CONTROL = _BV(RXCIE)|_BV(RXEN)|_BV(TXEN);
		uart.FDbits=8;
		uart.Stopbits=1;
		uart.Parity=0;
	/***Pre-Processor Case 2***/
	#elif defined (ATMEGA_USART)
		/* Set baud rate */
		if ( baudrate & 0x8000 )
		{
    		UART0_STATUS = (1<<U2X);  //Enable 2x speed 
    		baudrate &= ~0x8000;
		}
		UBRRH = (unsigned char)(baudrate>>8);
		UBRRL = (unsigned char) baudrate;
		/* Enable USART receiver and transmitter and receive complete interrupt */
		UART0_CONTROL = _BV(RXCIE)|(1<<RXEN)|(1<<TXEN);
		/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
		#ifdef URSEL
			UCSRC = (1<<URSEL)|(3<<UCSZ0);
			uart.FDbits=8;
			uart.Stopbits=1;
			uart.Parity=0;
		#else
			switch(FDbits){
				case 9:
					UART0_CONTROL |= (1<<UCSZ2);
					UCSRC |= (3<<UCSZ0);
					uart.FDbits=9;
					break;
				case 8:
					UART0_CONTROL &= ~(1<<UCSZ2);
					UCSRC |= (3<<UCSZ00);
					uart.FDbits=8;
					break;
				case 7:	
					UART0_CONTROL &= ~(1<<UCSZ2);
					UCSRC |= (1<<UCSZ1);
					UCSRC &= ~(1<<UCSZ0);
					uart.FDbits=7;
					break;
				case 6:	
					UART0_CONTROL &= ~(1<<UCSZ2);
					UCSRC &= ~(1<<UCSZ1);
					UCSRC |= (1<<UCSZ0);
					uart.FDbits=6;
					break;
				case 5:	
					UART0_CONTROL &= ~(1<<UCSZ2);
					UCSRC &= ~(3<<UCSZ0);
					uart.FDbits=5;
					break;
				default:
					UART0_CONTROL &= ~(1<<UCSZ2);
					UCSRC |= (3<<UCSZ0);
					uart.FDbits=8;
					break;
			}
			switch(Stopbits){
				case 1:
					UCSRC &= ~(1<<USBS);
					uart.Stopbits=1;
					break;
				case 2:
					UCSRC |= (1<<USBS);
					uart.Stopbits=2;
					break;	
				default:
					UCSRC &= ~(1<<USBS);
					uart.Stopbits=1;
					break;
			}
			switch(Parity){
				case 0:
					UCSRC &= ~(3<<UPM0);
					uart.Parity=0;
					break;
				case 2:
					UCSRC |= (1<<UPM1);
					UCSRC &= ~(1<<UPM0);
					uart.Parity=2;
					break;
				case 3:
					UCSRC |= (3<<UPM0);
					uart.Parity=3;
					break;	
				default:
					UCSRC &= ~(3<<UPM0);
					uart.Parity=0;
					break;
			}
		#endif 
	/***Pre-Processor Case 3***/
	#elif defined (ATMEGA_USART0 )
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
	/***Pre-Processor Case 4***/
	#elif defined ( ATMEGA_UART )
		/* set baud rate */
		if ( baudrate & 0x8000 ) 
		{
    		UART0_STATUS = (1<<U2X);  //Enable 2x speed 
    		baudrate &= ~0x8000;
		}
		UBRRHI = (unsigned char)(baudrate>>8);
		UBRR   = (unsigned char) baudrate;
		/* Enable UART receiver and transmitter and receive complete interrupt */
		UART0_CONTROL = _BV(RXCIE)|(1<<RXEN)|(1<<TXEN);
		uart.FDbits=8;
		uart.Stopbits=1;
		uart.Parity=0;
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
#if defined( AT90_UART )
    UART_LastRxError = (usr & (_BV(FE)|_BV(DOR)) );
#elif defined( ATMEGA_USART )
    UART_LastRxError = (usr & (_BV(FE)|_BV(DOR)) );
#elif defined( ATMEGA_USART0 )
    UART_LastRxError = (usr & (_BV(FE0)|_BV(DOR0)) );
#elif defined ( ATMEGA_UART )
    UART_LastRxError = (usr & (_BV(FE)|_BV(DOR)) );
#endif
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
#if defined( ATMEGA_USART1 )
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
#endif
/***EOF***/
