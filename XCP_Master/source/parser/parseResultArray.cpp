/**
*@file parseResultArray.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This classes stores the result of the ELF-, DWARF- and source code comment parser for arrays.
*/

#include "parseResultArray.h"
#include "serialize/serializeA2lMeasurement.h"
#include "serialize/serializeA2lCharacteristic.h"

#include <iostream>


ParseResultArray::ParseResultArray() {
    elements = 0;
    sizePerElement = 0;
}

void ParseResultArray::traverse() {
    std::cout << name << "array: " << size << std::endl;
}


void ParseResultArray::write(QJsonObject& json) {
    ParseResultBase::write(json);
    json["elements"] = elements;
    json["sizePerElement"] = sizePerElement;
}

void ParseResultArray::write(QTextStream &stream) {
    if(isSelectedByUser) {
        if(srcParserObj != nullptr) {
            if(srcMeasurement != nullptr) {
                for(int i = 0; i < elements; i++) {
                    ParseResultVariable arrayElement = createParseResultVariable(i);
                    SerializeA2lMeasurement a2l(arrayElement);
                    a2l.write(stream);
                    stream << "\n";
                }
            } else if(srcCharateristic != nullptr) {
                for(int i = 0; i < elements; i++) {
                    ParseResultVariable arrayElement = createParseResultVariable(i);
                    SerializeA2lCharacteristic a2l(arrayElement);
                    a2l.write(stream);
                    stream << "\n";
                }
            }
        }
    }
}

ParseResultVariable ParseResultArray::createParseResultVariable(int elementNrOfArray) {
    ParseResultVariable var;
    var.setName(name + "._" + std::to_string(elementNrOfArray) + "_");
    var.setAbstractDataType(abstractDataType);
    uint64_t offsetAddress = address + (sizePerElement * elementNrOfArray);
    var.setAddress(offsetAddress);
    var.setDataType(dataType);
    var.setIsSelectedByUser(isSelectedByUser);
    var.setSize(sizePerElement);
    var.setSourceFileLine(sourceFileLine);
    var.setSourceFilePathName(sourceFilePathName);
    var.setSrcCharateristic(srcCharateristic);
    var.setSrcMeasurement(srcMeasurement);
    var.setSrcParserObj(srcParserObj);
    return var;
}

int ParseResultArray::getElements() const {
    return elements;
}

void ParseResultArray::setElements(int value) {
    elements = value;
}

int ParseResultArray::getSizePerElement() const {
    return sizePerElement;
}

void ParseResultArray::setSizePerElement(int value) {
    sizePerElement = value;
}

