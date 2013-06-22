
#include "usart.h"


#define USART_INTERRUPT_LEVEL 5

void usart0_irq_handler (void);
void usart1_irq_handler (void);

int initSerial()
{
	unsigned int mode =
                        AT91C_US_CLKS_CLOCK
                        | AT91C_US_CHRL_8_BITS
                        | AT91C_US_PAR_NONE
                        | AT91C_US_NBSTOP_1_BIT
                        | AT91C_US_CHMODE_NORMAL;

	initUsart0(mode, 115200);
	initUsart1(mode, 115200);
	return 1;
}

void initUsart0(unsigned int mode, unsigned int baud){

	//Enable USART0
	
 	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			AT91C_PA6_TXD0, 				// mux function A
			0); // mux funtion B

	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			AT91C_PA5_RXD0, 				// mux function A
			0); // mux funtion B  

    // Configure the USART in the desired mode @115200 bauds
    USART_Configure(AT91C_BASE_US0, mode, baud, BOARD_MCK);

    // Enable receiver & transmitter
    AT91C_BASE_US0->US_CR = AT91C_US_TXEN;
	AT91C_BASE_US0->US_CR = AT91C_US_RXEN;

	/* Enable the Rx interrupts.  The Tx interrupts are not enabled
	until there are characters to be transmitted. */
	AT91F_US_EnableIt( AT91C_BASE_US0, AT91C_US_RXRDY );
	
	/* Enable the interrupts in the AIC. */
	AT91F_AIC_ConfigureIt( AT91C_BASE_AIC, AT91C_ID_US0, USART_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_EXT_LOW_LEVEL, usart0_irq_handler );
	AT91F_AIC_EnableIt( AT91C_BASE_AIC, AT91C_ID_US0 );
	
}

void initUsart1(unsigned int mode, unsigned int baud){
	
 	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			AT91C_PA22_TXD1, 				// mux function A
			0); // mux funtion B

	AT91F_PIO_CfgPeriph(AT91C_BASE_PIOA, 
			AT91C_PA21_RXD1, 				// mux function A
			0); // mux funtion B  

    // Configure the USART in the desired mode @115200 bauds
    USART_Configure(AT91C_BASE_US1, mode, baud, BOARD_MCK);

    // Enable receiver & transmitter
    AT91C_BASE_US1->US_CR = AT91C_US_TXEN;
	AT91C_BASE_US1->US_CR = AT91C_US_RXEN;

	/* Enable the Rx interrupts.  The Tx interrupts are not enabled
	until there are characters to be transmitted. */
	AT91F_US_EnableIt( AT91C_BASE_US1, AT91C_US_RXRDY );
	
	/* Enable the interrupts in the AIC. */
	AT91F_AIC_ConfigureIt( AT91C_BASE_AIC, AT91C_ID_US1, USART_INTERRUPT_LEVEL, AT91C_AIC_SRCTYPE_EXT_LOW_LEVEL, usart1_irq_handler );
	AT91F_AIC_EnableIt( AT91C_BASE_AIC, AT91C_ID_US1 );
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Configures an USART peripheral with the specified parameters.
/// \param usart  Pointer to the USART peripheral to configure.
/// \param mode  Desired value for the USART mode register (see the datasheet).
/// \param baudrate  Baudrate at which the USART should operate (in Hz).
/// \param masterClock  Frequency of the system master clock (in Hz).
//------------------------------------------------------------------------------
void USART_Configure(AT91S_USART *usart,
                            unsigned int mode,
                            unsigned int baudrate,
                            unsigned int masterClock)
{
    // Reset and disable receiver & transmitter
    usart->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX
                   | AT91C_US_RXDIS | AT91C_US_TXDIS;

    // Configure mode
    usart->US_MR = mode;

    // Configure baudrate
    // Asynchronous, no oversampling
    if (((mode & AT91C_US_SYNC) == 0)
        && ((mode & AT91C_US_OVER) == 0)) {
    
        usart->US_BRGR = (masterClock / baudrate) / 16;
    }
    // TODO other modes
}
