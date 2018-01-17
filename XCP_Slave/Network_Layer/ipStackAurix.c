/**
*@file ipStackAurix.c
*@author Michael Wolf
*@date 21.06.2017
*@brief Initializes and manages lwIP stack.
*/


#include "Ifx_Lwip.h"
#include <Tft/conio_tft.h>
#include "HM_Config.h"

eth_addr_t ethAddr;

void initIpStack(void) {
#ifdef HM_DISABLE_RTC
	uint8 uniqueId[] = {9, 9, 8, 8, 7, 7, 6, 6};
#else
	/* initialize the TCP/IP stack */
	uint8 uniqueId[8];
	/* first we get the ethAddr from our RTC device */
	rtc_get_unique_id(&uniqueId[0]);
#endif
	ethAddr.addr[0] = uniqueId[2];
	ethAddr.addr[1] = uniqueId[3];
	ethAddr.addr[2] = uniqueId[4];
	ethAddr.addr[3] = uniqueId[5];
	ethAddr.addr[4] = uniqueId[6];
	ethAddr.addr[5] = uniqueId[7];

	/* initialize the lwip stack with unique id */
	Ifx_Lwip_init(ethAddr);
}

void pollIncomingEthernetPackets(void) {
	Ifx_Lwip_pollTimerFlags();
	Ifx_Lwip_pollReceiveFlags();
}

void printIpStackInfo(void){
	conio_ascii_printf(DISPLAYSTDOUT1, (uint8 *) "MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", ethAddr.addr[0], ethAddr.addr[1], ethAddr.addr[2], ethAddr.addr[3], ethAddr.addr[4], ethAddr.addr[5]);
	/*print IP-Addr.*/
	LWIP_DEBUGF(PING_DEBUG, ("IP Address : "));
	uint8* ipAddr = Ifx_Lwip_getIpAddrPtr();
	ip_addr_debug_print(PING_DEBUG, ipAddr);
	LWIP_DEBUGF(PING_DEBUG, ("\n"));
}
