/**
*@file xcpOptions.cpp
*@author Michael Wolf
*@date 27.11.2017
*@brief Helper class for various XCP settings.
*/

#include "xcpOptions.h"

const std::map<const XcpOptions::VERSION, const QString> XcpOptions::getVersionsKeyValue() {
    static const std::map<const XcpOptions::VERSION, const QString> versions {
        { VERSION_1_0, "1.0"},
        { VERSION_1_1, "1.1"},
        { VERSION_1_2, "1.2"},
        { VERSION_1_3, "1.3"},
        { VERSION_1_4, "1.4"}
    };
    return versions;
}

const std::map<const QString, const XcpOptions::VERSION> XcpOptions::getVersionsValueKey() {
    static const std::map<const QString, const XcpOptions::VERSION> versions {
        { "1.0", VERSION_1_0},
        { "1.1", VERSION_1_1},
        { "1.2", VERSION_1_2},
        { "1.3", VERSION_1_3},
        { "1.4", VERSION_1_4}
    };
    return versions;
}

const std::map<const XcpOptions::ENDIAN, const QString> XcpOptions::getEndianTypesKeyValue() {
    static const std::map<const XcpOptions::ENDIAN, const QString> endianTypes {
        {ENDIAN_LITTLE, "Little-endian"},
        {ENDIAN_BIG, "Big-endian"}
    };
    return endianTypes;
}

const std::map<const QString, const XcpOptions::ENDIAN> XcpOptions::getEndianTypesValueKey() {
    static const std::map<const QString, const XcpOptions::ENDIAN> endianTypes {
        {"Little-endian", ENDIAN_LITTLE, },
        {"Big-endian", ENDIAN_BIG}
    };
    return endianTypes;
}

const std::map<const XcpOptions::ADDRESS_GRANULARITY, const QString> XcpOptions::getAddrGranularitiesKeyValue() {
    static const std::map<const XcpOptions::ADDRESS_GRANULARITY, const QString> addrGranularities {
        {ADDRESS_GRANULARITY_BYTE, "BYTE"},
        {ADDRESS_GRANULARITY_WORD, "WORD"},
        {ADDRESS_GRANULARITY_DWORD, "DWORD"}
    };
    return addrGranularities;
}

const std::map<const QString, const XcpOptions::ADDRESS_GRANULARITY> XcpOptions::getAddrGranularitiesValueKey() {
    static const std::map<const QString, const XcpOptions::ADDRESS_GRANULARITY> addrGranularities {
        {"BYTE", ADDRESS_GRANULARITY_BYTE},
        {"WORD", ADDRESS_GRANULARITY_WORD},
        {"DWORD", ADDRESS_GRANULARITY_DWORD}
    };
    return addrGranularities;
}

const std::map<const XcpOptions::DAQ_MODE, const QString> XcpOptions::getDaqModesKeyValue() {
    static const std::map<const XcpOptions::DAQ_MODE, const QString> daqModes {
        {DAQ_MODE_DYNAMIC, "dynamic"},
        {DAQ_MODE_STATIC, "static"}
    };
    return daqModes;
}

const std::map<const QString, const XcpOptions::DAQ_MODE> XcpOptions::getDaqModesValueKey() {
    static const std::map<const QString, const XcpOptions::DAQ_MODE> daqModes {
        {"dynamic", DAQ_MODE_DYNAMIC},
        {"static", DAQ_MODE_STATIC}
    };
    return daqModes;
}
