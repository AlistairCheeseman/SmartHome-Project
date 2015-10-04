/* 
* SPI.h
*
* Created: 28/02/2015 08:41:39
* Author: Ali
*/


#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>
#include "Timing.h"
#include <avr/io.h>

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06
//#define SPI_CLOCK_DIV64 0x07

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR


#define MSBFIRST 0x00
#define LSBFIRST 0x01

namespace SPI
{
	//send data
	uint8_t transfer(uint8_t tx_);
	//configure hardware spi
void init();
//configure subs
void setBitOrder(uint8_t bitOrder);
void setDataMode(uint8_t mode);
void setClockDivider(uint8_t);
//disable and stop spi functions
void stop();
}; //SPI

#endif //__SPI_H__
