/**
*@file ethernetconfig.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The EthernetConfig class is used to save the users transport and network layer settings.
*/

#include "ethernetconfig.h"

EthernetConfig::EthernetConfig(QHostAddress ipClient, int portClient, QHostAddress ipHost, PROTOCOL protocol)
    : ipClient(ipClient),
      portClient(portClient),
      ipHost(ipHost),
      protocol(protocol) {
}

const std::map<const EthernetConfig::PROTOCOL, const QString> EthernetConfig::getProtocolTypes() {
    static const std::map<const PROTOCOL, const QString> protocolTypes {
        {PROTOCOL_UDP, "UDP"},
        {PROTOCOL_TCP, "TCP"}
    };
    return protocolTypes;
}

QHostAddress EthernetConfig::getIpHost() const {
    return ipHost;
}

void EthernetConfig::setIpHost(const QHostAddress &value) {
    ipHost = value;
}

bool EthernetConfig::setIpHost(const QString &ipString) {
    QHostAddress ipTemp;
    bool isValid = ipTemp.setAddress(ipString);
    if(isValid) {
        ipHost = ipTemp;
        return true;
    } else {
        return false;
    }
}

QHostAddress EthernetConfig::getIpClient() const {
    return ipClient;
}

void EthernetConfig::setIpClient(const QHostAddress &value) {
    ipClient = value;
}

bool EthernetConfig::setIpClient(const QString &ipString) {
    QHostAddress ipTemp;
    bool isValid = ipTemp.setAddress(ipString);
    if(isValid) {
        ipClient = ipTemp;
        return true;
    } else {
        return false;
    }
}

int EthernetConfig::getPortClient() const {
    return portClient;
}

void EthernetConfig::setPortClient(int value) {
    portClient = value;
}


EthernetConfig::PROTOCOL EthernetConfig::getProtocol() const {
    return protocol;
}

void EthernetConfig::setProtocol(const PROTOCOL &value) {
    protocol = value;
}

bool EthernetConfig::setProtocol(const QString &protocolString) {
    bool isFound = false;
    if(protocolString.compare("UDP") == 0) {
        protocol = PROTOCOL_UDP;
        isFound = true;
    } else if(protocolString.compare("TCP") == 0) {
        protocol = PROTOCOL_TCP;
        isFound = true;
    }
    return isFound;
}


void EthernetConfig::write(QTextStream& stream) {
    if(protocol == EthernetConfig::PROTOCOL_UDP) {
        stream << "\t" << "/begin " << "XCP_ON_UDP_IP" << "\n";
        writeBody(stream);
        stream << "\t" << "/end " << "XCP_ON_UDP_IP" << "\n";
    } else {
        stream << "\t" << "/begin " << "XCP_ON_TCP_IP" << "\n";
        writeBody(stream);
        stream << "\t" << "/end " << "XCP_ON_TCP_IP" << "\n";
    }
}

void EthernetConfig::writeBody(QTextStream &stream) {
    static const QString VERSION = "0100";
    QString portHex = QString::number(portClient, 16).toUpper();
    QString ipString = ipClient.toString();

    stream << "\t\t" << "0x" << VERSION << "\n";
    stream << "\t\t" << "0x" << portHex << "\n";
    stream << "\t\t" << "ADDRESS" << " \"" << ipString << "\"" << "\n";
}































