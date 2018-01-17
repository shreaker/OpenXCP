/**
*@file parseResultBase.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The abstract class ParseResultBase is the base class for the derived classes ParseResultVariable, ParseResultArray and ParseResultStruct. It is used in software for saving parsing results.
*/


#include "parseResultBase.h"

#include <QString>

ParseResultBase::ParseResultBase() {
    isSelectedByUser = false;

    address = 0;
    size = 0;
    sourceFileLine = 0;
    srcCharateristic = nullptr;
    srcMeasurement = nullptr;
    srcParserObj = nullptr;
}



void ParseResultBase::write(QJsonObject& json) {
    json["XcpType"] = QString::fromStdString(srcParserObj->getTypeString());
    json["name"] = QString::fromStdString(name);
    json["long id"] = QString::fromStdString(srcParserObj->getComment());
    json["abstract datatype"] = QString::fromStdString(abstractDataType);
    json["datatype"] = QString::fromStdString(dataType);
    json["size"] = (int) size;
    json["lower limit"] = srcParserObj->getLowerLimit();
    json["upper limit"] = srcParserObj->getUpperLimit();
    json["ECU_ADDRESS"] =  QStringLiteral("0x") + QString::number(address, 16);
    json["PHYS_UNIT"] = QString::fromStdString(srcParserObj->getPhyUnit());
    if(srcMeasurement != nullptr) {
        json["conversion"] = QString::fromStdString(srcMeasurement->getConversionFunction());
        json["READ_WRITE"] = srcMeasurement->getIsWriteable();
        json["MAX_REFRESH"] = (int) srcMeasurement->getMaxRefreshRate();
        json["DISCRETE"] =  srcMeasurement->getIsDiscrete();
    } else if(srcCharateristic != nullptr) {
        json["STEP_SIZE"] = srcCharateristic->getStepSize();
    }

}

string ParseResultBase::getName() const {
    return name;
}

void ParseResultBase::setName(const string &value) {
    name = value;
}

uint64_t ParseResultBase::getAddress() const {
    return address;
}

void ParseResultBase::setAddress(const uint64_t &value) {
    address = value;
}

uint32_t ParseResultBase::getSize() const {
    return size;
}

void ParseResultBase::setSize(const uint32_t &value) {
    size = value;
}

string ParseResultBase::getAbstractDataType() const {
    return abstractDataType;
}

void ParseResultBase::setAbstractDataType(const string &value) {
    abstractDataType = value;
}

string ParseResultBase::getSourceFilePathName() const {
    return sourceFilePathName;
}

void ParseResultBase::setSourceFilePathName(const string &value) {
    sourceFilePathName = value;
}

uint32_t ParseResultBase::getSourceFileLine() const {
    return sourceFileLine;
}

void ParseResultBase::setSourceFileLine(const uint32_t &value) {
    sourceFileLine = value;
}

bool ParseResultBase::getIsSelectedByUser() const {
    return isSelectedByUser;
}

void ParseResultBase::setIsSelectedByUser(bool value) {
    isSelectedByUser = value;
}

string ParseResultBase::getDataType() const {
    return dataType;
}

void ParseResultBase::setDataType(const string &value) {
    dataType = value;
}

SourceParserObj *ParseResultBase::getSrcParserObj() const {
    return srcParserObj;
}

void ParseResultBase::setSrcParserObj(SourceParserObj *value) {
    srcParserObj = value;
}

std::shared_ptr<SourceParseCharacteristic> ParseResultBase::getSrcCharateristic() const {
    return srcCharateristic;
}

void ParseResultBase::setSrcCharateristic(const std::shared_ptr<SourceParseCharacteristic> &value) {
    srcCharateristic = value;
    srcParserObj = value.get();
}

std::shared_ptr<SourceParseMeasurement> ParseResultBase::getSrcMeasurement() const {
    return srcMeasurement;
}

void ParseResultBase::setSrcMeasurement(const std::shared_ptr<SourceParseMeasurement> &value) {
    srcMeasurement = value;
    srcParserObj = value.get();
}



