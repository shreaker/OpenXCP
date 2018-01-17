/**
*@file sourceParserobj.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This abstract class holds the data which was obtained parsing the source code comments by the source parser.
*/

#ifndef SOURCEPARSEROBJ_H
#define SOURCEPARSEROBJ_H

#include <string>

using std::string;

class SourceParserObj {
  public:

    SourceParserObj();

    enum class TypeId {Measurement, Characteristic};

    virtual TypeId getTypeId() const = 0;

    virtual string getTypeString() const = 0;

    string getComment() const;
    void setComment(const string &value);

    double getLowerLimit() const;
    void setLowerLimit(const double &limit);

    double getUpperLimit() const;
    void setUpperLimit(const double &limit);

    string getPhyUnit() const;
    void setPhyUnit(const string &phyUnit);


  protected:
    string comment;
    double lowerLimit;
    double upperLimit;
    string phyUnit;
};

#endif // SOURCECOMMENTOBJ_H
