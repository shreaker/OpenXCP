/**
*@file dwarfParser.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief The DWARF tree parser.
*/

#include "dwarfParser.h"
#include "dwarfTreeObj.h"
#include "libs/libelfin/elf/elf++.hh"

#include <fcntl.h>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include <vector>

#include <unordered_set>


DwarfParser::DwarfParser() {

}

DwarfParser::DwarfParser(const string& elfFilePathName) : elfFilePathName(elfFilePathName) {
}


void DwarfParser::parse() {
    int elfFile = open(elfFilePathName.data(), O_RDONLY);
    elf::elf ef(elf::create_mmap_loader(elfFile));
    dwarf::dwarf dw(dwarf::elf::create_loader(ef));

    for (const auto& cu : dw.compilation_units()) {
        resolveVariables(cu.root(), true);
    }


    for(auto &dwarfTreeElement : dwarfTree) {
        lookupStructElementsInDwarfTree(*dwarfTreeElement.second, dwarfTreeElement.first, 0);
    }
}

void DwarfParser::resolveVariables(const dwarf::die& node, bool isParentNodeSubprogram) {
    uint64_t sectionOffset = node.get_section_offset();

    if(node.tag == dwarf::DW_TAG::subprogram) {
        isParentNodeSubprogram = true;
    } else if(node.tag == dwarf::DW_TAG::compile_unit) {
        isParentNodeSubprogram = false;
    }


    if(!isParentNodeSubprogram) { //ignore local variables from methods/functions
        if(node.tag == dwarf::DW_TAG::compile_unit) {
            lookupSrcPathCompileUnit(node);
        } else if(node.tag == dwarf::DW_TAG::structure_type) {
            lookupStructs(node);
        } else if((node.tag == dwarf::DW_TAG::variable) || (node.tag == dwarf::DW_TAG::member)) {
            if(isVariableLocationValid(node) || (node.tag == dwarf::DW_TAG::member)) {  //@TODO refactor code (gcc 4.6 and gcc 4.9 compliance)
                shared_ptr<DwarfTreeObj> dwarfTreeObj = std::make_shared<DwarfTreeObj>();
                dwarfTreeObj->setTag(to_string(node.tag));
                addInfoToDwarfTreeObj(node, *dwarfTreeObj);
                addDwarfTreeObjToMap(sectionOffset, dwarfTreeObj);
            }
        }
    }

    for (const auto& child : node) {
        resolveVariables(child, isParentNodeSubprogram);
    }
}

bool DwarfParser::isVariableLocationValid(const dwarf::die &node) {
    static const dwarf::value::type exprLoc = dwarf::value::type::exprloc;

    dwarf::value::type valueVariable = node.resolve(dwarf::DW_AT::location).get_type();
    dwarf::value::type valueStruct = node.resolve(dwarf::DW_AT::data_member_location).get_type();
    return (valueVariable == exprLoc || valueStruct == exprLoc) ? true : false;
}



void DwarfParser::lookupSrcPathCompileUnit(const dwarf::die& node) {
    string dir, src;
    for (const auto &attr : node.attributes()) {
        dwarf::DW_AT attributeName = attr.first;
        string value = to_string(attr.second);

        if(attributeName == dwarf::DW_AT::name) {
            src = value;
        }
        if(attributeName == dwarf::DW_AT::comp_dir) {
            dir = value;
        }
    }
    std::stringstream ss;
    ss << std::regex_replace(dir, std::regex(R"(\\+)"), "/" ) << "/" << src;
    srcPathCompileUnitLast = ss.str();
}

void DwarfParser::lookupStructs(const dwarf::die& node) {
    for (const auto& attr : node.attributes()) {
        dwarf::DW_AT attributeName = attr.first;
        string value = to_string(attr.second);

        if(attributeName == dwarf::DW_AT::name) {
            structNameLast = value;
            break;
        }
    }
}


static bool isStructDepthLevelZero(const DwarfTreeObj &dwarf) {
    return (dwarf.getAbsType() == AbstractType::Struct && (dwarf.getTag().compare("DW_TAG_variable") == 0));
}

static bool isOutOfScope(const string &srcFileParent, const string &srcFileChild) {
    return (srcFileParent.compare(srcFileChild) != 0);
}

static bool isStructChildValid(const DwarfTreeObj &dwarfChild) {
    return  ((dwarfChild.getTag().compare("DW_TAG_member") == 0) && dwarfChild.getAbsType() != AbstractType::Pointer);
}

static bool isChildOfParentStruct(const string &parentDataType, const string &childParentStructName) {
    return (parentDataType.compare(childParentStructName) == 0);
}

/**
 * @brief   reverse lookup, starting from struct name, set setStructElementsMapKey
 */
void DwarfParser::lookupStructElementsInDwarfTree(DwarfTreeObj &dwarf, uint64_t dwarfKey,  int depth) {

    if(isStructDepthLevelZero(dwarf) || depth > 0) {
        string parentDataType = dwarf.getDataType().first;
        string srcFileParent = dwarf.getSourcePath();

        if(parentDataType.size() > 0) {
            map<uint64_t, shared_ptr<DwarfTreeObj>>::reverse_iterator itRevDwarfTreeElement(dwarfTree.find(dwarfKey));
            vector<uint64_t> dwarfStructKeys;
            for (auto &it = itRevDwarfTreeElement; it != dwarfTree.rend(); ++it) {
                shared_ptr<DwarfTreeObj> dwarfChild = it->second;

                if((isChildOfParentStruct(parentDataType, dwarfChild->getStructName())) && isStructChildValid(*dwarfChild)) {
                    uint64_t key = it->first;
                    dwarfStructKeys.push_back(key);

                    if(dwarfChild->getAbsType() == AbstractType::Struct) {
                        if(depth <= STRUCT_MEMBER_SEARCH_DEPTH_MAX) {
                            lookupStructElementsInDwarfTree(*dwarfChild, key, ++depth);
                        }
                    }
                }

                if(isOutOfScope(srcFileParent, dwarfChild->getSourcePath())) {
                    break;
                }
            }
            addStructElementsToStruct(dwarf, dwarfStructKeys);
        }
    }
}

void DwarfParser::addStructElementsToStruct(DwarfTreeObj &dwarfStruct,vector<uint64_t> &dwarfStructKeys) {
    uint64_t baseAddr = dwarfStruct.getAddr();
    uint32_t offset = 0;
    for(vector<uint64_t>::reverse_iterator it = dwarfStructKeys.rbegin(); it != dwarfStructKeys.rend(); ++it) { //set element to correct position
        shared_ptr<DwarfTreeObj> dwarfChild = dwarfTree.at(*it);
        vector<uint64_t> keysChild = dwarfChild->getStructElementsMapKey();

        dwarfChild->setAddr(baseAddr+offset);
        dwarfStruct.setStructElementsMapKey(*it);
        offset += dwarfChild->getDataType().second;

    }
}

void DwarfParser::addInfoToDwarfTreeObj(const dwarf::die& node, DwarfTreeObj& dwarfTreeObj) {
    for (const auto &attr : node.attributes()) {
        dwarf::DW_AT attributeName = attr.first;
        string value = to_string(attr.second);

        if(attributeName == dwarf::DW_AT::name) {
            dwarfTreeObj.setAttributeName(value);
        }
        if(attributeName == dwarf::DW_AT::decl_file) {
            dwarfTreeObj.setSourcePath(srcPathCompileUnitLast);
        }
        if(attributeName == dwarf::DW_AT::decl_line) {
            dwarfTreeObj.setDeclLine(stringHexToDec(value));
        }
        if(attributeName == dwarf::DW_AT::type) {
            dwarfTreeObj.setDataType(lookupDataType(node));
        }
    }

    if(node.tag == dwarf::DW_TAG::member) {
        AbstractType absType = lookupAbstractTypeForVariable(node);
        dwarfTreeObj.setAbsType(absType);
        dwarfTreeObj.setStructName(structNameLast);
    } else if(node.tag == dwarf::DW_TAG::variable) {
        AbstractType absType = lookupAbstractTypeForVariable(node);
        dwarfTreeObj.setAbsType(absType);
        dwarfTreeObj.setAddr(lookupAddrOfVariable(node, absType));
    }

}

std::pair<string, uint32_t> DwarfParser::lookupDataType(const dwarf::die& node) {
    string name;
    uint32_t byteSize = 0;
    std::pair <string, uint32_t> ret;

    dwarf::value value = node.resolve(dwarf::DW_AT::type);
    if(value.valid()) {
        dwarf::die nodeChild = value.as_reference();

        if((nodeChild.tag == dwarf::DW_TAG::base_type) || (nodeChild.tag == dwarf::DW_TAG::structure_type)) {
            for(auto &attr : nodeChild.attributes()) {
                dwarf::DW_AT attributeName = attr.first;
                string value = to_string(attr.second);
                if(attributeName == dwarf::DW_AT::name) {
                    name = value;
                }
                if(attributeName == dwarf::DW_AT::byte_size) {
                    byteSize = stringHexToDec(value);
                }
            }
            ret = std::make_pair(name, byteSize);
        } else {
            ret = lookupDataType(nodeChild);
        }
    }
    return  ret;
}

AbstractType DwarfParser::lookupAbstractTypeForVariable(const dwarf::die& node) {
    AbstractType retType = AbstractType::NotFound;

    dwarf::value value = node.resolve(dwarf::DW_AT::type);
    if(value.valid()) {
        dwarf::die nodeChild = value.as_reference();

        if(nodeChild.tag == dwarf::DW_TAG::array_type) {
            retType = AbstractType::Array;
        } else if(nodeChild.tag == dwarf::DW_TAG::structure_type) {
            retType = AbstractType::Struct;
        } else if(nodeChild.tag == dwarf::DW_TAG::pointer_type) {
            retType = AbstractType::Pointer;
        } else if(nodeChild.tag == dwarf::DW_TAG::base_type) {
            retType = AbstractType::Variable;
        } else {
            retType = lookupAbstractTypeForVariable(nodeChild);
        }
    }
    return  retType;
}

uint64_t DwarfParser::lookupAddrOfVariable(const dwarf::die &node, AbstractType absType) {
    uint64_t addr = 0;
    if ((node.tag == dwarf::DW_TAG::variable) && (absType != AbstractType::Pointer)) { //dwarf library does not support pointers yet
        dwarf::value location = node.resolve(dwarf::DW_AT::location);

        if (isVariableLocationValid(node)) {
            try {
                dwarf::expr_result result = location.as_exprloc().evaluate(&dwarf::no_expr_context);
                addr = result.value;
            } catch(dwarf::expr_error& caught) {
                std::cout << "Error lookupAddrOfVariable(): " << caught.what() << std::endl;
            }
        }
    }
    return addr;
}

void DwarfParser::addDwarfTreeObjToMap(uint64_t key, shared_ptr<DwarfTreeObj> dwarfTreeObj) {
    size_t sizeName = dwarfTreeObj->getAttributeName().size();
    AbstractType absType = dwarfTreeObj->getAbsType();

    if((sizeName > 0) && (absType != AbstractType::NotFound)) {
        dwarfTree.insert(std::make_pair(key, dwarfTreeObj));
    }
}

/**************************************************************
 *  Utility
 * ***********************************************************/
uint32_t DwarfParser::stringHexToDec(const string& hexString) {
    string hex = hexString.substr(2, hexString.size());
    return (uint32_t) strtoul(hex.c_str(), nullptr, 16);
}

/**************************************************************
 *  Getter / Setter
 * ***********************************************************/

map<uint64_t, std::shared_ptr<DwarfTreeObj>> DwarfParser::getDwarfTree() const {
    return dwarfTree;
}

void DwarfParser::setElfFilePathName(const string &value) {
    elfFilePathName = value;
}





