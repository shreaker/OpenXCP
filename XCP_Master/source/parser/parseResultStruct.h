/**
*@file parseResultStruct.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This classes stores the result of the ELF-, DWARF- and source code comment parser for structs.
*/

#ifndef PARSE_RESULT_STRUCT_H
#define PARSE_RESULT_STRUCT_H

#include "parseResultBase.h"


#include <vector>
#include <memory>

class ParseResultStruct : public ParseResultBase {
  public:
    ParseResultStruct();

    void traverse() override;

    void write(QJsonObject& json) override;

    void write(QTextStream &stream) override;

    void addElement(std::shared_ptr<ParseResultBase> element);


    std::vector<std::shared_ptr<ParseResultBase> > getElements() const;

  private:
    std::vector<std::shared_ptr<ParseResultBase>> elements;

};

#endif
