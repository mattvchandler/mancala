cmake_minimum_required (VERSION 2.6)
project(mancala)

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LINK_DIRS})

add_executable(mancala
    appstart.cpp
    board.cpp
    draw.cpp
    gui.cpp)

target_link_libraries(mancala ${GTKMM_LIBRARIES})
