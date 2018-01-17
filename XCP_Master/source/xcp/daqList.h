/**
*@file daqList.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Represents a XCP DAQ list.
*/

#ifndef DAQLIST_H
#define DAQLIST_H

#include <memory>
#include <QList>

#include "record/recordElement.h"

class DaqList {
  public:
    DaqList(const uint16_t eventChannelNr);

    uint16_t getEventChannel() const;
    void setEventChannel(const uint16_t &value);

    uint16_t getDaqListNr() const;
    void setDaqListNr(const uint16_t &value);

    QList<std::shared_ptr<RecordElement>> getOdtEntries() const;
    void setOdtEntries(const QList<std::shared_ptr<RecordElement>>& recordElementList);
    void addToOdtEntries(const std::shared_ptr<RecordElement> recordElement);

  private:
    uint16_t eventChannel;
    uint16_t daqListNr;
    QList<std::shared_ptr<RecordElement>> odtEntries;
};

#endif // DAQLIST_H
