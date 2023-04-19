QT       -= core gui

CONFIG(debug, debug|release) {
    TARGET = e384commlibd
    DEFINES += DEBUG
    # comment or uncomment depending on the desired verbosity
#    DEFINES += DEBUG_TX_DATA_PRINT
#    DEFINES += DEBUG_RX_RAW_DATA_PRINT
#    DEFINES += DEBUG_RX_PROCESSING_PRINT
#    DEFINES += DEBUG_RX_DATA_PRINT
}

CONFIG(release, debug|release) {
    TARGET = e384commlib
#    DEFINES += DEBUG
#    DEFINES += DEBUG_RX_PROCESSING_PRINT
}

TEMPLATE = lib
CONFIG += c++14

# use as static library
DEFINES += E384COMMLIB_STATIC

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
    src/commandcoder.cpp \
    src/messagedispatcher.cpp \
    src/messagedispatcher_opalkelly.cpp \
    src/devices/messagedispatcher_384nanopores.cpp \
    src/devices/messagedispatcher_384patchclamp.cpp \
    src/devices/messagedispatcher_4x10mhz.cpp

HEADERS += \
    src/e384commlib_global.h \
    src/e384commlib_global_addendum.h \
    src/e384commlib_errorcodes.h \
    src/commandcoder.h \
    src/messagedispatcher.h \
    src/messagedispatcher_opalkelly.h \
    src/devices/messagedispatcher_384nanopores.h \
    src/devices/messagedispatcher_384patchclamp.h \
    src/devices/messagedispatcher_4x10mhz.h \
    src/utils.h

contains(DEFINES, DEBUG) {
    SOURCES += src/devices/messagedispatcher_384fake.cpp
    HEADERS += src/devices/messagedispatcher_384fake.h
}
contains(DEFINES, E384CL_LABVIEW_COMPATIBILITY) {
    SOURCES += src/e384commlib_labview.cpp
    HEADERS += src/e384commlib_labview.h
    include(LabVIEW/includelabview.pri)
}

INCLUDEPATH += \
    ./src \
    ./src/devices
DEPENDPATH += \
    ./src \
    ./src/devices

include(frontPanel/includefrontpanel.pri)
