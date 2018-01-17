/**
*@file ethernetconfig.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The EthernetConfig class is used to save the users transport and network layer settings.
*/

#ifndef ETHERNETCONFIG_H
#define ETHERNETCONFIG_H

#include <QHostAddress>
#include <QString>
#include <map>

class EthernetConfig {
  public:
    enum PROTOCOL {PROTOCOL_UDP, PROTOCOL_TCP};

    EthernetConfig(QHostAddress ipClient, int portClient, QHostAddress ipHost, PROTOCOL protocol);

    static const std::map<const EthernetConfig::PROTOCOL, const QString> getProtocolTypes();

    QHostAddress getIpHost() const;
    void setIpHost(const QHostAddress& value);
    bool setIpHost(const QString& ipString);

    QHostAddress getIpClient() const;
    void setIpClient(const QHostAddress& value);
    bool setIpClient(const QString& ipString);

    int getPortClient() const;
    void setPortClient(int value);


    PROTOCOL getProtocol() const;
    void setProtocol(const PROTOCOL &value);
    bool setProtocol(const QString& protocolString);

    void write(QTextStream& stream);

  private:
    QHostAddress ipHost;
    QHostAddress ipClient;
    int portClient;
    PROTOCOL protocol;

  private:
    void writeBody(QTextStream& stream);

};

#endif // ETHERNETCONFIG_H
