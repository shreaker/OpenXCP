/**
*@file xcpTask.c
*@author Michael Wolf
*@date 21.06.2017
*@brief Interface between application business logic and XCP driver. Inits XCP driver and triggers XCP events in a loop.
*/

#include <debug.h>

#include "XcpBasic.h"
#include "udpXcp.h"


void xcpTaskInit(void) {

	err_t ret = udpXcpInit();
	if (ret == ERR_OK) {
		LWIP_DEBUGF(ECHO_DEBUG, ("XCP-on-UDP initialized.\n"));
	} else {
		LWIP_DEBUGF(ECHO_DEBUG, ("Error opening XCP socket.\n"));
	}

	XcpInit();

}

/*
 * Is called every 1 ms from main loop.
 */
void xcpTaskLoop() {
	static uint32 msCounter = 0;

	if((msCounter % 1) == 0) {
		XcpEvent(0);
		ApplXcpSendFlush();
	}
	if((msCounter % 100) == 0) {
		XcpEvent(1);
		ApplXcpSendFlush();
	}

	msCounter++;

	/* XCP driver background processing */
#ifdef XCP_ENABLE_CHECKSUM
	XcpBackground();
#endif

}
