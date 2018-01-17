/**
*@file serializeJson.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize project settings, XCP settings, measurement- and calibration- settings to an JSON file.
*/

#include "serializeJson.h"

#include <memory>
#include <iostream>

#include <QJsonArray>
#include <QJsonDocument>

#include "parser/sourceParserobj.h"
#include "parser/sourceparsemeasurement.h"
#include "parser/sourceparsecharacteristic.h"


SerializeJson::SerializeJson(const QSharedPointer<Model>& model,const QString& saveFile)
    : SerializeBase(model, saveFile),
      writeJson(QJsonObject()),
      readJson(QJsonObject()) {

}

bool SerializeJson::load() {
    QFile loadFile(model->getFilesProjectFile());

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open OpenXCP project file (*.json).");
        return false;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    readJson = loadDoc.object();
    read();

    loadFile.close();

    return true;
}


void SerializeJson::read() {
    readFileSettings();
    readXcpConfig();
    readTransportProtocolConfig(model->getTransportProtocolSelected());
    readParseResults();
}

void SerializeJson::readFileSettings() {
    QJsonObject jsonObj = QJsonObject(readJson["files"].toObject());

    model->setFilesProjectFile(jsonObj["project file"].toString());
    model->setFilesElfFile(jsonObj["ELF file"].toString());
    model->setFilesSourcePath(jsonObj["Source path"].toString());
    model->setFilesA2lFile(jsonObj["A2L file"].toString());
    model->setHashElfFile(jsonObj["ELF file hash"].toString().toUtf8());
    model->setFilesRecordPath(jsonObj["Record path"].toString());
}

void SerializeJson::readTransportProtocolConfig(Model::TRANSPORT_PROTOCOL transportProtocol) {
    switch (transportProtocol) {
    case Model::TRANSPORT_PROTOCOL::ETHERNET: {
        readEthernetConfig();
        break;
    }
    case Model::TRANSPORT_PROTOCOL::USB: {
        readUsbConfig();
        break;
    }
    default:
        break;
    }
}

void SerializeJson::readXcpConfig() {
    QJsonObject jsonObj = QJsonObject(readJson["xcp configuration"].toObject());
    shared_ptr<XcpHostConfig> xcpConfig = model->getXcpHostConfig();
    try {
        QString value;
        value = jsonObj["DAQ"].toString();
        xcpConfig->setDaqMode(XcpOptions::getDaqModesValueKey().at(value));
        value = jsonObj["address granularity"].toString();
        xcpConfig->setAddrGranularity(XcpOptions::getAddrGranularitiesValueKey().at(value));
        value = jsonObj["endian"].toString();
        xcpConfig->setEndian(XcpOptions::getEndianTypesValueKey().at(value));
        value = jsonObj["xcp version"].toString();
        xcpConfig->setVersion(XcpOptions::getVersionsValueKey().at(value));
        value = jsonObj["transport protocol"].toString();
        model->setTransportProtocolSelected(Model::getTransportProtocolsValueKey().at(value));
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in readXcpConfig(): " << oor.what() << std::endl;
    }
    xcpConfig->setMaxCto(jsonObj["max CTO"].toInt());
    xcpConfig->setMaxDto(jsonObj["max DTO"].toInt());
    xcpConfig->setTimeout(jsonObj["timeout"].toInt());

    readXcpConfigEvents(jsonObj["events"].toArray());
}

void SerializeJson::readXcpConfigEvents(const QJsonArray &eventArray) {
    model->getXcpHostConfig()->removeAllEvents();
    for(int i = 0; i < eventArray.size(); i++) {
        QJsonObject json = eventArray[i].toObject();
        XcpEvent event = XcpEvent(json["name"].toString(), json["channel"].toInt(), json["rateInMs"].toInt());
        model->getXcpHostConfig()->addEvent(event);
    }
}

void SerializeJson::readParseResults() {
    QJsonArray variableArray = readJson["variables"].toArray();

    for(int i = 0; i < variableArray.size(); i++) {
        QJsonObject jsonObj = variableArray[i].toObject();

        QString abstractDataType = jsonObj["abstract datatype"].toString();
        if((abstractDataType.compare("array") == 0) || (abstractDataType.compare("struct member array") == 0)) {
            shared_ptr<ParseResultArray> parseResultArray = std::make_shared<ParseResultArray>();
            parseResultArray->setElements(jsonObj["elements"].toInt());
            parseResultArray->setSizePerElement(jsonObj["sizePerElement"].toInt());
            readParseResultEntry(jsonObj, parseResultArray);
            model->addToParseResults(parseResultArray);
        } else {
            shared_ptr<ParseResultVariable> parseResultVariable = std::make_shared<ParseResultVariable>();
            readParseResultEntry(jsonObj, parseResultVariable);
            model->addToParseResults(parseResultVariable);
        }
    }
}

void SerializeJson::readParseResultEntry(QJsonObject& jsonObj, shared_ptr<ParseResultBase> parseResult) {
    QString xcpType = jsonObj["XcpType"].toString();
    if(xcpType.compare("MEASUREMENT") == 0) {
        shared_ptr<SourceParseMeasurement> measurement = std::make_shared<SourceParseMeasurement>();
        measurement->setConversionFunction(jsonObj["conversion"].toString().toStdString());
        measurement->setIsWriteable(jsonObj["READ_WRITE"].toBool());
        measurement->setMaxRefreshRate(jsonObj["MAX_REFRESH"].toInt());
        measurement->setIsDiscrete(jsonObj["DISCRETE"].toBool());
        parseResult->setSrcMeasurement(measurement);
    } else if(xcpType.compare("CHARACTERISTIC") == 0) {
        shared_ptr<SourceParseCharacteristic> characteristic = std::make_shared<SourceParseCharacteristic>();
        characteristic->setStepSize(jsonObj["STEP_SIZE"].toDouble());
        parseResult->setSrcCharateristic(characteristic);
    }

    SourceParserObj* srcParserObj = parseResult->getSrcParserObj();
    if(srcParserObj != nullptr) {
        srcParserObj->setComment(jsonObj["long id"].toString().toStdString());
        srcParserObj->setLowerLimit(jsonObj["lower limit"].toDouble());
        srcParserObj->setUpperLimit(jsonObj["upper limit"].toDouble());
        srcParserObj->setPhyUnit(jsonObj["PHYS_UNIT"].toString().toStdString());
    }

    parseResult->setName(jsonObj["name"].toString().toStdString());
    parseResult->setAbstractDataType(jsonObj["abstract datatype"].toString().toStdString());
    parseResult->setDataType(jsonObj["datatype"].toString().toStdString());
    parseResult->setSize(jsonObj["size"].toInt());
    QString hexString = jsonObj["ECU_ADDRESS"].toString().remove(0,2);
    uint64_t address = (uint64_t) strtoul(hexString.toStdString().c_str(), nullptr, 16);
    parseResult->setAddress(address);
    parseResult->setIsSelectedByUser(true);
}

void SerializeJson::readEthernetConfig() {
    QJsonObject jsonObj = QJsonObject(readJson["ethernet"].toObject());

    shared_ptr<EthernetConfig> ethernet = model->getEthernetConfig();
    ethernet->setIpHost(jsonObj["ip host"].toString());
    ethernet->setIpClient(jsonObj["ip client"].toString());
    ethernet->setPortClient(jsonObj["port client"].toInt());
    ethernet->setProtocol(jsonObj["protocol"].toString());
}

void SerializeJson::readUsbConfig() {

}


bool SerializeJson::save() {
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open OpenXCP project file (*.json).");
        return false;
    }

    write();
    QJsonDocument saveDoc(writeJson);
    saveFile.write(saveDoc.toJson());

    saveFile.close();

    return true;
}

void SerializeJson::write() {
    writeFileSettings();
    writeTransportProtocolConfig();
    writeXcpConfig();
    writeParseResult();
}

void SerializeJson::writeFileSettings() {
    QJsonObject jsonObj;
    jsonObj["project file"] = model->getFilesProjectFile();
    jsonObj["ELF file"] = model->getFilesElfFile();
    jsonObj["Source path"] = model->getFilesSourcePath();
    jsonObj["A2L file"] = model->getFilesA2lFile();
    jsonObj["ELF file hash"] = QString::fromUtf8(model->getHashElfFile());
    jsonObj["Record path"] = model->getFilesRecordPath();
    writeJson["files"] = jsonObj;
}

void SerializeJson::writeTransportProtocolConfig() {
    switch (model->getTransportProtocolSelected()) {
    case Model::TRANSPORT_PROTOCOL::ETHERNET:
        writeEthernetConfig();
        break;
    case Model::TRANSPORT_PROTOCOL::USB:
        writeUsbConfig();
        break;
    default:
        break;
    }
}

void SerializeJson::writeEthernetConfig() {
    QJsonObject jsonObj;
    jsonObj["ip host"] = model->getEthernetConfig()->getIpHost().toString();
    jsonObj["ip client"] = model->getEthernetConfig()->getIpClient().toString();
    jsonObj["port client"] = model->getEthernetConfig()->getPortClient();
    try {
        jsonObj["protocol"] = EthernetConfig::getProtocolTypes().at(model->getEthernetConfig()->getProtocol());
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in writeEthernetConfig(): " << oor.what() << std::endl;
    }
    writeJson["ethernet"] = jsonObj;
}

void SerializeJson::writeUsbConfig() {
    QJsonObject jsonObj;
    writeJson["usb"] = jsonObj;
}

void SerializeJson::writeXcpConfig() {
    shared_ptr<XcpHostConfig> xcpConfig = model->getXcpHostConfig();
    QJsonObject jsonObj;
    try {
        jsonObj["xcp version"] = XcpOptions::getVersionsKeyValue().at(xcpConfig->getVersion());
        jsonObj["transport protocol"] = Model::getTransportProtocolsKeyValue().at(model->getTransportProtocolSelected());
        jsonObj["endian"] = XcpOptions::getEndianTypesKeyValue().at(xcpConfig->getEndian());
        jsonObj["address granularity"] = XcpOptions::getAddrGranularitiesKeyValue().at(xcpConfig->getAddrGranularity());
        jsonObj["DAQ"] = XcpOptions::getDaqModesKeyValue().at(xcpConfig->getDaqMode());
    } catch (const std::out_of_range& oor) {
        std::cerr << "Out of Range error in writeXcpConfig(): " << oor.what() << '\n';
    }
    jsonObj["timeout"] = xcpConfig->getTimeout();
    jsonObj["max CTO"] = xcpConfig->getMaxCto();
    jsonObj["max DTO"] = xcpConfig->getMaxDto();

    writeXcpConfigEvents(jsonObj);

    writeJson["xcp configuration"] = jsonObj;
}

void SerializeJson::writeXcpConfigEvents(QJsonObject &jsonXcpConfig) {
    QJsonArray jsonArray;
    for(const auto& event : model->getXcpHostConfig()->getEvents()) {
        QJsonObject jsonObj;
        jsonObj["name"] = event.getName();
        jsonObj["channel"] = event.getChannel();
        jsonObj["rateInMs"] = event.getRateInMs();
        jsonArray.append(jsonObj);
    }
    jsonXcpConfig["events"] = jsonArray;
}

void SerializeJson::writeParseResult() {
    setParseResultToSave();
    QJsonArray parseResultArray;
    for(const auto& data : parseResults) {
        if(data->getIsSelectedByUser() && (data->getSrcParserObj() != nullptr)) {
            QJsonObject jsonObj;
            data->write(jsonObj);
            parseResultArray.append(jsonObj);
        }
        writeJson["variables"] = parseResultArray;
    }
}




