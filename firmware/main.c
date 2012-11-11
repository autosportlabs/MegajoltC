
#include <stddef.h>
#include "io.h"
#include "runtime.h"
#include "system.h"
#include "comm.h"

int main( void )
{
	setupHardware();
	initComms();
	initIO();
	initIgnition();


	while(1){
		processCommand();
		processIgnition();
	}
   return 0;
}
