/**
*@file serializeA2lMeasurement.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize measurement- settings to an A2L file.
*/

#ifndef SERIALIZEA2LMEASUREMENT_H
#define SERIALIZEA2LMEASUREMENT_H

#include "SerializeA2lBase.h"

class SerializeA2lMeasurement :public SerializeA2lBase {
  public:
    SerializeA2lMeasurement(const ParseResultBase& var);
    void write(QTextStream& stream) override;

  private:
    QString conversrionFunctionToA2lConversion(std::string conversionFunction);

    QString conversion;
    int maxRefresh;
    bool discrete;
    bool readWrite;
};

#endif
