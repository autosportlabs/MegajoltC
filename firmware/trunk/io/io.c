/*
 * io.c
 *
 *  Created on: Jul 26, 2011
 *      Author: brent
 */
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "io.h"

void StartIOTasks(void){
	InitIO();
}
void InitIO(void){
	InitLEDs();
}

void InitLEDs(void){
    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, LED_MASK ) ;
   //* Clear the LED's.
    AT91F_PIO_SetOutput( AT91C_BASE_PIOA, LED_MASK ) ;
}

void EnableLED(unsigned int Led){
        AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, Led );
}

void DisableLED(unsigned int Led){
        AT91F_PIO_SetOutput( AT91C_BASE_PIOA, Led );
}

void ToggleLED (unsigned int Led){
    if ( (AT91F_PIO_GetInput(AT91C_BASE_PIOA) & Led ) == Led )
    {
        AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, Led );
    }
    else
    {
        AT91F_PIO_SetOutput( AT91C_BASE_PIOA, Led );
    }
}
