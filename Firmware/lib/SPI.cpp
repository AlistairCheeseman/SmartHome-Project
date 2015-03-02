/* 
* SPI.cpp
*
* Created: 28/02/2015 08:41:39
* Author: Ali
*/


#include "SPI.h"

void SPI::init()
{
		// Set MOSI, SCK,SS as output, MISO as input
		DDRB |= (1<<PB3)|(1<<PB5)|(1<<PB2);
		DDRB &= ~(1<<PB4); 
		
		// Latch Disable (RCK Low)
		// SPI_PORT &= ~(1<<SPI_CS);
		// Enable SPI, Master, set clock rate fck/2 (maximum)
		SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1); // this is here until xtal sorted.
		//	SPSR = (1<<SPI2X);
}
void SPI::stop()
{
	SPCR &= ~(1<<SPE);
}
unsigned char SPI::transfer(unsigned char dataout)
{
	unsigned char datain;
	// Start transmission (MOSI)
	//SPI_PORT |= (1<<SPI_CS);
	SPDR = dataout;
	// Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	// Get return Value;
	datain = SPDR;
	// Latch the Output using rising pulse to the RCK Pin
	//SPI_PORT |= (1<<SPI_CS);
//	_delay_us(1);             // Hold pulse for 1 micro second
	// Disable Latch
	//	SPI_PORT &= ~(1<<SPI_CS);
	// Return Serial In Value (MISO)
	//SPI_PORT |= (0<<SPI_CS);
	return datain;
}


void SPI::setBitOrder(uint8_t bitOrder)
{
	if(bitOrder == LSBFIRST) {
		SPCR |= _BV(DORD);
		} else {
		SPCR &= ~(_BV(DORD));
	}
}

void SPI::setDataMode(uint8_t mode)
{
	SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

void SPI::setClockDivider(uint8_t rate)
{
	SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
	SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}







