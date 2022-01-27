#ifndef SYNC_H
#define SYNC_H

#include <stdint.h>
#include <atomic>
#include <mutex>
#include <queue>

#include "./pru_1/arm_pru_common.h"

//These are the same in the PRU
typedef struct Stats Stats;

extern std::atomic<bool> shouldRun;

extern std::atomic<short> powerLevel;
extern std::atomic<short> powerMode;

//UART message queue
extern std::mutex uartQueueMutex;
extern std::queue<DataInput> uartQueue;

extern std::atomic<int16_t> g_x_val;
extern std::atomic<int16_t> g_y_val;
extern std::atomic<int16_t> g_z_val;
extern std::atomic<int16_t> g_light_val;
extern std::atomic<int16_t> g_temp_val;

extern std::atomic<int16_t> g_x_mean;
extern std::atomic<int16_t> g_x_variance;
extern std::atomic<int16_t> g_y_mean;
extern std::atomic<int16_t> g_y_variance;
extern std::atomic<int16_t> g_z_mean;
extern std::atomic<int16_t> g_z_variance;
extern std::atomic<int16_t> g_light_mean;
extern std::atomic<int16_t> g_light_variance;
extern std::atomic<int16_t> g_temp_mean;
extern std::atomic<int16_t> g_temp_variance;

//Power mode macros
#define POTENCIOMETRO  1
#define SERVER 2
#define EMERGENCIA 3

//Led power macros
#define POWER_NULL 0
#define LEVEL1 1
#define LEVEL2 2
#define LEVEL3 3

#endif
