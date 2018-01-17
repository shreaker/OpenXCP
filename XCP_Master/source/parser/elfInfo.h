/**
*@file elfInfo.h
*@author Michael Wolf
*@date 27.11.2017
*@brief The result of the ELF parser is saved in this data structure.
*/

#ifndef ELFINFO_H
#define ELFINFO_H

#include <string>
#include <ostream>

using std::string;

class ElfInfo {
  public:
    ElfInfo();

    string getSymbolName() const;
    void setSymbolName(const string &symbolName);

    unsigned int getAddr() const;
    void setAddr(unsigned int addr);

    unsigned int getSize() const;
    void setSize(unsigned int size);

    friend std::ostream& operator<<(std::ostream& out, const ElfInfo& elfInfo);


  private:
    string symbolName;
    unsigned int addr;
    unsigned int size;
};

#endif // ELFINFO_H
