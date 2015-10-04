#include "Timing.h"
#include <avr/interrupt.h>




volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;


unsigned long Timing::millis()
{
	unsigned long m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer0_millis;
	SREG = oldSREG;

	return m;
}

/* Time/Counter0 Overflow */
ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;

}
void Timing::init()
{
	//init interrupts and set the timer going.
	sei();
	
	TCCR0A|= (1<< WGM01);
	TCCR0A|= (1<< WGM00);
	/*set clock select to clk/64 */
	TCCR0B |= (1<< CS01);
	TCCR0B |= (1<< CS00);

	/* enable interrupt on overflow.*/
	TIMSK0  |=(1<<TOIE0);
	
	
	
}