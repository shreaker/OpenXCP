/**
*@file ipStackAurix.h
*@author Michael Wolf
*@date 21.06.2017
*@brief Initializes and manages lwIP stack.
*/

#ifndef IPSTACKAURIX_H_
#define IPSTACKAURIX_H_

void initIpStack(void);

void pollIncomingEthernetPackets(void);

void printIpStackInfo(void);

#endif /* IP_UDPSTACKAURIX_H_ */
