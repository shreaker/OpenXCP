/**
*@file sourceParserobj.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This abstract class holds the data which was obtained parsing the source code comments by the source parser.
*/

#include "sourceParserobj.h"

#include <limits>

SourceParserObj::SourceParserObj() {
    lowerLimit = std::numeric_limits<int>::min();
    upperLimit = std::numeric_limits<int>::max();
}

string SourceParserObj::getComment() const {
    return comment;
}

void SourceParserObj::setComment(const string &comment) {
    this->comment = comment;
}

double SourceParserObj::getLowerLimit() const {
    return lowerLimit;
}

void SourceParserObj::setLowerLimit(const double &limit) {
    lowerLimit = limit;
}

double SourceParserObj::getUpperLimit() const {
    return upperLimit;
}

void SourceParserObj::setUpperLimit(const double &limit) {
    upperLimit = limit;
}


string SourceParserObj::getPhyUnit() const {
    return phyUnit;
}

void SourceParserObj::setPhyUnit(const string &phyUnit) {
    this->phyUnit = phyUnit;
}
