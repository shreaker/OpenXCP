/**
*@file parseResultVariable.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This classes stores the result of the ELF-, DWARF- and source code comment parser for simple variables.
*/

#include "parseResultVariable.h"

#include <iostream>

#include "serialize/serializeA2lMeasurement.h"
#include "serialize/serializeA2lCharacteristic.h"


ParseResultVariable::ParseResultVariable() {

}

void ParseResultVariable::traverse() {
    std::cout << name << std::endl;
}


void ParseResultVariable::write(QJsonObject& json) {
    ParseResultBase::write(json);
}

void ParseResultVariable::write(QTextStream &stream) {
    if(isSelectedByUser) {
        if(srcParserObj != nullptr) {
            if(srcMeasurement != nullptr) {
                SerializeA2lMeasurement a2l(*this);
                a2l.write(stream);
            } else if(srcCharateristic != nullptr) {
                SerializeA2lCharacteristic a2l(*this);
                a2l.write(stream);
            }
            stream << "\n";
        }
    }
}
