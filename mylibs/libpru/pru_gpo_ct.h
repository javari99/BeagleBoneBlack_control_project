/**
 * pru_gpo_ct.h
 *
 *  @date       09/12/2021
 *  @author     Javier Arin
 *  @brief      This file contains all the GPO mask constants for
 *              PRU0 and PRU1 as well as definitions for their associate
 *              LEDs
 */

#ifndef PRU_GPO_CT_H_
#define PRU_GPO_CT_H_

/**
 * PRU0 GPO Constants
 */

//Define pin mask on the __R30 register for GPO of PRU0
#define PR1_PRU0_GPO0   (1 << 0)
#define PR1_PRU0_GPO1   (1 << 1)
#define PR1_PRU0_GPO2   (1 << 2)
#define PR1_PRU0_GPO3   (1 << 3)
#define PR1_PRU0_GPO4   (1 << 4)
#define PR1_PRU0_GPO5   (1 << 5)
#define PR1_PRU0_GPO6   (1 << 6)
#define PR1_PRU0_GPO7   (1 << 7)
#define PR1_PRU0_GPO8   (1 << 8)
#define PR1_PRU0_GPO9   (1 << 9)
#define PR1_PRU0_GPO10  (1 << 10)
#define PR1_PRU0_GPO11  (1 << 11)
#define PR1_PRU0_GPO12  (1 << 12)
#define PR1_PRU0_GPO13  (1 << 13)
#define PR1_PRU0_GPO14  (1 << 14)
#define PR1_PRU0_GPO15  (1 << 15)

//Associate LEDs to PRU0_GPO pins
#define PRU0_BLUE_LED       PR1_PRU0_GPO0
#define PRU0_GREEN_LED      PR1_PRU0_GPO1
#define PRU0_ORANGE_LED     PR1_PRU0_GPO2
#define PRU0_RED_LED        PR1_PRU0_GPO3
#define PRU0_ALL_LEDS       (PRU0_BLUE_LED | PRU0_ORANGE_LED | PRU0_GREEN_LED | PRU0_RED_LED)
//##############################################################################


/**
 * PRU1 GPO Constants
 */

//Define pin mask on the __R30 register for GPO of PRU0
#define PR1_PRU1_GPO0   (1 << 0)
#define PR1_PRU1_GPO1   (1 << 1)
#define PR1_PRU1_GPO2   (1 << 2)
#define PR1_PRU1_GPO3   (1 << 3)
#define PR1_PRU1_GPO4   (1 << 4)
#define PR1_PRU1_GPO5   (1 << 5)
#define PR1_PRU1_GPO6   (1 << 6)
#define PR1_PRU1_GPO7   (1 << 7)
#define PR1_PRU1_GPO8   (1 << 8)
#define PR1_PRU1_GPO9   (1 << 9)
#define PR1_PRU1_GPO10  (1 << 10)
#define PR1_PRU1_GPO11  (1 << 11)
#define PR1_PRU1_GPO12  (1 << 12)
#define PR1_PRU1_GPO13  (1 << 13)
#define PR1_PRU1_GPO14  (1 << 14)
#define PR1_PRU1_GPO15  (1 << 15)

//Associate LEDs to PRU0_GPO pins
#define PRU1_BLUE_LED       PR1_PRU1_GPO3
#define PRU1_GREEN_LED      PR1_PRU1_GPO4
#define PRU1_RED_LED        PR1_PRU1_GPO5
#define PRU1_ALL_LEDS       (PRU1_BLUE_LED | PRU1_GREEN_LED | PRU1_RED_LED)
//##############################################################################


#endif /* PRU_GPO_CT_H_ */
