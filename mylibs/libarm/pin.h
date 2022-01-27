//Author: Javier Arin
//Description: Librería para controlar los GPIO

#ifndef PIN_H
#define PIN_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define SYSFS_GPIO_DIR "/sys/class/gpio"

#define DIRECTION_OUT 1
#define DIRECTION_IN 0

#define PIN_ON 1
#define PIN_OFF 0

#define IN_BUF_SIZE 128

void set_pin_direction(int pin, int direction){
    int fd;
    char buf[IN_BUF_SIZE];

    sprintf(buf, SYSFS_GPIO_DIR"/gpio%d/direction", pin);
    fd = open(buf, O_WRONLY);
    if(direction == DIRECTION_IN)
        write(fd, "in", 2);
    else if(direction == DIRECTION_OUT)
        write(fd, "out", 3);

    close(fd);
}

void write_pin(int pin, int value){
    int fd;
    char buf[IN_BUF_SIZE];

    sprintf(buf, SYSFS_GPIO_DIR"/gpio%d/value", pin);
    fd = open(buf, O_WRONLY);
    if(value == PIN_ON ) write(fd, "1", 1);
    if(value == PIN_OFF) write(fd, "0", 1);

    close(fd);
}

int read_pin(int pin, char* buff, int buffSize){
    int fd;
    char buf[IN_BUF_SIZE];
    
    sprintf(buf, SYSFS_GPIO_DIR"/gpio%d/value", pin);
    fd = open(buf, O_RDONLY);
    int len = read(fd, buff, buffSize);
    
    close(fd);
    return len;
}

int read_pin_as_int(int pin){
    char buf[IN_BUF_SIZE];
    int len = read_pin(pin, buf, IN_BUF_SIZE);
    return atoi(buf);
}

#endif
