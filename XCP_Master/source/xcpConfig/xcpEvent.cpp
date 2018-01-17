/**
*@file xcpEvent.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Data storage class for DAQ events.
*/

#include "xcpEvent.h"

XcpEvent::XcpEvent() {
    name = "dummyEvent";
    channel = 1;
    rateInMs = 10;
}

XcpEvent::XcpEvent(QString name, int channel, int rateInMs)
    : name(name),
      channel(channel),
      rateInMs(rateInMs) {

}

QString XcpEvent::getName() const {
    return name;
}

void XcpEvent::setName(const QString &value) {
    name = value;
}

uint16_t XcpEvent::getChannel() const {
    return channel;
}

void XcpEvent::setChannel(uint16_t value) {
    channel = value;
}

int XcpEvent::getRateInMs() const {
    return rateInMs;
}

void XcpEvent::setRateInMs(int value) {
    rateInMs = value;
}

void XcpEvent::write(QTextStream &stream) {
    static const int NAME_MAX_SIZE = 101;
    static const int NAME_SHORT_MAX_SIZE = 9;
    QString channelName = name;
    chopStringIfTooLong(channelName, NAME_MAX_SIZE);
    QString channelNameShort = name;
    chopStringIfTooLong(channelNameShort, NAME_SHORT_MAX_SIZE);
    QString channelNumberHex = QString::number(channel, 16).toUpper();
    QString maxDaqListHex = "FF";
    QString timeCycleHex, timeUnitHex;
    lookupTimeCycleAndRateForA2l(rateInMs, timeCycleHex, timeUnitHex);
    QString priorityHex = "00";

    stream << "\t\t" << " /begin EVENT" << "\n";
    stream << "\t\t\t" << "\"" << channelName << "\"" << "\n";
    stream << "\t\t\t" << "\"" << channelNameShort << "\"" << "\n";
    stream << "\t\t\t" << "0x" << channelNumberHex << "\n";
    stream << "\t\t\t" << "DAQ" << "\n";
    stream << "\t\t\t" << "0x" << maxDaqListHex << "\n";
    stream << "\t\t\t" << "0x" << timeCycleHex << "\n";
    stream << "\t\t\t" << "0x" << timeUnitHex << "\n";
    stream << "\t\t\t" << "0x" << priorityHex << "\n";
    stream << "\t\t" << " /end EVENT" << "\n";
}

void XcpEvent::chopStringIfTooLong(QString& str, const int maxLength) {
    int size = str.size();
    int difference = size - maxLength;
    if(difference > 0) {
        str.chop(difference);
    }
}

void XcpEvent::lookupTimeCycleAndRateForA2l(const int rateInMs, QString& timeCycle, QString& timeUnit) {
    static const int NON_CYCLIC = 0;
    switch (rateInMs) {
    case NON_CYCLIC:
        timeCycle = "0", timeUnit = "0";
        break;
    case 10:
        timeCycle = "0A", timeUnit = "06";
        break;
    case 20:
        timeCycle = "14", timeUnit = "06";
        break;
    case 40:
        timeCycle = "28", timeUnit = "06";
        break;
    case 60:
        timeCycle = "3C", timeUnit = "06";
        break;
    case 80:
        timeCycle = "50", timeUnit = "06";
        break;
    case 100:
        timeCycle = "64", timeUnit = "06";
        break;
    case 200:
        timeCycle = "C8", timeUnit = "06";
        break;
    case 400:
        timeCycle = "28", timeUnit = "07";
        break;
    case 600:
        timeCycle = "3C", timeUnit = "07";
        break;
    case 800:
        timeCycle = "50", timeUnit = "07";
        break;
    case 1000:
        timeCycle = "64", timeUnit = "07";
        break;
    case 2000:
        timeCycle = "C8", timeUnit = "07";
        break;
    default:
        timeCycle = "0A", timeUnit = "06";
        break;
    }
}
