/*
 * baseCommands.c
 *
 *  Created on: Jul 23, 2011
 *      Author: brent
 */
#include "FreeRTOS.h"
#include "task.h"
#include "baseCommands.h"
#include "usb_comm.h"
#include "task.h"
#include "modp_atonum.h"
#include "board.h"


void ShowTaskInfo(unsigned char argc, char **argv){

	SendString("Task Info");
	SendCrlf();
	SendString("Status\tPri\tStack\tTask#\tName");
	SendCrlf();
	char taskList[200];
	vTaskList(taskList);
	SendString(taskList);
	SendCrlf();
}



static unsigned int readADC(unsigned int channel){

	AT91F_ADC_StartConversion (AT91C_BASE_ADC);

	/* Fourth Step: Waiting Stop Of conversion by pulling */
	while (!((AT91F_ADC_GetStatus (AT91C_BASE_ADC)) & (1<<channel)));

	/* Fifth Step: Read the ADC result output */
	unsigned int result = AT91F_ADC_GetLastConvertedData(AT91C_BASE_ADC);
	return result;
}

extern void timer0_c_irq_handler(void);

unsigned int g_adcValue;
static int g_triggerWheelRunning = 0;
xTaskHandle g_taskHandle = NULL;


static int updateADC(){
	unsigned int adc = readADC(4);
	return (adc * 60) + 1000;
}


void RunTriggerwheel(unsigned char argc, char **argv){

	//speed is on ADC4
	AT91F_ADC_EnableChannel(AT91C_BASE_ADC,ADC_CHANNEL);
	AT91F_PIO_CfgOutput(AT91C_BASE_PIOA,TW_PORT);
	AT91F_PIO_SetOutput(AT91C_BASE_PIOA,TW_PORT);


	int runTask = 1;
	if (argc > 1){
		runTask = modp_atoi(argv[1]);
	}

	if (argc > 2){
		g_adcValue = modp_atoi(argv[2]);
	}
	else{
		g_adcValue = updateADC();
	}

	if (! g_triggerWheelRunning){
		AT91C_BASE_TC0->TC_RC = g_adcValue;

		//* Open timer0
		AT91F_TC_Open(AT91C_BASE_TC0,TC_CLKS_MCK2 | AT91C_TC_WAVESEL_UP_AUTO,AT91C_ID_TC0);

		//* Open Timer 0 interrupt
		AT91F_AIC_ConfigureIt ( AT91C_BASE_AIC, AT91C_ID_TC0, TIMER0_INTERRUPT_LEVEL,AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL, timer0_c_irq_handler);
		AT91C_BASE_TC0->TC_IER = AT91C_TC_CPCS;  //  IRQ enable CPC
		AT91C_BASE_TC0->TC_RC = g_adcValue;
		AT91F_AIC_EnableIt (AT91C_BASE_AIC, AT91C_ID_TC0);

		//* Start timer0
		AT91C_BASE_TC0->TC_CCR = AT91C_TC_SWTRG ;
		g_triggerWheelRunning = 1;
	}
	if (runTask){
		xTaskCreate( onTriggerwheelTask,( signed portCHAR * ) "triggerWheel", 50, NULL,1,&g_taskHandle );
	}
	else{
		vTaskSuspendAll();
	}
}


void onTriggerwheelTask(void *pvParameters){

	while(1){
		g_adcValue = updateADC();
		vTaskDelay(100);
	}

}
