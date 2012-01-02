/*
 * io.h
 *
 *  Created on: Jul 26, 2011
 *      Author: brent
 */

#ifndef IO_H_
#define IO_H_

#define LED_1 ( 1 << 27 )
#define LED_2 ( 1 << 28 )
#define LED_3 (1 << 31)

#define LED_MASK LED_1 | LED_2 | LED_3

void StartIOTasks(void);
void InitIO(void);
void InitLEDs(void);
void EnableLED(unsigned int Led);
void DisableLED(unsigned int Led);
void ToggleLED (unsigned int Led);

#endif /* IO_H_ */
