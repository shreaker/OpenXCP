/**
*@file xcpOptions.h
*@author Michael Wolf
*@date 27.11.2017
*@brief Helper class for various XCP settings.
*/

#ifndef XCPOPTIONS_H
#define XCPOPTIONS_H

#include <map>
#include <QString>

class XcpOptions {
  public:

    XcpOptions() = delete;

    enum VERSION {VERSION_1_0, VERSION_1_1, VERSION_1_2, VERSION_1_3, VERSION_1_4};
    enum ENDIAN {ENDIAN_LITTLE, ENDIAN_BIG};
    enum ADDRESS_GRANULARITY {ADDRESS_GRANULARITY_BYTE, ADDRESS_GRANULARITY_WORD, ADDRESS_GRANULARITY_DWORD};
    enum DAQ_MODE {DAQ_MODE_DYNAMIC, DAQ_MODE_STATIC};


    static const std::map<const XcpOptions::VERSION, const QString> getVersionsKeyValue();
    static const std::map<const QString, const XcpOptions::VERSION> getVersionsValueKey();

    static const std::map<const XcpOptions::ENDIAN, const QString> getEndianTypesKeyValue();
    static const std::map<const QString, const XcpOptions::ENDIAN> getEndianTypesValueKey();

    static const std::map<const XcpOptions::ADDRESS_GRANULARITY, const QString> getAddrGranularitiesKeyValue();
    static const std::map<const QString, const XcpOptions::ADDRESS_GRANULARITY> getAddrGranularitiesValueKey();

    static const std::map<const XcpOptions::DAQ_MODE, const QString> getDaqModesKeyValue();
    static const std::map<const QString, const XcpOptions::DAQ_MODE> getDaqModesValueKey();


};

#endif // XCPOPTIONS_H
