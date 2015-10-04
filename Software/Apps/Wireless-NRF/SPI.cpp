/* 
 * File:   SPI.cpp
 * Author: alistair
 * 
 * Created on 26 January 2015, 13:50
 * Inspiration taken from www.kernel.org/doc/Documentation/spi/spidev_test.c 
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


#include "SPI.h"


void pabort(const char * error) {
    perror(error);
    abort();
}

SPI::SPI(char *spidev, int bitlength, int speed) {
    //default settings should be 8 bits, /dev/spidev1.0, 16000000 (16MHz)
    this->spidev = spidev;
    this->bits = bitlength;
    this->speed = speed;

    this->init();
}

SPI::~SPI() {
    close(this->fd);
}

void SPI::init() {
    int ret = 0;

    this->fd = open(this->spidev, O_RDWR);
    if (fd < 0) {
        pabort("can't open device");
    }

    /*
     * spi mode
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");

   fprintf(stdout,"spi mode: 0x%x\n", mode);
 fprintf(stdout,"bits per word: %d\n", bits);
  fprintf(stdout,"max speed: %d Hz (%d KHz)\n", speed, speed / 1000);


}

uint8_t SPI::transfer(uint8_t tx_) {
    int ret;
    
    uint8_t tx[] = {0};
    uint8_t rx[ARRAY_SIZE(tx)] = {0};
    tx[0] = tx_;
    
    struct spi_ioc_transfer tr;
    
        tr.tx_buf = (unsigned long)tx;
        tr.rx_buf = (unsigned long) rx;
        tr.len =  ARRAY_SIZE(tx);
        tr.delay_usecs = 0;
        tr.speed_hz = this->speed;
        tr.bits_per_word = this->bits;
    
    ret = ioctl(this->fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        pabort("can't send spi message");
    }
    return(rx[0]);
}