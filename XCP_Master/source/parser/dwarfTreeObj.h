/**
*@file dwarfTreeObj.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This class represents a DWARF tree object.
*/

#ifndef DWARFTREEOBJ_H
#define DWARFTREEOBJ_H

#include <string>
#include <ostream>
#include <vector>

using std::string;
using std::vector;

enum class AbstractType {NotFound, Variable, Array, Struct, StructMember, Pointer};


class DwarfTreeObj {
  public:
    DwarfTreeObj();

    string getTag() const;
    void setTag(const string &tag);

    string getAttributeName() const;
    void setAttributeName(const string &attributeName);

    uint64_t getAddr() const;
    void setAddr(uint64_t value);

    uint32_t getDeclLine() const;
    void setDeclLine(const uint32_t &lineNr);

    std::pair<string, uint32_t> getDataType() const;
    void setDataType(const std::pair<string, uint32_t> &dataType);

    string getStructName() const;
    void setStructName(const string &structName);

    string getSourcePath() const;
    void setSourcePath(const string &srcPath);

    AbstractType getAbsType() const;
    void setAbsType(const AbstractType &type);

    std::vector<uint64_t> getStructElementsMapKey() const;
    void setStructElementsMapKey(const std::vector<uint64_t> &keys);
    void setStructElementsMapKey(const uint64_t &key);

    friend std::ostream& operator<<(std::ostream& out, const DwarfTreeObj& dwarfTreeObj);


  private:
    string tag;
    string attributeName;
    uint64_t addr;
    uint32_t declLine;
    std::pair <string, uint32_t> dataType;
    AbstractType absType;
    string structName;
    std::vector<uint64_t> structElementsId;
    string sourcePath;

};


#endif // DWARFTREEOBJ_H
