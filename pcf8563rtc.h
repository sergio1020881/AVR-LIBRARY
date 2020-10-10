/***************************************************************************************************
                                   ExploreEmbedded	
 ***************************************************************************************************
 * File:   pcf8563rtc.h
 * Version: 15.0
 * Author: ExploreEmbedded
 * Website: http://www.exploreembedded.com/wiki
 * Description: Contains the function prototypes for RTC-Ds1307 date and time read write

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
 **************************************************************************************************/
/***************************************************************************************************
                             Revision History
****************************************************************************************************
15.0: Initial version 
***************************************************************************************************/
#ifndef _PCF8563RTC_H_
	#define _PCF8563RTC_H_
/***************************************************************************************************
                             Commonly used Ds1307 macros/Constants
***************************************************************************************************
  Below values are fixed and should not be changed. Refer Ds1307 DataSheet for more info  ********/

#define PCF8563ReadMode_U8   0xA3u  // PCF8563 ID
#define PCF8563WriteMode_U8  0xA2u  // PCF8563 ID
#define PCF8563SecondRegAddress_U8   0x02u   // Address to access PC8563 SEC register
#define PCF8563DateRegAddress_U8     0x05u   // Address to access PC8563 DATE register
#define PCF8563ControlRegAddress_U8  0x0Du   // Address to access PC8563 CONTROL register
//
#define C_Ds1307ReadMode_U8   0xD1u  // DS1307 ID
#define C_Ds1307WriteMode_U8  0xD0u  // DS1307 ID
#define C_Ds1307SecondRegAddress_U8   0x00u   // Address to access Ds1307 SEC register
#define C_Ds1307DateRegAddress_U8     0x04u   // Address to access Ds1307 DATE register
#define C_Ds1307ControlRegAddress_U8  0x07u   // Address to access Ds1307 CONTROL register
/**************************************************************************************************/
struct date{
	uint8_t years;
	uint8_t century_months;
	uint8_t weekdays;
	uint8_t days;	
};
struct time{
	uint8_t hours;
	uint8_t minutes;
	uint8_t VL_seconds;
};
struct alarm{
	uint8_t minute_alarm;
	uint8_t	hour_alarm;
	uint8_t day_alarm;
	uint8_t weekday_alarm;
	
};
/***************************************************************************************************
                             Function Prototypes
***************************************************************************************************/
void PCF8563RTC_Init(uint8_t prescaler);
void PCF8563RTC_SetTime(uint8_t var_hour_u8, uint8_t var_min_u8, uint8_t var_sec_u8);
void PCF8563RTC_SetDate(uint8_t var_day_u8, uint8_t var_month_u8, uint8_t var_year_u8);
struct time PCF8563RTC_GetTime(void);
struct date PCF8563RTC_GetDate(void);
uint8_t PCF8563RTC_bcd2dec(uint8_t num);
/**************************************************************************************************/
#endif
/*EOF*/
