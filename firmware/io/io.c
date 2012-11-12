/*
 * io.c
 *
 *  Created on: Jul 26, 2011
 *      Author: brent
 */
#include "board.h"
#include "io.h"

#define   TRGEN    (0x0)    // Software triggering
#define   TRGSEL   (0x0)    // Without effect in Software triggering
#define   LOWRES   (0x0)    // 10-bit result output format
#define   SLEEP    (0x0)    // Normal Mode (instead of SLEEP Mode)
#define   PRESCAL  (0x4)    // Max value
#define   STARTUP  (0xc)    // This time period must be higher than 20 �s
#define   SHTIM    (0x3)    // Must be higher than 3 ADC clock cycles but depends on output
                            // impedance of the analog driver to the ADC input

void readAllADC(	unsigned int *a0,
						unsigned int *a1,
						unsigned int *a2,
						unsigned int *a3,
						unsigned int *a4,
						unsigned int *a5,
						unsigned int *a6,
						unsigned int *a7 ){

        /* Third Step: Start the conversion */
        AT91F_ADC_StartConversion (AT91C_BASE_ADC);

        /* Fourth Step: Waiting Stop Of conversion by polling */
        while (!((AT91F_ADC_GetStatus (AT91C_BASE_ADC)) & (1<<7)));

        if (NULL != a0) *a0 = AT91F_ADC_GetConvertedDataCH0 (AT91C_BASE_ADC);
        if (NULL != a1) *a1 = AT91F_ADC_GetConvertedDataCH1 (AT91C_BASE_ADC);
        if (NULL != a2) *a2 = AT91F_ADC_GetConvertedDataCH2 (AT91C_BASE_ADC);
        if (NULL != a3) *a3 = AT91F_ADC_GetConvertedDataCH3 (AT91C_BASE_ADC);
        if (NULL != a4) *a4 = AT91F_ADC_GetConvertedDataCH4 (AT91C_BASE_ADC);
        if (NULL != a5) *a5 = AT91F_ADC_GetConvertedDataCH5 (AT91C_BASE_ADC);
        if (NULL != a6) *a6 = AT91F_ADC_GetConvertedDataCH6 (AT91C_BASE_ADC);
        if (NULL != a7) *a7 = AT91F_ADC_GetConvertedDataCH7 (AT91C_BASE_ADC);
}

unsigned int readADC(unsigned int channel){

       /* Clear all previous setting and result */
    //   AT91F_ADC_SoftReset (AT91C_BASE_ADC);

       /* First Step: Set up by using ADC Mode register */
     //  AT91F_ADC_CfgModeReg (AT91C_BASE_ADC,
       //                     (SHTIM << 24) | (STARTUP << 16) | (PRESCAL << 8) |
         //                   (SLEEP << 5) | (LOWRES <<4) | (TRGSEL << 1) | (TRGEN )) ;

        /* Second Step: Select the active channel */
 //       AT91F_ADC_EnableChannel (AT91C_BASE_ADC, (1<<channel));

        /* Third Step: Start the conversion */
        AT91F_ADC_StartConversion (AT91C_BASE_ADC);

        /* Fourth Step: Waiting Stop Of conversion by pulling */
        while (!((AT91F_ADC_GetStatus (AT91C_BASE_ADC)) & (1<<channel)));

        unsigned int result = 0;

        switch (channel){
        case 0:
        	result = AT91F_ADC_GetConvertedDataCH0 (AT91C_BASE_ADC);
        	break;
        case 1:
        	result = AT91F_ADC_GetConvertedDataCH1 (AT91C_BASE_ADC);
        	break;
        case 2:
        	result = AT91F_ADC_GetConvertedDataCH2 (AT91C_BASE_ADC);
        	break;
        case 3:
        	result = AT91F_ADC_GetConvertedDataCH3 (AT91C_BASE_ADC);
        	break;
        case 4:
        	result = AT91F_ADC_GetConvertedDataCH4 (AT91C_BASE_ADC);
        	break;
        case 5:
        	result = AT91F_ADC_GetConvertedDataCH5 (AT91C_BASE_ADC);
        	break;
        case 6:
        	result = AT91F_ADC_GetConvertedDataCH6 (AT91C_BASE_ADC);
        	break;
        case 7:
        	result = AT91F_ADC_GetConvertedDataCH7 (AT91C_BASE_ADC);
        	break;
        default:
        	break;
        }
	return result;
}


void enableLED(unsigned int Led){
        AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, Led );
}

void disableLED(unsigned int Led){
        AT91F_PIO_SetOutput( AT91C_BASE_PIOA, Led );
}

void toggleLED (unsigned int Led){
    if ( (AT91F_PIO_GetInput(AT91C_BASE_PIOA) & Led ) == Led )
    {
        AT91F_PIO_ClearOutput( AT91C_BASE_PIOA, Led );
    }
    else
    {
        AT91F_PIO_SetOutput( AT91C_BASE_PIOA, Led );
    }
}

static void initLEDs(void){
    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, LED_MASK ) ;
   //* Clear the LED's.
    AT91F_PIO_SetOutput( AT91C_BASE_PIOA, LED_MASK ) ;
}

static void initTachOut(){
    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, TACH_OUT ) ;
}

static void initOptionInput(){
	AT91F_PIO_CfgInput(AT91C_BASE_PIOA, OPTION_IN);
	AT91C_BASE_PIOA->PIO_PPUER = OPTION_IN; //enable pullup
	AT91C_BASE_PIOA->PIO_IFER = OPTION_IN; //enable input filter
	AT91C_BASE_PIOA->PIO_MDER = OPTION_IN; //enable multi drain
}

static void initPgmOutputs(){
    AT91F_PIO_CfgOutput( AT91C_BASE_PIOA, SHIFT_LIGHT | PGM_OUT ) ;
}


static void initAdc(void){
	/* Clear all previous setting and result */
	AT91F_ADC_SoftReset (AT91C_BASE_ADC);

	/* First Step: Set up by using ADC Mode register */
	AT91F_ADC_CfgModeReg (AT91C_BASE_ADC,
					   (SHTIM << 24) | (STARTUP << 16) | (PRESCAL << 8) |
						(SLEEP << 5) | (LOWRES << 4) | (TRGSEL << 1) | (TRGEN )) ;


	/* Second Step: Select the active channel */
	AT91F_ADC_EnableChannel (AT91C_BASE_ADC, ALL_ADC);
}

void initIO(void){
	initLEDs();
	initTachOut();
	initOptionInput();
	initPgmOutputs();
	initAdc();
}
