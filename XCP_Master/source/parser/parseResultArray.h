/**
*@file parseResultArray.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This classes stores the result of the ELF-, DWARF- and source code comment parser for arrays.
*/

#ifndef PARSE_RESULT_ARRAY_H
#define PARSE_RESULT_ARRAY_H

#include "parseResultBase.h"

#include "parseResultVariable.h"

class ParseResultArray : public ParseResultBase {
  public:
    ParseResultArray();

    void traverse() override;

    void write(QJsonObject& json) override;

    void write(QTextStream &stream) override;

    int getElements() const;
    void setElements(int value);

    int getSizePerElement() const;
    void setSizePerElement(int value);

    ParseResultVariable createParseResultVariable(int elementNrOfArray);

    int elements;
    int sizePerElement;
};

#endif
