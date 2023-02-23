QT       -= core gui

CONFIG(debug, debug|release) {
    TARGET = e384commlibd
    DEFINES += DEBUGPRINT
}

CONFIG(release, debug|release) {
    TARGET = e384commlib
}

TEMPLATE = lib
CONFIG += c++14

# use as static library
#DEFINES += E384COMMLIB_STATIC

contains(DEFINES, E384COMMLIB_STATIC) {
CONFIG += staticlib
} else {
# or create .dll
DEFINES += E384COMMLIB_LIBRARY
DEFINES += E384CL_LABVIEW_COMPATIBILITY
}

include(version.pri)

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
    "VERSION_MINOR=$$VERSION_MINOR"\
    "VERSION_PATCH=$$VERSION_PATCH"

VERSION_FULL = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}

SOURCES += \
    src/e384commlib.cpp \
    src/commandcoder.cpp \
    src/messagedispatcher.cpp \
    src/messagedispatcher_opalkelly.cpp \
    src/devices/messagedispatcher_384nanopores.cpp

HEADERS += \
    src/e384commlib_global.h \
    src/e384commlib_global_addendum.h \
    src/e384commlib_errorcodes.h \
    src/e384commlib.h \
    src/commandcoder.h \
    src/messagedispatcher.h \
    src/messagedispatcher_opalkelly.h \
    src/devices/messagedispatcher_384nanopores.h \
    src/utils.h

INCLUDEPATH += ./src \
    ./src/devices
DEPENDPATH += ./src \
    ./src/devices

include(LabVIEW/includelabview.pri)
include(frontPanel/includefrontpanel.pri)
