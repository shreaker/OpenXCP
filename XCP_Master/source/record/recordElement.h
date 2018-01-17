/**
*@file recordElement.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Data-structure for parameters/signals during a measurement and calibration session. A RecordElement object has a ParseResultBase member.
*/

#ifndef RECORDELEMENT_H
#define RECORDELEMENT_H

#include "parser/parseResultBase.h"
#include "xcpConfig/xcpEvent.h"

#include <memory>

class RecordElement {
  public:
    enum class TriggerMode {Polling, Event};

    RecordElement(const std::shared_ptr<ParseResultBase> parseResult,
                  const bool isSelected = false,
                  const bool isFloat = false,
                  const TriggerMode triggerMode = TriggerMode::Polling,
                  const int pollingRateInMs = 1000,
                  const XcpEvent& event = XcpEvent());

    bool getIsSelected() const;
    void setIsSelected(bool value);

    bool getIsFloat() const;
    void setIsFloat(bool value);

    std::shared_ptr<ParseResultBase> getParseResult() const;
    void setParseResult(const std::shared_ptr<ParseResultBase> &value);

    XcpEvent getEvent() const;
    void setEvent(const XcpEvent &value);

    TriggerMode getTriggerMode() const;
    void setTriggerMode(const TriggerMode &value);

    int getPollingRateInMs() const;
    void setPollingRateInMs(int value);

    QList<int> getValueHistoryInt() const;
    void setValueHistoryInt(const QList<int> &value);

    int64_t getLatestValueInt() const;
    void setLatestValueIntAddHistory(int64_t value);

    int64_t getValueToSetInt() const;
    void setValueToSetInt(int64_t value);

    QList<float> getValueHistoryFloat() const;
    void setValueHistoryFloat(const QList<float> &value);

    float getLatestValueFloat() const;
    void setLatestValueFloatAddHistory(float value);


    float getValueToSetFloat() const;
    void setValueToSetFloat(float value);

  private:
    bool isSelected;
    bool isFloat;
    std::shared_ptr<ParseResultBase> parseResult;
    TriggerMode triggerMode;
    int pollingRateInMs;
    XcpEvent event;

    QList<int> valueHistoryInt;
    int64_t latestValueInt;
    int64_t valueToSetInt;
    QList<float> valueHistoryFloat;
    float latestValueFloat;
    float valueToSetFloat;
};

#endif // RECORDELEMENT_H
