/**
*@file xcpClientConfig.h
*@author Michael Wolf
*@date 27.11.2017
*@brief XcpClientConfig class saves the received settings from the slave. Also the XcpClientConfig stores additional information about the slaves internal state and XCP configuration.
*/

#ifndef XCPCLIENTCONFIG_H
#define XCPCLIENTCONFIG_H

#include "xcpOptions.h"

class XcpClientConfig {
  public:
    struct IsSupported {
        bool calibrationPaging;
        bool daqList;
        bool stimulation;
        bool programming;
        bool slaveBlockModeAvailable;
        bool optionalAvailable;
    };

    struct SessionStatus {
        bool storeCalReq;
        bool storeDaqReq;
        bool clearDaqReq;
        bool daqRunning;
        bool resume;
    };

    struct ResourceProtectStatus {
        bool calPag;
        bool daq;
        bool stim;
        bool pgm;
    };

  public:
    XcpClientConfig();

    XcpOptions::VERSION getVersion() const;
    void setVersion(const XcpOptions::VERSION &value);

    XcpOptions::ENDIAN getEndian() const;
    void setEndian(const XcpOptions::ENDIAN &value);

    XcpOptions::ADDRESS_GRANULARITY getAddrGranularity() const;
    void setAddrGranularity(const XcpOptions::ADDRESS_GRANULARITY &value);

    XcpOptions::DAQ_MODE getDaqMode() const;
    void setDaqMode(const XcpOptions::DAQ_MODE &value);

    int getMaxCto() const;
    void setMaxCto(int value);

    int getMaxDto() const;
    void setMaxDto(int value);

    uint8_t getStateNum() const;
    void setStateNum(const uint8_t &value);

    uint16_t getSessionConfigId() const;
    void setSessionConfigId(const uint16_t &value);

    uint8_t getSynchErrorCode() const;
    void setSynchErrorCode(const uint8_t &value);
    bool getIsSlaveBlockModeAvailable() const;
    void setIsSlaveBlockModeAvailable(bool value);
    bool getIsOptionalAvailable() const;
    void setIsOptionalAvailable(bool value);

    IsSupported getIsSupported() const;
    void setIsSupported(const IsSupported &value);

    SessionStatus getSessionStatus() const;
    void setSessionStatus(const SessionStatus &value);

    ResourceProtectStatus getResourceProtectStatus() const;
    void setResourceProtectStatus(const ResourceProtectStatus &value);

  private:
    XcpOptions::VERSION version;
    XcpOptions::ENDIAN endian;
    XcpOptions::ADDRESS_GRANULARITY addrGranularity;
    XcpOptions::DAQ_MODE daqMode;
    int maxCto;
    int maxDto;

    IsSupported isSupported;
    SessionStatus sessionStatus;
    ResourceProtectStatus resourceProtectStatus;

    uint8_t stateNum;
    uint16_t sessionConfigId;

    uint8_t synchErrorCode;
};

#endif // XCPCLIENTCONFIG_H
