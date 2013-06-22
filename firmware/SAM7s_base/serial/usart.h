#ifndef USART_H
#define USART_H

#define USART_MODE_8N1 AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT | AT91C_US_CHMODE_NORMAL

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <board.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USART modes"
/// This page lists several common operating modes for an USART peripheral.
/// 
/// !Modes
/// - USART_MODE_ASYNCHRONOUS

/// Basic asynchronous mode, i.e. 8 bits no parity.
#define USART_MODE_ASYNCHRONOUS     (AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

int initSerial();

void initUsart0(unsigned int mode, unsigned int baud);

void initUsart1(unsigned int mode, unsigned int baud);

void USART_Configure(AT91S_USART *usart, unsigned int mode, unsigned int baudrate, unsigned int masterClock);

#endif //#ifndef USART_H

