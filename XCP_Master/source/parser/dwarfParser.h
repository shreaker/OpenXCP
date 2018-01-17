/**
*@file dwarfParser.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The DWARF tree parser.
*/

#ifndef DWARFPARSER_H
#define DWARFPARSER_H

#include "libs/libelfin/dwarf/dwarf++.hh"
#include "dwarfTreeObj.h"

#include <string>
#include <memory>

using std::string;
using std::map;
using std::shared_ptr;

class DwarfParser {
  public:
    DwarfParser();
    DwarfParser(const string& elfFilePathName);
    void parse();

    map<uint64_t, shared_ptr<DwarfTreeObj>> getDwarfTree() const;

    void setElfFilePathName(const string &value);

  private:
    string elfFilePathName;
    map<uint64_t,shared_ptr<DwarfTreeObj>> dwarfTree;
    string srcPathCompileUnitLast;
    string structNameLast;
    static const int STRUCT_MEMBER_SEARCH_DEPTH_MAX = 3;

    void resolveVariables(const dwarf::die& node, bool isGlobalVariable);

    bool isVariableLocationValid(const dwarf::die& node);
    bool isParentNodeSubProgramm(const dwarf::die& node);

    void addInfoToDwarfTreeObj(const dwarf::die& node, DwarfTreeObj& dwarfTreeObj);
    void addDwarfTreeObjToMap(uint64_t key, shared_ptr<DwarfTreeObj> dwarfTreeObj);

    AbstractType lookupAbstractTypeForVariable(const dwarf::die& node);
    std::pair<string, uint32_t> lookupDataType(const dwarf::die& node);
    void lookupSrcPathCompileUnit(const dwarf::die& node);
    void lookupStructs(const dwarf::die& node);
    uint64_t lookupAddrOfVariable(const dwarf::die& node, AbstractType absType);
    void lookupStructElementsInDwarfTree(DwarfTreeObj& dwarf, uint64_t dwarfKey, int depth);
    void addStructElementsToStruct(DwarfTreeObj& dwarfStruct,vector<uint64_t>& dwarfStructKeys);

    uint32_t stringHexToDec(const string& hexString);
};



#endif // DWARFPARSER_H
