cmake_minimum_required(VERSION 2.6)
# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(lxqt-globalkeys)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Set default library installation path
set(LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE PATH "Installation directory for libraries")

find_package(LXQT REQUIRED)
include(${LXQT_USE_FILE})

#************************************************
# Build config.cmake files
#************************************************
#set(LXQT_SHARE_DIR ${CMAKE_INSTALL_PREFIX}/share/lxqt)
#set(LXQT_TRANSLATIONS_DIR ${LXQT_SHARE_DIR}/translations)
include(cmake/FindInstallConfigPath.cmake)

set(LXQT_GLOBALKEYS_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/lxqt-globalkeys)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/lxqt_globalkeys-config.cmake
    @ONLY
)

set(LXQT_GLOBALKEYS_UI_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/lxqt-globalkeys-ui)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt_globalkeys_ui-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/lxqt_globalkeys_ui-config.cmake
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

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lxqt_globalkeys-config.cmake DESTINATION share/cmake/lxqt_globalkeys)
install(FILES cmake/lxqt_globalkeys_use.cmake DESTINATION share/cmake/lxqt_globalkeys)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lxqt_globalkeys_ui-config.cmake DESTINATION share/cmake/lxqt_globalkeys_ui)
install(FILES cmake/lxqt_globalkeys_ui_use.cmake DESTINATION share/cmake/lxqt_globalkeys_ui)
