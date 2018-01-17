/**
*@file sourceparsemeasurement.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This class represents the XCP attributes for MEASUREMENT signals.
*/

#include "sourceparsemeasurement.h"

SourceParseMeasurement::SourceParseMeasurement() {
    isDiscrete = false;
    maxRefreshRate = 10;
    isWriteable = false;
}

SourceParserObj::TypeId SourceParseMeasurement::getTypeId() const {
    return TypeId::Measurement;
}

string SourceParseMeasurement::getTypeString() const {
    return "MEASUREMENT";
}

bool SourceParseMeasurement::getIsDiscrete() const {
    return isDiscrete;
}

void SourceParseMeasurement::setIsDiscrete(bool value) {
    isDiscrete = value;
}

uint32_t SourceParseMeasurement::getMaxRefreshRate() const {
    return maxRefreshRate;
}

void SourceParseMeasurement::setMaxRefreshRate(const uint32_t &value) {
    maxRefreshRate = value;
}

string SourceParseMeasurement::getConversionFunction() const {
    return conversionFunction;
}

void SourceParseMeasurement::setConversionFunction(const string &value) {
    conversionFunction = value;
}

bool SourceParseMeasurement::getIsWriteable() const {
    return isWriteable;
}

void SourceParseMeasurement::setIsWriteable(bool value) {
    isWriteable = value;
}

std::ostream& operator<<(std::ostream &out, const SourceParseMeasurement &measurement) {
    out << "MEASUREMENT" " comment: " << measurement.comment << " lowerLimit: " << measurement.lowerLimit << " upperLimit: " << measurement.upperLimit << " phyUnit: " << measurement.phyUnit << " isDiscrete: "  << measurement.isDiscrete << " maxRefreshRate: " << measurement.maxRefreshRate << " conversionFunc: " << measurement.conversionFunction << " writeable: " << measurement.isWriteable;
    return out;
}
