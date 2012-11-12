/*
 * hardware.c
 *
 *  Created on: Oct 27, 2012
 *      Author: brent
 */
#include "system.h"
#include "board.h"

#define WATCHDOG_TIMEOUT		3000
/*-----------------*/
/* Clock Selection */
/*-----------------*/
#define TC_CLKS                  0x7
#define TC_CLKS_MCK2             0x0
#define TC_CLKS_MCK8             0x1
#define TC_CLKS_MCK32            0x2
#define TC_CLKS_MCK128           0x3
#define TC_CLKS_MCK1024          0x4

/* ADC field definition for the Mode Register: Reminder
                       TRGEN    => Selection bewteen Software or hardware start of conversion
                       TRGSEL   => Relevant if the previous field set a Hardware Triggering Mode
                       LOWRES   => 10-bit result if ths bit is cleared 0
                       SLEEP    => normal mode if ths is cleared
                       PRESCAL  => ADCclock = MCK / [(PRESCAL + 1)*2]
                       STARTUP  => Startup Time = [(STARTUP + 1)*8] / ADCclock
                       SHTIM    => Tracking time = (SHTIM + 1) / ADCclock
 */
#define   TRGEN    (0x0)    // Software triggering
#define   TRGSEL   (0x0)    // Without effect in Software triggering
#define   LOWRES   (0x0)    // 10-bit result output format
#define   SLEEP    (0x0)    // Normal Mode (instead of SLEEP Mode)
#define   PRESCAL  (0x4)    // Max value
#define   STARTUP  (0xc)    // This time period must be higher than 20 �s
#define   SHTIM    (0x3)    // Must be higher than 3 ADC clock cycles but depends on output
                            // impedance of the analog driver to the ADC input


static void initWatchdog(int timeoutMs){
	 int counter= AT91F_WDTGetPeriod(timeoutMs);
	 AT91F_WDTSetMode(AT91C_BASE_WDTC, AT91C_WDTC_WDRSTEN | AT91C_WDTC_WDRPROC | counter | (counter << 16));
	 AT91F_WDTC_CfgPMC();
}

void initSystemHardware( void )
{
	/* When using the JTAG debugger the hardware is not always initialised to
	the correct default state.  This line just ensures that this does not
	cause all interrupts to be masked at the start. */
	AT91C_BASE_AIC->AIC_EOICR = 0;

	/* Enable the peripheral clock. */
   AT91F_PMC_EnablePeriphClock( AT91C_BASE_PMC, (1 << AT91C_ID_PIOA) |  //Enable Clock for PIO
                                                (1 << AT91C_ID_IRQ0) |  //Enable Clock for IRQ0
                                                (1 << AT91C_ID_PWMC) |	//Enable Clock for the PWM controller
                                                (1 << AT91C_ID_US0)  |  //USART0
                                                (1 << AT91C_ID_US1)		//USART1
                              );

   /* Enable reset-button */
   AT91F_RSTSetMode( AT91C_BASE_RSTC , AT91C_RSTC_URSTEN );

   initWatchdog(WATCHDOG_TIMEOUT);
}
