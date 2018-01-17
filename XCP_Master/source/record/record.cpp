/**
*@file record.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This class is used for saving recorded data from the slaves monitored measurement and calibration variables. The class is a data structure for serialization of the records to a CSV file.
*/

#include "record.h"

Record::Record(double relativeTime, uint64_t id, int64_t value, int position)
    : relativeTime(relativeTime),
      id(id),
      value (value),
      position(position) {

}

double Record::getRelativeTime() const {
    return relativeTime;
}

void Record::setRelativeTime(double value) {
    relativeTime = value;
}

uint64_t Record::getId() const {
    return id;
}

void Record::setId(const uint64_t &value) {
    id = value;
}


int64_t Record::getValue() const {
    return value;
}

void Record::setValue(int64_t  value) {
    value = value;
}

int Record::getPosition() const {
    return position;
}

void Record::setPosition(int value) {
    position = value;
}


