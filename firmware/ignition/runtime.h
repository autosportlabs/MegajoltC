#ifndef RUNTIME_H_
#define RUNTIME_H_

#include "ignition_config.h"

//the number of timer ticks representing the
//trigger wheel ISR latency
//126
//210
//1050
#define ISR_OVEREAD_TIMER_TICK_COUNTS		0

#define USER_OUTPUT_RPM_HYSTERESIS  		100
#define USER_OUTPUT_LOAD_HYSTERESIS 		2
#define DEFAULT_DWELL_USEC		 			4000
#define CRANK_TEETH							36
#define MAX_DEGREES							360
#define DEGREES_PER_TOOTH					(MAX_DEGREES / CRANK_TEETH)
#define CRANKING_REVOLUTION_PERIOD_RAW		3003427 //480 RPM
#define MAX_DWELL_DEGREES					300
#define COIL_DRIVERS						8
#define DEFAULT_LOCKED_ADVANCE				10

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


void initIgnition();
void processRuntime();

void enableLockedAdvance(unsigned int argc, char **argv);
void disableLockedAdvance(unsigned int argc, char **argv);
void getRuntime(unsigned int argc, char **argv);
void getDebug(unsigned int argc, char **argv);
void setIgnitionCell(unsigned int argc, char **argv);
void setIgnitionRPMBin(unsigned int argc, char **argv);
void setIgnitionLoadBin(unsigned int argc, char **argv);
void getEngineConfig(unsigned int argc, char **argv);
void getRpmBins(unsigned int argc, char **argv);
void getLoadBins(unsigned int argc, char **argv);
void getIgnMap(unsigned int argc, char **argv);
void getUserOutCfg(unsigned int argc, char **argv);
void setUserOutCfg(unsigned int argc, char **argv);

#define IGNITION_COMMANDS \
		{"getRuntime", "Outputs a delimited list of runtime values.","getRuntime", getRuntime}, \
		{"getDebug", "Output debug values.","getDebug", getDebug}, \
		{"setIgnCell","Sets an ignition map cell.","setIgnCell <rpmBin> <loadBin> <advance>", setIgnitionCell}, \
		{"setIgnRpmBin", "Sets an ignition map RPM bin.","setIgnRpmBin <rpmBin> <value>", setIgnitionRPMBin}, \
		{"setIgnLoadBin", "Sets an ignition map load bin.,","setIgnLoadBin <rpmBin> <value>", setIgnitionLoadBin}, \
		{"getEngineCfg", "Gets the current engine configuration.","getEngineCfg", getEngineConfig}, \
		{"getIgnRpmBins", "Gets the currently active ignition RPM bins.","getIgnRpmBins", getRpmBins}, \
		{"getIgnLoadBins", "Gets the currently active load bins.","getIgnLoadBins", getLoadBins}, \
		{"getIgnMap", "Gets the currently active ignition map, in RPM x Load format.","getIgnMap", getIgnMap}, \
		{"getUserOutCfg", "Gets the currently active user output configuration.","getUserOutcfg <output>", getUserOutCfg}, \
		{"setUserOutCfg", "Sets the currently active user output configuration.","setUserOutcfg <output> <type> <mode> <trigger>", getUserOutCfg}, \
		{"enLockedAdv", "Enables locking of ignition advance","<advance>", enableLockedAdvance}, \
		{"disLockedAdv", "Disabled locking ignition advance","",disableLockedAdvance}

#endif /*RUNTIME_H_*/
