/*
 * io.h
 *
 *  Created on: Jul 26, 2011
 *      Author: brent
 */

#ifndef IO_H_
#define IO_H_

#define LED_MASK (1 << 27 ) | (1 << 28 )

void StartIOTasks(void);
void InitIO(void);
void InitLEDs(void);
void EnableLED(unsigned int Led);
void DisableLED(unsigned int Led);
void ToggleLED (unsigned int Led);

#endif /* IO_H_ */
