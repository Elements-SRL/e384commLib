cmake_minimum_required(VERSION 3.21)

project(e384commlib
    VERSION 0.42.1
    LANGUAGES CXX
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(BUILD_FOR_LABVIEW "Build the library as a DLL wrapper for LabVIEW" OFF)

# Library

set(LIB_SOURCES
    src/commandcoder.cpp
    src/framemanager.cpp
    src/messagedispatcher.cpp
    src/devices/EMCR/emcrdevice.cpp
    src/devices/EMCR/emcropalkellydevice.cpp
    src/devices/EMCR/emcrudbdevice.cpp
    src/devices/EMCR/emcrftdidevice.cpp
    src/devices/EMCR/emcr192blm_el03c_mb02_mez03_fw_v01.cpp
    src/devices/EMCR/emcr192blm_el03c_mb02_mez03_fw_v02.cpp
    src/devices/EMCR/emcr192blm_el03c_mb02_mez03_fw_v03.cpp
    src/devices/EMCR/emcr192blm_el03c_mb02_mez03_fw_v04.cpp
    src/devices/EMCR/emcr192blm_el03c_mb02_mez03_fw_v05.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v01.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v02.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v03.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v04.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v05.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v06.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v08.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez04_fw_v09.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez05_fw_v07.cpp
    src/devices/EMCR/emcr192blm_el03c_mb03_mez05_fw_v09.cpp
    src/devices/EMCR/emcr8blm_el03c_digbrd_fw_v01.cpp
    src/devices/EMCR/emcr384nanopores.cpp
    src/devices/EMCR/emcr384nanopores_sr7p5khz_v01.cpp
    src/devices/EMCR/emcr384patchclamp_prot_v01_fw_v02.cpp
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v03.cpp
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v04.cpp
    src/devices/EMCR/emcr384patchclamp_prot_v04_fw_v05.cpp
    src/devices/EMCR/emcr384patchclamp_prot_v05_fw_v06.cpp
    src/devices/EMCR/emcr384patchclamp_el07cd_prot_v06_fw_v01.cpp
    src/devices/EMCR/emcr384patchclamp_el07cd_prot_v06_fw_v02.cpp
    src/devices/EMCR/emcr384patchclamp_el07c_prot_v07_fw_v03.cpp
    src/devices/EMCR/emcr384patchclamp_el07c_prot_v08_fw_v04.cpp
    src/devices/EMCR/emcr384patchclamp_el07e_fw_v04.cpp
    src/devices/EMCR/emcr384patchclamp_el07e_fw_v05.cpp
    src/devices/EMCR/emcr384patchclamp_el07e_fw_v06.cpp
    src/devices/EMCR/emcr384patchclamp_el07e_fw_v07.cpp
    src/devices/EMCR/emcr16patchclamp_el07e_digbrd_fw_v01.cpp
    src/devices/EMCR/emcr384voltageclamp_prot_v04_fw_v03.cpp
    src/devices/EMCR/emcr10mhz.cpp
    src/devices/EMCR/emcr10mhzsb.cpp
    src/devices/EMCR/emcr2x10mhz.cpp
    src/devices/EMCR/emcr2x10mhz_fet.cpp
    src/devices/EMCR/emcr2x10mhz_sb_pcbv02_festim.cpp
    src/devices/EMCR/emcr4x10mhz.cpp
    src/devices/EMCR/emcr4x10mhz_sb_pcbv02_festim_ch12_v01.cpp
    src/devices/EMCR/emcr24x10mhz_only8ch.cpp
    src/devices/EMCR/emcr24x10mhz_el05c12_pcbv01.cpp
    src/devices/EMCR/emcr24x10mhz_el05c34_pcbv01.cpp
    src/devices/EMCR/emcr32x10mhz_only_dig_pcbv01.cpp
    src/devices/EMCR/emcrtestboardel07ab.cpp
    src/devices/EMCR/emcrtestboardel07cd.cpp
    src/devices/EMCR/emcrqc01atb_v01.cpp
    src/devices/EMCR/emcrqc01atb_pcbv02.cpp
    src/devices/EMCR/emcr8patchclamp_el07cd_artix7.cpp
    src/devices/EMCR/emcr8patchclamp_el07e_artix7_pcbv03_fw_v01.cpp
    src/devices/EMCR/emcr8npatchclamp_el07c_artix7_pcbv01.cpp
    src/devices/EMCR/emcr8npatchclamp_el07e_artix7_pcbv02.cpp
    src/devices/EMCR/emcr8npatchclamp_el07e_artix7_pcbv02_fw_v02.cpp
    src/devices/EMCR/emcrsuperduck_pcbv01.cpp
    src/devices/EMCR/emcr384nanoporesfake.cpp
    src/devices/EMCR/emcr384patchclampfake.cpp
    src/devices/EMCR/emcr10mhzfake.cpp
    src/devices/EMCR/emcr2x10mhzfake.cpp
    src/devices/EMCR/emcr4x10mhzfake.cpp
    src/devices/EMCR/emcrtestboardel07cdfake.cpp
    src/devices/EZPatch/ezpatchdevice.cpp
    src/devices/EZPatch/ezpatchftdidevice.cpp
    src/devices/EZPatch/ezpatchepatchel03d.cpp
    src/devices/EZPatch/ezpatchepatchel03f_4d.cpp
    src/devices/EZPatch/ezpatchepatchel04e.cpp
    src/devices/EZPatch/ezpatchepatchel04f.cpp
    src/devices/EZPatch/ezpatchepatchel03f_4e.cpp
    src/devices/EZPatch/ezpatchepatchel03f_4f.cpp
    src/devices/EZPatch/ezpatche4pel04f.cpp
    src/devices/EZPatch/ezpatche4ppatch.cpp
    src/devices/EZPatch/ezpatche4ppatch_el07ab.cpp
    src/devices/EZPatch/ezpatche8ppatch.cpp
    src/devices/EZPatch/ezpatche8ppatch_el07ab.cpp
    src/devices/EZPatch/ezpatche8ppatch_el07cd.cpp
    src/devices/EZPatch/ezpatchfakepatch.cpp
    src/devices/EZPatch/ezpatchfakep8.cpp
    src/model/boardmodel.cpp
    src/model/channelmodel.cpp
    src/calibration/tomlcalibrationmanager.cpp
    src/calibration/csvcalibrationmanager.cpp
    src/peripherals/okprogrammer.cpp
    src/peripherals/udbprogrammer.cpp
    src/peripherals/udbutils.cpp
)

if(BUILD_FOR_LABVIEW)
    set(TARGET_NAME "e384commlib_labview")

    list(APPEND LIB_SOURCES src/e384commlib_labview.cpp)
    set(LIB_HEADERS src/e384commlib_labview.h)

    add_library(${TARGET_NAME} SHARED ${LIB_SOURCES} ${LIB_HEADERS})
    add_library(e384commlib::e384commlib ALIAS ${TARGET_NAME})

    target_compile_definitions(${TARGET_NAME} PRIVATE
        E384COMMLIB_LIBRARY
    )

    target_compile_definitions(${TARGET_NAME} PUBLIC
        E384COMMLIB_LABVIEW_WRAPPER
    )

    cmake_path(SET LABVIEW_TO_C_PATH_CMAKE "$ENV{LABVIEW_TO_C_PATH}")
    target_include_directories(${TARGET_NAME}
        PUBLIC
        ${LABVIEW_TO_C_PATH_CMAKE}/include)
    target_link_libraries(${TARGET_NAME}
        PUBLIC
        "${LABVIEW_TO_C_PATH_CMAKE}/lib/labview.lib"
        "user32.lib")

else()
    set(TARGET_NAME "e384commlib")

    add_library(${TARGET_NAME} STATIC ${LIB_SOURCES})
    add_library(e384commlib::e384commlib ALIAS ${TARGET_NAME})

    target_compile_definitions(${TARGET_NAME} PUBLIC E384COMMLIB_STATIC)

endif()

# C standard

set_target_properties(${TARGET_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Include directories

target_include_directories(${TARGET_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/calibration>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/devices/EMCR>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/devices/EZPatch>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/model>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/peripherals>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/model>
)

# Defines

target_compile_definitions(${TARGET_NAME}
    PUBLIC
    COMMLIB_MAJOR=${PROJECT_VERSION_MAJOR}
    COMMLIB_MINOR=${PROJECT_VERSION_MINOR}
    COMMLIB_PATCH=${PROJECT_VERSION_PATCH}
)

# Transitive dependency

cmake_path(SET FRONT_PANEL_PATH_CMAKE "$ENV{FRONT_PANEL_PATH}")
include(${FRONT_PANEL_PATH_CMAKE}cmake/FindFrontPanel.cmake)

cmake_path(SET CY_API_PATH_CMAKE "$ENV{CY_API_PATH}")
include(${CY_API_PATH_CMAKE}cmake/FindCyAPI.cmake)

cmake_path(SET TOML_PP_PATH_CMAKE "$ENV{TOML_PP_PATH}")
include(${TOML_PP_PATH_CMAKE}cmake/Findtomlpp.cmake)

cmake_path(SET FTDI_UTILS_PATH_CMAKE "$ENV{FTDI_UTILS_PATH}")
list(APPEND CMAKE_PREFIX_PATH "${FTDI_UTILS_PATH_CMAKE}")

find_package(ftdi_utils REQUIRED CONFIG)

target_link_libraries(${TARGET_NAME}
    PUBLIC
    FrontPanel::FrontPanel
    CyAPI::CyAPI
    tomlpp::tomlpp
    ftdi_utils::ftdi_utils
)

# target name debug/release

set_target_properties(${TARGET_NAME} PROPERTIES
    DEBUG_POSTFIX d
)

# Install

cmake_path(SET E384COMMLIB_PATH_CMAKE "$ENV{E384COMMLIB_PATH}")
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${E384COMMLIB_PATH_CMAKE} CACHE PATH "Install path" FORCE)
endif()

install(TARGETS ${TARGET_NAME}
    EXPORT e384commlibTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install header

install(DIRECTORY src/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)

# Export target

install(EXPORT e384commlibTargets
    FILE e384commlibTargets.cmake
    NAMESPACE e384commlib::
    DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/\${CMAKE_INSTALL_CONFIG_NAME}"
)

# Config package

configure_package_config_file(
    cmake/e384commlibConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/e384commlibConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/e384commlibConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/e384commlibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/e384commlibConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake
)
