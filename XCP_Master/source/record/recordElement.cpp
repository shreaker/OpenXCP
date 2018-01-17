/**
*@file recordElement.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Data-structure for parameters/signals during a measurement and calibration session. A RecordElement object has a ParseResultBase member.
*/

#include "recordElement.h"


RecordElement::RecordElement(const std::shared_ptr<ParseResultBase> parseResult, const bool isSelected, const bool isFloat, const RecordElement::TriggerMode triggerMode, const int pollingRateInMs, const XcpEvent& event)
    : parseResult(parseResult),
      isSelected(isSelected),
      isFloat(isFloat),
      triggerMode(triggerMode),
      pollingRateInMs(pollingRateInMs),
      event(event),
      valueHistoryInt(QList<int>()),
      latestValueInt(0),
      valueToSetInt(0),
      valueHistoryFloat(QList<float>()),
      latestValueFloat(0),
      valueToSetFloat(0) {

}

bool RecordElement::getIsSelected() const {
    return isSelected;
}

void RecordElement::setIsSelected(bool value) {
    isSelected = value;
}

bool RecordElement::getIsFloat() const {
    return isFloat;
}

void RecordElement::setIsFloat(bool value) {
    isFloat = value;
}

std::shared_ptr<ParseResultBase> RecordElement::getParseResult() const {
    return parseResult;
}

void RecordElement::setParseResult(const std::shared_ptr<ParseResultBase> &value) {
    parseResult = value;
}

XcpEvent RecordElement::getEvent() const {
    return event;
}

void RecordElement::setEvent(const XcpEvent &value) {
    event = value;
}

RecordElement::TriggerMode RecordElement::getTriggerMode() const {
    return triggerMode;
}

void RecordElement::setTriggerMode(const RecordElement::TriggerMode &value) {
    triggerMode = value;
}

int RecordElement::getPollingRateInMs() const {
    return pollingRateInMs;
}

void RecordElement::setPollingRateInMs(int value) {
    pollingRateInMs = value;
}

QList<int> RecordElement::getValueHistoryInt() const {
    return valueHistoryInt;
}

void RecordElement::setValueHistoryInt(const QList<int> &value) {
    valueHistoryInt = value;
}

int64_t RecordElement::getLatestValueInt() const {
    return latestValueInt;
}

int64_t RecordElement::getValueToSetInt() const {
    return valueToSetInt;
}

void RecordElement::setValueToSetInt(int64_t value) {
    valueToSetInt = value;
}

void RecordElement::setLatestValueIntAddHistory(int64_t value) {
    latestValueInt = value;
    valueHistoryInt.append(latestValueInt);
}

QList<float> RecordElement::getValueHistoryFloat() const {
    return valueHistoryFloat;
}

void RecordElement::setValueHistoryFloat(const QList<float> &value) {
    valueHistoryFloat = value;
}

float RecordElement::getLatestValueFloat() const {
    return latestValueFloat;
}

void RecordElement::setLatestValueFloatAddHistory(float value) {
    latestValueFloat = value;
    valueHistoryFloat.append(latestValueFloat);
}

float RecordElement::getValueToSetFloat() const {
    return valueToSetFloat;
}

void RecordElement::setValueToSetFloat(float value) {
    valueToSetFloat = value;
}


