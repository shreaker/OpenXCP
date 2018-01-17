/**
*@file parserManager.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Is responsible for managing the three parsers in OpenXCP (ELF-, DWARF- and Source-parser).
*/

#ifndef PARSERMANAGER_H
#define PARSERMANAGER_H

#include "elfParser.h"
#include "dwarfParser.h"
#include "sourceParser.h"

#include "parseResultVariable.h"
#include "parseResultArray.h"
#include "parseResultStruct.h"

#include <string>
#include <memory>

using std::string;
using std::vector;
using std::map;
using std::shared_ptr;

class ParserManager : public QObject {

    Q_OBJECT

  public:
    ParserManager(const string& elfFileName, const string& workspaceSourcePath);
    void parse();

    vector<shared_ptr<ParseResultVariable>> getVariables() const;
    vector<shared_ptr<ParseResultArray>> getArrays() const;
    vector<shared_ptr<ParseResultStruct>> getStructs() const;

  signals:
    void sigParseProgress(int percent);


  private:
    string elfFileName;
    string workspaceSourcePath;

    ElfParser elfParser;
    DwarfParser dwarfParser;
    SourceParser srcParser;

    vector<ElfInfo> elfInfos;
    map<uint64_t, shared_ptr<DwarfTreeObj>> dwarfTree;


    vector<shared_ptr<ParseResultVariable>> variables;
    vector<shared_ptr<ParseResultArray>> arrays;
    vector<shared_ptr<ParseResultStruct>> structs;

    uint32_t structAddrOffset;

    void addElfDwarfInfoToFound();

    void addVariableToVariables(const DwarfTreeObj& dwarf);

    void addArrayToArrays(const DwarfTreeObj& dwarf);
    uint32_t resolveArraySize(const string& dwarfArrayName);

    void addStructToStructs(const DwarfTreeObj& dwarf);
    void addVariabelToStruct(const DwarfTreeObj& dwarfStructElement, const AbstractType absType, shared_ptr<ParseResultStruct>& foundStruct);
    void resolveStruct(const DwarfTreeObj& dwarf, shared_ptr<ParseResultStruct>& foundStruct);
    void addAddressOfStructMembers(DwarfTreeObj& dwarfStructElement, uint64_t baseAddr);

    bool isInLocalWorkspace(const string& path);

    bool hasChild(const DwarfTreeObj& dwarf);

#ifdef DEBUG
    void writeDwarfTreeToFile();
#endif
};

#endif // PARSERMANAGEMENT_H
