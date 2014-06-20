cmake_minimum_required(VERSION 2.8.9)
# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(lxqt-globalkeys)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Set default library installation path
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation directory for libraries")

# Support Qt5
option(USE_QT5 "Build with Qt5." $ENV{USE_QT5})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
if(USE_QT5)
    cmake_minimum_required(VERSION 2.8.11)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5DBus REQUIRED)
    find_package(Qt5LinguistTools REQUIRED QUIET)
    find_package(lxqt-qt5 REQUIRED QUIET)
    set(QT_VERSION_SUFFIX "-qt5")
    message(STATUS "Building with Qt${Qt5Core_VERSION_STRING}")
else()
    set(QT_USE_QTDBUS 1)
    find_package(Qt4 REQUIRED)
    include(${QT_USE_FILE})
    find_package(LXQT REQUIRED)
    set(QT_VERSION_SUFFIX "")
    message(STATUS "Building with Qt${QTVERSION}")
endif()
include(${LXQT_USE_FILE})

set(LXQT_GLOBALKEYS_LIBRARY_NAME lxqt-globalkeys${QT_VERSION_SUFFIX})
set(LXQT_GLOBALKEYS_UI_LIBRARY_NAME lxqt-globalkeys-ui${QT_VERSION_SUFFIX})

#************************************************
# Build config.cmake files
#************************************************
#set(LXQT_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/lxqt)
#set(LXQT_TRANSLATIONS_DIR ${LXQT_SHARE_DIR}/translations)
include(cmake/FindInstallConfigPath.cmake)

if(USE_QT5)
    set(LXQT_GLOBALKEYS_CMAKE_NAME ${LXQT_GLOBALKEYS_LIBRARY_NAME})
    set(LXQT_GLOBALKEYS_UI_CMAKE_NAME ${LXQT_GLOBALKEYS_UI_LIBRARY_NAME})
else() # keep old cmake module names for Qt4
    set(LXQT_GLOBALKEYS_CMAKE_NAME lxqt_globalkeys)
    set(LXQT_GLOBALKEYS_UI_CMAKE_NAME lxqt_globalkeys_ui)
endif()

set(LXQT_GLOBALKEYS_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/${LXQT_GLOBALKEYS_LIBRARY_NAME})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config.cmake
    @ONLY
)

set(LXQT_GLOBALKEYS_UI_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/${LXQT_GLOBALKEYS_UI_LIBRARY_NAME})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys_ui-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config.cmake
    @ONLY
)
#************************************************
# End of build config.cmake
#************************************************
set(LXQT_GLOBALKEYS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(daemon)
add_subdirectory(config)
add_subdirectory(client)
add_subdirectory(ui)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_CMAKE_NAME}-config.cmake DESTINATION share/cmake/${LXQT_GLOBALKEYS_CMAKE_NAME})
install(FILES cmake/lxqt_globalkeys_use.cmake DESTINATION share/cmake/${LXQT_GLOBALKEYS_CMAKE_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LXQT_GLOBALKEYS_UI_CMAKE_NAME}-config.cmake DESTINATION share/cmake/${LXQT_GLOBALKEYS_UI_CMAKE_NAME})
install(FILES cmake/lxqt_globalkeys_ui_use.cmake DESTINATION share/cmake/${LXQT_GLOBALKEYS_UI_CMAKE_NAME})

# building tarball with CPack -------------------------------------------------
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set (CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set (CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include (CPack)
