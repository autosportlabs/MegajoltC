#include "usb.h"
#include "board.h"
#include "cdc_enumerate.h"
#include "mod_string.h"


#define BUFFER_SIZE 200

static struct _AT91S_CDC 	pCDC;
static char buffer[BUFFER_SIZE];
static int bufferIndex = 0;

char * USB_readLine(void){

	if (pCDC.IsConfigured(&pCDC)) {
		char byte;
		for (unsigned int length = 0; (length = pCDC.Read(&pCDC, &byte, 1)) != 0; ){
			pCDC.Write(&pCDC, &byte, 1);
			if (bufferIndex == BUFFER_SIZE - 1 || byte == '\r'){
				pCDC.Write(&pCDC, "\n", 1);
				buffer[bufferIndex] = '\0';
				bufferIndex = 0;
				return buffer;
			}
			else if ('\b' == byte && bufferIndex > 0){
				bufferIndex--;
			}
			else{
			   buffer[bufferIndex++] = byte;
			}
		}
	}
	return 0;
}

void USB_write(const char * data, size_t length){

	if (pCDC.IsConfigured(&pCDC)) {
		pCDC.Write(&pCDC, data, length );
	}
}

void USB_Open(void)
{
    // Set the PLL USB Divider
    AT91C_BASE_CKGR->CKGR_PLLR |= AT91C_CKGR_USBDIV_1 ;

    // Specific Chip USB Initialisation
    // Enables the 48MHz USB clock UDPCK and System Peripheral USB Clock
    AT91C_BASE_PMC->PMC_SCER = AT91C_PMC_UDP;
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_UDP);

    // Enable UDP PullUp (USB_DP_PUP) : enable & Clear of the corresponding PIO
    // Set in PIO mode and Configure in Output
    AT91F_PIO_CfgOutput(AT91C_BASE_PIOA,AT91C_PIO_PA16);
    // Clear for set the Pul up resistor
    AT91F_PIO_ClearOutput(AT91C_BASE_PIOA,AT91C_PIO_PA16);

    // CDC Open by structure initialization
    AT91F_CDC_Open(&pCDC, AT91C_BASE_UDP);

    bufferIndex = 0;
    memset(buffer, 0, BUFFER_SIZE);
}
