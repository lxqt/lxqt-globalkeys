cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

project(lxqt-globalkeys)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

if(NOT CMAKE_BUILD_TYPE)
     set(CMAKE_BUILD_TYPE Release)
endif()

set(LXQTBT_MINIMUM_VERSION "0.5.0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
find_package(lxqt REQUIRED)


# Patch Version
set(LXQT_GLOBALKEYS_PATCH_VERSION 0)

set(LXQT_GLOBALKEYS_VERSION ${LXQT_MAJOR_VERSION}.${LXQT_MINOR_VERSION}.${LXQT_GLOBALKEYS_PATCH_VERSION})
add_definitions("-DLXQT_GLOBALKEYS_VERSION=\"${LXQT_GLOBALKEYS_VERSION}\"")

include(LXQtPreventInSourceBuilds)
include(LXQtTranslate)
include(LXQtCreatePkgConfigFile)
include(LXQtCreatePortableHeaders)
include(LXQtCompilerSettings NO_POLICY_SCOPE)

include(GNUInstallDirs) # Standard directories for installation
include(CMakePackageConfigHelpers)

add_definitions(-DQT_NO_FOREACH)

set(LXQT_GLOBALKEYS_LIBRARY_NAME lxqt-globalkeys)
set(LXQT_GLOBALKEYS_UI_LIBRARY_NAME lxqt-globalkeys-ui)
set(LXQT_INSTALL_CMAKE_DIR     "${CMAKE_INSTALL_DATAROOTDIR}/cmake")

#************************************************
# Build config.cmake files
#************************************************
set(LXQT_GLOBALKEYS_CMAKE_NAME ${LXQT_GLOBALKEYS_LIBRARY_NAME})
set(LXQT_GLOBALKEYS_UI_CMAKE_NAME ${LXQT_GLOBALKEYS_UI_LIBRARY_NAME})

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys-config.cmake.in"
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config.cmake"
    INSTALL_DESTINATION "${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}"
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys_ui-config.cmake.in"
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config.cmake"
    INSTALL_DESTINATION "${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}"
)
#************************************************
# End of build config.cmake
#************************************************

#************************************************
# Generate config-version.cmake files
#************************************************
write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config-version.cmake"
    VERSION ${LXQT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config-version.cmake"
    VERSION ${LXQT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
#************************************************
# End of generate config-version.cmake files
#************************************************

set(LXQT_GLOBALKEYS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(daemon)
add_subdirectory(config)
add_subdirectory(client)
add_subdirectory(ui)

install(FILES
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config.cmake"
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config-version.cmake"
    DESTINATION "${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}"
)

install(FILES
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config.cmake"
    "${CMAKE_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config-version.cmake"
    DESTINATION "${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}"
)

# merged from lxqt-common
add_subdirectory(autostart)
add_subdirectory(xdg)

