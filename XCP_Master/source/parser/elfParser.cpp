/**
*@file elfParser.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The ELF file parser.
*/


#include "elfParser.h"
#include "elfInfo.h"

#include "libs/libelfin/elf/elf++.hh"

#include <iostream>
#include <algorithm>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>


ElfParser::ElfParser() {

}

ElfParser::ElfParser(const string& elfFilePathName) : elfFilePathName(elfFilePathName) {

}

void ElfParser::parseElfFile() {

    int elfFile = open(elfFilePathName.data(), O_RDONLY);
    elf::elf f(elf::create_mmap_loader(elfFile));
    for (const auto &sec : f.sections()) {

        auto type = sec.get_hdr().type;
        if (type != elf::sht::symtab && type != elf::sht::dynsym) {
            continue;
        }

        string symbolName;
        for (const auto &sym : sec.as_symtab()) {
            auto &d = sym.get_data();
            symbolName = sym.get_name();

            if((d.type() == elf::stt::object) && (d.size > 0)) {
                ElfInfo elfInfoObj;
                elfInfoObj.setSymbolName(sym.get_name());
                elfInfoObj.setAddr(d.value);
                elfInfoObj.setSize(d.size);
                elfInfos.push_back(elfInfoObj);
            }
        }
    }
}

/**************************************************************
 *  Getter / Setter
 * ***********************************************************/
vector<ElfInfo> ElfParser::getElfInfos() const {
    return elfInfos;
}

void ElfParser::setElfFilePathName(const string &value) {
    elfFilePathName = value;
}

