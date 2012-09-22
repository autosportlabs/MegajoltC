/*
 * constants.h
 *
 *  Created on: Jul 23, 2011
 *      Author: brent
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include "command.h"

//Include all of the command definitions here
#include "baseCommands.h"
#include "luaCommands.h"


#define FIRMWARE_VERSION "1.0.0.0"
#define WELCOME_MSG "Welcome to SAMStamp : Firmware Version " FIRMWARE_VERSION
#define COMMAND_PROMPT "SAMStamp"

#define LUA_ENABLED

#define SYSTEM_COMMANDS {BASE_COMMANDS, LUA_COMMANDS, NULL_COMMAND}

//USB manufacturer string descriptor
//first element = length of this entire array, in bytes
//second element = 0x03 = descriptor type string (don't change)
#define USB_MFG_STRING_DESCRIPTOR 	\
	{ \
		18, \
		0x03, \
		'M', 0x00, 'a', 0x00, 'k', 0x00, 'e', 0x00, 'r', 0x00, 'B', 0x00, 'u', 0x00, 'g', 0x00  \
	}

//USB product string descriptor.
//first element = length of this entire array, in bytes
//second element = 0x03 = descriptor type string (don't change)
#define USB_PRODUCT_STRING_DESCRIPTOR \
	{ \
		20, \
		0x03, \
		'T', 0x00, 'W', 0x00, ' ', 0x00, 'S', 0x00, 'i', 0x00, 'm', 0x00, ' ', 0x00, ':', 0x00, ')', 0x00 \
	}


#endif /* CONSTANTS_H_ */
