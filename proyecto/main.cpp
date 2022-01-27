//USES C++ 14 check __cuplusplus for the supported gcc version

#include "../mylibs/libarm/uart4.h"

#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <sstream>
#include <string>

#include "sync.h"
#include "utils.h"
#include "uart_process.h"
#include "web_server_process.h"
#include "led_control.h"
#include "arm_pru_sharedmem.h"


//Declare all the extern variables so the main can link against them
std::atomic<int16_t> g_x_val;
std::atomic<int16_t> g_y_val;
std::atomic<int16_t> g_z_val;
std::atomic<int16_t> g_light_val;
std::atomic<int16_t> g_temp_val;
std::atomic<bool> shouldRun;
std::atomic<short> powerLevel;
std::atomic<short> powerMode;
std::mutex uartQueueMutex;
std::queue<DataInput> uartQueue;
std::atomic<int16_t> g_x_mean;
std::atomic<int16_t> g_x_variance;
std::atomic<int16_t> g_y_mean;
std::atomic<int16_t> g_y_variance;
std::atomic<int16_t> g_z_mean;
std::atomic<int16_t> g_z_variance;
std::atomic<int16_t> g_light_mean;
std::atomic<int16_t> g_light_variance;
std::atomic<int16_t> g_temp_mean;
std::atomic<int16_t> g_temp_variance;

int main(int argc, char* argv[]){
    //Check for port number
    if(argc < 2){
        std::cout << "ERROR: no port provided\n";
	    exit(1);
    }
    int portno = atoi(argv[1]);

    //This global variable controls the execution loop of the different worker threads
    shouldRun = true;

    //Launch threads
    std::cout << "Starting threads...." << std::endl;
    std::thread UartThread(UartProcess::run);
    std::thread LedControlThread(led_control_run);
    std::thread WebServerThread(WebServer::StartServer, portno);
    std::thread PruControllerThread(run_shared_memory);

    //Exit command 
    while(1){
        std::string userInput;
        std::cin >> userInput;
        if(userInput == "exit"){
            std::cout << "Exiting..." << std::endl;
            shouldRun = false;
            break;
        }
    }

    std::cout << "Joining threads..." << std::endl;
    UartThread.join();
    WebServerThread.join();
    LedControlThread.join();
    PruControllerThread.join();
    
    return 0;
}
