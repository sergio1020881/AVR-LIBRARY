/***************************************************************************************************
                                   ExploreEmbedded	
****************************************************************************************************
 * File:   i2c.c
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Contains the library routines for I2C read,write operation

The libraries have been tested on ExploreEmbedded development boards. We strongly believe that the 
library works on any of development boards for respective controllers. However, ExploreEmbedded 
disclaims any kind of hardware failure resulting out of usage of libraries, directly or indirectly.
Files may be subject to change without prior notice. The revision history contains the information 
related to updates. 

GNU GENERAL PUBLIC LICENSE: 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Errors and omissions should be reported to codelibraries@exploreembedded.com
***************************************************************************************************/
/***************************************************************************************************
                             Revision History
****************************************************************************************************
15.0: Initial version 
***************************************************************************************************/
#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
/***Library***/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include "i2c.h"
/***Define***/
#if defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
	#define ATMEGA_I2C
	#define I2C_SCL_CLOCK 100000UL
	#define TWI_BIT_RATE_REGISTER TWBR
	#define TWI_CONTROL_REGISTER TWCR
	#define TWI_STATUS_REGISTER TWSR
	#define TWI_STATUS_MASK 0B11111000
	#define TWI_DATA_REGISTER TWDR
	#define TWI_ADDRESS_REGISTER TWAR
	#define TWI_ADDRESS_REGISTER_MASK 0B11111110
	#define TWI_PRESCALER_MASK 0B00000011
#else
	#error "Not Atmega 128"
#endif
/***************************************************************************************************
                         void I2C_Init()
****************************************************************************************************
 * I/P Arguments: none.
 * Return value	: none
 * description :This function is used to initialize the I2C module
****************************************************************************************************/
void I2C_Init(uint8_t prescaler)
{
  //TWI_STATUS_REGISTER=0x00; //set presca1er bits to zero
  //TWI_BIT_RATE_REGISTER=0x46; //SCL frequency is 50K for 16Mhz
  //TWI_CONTROL_REGISTER=0x04; //enab1e TWI module
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
}
/***************************************************************************************************
                         void I2C_Start()
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none
 * description  :This function is used to generate I2C Start Condition.
                 Start Condition: SDA goes low when SCL is High.

                               ____________
                SCL:          |            |
                      ________|            |______
                           _________
                SDA:      |         |
                      ____|         |____________

***************************************************************************************************/
void I2C_Start()
{
  TWI_CONTROL_REGISTER = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
  while (!(TWI_CONTROL_REGISTER & (1<<TWINT)));
}
/***************************************************************************************************
                         void I2C_Stop()
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none
 * description  :This function is used to generate I2C Stop Condition.
                 Stop Condition: SDA goes High when SCL is High.

                               ____________
                SCL:          |            |
                      ________|            |______
                                 _________________
                SDA:            |
                      __________|

***************************************************************************************************/
void I2C_Stop(void)
{
  TWI_CONTROL_REGISTER = ((1<< TWINT) | (1<<TWEN) | (1<<TWSTO));
   _delay_us(100); //wait for a short time
}
/***************************************************************************************************
                         void I2C_Write(uint8_t var_i2cData_u8)
****************************************************************************************************
 * I/P Arguments: uint8_t-->8bit data to be sent.
 * Return value  : none
 * description  :This function is used to send a byte on SDA line using I2C protocol
                 8bit data is sent bit-by-bit on each clock cycle.
                 MSB(bit) is sent first and LSB(bit) is sent at last.
                 Data is sent when SCL is low.
         ___     ___     ___     ___     ___     ___     ___     ___     ___     ___
 SCL:   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
      __|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___

 SDA:    D8       D7     D6      D5      D4       D3      D2      D1      D0     ACK

***************************************************************************************************/
void I2C_Write(uint8_t var_i2cData_u8)
{
  TWI_DATA_REGISTER = var_i2cData_u8 ;
  TWI_CONTROL_REGISTER = ((1<< TWINT) | (1<<TWEN));
  while (!(TWI_CONTROL_REGISTER & (1 <<TWINT)));
}
/***************************************************************************************************
                         uint8_t I2C_Read(uint8_t var_ackOption_u8)
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : uint8_t(received byte)
 * description :This function is used to receive a byte on SDA line using I2C protocol.
               8bit data is received bit-by-bit each clock and finally packed into Byte.
               MSB(bit) is received first and LSB(bit) is received at last.

         ___     ___     ___     ___     ___     ___     ___     ___     ___     ___
SCL:    |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
      __|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |__

 SDA:    D8       D7     D6      D5       D4     D3       D2      D1     D0      ACK

***************************************************************************************************/
uint8_t I2C_Read(uint8_t var_ackOption_u8)
{
 TWI_CONTROL_REGISTER = ((1<< TWINT) | (1<<TWEN) | (var_ackOption_u8<<TWEA));
   while ( !(TWI_CONTROL_REGISTER & (1 <<TWINT)));
   return TWI_DATA_REGISTER;
}
/***************************************************************************************************
                         void I2C_Start()
****************************************************************************************************
 * I/P Arguments: void.
 * Return value  : uint8_t
 * description  : status
***************************************************************************************************/
uint8_t I2C_status(void){
	return TWI_STATUS_REGISTER & TWI_STATUS_MASK;
}
/*EOF*/
