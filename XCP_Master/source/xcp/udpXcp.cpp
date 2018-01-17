
/**
*@file udpxcp.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief XCP transport layer, Ethernet UDP/IP implementation.
*/


#include "udpxcp.h"

#include <QDataStream>
#include <QtDebug>
#include <iostream>

struct XcpEthernetMsg {
    uint16_t len;
    uint16_t ctr;
    QByteArray packet;
    XcpEthernetMsg() {}
    XcpEthernetMsg(uint16_t len, uint16_t ctr, QByteArray packet) : len(len), ctr(ctr), packet(packet) {}
};

QDataStream& operator<<(QDataStream& stream, const XcpEthernetMsg& xcpEthernetMsg) {
    stream << xcpEthernetMsg.len << xcpEthernetMsg.ctr;
    int ret = stream.writeRawData(xcpEthernetMsg.packet.data(), xcpEthernetMsg.len);
    if(ret == -1) {
        qCritical() << "Error: udpXcp: writeRawData().";
    }
    return stream;
}

template <typename T> T get(QDataStream& stream) {
    T value;
    stream >> value;
    return value;
}

QDataStream& operator>>(QDataStream& stream, XcpEthernetMsg& xcpEthernetMsg) {
    xcpEthernetMsg.len = get<uint16_t>(stream);
    xcpEthernetMsg.ctr = get<uint16_t>(stream);
    char* tmp = new char[xcpEthernetMsg.len];
    stream.readRawData(tmp, xcpEthernetMsg.len);
    xcpEthernetMsg.packet.clear();
    xcpEthernetMsg.packet.append(tmp, xcpEthernetMsg.len);
    delete[] tmp;
    return stream;
}


UdpXcp::UdpXcp(QObject *parent) : QObject(parent) {
    udpSocket = nullptr;
    ipClient = QHostAddress();
    portClient = 0;
    endian = QDataStream::ByteOrder::LittleEndian;
    packetCounter = 0;
}

UdpXcp::~UdpXcp() {
    delete udpSocket;
}

bool UdpXcp::initSocket(const EthernetConfig& ethernetConfig, const XcpOptions::ENDIAN& endian) {
    packetCounter = 0;

    ipClient = ethernetConfig.getIpClient();
    portClient = ethernetConfig.getPortClient();
    if(endian == XcpOptions::ENDIAN::ENDIAN_LITTLE) {
        this->endian = QDataStream::ByteOrder::LittleEndian;
    } else {
        this->endian = QDataStream::ByteOrder::BigEndian;
    }

    udpSocket = new QUdpSocket(this);
    QObject::connect(udpSocket, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

    static const int PORT_RANDOM = 0;
    bool ret = udpSocket->bind(ethernetConfig.getIpHost(), PORT_RANDOM);
    if(!ret) {
        qCritical() << "Error: udpXcp: initSocket(): failed to bind udp socket.";
        return false;
    }
    return true;
}

void UdpXcp::writeDatagram(const QByteArray& payload) {
    XcpEthernetMsg xcpEthernetMsg(payload.size(), packetCounter, payload);

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    stream.setByteOrder(endian);
    stream << xcpEthernetMsg;

    udpSocket->writeDatagram(buffer, ipClient, portClient);

    packetCounter++;

#ifdef DEBUG
    printMsg("CMD", xcpEthernetMsg.len, xcpEthernetMsg.ctr, xcpEthernetMsg.packet);
#endif
}

void UdpXcp::readPendingDatagrams() {
    XcpEthernetMsg xcpEthernetMsg;
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray buffer(udpSocket->pendingDatagramSize(), Qt::Uninitialized);
        QDataStream stream(&buffer, QIODevice::ReadOnly);
        stream.setByteOrder(endian);
        udpSocket->readDatagram(buffer.data(), buffer.size());
        stream >> xcpEthernetMsg;
        emit sigResponseReceived(xcpEthernetMsg.packet);

#ifdef DEBUG
        printMsg("RESP.", xcpEthernetMsg.len, xcpEthernetMsg.ctr, xcpEthernetMsg.packet);
#endif
    }
}

#ifdef DEBUG
void UdpXcp::printMsg(const QString& msgName, const uint16_t& len, const uint16_t& ctr, const QByteArray& packet) {
    qDebug() << msgName << "length:" << len << "counter:" << ctr << "packet:" << packet.toHex(' ').toUpper();
}
#endif





