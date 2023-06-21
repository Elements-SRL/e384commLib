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
    DEFINES += DEBUG
#    DEFINES += DEBUG_MAX_SPEED
#    DEFINES += DEBUG_TX_DATA_PRINT
}

#DEFINES += E384NPR_ADDITIONAL_SR_FLAG
DEFINES += DISABLE_IIR

TEMPLATE = lib
CONFIG += c++14

# use as static library
#DEFINES += E384COMMLIB_LABVIEW_WRAPPER
DEFINES += E384COMMLIB_PYTHON_WRAPPER

contains(DEFINES, E384COMMLIB_LABVIEW_WRAPPER) {
    # create .dll
    DEFINES += E384COMMLIB_LIBRARY
}
contains(DEFINES, E384COMMLIB_PYTHON_WRAPPER) {
    # create .dll
    DEFINES += E384COMMLIB_LIBRARY
#    TARGET = e384CommLibPython
}
! contains(DEFINES, E384COMMLIB_LIBRARY){
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
contains(DEFINES, E384COMMLIB_LABVIEW_WRAPPER) {
    SOURCES += src/e384commlib_labview.cpp
    HEADERS += src/e384commlib_labview.h
    include(LabVIEW/includelabview.pri)
}

#contains(DEFINES, E384COMMLIB_PYTHON_WRAPPER){
#    SOURCES += \
#        src/e384commlib_python.cpp
#    LIBS += -L"C:\Users\lucar\AppData\Local\Programs\Python\Python310\libs" -lpython310
##    LIBS += -L"C:\Users\lucar\build-e384commlib-Desktop_Qt_5_12_11_MSVC2017_64bit-Release\release" -le384commlib
#    INCLUDEPATH += C:\Users\lucar\pybind11\include \
#            "C:\Users\lucar\AppData\Local\Programs\Python\Python310\include"
##            C:\Users\lucar\e384commLib
#}

INCLUDEPATH += \
    ./src \
    ./src/devices \
    ./src/calibration
DEPENDPATH += \
    ./src \
    ./src/devices \
    ./src/calibration

include(frontPanel/includefrontpanel.pri)
