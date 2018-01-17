/**
*@file parseResultStruct.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This classes stores the result of the ELF-, DWARF- and source code comment parser for structs.
*/

#include "parseResultStruct.h"

#include "parseResultVariable.h"
#include "parseResultArray.h"

ParseResultStruct::ParseResultStruct() {

}

void ParseResultStruct::traverse() {
    for(auto& i : elements) {
        i->traverse();
    }
}

void ParseResultStruct::write(QJsonObject& json) {
    for(const auto& element : elements) {
        element->write(json);
    }
}

void ParseResultStruct::write(QTextStream &stream) {
    for(auto& element : elements) {
        element->write(stream);
    }
}


void ParseResultStruct::addElement(std::shared_ptr<ParseResultBase> element) {
    elements.push_back(element);
}

std::vector<std::shared_ptr<ParseResultBase>> ParseResultStruct::getElements() const {
    return elements;
}





