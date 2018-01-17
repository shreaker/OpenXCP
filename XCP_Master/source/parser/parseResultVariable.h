/**
*@file parseResultVariable.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This classes stores the result of the ELF-, DWARF- and source code comment parser for simple variables.
*/

#ifndef PARSE_RESULT_VARIABLE_H
#define PARSE_RESULT_VARIABLE_H

#include "parseResultBase.h"

class ParseResultVariable : public ParseResultBase {
  public:
    ParseResultVariable();

    void traverse() override;

    void write(QJsonObject& json) override;
    void write(QTextStream &stream) override;

  private:





};

#endif
