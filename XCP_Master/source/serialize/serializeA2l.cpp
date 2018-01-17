/**
*@file serializeA2l.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize XCP settings, measurement- and calibration- settings to an A2L file.
*/

#include "serializeA2l.h"

SerializeA2L::SerializeA2L(const QSharedPointer<Model> &model,const QString& saveFile)
    : SerializeBase(model, saveFile) {
}

bool SerializeA2L::save() {
    QFile a2lTemplateFile("://resource/A2L_template.a2l");
    if(!a2lTemplateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Couldn't open A2l template file.");
        return false;
    }

    if(saveFile.exists()) {
        saveFile.remove();
    }
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Couldn't open A2l save file.");
        return false;
    }

    QTextStream streamSaveFile(&saveFile);
    QTextStream streamTemplateFile(&a2lTemplateFile);
    QString line;

    while(streamTemplateFile.readLineInto(&line)) {
        streamSaveFile << line << "\n";
        if(line.contains("/begin IF_DATA XCP")) {
            model->getXcpHostConfig()->write(streamSaveFile);
        } else if(line.contains("/* OpenXCP Ethernet */")) {
            writeEthernetConfig(streamSaveFile);
        } else if(line.contains("/* OpenXCP MEASUREMENT / CHARACTERISTIC */")) {
            writeParseResults(streamSaveFile);
        }
    }

    a2lTemplateFile.close();
    saveFile.close();

    return true;
}

bool SerializeA2L::load() {
    return false; //not implemented
}


void SerializeA2L::writeParseResults(QTextStream& stream) {
    stream << "\n";
    setParseResultToSave();
    for(const auto& data : parseResults) {
        data->write(stream);
    }
}

void SerializeA2L::writeEthernetConfig(QTextStream &stream) {
    if(model->getTransportProtocolSelected() == Model::TRANSPORT_PROTOCOL::ETHERNET) {
        stream << "\n";
        model->getEthernetConfig()->write(stream);
    }

}
















