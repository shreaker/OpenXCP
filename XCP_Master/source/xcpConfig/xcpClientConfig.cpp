/**
*@file xcpClientConfig.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief XcpClientConfig class saves the received settings from the slave. Also the XcpClientConfig stores additional information about the slaves internal state and XCP configuration.
*/

#include "xcpClientConfig.h"

XcpClientConfig::XcpClientConfig() {
    isSupported = {false};
    version = XcpOptions::VERSION_1_0;
    endian = XcpOptions::ENDIAN_LITTLE;
    addrGranularity = XcpOptions::ADDRESS_GRANULARITY_BYTE;
    maxCto = 8;
    maxDto = 8;
    daqMode = XcpOptions::DAQ_MODE_DYNAMIC;
    sessionStatus = {false};
    resourceProtectStatus = {false};
    stateNum = 0;
    sessionConfigId = 0;
    synchErrorCode = 0;
}

XcpOptions::VERSION XcpClientConfig::getVersion() const {
    return version;
}

void XcpClientConfig::setVersion(const XcpOptions::VERSION &value) {
    version = value;
}

XcpOptions::ENDIAN XcpClientConfig::getEndian() const {
    return endian;
}

void XcpClientConfig::setEndian(const XcpOptions::ENDIAN &value) {
    endian = value;
}

XcpOptions::ADDRESS_GRANULARITY XcpClientConfig::getAddrGranularity() const {
    return addrGranularity;
}

void XcpClientConfig::setAddrGranularity(const XcpOptions::ADDRESS_GRANULARITY &value) {
    addrGranularity = value;
}

XcpOptions::DAQ_MODE XcpClientConfig::getDaqMode() const {
    return daqMode;
}

void XcpClientConfig::setDaqMode(const XcpOptions::DAQ_MODE &value) {
    daqMode = value;
}

int XcpClientConfig::getMaxCto() const {
    return maxCto;
}

void XcpClientConfig::setMaxCto(int value) {
    maxCto = value;
}

int XcpClientConfig::getMaxDto() const {
    return maxDto;
}

void XcpClientConfig::setMaxDto(int value) {
    maxDto = value;
}

uint8_t XcpClientConfig::getStateNum() const {
    return stateNum;
}

void XcpClientConfig::setStateNum(const uint8_t &value) {
    stateNum = value;
}

uint16_t XcpClientConfig::getSessionConfigId() const {
    return sessionConfigId;
}

void XcpClientConfig::setSessionConfigId(const uint16_t &value) {
    sessionConfigId = value;
}

uint8_t XcpClientConfig::getSynchErrorCode() const {
    return synchErrorCode;
}

void XcpClientConfig::setSynchErrorCode(const uint8_t &value) {
    synchErrorCode = value;
}

XcpClientConfig::IsSupported XcpClientConfig::getIsSupported() const {
    return isSupported;
}

void XcpClientConfig::setIsSupported(const XcpClientConfig::IsSupported &value) {
    isSupported = value;
}

XcpClientConfig::SessionStatus XcpClientConfig::getSessionStatus() const {
    return sessionStatus;
}

void XcpClientConfig::setSessionStatus(const XcpClientConfig::SessionStatus &value) {
    sessionStatus = value;
}

XcpClientConfig::ResourceProtectStatus XcpClientConfig::getResourceProtectStatus() const {
    return resourceProtectStatus;
}

void XcpClientConfig::setResourceProtectStatus(const XcpClientConfig::ResourceProtectStatus &value) {
    resourceProtectStatus = value;
}
