//Author: Javier Arin
//Description: Header file for temperature sensor related stuff

#ifndef TEMP_H
#define TEMP_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "accel.h"

#define TEMP_ADDR 0x49
#define TEMP_DATA 0x00

float read_temp_data(int i2cpin){
    int file=0;  
    unsigned char rxBuf[2];
    char filename[128];

    if(i2cpin == I2C1) strcpy(filename, I2C1_DIR);
    else if(i2cpin == I2C2)strcpy(filename, I2C2_DIR);
    else return -999;

    if ((file = open(filename, O_RDWR )) < 0) {
        perror("Failed to open the i2c bus");       
        return -999;
    }  

    //printf("I2C‐ADXL345 App\nI2c:'%s' Abierto\n",filename);

    if (ioctl(file, I2C_SLAVE, TEMP_ADDR) < 0) {
        printf("Failed to acquire bus access\n");  
        return -999;
    }

    int len=I2CRead(file,TEMP_DATA, 2, rxBuf);
    if(len <= 0) return -999;
    int temp = (rxBuf[0] << 8) + rxBuf[1];
    float temp2 = (temp >> 5)*0.125;
    return temp2;
}

#endif
