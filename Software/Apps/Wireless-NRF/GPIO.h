/* 
 * File:   GPIO.h
 * Author: alistair
 *
 * Created on 26 January 2015, 14:34
 *  The GPIO Class handles the Hardware calls to the kernel for GPIO Access.
 */

#ifndef GPIO_H
#define	GPIO_H

class GPIO {
public:

#define GPIO_DDR_OUT 1
#define GPIO_DDR_IN 0
#define GPIO_VAL_HIGH 1
#define GPIO_VAL_LOW 0

    GPIO();
    virtual ~GPIO();
    static void open(int port, int DDR);
    static void close(int port);
    static int read(int port);
    static void write(int port, int value);
private:

};

#endif	/* GPIO_H */

