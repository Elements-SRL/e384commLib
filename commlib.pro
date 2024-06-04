QT     -= core gui
CONFIG -= qt

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -O0 # No optimization for debug
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
    QMAKE_CXXFLAGS += -O2
    TARGET = e384commlib
    # comment or uncomment depending on the desired verbosity
#    DEFINES += DEBUG_RX_RAW_DATA_PRINT
#    DEFINES += DEBUG_RX_DATA_PRINT
#    DEFINES += DEBUG_TX_DATA_PRINT
#    DEFINES += DEBUG_MAX_SPEED
}

#DEFINES += E384PATCH_ADDITIONAL_SR_FLAG

TEMPLATE = lib
CONFIG += c++14

#DEFINES += E384COMMLIB_LABVIEW_WRAPPER
#DEFINES += E384COMMLIB_LIBRARY

contains(DEFINES, E384COMMLIB_LABVIEW_WRAPPER) {
    TARGET = e384commlib_labview
    # create .dll
    DEFINES += E384COMMLIB_LIBRARY
    SOURCES += src/e384commlib_labview.cpp
    HEADERS += src/e384commlib_labview.h
    include($$(LABVIEW_TO_C_PATH)/includelabview.pri)
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
    src/devices/EMCR/emcrudbdevice.cpp \
    src/devices/EMCR/emcr384nanopores.cpp \
    src/devices/EMCR/emcr384nanopores_sr7p5khz_v01.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v01_fw_v02.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v03.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v04.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v05.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v05_fw_v06.cpp \
    src/devices/EMCR/emcr10mhz.cpp \
    src/devices/EMCR/emcr2x10mhz.cpp \
    src/devices/EMCR/emcr4x10mhz.cpp \
    src/devices/EZPatch/ezpatchdevice.cpp \
    src/devices/EZPatch/ezpatchftdidevice.cpp \
    src/devices/EZPatch/ftdieeprom.cpp \
    src/devices/EZPatch/ftdieeprom56.cpp \
    src/devices/EZPatch/ftdieepromdemo.cpp \
    src/devices/EZPatch/ezpatchepatchel03d.cpp \
    src/devices/EZPatch/ezpatchepatchel03f_4d.cpp \
    src/devices/EZPatch/ezpatchepatchel04e.cpp \
    src/devices/EZPatch/ezpatchepatchel04f.cpp \
    src/devices/EZPatch/ezpatchepatchel03f_4e.cpp \
    src/devices/EZPatch/ezpatchepatchel03f_4f.cpp \
    src/devices/EZPatch/ezpatche4pel04f.cpp \
    src/devices/EZPatch/ezpatche4ppatchliner.cpp \
    src/devices/EZPatch/ezpatche8ppatchliner.cpp \
    src/devices/EZPatch/ezpatche4ppatchliner_el07ab.cpp \
    src/devices/EZPatch/ezpatche8ppatchliner_el07ab.cpp \
    src/model/boardmodel.cpp \
    src/model/channelmodel.cpp \
    src/calibration/calibrationmanager.cpp \
    src/programming/udbprogrammer.cpp \
    src/udbutils.cpp

HEADERS += \
    src/e384commlib_global.h \
    src/e384commlib_global_addendum.h \
    src/e384commlib_errorcodes.h \
    src/commandcoder.h \
    src/messagedispatcher.h \
    src/devices/EMCR/emcrdevice.h \
    src/devices/EMCR/emcropalkellydevice.h \
    src/devices/EMCR/emcrudbdevice.h \
    src/devices/EMCR/emcr384nanopores.h \
    src/devices/EMCR/emcr384nanopores_sr7p5khz_v01.h \
    src/devices/EMCR/emcr384patchclamp_prot_v01_fw_v02.h \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v03.h \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v04.h \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v05.h \
    src/devices/EMCR/emcr384patchclamp_prot_v05_fw_v06.h \
    src/devices/EMCR/emcr10mhz.h \
    src/devices/EMCR/emcr2x10mhz.h \
    src/devices/EMCR/emcr4x10mhz.h \
    src/devices/EZPatch/ezpatchdevice.h \
    src/devices/EZPatch/ezpatchftdidevice.h \
    src/devices/EZPatch/ftdieeprom.h \
    src/devices/EZPatch/ftdieeprom56.h \
    src/devices/EZPatch/ftdieepromdemo.h \
    src/devices/EZPatch/ezpatchepatchel03d.h \
    src/devices/EZPatch/ezpatchepatchel03f_4d.h \
    src/devices/EZPatch/ezpatchepatchel04e.h \
    src/devices/EZPatch/ezpatchepatchel04f.h \
    src/devices/EZPatch/ezpatchepatchel03f_4e.h \
    src/devices/EZPatch/ezpatchepatchel03f_4f.h \
    src/devices/EZPatch/ezpatche4pel04f.h \
    src/devices/EZPatch/ezpatche4ppatchliner.h \
    src/devices/EZPatch/ezpatche8ppatchliner.h \
    src/devices/EZPatch/ezpatche4ppatchliner_el07ab.h \
    src/devices/EZPatch/ezpatche8ppatchliner_el07ab.h \
    src/model/boardmodel.h \
    src/model/channelmodel.h \
    src/calibration/calibrationmanager.h \
    src/programming/udbprogrammer.h \
    src/udbutils.h \
    src/utils.h

contains(DEFINES, DEBUG) {
    SOURCES += \
    src/devices/EMCR/emcr384nanoporesfake.cpp \
    src/devices/EMCR/emcr384patchclampfake.cpp \
    src/devices/EMCR/emcr10mhzfake.cpp \
    src/devices/EMCR/emcr2x10mhzfake.cpp \
    src/devices/EMCR/emcr4x10mhzfake.cpp \
    src/devices/EZPatch/ezpatchfakepatch.cpp \
    src/devices/EZPatch/ezpatchfakep8.cpp
    HEADERS += \
    src/devices/EMCR/emcr384nanoporesfake.h \
    src/devices/EMCR/emcr384patchclampfake.h \
    src/devices/EMCR/emcr10mhzfake.h \
    src/devices/EMCR/emcr2x10mhzfake.h \
    src/devices/EMCR/emcr4x10mhzfake.h \
    src/devices/EZPatch/ezpatchfakepatch.h \
    src/devices/EZPatch/ezpatchfakep8.h
}

INCLUDEPATH += \
    ./src \
    ./src/devices \
    ./src/devices/EMCR \
    ./src/devices/EZPatch \
    ./src/model \
    ./src/calibration \
    ./src/programming
DEPENDPATH += \
    ./src \
    ./src/devices \
    ./src/devices/EMCR \
    ./src/devices/EZPatch \
    ./src/model \
    ./src/calibration \
    ./src/programming

include($$(CY_API_PATH)includecyapi.pri)
include($$(FRONT_PANEL_PATH)includefrontpanel.pri)
include($$(FTD2XX_PATH)includeftd2xx.pri)
