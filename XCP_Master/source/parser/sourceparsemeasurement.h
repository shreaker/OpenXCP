/**
*@file sourceparsemeasurement.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This class represents the XCP attributes for MEASUREMENT signals.
*/

#ifndef SOURCEPARSEMEASUREMENT_H
#define SOURCEPARSEMEASUREMENT_H

#include "sourceParserobj.h"

#include <string>
#include <ostream>


using std::string;


class SourceParseMeasurement : public SourceParserObj {
  public:

    SourceParseMeasurement();

    SourceParserObj::TypeId getTypeId() const override;
    string getTypeString() const override;

    bool getIsDiscrete() const;
    void setIsDiscrete(bool value);

    uint32_t getMaxRefreshRate() const;
    void setMaxRefreshRate(const uint32_t &value);

    string getConversionFunction() const;
    void setConversionFunction(const string &value);

    friend std::ostream& operator<<(std::ostream& out, const SourceParseMeasurement& measurement);

    bool getIsWriteable() const;
    void setIsWriteable(bool value);

  private:
    bool isDiscrete;
    uint32_t maxRefreshRate;
    string conversionFunction;
    bool isWriteable;
};

#endif // SOURCEPARSEMEASUREMENT_H
