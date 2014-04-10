/*
 * timer.h
 *
 * Created: 09-04-2014 14:30:00
 *  Author: SERGIO
 */
#ifndef TIMER_H_
  #define TIMER_H_
#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
	#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif
/*
** constants and macros
*/
/*
** global variables
*/
unsigned int TIMER0_COMPARE_MATCH;
unsigned int TIMER0_OVERFLOW;
/*
** global prototypes
*/
struct TIMER0{
	unsigned char wavegenmode;
	unsigned char compoutmode;
	unsigned int prescaler;
	unsigned char compare;
	unsigned char interrupt;
	unsigned char state;
	
	// prototype pointers
	
	void (*start)(struct TIMER0* timer0, unsigned int prescaler);
	uint8_t (*cmpm)(struct TIMER0* timer0, unsigned int multiplier);
	uint8_t (*ovfm)(struct TIMER0* timer0, unsigned int multiplier);
	void (*stop)(struct TIMER0* timer0);
	
};
typedef struct TIMER0 TIMER0;
/*
** global object function header
*/
TIMER0 TIMER0enable(unsigned char wavegenmode, unsigned char compoutmod, unsigned char compare, unsigned char interrupt);
/*
** global function header
*/
#endif /* TIMER_H_ */
/***EOF***/
