//Author: Javier Arin
//Description: Librería para controlar el ADC de la BBK

#ifndef ANALOG_H
#define ANALOG_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

int readAnalog(int number){
    int fd;
    const int buffSize = 128;
    char buf[buffSize];

    sprintf(buf, LDR_PATH"%d_raw", number);
    fd = open(buf, O_RDONLY);
    int len = read(fd, buf, buffSize);
    close(fd);

    return atoi(buf);
}

#endif