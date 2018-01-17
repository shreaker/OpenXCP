/**
*@file commandPayload.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief This data-structure is used to process a XCP CTO command and to match the slaves response to the command request.
*/

#include "commandPayload.h"

CommandPayload::CommandPayload() {
    cmd = UNINITIALIZED;
    id = UNINITIALIZED;
    sizeDataType = UNINITIALIZED;
    rateInMsec = UNINITIALIZED;
}

CommandPayload::CommandPayload(QByteArray payload, uint8_t cmd, uint64_t id, uint32_t sizeDataType, uint32_t rateInMsec)
    : payload(payload),
      cmd(cmd),
      id(id),
      sizeDataType(sizeDataType),
      rateInMsec(rateInMsec) {
}

QByteArray CommandPayload::getPayload() const {
    return payload;
}

void CommandPayload::setPayload(const QByteArray &value) {
    payload = value;
}

uint8_t CommandPayload::getCmd() const {
    return cmd;
}

void CommandPayload::setCmd(const uint8_t &value) {
    cmd = value;
}

uint64_t CommandPayload::getId() const {
    return id;
}

void CommandPayload::setId(const uint64_t &value) {
    id = value;
}

uint32_t CommandPayload::getSizeDataType() const {
    return sizeDataType;
}

void CommandPayload::setSizeDataType(const uint32_t &value) {
    sizeDataType = value;
}

uint32_t CommandPayload::getRateInMsec() const {
    return rateInMsec;
}

void CommandPayload::setRateInMsec(const uint32_t &value) {
    rateInMsec = value;
}




