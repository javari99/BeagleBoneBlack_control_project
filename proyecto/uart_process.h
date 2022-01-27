#ifndef UARTPROCESS_H
#define UARTPROCESS_H


#include "../mylibs/libarm/uart4.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include "sync.h"

namespace UartProcess{

uart4_instance uart4;
int crc(std::string str){
    int res = 0;
    for(char c : str){
        res += c;
    }
    return res;
}

void run(){

    g_x_val = 0;
    g_y_val = 0;
    g_z_val = 0;
    g_light_val = 0;
    g_temp_val = 0;

    char rxBuf[128];

    setup_uart4(&uart4);

    while(shouldRun){
        int num = read_uart4(&uart4, rxBuf, 128);
        if(num > 0) {
            //std::cout << "Received: "<< rxBuf << std::endl;
            //std::cout << rxBuf << " length: " << strlen(rxBuf) <<std::endl;
            if(rxBuf[0] == '\n' || rxBuf[0] == 'r'){
                continue;
            }
            //TODO: Checksum, if error repeat message
            char delim = '&';
            std::string str(rxBuf);

            std::vector<std::string> splittedList = Utils::splitString(str, delim);
            //std::cout << "x: "<<splittedList[0] << " y: " << splittedList[1] << " z: " << splittedList[2] << " temp: " << splittedList[3] << " luz: " << splittedList[4] << " checksum: " << splittedList[5] << std::endl;

            bool err = false;
            int calculatedchecksum = 4 * '&' +  crc(splittedList[0]) + crc(splittedList[1]) + crc(splittedList[2]) + crc(splittedList[3]) + crc(splittedList[4]);
            //std::cout << "Calculated checksum: " << calculatedchecksum << std::endl;
            if(calculatedchecksum != atoi(splittedList[5].c_str())) err=true;

            if(err){
               //Send repeat message with uart
               std::cout << "Error on msg checksum, sending repeat message.." << std::endl;
               write_uart4(&uart4, "r", 2);
               continue;
            }

            DataInput receivedData;
            receivedData.x = atoi(splittedList[0].c_str());
            receivedData.y = atoi(splittedList[1].c_str());
            receivedData.z = atoi(splittedList[2].c_str());
            receivedData.temp = atoi(splittedList[3].c_str());
            receivedData.light = atoi(splittedList[4].c_str());

            g_x_val = receivedData.x;
            g_y_val = receivedData.y;
            g_z_val = receivedData.z;
            g_light_val = receivedData.light;
            g_temp_val = receivedData.temp;

            {
                const std::lock_guard<std::mutex> lock(uartQueueMutex);
                uartQueue.push(receivedData);
            }
            /*
            for(std::string str : splittedList){
                str.push_back('\n');
                write_uart4(&uart4, str.c_str(), str.length());
            }
            */

        }
        memset(rxBuf, '\0', 128);
    }
    return;
}
};


#endif
