/**
*@file xcpHostConfig.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief XcpHostConfig class is responsible for holding the XCP settings.
*/

#include "xcpHostConfig.h"

#include "iostream"

XcpHostConfig::XcpHostConfig(XcpOptions::VERSION version,
                             XcpOptions::ENDIAN endian,
                             XcpOptions::ADDRESS_GRANULARITY addrGranularity,
                             int timeout,
                             int maxCto,
                             int maxDto,
                             XcpOptions::DAQ_MODE daqMode,
                             QVector<XcpEvent> events)
    : version(version),
      endian(endian),
      addrGranularity(addrGranularity),
      timeout(timeout),
      maxCto(maxCto),
      maxDto(maxDto),
      daqMode(daqMode),
      events(events) {

}

XcpOptions::VERSION XcpHostConfig::getVersion() const {
    return version;
}

void XcpHostConfig::setVersion(const XcpOptions::VERSION &value) {
    version = value;
}

XcpOptions::ENDIAN XcpHostConfig::getEndian() const {
    return endian;
}

void XcpHostConfig::setEndian(const XcpOptions::ENDIAN &value) {
    endian = value;
}

XcpOptions::ADDRESS_GRANULARITY XcpHostConfig::getAddrGranularity() const {
    return addrGranularity;
}

void XcpHostConfig::setAddrGranularity(const XcpOptions::ADDRESS_GRANULARITY &value) {
    addrGranularity = value;
}

XcpOptions::DAQ_MODE XcpHostConfig::getDaqMode() const {
    return daqMode;
}

void XcpHostConfig::setDaqMode(const XcpOptions::DAQ_MODE &value) {
    daqMode = value;
}

int XcpHostConfig::getTimeout() const {
    return timeout;
}

void XcpHostConfig::setTimeout(int value) {
    timeout = value;
}

int XcpHostConfig::getMaxCto() const {
    return maxCto;
}

void XcpHostConfig::setMaxCto(int value) {
    maxCto = value;
}

int XcpHostConfig::getMaxDto() const {
    return maxDto;
}

void XcpHostConfig::setMaxDto(int value) {
    maxDto = value;
}

QVector<XcpEvent> XcpHostConfig::getEvents() const {
    return events;
}

void XcpHostConfig::setEvents(const QVector<XcpEvent> &value) {
    events = value;
}

void XcpHostConfig::addEvent(const XcpEvent& event) {
    events.append(event);
}

bool  XcpHostConfig::removeEvent(const QString &eventName) {
    bool isRemoved = false;
    for(int i = 0; i < events.size(); i++) {
        bool isEqual = !QString::compare(events.at(i).getName(), eventName, Qt::CaseInsensitive);
        if(isEqual) {
            events.remove(i);
            isRemoved = true;
            break;
        }
    }
    return isRemoved;
}

void XcpHostConfig::removeAllEvents() {
    events.clear();
}


void XcpHostConfig::write(QTextStream &stream) {
    writeProtocolLayer(stream);
    writeDAQ(stream);
}

void XcpHostConfig::writeProtocolLayer(QTextStream &stream) {
    QString versionHexPseudo;
    switch (version) {
    case XcpOptions::VERSION_1_0:
        versionHexPseudo = "0100";
        break;
    case XcpOptions::VERSION_1_1:
        versionHexPseudo = "0101";
        break;
    case XcpOptions::VERSION_1_2:
        versionHexPseudo = "0102";
        break;
    case XcpOptions::VERSION_1_3:
        versionHexPseudo = "0103";
        break;
    default:
        versionHexPseudo = "0100";
        break;
    }
    QString timeoutHex = QString::number(timeout, 16).toUpper();
    QString maxCtoHex = QString::number(maxCto, 16).toUpper();
    QString maxDtoHex = QString::number(maxDto, 16).toUpper();
    QString byteOrder = (endian == XcpOptions::ENDIAN::ENDIAN_LITTLE ? "BYTE_ORDER_MSB_LAST" : "BYTE_ORDER_MSB_FIRST");
    QString addressGranularity;
    switch (addrGranularity) {
    case XcpOptions::ADDRESS_GRANULARITY_BYTE:
        addressGranularity = "ADDRESS_GRANULARITY_BYTE";
        break;
    case XcpOptions::ADDRESS_GRANULARITY_WORD:
        addressGranularity = "ADDRESS_GRANULARITY_WORD";
        break;
    case XcpOptions::ADDRESS_GRANULARITY_DWORD:
        addressGranularity = "ADDRESS_GRANULARITY_DWORD";
        break;
    default:
        addressGranularity = "ADDRESS_GRANULARITY_BYTE";
        break;
    }

    stream << "\t\t" << "/begin PROTOCOL_LAYER" << "\n";
    stream << "\t\t\t" << "0x" << versionHexPseudo << "\n";
    stream << "\t\t\t" << "0x" << timeoutHex << "\n";
    stream << "\t\t\t" << "0x2710" << "\n";
    stream << "\t\t\t" << "0x00" << "\n";
    stream << "\t\t\t" << "0x00" << "\n";
    stream << "\t\t\t" << "0x00" << "\n";
    stream << "\t\t\t" << "0x00" << "\n";
    stream << "\t\t\t" << "0x00" << "\n";
    stream << "\t\t\t" << "0x" << maxCtoHex << "\n";
    stream << "\t\t\t" << "0x" << maxDtoHex << "\n";
    stream << "\t\t\t" << byteOrder << "\n";
    stream << "\t\t\t" << addressGranularity << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD ALLOC_ODT_ENTRY" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD ALLOC_ODT" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD ALLOC_DAQ" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD FREE_DAQ" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD GET_DAQ_RESOLUTION_INFO" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD GET_DAQ_PROCESSOR_INFO" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD START_STOP_SYNCH" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD START_STOP_DAQ_LIST" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD GET_DAQ_LIST_MODE" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD SET_DAQ_LIST_MODE" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD WRITE_DAQ" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD SET_DAQ_PTR" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD CLEAR_DAQ_LIST" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD DOWNLOAD" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD SHORT_UPLOAD" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD UPLOAD" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD SET_MTA" << "\n";
    stream << "\t\t\t" << "OPTIONAL_CMD GET_COMM_MODE_INFO" << "\n";
    stream << "\t\t" << "/end PROTOCOL_LAYER" << "\n";
}

void XcpHostConfig::writeDAQ(QTextStream &stream) {
    QString daqConfig;
    try {
        daqConfig = XcpOptions::getDaqModesKeyValue().at(daqMode).toUpper();
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in XcpConfig::writeDAQ: " << oor.what() << std::endl;
    }
    QString maxDaqHex = "00";
    QString maxEventHex = QString::number(events.size(), 16).toUpper();
    QString minDaqHex = "00";
    QString maxOdtEntrySizeDaqHex ="07";

    stream << "\t\t" << "/begin DAQ" << "\n";
    stream << "\t\t\t" << daqConfig << "\n";
    stream << "\t\t\t" << "0x" << maxDaqHex << "\n";
    stream << "\t\t\t" << "0x" << maxEventHex << "\n";
    stream << "\t\t\t" << "0x" << minDaqHex << "\n";
    stream << "\t\t\t" << "OPTIMISATION_TYPE_DEFAULT" << "\n";
    stream << "\t\t\t" << "ADDRESS_EXTENSION_FREE" << "\n";
    stream << "\t\t\t" << "IDENTIFICATION_FIELD_TYPE_ABSOLUTE" << "\n";
    stream << "\t\t\t" << "GRANULARITY_ODT_ENTRY_SIZE_DAQ_BYTE" << "\n";
    stream << "\t\t\t" << "0x" << maxOdtEntrySizeDaqHex << "\n";
    stream << "\t\t\t" << "OVERLOAD_INDICATION_PID" << "\n";
    stream << "\t\t\t" << "PRESCALER_SUPPORTED" << "\n";

    for(auto& event : events) {
        event.write(stream);
    }

    stream << "\t\t" << "/end DAQ" << "\n";
}




























