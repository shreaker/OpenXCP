/**
*@file serializea2lcharacteristic.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Serialize calibration- settings to an A2L file.
*/

#ifndef SERIALIZEA2LCHARACTERISTIC_H
#define SERIALIZEA2LCHARACTERISTIC_H


#include "SerializeA2lBase.h"

class SerializeA2lCharacteristic : public SerializeA2lBase {
  public:
    SerializeA2lCharacteristic(const ParseResultBase& var);

    void write(QTextStream& stream ) override;

  private:
    double stepSize;

};

#endif // SERIALIZEA2LCHARACTERISTIC_H
