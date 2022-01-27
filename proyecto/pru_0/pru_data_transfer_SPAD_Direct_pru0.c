

/**
 * @author  Javier Arin
 * @date    30/11/2021
 * @brief   A program that sends an interrupt when a switch on the PRU cape is pressed.
 *          The event channel and host are configurable via pru_interrupt.h. The receiver
 *          is the PRU1
 *
 * pru_interrupt_pru0.c
 */

#include <stdint.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <pru_ctrl.h>

#include "../../mylibs/libpru/pru_gpi_ct.h"
#include "../../mylibs/libpru/pru_gpo_ct.h"
#include "../../mylibs/libpru/pru_time_ct.h"
#include "../pru_1/arm_pru_common.h"

#include "resource_table_0.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

DirectMsg directMsg;

int main(void)
{
    //Set memory to default config
    CT_CFG.GPCFG0 = 0;      //Reset the options
    __R30 = 0;              //Reset all output
    directMsg.pressed_button_option = NO_BUTTON_PRESSED;

    while(1){
        // Send the interrupt and blink the led if the switch has been pressed (The switch is pullup, active low)
        if(!(__R31 & PRU_CAPE_PRU0_SW1)) {
            directMsg.pressed_button_option = BUTTON1_PRESSED;
            __xout(XIN_DEVICE_ID, XIN_REGISTER, 0, directMsg);
            __R30 ^= PRU0_GREEN_LED;
            __delay_cycles(DELAY_HALF_SECOND);
            __R30 ^= PRU0_GREEN_LED;

        }else if(!(__R31 & PRU_CAPE_PRU0_SW2)) {
            directMsg.pressed_button_option = BUTTON2_PRESSED;
            __xout(XIN_DEVICE_ID, XIN_REGISTER, 0, directMsg);
            __R30 ^= PRU0_BLUE_LED;
            __delay_cycles(DELAY_HALF_SECOND);
            __R30 ^= PRU0_BLUE_LED;

        }else {
            directMsg.pressed_button_option = NO_BUTTON_PRESSED;
            __xout(XIN_DEVICE_ID, XIN_REGISTER, 0, directMsg);
        }
    }

    return 0;
}
