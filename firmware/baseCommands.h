/*
 * baseCommands.h
 *
 *  Created on: Jul 23, 2011
 *      Author: brent
 */

#ifndef BASECOMMANDS_H_
#define BASECOMMANDS_H_

#include "constants.h"

#define BASE_COMMANDS {"getFreeMem", "Gets Available Memory","getFreeMem", getFreeMem}

void getFreeMem(unsigned int argc, char **argv);


#endif /* BASECOMMANDS_H_ */
