#ifndef RUNTIME_H_
#define RUNTIME_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ignition_config.h"

//the number of timer ticks representing the
//trigger wheel ISR latency
#define ISR_OVEREAD_TIMER_TICK_COUNTS		126

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
	int coilChargeDegrees;
	int coilChargeTooth;
	int coilFireDegrees;
	int coilFireTooth;
	unsigned int physicalCoilDriverPorts;
	unsigned int timerFireCount;
	unsigned int timerChargeCount;

};


//On revolution
void onRevolutionTask(void *);


void getRuntime(unsigned char argc, char **argv);
void getDebug(unsigned char argc, char **argv);
void setIgnitionCell(unsigned char argc, char **argv);
void setIgnitionRPMBin(unsigned char argc, char **argv);
void setIgnitionLoadBin(unsigned char argc, char **argv);
void getEngineConfig(unsigned char argc, char **argv);
void getRpmBins(unsigned char argc, char **argv);
void getLoadBins(unsigned char argc, char **argv);
void getIgnMap(unsigned char argc, char **argv);
void getUserOutCfg(unsigned char argc, char **argv);
void setUserOutCfg(unsigned char argc, char **argv);

#endif /*RUNTIME_H_*/
