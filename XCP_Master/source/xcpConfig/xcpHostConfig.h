/**
*@file xcpHostConfig.h
*@author Michael Wolf
*@date 27.11.2017
*@brief XcpHostConfig class is responsible for holding the XCP settings.
*/

#ifndef XCPHOSTCONFIG_H
#define XCPHOSTCONFIG_H

#include <QString>
#include <QVector>
#include <map>
#include <QTextStream>

#include "xcpConfig/xcpEvent.h"
#include "xcpConfig/xcpOptions.h"

class XcpHostConfig {
  public:
    XcpHostConfig(XcpOptions::VERSION version,
                  XcpOptions::ENDIAN endian,
                  XcpOptions::ADDRESS_GRANULARITY addrGranularity,
                  int timeout,
                  int maxCto,
                  int maxDto,
                  XcpOptions::DAQ_MODE daqMode,
                  QVector<XcpEvent> events = QVector<XcpEvent>());

    XcpOptions::VERSION getVersion() const;
    void setVersion(const XcpOptions::VERSION &value);

    XcpOptions::ENDIAN getEndian() const;
    void setEndian(const XcpOptions::ENDIAN &value);

    XcpOptions::ADDRESS_GRANULARITY getAddrGranularity() const;
    void setAddrGranularity(const XcpOptions::ADDRESS_GRANULARITY &value);

    XcpOptions::DAQ_MODE getDaqMode() const;
    void setDaqMode(const XcpOptions::DAQ_MODE &value);

    int getTimeout() const;
    void setTimeout(int value);

    int getMaxCto() const;
    void setMaxCto(int value);

    int getMaxDto() const;
    void setMaxDto(int value);

    QVector<XcpEvent> getEvents() const;
    void setEvents(const QVector<XcpEvent> &value);
    void addEvent(const XcpEvent& event);
    bool removeEvent(const QString& eventName);
    void removeAllEvents();

    void write(QTextStream& stream);

  private:
    void writeProtocolLayer(QTextStream& stream) ;
    void writeDAQ(QTextStream& stream);

  private:
    XcpOptions::VERSION version;
    XcpOptions::ENDIAN endian;
    XcpOptions::ADDRESS_GRANULARITY addrGranularity;
    XcpOptions::DAQ_MODE daqMode;
    int timeout;
    int maxCto;
    int maxDto;
    QVector<XcpEvent> events;


};

#endif // XCPHOSTCONFIG_H
