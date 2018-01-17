/**
*@file serializea2lcharacteristic.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize calibration- settings to an A2L file.
*/

#include "serializeA2lCharacteristic.h"

#include <iostream>

SerializeA2lCharacteristic::SerializeA2lCharacteristic(const ParseResultBase& var) : SerializeA2lBase(var) {
    stepSize = var.getSrcCharateristic()->getStepSize();
}

void SerializeA2lCharacteristic::write(QTextStream &stream) {
    stream << "\t" << "/begin " << type << " " << ident << " \"" << longidentifier << "\"" <<"\n";
    stream << "\t\t" << "VALUE " << "0x" << ecuAddress << " " << "__" << datatype << "_S" << " 0 " << "NO_COMPU_METHOD " << lowerLimit << " " << upperLimit << "\n";
    stream << "\t\t" << "ECU_ADDRESS_EXTENSION 0x0" << "\n";
    stream << "\t\t" << "EXTENDED_LIMITS " << lowerLimit << " " << upperLimit << "\n";
    stream << "\t\t" << "FORMAT \"%.15\"" << "\n";
    stream << "\t\t" << "/begin IF_DATA CANAPE_EXT" << "\n";
    stream << "\t\t\t" << "100" << "\n";
    stream << "\t\t\t" << "LINK_MAP" << " \"" << ident << "\"" << " 0x" << ecuAddress << " 0x0 0 0x0 1 0x0 0x0" << "\n";
    stream << "\t\t\t" << "DISPLAY 0 " << lowerLimit << " " << upperLimit << "\n";
    stream << "\t\t" << "/end IF_DATA" << "\n";
    stream << "\t\t" << "SYMBOL_LINK " << "\"" << ident << "\" " << "0" << "\n";
    stream << "\t\t" << "PHYS_UNIT " << "\"" << physUnit << "\"" << "\n";
    stream << "\t\t" << "STEP_SIZE " << stepSize << "\n";
    stream << "\t" << "/end " << type << "\n";
}


