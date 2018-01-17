/**
*@file record.h
*@author Michael Wolf
*@date 27.11.2017
*@brief This class is used for saving recorded data from the slaves monitored measurement and calibration variables. The class is a data structure for serialization of the records to a CSV file.
*/

#ifndef RECORD_H
#define RECORD_H

#include <QString>

class Record {
  public:
    static const int NO_POSITION = -1;

    Record(double relativeTime, uint64_t id, int64_t value, int position = NO_POSITION);

    double getRelativeTime() const;
    void setRelativeTime(double value);

    uint64_t getId() const;
    void setId(const uint64_t &value);

    int64_t getValue() const;
    void setValue(int64_t value);

    int getPosition() const;
    void setPosition(int value);

  private:
    double relativeTime;
    uint64_t id;
    int64_t value;
    int position;

};

#endif // RECORDER_H


