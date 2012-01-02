#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "runtime.h"
#include "semphr.h"
#include "board.h"
#include "io.h"


extern xSemaphoreHandle 			xOnRevolutionHandle;
extern unsigned int					g_currentCrankRevolutionPeriodRaw;
extern unsigned int 				g_lastCrankRevolutionPeriodRaw;
extern unsigned int 				g_currentToothPeriodOverflowCount;
extern unsigned int 				g_currentInterToothPeriodRaw;
extern unsigned int 				g_lastInterToothPeriodRaw;
extern unsigned int					g_coilDriversToFire;
extern unsigned int					g_coilDriversToCharge;
extern unsigned int					g_currentTooth;
extern unsigned int 				g_engineIsRunning;
extern unsigned int					g_toothCountAtLastSyncAttempt;
extern unsigned int					g_wheelSyncAttempts;
extern unsigned int 				g_wheelSynchronized;
extern unsigned int 				g_recalculateTooth;
extern unsigned int 				g_coilFirePort[CRANK_TEETH];
extern unsigned int					g_coilFireTimerCount[CRANK_TEETH];
extern unsigned int					g_coilChargePort[CRANK_TEETH];
extern unsigned int					g_coilChargeTimerCount[CRANK_TEETH];

#define TRIGGER_WHEEL_OVERFLOW_THRESHOLD_ENGINE_NOT_RUNNING 10


void coilPackCharge_irq_handler(void )__attribute__ ((naked));
void coilPackCharge_irq_handler(void){
	
	portSAVE_CONTEXT()

	AT91C_BASE_PIOA->PIO_CODR = g_coilDriversToCharge;
	g_coilDriversToCharge = 0;		
	
	AT91C_BASE_AIC->AIC_EOICR = AT91C_BASE_TC2->TC_SR;      //  Interrupt Ack
	AT91C_BASE_AIC->AIC_ICCR  = (1 << AT91C_ID_TC2);        //  Interrupt Ack

	*AT91C_AIC_EOICR = 0;                                   // End of Interrupt
	portRESTORE_CONTEXT();
}

void coilPackFire_irq_handler(void )__attribute__ ((naked));
void coilPackFire_irq_handler(void){

	portSAVE_CONTEXT();
	AT91C_BASE_PIOA->PIO_SODR = g_coilDriversToFire;
	g_coilDriversToFire = 0;

	AT91C_BASE_AIC->AIC_EOICR = AT91C_BASE_TC1->TC_SR;      //  Interrupt Ack
	AT91C_BASE_AIC->AIC_ICCR  = (1 << AT91C_ID_TC1);        //  Interrupt Ack

	*AT91C_AIC_EOICR = 0;                                   // End of Interrupt
	portRESTORE_CONTEXT();
}


// Notes:
// Clock frequency is 48054840
// at 10,000 RPM:
// 		crank period is .006 seconds
// 		288329.04 clocks per revolution
//      with 36 teeth: 8009 clocks per tooth
//      per 0.1 degree: 800.9 clocks
//		per 0.01 degree: 80 clocks
//      ergo: a latency of 80 clocks introduces 0.01 degree of error at 10,000 RPMs
//
// TODO: count the number of clocks between ISR firing and coil pack trigger

void triggerWheel_irq_handler(void )__attribute__ ((naked));
void triggerWheel_irq_handler(void)
{
	portENTER_SWITCHING_ISR();

	if ( (AT91F_PIO_GetInput(AT91C_BASE_PIOA) & LED_3 ) == LED_3 )
	    {
	        AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, LED_3 );
	    }
	    else
	    {
	        AT91F_PIO_SetOutput( AT91C_BASE_PIOA, LED_3 );
	    }

	portCHAR xTaskWoken = pdFALSE;

	AT91PS_TC TC_pt = AT91C_BASE_TC0;
    if ( TC_pt->TC_SR & AT91C_TC_COVFS ){ //if timer overflowed
    	g_currentToothPeriodOverflowCount++;
    	if (g_currentToothPeriodOverflowCount > TRIGGER_WHEEL_OVERFLOW_THRESHOLD_ENGINE_NOT_RUNNING){
    		//Disable all coils
    		AT91C_BASE_PIOA->PIO_SODR = COIL_DRIVER_ALL_PORTS;
    		//TC_pt->TC_CCR = AT91C_TC_CLKDIS;
    		//TC_pt->TC_CCR = AT91C_TC_CLKEN;
    		g_engineIsRunning = 0;
    		g_wheelSynchronized = 0;
    		g_wheelSyncAttempts = 0;
    		//TODO: stop timer
    	}
    }
    else{
        g_currentInterToothPeriodRaw = ((g_currentToothPeriodOverflowCount << 16) + TC_pt->TC_RB);
        xTaskWoken = xSemaphoreGiveFromISR( xOnRevolutionHandle, xTaskWoken );;
    }

	AT91F_AIC_AcknowledgeIt (AT91C_BASE_AIC);
	portEXIT_SWITCHING_ISR( xTaskWoken );
}
