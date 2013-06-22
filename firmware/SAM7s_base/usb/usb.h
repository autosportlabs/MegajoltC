
/*
 * usb.h
 *
 *  Created on: Oct 31, 2012
 *      Author: brent
 */

#ifndef USB_H_
#define USB_H_
#include <stddef.h>

void USB_Open(void);
char * USB_readLine(void);
void USB_write(const char * data, size_t length);

#endif /* USB_H_ */
