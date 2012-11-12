
#include <stddef.h>
#include "io.h"
#include "runtime.h"
#include "system.h"
#include "comm.h"

int main( void )
{
	initSystemHardware();
	initIO();
	initComms();
	initIgnition();


	while(1){
		processCommand();
		processRuntime();
	}
   return 0;
}
