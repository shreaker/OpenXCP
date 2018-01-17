/**
*@file serializeA2l.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize XCP settings, measurement- and calibration- settings to an A2L file.
*/

#ifndef SERIALIZEA2L_H
#define SERIALIZEA2L_H

#include "serializeBase.h"
#include "a2l_constants.h"

#include <map>

#include <QString>
#include <QTextStream>

using std::string;
using std::map;

class SerializeA2L : public SerializeBase {
  public:
    SerializeA2L(const QSharedPointer<Model>& model,const QString& saveFile);

    bool save() override;
    bool load() override;

  private:
    void writeParseResults(QTextStream& stream);
    void writeEthernetConfig(QTextStream& stream);

};

#endif // SERIALIZEA2L_H
