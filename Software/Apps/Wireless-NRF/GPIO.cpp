/* 
 * File:   GPIO.cpp
 * Author: alistair
 * 
 * Created on 26 January 2015, 14:34
 */
#include <stdio.h>

#include "GPIO.h"

GPIO::GPIO() {
}

GPIO::~GPIO() {
}

void GPIO::open(int port, int DDR) {
    FILE *file;

    //make sure the port is acessible to the OS.
    file = fopen("/sys/class/gpio/export", "w");
    fprintf(file, "%d\n", port);
    fclose(file);

    //write the DDR
    const char* ddrPath = "/sys/class/gpio/gpio%d/direction";
    char* ddrFinalPath = new char[50];
    sprintf(ddrFinalPath, ddrPath, port);
    file = fopen(ddrFinalPath, "w");
    if (DDR == 0) fprintf(file, "in\n");
    else fprintf(file, "out\n");
    fclose(file);

}

void GPIO::close(int port) {
    //remove the port from the OS Layer.
    FILE *file;
    file = fopen("/sys/class/gpio/unexport", "w");
    fprintf(file, "%d\n", port);
    fclose(file);
}

int GPIO::read(int port) {
    //open the GPIO value as a file.
    FILE *file;
    const char* filePath = "/sys/class/gpio/gpio%d/value";
    char fileP[128];
    sprintf(fileP, filePath, port);
    file = fopen(fileP, "r");
//read the input and return it.
    int i;
    fscanf(file, "%d", &i);
    fclose(file);
    return i;
}

void GPIO::write(int port, int value) {
    //open the GPIO as a file for write access
    FILE *file;
    const char* filePath = "/sys/class/gpio/gpio%d/value";
    char fileP[128];
    sprintf(fileP, filePath, port);
    file = fopen(fileP, "w");

    //write the value
    if (value == 0) {
        fprintf(file, "0\n");
    } else
        fprintf(file, "1\n");
    fclose(file);
}
