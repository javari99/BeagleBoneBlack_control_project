/**
 * pru_interrupt.h
 *
 *  @date       09/12/2021
 *  @author     Javier Arin
 *  @brief      This file contains all the code related to
 *              creating, configuring and using pru interrupts
 */


#ifndef PRU_INTERRUPT_H_
#define PRU_INTERRUPT_H_

/**
 * User changeable macros
 */
#define EVT 3
#define CHAN 7
#define HOST 1 //It can only be 0 or 1

// Global interrupt flags
#define ENABLE_GLOBAL_INTERRUPTS 1
#define DISABLE_GLOBAL_INTERRUPTS 0

// __R31 event generator bit masks
#define ENABLE_STROBE (1 << 5)

// Correctly defne SYS_INTERRUPT (evt + 16)
#if EVT == 0
    #define SYS_NTERRUPT 16
#elif EVT == 1
    #define SYS_NTERRUPT 17
#elif EVT == 2
    #define SYS_NTERRUPT 18
#elif EVT == 3
    #define SYS_NTERRUPT 19
#elif EVT == 4
    #define SYS_NTERRUPT 20
#elif EVT == 5
    #define SYS_NTERRUPT 21
#elif EVT == 6
    #define SYS_NTERRUPT 22
#elif EVT == 7
    #define SYS_NTERRUPT 23
#elif EVT == 8
    #define SYS_NTERRUPT 24
#elif EVT == 9
    #define SYS_NTERRUPT 25
#elif EVT == 10
    #define SYS_NTERRUPT 26
#elif EVT == 11
    #define SYS_NTERRUPT 27
#elif EVT == 12
    #define SYS_NTERRUPT 28
#elif EVT == 13
    #define SYS_NTERRUPT 29
#elif EVT == 14
    #define SYS_NTERRUPT 30
#elif EVT == 15
    #define SYS_NTERRUPT 31
#else
    #error Invalid EVT number! (0-15)
#endif


// Correctly clamp EVT value to generate the propper CMR register number
#if (SYS_NTERRUPT < 4)
    #define CMR_REG_NUM 0
#elif (SYS_NTERRUPT < 8)
    #define CMR_REG_NUM 1
#elif (SYS_NTERRUPT < 12)
    #define CMR_REG_NUM 2
#elif (SYS_NTERRUPT < 16)
    #define CMR_REG_NUM 3
#elif (SYS_NTERRUPT < 20)
    #define CMR_REG_NUM 4
#elif (SYS_NTERRUPT < 24)
    #define CMR_REG_NUM 5
#elif (SYS_NTERRUPT < 28)
    #define CMR_REG_NUM 6
#elif (SYS_NTERRUPT < 32)
    #define CMR_REG_NUM 7
#elif (SYS_NTERRUPT < 36)
    #define CMR_REG_NUM 8
#elif (SYS_NTERRUPT < 40)
    #define CMR_REG_NUM 9
#elif (SYS_NTERRUPT < 44)
    #define CMR_REG_NUM 10
#elif (SYS_NTERRUPT < 48)
    #define CMR_REG_NUM 11
#elif (SYS_NTERRUPT < 52)
    #define CMR_REG_NUM 12
#elif (SYS_NTERRUPT < 56)
    #define CMR_REG_NUM 13
#elif (SYS_NTERRUPT < 60)
    #define CMR_REG_NUM 14
#elif (SYS_NTERRUPT < 64)
    #define CMR_REG_NUM 15
#else
    #error Invalid EVT number! (0-15)
#endif

// Correctly clamp CHAN value to generate the propper HMR register number
#if (CHAN < 4)
    #define HMR_REG_NUM 0
#elif (CHAN < 8)
    #define HMR_REG_NUM 1
#elif (CHAN < 10)
    #define HMR_REG_NUM 2
#else
    #error Invalid CHAN number! (0-9)
#endif

#if (HOST == 0)
    #define HOST_INTERRUPT_REC_BIT (1 << 30)
#elif (HOST == 1)
    #define HOST_INTERRUPT_REC_BIT (1 << 31)
#else
    #error Invalid host number! (0-1)
#endif

// Preprocessor time variable name generation macro stacks
#define CMR_NAME3(cmr_reg, sys_int) \
    CT_INTC## . ##CMR##cmr_reg##_bit## . ##CH_MAP_##sys_int
#define CMR_NAME2(cmr_reg, sys_int) \
    CMR_NAME3(cmr_reg, sys_int)
#define CMR_NAME(cmr_reg, sys_int) \
    CMR_NAME2(cmr_reg, sys_int)

#define HMR_NAME3(hmr_reg, chan) \
    CT_INTC## . ##HMR##hmr_reg##_bit## . ##HINT_MAP_##chan
#define HMR_NAME2(hmr_reg, chan) \
    HMR_NAME3(hmr_reg, chan)
#define HMR_NAME(hmr_reg, chan) \
    HMR_NAME2(hmr_reg, chan)


#endif /* PRU_INTERRUPT_H_ */
