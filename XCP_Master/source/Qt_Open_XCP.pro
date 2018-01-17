#-------------------------------------------------
#
# Project created by QtCreator 2017-07-21T13:53:33
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt_Open_XCP
TEMPLATE = app

#Uncomment for debug mode
#DEFINES += DEBUG

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS = -Wno-unused-parameter

SOURCES += \
    libs/libelfin/dwarf/abbrev.cc \
    libs/libelfin/dwarf/attrs.cc \
    libs/libelfin/dwarf/cursor.cc \
    libs/libelfin/dwarf/die.cc \
    libs/libelfin/dwarf/die_str_map.cc \
    libs/libelfin/dwarf/dwarf.cc \
    libs/libelfin/dwarf/dwarf_to_string.cc \
    libs/libelfin/dwarf/elf_dwarf.cc \
    libs/libelfin/dwarf/expr.cc \
    libs/libelfin/dwarf/line.cc \
    libs/libelfin/dwarf/rangelist.cc \
    libs/libelfin/dwarf/value.cc \
    libs/libelfin/elf/elf.cc \
    libs/libelfin/elf/elf_to_string.cc \
    libs/libelfin/elf/mmap_loader.cc \
    main/backend.cpp \
    main/controller.cpp \
    main/main.cpp \
    main/model.cpp \
    parser/dwarfparser.cpp \
    parser/dwarfTreeobj.cpp \
    parser/elfInfo.cpp \
    parser/elfParser.cpp \
    parser/parseResultArray.cpp \
    parser/parseResultBase.cpp \
    parser/parseResultStruct.cpp \
    parser/parseResultVariable.cpp \
    parser/parserManager.cpp \
    parser/sourceparsecharacteristic.cpp \
    parser/sourceparsemeasurement.cpp \
    parser/sourceparser.cpp \
    parser/sourceParserobj.cpp \
    record/record.cpp \
    record/recordElement.cpp \
    serialize/serializeA2l.cpp \
    serialize/serializeA2lBase.cpp \
    serialize/serializea2lcharacteristic.cpp \
    serialize/serializeA2lMeasurement.cpp \
    serialize/serializeBase.cpp \
    serialize/serializeJson.cpp \
    ui/dialogDeviceEventsInsert.cpp \
    ui/dialogRecordConfiguration.cpp \
    ui/dialogrecordconfigurationhelper.cpp \
    xcp/commandPayload.cpp \
    xcp/daqList.cpp \
    xcp/udpXcp.cpp \
    xcp/xcptask.cpp \
    xcpConfig/ethernetconfig.cpp \
    xcpConfig/xcpClientConfig.cpp \
    xcpConfig/xcpEvent.cpp \
    xcpConfig/xcpHostConfig.cpp \
    xcpConfig/xcpOptions.cpp \
    libs/libelfin/elf/mman.c

HEADERS += \
    libs/libelfin/dwarf/data.hh \
    libs/libelfin/dwarf/dwarf++.hh \
    libs/libelfin/dwarf/internal.hh \
    libs/libelfin/dwarf/small_vector.hh \
    libs/libelfin/elf/common.hh \
    libs/libelfin/elf/data.hh \
    libs/libelfin/elf/elf++.hh \
    libs/libelfin/elf/mman.h \
    libs/libelfin/elf/to_hex.hh \
    libs/qtcsv/include/qtcsv/abstractdata.h \
    libs/qtcsv/include/qtcsv/qtcsv_global.h \
    libs/qtcsv/include/qtcsv/reader.h \
    libs/qtcsv/include/qtcsv/stringdata.h \
    libs/qtcsv/include/qtcsv/variantdata.h \
    libs/qtcsv/include/qtcsv/writer.h \
    libs/qtcsv/sources/contentiterator.h \
    libs/qtcsv/sources/filechecker.h \
    libs/qtcsv/sources/symbols.h \
    main/backend.h \
    main/controller.h \
    main/model.h \
    parser/dwarfParser.h \
    parser/dwarfTreeObj.h \
    parser/elfInfo.h \
    parser/elfParser.h \
    parser/parseResultArray.h \
    parser/parseResultBase.h \
    parser/parseResultStruct.h \
    parser/parseResultVariable.h \
    parser/parserManager.h \
    parser/sourceparsecharacteristic.h \
    parser/sourceparsemeasurement.h \
    parser/sourceparser.h \
    parser/sourceparserconstants.h \
    parser/sourceParserobj.h \
    record/record.h \
    record/recordElement.h \
    serialize/a2l_constants.h \
    serialize/serializeA2l.h \
    serialize/SerializeA2lBase.h \
    serialize/serializea2lcharacteristic.h \
    serialize/serializeA2lMeasurement.h \
    serialize/serializeBase.h \
    serialize/serializeJson.h \
    ui/dialogDeviceEventsInsert.h \
    ui/dialogRecordConfiguration.h \
    ui/dialogrecordconfigurationhelper.h \
    xcp/commandPayload.h \
    xcp/daqList.h \
    xcp/udpxcp.h \
    xcp/xcpprotocoldefinitions.h \
    xcp/xcptask.h \
    xcpConfig/ethernetconfig.h \
    xcpConfig/xcpClientConfig.h \
    xcpConfig/xcpEvent.h \
    xcpConfig/xcpHostConfig.h \
    xcpConfig/xcpOptions.h


FORMS += \
    ui/dialogDeviceEventsInsert.ui \
    ui/dialogRecordConfiguration.ui \
    ui/dialogrecordconfigurationhelper.ui \
    ui/mainwindow.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    resource/LICENSE.md

win32: LIBS += -L$$PWD/libs/qtcsv/debug/ -lqtcsv1

INCLUDEPATH += $$PWD/libs/qtcsv/debug
DEPENDPATH += $$PWD/libs/qtcsv/debug
