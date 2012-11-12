/*
 * io.h
 *
 *  Created on: Jul 26, 2011
 *      Author: brent
 */
#include <stddef.h>

#ifndef IO_H_
#define IO_H_

#define LED_1 					( 1 << 27 )
#define LED_2 					( 1 << 28 )

#define LED_MASK 				( LED_1 | LED_2 )

#define TACH_OUT 				( 1 << 30 )
#define SHIFT_LIGHT 			( 1 << 20 )
#define PGM_OUT 				( 1 << 11 )
#define OPTION_IN 				( 1 << 26 )

#define BATT_ADC_CHANNEL		0
#define AUX_ADC_CHANNEL			4
#define TPS_ADC_CHANNEL 		5
#define MAP_ADC_CHANNEL			6
#define TEMP_ADC_CHANNEL 		7

#define BATT_ADC				( 1 << BATT_ADC_CHANNEL )
#define AUX_ADC					( 1 << AUX_ADC_CHANNEL )
#define TPS_ADC					( 1 << TPS_ADC_CHANNEL )
#define MAP_ADC					( 1 << MAP_ADC_CHANNEL )
#define TEMP_ADC				( 1 << TEMP_ADC_CHANNEL )

#define ALL_ADC					BATT_ADC | AUX_ADC | TPS_ADC | MAP_ADC | TEMP_ADC

void initIO(void);
void enableLED(unsigned int Led);
void disableLED(unsigned int Led);
void toggleLED (unsigned int Led);
unsigned int readADC(unsigned int channel);
void readAllADC(	unsigned int *a0,
						unsigned int *a1,
						unsigned int *a2,
						unsigned int *a3,
						unsigned int *a4,
						unsigned int *a5,
						unsigned int *a6,
						unsigned int *a7 );

#endif /* IO_H_ */
