/**
*@file sourceparsecharacteristic.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This class represents the XCP attributes for CHARACTERISTIC parameters.
*/

#ifndef SOURCEPARSECHARACTERISTIC_H
#define SOURCEPARSECHARACTERISTIC_H

#include <ostream>

#include "sourceParserobj.h"


class SourceParseCharacteristic : public SourceParserObj {
  public:

    SourceParseCharacteristic();

    TypeId getTypeId() const;
    string getTypeString() const override;

    double getStepSize() const;
    void setStepSize(double value);

    friend std::ostream& operator<<(std::ostream& out, const SourceParseCharacteristic& characteristic);

  private:
    double stepSize;
};

#endif // SOURCEPARSECHARACTERISTIC_H
