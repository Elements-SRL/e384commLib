QT     -= core gui
CONFIG -= qt

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
    DEFINES += DEBUG
#    DEFINES += DEBUG_MAX_SPEED
#    DEFINES += DEBUG_TX_DATA_PRINT
}

#DEFINES += E384NPR_ADDITIONAL_SR_FLAG
#DEFINES += DISABLE_IIR

TEMPLATE = lib
CONFIG += c++14

# use as static library
#DEFINES += E384COMMLIB_LABVIEW_WRAPPER
#DEFINES += E384COMMLIB_PYTHON_WRAPPER

contains(DEFINES, E384COMMLIB_LABVIEW_WRAPPER) {
    # create .dll
    DEFINES += E384COMMLIB_LIBRARY
    SOURCES += src/e384commlib_labview.cpp
    HEADERS += src/e384commlib_labview.h
    include($$(LABVIEW_TO_C_PATH)/includelabview.pri)
}

contains(DEFINES, E384COMMLIB_PYTHON_WRAPPER) {
    # create .dll
    DEFINES += E384COMMLIB_LIBRARY
    TARGET = e384CommLibPython
    CONFIG -= app_bundle

    SOURCES += src/e384commlib_python.cpp
    LIBS += -L"$$(LOCAL_PYTHON_3_10_7)\libs" -lpython310
    INCLUDEPATH += $$(LOCAL_PYBIND_11)\include \
            "$$(LOCAL_PYTHON_3_10_7)\include"
}

! contains(DEFINES, E384COMMLIB_LIBRARY){
    # build statically
    DEFINES += E384COMMLIB_STATIC
    CONFIG += staticlib
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
    src/devices/messagedispatcher_2x10mhz.cpp \
    src/devices/messagedispatcher_4x10mhz.cpp \
    src/model/boardmodel.cpp \
    src/model/channelmodel.cpp \
    src/calibration/calibrationmanager.cpp

HEADERS += \
    src/e384commlib_global.h \
    src/e384commlib_global_addendum.h \
    src/e384commlib_errorcodes.h \
    src/commandcoder.h \
    src/messagedispatcher.h \
    src/messagedispatcher_opalkelly.h \
    src/devices/messagedispatcher_384nanopores.h \
    src/devices/messagedispatcher_384patchclamp.h \
    src/devices/messagedispatcher_2x10mhz.h \
    src/devices/messagedispatcher_4x10mhz.h \
    src/model/boardmodel.h \
    src/model/channelmodel.h \
    src/calibration/calibrationmanager.h \
    src/utils.h

contains(DEFINES, DEBUG) {
    SOURCES += src/devices/messagedispatcher_384fake.cpp \
        src/devices/messagedispatcher_384fakepatchclamp.cpp \
        src/devices/messagedispatcher_4x10mhzfake.cpp
    HEADERS += src/devices/messagedispatcher_384fake.h \
        src/devices/messagedispatcher_384fakepatchclamp.h \
        src/devices/messagedispatcher_4x10mhzfake.h
}

INCLUDEPATH += \
    ./src \
    ./src/devices \
    ./src/model \
    ./src/calibration
DEPENDPATH += \
    ./src \
    ./src/devices \
    ./src/model \
    ./src/calibration

include($$(FRONT_PANEL_PATH)/includefrontpanel.pri)
