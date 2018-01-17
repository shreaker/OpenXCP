/**
*@file parserManager.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Is responsible for managing the three parsers in OpenXCP (ELF-, DWARF- and Source-parser).
*/

#include "parserManager.h"

#ifdef DEBUG
#include <iostream>
#include <fstream>
#endif

using namespace std;

ParserManager::ParserManager(const string& elfFileName, const string& workspaceSourcePath) : workspaceSourcePath(workspaceSourcePath) {
    elfParser.setElfFilePathName(elfFileName);
    dwarfParser.setElfFilePathName(elfFileName);
}

void ParserManager::parse() {
    elfParser.parseElfFile();
    emit sigParseProgress(30);

    dwarfParser.parse();
    emit sigParseProgress(60);

    elfInfos = elfParser.getElfInfos();
    dwarfTree = dwarfParser.getDwarfTree();
#ifdef DEBUG
    writeDwarfTreeToFile();
    emit sigParseProgress(70);
#endif
    addElfDwarfInfoToFound();
    emit sigParseProgress(80);

    for(const auto& var: variables) {
        srcParser.setVariablesToLookUp(var);
    }
    for(const auto& var: arrays) {
        srcParser.setVariablesToLookUp(var);
    }
    for(const auto& var: structs) {
        srcParser.setVariablesToLookUp(var);
    }
    srcParser.parse();
    emit sigParseProgress(90);
}

bool ParserManager::isInLocalWorkspace(const string& path) {
    return (path.find(workspaceSourcePath) != string::npos);
}

void ParserManager::addElfDwarfInfoToFound() {
    for(const auto& dwarfTreeElement : dwarfTree) {
        shared_ptr<DwarfTreeObj> dwarf = dwarfTreeElement.second;
        if(isInLocalWorkspace(dwarf->getSourcePath())) {
            AbstractType absType = dwarf->getAbsType();
            string tag = dwarf->getTag();

            if(tag.compare("DW_TAG_variable") == 0) {
                if(absType == AbstractType::Variable) {
                    addVariableToVariables(*dwarf);
                } else if(absType == AbstractType::Array) {
                    addArrayToArrays(*dwarf);
                } else if(absType == AbstractType::Struct) {
                    addStructToStructs(*dwarf);
                }
            }
        }
    }
}

void ParserManager::addVariableToVariables(const DwarfTreeObj &dwarf) {
    uint64_t addr = dwarf.getAddr();
    if(addr != 0) {
        shared_ptr<ParseResultVariable> foundVar = make_shared<ParseResultVariable>();
        foundVar->setName(dwarf.getAttributeName());
        foundVar->setAddress(dwarf.getAddr());
        foundVar->setAbstractDataType("variable");
        foundVar->setDataType(dwarf.getDataType().first);
        foundVar->setSize(dwarf.getDataType().second);
        foundVar->setSourceFilePathName(dwarf.getSourcePath());
        foundVar->setSourceFileLine(dwarf.getDeclLine());
        variables.push_back(foundVar);
    }
}

void ParserManager::addArrayToArrays(const DwarfTreeObj &dwarf) {
    uint64_t addr = dwarf.getAddr();
    if(addr != 0) {
        shared_ptr<ParseResultArray> foundArray = make_shared<ParseResultArray>();
        string name = dwarf.getAttributeName();
        foundArray->setName(name);
        foundArray->setAddress(addr);
        foundArray->setAbstractDataType("array");
        foundArray->setDataType(dwarf.getDataType().first);
        uint32_t sizePerElement = dwarf.getDataType().second;
        foundArray->setSizePerElement(sizePerElement);
        uint32_t size = resolveArraySize(name);
        foundArray->setSize(size);
        foundArray->setElements(size / sizePerElement);
        foundArray->setSourceFilePathName(dwarf.getSourcePath());
        foundArray->setSourceFileLine(dwarf.getDeclLine());
        arrays.push_back(foundArray);
    }
}

uint32_t ParserManager::resolveArraySize(const string &dwarfArrayName) {
    uint32_t size = 0;
    for(const auto &elf: elfInfos) {
        const string elfName = elf.getSymbolName();
        if(elfName.compare(dwarfArrayName) == 0) {
            size = elf.getSize();
            break;
        }
    }
    return size;
}

void ParserManager::addStructToStructs(const DwarfTreeObj &dwarf) {
    if(dwarf.getDataType().first.size() > 0) {
        int64_t addr = dwarf.getAddr();
        if(addr != 0) {
            shared_ptr<ParseResultStruct> foundStruct = make_shared<ParseResultStruct>();
            foundStruct->setName(dwarf.getAttributeName());
            foundStruct->setAddress(dwarf.getAddr());
            foundStruct->setAbstractDataType("struct");

            structAddrOffset = 0; //reset offset
            resolveStruct(dwarf, foundStruct);

            structs.push_back(foundStruct);
        }
    }
}


void ParserManager::resolveStruct(const DwarfTreeObj &dwarf, shared_ptr<ParseResultStruct>& foundStruct) {
    vector<uint64_t> keys = dwarf.getStructElementsMapKey();
    for(const auto &key : keys) {
        shared_ptr<DwarfTreeObj> dwarfChild = dwarfTree.at(key);
        if(hasChild(*dwarfChild)) {
            resolveStruct(*dwarfChild, foundStruct);
        } else {
            addAddressOfStructMembers(*dwarfChild, foundStruct->getAddress());
            AbstractType absType = dwarfChild->getAbsType();
            if(absType == AbstractType::Variable || absType == AbstractType::Array) {
                addVariabelToStruct(*dwarfChild, absType, foundStruct);
            }
        }
    }
}


void ParserManager::addAddressOfStructMembers(DwarfTreeObj &dwarfStructElement, uint64_t baseAddr) {
    dwarfStructElement.setAddr(baseAddr+structAddrOffset);
    structAddrOffset = structAddrOffset + dwarfStructElement.getDataType().second;
}

bool ParserManager::hasChild(const DwarfTreeObj &dwarf) {
    vector<uint64_t> keys = dwarf.getStructElementsMapKey();
    return (keys.size() > 0);
}


void ParserManager::addVariabelToStruct(const DwarfTreeObj &dwarfStructElement, const AbstractType absType, shared_ptr<ParseResultStruct>& foundStruct) {
    if(absType == AbstractType::Variable) {
        shared_ptr<ParseResultVariable> var = make_shared<ParseResultVariable>();
        var->setAbstractDataType("struct member variable");
        var->setName(dwarfStructElement.getAttributeName());
        var->setDataType(dwarfStructElement.getDataType().first);
        var->setSize(dwarfStructElement.getDataType().second);
        var->setAddress(dwarfStructElement.getAddr());
        var->setSourceFilePathName(dwarfStructElement.getSourcePath());
        var->setSourceFileLine(dwarfStructElement.getDeclLine());
        foundStruct->addElement(var);
    } else if(absType == AbstractType::Array) {
        shared_ptr<ParseResultArray> array = make_shared<ParseResultArray>();
        array->setAbstractDataType("struct member array");
        array->setName(dwarfStructElement.getAttributeName());
        array->setDataType(dwarfStructElement.getDataType().first);
        uint32_t sizePerElement = dwarfStructElement.getDataType().second;
        array->setSizePerElement(sizePerElement);
        uint32_t size = sizePerElement; //TODO resolve toal size, not in elf
        array->setSize(size);
        array->setAddress(dwarfStructElement.getAddr());
        array->setElements(size / sizePerElement);
        array->setSourceFilePathName(dwarfStructElement.getSourcePath());
        array->setSourceFileLine(dwarfStructElement.getDeclLine());
        foundStruct->addElement(array);
    }
}

#ifdef DEBUG
void ParserManager::writeDwarfTreeToFile() {
    ofstream ofs("dwarfTreeParser_debug.log", ofstream::out);
    for (const auto& i: dwarfTree) {
        ofs << *i.second;
    }
    ofs.close();
}
#endif

/**************************************************************
 *  Getter / Setter
 * ***********************************************************/
vector<shared_ptr<ParseResultVariable>> ParserManager::getVariables() const {
    return variables;
}

vector<shared_ptr<ParseResultArray> > ParserManager::getArrays() const {
    return arrays;
}

vector<shared_ptr<ParseResultStruct> > ParserManager::getStructs() const {
    return structs;
}





