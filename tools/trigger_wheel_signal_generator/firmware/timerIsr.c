/*
 * timerIsr.c
 *
 *  Created on: Aug 3, 2011
 *      Author: brent
 */
#include "FreeRTOS.h"
#include "baseCommands.h"
#include "board.h"
#include "task.h"

static volatile int tooth=0;
extern unsigned int g_adcValue;


/* The ISR can cause a context switch so is declared naked. */
void timer0_c_irq_handler( void ) __attribute__ ((naked));
void timer0_c_irq_handler(void)
{
	portENTER_SWITCHING_ISR();
	portCHAR xTaskWoken = pdFALSE;

	AT91PS_TC TC_pt = AT91C_BASE_TC0;
    unsigned int dummy;
    //* Acknowledge interrupt status
    dummy = TC_pt->TC_SR;
    //* Suppress warning variable "dummy" was set but never used
    dummy = dummy;


    if (tooth >=71){
    	AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, TW_PORT);
    	if (tooth >=72){
    		tooth=0;
    	}
    }
    else{
    	if ((tooth & 1) == 1){
    		AT91F_PIO_ClearOutput(AT91C_BASE_PIOA, TW_PORT);
    	}
    	else
    	{
    		AT91F_PIO_SetOutput(AT91C_BASE_PIOA, TW_PORT);
    	}
    }

    tooth++;
	AT91C_BASE_TC0->TC_RC = g_adcValue;

	/* Clear AIC to complete ISR processing */
	AT91F_AIC_AcknowledgeIt (AT91C_BASE_AIC);
	portEXIT_SWITCHING_ISR( xTaskWoken );

}
