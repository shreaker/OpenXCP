/**
*@file dwarfTreeObj.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This class represents a DWARF tree object.
*/


#include "dwarfTreeObj.h"

DwarfTreeObj::DwarfTreeObj():addr(0), declLine(0), absType(AbstractType::NotFound) {

}

string DwarfTreeObj::getTag() const {
    return tag;
}

void DwarfTreeObj::setTag(const string &tag) {
    this->tag = tag;
}

string DwarfTreeObj::getAttributeName() const {
    return attributeName;
}

void DwarfTreeObj::setAttributeName(const string &attributeName) {
    this->attributeName = attributeName;
}

uint64_t DwarfTreeObj::getAddr() const {
    return addr;
}

void DwarfTreeObj::setAddr(uint64_t value) {
    addr = value;
}

uint32_t DwarfTreeObj::getDeclLine() const {
    return declLine;
}

void DwarfTreeObj::setDeclLine(const uint32_t &lineNr) {
    declLine = lineNr;
}


string DwarfTreeObj::getStructName() const {
    return structName;
}

void DwarfTreeObj::setStructName(const string &structName) {
    this->structName = structName;
}

string DwarfTreeObj::getSourcePath() const {
    return sourcePath;
}

void DwarfTreeObj::setSourcePath(const string &srcPath) {
    sourcePath = srcPath;
}

AbstractType DwarfTreeObj::getAbsType() const {
    return absType;
}

void DwarfTreeObj::setAbsType(const AbstractType &type) {
    absType = type;
}

std::vector<uint64_t> DwarfTreeObj::getStructElementsMapKey() const {
    return structElementsId;
}

void DwarfTreeObj::setStructElementsMapKey(const std::vector<uint64_t> &keys) {
    structElementsId = keys;
}

void DwarfTreeObj::setStructElementsMapKey(const uint64_t &key) {
    structElementsId.push_back(key);
}

std::pair<string, uint32_t> DwarfTreeObj::getDataType() const {
    return dataType;
}

void DwarfTreeObj::setDataType(const std::pair<string, uint32_t> &dataType) {
    this->dataType = dataType;
}



std::ostream& operator<<(std::ostream& out, const DwarfTreeObj& dwarfTreeObj) {
    string absTypeString;
    if(dwarfTreeObj.absType == AbstractType::NotFound) {
        absTypeString = "NotFound";
    } else if(dwarfTreeObj.absType == AbstractType::Variable) {
        absTypeString = "Variable";
    } else if(dwarfTreeObj.absType == AbstractType::Array) {
        absTypeString = "Array";
    } else if(dwarfTreeObj.absType == AbstractType::Struct) {
        absTypeString = "Struct";
    } else if(dwarfTreeObj.absType == AbstractType::StructMember) {
        absTypeString = "StructMember";
    } else if(dwarfTreeObj.absType == AbstractType::Pointer) {
        absTypeString = "Pointer";
    }

    out << " ABS_TYPE: " << absTypeString  << " TAG: " << dwarfTreeObj.tag <<" VAR: " << dwarfTreeObj.attributeName <<  " ADDR: 0x" << std::hex << dwarfTreeObj.getAddr() << " TYPE: " << dwarfTreeObj.getDataType().first <<  " SIZE: " << std::dec << dwarfTreeObj.getDataType().second << " STRUCT_NAME: " << dwarfTreeObj.structName << " SRC: " << dwarfTreeObj.sourcePath << " LINE: " << std::dec << dwarfTreeObj.declLine <<  std::endl;
    return out;
}



