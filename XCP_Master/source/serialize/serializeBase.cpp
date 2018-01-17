/**
*@file serializeBase.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialization base class for A2L- and Json files.
*/

#include "serializeBase.h"

SerializeBase::SerializeBase(const QSharedPointer<Model>& model,const QString& saveFile)
    : model(model),
      saveFile(saveFile) {
}

void SerializeBase::setParseResultToSave() {
    for(const auto& var: model->getParseResults()) {
        parseResults.append(var);
    }
}
