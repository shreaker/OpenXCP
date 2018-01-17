/**
*@file sourceparsecharacteristic.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This class represents the XCP attributes for CHARACTERISTIC parameters.
*/


#include "sourceparsecharacteristic.h"

SourceParseCharacteristic::SourceParseCharacteristic() {
    stepSize = 1;
}

SourceParserObj::TypeId SourceParseCharacteristic::getTypeId() const {
    return TypeId::Characteristic;
}

string SourceParseCharacteristic::getTypeString() const {
    return "CHARACTERISTIC";
}

double SourceParseCharacteristic::getStepSize() const {
    return stepSize;
}

void SourceParseCharacteristic::setStepSize(double value) {
    stepSize = value;
}

std::ostream& operator<<(std::ostream &out, const SourceParseCharacteristic &characteristic) {
    out << "CHARACTERISTIC" << " comment: " << characteristic.comment << " lowerLimit: " << characteristic.lowerLimit << " upperLimit: " << characteristic.upperLimit << " phyUnit: " << characteristic.phyUnit << " stepSize: " << characteristic.stepSize;
    return out;
}
