/**
*@file parseResultBase.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The abstract class ParseResultBase is the base class for the derived classes ParseResultVariable, ParseResultArray and ParseResultStruct. It is used in software for saving parsing results.
*/

#ifndef PARSE_RESULT_BASE_H
#define PARSE_RESULT_BASE_H

#include <string>
#include <memory>

#include "sourceparsemeasurement.h"
#include "sourceparsecharacteristic.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>

using std::string;

class ParseResultBase {
  public:
    ParseResultBase();

    ParseResultBase(const bool isSelectedByUser, const string& name, const uint64_t address, const uint32_t size, const string& dataType, const string& abstractDataType, const string& sourceFilePathName, const uint32_t sourceFileLine);

    virtual void traverse() = 0;

    virtual void write(QJsonObject& json);

    virtual void write(QTextStream& stream) = 0;

    string getName() const;
    void setName(const string &value);

    uint64_t getAddress() const;
    void setAddress(const uint64_t &value);

    uint32_t getSize() const;
    void setSize(const uint32_t &value);

    string getAbstractDataType() const;
    void setAbstractDataType(const string &value);

    string getSourceFilePathName() const;
    void setSourceFilePathName(const string &value);

    uint32_t getSourceFileLine() const;
    void setSourceFileLine(const uint32_t &value);

    bool getIsSelectedByUser() const;
    void setIsSelectedByUser(bool value);

    string getDataType() const;
    void setDataType(const string &value);

    SourceParserObj *getSrcParserObj() const;
    void setSrcParserObj(SourceParserObj *value);

    std::shared_ptr<SourceParseCharacteristic> getSrcCharateristic() const;
    void setSrcCharateristic(const std::shared_ptr<SourceParseCharacteristic> &value);

    std::shared_ptr<SourceParseMeasurement> getSrcMeasurement() const;
    void setSrcMeasurement(const std::shared_ptr<SourceParseMeasurement> &value);

  protected:
    bool isSelectedByUser;
    string name;
    uint64_t address;
    uint32_t size;
    string dataType;
    string abstractDataType;
    string sourceFilePathName;
    uint32_t sourceFileLine;
    std::shared_ptr<SourceParseCharacteristic> srcCharateristic;
    std::shared_ptr<SourceParseMeasurement> srcMeasurement;
    SourceParserObj* srcParserObj;

};

#endif // PARSE_RESULT_BASE_H
