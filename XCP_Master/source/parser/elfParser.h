/**
*@file elfParser.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The ELF file parser.
*/

#ifndef ELFSYMS_H
#define ELFSYMS_H

#include <string>
#include <vector>

#include "elfInfo.h"

using std::string;
using std::vector;


class ElfParser {
  public:
    ElfParser();
    ElfParser(const string& elFilePathName);
    void parseElfFile();
    vector<ElfInfo> getElfInfos() const;

    void setElfFilePathName(const string &value);

  private:
    string elfFilePathName;
    vector<ElfInfo> elfInfos;

};

#endif // ELFSYMS_H
