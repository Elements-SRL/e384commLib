QT       -= core gui

include (./quietWarnings.pri)

CONFIG(debug, debug|release) {
    TARGET = e384commlibd
    DEFINES += DEBUGPRINT
}

CONFIG(release, debug|release) {
    TARGET = e384commlib
}

TEMPLATE = lib
CONFIG += c++11

# use as static library
DEFINES += E384COMMLIB_STATIC
CONFIG += staticlib

# or create .dll
#DEFINES += E384COMMLIB_LIBRARY

include(../version.pri)

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
    "VERSION_MINOR=$$VERSION_MINOR"\
    "VERSION_BUILD=$$VERSION_BUILD"

VERSION_FULL = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

SOURCES += \
    e384commlib.cpp \
    ftdieeprom.cpp \
    ftdieeprom56.cpp \
    ftdieepromdemo.cpp \
    calibrationeeprom.cpp \
    messagedispatcher.cpp

HEADERS += \
    e384commlib_global.h \
    e384commlib_errorcodes.h \
    e384commlib.h \
    ftdieeprom.h \
    ftdieeprom56.h \
    ftdieepromdemo.h \
    calibrationeeprom.h \
    messagedispatcher.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += ./

DEPENDPATH += ./

unix: INCLUDEPATH += /usr/local/include
unix: DEPENDPATH += /usr/local/include

include (./ftd2xx/includeftd2xx.pri)

LIBS += -L$$PWD/ftd2xx/win/x86 -lMPSSE
