QT     -= core gui
CONFIG -= qt

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += /Od
    TARGET = e384commlibd
    DEFINES += DEBUG
}

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += /O2
    TARGET = e384commlib
}

TEMPLATE = lib
CONFIG += c++20

# DEFINES += E384COMMLIB_LABVIEW_WRAPPER
# DEFINES += E384COMMLIB_LIBRARY

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
    src/framemanager.cpp \
    src/messagedispatcher.cpp \
    src/devices/EMCR/emcrdevice.cpp \
    src/devices/EMCR/emcropalkellydevice.cpp \
    src/devices/EMCR/emcrudbdevice.cpp \
    src/devices/EMCR/emcrftdidevice.cpp \
    src/devices/EMCR/emcr192blm_el03c_prot_v01_fw_v01.cpp \
    src/devices/EMCR/emcr192blm_el03c_mez03_mb04_fw_v01.cpp \
    src/devices/EMCR/emcr192blm_el03c_mez03_mb04_fw_v02.cpp \
    src/devices/EMCR/emcr384nanopores.cpp \
    src/devices/EMCR/emcr384nanopores_sr7p5khz_v01.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v01_fw_v02.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v03.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v04.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v05.cpp \
    src/devices/EMCR/emcr384patchclamp_prot_v05_fw_v06.cpp \
    src/devices/EMCR/emcr384patchclamp_el07cd_prot_v06_fw_v01.cpp \
    src/devices/EMCR/emcr384patchclamp_el07cd_prot_v06_fw_v02.cpp \
    src/devices/EMCR/emcr384patchclamp_el07c_prot_v07_fw_v03.cpp \
    src/devices/EMCR/emcr384patchclamp_el07c_prot_v08_fw_v255.cpp \
    src/devices/EMCR/emcr384patchclamp_el07e_fw_v01.cpp \
    src/devices/EMCR/emcr384voltageclamp_prot_v04_fw_v03.cpp \
    src/devices/EMCR/emcr10mhz.cpp \
    src/devices/EMCR/emcr10mhzsb.cpp \
    src/devices/EMCR/emcr2x10mhz.cpp \
    src/devices/EMCR/emcr2x10mhz_fet.cpp \
    src/devices/EMCR/emcr2x10mhz_sb_pcbv02_festim.cpp \
    src/devices/EMCR/emcr4x10mhz.cpp \
    src/devices/EMCR/emcr24x10mhz_only8ch.cpp \
    src/devices/EMCR/emcr24x10mhz_el05c12_pcbv01.cpp \
    src/devices/EMCR/emcr24x10mhz_el05c34_pcbv01.cpp \
    src/devices/EMCR/emcrtestboardel07ab.cpp \
    src/devices/EMCR/emcrtestboardel07cd.cpp \
    src/devices/EMCR/emcrqc01atb_v01.cpp \
    src/devices/EMCR/emcrqc01atb_pcbv02.cpp \
    src/devices/EMCR/emcr8patchclamp_el07cd_artix7.cpp \
    src/devices/EMCR/emcr8npatchclamp_el07c_artix7_pcbv01.cpp \
    src/devices/EMCR/emcr8npatchclamp_el07e_artix7_pcbv02.cpp \
    src/devices/EMCR/emcrsuperduck_pcbv01.cpp \
    src/devices/EMCR/emcr384nanoporesfake.cpp \
    src/devices/EMCR/emcr384patchclampfake.cpp \
    src/devices/EMCR/emcr10mhzfake.cpp \
    src/devices/EMCR/emcr2x10mhzfake.cpp \
    src/devices/EMCR/emcr4x10mhzfake.cpp \
    src/devices/EMCR/emcrtestboardel07cdfake.cpp \
    src/devices/EZPatch/ezpatchdevice.cpp \
    src/devices/EZPatch/ezpatchftdidevice.cpp \
    src/devices/EZPatch/ezpatchepatchel03d.cpp \
    src/devices/EZPatch/ezpatchepatchel03f_4d.cpp \
    src/devices/EZPatch/ezpatchepatchel04e.cpp \
    src/devices/EZPatch/ezpatchepatchel04f.cpp \
    src/devices/EZPatch/ezpatchepatchel03f_4e.cpp \
    src/devices/EZPatch/ezpatchepatchel03f_4f.cpp \
    src/devices/EZPatch/ezpatche4pel04f.cpp \
    src/devices/EZPatch/ezpatche4ppatch.cpp \
    src/devices/EZPatch/ezpatche4ppatch_el07ab.cpp \
    src/devices/EZPatch/ezpatche8ppatch.cpp \
    src/devices/EZPatch/ezpatche8ppatch_el07ab.cpp \
    src/devices/EZPatch/ezpatche8ppatch_el07cd.cpp \
    src/devices/EZPatch/ezpatchfakepatch.cpp \
    src/devices/EZPatch/ezpatchfakep8.cpp \
    src/model/boardmodel.cpp \
    src/model/channelmodel.cpp \
    src/calibration/tomlcalibrationmanager.cpp \
    src/calibration/csvcalibrationmanager.cpp \
    src/peripherals/okprogrammer.cpp \
    src/peripherals/udbprogrammer.cpp \
    src/peripherals/udbutils.cpp

HEADERS += \
    src/e384commlib_global.h \
    src/e384commlib_global_addendum.h \
    src/e384commlib_errorcodes.h \
    src/commandcoder.h \
    src/framemanager.h \
    src/messagedispatcher.h \
    src/devices/EMCR/emcrdevice.h \
    src/devices/EMCR/emcropalkellydevice.h \
    src/devices/EMCR/emcrudbdevice.h \
    src/devices/EMCR/emcrftdidevice.h \
    src/devices/EMCR/emcr192blm_el03c_prot_v01_fw_v01.h \
    src/devices/EMCR/emcr192blm_el03c_mez03_mb04_fw_v01.h \
    src/devices/EMCR/emcr192blm_el03c_mez03_mb04_fw_v02.h \
    src/devices/EMCR/emcr384nanopores.h \
    src/devices/EMCR/emcr384nanopores_sr7p5khz_v01.h \
    src/devices/EMCR/emcr384patchclamp_prot_v01_fw_v02.h \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v03.h \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v04.h \
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v05.h \
    src/devices/EMCR/emcr384patchclamp_prot_v05_fw_v06.h \
    src/devices/EMCR/emcr384patchclamp_el07cd_prot_v06_fw_v01.h \
    src/devices/EMCR/emcr384patchclamp_el07cd_prot_v06_fw_v02.h \
    src/devices/EMCR/emcr384patchclamp_el07c_prot_v07_fw_v03.h \
    src/devices/EMCR/emcr384patchclamp_el07c_prot_v08_fw_v255.h \
    src/devices/EMCR/emcr384patchclamp_el07e_fw_v01.h \
    src/devices/EMCR/emcr384voltageclamp_prot_v04_fw_v03.h \
    src/devices/EMCR/emcr10mhz.h \
    src/devices/EMCR/emcr10mhzsb.h \
    src/devices/EMCR/emcr2x10mhz.h \
    src/devices/EMCR/emcr2x10mhz_fet.h \
    src/devices/EMCR/emcr2x10mhz_sb_pcbv02_festim.h \
    src/devices/EMCR/emcr4x10mhz.h \
    src/devices/EMCR/emcr24x10mhz_only8ch.h \
    src/devices/EMCR/emcr24x10mhz_el05c12_pcbv01.h \
    src/devices/EMCR/emcr24x10mhz_el05c34_pcbv01.h \
    src/devices/EMCR/emcrtestboardel07ab.h \
    src/devices/EMCR/emcrtestboardel07cd.h \
    src/devices/EMCR/emcrqc01atb_v01.h \
    src/devices/EMCR/emcrqc01atb_pcbv02.h \
    src/devices/EMCR/emcr8patchclamp_el07cd_artix7.h \
    src/devices/EMCR/emcr8npatchclamp_el07c_artix7_pcbv01.h \
    src/devices/EMCR/emcr8npatchclamp_el07e_artix7_pcbv02.h \
    src/devices/EMCR/emcrsuperduck_pcbv01.h \
    src/devices/EMCR/emcr384nanoporesfake.h \
    src/devices/EMCR/emcr384patchclampfake.h \
    src/devices/EMCR/emcr10mhzfake.h \
    src/devices/EMCR/emcr2x10mhzfake.h \
    src/devices/EMCR/emcr4x10mhzfake.h \
    src/devices/EMCR/emcrtestboardel07cdfake.h \
    src/devices/EZPatch/ezpatchdevice.h \
    src/devices/EZPatch/ezpatchftdidevice.h \
    src/devices/EZPatch/ezpatchepatchel03d.h \
    src/devices/EZPatch/ezpatchepatchel03f_4d.h \
    src/devices/EZPatch/ezpatchepatchel04e.h \
    src/devices/EZPatch/ezpatchepatchel04f.h \
    src/devices/EZPatch/ezpatchepatchel03f_4e.h \
    src/devices/EZPatch/ezpatchepatchel03f_4f.h \
    src/devices/EZPatch/ezpatche4pel04f.h \
    src/devices/EZPatch/ezpatche4ppatch.h \
    src/devices/EZPatch/ezpatche4ppatch_el07ab.h \
    src/devices/EZPatch/ezpatche8ppatch.h \
    src/devices/EZPatch/ezpatche8ppatch_el07ab.h \
    src/devices/EZPatch/ezpatche8ppatch_el07cd.h \
    src/devices/EZPatch/ezpatchfakepatch.h \
    src/devices/EZPatch/ezpatchfakep8.h \
    src/model/boardmodel.h \
    src/model/channelmodel.h \
    src/calibration/tomlcalibrationmanager.h \
    src/calibration/csvcalibrationmanager.h \
    src/peripherals/ftdiutils.h \
    src/peripherals/okprogrammer.h \
    src/peripherals/udbprogrammer.h \
    src/peripherals/udbutils.h \
    src/speed_test.h \
    src/utils.h

INCLUDEPATH += \
    ./src \
    ./src/devices \
    ./src/devices/EMCR \
    ./src/devices/EZPatch \
    ./src/model \
    ./src/calibration \
    ./src/peripherals
DEPENDPATH += \
    ./src \
    ./src/devices \
    ./src/devices/EMCR \
    ./src/devices/EZPatch \
    ./src/model \
    ./src/calibration \
    ./src/peripherals

include($$(CY_API_PATH)includecyapi.pri)
include($$(FRONT_PANEL_PATH)includefrontpanel.pri)
include($$(FTDI_UTILS_PATH)includeftdiutils.pri)
include($$(TOML_PP_PATH)includetoml++.pri)
