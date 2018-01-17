/**
*@file udpxcp.h
*@author Michael Wolf
*@date 27.11.2017
*@brief XCP transport layer, Ethernet UDP/IP implementation.
*/

#ifndef UDPXCP_H
#define UDPXCP_H

#include <QByteArray>
#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>
#include <QDataStream>

#include <xcpConfig/xcpHostConfig.h>
#include <memory>
#include <stdint.h>

#include "xcpConfig/ethernetconfig.h"

#define XCP_PACKET_LENGTH 12
#define XCP_HEADER_LENGTH 4

class UdpXcp : public QObject {

    Q_OBJECT

  public:
    UdpXcp(QObject *parent = Q_NULLPTR);
    ~UdpXcp();

    bool initSocket(const EthernetConfig& ethernetConfig,const XcpOptions::ENDIAN& endian);
    void writeDatagram(const QByteArray& payload);

  signals:
    void sigResponseReceived(const QByteArray xcpPacket);

  private slots:
    void readPendingDatagrams();

  private:
    QUdpSocket* udpSocket;
    QHostAddress ipClient;
    int portClient;
    QDataStream::ByteOrder endian;

    uint16_t packetCounter;

#ifdef DEBUG
    void printMsg(const QString& msgName, const uint16_t& len, const uint16_t& ctr, const QByteArray& packet);
#endif

};



#endif // UDPXCP_H
