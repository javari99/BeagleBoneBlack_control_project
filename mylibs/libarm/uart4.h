//Autor: Javier Arin
/*Descripción:
    Se trata de una lirería que trata de implementar
    una instancia de UART4. Solo puede existir de forma
    global una instancia UART4 así que como mucho debería
    ser un singleton, pero con hacer solo una instancia y
    no más basta.*/

#ifndef UART4_H
#define UART4_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>

struct uart4_instance{
    int fd;
    struct termios term;
};

void setup_uart4(struct uart4_instance* inst){
    system("config-pin P9_11 uart && config-pin P9_13 uart");

    if((inst->fd = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY)) < 0){
        perror("UART: Failed to open UART4.\n");
    }

    tcgetattr(inst->fd, &(inst->term));
    inst->term.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
    inst->term.c_iflag = IGNPAR | ICRNL;
    tcsetattr(inst->fd, TCSANOW, &(inst->term));
}

void terminate_uart4(struct uart4_instance* inst){
    close(inst->fd);
    inst->fd = -1;

}

//Non Blocking function
int read_uart4(struct uart4_instance* inst, char* buf, int bufSize){
    return read(inst->fd, buf, bufSize);
}

int write_uart4(struct uart4_instance* inst, const char* buf, int bufSize){
    return write(inst->fd, buf, bufSize);
}




#endif
