cmake_minimum_required(VERSION 2.8.11)

project(lxqt-globalkeys)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

if(NOT CMAKE_BUILD_TYPE)
     set(CMAKE_BUILD_TYPE Release)
endif()

# Set default library installation path
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation directory for libraries")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5LinguistTools REQUIRED QUIET)
find_package(lxqt REQUIRED QUIET)
include(${LXQT_USE_FILE})

include(LXQtTranslate)

# Standard directories for installation
include(GNUInstallDirs)

set(LXQT_GLOBALKEYS_LIBRARY_NAME lxqt-globalkeys)
set(LXQT_GLOBALKEYS_UI_LIBRARY_NAME lxqt-globalkeys-ui)
set(LXQT_INSTALL_CMAKE_DIR     ${CMAKE_INSTALL_FULL_LIBDIR}/cmake)

#************************************************
# Build config.cmake files
#************************************************
#set(LXQT_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/lxqt)
#set(LXQT_TRANSLATIONS_DIR ${LXQT_SHARE_DIR}/translations)
include(cmake/FindInstallConfigPath.cmake)

set(LXQT_GLOBALKEYS_CMAKE_NAME ${LXQT_GLOBALKEYS_LIBRARY_NAME})
set(LXQT_GLOBALKEYS_UI_CMAKE_NAME ${LXQT_GLOBALKEYS_UI_LIBRARY_NAME})

set(LXQT_GLOBALKEYS_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/${LXQT_GLOBALKEYS_LIBRARY_NAME}")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config.cmake"
    @ONLY
)

set(LXQT_GLOBALKEYS_UI_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/${LXQT_GLOBALKEYS_UI_LIBRARY_NAME})
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys_ui-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config.cmake"
    @ONLY
)
#************************************************
# End of build config.cmake
#************************************************
set(LXQT_GLOBALKEYS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(daemon)
add_subdirectory(config)
add_subdirectory(client)
add_subdirectory(ui)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config.cmake DESTINATION ${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME})
install(FILES cmake/lxqt_globalkeys_use.cmake DESTINATION ${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config.cmake DESTINATION ${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME})
install(FILES cmake/lxqt_globalkeys_ui_use.cmake DESTINATION ${LXQT_INSTALL_CMAKE_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME})

# building tarball with CPack -------------------------------------------------
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set(CPACK_GENERATOR TBZ2)
set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include(CPack)
