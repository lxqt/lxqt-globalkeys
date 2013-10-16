cmake_minimum_required(VERSION 2.6)
# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

find_package(LXQT REQUIRED)
include(${LXQT_USE_FILE})

add_subdirectory(daemon)
add_subdirectory(config)
add_subdirectory(client)
add_subdirectory(ui)
