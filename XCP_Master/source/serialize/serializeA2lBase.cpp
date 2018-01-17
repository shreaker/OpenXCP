/**
*@file SerializeA2lBase.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize XCP settings, measurement- and calibration- settings to an A2L file. Abstact base class.
*/

#include "SerializeA2lBase.h"

#include <iostream>
#include <stdexcept>

SerializeA2lBase::SerializeA2lBase(const ParseResultBase& var) {
    type = QString::fromStdString(var.getSrcParserObj()->getTypeString());
    ident = QString::fromStdString(var.getName());
    longidentifier = QString::fromStdString(var.getSrcParserObj()->getComment());
    datatype = datatypeToA2lDatatype(var.getDataType());
    lowerLimit = var.getSrcParserObj()->getLowerLimit();
    upperLimit = var.getSrcParserObj()->getUpperLimit();
    ecuAddress = QString::number(var.getAddress(), 16).toUpper();
    physUnit = QString::fromStdString(var.getSrcParserObj()->getPhyUnit());
}

QString SerializeA2lBase::datatypeToA2lDatatype(std::string datatype) {
    QString ret = "";
    try {
        ret = a2lDatatypes.at(datatype);
    } catch (const std::out_of_range& oor) {
        std::cerr << "a2lDatatypes: Out of Range error: " << oor.what() << '\n';
    }
    return ret;
}
