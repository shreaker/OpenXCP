/**
*@file udpXcp.h
*@author Michael Wolf
*@date 21.06.2017
*@brief XCP transport protocol layer (XCP over Ethernet UDP/IP).
*/

#ifndef UDP_XCP_H_
#define UDP_XCP_H_


#include "lwip/udp.h"
#include "XcpBasic.h"

err_t udpXcpInit();

void udpXcpSend(uint16 lengthMsg, const uint8* msg);

err_t udpXcpSendFlush(void);


#endif /* UDP_XCP_H_ */
