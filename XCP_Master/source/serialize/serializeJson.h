/**
*@file serializeJson.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize project settings, XCP settings, measurement- and calibration- settings to an JSON file.
*/

#ifndef SERIALIZEJSON_H
#define SERIALIZEJSON_H

#include "serializeBase.h"
#include "main/model.h"
#include "parser/parseResultBase.h"

#include <QJsonObject>

class SerializeJson : public SerializeBase {
  public:
    SerializeJson(const QSharedPointer<Model>& model, const QString& saveFile);

    bool save() override;
    bool load() override;

  private:
    void write();
    void read();
    void writeFileSettings();
    void writeTransportProtocolConfig();
    void writeEthernetConfig();
    void writeUsbConfig();
    void writeXcpConfig();
    void writeXcpConfigEvents(QJsonObject& jsonXcpConfig);
    void writeParseResult();


    void readFileSettings();
    void readTransportProtocolConfig(Model::TRANSPORT_PROTOCOL transportProtocol);
    void readEthernetConfig();
    void readUsbConfig();
    void readXcpConfig();
    void readXcpConfigEvents(const QJsonArray& eventArray);
    void readParseResults();
    void readParseResultEntry(QJsonObject& jsonObj, shared_ptr<ParseResultBase> parseResult);


  private:
    QJsonObject writeJson;
    QJsonObject readJson;

};

#endif // SERIALIZEJSON_H
