/**
*@file serializeA2lMeasurement.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize measurement- settings to an A2L file.
*/

#include "serializeA2lMeasurement.h"

#include <iostream>

SerializeA2lMeasurement::SerializeA2lMeasurement(const ParseResultBase& var) : SerializeA2lBase(var) {
    conversion = conversrionFunctionToA2lConversion(var.getSrcMeasurement()->getConversionFunction());
    maxRefresh = (int) var.getSrcMeasurement()->getMaxRefreshRate();
    discrete = var.getSrcMeasurement()->getIsDiscrete();
    readWrite = var.getSrcMeasurement()->getIsWriteable();
}

void SerializeA2lMeasurement::write(QTextStream &stream) {
    stream << "\t" << "/begin " << type << " " << ident << " \"" << longidentifier << "\"" <<"\n";
    stream << "\t\t" << datatype << " " << conversion << " 0 0 " << lowerLimit << " " << upperLimit << "\n";
    (readWrite ? (stream << "\t\t" << "READ_WRITE" << "\n") : stream);
    stream << "\t\t" << "ECU_ADDRESS 0x" << ecuAddress << "\n";
    stream << "\t\t" << "ECU_ADDRESS_EXTENSION 0x0" << "\n";
    stream << "\t\t" << "FORMAT \"%.15\"" << "\n";
    stream << "\t\t" << "/begin IF_DATA CANAPE_EXT" << "\n";
    stream << "\t\t\t" << "100" << "\n";
    stream << "\t\t\t" << "LINK_MAP" << " \"" << ident << "\"" << " 0x" << ecuAddress << " 0x0 0 0x0 1 0x0 0x0" << "\n";
    stream << "\t\t\t" << "DISPLAY 0 " << lowerLimit << " " << upperLimit << "\n";
    stream << "\t\t" << "/end IF_DATA" << "\n";
    stream << "\t\t" << "SYMBOL_LINK " << "\"" << ident << "\" " << "0" << "\n";
    stream << "\t\t" << "PHYS_UNIT " << "\"" << physUnit << "\"" << "\n";
    (discrete ? (stream << "\t\t" << "DISCRETE" << "\n") : stream);
    stream << "\t" << "/end " << type << "\n";
}

QString SerializeA2lMeasurement::conversrionFunctionToA2lConversion(std::string conversionFunction) {
    QString ret;
    if(conversionFunction.size() == 0) {
        ret = "NO_COMPU_METHOD";
    } else {
        ret = QString::fromStdString(conversionFunction);
    }
    return ret;
}
