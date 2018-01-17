/**
*@file xcpEvent.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Data storage class for DAQ events.
*/

#ifndef XCPEVENT_H
#define XCPEVENT_H

#include <QString>
#include <QTextStream>

class XcpEvent {
  public:
    XcpEvent();
    XcpEvent(QString name, int channel, int rateInMs);
    QString getName() const;
    void setName(const QString &value);

    uint16_t getChannel() const;
    void setChannel(uint16_t value);

    int getRateInMs() const;
    void setRateInMs(int value);

    void write(QTextStream& stream);

  private:
    void chopStringIfTooLong(QString& str, const int maxLength);
    void lookupTimeCycleAndRateForA2l(const int rateInMs, QString& timeCycle, QString& timeUnit);

  private:
    QString name;
    uint16_t channel;
    int rateInMs;
};

#endif // XCPEVENT_H
