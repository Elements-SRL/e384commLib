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
#    DEFINES += DEBUG_LIQUID_JUNCTION_PRINT
#    DEFINES += DEBUG_MAX_SPEED
}

CONFIG(release, debug|release) {
    TARGET = e384commlib
    DEFINES += DEBUG
#    DEFINES += DEBUG_MAX_SPEED
}

DEFINES += E384NPR_ADDITIONAL_SR_FLAG
#DEFINES += E384PATCH_ADDITIONAL_SR_FLAG
#DEFINES += DISABLE_IIR

TEMPLATE = lib
CONFIG += c++14

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

! contains(DEFINES, E384COMMLIB_LIBRARY) {
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
    src/devices/EMCR/emcrdevice.cpp \
    src/devices/EMCR/emcropalkellydevice.cpp \
    src/devices/EMCR/emcr384nanopores.cpp \
    src/devices/EMCR/emcr384nanopores_sr7p5khz_v01.cpp \
    src/devices/EMCR/emcr384patchclamp.cpp \
    src/devices/EMCR/emcr384patchclamp_V04.cpp \
    src/devices/EMCR/emcr2x10mhz.cpp \
    src/devices/EMCR/emcr4x10mhz.cpp \
    src/model/boardmodel.cpp \
    src/model/channelmodel.cpp \
    src/calibration/calibrationmanager.cpp

HEADERS += \
    src/e384commlib_global.h \
    src/e384commlib_global_addendum.h \
    src/e384commlib_errorcodes.h \
    src/commandcoder.h \
    src/messagedispatcher.h \
    src/devices/EMCR/emcrdevice.h \
    src/devices/EMCR/emcropalkellydevice.h \
    src/devices/EMCR/emcr384nanopores.h \
    src/devices/EMCR/emcr384nanopores_sr7p5khz_v01.h \
    src/devices/EMCR/emcr384patchclamp.h \
    src/devices/EMCR/emcr384patchclamp_V04.h \
    src/devices/EMCR/emcr2x10mhz.h \
    src/devices/EMCR/emcr4x10mhz.h \
    src/model/boardmodel.h \
    src/model/channelmodel.h \
    src/calibration/calibrationmanager.h \
    src/utils.h

contains(DEFINES, DEBUG) {
    SOURCES += \
    src/devices/EMCR/emcr384nanoporesfake.cpp \
    src/devices/EMCR/emcr384patchclampfake.cpp \
    src/devices/EMCR/emcr2x10mhzfake.cpp \
    src/devices/EMCR/emcr4x10mhzfake.cpp
    HEADERS += \
    src/devices/EMCR/emcr384nanoporesfake.h \
    src/devices/EMCR/emcr384patchclampfake.h \
    src/devices/EMCR/emcr2x10mhzfake.h \
    src/devices/EMCR/emcr4x10mhzfake.h
}

INCLUDEPATH += \
    ./src \
    ./src/devices \
    ./src/devices/EMCR \
    ./src/model \
    ./src/calibration
DEPENDPATH += \
    ./src \
    ./src/devices \
    ./src/devices/EMCR \
    ./src/model \
    ./src/calibration

include($$(FRONT_PANEL_PATH)includefrontpanel.pri)
