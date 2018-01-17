/**
*@file serializeBase.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialization base class for A2L- and Json files.
*/

#ifndef SERIALIZEBASE_H
#define SERIALIZEBASE_H

#include "parser/parseResultBase.h"
#include "parser/parseResultVariable.h"
#include "main/model.h"

#include <memory>
#include <QVector>
#include <QSharedPointer>
#include <QFile>

class SerializeBase {
  public:
    SerializeBase(const QSharedPointer<Model>& model,const QString& saveFile);

    bool virtual save() = 0;
    bool virtual load() = 0;

  protected:
    void setParseResultToSave();

    const QSharedPointer<Model> model;
    QFile saveFile;
    QVector<std::shared_ptr<ParseResultBase>> parseResults;


  private:



};

#endif // SERIALIZEBASE_H
