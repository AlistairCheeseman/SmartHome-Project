/*
 * Timing.h
 *
 * Created: 28/02/2015 11:35:56
 *  Author: Ali
 */ 


#ifndef TIMING_H_
#define TIMING_H_


#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)
#include <util/delay.h>
#define delay(x) _delay_ms(x)
#define delayMicroseconds(x) _delay_us(x)

namespace Timing
{
unsigned long millis();
void init(void);

	
};



#endif /* TIMING_H_ */