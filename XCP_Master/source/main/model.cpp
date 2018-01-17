/**
*@file model.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The main data-storage class.
*/

#include "model.h"

Model::Model()
    : xcpHostConfig(std::make_shared<XcpHostConfig>(XcpOptions::VERSION_1_0, XcpOptions::ENDIAN_LITTLE, XcpOptions::ADDRESS_GRANULARITY_BYTE, 1000, 8, 8, XcpOptions::DAQ_MODE_DYNAMIC)),
      xcpClientConfig(std::make_shared<XcpClientConfig>()),
      ethernetConfig(std::make_shared<EthernetConfig>(QHostAddress("127.0.0.1"), 5555, QHostAddress("127.0.0.1"), EthernetConfig::PROTOCOL_UDP)),
      transportProtocolSelected(TRANSPORT_PROTOCOL::ETHERNET) {

    parseResults = QVector<shared_ptr<ParseResultBase>>();
    recordElements = QList<shared_ptr<RecordElement>>();
    hashElfFile = QByteArray();

    XcpEvent dummyEvent;
    xcpHostConfig->addEvent(dummyEvent);
}

const std::map<const Model::TRANSPORT_PROTOCOL, const QString> Model::getTransportProtocolsKeyValue() {
    static const std::map<const TRANSPORT_PROTOCOL, const QString> transportProtocols {
        {TRANSPORT_PROTOCOL::ETHERNET, "Ethernet"},
        {TRANSPORT_PROTOCOL::USB, "USB"},
    };
    return transportProtocols;
}

const std::map<const QString, const Model::TRANSPORT_PROTOCOL> Model::getTransportProtocolsValueKey() {
    static const std::map<const QString, const Model::TRANSPORT_PROTOCOL> transportProtocols {
        {"Ethernet", TRANSPORT_PROTOCOL::ETHERNET},
        {"USB", TRANSPORT_PROTOCOL::USB}
    };
    return transportProtocols;
}

QString Model::getFilesProjectFile() const {
    return filesProjectFile;
}

void Model::setFilesProjectFile(const QString& value) {
    filesProjectFile = value;
}

QString Model::getFilesA2lFile() const {
    return filesA2lFile;
}

void Model::setFilesA2lFile(const QString& value) {
    filesA2lFile = value;
}

QString Model::getFilesSourcePath() const {
    return filesSourcePath;
}

void Model::setFilesSourcePath(const QString& value) {
    filesSourcePath = value;
}

QString Model::getFilesElfFile() const {
    return filesElfFile;
}

void Model::setFilesElfFile(const QString& value) {
    filesElfFile = value;
}

QVector<shared_ptr<ParseResultBase>> Model::getParseResults() const {
    return parseResults;
}

void Model::setParseResults(const QVector<shared_ptr<ParseResultBase>>& value) {
    parseResults = value;
}

void Model::addToParseResults(const shared_ptr<ParseResultBase>& value) {
    parseResults.append(value);
}

bool Model::removeFromParseResults(const shared_ptr<ParseResultBase>& value) {
    return parseResults.removeOne(value);
}

void Model::removeAllFromParseResults() {
    parseResults.clear();
}

QList<shared_ptr<RecordElement> > Model::getRecordElements() const {
    return recordElements;
}

void Model::setRecordElements(const QList<shared_ptr<RecordElement>>& value) {
    recordElements = value;
}

void Model::addToRecordElements(const shared_ptr<ParseResultBase>& parseResult) {
    shared_ptr<RecordElement> recordElement = std::make_shared<RecordElement>(parseResult);
    QString dataType = QString::fromStdString(parseResult->getDataType());
    if(dataType.contains("float") || dataType.contains("double")) {
        recordElement->setIsFloat(true);
    }
    if(parseResult->getSrcMeasurement() != nullptr) {
        if(recordElement->getTriggerMode() == RecordElement::TriggerMode::Polling) {
            recordElement->setPollingRateInMs(parseResult->getSrcMeasurement()->getMaxRefreshRate());
        }
    }
    recordElements.append(recordElement);
}

bool Model::removeFromRecordElements(const shared_ptr<ParseResultBase>& parseResult) {
    bool isRemoved = false;
    for(const auto& recordElement: recordElements) {
        if(recordElement->getParseResult() == parseResult) {
            isRemoved = recordElements.removeOne(recordElement);
            break;
        }
    }
    return isRemoved;
}

void Model::removeAllFromRecordElements() {
    recordElements.clear();
}

Model::TRANSPORT_PROTOCOL Model::getTransportProtocolSelected() const {
    return transportProtocolSelected;
}

void Model::setTransportProtocolSelected(const TRANSPORT_PROTOCOL& value) {
    transportProtocolSelected = value;
}

shared_ptr<EthernetConfig> Model::getEthernetConfig() const {
    return ethernetConfig;
}

void Model::setEthernetConfig(const shared_ptr<EthernetConfig>& value) {
    ethernetConfig = value;
}

shared_ptr<XcpHostConfig> Model::getXcpHostConfig() const {
    return xcpHostConfig;
}

void Model::setXcpHostConfig(const shared_ptr<XcpHostConfig>& value) {
    xcpHostConfig = value;
}

QByteArray Model::getHashElfFile() const {
    return hashElfFile;
}

void Model::setHashElfFile(const QByteArray& value) {
    hashElfFile = value;
}

QString Model::getFilesRecordPath() const {
    return filesRecordPath;
}

void Model::setFilesRecordPath(const QString &value) {
    filesRecordPath = value;
}

shared_ptr<XcpClientConfig> Model::getXcpClientConfig() const {
    return xcpClientConfig;
}

void Model::setXcpClientConfig(const shared_ptr<XcpClientConfig>& value) {
    xcpClientConfig = value;
}






