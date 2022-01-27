#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <atomic>
#include "../mylibs/libarm/analog.h"
#include "../mylibs/libarm/pin.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "sync.h"

using namespace std;

//atomic<short> powerLevel;
//atomic<short> powerMode;

/*---------------------------------------------------------------*/
//DEFINES
#define POWER_LIMIT3 4096
#define POWER_LIMIT2 2730
#define POWER_LIMIT1 1365
#define POWER_LIMIT0 20

//DEJAR ESTOS QUE NO TODOS LOS GPIO VAN BIEN
#define LED_GREEN 60
#define LED_YELLOW 51
#define LED_RED 48

#define ANALOG_INPUT 0
/*---------------------------------------------------------------*/

void led_control_run(){
	set_pin_direction(LED_GREEN, DIRECTION_OUT);
	set_pin_direction(LED_YELLOW, DIRECTION_OUT);
	set_pin_direction(LED_RED, DIRECTION_OUT);
	powerMode = SERVER;
	powerLevel = POWER_NULL;
	std::cout << "Led control thread started..." << std::endl;

	while (shouldRun) {
		int valor = 0;
		if (powerMode == POTENCIOMETRO){
			valor = readAnalog(ANALOG_INPUT);
			//printf("Datos: %d\n", valor);
			if (valor <= POWER_LIMIT3)
			{
				write_pin(LED_GREEN,PIN_ON);
			}
			if (valor <= POWER_LIMIT0)
			{
				write_pin(LED_GREEN,PIN_OFF);
			}
			if(valor >=POWER_LIMIT1)
			{
				write_pin(LED_YELLOW,PIN_ON);
			}
			if(valor <POWER_LIMIT1){
				write_pin(LED_YELLOW,PIN_OFF);
			}
			if(valor >=POWER_LIMIT2)
			{
				write_pin(LED_RED,PIN_ON);
			}
			if(valor <POWER_LIMIT2)
			{
				write_pin(LED_RED,PIN_OFF);
			}
		}	
		else if (powerMode == SERVER)
		{
			if ( powerLevel == POWER_NULL){
				write_pin(LED_GREEN,PIN_OFF);
				write_pin(LED_YELLOW,PIN_OFF);
				write_pin(LED_RED,PIN_OFF);
			}
			if ( powerLevel == LEVEL1){
				write_pin(LED_GREEN,PIN_ON);
				write_pin(LED_YELLOW,PIN_OFF);
				write_pin(LED_RED,PIN_OFF);
			}
			if ( powerLevel == LEVEL2){
				write_pin(LED_GREEN,PIN_ON);
				write_pin(LED_YELLOW,PIN_ON);
				write_pin(LED_RED,PIN_OFF);
			}
			if ( powerLevel == LEVEL3){
				write_pin(LED_GREEN,PIN_ON);
				write_pin(LED_YELLOW,PIN_ON);
				write_pin(LED_RED,PIN_ON);
			}
		}
		else if (powerMode == EMERGENCIA)
		{
			while (1)
			{
				if( powerMode != EMERGENCIA){ break;}
				else{
					write_pin(LED_GREEN,PIN_ON);
					write_pin(LED_YELLOW,PIN_ON);
					write_pin(LED_RED,PIN_ON);
					usleep(1000*100);
					write_pin(LED_GREEN,PIN_OFF);
					write_pin(LED_YELLOW,PIN_OFF);
					write_pin(LED_RED,PIN_OFF);
					usleep(1000*100);
		
				}
			}
		}
		usleep(1000*100);
		//std::cout << "Datos: " << valor << ", Mode: " << powerMode << ", Level: " << powerLevel << std::endl; 

	}
	return;
}

#endif
