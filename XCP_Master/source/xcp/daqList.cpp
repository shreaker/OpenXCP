/**
*@file daqList.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Represents a XCP DAQ list.
*/


#include "daqList.h"


DaqList::DaqList(const uint16_t eventChannelNr)
    : eventChannel(eventChannelNr),
      daqListNr(0) {

}

uint16_t DaqList::getEventChannel() const {
    return eventChannel;
}

void DaqList::setEventChannel(const uint16_t &value) {
    eventChannel = value;
}

uint16_t DaqList::getDaqListNr() const {
    return daqListNr;
}

void DaqList::setDaqListNr(const uint16_t &value) {
    daqListNr = value;
}

QList<std::shared_ptr<RecordElement>> DaqList::getOdtEntries() const {
    return odtEntries;
}

void DaqList::setOdtEntries(const QList<std::shared_ptr<RecordElement>>& recordElementList) {
    odtEntries = recordElementList;
}

void DaqList::addToOdtEntries(const std::shared_ptr<RecordElement> recordElement) {
    odtEntries.append(recordElement);
}

