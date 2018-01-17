/**
*@file udpXcp.c
*@author Michael Wolf
*@date 21.06.2017
*@brief XCP transport protocol layer (XCP over Ethernet UDP/IP).
*/

#include "lwip/opt.h"
#include "lwip/pbuf.h"

#include <string.h>

#include "udpXcp.h"

/******************************************************************************/
/*------------------------Defines---------------------------------------------*/
/******************************************************************************/
#define PORT_LISTEN 5555

#define XCP_HEADER_LENGTH 4
#define XCP_PACKET_LENGTH kXcpMaxDTO
#define SEND_BUFFER_SIZE (XCP_HEADER_LENGTH + XCP_PACKET_LENGTH) * 2

#define MULTIPLE_XCP_MSGS_IN_UDP_PACKET
/******************************************************************************/
/*------------------------------Structs---------------------------------------*/
/******************************************************************************/


typedef struct masterConnect {
	struct ip_addr ip;
	u16_t port;
} MASTER_CONNECT;

typedef struct xcpMsg {
	struct header {
		unsigned short len; /* BYTE 1,2 */
		unsigned short ctr; /* BYTE 3,4 */
	} header;
	unsigned char packet[XCP_PACKET_LENGTH];  // BYTE[]
} XCP_MSG;

typedef struct xcpMsgInterface {
	int byte_pos;
	int bytes_to_read;
	XCP_MSG xcpMsg;
} XCP_MSG_INTERFACE;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
struct udp_pcb* pUdpPcb;
MASTER_CONNECT masterConnect;

XCP_MSG_INTERFACE xcpMsgInterface;
uint16 gReceivedPacketCounter = 0;
uint16 gPacketSendCounter = 0;
uint16 gDaqSendCounter = 1;

/**
 * @brief Doxygen Comment
 *
 * @XCP_Measurement
 * @XCP_Comment array gSendBuffer
 * @XCP_LowerLimit 0
 * @XCP_UpperLimit 255
 * @XCP_Discrete false
 * @XCP_MaxRefreshRate 500
 * @XCP_PhysUnit km/h
 * @XCP_ConversionFunction
 * @XCP_Writeable true
 */
uint8 gSendBuffer[SEND_BUFFER_SIZE];
uint32 gSendBufferLength = 0;
uint8* gSendBufferPosition = gSendBuffer;
/******************************************************************************/
/*------------------------ Function Prototypes--------------------------------*/
/******************************************************************************/
static void udpXcpReceive(void* arg, struct udp_pcb* pcb, struct pbuf* pBuf, struct ip_addr*, u16_t port);
static void xcpMessageInterfaceClear(XCP_MSG_INTERFACE *xcpMsg);
static int createXcpMsgFromUdpPayloadBytes(XCP_MSG_INTERFACE *xcpMsgInterface, uint8 udpPayloadByte);

/******************************************************************************/
/*------------------------ Functions------------------------------------------*/
/******************************************************************************/
err_t udpXcpInit() {

	pUdpPcb = udp_new();
	err_t ret = udp_bind(pUdpPcb, IP_ADDR_ANY, PORT_LISTEN);

	udp_recv(pUdpPcb, udpXcpReceive, NULL);

	return ret;
}

static void udpXcpReceive(void* arg, struct udp_pcb* pcb, struct pbuf* pBuf, struct ip_addr* ipAddr, u16_t port) {

	/* if packet is valid */
	if (pBuf != NULL) {

		masterConnect.ip = *ipAddr;
		masterConnect.port = port;

		uint8* payload = pBuf->payload;
		uint16 udpLength = pBuf->len;

		xcpMessageInterfaceClear(&xcpMsgInterface);
		if (udpLength > 0) {
			int i;
			for (i = 0; i < udpLength; ++i) {
				int ret = createXcpMsgFromUdpPayloadBytes(&xcpMsgInterface, payload[i]);
				if (ret == 1) {
					gReceivedPacketCounter = xcpMsgInterface.xcpMsg.header.ctr;
					XcpCommand((MEMORY_CONST vuint32*) xcpMsgInterface.xcpMsg.packet);
					xcpMessageInterfaceClear(&xcpMsgInterface);
				}
			}
		}
	} else {
		LWIP_DEBUGF(ECHO_DEBUG, ("Err: Received UDP PCBuffer is null\n"));
	}

	pbuf_free(pBuf);
}

static void xcpMessageInterfaceClear(XCP_MSG_INTERFACE *xcpMsg) {
	xcpMsg->byte_pos = 0;
	xcpMsg->bytes_to_read = XCP_HEADER_LENGTH;
	xcpMsg->xcpMsg.header.len = 0;
	xcpMsg->xcpMsg.header.ctr = 0;
	memset(xcpMsg->xcpMsg.packet, 0, XCP_PACKET_LENGTH);
}

static int createXcpMsgFromUdpPayloadBytes(XCP_MSG_INTERFACE *xcpMsgInterface, uint8 udpPayloadByte) {
	switch (xcpMsgInterface->byte_pos) {
	case 0: //length Byte 0
		xcpMsgInterface->xcpMsg.header.len = udpPayloadByte;
		xcpMsgInterface->bytes_to_read += udpPayloadByte;
		break;
	case 1: //length Byte 1
		xcpMsgInterface->xcpMsg.header.len += (udpPayloadByte << 8);
		xcpMsgInterface->bytes_to_read += (udpPayloadByte << 8);
		break;
	case 2: //counter Byte 0
		xcpMsgInterface->xcpMsg.header.ctr = udpPayloadByte;
		break;
	case 3: //counter Byte 1
		xcpMsgInterface->xcpMsg.header.ctr += (udpPayloadByte << 8);
		break;
	default: //data Bytes
		xcpMsgInterface->xcpMsg.packet[xcpMsgInterface->byte_pos - XCP_HEADER_LENGTH] = udpPayloadByte;
	}

	xcpMsgInterface->byte_pos++;
	if (xcpMsgInterface->byte_pos >= xcpMsgInterface->bytes_to_read) {
		return 1;
	}

	return 0;
}

void udpXcpSend(uint16 lengthXcpMsg, const uint8* xcpMsg) {
#ifdef	MULTIPLE_XCP_MSGS_IN_UDP_PACKET
		if ((gSendBufferLength + XCP_HEADER_LENGTH + lengthXcpMsg) > SEND_BUFFER_SIZE) {
			ApplXcpSendFlush();
		}
#endif

		XCP_MSG* pSendBuffer;
		uint16 counter = gPacketSendCounter++;

		pSendBuffer = (XCP_MSG*) gSendBufferPosition;
		if(lengthXcpMsg > XCP_PACKET_LENGTH){
			lengthXcpMsg = XCP_PACKET_LENGTH;
		}
		MEMCPY(pSendBuffer->packet, xcpMsg, lengthXcpMsg);
		pSendBuffer->header.len = lengthXcpMsg;
		pSendBuffer->header.ctr = counter;

		uint32 udpPayloadLength = (XCP_HEADER_LENGTH + lengthXcpMsg);
		gSendBufferPosition += udpPayloadLength;
		gSendBufferLength += udpPayloadLength;

#ifdef	MULTIPLE_XCP_MSGS_IN_UDP_PACKET
		if ((gSendBufferLength + XCP_HEADER_LENGTH) >= SEND_BUFFER_SIZE ) {
			ApplXcpSendFlush();
		}
#else
		ApplXcpSendFlush();
#endif
}

err_t udpXcpSendFlush(void) {
	err_t ret = ERR_OK;

		if(gSendBufferLength == 0){
			return 0;
		}

		err_t retUdp;
		do{
			struct pbuf* pBuf = pbuf_alloc(PBUF_TRANSPORT, gSendBufferLength, PBUF_RAM);
			MEMCPY(pBuf->payload, gSendBuffer, gSendBufferLength);
			retUdp = udp_sendto(pUdpPcb, pBuf, &masterConnect.ip, masterConnect.port);
			if(retUdp != ERR_OK){
				LWIP_DEBUGF(ECHO_DEBUG, ("\n udp_sendto() failed: %d\n", retUdp));
			}
			pbuf_free(pBuf);
		}while(retUdp != ERR_OK);


		//cleanup
		gSendBufferLength = 0;
		gSendBufferPosition = gSendBuffer;

	return ret;
}
