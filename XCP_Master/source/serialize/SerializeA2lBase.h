/**
*@file SerializeA2lBase.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize XCP settings, measurement- and calibration- settings to an A2L file. Abstact base class.
*/

#ifndef SERIALIZEA2LBASE_H
#define SERIALIZEA2LBASE_H

#include "a2l_constants.h"
#include "parser/parseResultBase.h"

#include <map>
#include <string>

#include <QString>
#include <QTextStream>

class SerializeA2lBase {
  public:
    SerializeA2lBase(const ParseResultBase& var);

    virtual void write(QTextStream& stream) = 0;

  protected:
    QString datatypeToA2lDatatype(std::string datatype);

    const std::map<const std::string, const QString> a2lDatatypes = {
        {"unsigned char", A2L_DATATYPE_UBYTE},
        {"char", A2L_DATATYPE_SBYTE},
        {"signed char", A2L_DATATYPE_SBYTE},
        {"short unsigned int", A2L_DATATYPE_UWORD},
        {"short int", A2L_DATATYPE_SWORD},
        {"unsigned int", A2L_DATATYPE_ULONG},
        {"int", A2L_DATATYPE_SLONG},
        {"long unsigned int", A2L_DATATYPE_A_UINT64},
        {"long int", A2L_DATATYPE_A_INT64},
        {"float", A2L_DATATYPE_FLOAT32_IEEE},
        {"double", A2L_DATATYPE_FLOAT64_IEEE}
    };

    QString type;
    QString ident;
    QString longidentifier;
    QString datatype;
    float lowerLimit;
    float upperLimit;
    QString ecuAddress;
    QString physUnit;
};

#endif // SERIALIZEA2LBASE_H
