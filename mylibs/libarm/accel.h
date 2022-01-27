//Author: Javier Arin
//Description: Header file for accel sensor related stuff

#ifndef ACCEL_H
#define ACCEL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2C1_DIR "/dev/i2c-1"
#define I2C2_DIR "/dev/i2c-2"

#define I2C1 1
#define I2C2 2

#define ACCEL_ADDR 0x53
#define POWER_CTL 0x2D
#define MEASUREMENT_CONTINOUS 0x08
#define DATA 0x32

void setup_i2c_pins(int i2cpin){
    if(i2cpin == I2C2) 
        system("config-pin P9_20 i2c && config-pin P9_19 i2c");
    else if(i2cpin == I2C1)
        system("config-pin P9_18 i2c && config-pin P9_17 i2c");
}

int I2CRead(int file, char slaveAddr, int numBytes, unsigned char* rxbuf){
    rxbuf[0] = slaveAddr;

    //Send what you want to read
    if(write(file, rxbuf, 1) != 1) 
        return -1;
    //Read the actual value
    if(read(file, rxbuf, numBytes) != numBytes)
        return -2;
    return numBytes;
}

int startup_accel(int i2cpin){
    int file=0;  
    unsigned char txBuf[2]= {POWER_CTL, MEASUREMENT_CONTINOUS};
    char filename[128];

    if(i2cpin == I2C1) strcpy(filename, I2C1_DIR);
    else if(i2cpin == I2C2)strcpy(filename, I2C2_DIR);
    else return -10;

    if ((file = open(filename, O_RDWR )) < 0) {
        perror("Failed to open the i2c bus");       
        return -1;
    }  

    //printf("I2C‐ADXL345 App\nI2c:'%s' Abierto\n",filename);

    if (ioctl(file, I2C_SLAVE, ACCEL_ADDR) < 0) {
        printf("Failed to acquire bus access\n");  
        return -1;
    }

    if(write(file, txBuf, 2) != 2){
        perror("Failed to write to bus");
        return -1;
    }
    return 0;
}

int read_accel_data(int i2cpin, float* res){
    int file=0;  
    unsigned char rxBuf[6];
    char filename[128];

    if(i2cpin == I2C1) strcpy(filename, I2C1_DIR);
    else if(i2cpin == I2C2)strcpy(filename, I2C2_DIR);
    else return -10;

    if ((file = open(filename, O_RDWR )) < 0) {
        perror("Failed to open the i2c bus");       
        return -1;
    }  

    //printf("I2C‐ADXL345 App\nI2c:'%s' Abierto\n",filename);

    if (ioctl(file, I2C_SLAVE, ACCEL_ADDR) < 0) {
        printf("Failed to acquire bus access\n");  
        return -1;
    }

    int len=I2CRead(file, DATA, 6, rxBuf); //Reg 0x0=ID adxl345
    if (len<0){
        printf("Error en I2c\n");
    }
    //Conversion
    char xval[2], yval[2], zval[2];
    xval[0] = rxBuf[0];
    xval[1] = rxBuf[1];
    yval[0] = rxBuf[2];
    yval[1] = rxBuf[3];
    zval[0] = rxBuf[4];
    zval[1] = rxBuf[5];

    int xvalue = *(short*)xval;
    int yvalue = *(short*)yval;
    int zvalue = *(short*)zval;

    res[0] = xvalue/255.0;
    res[1] = yvalue/255.0;
    res[2] = zvalue/255.0;

    close(file);
    return 0;
}

int read_accel_id(int i2cpin){   
    int file=0;  
    unsigned char rxBuf[128];
    char filename[128];

    if(i2cpin == I2C1) strcpy(filename, I2C1_DIR);
    else if(i2cpin == I2C2)strcpy(filename, I2C2_DIR);
    else return -10;

    if ((file = open(filename, O_RDWR )) < 0) {
        perror("Failed to open the i2c bus");       
        return -1;
    }  

    //printf("I2C‐ADXL345 App\nI2c:'%s' Abierto\n",filename);

    if (ioctl(file, I2C_SLAVE, ACCEL_ADDR) < 0) {
        printf("Failed to acquire bus access\n");  
        return -1;
    }
    int len=I2CRead(file,0x0, 1, rxBuf); //Reg 0x0=ID adxl345
    if (len>0){
        //printf("ID Dev=0x%x\n",rxBuf[0]); //ID=0xe5
    }else{
        printf("Error en I2c\n");
    }
    close(file);
    return (int)rxBuf[0];
}


#endif
