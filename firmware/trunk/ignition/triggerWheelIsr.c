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

void coilPack_irq_handler( void )__attribute__ ((interrupt ("IRQ")));
void coilPack_irq_handler(void){
	
	AT91C_BASE_PIOA->PIO_SODR = g_coilDriversToFire;
	AT91C_BASE_PIOA->PIO_CODR = g_coilDriversToCharge;
	g_coilDriversToFire = 0;
	g_coilDriversToCharge = 0;		
	
	AT91C_BASE_AIC->AIC_EOICR = AT91C_BASE_TC2->TC_SR;      //  Interrupt Ack
	AT91C_BASE_AIC->AIC_ICCR  = (1 << AT91C_ID_TC2);        //  Interrupt Ack

	*AT91C_AIC_EOICR = 0;                                   // End of Interrupt
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

	AT91PS_TC TC_pt = AT91C_BASE_TC0;
	portCHAR xTaskWoken = pdFALSE;

    if ( TC_pt->TC_SR & AT91C_TC_COVFS ){
    	g_currentToothPeriodOverflowCount++;
    	if (g_currentToothPeriodOverflowCount > TRIGGER_WHEEL_OVERFLOW_THRESHOLD_ENGINE_NOT_RUNNING){
    		//Disable all coils
    		AT91C_BASE_PIOA->PIO_SODR = COIL_DRIVER_ALL_PORTS;
    		TC_pt->TC_CCR = AT91C_TC_CLKDIS;
    		TC_pt->TC_CCR = AT91C_TC_CLKEN;
    		g_engineIsRunning = 0;
    		g_wheelSynchronized = 0;
    		g_wheelSyncAttempts = 0;
    		//TODO: stop timer
    	}
    	goto trigger_wheel_irq_handler_exit;
    }

	unsigned int currentInterToothPeriodRaw = ((g_currentToothPeriodOverflowCount << 16) + TC_pt->TC_RB);

	unsigned int currentTooth = g_currentTooth;
	unsigned int lastInterToothPeriodRaw = g_lastInterToothPeriodRaw;
	unsigned int wheelSynchronized = g_wheelSynchronized;
	unsigned int currentCrankRevolutionPeriodRaw = g_currentCrankRevolutionPeriodRaw;
  
  //	struct logical_coil_driver *previous_coil_drivers = NULL;
  	
	//if the inter-tooth period is more than 1.5 times the last tooth, we
	//detected the missing tooth
	if (currentInterToothPeriodRaw > lastInterToothPeriodRaw + (lastInterToothPeriodRaw / 2)){

		//found a missing tooth
		//the missing tooth is 2x longer than normal - adjust the period
		currentInterToothPeriodRaw = currentInterToothPeriodRaw / 2;
		//missing tooth is tooth 'zero' 
		//we detect the missing tooth by detecting the tooth following
		g_toothCountAtLastSyncAttempt = currentTooth;
		
		if (currentTooth != CRANK_TEETH - 1){
			//we either:
			//1. had a partial tooth count - first time cranking or power-up
			//2. we had noise - extra pulses
			//3. we missed a tooth
			wheelSynchronized = 0;
			//start over again
			currentCrankRevolutionPeriodRaw = currentInterToothPeriodRaw;
			g_wheelSyncAttempts++;
		}
		else{
			//looks good, we counted the correct number of teeth
			//before the missing tooth: we're synchronized to the 
			//crank trigger
			wheelSynchronized = 1;
			//save the last period
			g_lastCrankRevolutionPeriodRaw = currentCrankRevolutionPeriodRaw + currentInterToothPeriodRaw;
			//start tooth #1 with the current period
			currentCrankRevolutionPeriodRaw = currentInterToothPeriodRaw;
			//we're at a missing tooth- signal RPM/advance calculation task
			g_engineIsRunning = 1;
		}
		currentTooth = 1;

	}
	else{
		//we simply detected the next tooth
		currentCrankRevolutionPeriodRaw+=currentInterToothPeriodRaw;			
		currentTooth++;
	}
	
	if (wheelSynchronized){
		if (currentTooth >= g_recalculateTooth) xTaskWoken = xSemaphoreGiveFromISR( xOnRevolutionHandle, xTaskWoken );
		
		unsigned int firePort = g_coilFirePort[currentTooth];
		if (0 !=  firePort){
			g_coilDriversToFire = firePort;
			AT91C_BASE_TC2->TC_RC = g_coilFireTimerCount[currentTooth];
			//Fire the coil
			AT91C_BASE_TC2->TC_CCR = AT91C_TC_SWTRG;
			goto trigger_wheel_irq_handler_end;
		}
		
		unsigned int chargePort = g_coilChargePort[currentTooth];
		if (0 != chargePort){
			g_coilDriversToCharge = chargePort;
			AT91C_BASE_TC2->TC_RC = g_coilChargeTimerCount[currentTooth];
			AT91C_BASE_TC2->TC_CCR = AT91C_TC_SWTRG;
			goto trigger_wheel_irq_handler_end;
		}
	}

trigger_wheel_irq_handler_end:
	g_currentToothPeriodOverflowCount = 0;
	g_wheelSynchronized = wheelSynchronized;
	g_currentTooth = currentTooth;
	g_lastInterToothPeriodRaw = currentInterToothPeriodRaw;
	g_currentCrankRevolutionPeriodRaw = currentCrankRevolutionPeriodRaw;

trigger_wheel_irq_handler_exit:
	AT91F_AIC_AcknowledgeIt (AT91C_BASE_AIC);
	portEXIT_SWITCHING_ISR( xTaskWoken);
}
