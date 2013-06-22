#ifndef HEAP_H_
#define HEAP_H_
#include <stdlib.h>

#define portBYTE_ALIGNMENT	4
#define portBASE_TYPE       long
#define portCHAR        	char
#define pdTRUE 				1
#define pdFALSE				0

void *pvPortMalloc( size_t xWantedSize );
void vPortFree( void *pv );
void * pvPortRealloc( void *pv, size_t xWantedSize );

#endif
