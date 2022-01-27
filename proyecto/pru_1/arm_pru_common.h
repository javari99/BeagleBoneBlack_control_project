

/**
 * @author  Javier Arin
 * @date    30/11/2021
 * @brief   PRU-ARM memory sharing headers
 *
 * arm_pru_common.h
 */

#ifndef ARM_PRU_COMMON_H
#define ARM_PRU_COMMON_H

#include <stdint.h>

#define PRU_ADDR        0x4A300000      // Start of PRU memory Page 185 am335x TRM
#define PRU_LEN         0x80000         // Length of PRU memory
#define PRU0_DRAM       0x00000         // Offset to PRU0 Data RAM
#define PRU1_DRAM       0x02000         // Offset to PRU1 Data RAM
#define PRU_SHAREDMEM   0x10000         // Offset to PRU shared memory
#define PRU_SHAREDMEM_DATA_FROM_ARM   0x01000  // Offset inside the PRU shared memory

struct DataInput {
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t light;
    int16_t temp;
};
typedef struct DataInput DataInput;

struct Stats {
    int16_t x_mean;
    int16_t x_variance;
    int16_t y_mean;
    int16_t y_variance;
    int16_t z_mean;
    int16_t z_variance;
    int16_t temp_mean;
    int16_t temp_variance;
    int16_t light_mean;
    int16_t light_variance;
    uint8_t power_mode;
};
typedef struct Stats Stats;

#define XIN_REGISTER 20
#define XIN_DEVICE_ID 10

typedef struct DirectMsg {
    uint32_t pressed_button_option;
}DirectMsg;

#define BUTTON1_PRESSED 1
#define BUTTON2_PRESSED 2
//#define BUTTON_BOTH_PRESSED 3
#define NO_BUTTON_PRESSED 0

#endif
