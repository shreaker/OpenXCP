/**
*@file elfInfo.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The result of the ELF parser is saved in this data structure.
*/

#include "elfInfo.h"

ElfInfo::ElfInfo():addr(0),size(0) {

}

string ElfInfo::getSymbolName() const {
    return symbolName;
}

void ElfInfo::setSymbolName(const string &symbolName) {
    this->symbolName = symbolName;
}

unsigned int ElfInfo::getAddr() const {
    return addr;
}

void ElfInfo::setAddr(unsigned int addr) {
    this->addr = addr;
}

unsigned int ElfInfo::getSize() const {
    return size;
}

void ElfInfo::setSize(unsigned int size) {
    this->size = size;
}

std::ostream& operator<<(std::ostream& out, const ElfInfo& elfInfo) {
    out << " " << elfInfo.getSymbolName() << " "  << std::dec << elfInfo.getSize() << " " << "0x" << std::hex << elfInfo.getAddr() << std::endl;
    return out;
}


