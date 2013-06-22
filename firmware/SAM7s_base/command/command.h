/*
 * command.h
 *
 *  Created on: Jul 23, 2011
 *      Author: brent
 */

#ifndef COMMAND_H_
#define COMMAND_H_

struct cmd_t
{
	const char *cmd;
	const char *help;
	const char *paramHelp;
	void (*func)(unsigned int argc, char **argv);
};

#define NULL_COMMAND {NULL, NULL,NULL, NULL}

#endif
