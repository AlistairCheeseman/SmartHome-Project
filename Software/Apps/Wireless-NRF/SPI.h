/* 
 * File:   SPI.h
 * Author: alistair
 *
 * Created on 26 January 2015, 13:50
 * The SPI Class handles the Hardware calls to the kernel for SPI Access.
 */

#ifndef SPI_H
#define	SPI_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>


class SPI {
public:
    SPI(char *spidev, int bitlength, int speed);
    uint8_t transfer(uint8_t tx_);
    virtual ~SPI();
private:
    char* spidev;
    uint8_t mode;
    uint8_t bits;
    uint32_t speed;
    int fd;

    void init();

};

#endif	/* SPI_H */

