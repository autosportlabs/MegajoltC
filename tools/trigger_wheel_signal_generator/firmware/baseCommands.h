/*
 * baseCommands.h
 *
 *  Created on: Jul 23, 2011
 *      Author: brent
 */

#ifndef BASECOMMANDS_H_
#define BASECOMMANDS_H_

#define	TW_PORT (1 << 3)
#define ADC_CHANNEL (1 << 4)
#define TIMER0_INTERRUPT_LEVEL		7
#define TIMER1_INTERRUPT_LEVEL		4

/*-----------------*/
/* Clock Selection */
/*-----------------*/
#define TC_CLKS                  0x7
#define TC_CLKS_MCK2             0x0
#define TC_CLKS_MCK8             0x1
#define TC_CLKS_MCK32            0x2
#define TC_CLKS_MCK128           0x3
#define TC_CLKS_MCK1024          0x4

#include "constants.h"

#define BASE_COMMANDS \
		{"showTasks", "Show status of running tasks", "", ShowTaskInfo}, \
		{"runTw", "Runs Triggerwheel routine. speed on ADC4", "", RunTriggerwheel}


void ShowTaskInfo(unsigned char argc, char **argv);
void RunTriggerwheel(unsigned char argc, char **argv);
void LockADC(unsigned char argc, char **argv);
void onTriggerwheelTask(void *pvParameters);
#endif /* BASECOMMANDS_H_ */
