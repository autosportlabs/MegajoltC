/*
 * ignitionTask.c
 *
 *  Created on: Jul 26, 2011
 *      Author: brent
 */
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "ignitionTask.h"
#include "io.h"


void startIgnitionTasks(){
	xTaskCreate( ignitionTestTask,( signed portCHAR * ) "ignition", IGNITION_TASK_STACK, NULL, IGNITION_TASK_PRIORITY, NULL );
}

void ignitionTestTask(void *pvParameters){

	AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, KOILS_ENABLE | KOILS_MASK );
	AT91F_PIO_SetOutput( AT91C_BASE_PIOA, KOILS_ENABLE | KOILS_MASK ) ;

	enableCoils();
	portTickType onTime = 4 / portTICK_RATE_MS;
	portTickType offTime = 10 / portTICK_RATE_MS;

    while (1){
    	coilOn(KOILS_MASK);
    	EnableLED(LED_MASK);
    	vTaskDelay(onTime);
    	coilOff(KOILS_MASK);
    	DisableLED(LED_MASK);
    	vTaskDelay(offTime);
    }
}

void enableCoils(){
	AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, KOILS_ENABLE);
}

void disableCoils(){
	AT91F_PIO_SetOutput( AT91C_BASE_PIOA, KOILS_ENABLE);
}

void coilOn(unsigned int coil){
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, coil);
}

void coilOff(unsigned int coil){
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA, coil);
}
