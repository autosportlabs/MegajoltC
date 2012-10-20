/*
 * baseCommands.c
 *
 *  Created on: Jul 23, 2011
 *      Author: brent
 */
#include "baseCommands.h"
#include "usb_comm.h"
#include "task.h"

static void outputTaskInfo(xList *pxList, char * status)
{
	volatile tskTCB *pxNextTCB, *pxFirstTCB;
	unsigned portSHORT usStackRemaining;

	if (listLIST_IS_EMPTY( pxList ) ) return;


	/* Write the details of all the TCB's in pxList into the buffer. */
	listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
	do
	{
		listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
		usStackRemaining = usTaskCheckFreeStackSpace( ( unsigned portCHAR * ) pxNextTCB->pxStack );

		SendString(status);
		SendChar('\t');
		SendUint(pxNextTCB->uxPriority);
		SendChar('\t');
		SendUint(usStackRemaining);
		SendChar('\t');
		SendUint(pxNextTCB->uxTCBNumber);
		SendChar('\t');
		SendString((char *)pxNextTCB->pcTaskName);
		SendCrlf();
	} while( pxNextTCB != pxFirstTCB );
}

void ShowTaskInfo(unsigned int argc, char **argv){

	unsigned portBASE_TYPE uxQueue = getTopUsedPriority() + 1;
	xList * readyTasksList = getReadyTasksList();
	SendString("Status\tPri.\tStack\tTCB\tName\r\n");
	SendString("------\t----\t-----\t---\t----\r\n");

	vTaskSuspendAll();
	do
	{
		uxQueue--;
		outputTaskInfo( ( xList * ) &( readyTasksList[ uxQueue ] ), "Ready" );
	}while( uxQueue > ( unsigned portSHORT ) tskIDLE_PRIORITY );


	outputTaskInfo(getDelayedTasksList(), "Blocked");
	outputTaskInfo(getOverflowDelayedTasksList(), "Blocked");
#if ( INCLUDE_vTaskDelete == 1 )
	outputTaskInfo(getTasksWaitingTermination(), "Deleted" );
#endif

#if ( INCLUDE_vTaskSuspend == 1 )
	outputTaskInfo(getSuspendedTasksList(), "Suspended" );
#endif
	xTaskResumeAll();
}

