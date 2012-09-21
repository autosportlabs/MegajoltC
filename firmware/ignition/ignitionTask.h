/*
 * ignitionTask.h
 *
 *  Created on: Jul 26, 2011
 *      Author: brent
 */

#ifndef IGNITIONTASK_H_
#define IGNITIONTASK_H_

#define IGNITION_TASK_STACK	100
#define IGNITION_TASK_PRIORITY ( tskIDLE_PRIORITY + 3 )

#define KOILS_ENABLE (1 << 4)
#define KOIL_1 (1 << 9)  //GPO0
#define KOIL_2 (1 << 10) //GPO1
#define KOIL_3 (1 << 11) //GPO2
#define KOIL_4 (1 << 12) //GPO3
#define KOIL_5 (1 << 13) //GPO4
#define KOIL_6 (1 << 21) //GPO5
#define KOIL_7 (1 << 22) //GPO6
#define KOIL_8 (1 << 29) //GPO7
#define KOILS_MASK (KOIL_1 | KOIL_2 | KOIL_3 | KOIL_4 | KOIL_5 | KOIL_6 | KOIL_7 | KOIL_8)

void startIgnitionTasks();
void ignitionTestTask(void *pvParameters);

void enableCoils();
void disableCoils();
void coilOn(unsigned int coil);
void coilOff(unsigned int coil);


#endif /* IGNITIONTASK_H_ */
