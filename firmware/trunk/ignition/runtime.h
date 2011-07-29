#ifndef RUNTIME_H_
#define RUNTIME_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ignition_config.h"


#define USER_OUTPUT_RPM_HYSTERESIS  		100
#define USER_OUTPUT_LOAD_HYSTERESIS 		2
#define DEFAULT_DWELL_USEC		 			6000
#define CRANK_TEETH							36
#define MAX_DEGREES							360
#define DEGREES_PER_TOOTH					(MAX_DEGREES / CRANK_TEETH)
#define CRANKING_REVOLUTION_PERIOD_RAW		3003427 //480 RPM
#define MAX_DWELL_DEGREES					170  
#define COIL_DRIVERS						8

#define COILS_ENABLE (1 << 4)
//Map coils to various PIOs
#define COIL_0 (1 << 9)  //GPO0
#define COIL_1 (1 << 10) //GPO1
#define COIL_2 (1 << 11) //GPO2
#define COIL_3 (1 << 12) //GPO3
#define COIL_4 (1 << 13) //GPO4
#define COIL_5 (1 << 21) //GPO5
#define COIL_6 (1 << 22) //GPO6
#define COIL_7 (1 << 29) //GPO7

//all ports
#define COIL_DRIVER_ALL_PORTS (COIL_0 | COIL_1 | COIL_2 | COIL_3 | COIL_4 | COIL_5 | COIL_6 | COIL_7)

struct logical_coil_driver{
	int cylinderTDC;
	int coilOnDegrees;
	int coilOnTooth;
	int coilOnInterToothDegrees;
	int coilFireDegrees;
	int coilFireTooth;
	int coilFireInterToothDegrees;
	unsigned int physicalCoilDriverPorts;
};


//On revolution
void onRevolutionTask(void *);


#endif /*RUNTIME_H_*/
