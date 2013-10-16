cmake_minimum_required(VERSION 2.6)
# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

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
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt-globalkeys-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/lxqt-globalkeys-config.cmake
    @ONLY
)

set(LXQT_GLOBALKEYS_UI_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/lxqt-globalkeys-ui)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lxqt-globalkeys-ui-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/lxqt-globalkeys-ui-config.cmake
    @ONLY
)
#************************************************
# End of build config.cmake
#************************************************

add_subdirectory(daemon)
add_subdirectory(config)
add_subdirectory(client)
add_subdirectory(ui)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lxqt-globalkeys-config.cmake DESTINATION share/cmake/lxqt)
install(FILES cmake/lxqt-globalkeys_use.cmake DESTINATION share/cmake/lxqt)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lxqt-globalkeys-ui-config.cmake DESTINATION share/cmake/lxqt)
install(FILES cmake/lxqt-globalkeys-ui_use.cmake DESTINATION share/cmake/lxqt)
