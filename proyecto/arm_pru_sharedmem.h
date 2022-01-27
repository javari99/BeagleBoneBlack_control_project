

/**
 * @author  Javier Arin
 * @date    30/11/2021
 * @brief   PRU-ARM memory sharing program, ARM code
 *
 * arm_pru_sharedmem.c
 */
//TODO: make this into a thread!
#ifndef ARM_PRU_SHAREDMEM_H
#define ARM_PRU_SHAREDMEM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/mman.h>
#include <math.h>

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>
#include <iostream>

#include "./pru_1/arm_pru_common.h"
#include "sync.h"

#define MAX_BUFFER_SIZE     512
char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES    50
#define DEVICE_NAME     "/dev/rpmsg_pru31"

#define MILISECOND 1000
#define DELAY 500*MILISECOND

int run_shared_memory(void)
{
    std::cout << std::flush;
    system("config-pin P8_44 pruout && config-pin P8_41 pruout && config-pin P8_42 pruout");
    system("config-pin P9_31 pruout && config-pin P9_29 pruout && config-pin P9_30 pruout && config-pin P9_28 pruout && config-pin P9_27 pruin && config-pin P9_25 pruin");
    std::cout << std::flush;
    std::cout << "starting pru thread..." << std::endl;

    struct pollfd pollfds[1];
    int i, n, fd;
    int result = 0;

    //Shared memory location pointers
    void* pru;
    void* pru1_sharedmem;
    //
    DataInput* pru1_data_input;
    Stats* pru1_stats_out;

    /* Open the rpmsg_pru character device file */
    pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

    /*
     * If the RPMsg channel doesn't exist yet the character device
     * won't either.
     * Make sure the PRU firmware is loaded and that the rpmsg_pru
     * module is inserted.
     */
    if (pollfds[0].fd < 0) {
        printf("Failed to open %s\n", DEVICE_NAME);
        return -1;
    }

    /* Map the PRU memory */
    fd = open ("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        printf ("ERROR: could not open /dev/mem.\n\n");
        return 1;
    }
    pru = mmap (0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
    if (pru == MAP_FAILED) {
        printf ("ERROR: could not map memory.\n\n");
        return 1;
    }
    close(fd);
    printf ("Using /dev/mem.\n");

    //Place the pointer on the start of the shared memory area
    pru1_sharedmem = pru + PRU_SHAREDMEM + PRU_SHAREDMEM_DATA_FROM_ARM;
    pru1_data_input = (DataInput*)(pru1_sharedmem);
    pru1_stats_out = (Stats*)(pru1_sharedmem + sizeof(DataInput));

    /* The RPMsg channel exists and the character device is opened */
    //printf("Opened %s, sending %d messages\n\n", DEVICE_NAME, NUM_MESSAGES);
    printf("Putting data on address 0x%08x\n\n", pru1_sharedmem);

    while(shouldRun) {
        DataInput data;
        bool dataReady = false;
        //Prevent uart queue data races
        //The mute will be locked just inside this scope, as long as we are getting data from it
        {
            const std::lock_guard<std::mutex> lock(uartQueueMutex);
            //Load data and remove it from the queue
            if(!uartQueue.empty()){
                data.x = uartQueue.front().x;
                data.y = uartQueue.front().y;
                data.z = uartQueue.front().z;
                data.light = uartQueue.front().light;
                data.temp = uartQueue.front().temp;
                dataReady = true;
                uartQueue.pop();
            }
        }
        if(!dataReady){
            //Let it sleep not to hardlock the queue
            std::this_thread::sleep_for(100ms);
            continue;
        }

        /* Generate the data of the shared memory and store it*/
        pru1_data_input->x = data.x;
        pru1_data_input->y = data.y;
        pru1_data_input->z = data.z;
        pru1_data_input->light = data.light;
        pru1_data_input->temp = data.temp;

        /* Send 'Data' to the PRU through the RPMsg channel so it triggers the interrupt*/
        result = write(pollfds[0].fd, "Data", 5);
        /*
        if (result > 0)
            printf("Message: Sent to PRU\n");
        */
        /* Poll until we receive a message from the PRU and then print it */
        result = read(pollfds[0].fd, readBuf, MAX_BUFFER_SIZE);
        
        if (result > 0){
            /* Read the data stored in the shared memory*/
            //printf("Estadisticas:\n%d\n%d\n%d\n%d\n%d\n\n", pru1_stats_out->x_mean, pru1_stats_out->y_mean, pru1_stats_out->z_mean, pru1_stats_out->temp_mean, pru1_stats_out->light_mean);
            g_x_mean = pru1_stats_out->x_mean;
            g_x_variance = pru1_stats_out->x_variance;
            g_y_mean = pru1_stats_out->y_mean;
            g_y_variance = pru1_stats_out->y_variance;
            g_z_mean = pru1_stats_out->z_mean;
            g_z_variance = pru1_stats_out->z_variance;
            g_light_mean = pru1_stats_out->light_mean;
            g_light_variance = pru1_stats_out->light_variance;
            g_temp_mean = pru1_stats_out->temp_mean;
            g_temp_variance = pru1_stats_out->temp_variance;

            //std::cout << "Mode from pru: " << pru1_stats_out->power_mode << std::endl;
            // If we were in emergency mode but the emergency has passed, return to potentiometer mode
            if(powerMode == EMERGENCIA && pru1_stats_out->power_mode != EMERGENCIA){
                powerMode = SERVER;
                powerLevel = 0;
                continue;
            }
            // If emergency mode, set powermode to emergency
            if(pru1_stats_out->power_mode == EMERGENCIA){
                powerMode = EMERGENCIA;
            //if the PRU0 states a mode put it, otherwise it will be 0 so the system will decide
            // which mode it is in
            }else if(pru1_stats_out->power_mode != 0){
                powerMode = pru1_stats_out->power_mode;
            }
        }
    }

    /* Received all the messages the example is complete */
    //printf("Received %d messages, closing %s\n", NUM_MESSAGES, DEVICE_NAME);

    if(munmap(pru, PRU_LEN)) {
        printf("munmap failed\n");
    } else {
        printf("munmap succeeded\n");
    }

    /* Close the rpmsg_pru character device file */
    close(pollfds[0].fd);

    return 0;
}

#endif
