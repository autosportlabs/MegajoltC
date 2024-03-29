#ifndef IGNITION_CONFIG_DEFAULTS_H_
#define IGNITION_CONFIG_DEFAULTS_H_


#define DEFAULT_MAP_CALIBRATION \
		{ \
		}

#define DEFAULT_GENERIC_CALIBRATION \
		{ \
		}

#define DEFAULT_TEMP_CALIBRATION \
		{ \
		}

#define DEFAULT_RPM_BINS \
	{500,1000,2000,2500,3000,3500,4000,5000,6000,7000,8000,9000}
 
#define DEFAULT_LOAD_BINS \
	{20,30,40,50,60,70,100,120,130,140,170,220}
  
#define DEFAULT_IGNITION_ADVANCE \
{	{12,20,25,27,30,32,35,36,36,38,40,42}, \
	{12,20,25,27,30,32,35,36,36,38,40,42}, \
	{15,20,25,27,30,32,35,36,36,36,40,42}, \
	{15,20,25,27,30,32,35,36,36,36,39,40}, \
	{15,20,25,27,28,29,30,33,34,34,36,38}, \
	{12,20,23,23,23,25,25,25,25,25,30,30}, \
	{12,18,20,22,22,23,23,23,24,24,25,25}, \
	{10,15,20,22,22,22,22,22,22,22,23,23}, \
	{10,15,20,22,22,22,22,22,22,22,22,20}, \
	{10,15,20,22,22,22,22,22,22,22,22,20}, \
	{10,15,20,22,22,22,22,22,22,22,21,19}, \
	{10,15,20,22,22,22,22,22,22,22,20,19}  \
}


/*

#define DEFAULT_IGNITION_ADVANCE \
{	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}, \
	{0,0,0,0,0,0,0,0,0,0,0,0}  \
}
*/


#define DEFAULT_OUTPUT_CONFIG {OUTPUT_TYPE_RPM,MODE_NORMAL,9000}
	
#define DEFAULT_IGNITION_MAP DEFAULT_RPM_BINS,DEFAULT_LOAD_BINS,DEFAULT_IGNITION_ADVANCE

#define DEFAULT_IGNITION_CONFIG {\
	{DEFAULT_IGNITION_MAP},\
	{\
	DEFAULT_OUTPUT_CONFIG,\
	DEFAULT_OUTPUT_CONFIG\
	},\
	LOAD_TYPE_MAP \
}

#define DEFAULT_ENGINE_CONFIG { \
	4, \
	{ \
		{ 0, 0 }, \
		{ 180, 1 }, \
		{ 180, 1 }, \
		{ 0, 0 } , \
		{ 0, 0 }, \
		{ 0, 0 }, \
		{ 0, 0 }, \
		{ 0, 0 } \
	}, \
	90, \
	12 \
}

#endif /*IGNITION_CONFIG_DEFAULTS_H_*/
