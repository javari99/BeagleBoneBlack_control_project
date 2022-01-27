
/**
 * @author  Javier Arin
 * @brief   A program that calculates the square root of the values pasted on a
 *          shared memory location with the ARM
 *
 * pru_interrupt_pru0.c
 */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "resource_table_1.h"
#include "arm_pru_common.h"
#include "../../mylibs/libpru/pru_gpo_ct.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Host-1 Interrupt sets bit 31 in register R31 */
#define HOST_INT			((uint32_t) 1 << 31)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 */
#define TO_ARM_HOST				18
#define FROM_ARM_HOST			19

/*
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
#define CHAN_NAME			"rpmsg-pru"
#define CHAN_DESC			"Channel 31"
#define CHAN_PORT			31

/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

uint8_t payload[RPMSG_BUF_SIZE];

//My custom macros
#define STORAGE_SIZE 10

uint16_t calculate_mean(int16_t* arr){
	int16_t sum = 0;
	uint8_t i;
	for (i = 0; i < STORAGE_SIZE; i++) {
		sum = sum + arr[i];
	}
	return sum/STORAGE_SIZE;
}
/*
uint16_t calculate_abs_mean(int16_t* arr){
	int16_t sum = 0;
	uint8_t i;
	for (i = 0; i < STORAGE_SIZE; i++) {
		sum = sum + abs(arr[i]);
	}
	return sum/STORAGE_SIZE;
}
*/
uint16_t calculate_deviation(int16_t* arr, uint16_t mean){
	int16_t sum = 0;
	uint8_t i;
	for (i = 0; i < STORAGE_SIZE; i++) {
		sum = sum + (arr[i] - mean)*(arr[i] - mean);
	}
	return sqrtf(sum/STORAGE_SIZE);
}

//Threshold values definition
#define MIN_ACCEL_VARIANCE 	900
#define MAX_LIGHT 	1000
#define MAX_TEMP 	25

/*
 * main.c
 */


void main(void)
{
    __R30 = 0;

	//Initialize data storage
	int16_t values_x[STORAGE_SIZE] = {0};
	int16_t values_y[STORAGE_SIZE] = {0};
	int16_t values_z[STORAGE_SIZE] = {0};
	int16_t values_light[STORAGE_SIZE] = {0};
	int16_t values_temp[STORAGE_SIZE] = {0};
	//Current data insertion index
	uint8_t dataIndex = 0;

	struct pru_rpmsg_transport transport;
	uint16_t src, dst, len;
	volatile uint8_t *status;

	volatile DataInput* pru1_dram_datainput = (DataInput*)(PRU_SHAREDMEM + PRU_SHAREDMEM_DATA_FROM_ARM);
	volatile Stats* pru1_dram_stats = (Stats*)(PRU_SHAREDMEM + PRU_SHAREDMEM_DATA_FROM_ARM + sizeof(DataInput));

	/* Allow OCP master port access by the PRU so the PRU can read external memories */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Clear the status of the PRU-ICSS system event that the ARM will use to 'kick' us */
	CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

	/* Make sure the Linux drivers are ready for RPMsg communication */
	status = &resourceTable.rpmsg_vdev.status;
	while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

	/* Initialize the RPMsg transport structure */
	pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

	/* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
	while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
	while (1) {
		/* Check bit 30 of register R31 to see if the ARM has kicked us */
		if (__R31 & HOST_INT) {
			/* Clear the event status */
			CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
			/* Receive all available messages, multiple messages can be sent per kick */
			while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS) {
				__R30 ^= PRU1_GREEN_LED; //Data processing LED
				__R30 &= (~PRU1_RED_LED); //Emergency led ~ is a bitwise NOT
				values_x[dataIndex] = pru1_dram_datainput->x;
				values_y[dataIndex] = pru1_dram_datainput->y;
				values_z[dataIndex] = pru1_dram_datainput->z;
				values_temp[dataIndex] = pru1_dram_datainput->temp;
				values_light[dataIndex] = pru1_dram_datainput->light;

				//Increment and clamp dataindex
				if(++dataIndex == STORAGE_SIZE) dataIndex = 0;

				//Calculate statistics
				pru1_dram_stats->x_mean = calculate_mean(values_x);
				pru1_dram_stats->x_variance = calculate_deviation(values_x, pru1_dram_stats->x_mean);
				pru1_dram_stats->y_mean = calculate_mean(values_y);
				pru1_dram_stats->y_variance = calculate_deviation(values_y, pru1_dram_stats->y_mean);
				pru1_dram_stats->z_mean = calculate_mean(values_z);
				pru1_dram_stats->z_variance = calculate_deviation(values_z, pru1_dram_stats->z_mean);
				pru1_dram_stats->temp_mean = calculate_mean(values_temp);
				pru1_dram_stats->temp_variance = calculate_deviation(values_temp, pru1_dram_stats->temp_mean);
				pru1_dram_stats->light_mean = calculate_mean(values_light);
				pru1_dram_stats->light_variance = calculate_deviation(values_light, pru1_dram_stats->light_mean);
				pru1_dram_stats->power_mode = 0;

				if(sqrtf(pru1_dram_stats->x_mean*pru1_dram_stats->x_mean + pru1_dram_stats->y_mean*pru1_dram_stats->y_mean + pru1_dram_stats->z_mean*pru1_dram_stats->z_mean) < MIN_ACCEL_VARIANCE || pru1_dram_stats->temp_mean > MAX_TEMP || pru1_dram_stats->light_mean > MAX_LIGHT){
					pru1_dram_stats->power_mode = 3;
					__R30 |= PRU1_RED_LED;
				} else{
					DirectMsg directMsg;
					__xin(XIN_DEVICE_ID, XIN_REGISTER, 0, directMsg);
					if(directMsg.pressed_button_option == BUTTON1_PRESSED){
						pru1_dram_stats->power_mode = 1;
					}else if(directMsg.pressed_button_option == BUTTON2_PRESSED){
						pru1_dram_stats->power_mode = 2;
					}
				}

				//NON BLOCKING!!
				//int a;

				/* Send a message back that the operation has been succesfully made */
			    strcpy((char*)payload, "Output");
				pru_rpmsg_send(&transport, dst, src, payload, 7);//len);
                memset(payload, 0, len);
			}
		}
	}
}
