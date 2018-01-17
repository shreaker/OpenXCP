/**
*@file commandPayload.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This data-structure is used to process a XCP CTO command and to match the slaves response to the command request.
*/

#ifndef COMMANDPAYLOAD_H
#define COMMANDPAYLOAD_H

#include <QByteArray>

class CommandPayload {
  public:
    static const int UNINITIALIZED = 0;

    CommandPayload();

    CommandPayload(QByteArray payload,
                   uint8_t cmd = UNINITIALIZED,
                   uint64_t id = UNINITIALIZED,
                   uint32_t sizeDataType = UNINITIALIZED,
                   uint32_t rateInMsec = UNINITIALIZED
                  );


    QByteArray getPayload() const;
    void setPayload(const QByteArray &value);

    uint8_t getCmd() const;
    void setCmd(const uint8_t &value);

    uint64_t getId() const;
    void setId(const uint64_t &value);

    uint32_t getSizeDataType() const;
    void setSizeDataType(const uint32_t &value);

    uint32_t getRateInMsec() const;
    void setRateInMsec(const uint32_t &value);

  private:
    QByteArray payload;
    uint8_t cmd;
    uint64_t id;
    uint32_t sizeDataType;
    uint32_t rateInMsec;
};

#endif // COMMANDPAYLOAD_H
